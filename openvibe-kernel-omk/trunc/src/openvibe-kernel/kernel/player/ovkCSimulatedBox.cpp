// #if defined __MY_COMPILE_ALL

#include "ovkCSimulatedBox.h"
#include "ovkCPlayer.h"
#include "ovkCBoxAlgorithmContext.h"
#include "ovkCMessageClock.h"
#include "ovkCMessageEvent.h"
#include "ovkCMessageSignal.h"

#include "../visualisation/ovkCVisualisationManager.h"
#include "ovkCOgreVisualisation.h"
#include "ovkCOgreObject.h"
#include "ovkCOgreWindow.h"
#include "ovkCOgreScene.h"
#include "../ovkGtkOVCustom.h"

#if defined OVK_OS_Windows
#  include <gdk/gdkwin32.h>
#elif defined OVK_OS_Linux
#  include <gdk/gdkx.h>
#elif defined OVK_OS_MacOS
#  define Cursor XCursor
#  include <gdk/gdkx.h>
#  undef Cursor
#else
#endif

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

typedef Ogre::Real ogre_float;
typedef Ogre::uint32 ogre_uint32;
typedef Ogre::uint64 ogre_uint64;
typedef Ogre::uint16 ogre_uint16;
typedef Ogre::uint8  ogre_uint8;
#define uint8  OpenViBE::uint8
#define uint16 OpenViBE::uint16
#define uint32 OpenViBE::uint32
#define uint64 OpenViBE::uint64
#define boolean OpenViBE::boolean

//#define _BoxAlgorithm_ScopeTester_
//#define _SimulatedBox_ScopeTester_
#define _MaxCrash_ 5

#define __OV_FUNC__ CString("unknown_function_name")
#define __OV_LINE__ uint32(__LINE__)
#define __OV_FILE__ CString(__FILE__)

// ________________________________________________________________________________________________________________
//

namespace
{
	template <typename T>
	T& _my_get_(deque<T>& rList, uint32 ui32Index)
	{
		typename deque<T>::iterator it=rList.begin()+ui32Index;
		return *it;
	}

	template <typename T>
	const T& _my_get_(const deque<T>& rList, uint32 ui32Index)
	{
		typename deque<T>::const_iterator it=rList.begin()+ui32Index;
		return *it;
	}
}

// ________________________________________________________________________________________________________________
//

#define _Bad_Time_ 0xffffffffffffffffll
static const CNameValuePairList s_oDummyNameValuePairList;

CSimulatedBox::CSimulatedBox(const IKernelContext& rKernelContext, CScheduler& rScheduler)
	:TKernelObject<IBoxIO>(rKernelContext)
	,m_ui32CrashCount(0)
	,m_bReadyToProcess(false)
	,m_bSuspended(false)
	,m_bCrashed(false)
	,m_pBoxAlgorithm(NULL)
	,m_pScenario(NULL)
	,m_pBox(NULL)
	,m_rScheduler(rScheduler)
	,m_ui64LastClockActivationDate(_Bad_Time_)
	,m_ui64ClockFrequency(0)
	,m_ui64ClockActivationStep(0)
	,m_pOgreVis(NULL)
	,m_oSceneIdentifier(OV_UndefinedIdentifier)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	m_pOgreVis = ((CVisualisationManager*)(&rKernelContext.getVisualisationManager()))->getOgreVisualisation();
}

CSimulatedBox::~CSimulatedBox(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	//delete OgreWidgets
	std::map<GtkWidget*, CIdentifier>::iterator it;
	for(it = m_mOgreWindows.begin(); it != m_mOgreWindows.end(); it = m_mOgreWindows.begin())
	{
		//this will destroy widget then call handleDestroyEvent to destroy COgreWindow
		//WARNING : this invalidates iterator!
		gtk_widget_destroy(it->first);
	}

	//clear simulated objects map
	m_mSimulatedObjects.clear();

	//delete OgreScene
	if(m_pOgreVis)
	{
		m_pOgreVis->deleteScene(m_oSceneIdentifier);
	}
}

boolean CSimulatedBox::handleDestroyEvent(GtkWidget* pOVCustomWidget)
{
	m_pOgreVis->deleteWindow(m_mOgreWindows[pOVCustomWidget]);
	m_mOgreWindows.erase(pOVCustomWidget);
	return true;
}

