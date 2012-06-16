// Copyright (c) 2006 Christian Lindequist Larsen
//
// This software is provided 'as-is', without any express or implied warranty. In no event will
// the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose, including commercial
// applications, and to alter it and redistribute it freely, subject to the following
// restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not claim that you
//        wrote the original software. If you use this software in a product, an acknowledgment
//        in the product documentation would be appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not be
//        misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source distribution.
//
#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "OgreRenderSystem.h"
#include "OgreStringConverter.h"

#include "ovkOMKDeclareVisualObjectExt.h"
#include "ovkOMKAddVisualObjectExt.h"
#include "OMKAddVisualObjectPrm.h"
#include "OMKVisBase.h"
#include "OMKOgreVis.h"
#include "ovkOMKSimulatedEntity.h"

#include "ovkCOgreWidget.h"

#if defined( __WIN32__ ) || defined( _WIN32 )
#  include <gdk/gdkwin32.h>
#else
#  include <gdk/gdkx.h>
#endif

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

//GtkOgre parent class
class GtkOgreClass
{
public:
	GtkWidgetClass parent_class;
};
static GtkWidgetClass *parent_class = NULL;

//GtkOgre macros
guint gtk_ogre_get_type(void);
#define GTK_OGRE(obj)         GTK_CHECK_CAST(obj, gtk_ogre_get_type(), GtkOgre)
#define GTK_OGRE_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_ogre_get_type(), GtkOgreClass)
#define GTK_IS_OGRE(obj)      GTK_CHECK_TYPE(obj, gtk_ogre_get_type())

//has a 3D widget been realized yet?
//boolean GtkOgre::ms_bFirst3DWidgetRealized = false;

GtkOgre::GtkOgre()
{
	//doesn't get called : object is created via Gtk methods
	//members are initialized in init()
}

GtkOgre::~GtkOgre()
{
	//doesn't get called : object is destroyed via Gtk methods
	//members are destroyed in destroy()
}

//really acts as a constructor
void GtkOgre::init()
{
	//GTK_WIDGET_SET_FLAGS(GTK_WIDGET(this), GTK_NO_WINDOW); //unset in realize

	m_sName=new Ogre::String();
	m_pSimulatedObjects = new std::map<CIdentifier, CString>();

	m_pOwner = NULL;
	m_pOgreVis = NULL;
	m_ui32RenderWindowId = 0;
	m_pRenderWindow = NULL;
	m_pSceneManager = NULL;

	m_pLight = NULL;

	m_pCamera = NULL;

	m_pViewport = NULL;

	m_bLeftButtonPressed = false;
	m_bMiddleButtonPressed = false;
	m_bRightButtonPressed = false;

	m_ui32X = m_ui32Y = 0;

	m_f32TranslationFactor = m_f32DefaultTranslationFactor = 0.02f; //in units per pixel
	m_f32RotationFactor = 1.f; //in degrees per pixel

	//set camera to lie at (0, 0, 1) by default
	m_oRight.x = 1; m_oRight.y = 0; m_oRight.z = 0;
	m_oUp.x = 0; m_oUp.y = 1; m_oRight.z = 0;
	m_oLook.x = 0; m_oUp.y = 0; m_oRight.z = -1;
	m_oPosition.x = 0; m_oPosition.y = 0; m_oPosition.z = 1;

	//set camera to lie at (0, 0, 1) by default
	m_oLookAt.x = 0; m_oLookAt.y = 0; m_oLookAt.z = 0;
	m_f32Theta = 90;
	m_f32Phi = 0;
	m_f32Radius = 1;
}

//really acts as a destructor
void GtkOgre::destroy()
{
	//FIXME : crashes
	//delete m_pSimulatedObjects;
	//delete m_sName;
}

boolean GtkOgre::setName(const Ogre::String& rName)
{
	//avoid space characters in names because of OMK msg parsing scheme
	std::string l_sName(rName.c_str());
	while(l_sName.find(' ') != std::string::npos)
	{
		l_sName.erase(l_sName.find(' '), 1);
	}
	*m_sName = l_sName;
	return true;
}

boolean GtkOgre::setOwner(OMK::SimulatedObject* pOwner)
{
	m_pOwner = pOwner;
	//retrieve Ogre visualisation
	m_pOgreVis = dynamic_cast<OMK::Vis::OgreVis*>(m_pOwner->getController().getPointerToSimulatedObjectNamed(OMK::Name("visuOgre")));
	return true;
}

boolean GtkOgre::handleDestroyEvent()
{
	 destroy();

std::cout << "handleDestroyEvent" << this << "\n";

	// Chain up
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy)(GTK_OBJECT(this));

std::cout << "handleDestroyEvent" << this << "\n";

	return true;
}

