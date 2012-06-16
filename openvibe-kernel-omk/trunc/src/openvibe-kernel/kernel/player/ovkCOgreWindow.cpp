#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "OgreRenderSystem.h"
#include "OgreStringConverter.h"

#include "ovkCOgreVisualisation.h"
#include "ovkCOgreScene.h"
#include "ovkCOgreWindow.h"
#include "ovkCOgreFrameListener.h"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

COgreWindow::COgreWindow(const IKernelContext& rKernelContext, const Ogre::String& rName, COgreVisualisation* pOgreVis, COgreScene* pOgreScene)
	:m_rKernelContext(rKernelContext)
	,m_sName(rName)
	,m_pOgreVis(pOgreVis)
	,m_pOgreScene(pOgreScene)
	,m_pRenderWindow(NULL)
	,m_pSceneManager(NULL)
	,m_pCamera(NULL)
	,m_f32NearFarClipRatio(1.f/1000)
	,m_pViewport(NULL)
	,m_pFrameListener(NULL)
	,m_bLeftButtonPressed(false)
	,m_bMiddleButtonPressed(false)
	,m_bRightButtonPressed(false)
	,m_i32X(0)
	,m_i32Y(0)
	,m_f32DefaultTranslationFactor(0.02f) //in units per pixel
	,m_f32TranslationFactor(0.02f)
	,m_f32RotationFactor(1.f) //in degrees per pixel
{
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

COgreWindow::~COgreWindow()
{
	destroyView();
}

const Ogre::String& COgreWindow::getName()
{
	return m_sName;
}

boolean COgreWindow::createRenderWindow(const std::string& rExternalHandle, unsigned int uiWidth, unsigned int uiHeight)
{
	//ensure Ogre render window hasn't already been allocated
	if(m_pRenderWindow != NULL)
	{
		return false;
	}

	//store external window handle in struct
	Ogre::NameValuePairList params;
	params["externalWindowHandle"] = rExternalHandle;

	//create render window from settings passed in argument
	try
	{
		m_pRenderWindow = Ogre::Root::getSingleton().createRenderWindow(
			m_sName,// + "_RenderWindow",
			uiWidth,
			uiHeight,
			false, //don't use fullscreen with non top level windows
			&params);
	}
	catch(Ogre::Exception& e)
	{
		m_rKernelContext.getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to create render window : " << e.what() << "\n";
		return false;
	}

	return m_pRenderWindow != NULL;
}

void COgreWindow::update()
{
	if(m_pRenderWindow == NULL)
	{
		return;
	}
	m_pRenderWindow->update(true);
	//FIXME : this call shouldn't have to be made, but for some reason,
	//the frameEnded callback doesn't get called...
	Ogre::FrameEvent e;
	m_pFrameListener->frameEnded(e);
	//
}

boolean COgreWindow::handleRealizeEvent()
{
	return createView();
}

boolean COgreWindow::handleUnrealizeEvent()
{
	if(m_pRenderWindow == NULL)
	{
		return false;
	}
	//Destroy the render window, but only if it's not the primary. Ogre will destroy the primary window on shutdown.
	if(!m_pRenderWindow->isPrimary())
	{
#if 1
		Ogre::Root::getSingleton().getRenderSystem()->destroyRenderWindow(m_pRenderWindow->getName());
#else
		//TODO : implement this method!
		m_pOgreVis->destroyRenderWindow(m_pRenderWindow);
#endif
		m_pRenderWindow = NULL;
		return true;
	}
	else
	{
		//primary window destroyed by Ogre upon shutdown
		return false;
	}
}

boolean COgreWindow::handleSizeAllocateEvent(unsigned int uiWidth, unsigned int uiHeight)
{
	if(m_pRenderWindow == NULL)
  {
		return false;
	}

#if defined OVK_OS_Windows
	m_pRenderWindow->resize(uiWidth, uiHeight);
	m_pRenderWindow->windowMovedOrResized();
#elif defined OVK_OS_Linux
	m_pRenderWindow->resize(uiWidth, uiHeight);
#endif

	update();
	return true;
}

boolean COgreWindow::handleExposeEvent()
{
	update();
	return true;
}

boolean COgreWindow::handleMotionEvent(int i32X, int i32Y)
{
	if(m_pCamera == NULL)
	{
		return false;
	}

	if(m_bLeftButtonPressed == true)
	{
		//rotate camera
		float32 f32Theta = get180Angle(m_f32Theta - m_f32RotationFactor * (i32Y - m_i32Y));
		float32 f32Phi = get360Angle(m_f32Phi - m_f32RotationFactor * (i32X - m_i32X));
		setCameraSphericalAngles(f32Theta, f32Phi);
	}
	else if(m_bRightButtonPressed == true)
	{
		//translate camera
		m_f32Radius += m_f32TranslationFactor * (i32Y - m_i32Y);
		if(m_f32Radius < 0)
		{
			m_f32Radius = 0;
		}
	}
	else if(m_bMiddleButtonPressed == true)
	{
	}

	updateCamera();
	//update();
	m_i32X = i32X;
	m_i32Y = i32Y;

	return true;
}

boolean COgreWindow::handleButtonPressEvent(unsigned int uiButton, int i32X, int i32Y)
{
	m_i32X = i32X;
	m_i32Y = i32Y;

	if(uiButton == 1) //left button
	{
		m_bLeftButtonPressed = true;
	}
	else if(uiButton == 2)
	{
		m_bMiddleButtonPressed = true;
	}
	else if(uiButton == 3) //right button
	{
		m_bRightButtonPressed = true;
	}

	return true;
}

boolean COgreWindow::handleButtonReleaseEvent(unsigned int uiButton, int i32X, int i32Y)
{
	if(uiButton == 1) //left button
	{
		m_bLeftButtonPressed = false;
	}
	else if(uiButton == 2)
	{
		m_bMiddleButtonPressed = false;
	}
	else if(uiButton == 3) //right button
	{
		m_bRightButtonPressed = false;
	}

	return true;
}

boolean COgreWindow::getCameraSphericalCoordinates(float32& rTheta, float32& rPhi, float32& rRadius)
{
	rTheta = m_f32Theta;
	rPhi = m_f32Phi;
	rRadius = m_f32Radius;
	return true;
}

boolean COgreWindow::setCameraSphericalCoordinates(float32 f32Theta, float32 f32Phi, float32 f32Radius)
{
	setCameraSphericalAngles(f32Theta, f32Phi);
	m_f32Radius = f32Radius;
	//reflect changes
	updateCamera();
	return true;
}

boolean COgreWindow::setBackgroundColor(float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	if(m_pViewport == NULL)
	{
		return false;
	}

	m_pViewport->setBackgroundColour(Ogre::ColourValue(f32ColorRed, f32ColorGreen, f32ColorBlue));

	return true;
}

boolean COgreWindow::setCameraToEncompassObjects()
{
	float32 l_f32MinX, l_f32MinY, l_f32MinZ;
	float32 l_f32MaxX, l_f32MaxY, l_f32MaxZ;

	m_pOgreScene->getBoundingBox(l_f32MinX, l_f32MinY, l_f32MinZ, l_f32MaxX, l_f32MaxY, l_f32MaxZ);

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

	//update clip planes accordingly, so that near/far ratio is constant
	float32 l_f32FarClipDistance = 4*l_f32LargestDimension; //purely arbitrary!
	float32 l_f32NearClipDistance = l_f32FarClipDistance * m_f32NearFarClipRatio;
	if(l_f32NearClipDistance < 1)
	{
		l_f32NearClipDistance = 1;
	}
	m_pCamera->setNearClipDistance(l_f32NearClipDistance);
	m_pCamera->setFarClipDistance(l_f32FarClipDistance);

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

boolean COgreWindow::createView()
{
	if(m_pRenderWindow == NULL)
	{
		return false;
	}

	//initialize scene
	m_pOgreScene->init();

	//retrieve scene manager
	m_pSceneManager = m_pOgreScene->getSceneManager();

	//create default camera
	m_pCamera = m_pSceneManager->createCamera(string(m_sName) + "_Camera_0");
	m_pCamera->setAutoAspectRatio(true);
	float32 l_f32NearClipDistance = 1.f;
	m_pCamera->setNearClipDistance(l_f32NearClipDistance);
	m_pCamera->setFarClipDistance(l_f32NearClipDistance / m_f32NearFarClipRatio);
	m_pCamera->setFixedYawAxis(true);
	updateCamera();

	//create default viewport
	m_pViewport = m_pRenderWindow->addViewport(m_pCamera);
	m_pViewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));

	//create frame listener
	m_pFrameListener= new COgreFrameListener(m_pRenderWindow);

	if(m_pOgreVis->isOgreStatsOverlayEnabled() == true)
	{
		m_pFrameListener->showDebugOverlay(true);
	}

	m_pOgreVis->getOgreRoot()->addFrameListener(m_pFrameListener);

	return true;
}