boolean CSimulatedBox::handleRealizeEvent(GtkWidget* pOVCustomWidget)
{
	//create render window embedded in this widget
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}

	// added for Ogre 1.7
	gtk_widget_realize(pOVCustomWidget);
	gtk_widget_set_double_buffered(pOVCustomWidget, FALSE);

	std::string l_sExternalHandle;
#if defined OVK_OS_Windows
	l_sExternalHandle=Ogre::StringConverter::toString((unsigned long)GDK_WINDOW_HWND(pOVCustomWidget->window));
#elif defined OVK_OS_Linux
	::XSync(GDK_WINDOW_XDISPLAY(pOVCustomWidget->window), False);

	::GdkDisplay* l_pGdkDisplay=gdk_drawable_get_display(GDK_DRAWABLE(pOVCustomWidget->window));
	::GdkScreen* l_pGdkScreen=gdk_drawable_get_screen(GDK_DRAWABLE(pOVCustomWidget->window));
	::GdkVisual* l_pGdkVisual=gdk_drawable_get_visual(GDK_DRAWABLE(pOVCustomWidget->window));

	::Display* l_pXDisplay=GDK_DISPLAY_XDISPLAY(l_pGdkDisplay);
	::Screen* l_pXScreen=GDK_SCREEN_XSCREEN(l_pGdkScreen);
	::XID l_pXWindow=GDK_WINDOW_XWINDOW(pOVCustomWidget->window);
	::Visual* l_pXVisual=GDK_VISUAL_XVISUAL(l_pGdkVisual);
	int l_iScreenIndex=::XScreenNumberOfScreen(l_pXScreen);

	::XVisualInfo l_oXVisualInfo;
	::memset(&l_oXVisualInfo, 0, sizeof(::XVisualInfo));
	l_oXVisualInfo.visual=l_pXVisual;
	l_oXVisualInfo.visualid=::XVisualIDFromVisual(l_pXVisual);
	l_oXVisualInfo.screen=l_iScreenIndex;
	l_oXVisualInfo.depth=24;

	l_sExternalHandle=
		Ogre::StringConverter::toString(reinterpret_cast<unsigned long>(l_pXDisplay))+":"+
		Ogre::StringConverter::toString(static_cast<unsigned int>(l_iScreenIndex))+":"+
		Ogre::StringConverter::toString(static_cast<unsigned long>(l_pXWindow))+":"+
		Ogre::StringConverter::toString(reinterpret_cast<unsigned long>(&l_oXVisualInfo));

	// added for Ogre 1.7
	l_sExternalHandle=Ogre::StringConverter::toString(static_cast<unsigned long>(l_pXWindow));
#else
		#error failed compilation
#endif

	try
	{
		l_pOgreWindow->createRenderWindow(l_sExternalHandle, pOVCustomWidget->allocation.width, pOVCustomWidget->allocation.height);
		getLogManager() << LogLevel_Trace << "Created 3D widget\n";
	}
	catch(Ogre::Exception& e)
	{
		getLogManager() << LogLevel_Error << "Could not create render window : " << e.what() << "\n";
		return false;
	}

	//handle realization
	l_pOgreWindow->handleRealizeEvent();
	return true;
}

boolean CSimulatedBox::handleUnrealizeEvent(GtkWidget* pOVCustomWidget)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleUnrealizeEvent();
	return true;
}

boolean CSimulatedBox::handleSizeAllocateEvent(GtkWidget* pOVCustomWidget, unsigned int uiWidth, unsigned int uiHeight)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleSizeAllocateEvent(uiWidth, uiHeight);
	return true;
}

boolean CSimulatedBox::handleExposeEvent(GtkWidget* pOVCustomWidget)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleExposeEvent();
	return true;
}

boolean CSimulatedBox::handleMotionEvent(GtkWidget* pOVCustomWidget, int i32X, int i32Y)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleMotionEvent(i32X, i32Y);
	return true;
}

boolean CSimulatedBox::handleButtonPressEvent(GtkWidget* pOVCustomWidget, unsigned int uiButton, int i32X, int i32Y)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleButtonPressEvent(uiButton, i32X, i32Y);
	return true;
}

boolean CSimulatedBox::handleButtonReleaseEvent(GtkWidget* pOVCustomWidget, unsigned int uiButton, int i32X, int i32Y)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleButtonReleaseEvent(uiButton, i32X, i32Y);
	return true;
}