boolean GtkOgre::handleRealizeEvent()
{
	//ensure Ogre render window hasn't already been allocated
	if(m_pRenderWindow != NULL)
	{
		return false;
	}

	//create GdkWindow
	GdkWindowAttr l_oWindowAttributes;
	memset(&l_oWindowAttributes, 0, sizeof(l_oWindowAttributes));
	//initial position and size
	l_oWindowAttributes.x = m_oWidget.allocation.x;
	l_oWindowAttributes.y = m_oWidget.allocation.y;
	l_oWindowAttributes.width = m_oWidget.allocation.width;
	l_oWindowAttributes.height = m_oWidget.allocation.height;
	//other attributes
	l_oWindowAttributes.event_mask = GDK_ALL_EVENTS_MASK; //0xFFFFFFFF; //get_events () | Gdk::EXPOSURE_MASK | Gdk::ALL_EVENTS_MASK;
	l_oWindowAttributes.window_type = GDK_WINDOW_CHILD;
	l_oWindowAttributes.wclass = GDK_INPUT_OUTPUT;
	//parent window
	GdkWindow* l_pParent = gtk_widget_get_parent_window(&m_oWidget);

	m_oWidget.window = GDK_DRAWABLE(gdk_window_new(l_pParent, &l_oWindowAttributes, GDK_WA_X | GDK_WA_Y));

	std::string l_sExternalHandle;
#if defined OVK_OS_Windows
	l_sExternalHandle=Ogre::StringConverter::toString(static_cast<unsigned long>(GDK_WINDOW_HWND(m_oWidget.window)));
#elif defined OVK_OS_Linux
	::GdkDisplay* l_pGdkDisplay=gdk_drawable_get_display(GDK_DRAWABLE(m_oWidget.window));
	::GdkScreen* l_pGdkScreen=gdk_drawable_get_screen(GDK_DRAWABLE(m_oWidget.window));
	::GdkVisual* l_pGdkVisual=gdk_drawable_get_visual(GDK_DRAWABLE(m_oWidget.window));

	::Display* l_pXDisplay=GDK_DISPLAY_XDISPLAY(l_pGdkDisplay);
	::Screen* l_pXScreen=GDK_SCREEN_XSCREEN(l_pGdkScreen);
	::XID l_pXWindow=GDK_WINDOW_XWINDOW(m_oWidget.window);
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
#else
		#error failed compilation
#endif

	//create Ogre render window in GtkWidget using GdkWindow handle
	m_ui32RenderWindowId = m_pOgreVis->createRenderWindow(
		*m_sName + "_RenderWindow",
		l_sExternalHandle,
		m_oWidget.allocation.width,
		m_oWidget.allocation.height,
		false);

	m_pRenderWindow = m_pOgreVis->getRenderWindow(m_ui32RenderWindowId);

	//widget has a window now
	GTK_WIDGET_UNSET_FLAGS(&m_oWidget, GTK_NO_WINDOW);

	//set event mask as needed here
	//gdk_window_set_events(widget->window, GDK_ALL_EVENTS_MASK);

	//setup event handlers
	g_signal_connect(G_OBJECT(this), "button-press-event", G_CALLBACK(button_press_event_in_frame_cb), this);
	g_signal_connect(G_OBJECT(this), "button-release-event", G_CALLBACK(button_release_event_in_frame_cb), this);
	g_signal_connect(G_OBJECT(this), "motion-notify-event", G_CALLBACK(motion_notify_event_in_frame_cb), this);

	gtk_widget_set_double_buffered(&m_oWidget, FALSE);
	gdk_window_set_user_data(m_oWidget.window, G_OBJECT(this));
	gdk_window_set_back_pixmap(m_oWidget.window, NULL, FALSE);

	//set realized flag
	GTK_WIDGET_SET_FLAGS(this, GTK_REALIZED);

	//complete Ogre initialisation upon primary window realization
	//FIXME : don't use a static, won't work when player is launched more than once!
	//if(ms_bFirst3DWidgetRealized == false)
	{
		m_pOgreVis->completeOgreInitialisation();
		//ms_bFirst3DWidgetRealized = true;
	}

	//create scene manager
	createSceneManager();

	//create default scene (camera, viewport, light...)
	createDefaultScene();

	return true;
}

boolean GtkOgre::handleUnrealizeEvent()
{
	//Hide all windows
	//FIXME: Is this necessary?
	if (GTK_WIDGET_MAPPED(&m_oWidget))
	{
		gtk_widget_unmap(&m_oWidget);
	}

	GTK_WIDGET_UNSET_FLAGS(&m_oWidget, GTK_MAPPED);

	//Destroy the render window, but only if it's not the primary. Ogre will destroy the primary window on shutdown.
	if(!m_pRenderWindow->isPrimary())
	{
#if 1
		Ogre::Root::getSingleton().getRenderSystem()->destroyRenderWindow(m_pRenderWindow->getName());
#else
		//TODO : implement this method!
		m_pOgreVis->destroyRenderWindow(m_ui32RenderWindowId);
#endif
	}

	m_pRenderWindow = NULL;

	//This destroys widget->window and unsets the realized flag
	if(GTK_WIDGET_CLASS(parent_class)->unrealize != NULL)
	{
		(*GTK_WIDGET_CLASS(parent_class)->unrealize)(&m_oWidget);
	}

	return true;
}

boolean GtkOgre::handleSizeRequestEvent(GtkRequisition* pRequisition)
{
	m_oWidget.requisition = *pRequisition;
	return true;
}

boolean GtkOgre::handleSizeAllocateEvent(GtkAllocation* pAllocation)
{
	// Chain up - this will adjust the size of the actual window
  if (GTK_WIDGET_CLASS(parent_class)->size_allocate)
    (*GTK_WIDGET_CLASS(parent_class)->size_allocate)(&m_oWidget, pAllocation);

	m_oWidget.allocation = *pAllocation;

	if(m_oWidget.window != NULL)
	{
		gdk_window_move_resize(m_oWidget.window, pAllocation->x, pAllocation->y, pAllocation->width, pAllocation->height);
	}

  if(m_pRenderWindow != NULL)
  {
#ifdef _WIN32
		m_pRenderWindow->resize(m_oWidget.allocation.width, m_oWidget.allocation.height);
		m_pRenderWindow->windowMovedOrResized();
#else
		m_pRenderWindow->resize(m_oWidget.allocation.width, m_oWidget.allocation.height);
#endif
		handleExposeEvent();
  }

	return true;
}

boolean GtkOgre::handleExposeEvent()
{
	if(m_pRenderWindow == NULL)
	{
		return false;
	}

	m_pRenderWindow->update(true);

	return true;
}