boolean COgreWindow::destroyView()
{
	if(m_pSceneManager == NULL)
	{
		return false;
	}

	//destroy default camera
	m_pSceneManager->destroyCamera(m_pCamera);

	//viewport will be destroyed along with render window

	if(m_pFrameListener != NULL)
	{
		delete m_pFrameListener;
	}

	return true;
}

boolean COgreWindow::computeTranslationFactor(OpenViBE::float32 f32SceneSize)
{
	m_f32TranslationFactor = f32SceneSize * m_f32DefaultTranslationFactor;

	return true;
}

//FIXME : detect when mouse exits/enters Gtk windows and update
//camera only when it is inside windows. Otherwise, when mouse exits window,
//weird things happen with cursor values
#define ANGLE_MAX_VAL 1E3

float32 COgreWindow::get360Angle(float32 f32Angle)
{
	if(f32Angle > ANGLE_MAX_VAL || f32Angle < -ANGLE_MAX_VAL)
	{
		f32Angle = 0;
	}

	f32Angle -= (int)(f32Angle/360)*360;

	if(f32Angle < 0)
	{
		f32Angle += 360;
	}

	return f32Angle;
}

float32 COgreWindow::get180Angle(float32 f32Angle)
{
	if(f32Angle > ANGLE_MAX_VAL || f32Angle < -ANGLE_MAX_VAL)
	{
		f32Angle = 0;
	}

	if(f32Angle < 0)
	{
		f32Angle = 0;
	}
	else if(f32Angle > 180)
	{
		f32Angle = 180;
	}

	return f32Angle;
}

boolean COgreWindow::setCameraSphericalAngles(float32 f32Theta, float32 f32Phi)
{
	//FIXME : when using angles of 0 or 180 degrees, camera up vector is set to (1, 0, 0)
	//when using forcedYawAxis on camera, resulting in break of continuity in camera orientation
#define THETA_THRESHOLD 0.1f
	if(f32Theta < THETA_THRESHOLD)
	{
		m_f32Theta = THETA_THRESHOLD;
	}
	else if(f32Theta > 180 - THETA_THRESHOLD)
	{
		m_f32Theta = 180 - THETA_THRESHOLD;
	}
	else
	{
		m_f32Theta = f32Theta;
	}

	m_f32Phi = f32Phi;

	return true;
}

boolean COgreWindow::updateCamera()
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