CIdentifier CSimulatedBox::create3DWidget(::GtkWidget*& p3DWidget)
{
	//don't attempt to create 3D widget if Ogre wasn't initialized properly.
	if(!m_pOgreVis || m_pOgreVis->ogreInitialized() == false || m_pOgreVis->resourcesInitialized() == false)
	{
		this->getLogManager() << LogLevel_Error << "Plugin " << m_pBox->getName() << " was disabled because the required 3D context couldn't be created!\n";
		m_bSuspended=true;
		return OV_UndefinedIdentifier;
	}

	//create Ogre widget
	::GtkWidget* l_pOVCustomWidget = gtk_ov_custom_new(this);
	p3DWidget = GTK_WIDGET(l_pOVCustomWidget);

	//create a window and generate an identifier for this widget
	CIdentifier l_oWindowIdentifier = createOgreWindow();

	//associate identifier to widget in a map
	m_mOgreWindows[l_pOVCustomWidget] = l_oWindowIdentifier;

	return l_oWindowIdentifier;
}

boolean CSimulatedBox::is3DWidgetRealized(
	const CIdentifier& rWidgetIdentifier) const
{
	return (m_pOgreVis!=NULL) && (m_pOgreVis->getOgreScene(m_oSceneIdentifier)!=NULL) && (m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getSceneManager()!=NULL);
}

boolean CSimulatedBox::update3DWidget(const CIdentifier& rWindowIdentifier)
{
	map<GtkWidget*, CIdentifier>::iterator it;

	for(it = m_mOgreWindows.begin(); it != m_mOgreWindows.end(); it++)
	{
		if(it->second == rWindowIdentifier)
		{
			m_pOgreVis->getOgreWindow(it->second)->update();
			return true;
		}
	}

	return false;
}

boolean CSimulatedBox::setBackgroundColor(const CIdentifier& rWindowIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->setBackgroundColor(f32ColorRed, f32ColorGreen, f32ColorBlue);
	}
	return true;
}

boolean CSimulatedBox::setCameraToEncompassObjects(const CIdentifier& rWindowIdentifier)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->setCameraToEncompassObjects();
	}
	return true;
}

boolean CSimulatedBox::setCameraSphericalCoordinates(const CIdentifier& rWindowIdentifier, float32 f32Theta, float32 f32Phi, float32 f32Radius)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->setCameraSphericalCoordinates(f32Theta, f32Phi, f32Radius);
		return true;
	}
	else
	{
		return false;
	}
}

boolean CSimulatedBox::getCameraSphericalCoordinates(const CIdentifier& rWindowIdentifier, float32& rTheta, float32& rPhi, float32& rRadius)
{
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->getCameraSphericalCoordinates(rTheta, rPhi, rRadius);
		return true;
	}
	else
	{
		return false;
	}
}

CIdentifier CSimulatedBox::createObject(const CString& rObjectFileName, const CNameValuePairList* pObjectParams)
{
	if(rObjectFileName == CString(""))
	{
		return OV_UndefinedIdentifier;
	}

	//generate a name from an identifier for this object
	CIdentifier l_oIdentifier = getUnusedIdentifier();

	string l_oSceneFileName(rObjectFileName);
	l_oSceneFileName += ".mesh";

	if(pObjectParams == NULL)
	{
		pObjectParams = &s_oDummyNameValuePairList;
	}

	if(m_pOgreVis->getOgreScene(m_oSceneIdentifier)->createObject(l_oIdentifier, l_oSceneFileName.c_str(), *pObjectParams) == NULL)
	{
		return OV_UndefinedIdentifier;
	}

	m_mSimulatedObjects[l_oIdentifier] = l_oIdentifier.toString();

	return l_oIdentifier;
}