boolean GtkOgre::handleMotionEvent(GdkEventButton *bevent)
{
	if(m_pCamera == NULL)
	{
		return false;
	}

	if(m_bLeftButtonPressed == true)
	{
		//rotate camera
		m_f32Phi -= m_f32RotationFactor * (bevent->x - m_ui32X);
		get360Angle(m_f32Phi);

		m_f32Theta -= m_f32RotationFactor * (bevent->y - m_ui32Y);
		get180Angle(m_f32Theta);
		//FIXME : when using angles of 0 or 180 degrees, camera up vector is set to (1, 0, 0)
		//when using forcedYawAxis on camera, resulting in break of continuity in camera orientation
#define THETA_THRESHOLD 0.1f
		if(m_f32Theta < THETA_THRESHOLD)
			m_f32Theta = THETA_THRESHOLD;
		else if(m_f32Theta > 180 - THETA_THRESHOLD)
			m_f32Theta = 180 - THETA_THRESHOLD;
	}
	else if(m_bRightButtonPressed == true)
	{
		//translate camera
		m_f32Radius += m_f32TranslationFactor * (bevent->y - m_ui32Y);
		if(m_f32Radius < 0)
		{
			m_f32Radius = 0;
		}
	}
	else if(m_bMiddleButtonPressed == true)
	{
	}

	updateCamera();
	m_pRenderWindow->update(true);
	m_ui32X = (uint32)bevent->x;
	m_ui32Y = (uint32)bevent->y;

	return true;
}

boolean GtkOgre::handleButtonPressEvent(GdkEventButton *bevent)
{
	m_ui32X = (uint32)bevent->x;
	m_ui32Y = (uint32)bevent->y;

	if(bevent->button == 1) //left button
		m_bLeftButtonPressed = true;
	else if(bevent->button == 2)
		m_bMiddleButtonPressed = true;
	else if(bevent->button == 3) //right button
		m_bRightButtonPressed = true;

	return true;
}

boolean GtkOgre::handleButtonReleaseEvent(GdkEventButton *bevent)
{
	if(bevent->button == 1) //left button
		m_bLeftButtonPressed = false;
	else if(bevent->button == 2)
		m_bMiddleButtonPressed = false;
	else if(bevent->button == 3) //right button
		m_bRightButtonPressed = false;

	return true;
}

boolean GtkOgre::setBackgroundColor(float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	if(m_pViewport == NULL)
	{
		return false;
	}

	m_pViewport->setBackgroundColour(Ogre::ColourValue(f32ColorRed, f32ColorGreen, f32ColorBlue));

	return true;
}

boolean GtkOgre::setCameraToEncompassObjects()
{
	std::map<OpenViBE::CIdentifier, OpenViBE::CString>::iterator it;

	float32 l_f32MinX = FLT_MAX;
	float32 l_f32MinY = FLT_MAX;
	float32 l_f32MinZ = FLT_MAX;
	float32 l_f32MaxX = -FLT_MAX;
	float32 l_f32MaxY = -FLT_MAX;
	float32 l_f32MaxZ = -FLT_MAX;

	//get each object's bounding box and update global bounding box
	for(it = m_pSimulatedObjects->begin(); it != m_pSimulatedObjects->end(); it++)
	{
		float32 l_f32ObjectMinX, l_f32ObjectMinY, l_f32ObjectMinZ;
		float32 l_f32ObjectMaxX, l_f32ObjectMaxY, l_f32ObjectMaxZ;

		if(getObjectWorldBoundingBox(it->first, l_f32ObjectMinX, l_f32ObjectMinY, l_f32ObjectMinZ,
			l_f32ObjectMaxX, l_f32ObjectMaxY, l_f32ObjectMaxZ) == true)
		{
			if(l_f32ObjectMinX < l_f32MinX)
				l_f32MinX = l_f32ObjectMinX;
			if(l_f32ObjectMaxX > l_f32MaxX)
				l_f32MaxX = l_f32ObjectMaxX;

			if(l_f32ObjectMinY < l_f32MinY)
				l_f32MinY = l_f32ObjectMinY;
			if(l_f32ObjectMaxY > l_f32MaxY)
				l_f32MaxY = l_f32ObjectMaxY;

			if(l_f32ObjectMinZ < l_f32MinZ)
				l_f32MinZ = l_f32ObjectMinZ;
			if(l_f32ObjectMaxZ > l_f32MaxZ)
				l_f32MaxZ = l_f32ObjectMaxZ;
		}
	}

	//get largest BB dimension
	float32 l_f32LargestDimension = l_f32MaxX - l_f32MinX;
	if(l_f32MaxY - l_f32MinY >  l_f32LargestDimension)
	{
		l_f32LargestDimension = l_f32MaxY - l_f32MinY;
	}
	if(l_f32MaxZ - l_f32MinZ >  l_f32LargestDimension)
	{
		l_f32LargestDimension = l_f32MaxZ - l_f32MinZ;
	}

	//look at BB center
	m_oLookAt.x = (l_f32MinX + l_f32MaxX) / 2;
	m_oLookAt.y = (l_f32MinY + l_f32MaxY) / 2;
	m_oLookAt.z = (l_f32MinZ + l_f32MaxZ) / 2;

	//place camera at a distance from bounding box's +Z face on the +Z axis
	m_f32Phi = 0;
	m_f32Theta = 90;
	m_f32Radius = m_pCamera->getNearClipDistance() + (l_f32MaxZ - l_f32MinZ)/2 + 2*l_f32LargestDimension;

	updateCamera();

	//adapt translation factor (or 'speed') to scene size
	computeTranslationFactor(l_f32LargestDimension);

	return true;
}