CIdentifier CSimulatedBox::createObject(const EStandard3DObject eStandard3DObject, const CNameValuePairList* pObjectParams)
{
	//TODO : read mapping of standard 3D objects to .scene file names from a config file
	if(eStandard3DObject == Standard3DObject_Sphere)
	{
		return createObject("sphere_ov", pObjectParams);
	}
	else if(eStandard3DObject == Standard3DObject_Cone)
	{
		return createObject("cone_ov", pObjectParams);
	}
	//TODO : create a cube_ov file!
	/*else if(eStandard3DObject == Standard3DObject_Cube)
	{
		return OV_UndefinedIdentifier;
		//return createObject("cube_ov", pObjectParams);
	}*/
	else if(eStandard3DObject == Standard3DObject_Quad)
	{
		//generate a name from an identifier for this object
		CIdentifier l_oIdentifier = getUnusedIdentifier();

		if(pObjectParams == NULL)
		{
			pObjectParams = &s_oDummyNameValuePairList;
		}

		if(m_pOgreVis->getOgreScene(m_oSceneIdentifier)->createPlane(l_oIdentifier, *pObjectParams) == NULL)
		{
			return OV_UndefinedIdentifier;
		}

		m_mSimulatedObjects[l_oIdentifier] = l_oIdentifier.toString();

		return l_oIdentifier;
	}
	else
	{
		return OV_UndefinedIdentifier;
	}
}

boolean CSimulatedBox::removeObject(const CIdentifier& rObjectIdentifier)
{
	boolean res = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->removeObject(rObjectIdentifier);
	m_mSimulatedObjects.erase(rObjectIdentifier);
	return res;
}

boolean CSimulatedBox::setObjectVisible(const CIdentifier& rIdentifier, boolean bVisible)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setVisible(bVisible);
	}
	else
	{
		return false;
	}
}

boolean CSimulatedBox::setObjectScale(const CIdentifier& rIdentifier, float32 f32ScaleX, float32 f32ScaleY, float32 f32ScaleZ)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setScale(f32ScaleX, f32ScaleY, f32ScaleZ);
	}
	else
	{
		return false;
	}
}

boolean CSimulatedBox::setObjectScale(const CIdentifier& rIdentifier, float32 f32Scale)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setScale(f32Scale, f32Scale, f32Scale);
	}
	else
	{
		return false;
	}
}

boolean CSimulatedBox::setObjectPosition(const CIdentifier& rIdentifier, float32 f32PositionX, float32 f32PositionY, float32 f32PositionZ)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setPosition(f32PositionX, f32PositionY, f32PositionZ);
	}
	else
	{
		return false;
	}
}

boolean CSimulatedBox::setObjectOrientation(const CIdentifier& rIdentifier, float32 f32OrientationX, float32 f32OrientationY,
	float32 f32OrientationZ, float32 f32OrientationW)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setRotation(f32OrientationX, f32OrientationY, f32OrientationZ, f32OrientationW);
	}
	else
	{
		return false;
	}
}

boolean CSimulatedBox::setObjectColor(const CIdentifier& rIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setDiffuseColor(f32ColorRed, f32ColorGreen, f32ColorBlue);
	}
	else
	{
		return false;
	}
}

boolean CSimulatedBox::setObjectTransparency(const CIdentifier& rIdentifier, float32 f32Transparency)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setTransparency(f32Transparency);
	}
	else
	{
		return false;
	}
	return true;
}

boolean CSimulatedBox::setObjectVertexColorArray(const CIdentifier& rIdentifier, const uint32 ui32VertexColorCount, const float32* pVertexColorArray)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}

	if(sizeof(ogre_float) == sizeof(float32))
	{
		return l_pOgreObject->setVertexColorArray((ogre_uint32)ui32VertexColorCount, (ogre_float*)pVertexColorArray);
	}
	else
	{
		ogre_float* l_pOgreVertexColorArray(new ogre_float[4*ui32VertexColorCount]);
		for(uint32 i=0; i<ui32VertexColorCount; i++)
		{
			l_pOgreVertexColorArray[4*i] = pVertexColorArray[4*i];
			l_pOgreVertexColorArray[4*i+1] = pVertexColorArray[4*i+1];
			l_pOgreVertexColorArray[4*i+2] = pVertexColorArray[4*i+2];
			l_pOgreVertexColorArray[4*i+3] = pVertexColorArray[4*i+3];
		}
		boolean l_bRes = l_pOgreObject->setVertexColorArray((ogre_uint32)ui32VertexColorCount, l_pOgreVertexColorArray);
		delete[] l_pOgreVertexColorArray;
		return l_bRes;
	}
}

boolean CSimulatedBox::getObjectAxisAlignedBoundingBox(const CIdentifier& rIdentifier, float32* pMinimum, float32* pMaximum)
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	ogre_float l_oMin[3];
	ogre_float l_oMax[3];
	l_pOgreObject->getWorldBoundingBox(l_oMin[0], l_oMin[1], l_oMin[2], l_oMax[0], l_oMax[1], l_oMax[2]);
	pMinimum[0] = l_oMin[0];
	pMinimum[1] = l_oMin[1];
	pMinimum[2] = l_oMin[2];
	pMaximum[0] = l_oMax[0];
	pMaximum[1] = l_oMax[1];
	pMaximum[2] = l_oMax[2];
	return true;
}