CIdentifier GtkOgre::createObject(const CString& rObjectFileName)
{
	if(rObjectFileName == CString(""))
	{
		return OV_UndefinedIdentifier;
	}

	//generate a name from an identifier for this object
	CIdentifier l_oIdentifier = getUnusedIdentifier();
	std::string l_sName = l_oIdentifier.toString().toASCIIString();

	//OSO name must not contain any space character
	l_sName.erase(l_sName.find(' '), 1);

	//use same name for VO as for OSO (there is a single VO per OSO for now)
	CString l_sOSOName(l_sName.c_str());
	CString l_sVOName(l_sName.c_str());

	//object parameters
	//-----------------
	OMK::ConfigurationParameterDescriptor* objectParam = 0;

	//these are hardcoded in VO declaration extension
	//OMK::ParametersAccessor::set( visualObjectParam, "Class", "OgreObject") ;
	//OMK::ParametersAccessor::set( visualObjectParam, "ResourceGroup", "General") ;
	OMK::ParametersAccessor::set( objectParam, "GeometryFile", rObjectFileName + ".scene") ;
	OMK::ParametersAccessor::set( objectParam, "SceneManager", m_pSceneManager->getName() ) ;
/*
	std::vector<float> position(3);
	position[0] = pPosition[0]; position[1] = pPosition[1]; position[2] = pPosition[2];
	OMK::ParametersAccessor::set( objectParam, "Translate", position);
*/
	OMK::ParametersAccessor::set( objectParam, "VisualObjectName", l_sVOName) ;

	//animators subsection
	//--------------------
	// OMK::ConfigurationParameterDescriptor* animatorParam = 0;

	//animators
	//---------
	OMK::ConfigurationParameterDescriptor* translateParam = 0;
	OMK::ParametersAccessor::set(translateParam, "Class", "TransformAnimator") ;
	OMK::ParametersAccessor::set(translateParam, "NodeName", "DCS_position") ;
	OMK::ParametersAccessor::set(translateParam, "VisualObjectName", l_sVOName) ;
	OMK::ParametersAccessor::set(objectParam, "translateObjectAnimator", translateParam) ;

	OMK::ConfigurationParameterDescriptor* rotateParam = 0;
	OMK::ParametersAccessor::set(rotateParam, "Class", "TransformAnimator") ;
	OMK::ParametersAccessor::set(rotateParam, "NodeName", "DCS_rotation") ;
	OMK::ParametersAccessor::set(rotateParam, "VisualObjectName", l_sVOName) ;
	OMK::ParametersAccessor::set(objectParam, "rotateObjectAnimator", rotateParam) ;

	OMK::ConfigurationParameterDescriptor* scaleParam = 0;
	OMK::ParametersAccessor::set(scaleParam, "Class", "TransformAnimator") ;
	OMK::ParametersAccessor::set(scaleParam, "NodeName", "DCS_scale") ;
	OMK::ParametersAccessor::set(scaleParam, "VisualObjectName", l_sVOName) ;
	OMK::ParametersAccessor::set(objectParam, "scaleObjectAnimator", scaleParam) ;

	OMK::ConfigurationParameterDescriptor* colorParam = 0;
	OMK::ParametersAccessor::set(colorParam, "Class", "OMKColorShaderAnimator") ;
	OMK::ParametersAccessor::set(colorParam, "NodeName", "DCS_position") ;
	OMK::ParametersAccessor::set(colorParam, "VisualObjectName", l_sVOName) ;
	OMK::ParametersAccessor::set(objectParam, "colorObjectAnimator", colorParam) ;

	OMK::ConfigurationParameterDescriptor* vertexDataParam = 0;
	OMK::ParametersAccessor::set(vertexDataParam, "Class", "OMKVertexBufferAnimator") ;
	OMK::ParametersAccessor::set(vertexDataParam, "NodeName", "DCS_position") ;
	OMK::ParametersAccessor::set(vertexDataParam, "VisualObjectName", l_sVOName) ;
	OMK::ParametersAccessor::set(objectParam, "vertexDataAnimator", vertexDataParam) ;

	//TODO : use log manager instead
	cout << "config file = " << *objectParam << endl;

	//TODO : retrieve name of process and object frequency from somewhere! (Player?, ...)
	m_pOwner->getController().createObject(
		OMK::ObjectDescriptor(
			OMK::Name(l_sOSOName),
			"OMKSimulatedEntity",
			"ProcessA",
			60,
			objectParam),
			m_pOwner->getFathersDescriptor().getName());

	//store it in map
	(*m_pSimulatedObjects)[l_oIdentifier] = l_sOSOName;

	return l_oIdentifier;
}

CIdentifier GtkOgre::createObject(const EStandard3DObject eStandard3DObject)
{
	//TODO : read mapping of standard 3D objects to .scene file names from a config file
	if(eStandard3DObject == Standard3DObject_Sphere)
	{
		return createObject("sphere_ov");
	}
	else if(eStandard3DObject == Standard3DObject_Cone)
	{
		return createObject("cone_ov");
	}
	else
	{
		return OV_UndefinedIdentifier;
	}
}

boolean GtkOgre::removeObject(const CIdentifier& rIdentifier)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	m_pOwner->getController().destroyObject(OMK::Name((*m_pSimulatedObjects)[rIdentifier]), true);
	m_pSimulatedObjects->erase(rIdentifier);
	return true;
}

boolean GtkOgre::setObjectScale(const CIdentifier& rIdentifier, float32 f32ScaleX, float32 f32ScaleY, float32 f32ScaleZ)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	OMK::Type::Transform l_oTransform;
	l_oTransform.setScale(Wm4::Vector3f(f32ScaleX, f32ScaleY, f32ScaleZ));
	m_pOwner->sendValuedEvent<OMK::Type::TransformType>(
		OMK::Name((*m_pSimulatedObjects)[rIdentifier]),
		OMKSimulatedEntity::SCALE_EVENT_ID,
		l_oTransform);
	return true;
}

boolean GtkOgre::setObjectScale(const CIdentifier& rIdentifier, float32 f32Scale)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	OMK::Type::Transform l_oTransform;
	l_oTransform.setUniformScale((float)f32Scale);
	m_pOwner->sendValuedEvent<OMK::Type::TransformType>(
		OMK::Name((*m_pSimulatedObjects)[rIdentifier]),
		OMKSimulatedEntity::SCALE_EVENT_ID,
		l_oTransform);
	return true;
}

boolean GtkOgre::setObjectPosition(const CIdentifier& rIdentifier, float32 f32PositionX, float32 f32PositionY, float32 f32PositionZ)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	m_pOwner->sendValuedEvent<OMK::Type::TransformType>(
		OMK::Name((*m_pSimulatedObjects)[rIdentifier]),
		OMKSimulatedEntity::TRANSLATE_EVENT_ID,
		OMK::Type::Transform(Wm4::Vector3f(f32PositionX, f32PositionY, f32PositionZ)));
	return true;
}

boolean GtkOgre::setObjectOrientation(const CIdentifier& rIdentifier, float32 f32OrientationX, float32 f32OrientationY,
	float32 f32OrientationZ, float32 f32OrientationW)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	Wm4::Quaternion<float32> l_quaternion(f32OrientationW, f32OrientationX, f32OrientationY, f32OrientationZ);
	Wm4::Matrix3f l_rotationMatrix;
	l_quaternion.ToRotationMatrix(l_rotationMatrix);
	OMK::Type::Transform l_oTransform;
	l_oTransform.setRotate(l_rotationMatrix);
	/*
	m_pOwner->sendValuedEvent<OMK::Type::TransformType>(
	OMK::Name((*m_pSimulatedObjects)[rIdentifier]),
	OMKSimulatedEntity::ROTATE_EVENT_ID,
	l_oTransform);
	*/
	return true;
}

boolean GtkOgre::setObjectColor(const CIdentifier& rIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	m_pOwner->sendValuedEvent<OMK::Type::ColorType>(
		OMK::Name((*m_pSimulatedObjects)[rIdentifier]),
		OMKSimulatedEntity::COLOR_EVENT_ID,
		OMK::Type::Color(f32ColorRed, f32ColorGreen, f32ColorBlue));
	return true;
}

boolean GtkOgre::setObjectTransparency(const CIdentifier& rIdentifier, float32 f32Transparency)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	m_pOwner->sendValuedEvent<OMK::Type::ColorType>(
		OMK::Name((*m_pSimulatedObjects)[rIdentifier]),
		OMKSimulatedEntity::TRANSPARENCY_EVENT_ID,
		OMK::Type::Color(1, 1, 1, f32Transparency));
	return true;
}

boolean GtkOgre::setObjectVertexColorArray(const CIdentifier& rIdentifier, const uint32 ui32VertexColorCount, const float32* pVertexColorArray)
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}
	OMK::Type::VertexData vd;
	vd.setVertexColorArray(ui32VertexColorCount, pVertexColorArray);
	m_pOwner->sendValuedEvent<OMK::Type::VertexDataType>(
		OMK::Name((*m_pSimulatedObjects)[rIdentifier]),
		OMKSimulatedEntity::VERTEXDATA_EVENT_ID,
		vd);

	return true;
}

boolean GtkOgre::getObjectVertexCount( const CIdentifier& rIdentifier, uint32& ui32VertexCount) const
{
	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(rIdentifier, l_pEntity) == false)
	{
		return false;
	}

	Ogre::SubMesh* l_pSubMesh;
	if(getFirstSubMesh(l_pEntity, l_pSubMesh) == false)
	{
		return false;
	}

	ui32VertexCount = l_pSubMesh->vertexData->vertexCount;

	return true;
}

boolean GtkOgre::getObjectVertexPositionArray( const OpenViBE::CIdentifier& rIdentifier,	float32* pVertexPositionArray) const
{
	if(pVertexPositionArray == NULL)
	{
		return false;
	}

	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(rIdentifier, l_pEntity) == false)
	{
		return false;
	}

	Ogre::SubMesh* l_pSubMesh;
	if(getFirstSubMesh(l_pEntity, l_pSubMesh) == false)
	{
		return false;
	}

	//get world matrix
	Ogre::Matrix4 l_oMatrix4;
	l_pEntity->getParentSceneNode()->getWorldTransforms(&l_oMatrix4);

	//get pointer to POSITION element
	const Ogre::VertexElement* l_pPositionElement = l_pSubMesh->vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

	//retrieve VB for POSITION element
	Ogre::HardwareVertexBufferSharedPtr l_pVertexBuffer = l_pSubMesh->vertexData->vertexBufferBinding->getBuffer(l_pPositionElement->getSource());

	//lock VB for reading
	unsigned char* l_pVertexBufferPointer = static_cast<unsigned char*>(l_pVertexBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

	//iterate through VB
	float* l_pReal;

	Ogre::Vector3 l_oWorldVector;

	for( size_t j=0; j<l_pSubMesh->vertexData->vertexCount; ++j, l_pVertexBufferPointer += l_pVertexBuffer->getVertexSize())
	{
		//get pointer to POSITION data
		l_pPositionElement->baseVertexPointerToElement(l_pVertexBufferPointer, &l_pReal);
#if 0
		//store it
		pVertexPositionArray[j*3] = l_pReal[0];
		pVertexPositionArray[j*3+1] = l_pReal[1];
		pVertexPositionArray[j*3+2] = l_pReal[2];
#else
		Ogre::Vector3 tmp(l_pReal[0], l_pReal[1], l_pReal[2]);
		l_oWorldVector = l_oMatrix4 * tmp;
		pVertexPositionArray[j*3] = l_oWorldVector[0];
		pVertexPositionArray[j*3+1] = l_oWorldVector[1];
		pVertexPositionArray[j*3+2] = l_oWorldVector[2];
#endif
	}

	//unlock VB
	l_pVertexBuffer->unlock();

	return true;
}