boolean CSimulatedBox::getObjectVertexCount(const CIdentifier& rIdentifier, uint32& ui32VertexCount) const
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	return l_pOgreObject->getVertexCount((ogre_uint32&)ui32VertexCount);
}

boolean CSimulatedBox::getObjectVertexPositionArray( const CIdentifier& rIdentifier, const uint32 ui32VertexCount, float32* pVertexPositionArray) const
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	if(sizeof(ogre_float) == sizeof(float32))
	{
		return l_pOgreObject->getVertexPositionArray((ogre_uint32)ui32VertexCount, (ogre_float*)pVertexPositionArray);
	}
	else
	{
		ogre_float* l_pOgreVertexPositionArray(new ogre_float[4*ui32VertexCount]);
		boolean l_bRes = l_pOgreObject->getVertexPositionArray((ogre_uint32)ui32VertexCount, pVertexPositionArray);
		if(l_bRes == true)
		{
			for(uint32 i=0; i<ui32VertexCount; i++)
			{
				pVertexPositionArray[4*i] = l_pOgreVertexPositionArray[4*i];
				pVertexPositionArray[4*i+1] = l_pOgreVertexPositionArray[4*i+1];
				pVertexPositionArray[4*i+2] = l_pOgreVertexPositionArray[4*i+2];
				pVertexPositionArray[4*i+3] = l_pOgreVertexPositionArray[4*i+3];
			}
		}
		delete[] l_pOgreVertexPositionArray;
		return l_bRes;
	}
}

boolean CSimulatedBox::getObjectTriangleCount(const CIdentifier& rIdentifier, uint32& ui32TriangleCount) const
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	return l_pOgreObject->getTriangleCount((ogre_uint32&)ui32TriangleCount);
}

boolean CSimulatedBox::getObjectTriangleIndexArray(const CIdentifier& rIdentifier, uint32 ui32TriangleCount, uint32* pTriangleIndexArray) const
{
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	if(sizeof(ogre_uint32) == sizeof(uint32))
	{
		return l_pOgreObject->getTriangleIndexArray((ogre_uint32)ui32TriangleCount, (ogre_uint32*)pTriangleIndexArray);
	}
	else
	{
		ogre_uint32* l_pOgreTriangleIndexArray(new ogre_uint32[3*ui32TriangleCount]);
		boolean l_bRes = l_pOgreObject->getTriangleIndexArray((ogre_uint32)ui32TriangleCount, pTriangleIndexArray);
		if(l_bRes == true)
		{
			for(uint32 i=0; i<ui32TriangleCount; i++)
			{
				pTriangleIndexArray[4*i] = l_pOgreTriangleIndexArray[4*i];
				pTriangleIndexArray[4*i+1] = l_pOgreTriangleIndexArray[4*i+1];
				pTriangleIndexArray[4*i+2] = l_pOgreTriangleIndexArray[4*i+2];
			}
		}
		delete[] l_pOgreTriangleIndexArray;
		return l_bRes;
	}
}

CIdentifier CSimulatedBox::createOgreWindow()
{
	//create unique window identifier
	CIdentifier l_oWindowIdentifier = getUnusedIdentifier();

	//ensure a scene was created
	if(m_oSceneIdentifier == OV_UndefinedIdentifier)
	{
		m_oSceneIdentifier = getUnusedIdentifier();
		m_pOgreVis->createScene(m_oSceneIdentifier);
	}

	//create window
	m_pOgreVis->createWindow(l_oWindowIdentifier, l_oWindowIdentifier.toString().toASCIIString(), m_oSceneIdentifier);

	return l_oWindowIdentifier;
}

CIdentifier CSimulatedBox::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	std::map<CIdentifier, CString>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_mSimulatedObjects.find(l_oResult);
	}
	while(i!=m_mSimulatedObjects.end() || l_oResult==OV_UndefinedIdentifier);
	//TODO : browse window and scene maps as well to guarantee ID unicity!
	return l_oResult;
}