boolean GtkOgre::createSceneManager()
{
	if(m_pOgreVis->createSceneManager(m_ui32RenderWindowId, "DefaultSceneManager", *m_sName + "_SceneManager") == false)
	{
		return false;
	}

	m_pSceneManager = m_pOgreVis->getSceneManager(m_ui32RenderWindowId);

	return true;
}

boolean GtkOgre::createDefaultScene()
{
	if(m_pSceneManager == NULL)
	{
		return false;
	}

	//default light
	m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1, 1));

	//back light
	m_pLight = m_pSceneManager->createLight(*m_sName + "_Light_0");
	m_pLight->setType(Ogre::Light::LT_DIRECTIONAL);
	m_pLight->setDirection(Ogre::Real(0), Ogre::Real(0), Ogre::Real(-1));
	m_pLight->setDiffuseColour(1, 1, 1);

	//front light
	Ogre::Light* l_pLight2 = m_pSceneManager->createLight(*m_sName + "_Light_1");
	l_pLight2->setType(Ogre::Light::LT_DIRECTIONAL);
	l_pLight2->setDirection(Ogre::Real(0), Ogre::Real(0), Ogre::Real(1));
	l_pLight2->setDiffuseColour(1, 1, 1);

	//top light
	Ogre::Light* l_pLight3 = m_pSceneManager->createLight(*m_sName + "_Light_2");
	l_pLight3->setType(Ogre::Light::LT_DIRECTIONAL);
	l_pLight3->setDirection(Ogre::Real(0), Ogre::Real(-1), Ogre::Real(0));
	l_pLight3->setDiffuseColour(1, 1, 1);

	//default camera
	m_pCamera = m_pSceneManager->createCamera(*m_sName + "_Camera_0");
	m_pCamera->setAutoAspectRatio(true);
	m_pCamera->setNearClipDistance(1.0);
	m_pCamera->setFarClipDistance(1000.0);
	m_pCamera->setFixedYawAxis(true);
	updateCamera();

	//default viewport
	m_pViewport = m_pRenderWindow->addViewport(m_pCamera);
	m_pViewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));

	return true;
}

boolean GtkOgre::computeTranslationFactor(OpenViBE::float32 f32SceneSize)
{
	m_f32TranslationFactor = f32SceneSize * m_f32DefaultTranslationFactor;

	return true;
}

//FIXME : detect when mouse exits/enters Gtk windows and update
//camera only when it is inside windows. Otherwise, when mouse exits window,
//weird things happen with cursor values
#define ANGLE_MAX_VAL 1E3

void GtkOgre::get360Angle(float32& f32Angle)
{
	if(f32Angle > ANGLE_MAX_VAL || f32Angle < -ANGLE_MAX_VAL)
		f32Angle = 0;

	f32Angle -= (int)(f32Angle/360)*360;

	if(f32Angle < 0)
	{
		f32Angle += 360;
	}
}

void GtkOgre::get180Angle(float32& f32Angle)
{
	if(f32Angle > ANGLE_MAX_VAL || f32Angle < -ANGLE_MAX_VAL)
	{
		f32Angle = 0;
	}

	//f32Angle -= (int)(f32Angle/180)*180;

	if(f32Angle < 0)
	{
		f32Angle = 0;
	}
	else if(f32Angle > 180)
	{
		f32Angle = 180;
	}
}

boolean GtkOgre::updateCamera()
{
	float32 l_f32PhiRadians = m_f32Phi * 3.1415926535 / 180.0;
	float32 l_f32ThetaRadians = m_f32Theta * 3.1415926535 / 180.0;

	m_pCamera->setPosition(
		m_oLookAt.x + m_f32Radius * sinf(l_f32ThetaRadians) * sinf(l_f32PhiRadians),
		m_oLookAt.y + m_f32Radius * cosf(l_f32ThetaRadians),
		m_oLookAt.z + m_f32Radius * sinf(l_f32ThetaRadians) * cosf(l_f32PhiRadians));

	m_pCamera->lookAt(m_oLookAt);

	return true;
}

CIdentifier GtkOgre::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, CString>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_pSimulatedObjects->find(l_oResult);
	}
	while(i!=m_pSimulatedObjects->end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}

boolean GtkOgre::getFirstEntityInHierarchy(const CIdentifier& rIdentifier, Ogre::Entity*& pEntity) const
{
	if(m_pSimulatedObjects->find(rIdentifier) == m_pSimulatedObjects->end())
	{
		return false;
	}

	//retrieve visual object
	OMK::Vis::VisualObject* l_pVisualObject = m_pOgreVis->getVisualObject(OMK::Name(m_pSimulatedObjects->find(rIdentifier)->second));
	if(l_pVisualObject == NULL)
	{
		return false;
	}

	//cast it to OgreObject
	OMK::Vis::OgreObject* l_pOgreObject = dynamic_cast<OMK::Vis::OgreObject*>(l_pVisualObject);
	if(l_pOgreObject == NULL)
	{
		return false;
	}

	//retrieve Entity
	if(getFirstEntityInHierarchy(&l_pOgreObject->getSceneNode(), pEntity) == false)
	{
		return false;
	}

	return true;
}

boolean GtkOgre::getFirstEntityInHierarchy(Ogre::SceneNode* pNode, Ogre::Entity*& pEntity) const
{
	for(uint32 i=0; i<pNode->numAttachedObjects(); i++)
	{
		Ogre::MovableObject* pMovableObject = pNode->getAttachedObject(0);
		if(pMovableObject->getMovableType() == Ogre::EntityFactory::FACTORY_TYPE_NAME)
		{
			pEntity = dynamic_cast<Ogre::Entity*>(pMovableObject);
			return true;
		}
	}

	for(uint32 i=0; i<pNode->numChildren(); i++)
	{
		Ogre::SceneNode* l_pChildNode = dynamic_cast<Ogre::SceneNode*>(pNode->getChild(i));

		if(l_pChildNode != NULL && getFirstEntityInHierarchy(l_pChildNode, pEntity) == true)
		{
			return true;
		}
	}

	return false;
}

boolean GtkOgre::getFirstSubMesh(Ogre::Entity* l_pEntity, Ogre::SubMesh*& l_pSubMesh) const
{
	Ogre::Mesh* l_pMesh = l_pEntity->getMesh().get();
	if(l_pMesh == NULL)
	{
		return false;
	}

	l_pSubMesh = l_pMesh->getSubMesh(0);
	//TODO : support shared vertices (they are stored at the Mesh level)
	if(l_pSubMesh == NULL || l_pSubMesh->useSharedVertices)
	{
		//Log << ...
		return false;
	}

	return true;
}

boolean GtkOgre::getObjectWorldBoundingBox(const CIdentifier& rIdentifier, float32& rMinX,	float32& rMinY,	float32& rMinZ,
	float32& rMaxX,	float32& rMaxY,	float32& rMaxZ)
{
	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(rIdentifier, l_pEntity) == false)
	{
		return false;
	}

	const Ogre::AxisAlignedBox& l_rWorldAABB = l_pEntity->getWorldBoundingBox();

	rMinX = l_rWorldAABB.getMinimum().x;
	rMinY = l_rWorldAABB.getMinimum().y;
	rMinZ = l_rWorldAABB.getMinimum().z;

	rMaxX = l_rWorldAABB.getMaximum().x;
	rMaxY = l_rWorldAABB.getMaximum().y;
	rMaxZ = l_rWorldAABB.getMaximum().z;

	return true;
}

static void gtk_ogre_class_init(GtkOgreClass *klass)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	object_class = (GtkObjectClass*)klass;
	widget_class = (GtkWidgetClass*)klass;

	parent_class = (GtkWidgetClass*)gtk_type_class(gtk_widget_get_type());

	object_class->destroy = GtkOgre::gtk_ogre_destroy;

	widget_class->realize = GtkOgre::gtk_ogre_realize;
	widget_class->unrealize = GtkOgre::gtk_ogre_unrealize;

	widget_class->size_request = GtkOgre::gtk_ogre_size_request;
	widget_class->size_allocate = GtkOgre::gtk_ogre_size_allocate;

	widget_class->expose_event = GtkOgre::gtk_ogre_expose;
}

static void gtk_ogre_init(GtkOgre *pOgreWidget)
{
	pOgreWidget->init();
}

guint gtk_ogre_get_type(void)
{
	static guint ogre_type = 0;
	if (!ogre_type)
	{
		static const GtkTypeInfo ogre_info =
		{
			"GtkOgre",
			sizeof(GtkOgre),
			sizeof(GtkOgreClass),
			(GtkClassInitFunc)gtk_ogre_class_init,
			(GtkObjectInitFunc)gtk_ogre_init,
			NULL,
			NULL,
			(GtkClassInitFunc)NULL,
		};
		ogre_type = gtk_type_unique(gtk_widget_get_type(), &ogre_info);
	}
	return ogre_type;
}

GtkOgre* gtk_ogre_new(const Ogre::String& rName, OMK::SimulatedObject* pOwner)
{
	//create widget
	GtkOgre* l_pOgreWidget = /*(GtkOgre*)*/GTK_OGRE(gtk_type_new(gtk_ogre_get_type()));

	if(l_pOgreWidget)
	{
		//set required parameters
		l_pOgreWidget->setName(rName);
		l_pOgreWidget->setOwner(pOwner);
	}

	return l_pOgreWidget;
}

// GtkObject functions
void GtkOgre::gtk_ogre_destroy(GtkObject *pObject)
{
	g_return_if_fail(pObject != NULL);
	g_return_if_fail(GTK_IS_OGRE(pObject));

	GTK_OGRE(pObject)->handleDestroyEvent();
}

// GtkWidget functions
void GtkOgre::gtk_ogre_realize(GtkWidget *pWidget)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OGRE(pWidget));

	GTK_OGRE(pWidget)->handleRealizeEvent();
}

void GtkOgre::gtk_ogre_unrealize(GtkWidget *pWidget)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OGRE(pWidget));

	GTK_OGRE(pWidget)->handleUnrealizeEvent();
}

void GtkOgre::gtk_ogre_size_request(GtkWidget *pWidget, GtkRequisition* pRequisition)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OGRE(pWidget));

	GTK_OGRE(pWidget)->handleSizeRequestEvent(pRequisition);
}

void GtkOgre::gtk_ogre_size_allocate(GtkWidget *pWidget, GtkAllocation *pAllocation)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OGRE(pWidget));

	GTK_OGRE(pWidget)->handleSizeAllocateEvent(pAllocation);
}

// Update the render window and swap buffers
/*static*/ gint GtkOgre::gtk_ogre_expose(GtkWidget *pWidget, GdkEventExpose *pEvent)
{
	return GTK_OGRE(pWidget)->handleExposeEvent();
}