boolean CSimulatedBox::setScenarioIdentifier(const CIdentifier& rScenarioIdentifier)
{
	// FIXME test if rScenario is a scenario identifier
	m_pScenario=&getScenarioManager().getScenario(rScenarioIdentifier);
	return m_pScenario!=NULL;
}

boolean CSimulatedBox::getBoxIdentifier(CIdentifier& rBoxIdentifier)
{
	if(m_pBox == NULL)
	{
		return false;
	}
	rBoxIdentifier = m_pBox->getIdentifier();
	return true;
}

boolean CSimulatedBox::setBoxIdentifier(const CIdentifier& rBoxIdentifier)
{
	if(!m_pScenario)
	{
		return false;
	}

	m_pBox=m_pScenario->getBoxDetails(rBoxIdentifier);
	return m_pBox!=NULL;
}

boolean CSimulatedBox::initialize(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	// FIXME test for already initialized boxes etc
	if(!m_pBox) return false;
	if(!m_pScenario) return false;

	m_vInput.resize(m_pBox->getInputCount());
	m_vOutput.resize(m_pBox->getOutputCount());
	m_vCurrentOutput.resize(m_pBox->getOutputCount());

	m_oBenchmarkChronoProcessClock.reset(1024);
	m_oBenchmarkChronoProcessInput.reset(1024);
	m_oBenchmarkChronoProcess.reset(1024);

	m_ui64LastClockActivationDate=_Bad_Time_;
	m_ui64ClockFrequency=0;
	m_ui64ClockActivationStep=0;

	m_pBoxAlgorithm=getPluginManager().createBoxAlgorithm(m_pBox->getAlgorithmClassIdentifier(), NULL);
	if(!m_pBoxAlgorithm)
	{
		getLogManager() << LogLevel_Error << "Could not create box algorithm with class id " << m_pBox->getAlgorithmClassIdentifier() << "... This box will be deactivated but this might alter the scenario behavior!\n";
		m_bSuspended=true;
		return false;
	}

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::initialize)"));
#endif
			try
			{
				if(!m_pBoxAlgorithm->initialize(l_oBoxAlgorithmContext))
				{
					getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because initialization phase returned bad status\n";
					m_bSuspended=true;
				}
			}
			catch (...)
			{
				this->handleCrash("initialization callback");
			}
		}
	}

	return true ;
}

boolean CSimulatedBox::uninitialize(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bCrashed) return false;
	if(!m_pBoxAlgorithm) return false;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::uninitialize)"));
#endif
			{
				try
				{
					if(!m_pBoxAlgorithm->uninitialize(l_oBoxAlgorithmContext))
					{
						getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because uninitialization phase returned bad status\n";
						m_bSuspended=true;
					}
				}
				catch (...)
				{
					this->handleCrash("uninitialization callback");
				}
			}
		}
	}

	getPluginManager().releasePluginObject(m_pBoxAlgorithm);
	m_pBoxAlgorithm=NULL;

	return true ;
}

boolean CSimulatedBox::processClock(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::getClockFrequency)"));
#endif
			try
			{
				uint64 l_ui64NewClockFrequency=m_pBoxAlgorithm->getClockFrequency(l_oBoxAlgorithmContext);
				if(l_ui64NewClockFrequency==0)
				{
					m_ui64ClockActivationStep=_Bad_Time_;
					m_ui64LastClockActivationDate=_Bad_Time_;
				}
				else
				{
					if(l_ui64NewClockFrequency > m_rScheduler.getFrequency()<<32)
					{
						this->getLogManager() << LogLevel_ImportantWarning << "Box " << m_pBox->getName() << " requested higher clock frequency (" << l_ui64NewClockFrequency << ") than what the scheduler can handle (" << (m_rScheduler.getFrequency()<<32) << ")\n";
					}

					// note: 1LL should be left shifted 64 bits but this
					//       would result in an integer over shift (the one
					//       would exit). Thus the left shift of 63 bits
					//       and the left shift of 1 bit after the division
					m_ui64ClockActivationStep=((1LL<<63)/l_ui64NewClockFrequency)<<1;
				}
				m_ui64ClockFrequency=l_ui64NewClockFrequency;
			}
			catch (...)
			{
				this->handleCrash("clock frequency request callback");
			}
		}
	}

	if((m_ui64ClockFrequency!=0) && (m_ui64LastClockActivationDate==_Bad_Time_ || m_rScheduler.getCurrentTime()-m_ui64LastClockActivationDate>=m_ui64ClockActivationStep))
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processClock)"));
#endif
			try
			{
				m_oBenchmarkChronoProcessClock.stepIn();

				if(m_ui64LastClockActivationDate==_Bad_Time_)
				{
					m_ui64LastClockActivationDate=m_rScheduler.getCurrentTime();
				}
				else
				{
					m_ui64LastClockActivationDate=m_ui64LastClockActivationDate+m_ui64ClockActivationStep;
				}

				CMessageClock l_oClockMessage(this->getKernelContext());
				l_oClockMessage.setTime(m_ui64LastClockActivationDate);
				m_pBoxAlgorithm->processClock(l_oBoxAlgorithmContext, l_oClockMessage);
				m_oBenchmarkChronoProcessClock.stepOut();
			}
			catch (...)
			{
				this->handleCrash("clock processing callback");
			}
			m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
		}
	}

	return true;
}