gboolean GtkOgre::button_press_event_in_frame_cb(GtkWidget *pWidget, GdkEventButton *pEvent, gpointer user_data)
{
	return GTK_OGRE(pWidget)->handleButtonPressEvent(pEvent);
}

gboolean GtkOgre::motion_notify_event_in_frame_cb(GtkWidget *pWidget, GdkEventButton *pEvent, gpointer user_data)
{
	return GTK_OGRE(pWidget)->handleMotionEvent(pEvent);
}

gboolean GtkOgre::button_release_event_in_frame_cb(GtkWidget *pWidget, GdkEventButton *pEvent, gpointer user_data)
{
	return GTK_OGRE(pWidget)->handleButtonReleaseEvent(pEvent);
}

/*
static bool s_firstCall = true;

boolean createObject(CString& rObjectName, CString& rMeshName, float32 pPosition[3], float32 pColor[4])
{
	//return true;

	AddVisualObjectPrm visualObject;
	visualObject.setObjectName(OMK::Name(rObjectName));
	visualObject.setClassId(OMK::Name("OgreObject"));
	//visualObject.setVisuName(OMK::Name("visuOgre"));

	//set visual object properties
	OMK::ConfigurationParameterDescriptor* configParam = 0;
	OMK::ParametersAccessor::set(configParam, "ResourceGroup", "General") ;
	OMK::ParametersAccessor::set(configParam, "GeometryFile", rMeshName) ;
	std::vector<float> position(3);
	position[0] = pPosition[0]; position[1] = pPosition[1]; position[2] = pPosition[2] - 10;
	OMK::ParametersAccessor::set(configParam, "Translate", position) ;

	if(rMeshName != CString("scaledSkull.scene"))
	{
		//animator subsection
		OMK::ConfigurationParameterDescriptor* animParam = 0;

		//color animator properties
		OMK::ConfigurationParameterDescriptor* colorParam = 0;
		OMK::ParametersAccessor::set(colorParam, "Class", "OMKColorShaderAnimator") ;
		OMK::ParametersAccessor::set(colorParam, "NodeName", "MainNODE") ;
		std::vector<float> color(4);
		color[0] = pColor[0]; color[1] = pColor[1]; color[2] = pColor[2];  color[3] = pColor[3];
		OMK::ParametersAccessor::set(colorParam, "Color", color) ;

		OMK::ParametersAccessor::set(animParam, "color", colorParam) ;

		OMK::ParametersAccessor::set(configParam, "Animator", animParam) ;
	}

	visualObject.setConfigParam(configParam);

	//Name visualisation;
	//Don't send a signal if it's a local object ie on only one visu
	//if ( OMK::ParametersAccessor::get( node, "LocalVisualisation", visualisation))
		//sendValuedEvent(visualisation, Vis::VisBase::ADD_VISUAL_OBJECT_EVENT_ID, visualObject);
	//else
		fireValuedSignal(Vis::VisBase::ADD_VISUAL_OBJECT_EVENT_ID, visualObject);

	return true;
}

boolean createObject()
{
	if(s_firstCall == true)
	{
		AddVisualObjectPrm visualObject;
		visualObject.setObjectName(OMK::Name("MyObject"));
		visualObject.setClassId(OMK::Name("OgreObject"));
		//visualObject.setVisuName(OMK::Name("visuOgre"));

		//set visual object properties
		OMK::ConfigurationParameterDescriptor* configParam = 0;
		OMK::ParametersAccessor::set(configParam, "ResourceGroup", "General") ;
		OMK::ParametersAccessor::set(configParam, "GeometryFile", "robot.scene") ;
		std::vector<float> position(3);
		position[0] = 0.f; position[1] = 0.f; position[2] = 0.f;
		OMK::ParametersAccessor::set(configParam, "Translate", position) ;

		//animator subsection
		OMK::ConfigurationParameterDescriptor* animParam = 0;

		//color animator properties
		OMK::ConfigurationParameterDescriptor* colorParam = 0;
		OMK::ParametersAccessor::set(colorParam, "Class", "OMKColorShaderAnimator") ;
		OMK::ParametersAccessor::set(colorParam, "NodeName", "Robot") ;
		std::vector<float> color(4);
		color[0] = 0.f; color[1] = 1.f; color[2] = 0.f;  color[3] = 1.f;
		OMK::ParametersAccessor::set(colorParam, "Color", color) ;

		OMK::ParametersAccessor::set(animParam, "color", colorParam) ;

		OMK::ParametersAccessor::set(configParam, "Animator", animParam) ;

		visualObject.setConfigParam(configParam);

		//Name visualisation;
		//Don't send a signal if it's a local object ie on only one visu
		//if ( OMK::ParametersAccessor::get( node, "LocalVisualisation", visualisation))
			//sendValuedEvent(visualisation, Vis::VisBase::ADD_VISUAL_OBJECT_EVENT_ID, visualObject);
		//else
			fireValuedSignal(Vis::VisBase::ADD_VISUAL_OBJECT_EVENT_ID, visualObject);

		s_firstCall = false;
	}
	else
	{
		AddAnimatorPrm animator;
		animator.setObjectName(OMK::Name("MyAnimator"));
		animator.setClassId(Name("OMKColorShaderAnimator"));
		animator.setVisualObjectName(OMK::Name("MyObject"));
		//animator.setVisuName(OMK::Name("visuOgre"));
		OMK::ConfigurationParameterDescriptor* configParam = 0;
		OMK::ParametersAccessor::set(configParam, "NodeName", "Robot") ;
		animator.setConfigParam(configParam);
		fireValuedSignal(Vis::VisBase::ADD_ANIMATOR_EVENT_ID, animator);
	}

	return true;
}
*/