boolean CSimulatedBox::processInput(const uint32 ui32InputIndex, const CChunk& rChunk)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	m_vInput[ui32InputIndex].push_back(rChunk);

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processInput)"));
#endif
			try
			{
				m_oBenchmarkChronoProcessInput.stepIn();
				m_pBoxAlgorithm->processInput(l_oBoxAlgorithmContext, ui32InputIndex);
				m_oBenchmarkChronoProcessInput.stepOut();
			}
			catch (...)
			{
				this->handleCrash("input processing callback");
			}
		}
		m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
	}

	return true;
}

boolean CSimulatedBox::process(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	if(!m_bReadyToProcess) return true;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::process)"));
#endif
			try
			{
				m_oBenchmarkChronoProcess.stepIn();
				if(!m_pBoxAlgorithm->process(l_oBoxAlgorithmContext))
				{
					getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because process phase returned bad status\n";
					m_bSuspended=true;
				}
				m_oBenchmarkChronoProcess.stepOut();
			}
			catch (...)
			{
				this->handleCrash("processing callback");
			}
		}
	}

	// perform output sending
	CIdentifier l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(OV_UndefinedIdentifier, m_pBox->getIdentifier());
	while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
	{
		const ILink* l_pLink=m_pScenario->getLinkDetails(l_oLinkIdentifier);
		if(l_pLink)
		{
			CIdentifier l_oTargetBoxIdentifier=l_pLink->getTargetBoxIdentifier();
			uint32 l_ui32TargetBoxInputIndex=l_pLink->getTargetBoxInputIndex();

			uint32 l_ui32SourceOutputIndex=l_pLink->getSourceBoxOutputIndex();
			deque < CChunk >::iterator i=m_vOutput[l_ui32SourceOutputIndex].begin();
			while(i!=m_vOutput[l_ui32SourceOutputIndex].end())
			{
				m_rScheduler.sendInput(*i, l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex);
				++i;
			}
		}
		l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(l_oLinkIdentifier, m_pBox->getIdentifier());
	}

	// iterators for input and output chunks
	vector < deque< CChunk > >::iterator i;
	deque < CChunk >::iterator j;
	vector < CChunk >::iterator k;

	// perform input cleaning
	i=m_vInput.begin();
	while(i!=m_vInput.end())
	{
		j=i->begin();
		while(j!=i->end())
		{
			if(j->isDeprecated())
			{
				j=i->erase(j);
			}
			else
			{
				++j;
			}
		}
		++i;
	}

	// flushes sent output chunks
	i=m_vOutput.begin();
	while(i!=m_vOutput.end())
	{
		i->resize(0);
		++i;
	}

	// discards waiting output chunks
	k=m_vCurrentOutput.begin();
	while(k!=m_vCurrentOutput.end())
	{
		// checks buffer size
		if(k->getBuffer().getSize())
		{
			// the buffer has been (partially ?) filled but not sent
			CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
			l_oBoxAlgorithmContext.getPlayerContext()->getLogManager() << LogLevel_Warning << "Output buffer filled but not marked as ready to send\n"; // $$$ should use log
			k->getBuffer().setSize(0, true);
		}

		++k;
	}

	m_bReadyToProcess=false;

	// FIXME : might not be the most relevant place where to refresh 3D windows!
	//moved to plugins code
	/*
	map<GtkWidget*, CIdentifier>::iterator it;

	for(it = m_mOgreWindows.begin(); it != m_mOgreWindows.end(); it++)
	{
		m_pOgreVis->getOgreWindow(it->second)->update();
	}*/

#if 1
/*-----------------------------------------------*/
/* TODO send this messages with better frequency */
	if(m_oBenchmarkChronoProcessClock.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process clock" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessClock.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcessInput.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process input" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessInput.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcess.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process      " << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcess.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
/* TODO Thank you for reading :)                 */
/*-----------------------------------------------*/
#endif

	return true;
}

boolean CSimulatedBox::isReadyToProcess(void) const
{
	return m_bReadyToProcess;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// - --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

uint64 CSimulatedBox::getCurrentTime(void) const
{
	return m_rScheduler.getCurrentTime();
}

CString CSimulatedBox::getName(void) const
{
	return m_pBox->getName();
}

const IScenario& CSimulatedBox::getScenario(void) const
{
	return *m_pScenario;
}

// ________________________________________________________________________________________________________________
//

uint32 CSimulatedBox::getInputChunkCount(
	const uint32 ui32InputIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	return m_vInput[ui32InputIndex].size();
}

boolean CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex,
	uint64& rStartTime,
	uint64& rEndTime,
	uint64& rChunkSize,
	const uint8*& rpChunkBuffer) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	rStartTime=l_rChunk.getStartTime();
	rEndTime=l_rChunk.getEndTime();
	rChunkSize=l_rChunk.getBuffer().getSize();
	rpChunkBuffer=l_rChunk.getBuffer().getDirectPointer();
	return true;
}

const IMemoryBuffer* CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return NULL;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return NULL;
	}
	return &_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).getBuffer();
}

uint64 CSimulatedBox::getInputChunkStartTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getStartTime();
}

uint64 CSimulatedBox::getInputChunkEndTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getEndTime();
}

boolean CSimulatedBox::markInputAsDeprecated(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex)
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}
	_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).markAsDeprecated(true);
	return true;
}

// ________________________________________________________________________________________________________________
//

uint64 CSimulatedBox::getOutputChunkSize(
	const uint32 ui32OutputIndex) const
{
	if(ui32OutputIndex>=m_vCurrentOutput.size())
	{
		return 0;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getSize();
}

boolean CSimulatedBox::setOutputChunkSize(
	const uint32 ui32OutputIndex,
	const uint64 ui64Size,
	const boolean bDiscard)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(ui64Size, bDiscard);
}

uint8* CSimulatedBox::getOutputChunkBuffer(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return NULL;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getDirectPointer();
}

boolean CSimulatedBox::appendOutputChunkData(
	const uint32 ui32OutputIndex,
	const uint8* pBuffer,
	const uint64 ui64BufferSize)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().append(pBuffer, ui64BufferSize);
}

IMemoryBuffer* CSimulatedBox::getOutputChunk(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return NULL;
	}
	return &m_vCurrentOutput[ui32OutputIndex].getBuffer();
}

boolean CSimulatedBox::markOutputAsReadyToSend(
	const uint32 ui32OutputIndex,
	const uint64 ui64StartTime,
	const uint64 ui64EndTime)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}

	// sets start and end time
	m_vCurrentOutput[ui32OutputIndex].setStartTime(min(ui64StartTime, ui64EndTime));
	m_vCurrentOutput[ui32OutputIndex].setEndTime(max(ui64StartTime, ui64EndTime));

	// copies chunk
	m_vOutput[ui32OutputIndex].push_back(m_vCurrentOutput[ui32OutputIndex]);

	// resets chunk size
	m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(0, true);

	return true;
}

// ________________________________________________________________________________________________________________
//

void CSimulatedBox::handleCrash(const char* sHintName)
{
	m_ui32CrashCount++;

	this->getLogManager() << LogLevel_Error << "At time " << time64(m_rScheduler.getCurrentTime()) << ", plugin code caused a crash " << m_ui32CrashCount << " time(s)\n";
	this->getLogManager() << LogLevel_Error << "  [name:" << m_pBox->getName() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [identifier:" << m_pBox->getIdentifier() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [algorithm class identifier:" << m_pBox->getAlgorithmClassIdentifier() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [location:" << sHintName << "]\n";

	if(m_ui32CrashCount>=_MaxCrash_)
	{
		this->getLogManager() << LogLevel_Fatal << "  This plugin has been disabled !\n";
		m_bCrashed=true;
	}
}

// #endif // __MY_COMPILE_ALL
