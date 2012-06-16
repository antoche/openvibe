#ifndef __OpenViBEApplication_COgreVRApplication_H__
#define __OpenViBEApplication_COgreVRApplication_H__

#include "ova_defines.h"
#include <Ogre.h>
#include <OIS.h>
#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "ovaCAbstractVrpnPeripheral.h"

#include <map>

#define MAX_FREQUENCY 60.f

/**
 * \namespace OpenViBEVRDemos
 * \author Laurent Bonnet (INRIA/IRISA)
 * \date 2010-02-16
 * \brief Contains all VR-demos related code (Ogre application and VRPN peripheral).
 */
namespace OpenViBEVRDemos {

	/**
	 * \class COgreVRApplication
	 * \author Laurent Bonnet (INRIA/IRISA)
	 * \date 2010-02-16
	 * \brief Basic framework to design VR applications.
	 *
	 * \details This class is based on Ogre3D, and should be derived to design a specific VR application.
	 * It handles the rendering engine setup and configuration. Developer has to initialise the right 3D scene,
	 * and give the loop code that will be called each time a frame is rendered.
	 */
	class COgreVRApplication : public Ogre::FrameListener ,OIS::KeyListener, OIS::MouseListener, Ogre::WindowEventListener
	{
		public:

			/**
			* \brief Default constructor.
			*/
			COgreVRApplication();

			/**
			* \brief Destructor.
			*/
			virtual ~COgreVRApplication();

			/**
			* \brief Launches the application (engine setup, initialization, rendering start).
			*/
			virtual void go(void);

		protected:

			bool m_bContinue; //!< Tells if the rendering process should continue after the current frame.

			/**
			* \brief Main Ogre and custom setup steps
			* \return \em true if the setup is a success.
			*/
			virtual bool setup(void);

			/**
			* \brief Configuration management
			* \return \em true if the configuration step is a success.
			*/
			virtual bool configure(void);

			/**
			* \brief Parsing of the resource configuration file
			*/
			virtual void setupResources(void);

			//----- Main Ogre objects ------//
			Ogre::Root* m_poRoot;					//!< Ogre root.
			Ogre::SceneManager* m_poSceneManager;	//!< The scene manager used.
			Ogre::RenderWindow* m_poWindow;			//!< The render window used.
			Ogre::Camera* m_poCamera;				//!< The camera used.
			Ogre::SceneNode* m_poCameraNode;			//!< The camera node.
			
			Ogre::String m_sResourcePath;			//!< Path to the file resource.cfg for Ogre.

			bool m_bCameraMode;
			std::map<OIS::KeyCode,bool> m_mKeysPressed;
			virtual void updateCamera(void);

			/**
			* \brief Initialize the scene. This function has to be implemented
			* with the specific Ogre code.
			* \return \em true if the initialization process is a success.
			*/
			virtual bool initialise(void)=0;
			/**
			* \brief Main loop. Developer has to implement this function
			* with the specific behaviour of its a application.
			* \return \em true if the configuration step is a success.
			*/
			virtual bool process(double timeSinceLastProcess)=0;

			/**
			* \brief Frame started callback.
			*/
			bool frameStarted(const Ogre::FrameEvent& evt);

			/**
			* \brief Frame ended callback.
			* \return \em true if the rendering engine should continue.
			*/
			bool frameEnded(const Ogre::FrameEvent& evt) { return true; }

			//-------OIS-------//
			OIS::InputManager* m_poInputManager;				//!< The OIS input manager.
			OIS::Mouse* m_poMouse;								//!< The mouse.
			OIS::Keyboard* m_poKeyboard;						//!< The keyboard.

			/**
			* \brief Initialize the OIS plugin.
			* \return \em true if the setup is a success.
			*/
			virtual bool initOIS(void);

			/**
			* \brief Mouse moved callback, launched when the mouse is moved.
			* \return \em true if the rendering engine should continue.
			*/
			bool mouseMoved(const OIS::MouseEvent& arg);

			/**
			* \brief Mouse pressed callback, launched when a mouse button is pressed.
			* \return \em true if the rendering engine should continue.
			*/
			bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }

			/**
			* \brief Mouse released callback, launched when a mouse button is released.
			* \return \em true if the rendering engine should continue.
			*/
			bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }

			/**
			* \brief Key pressed callback, launched when a key is pressed.
			* \return \em true if the rendering engine should continue.
			*/
			bool keyPressed(const OIS::KeyEvent& evt);
			/**
			* \brief Key released callback, launched when a key is released.
			* \return \em true if the rendering engine should continue.
			*/
			bool keyReleased(const OIS::KeyEvent& evt) ;

			virtual void windowResized(Ogre::RenderWindow* rw);

			//-------CEGUI-------//
			CEGUI::OgreRenderer * m_rGUIRenderer;  //!< The CEGUI renderer.
			//CEGUI::System& m_rGUISystem;               //!< The CEGUI system.
			CEGUI::WindowManager *m_poGUIWindowManager; //!< The CEGUI window manager.
			CEGUI::Window *m_poSheet;                   //!< The default sheet.

			/**
			* \brief Initialize the CEGUI plugin.
			* \return \em true if the setup is a success.
			*/
			virtual bool initCEGUI(void);

			//-------VRPN-------//
			CAbstractVrpnPeripheral * m_poVrpnPeripheral;//!< A VRPN peripheric, handles at most one Analog + one Button server.

			//------CLOCK------//
			double m_dClock; //!< Clock to impose a maximum frequency.

	};
};
#endif //__OpenViBEApplication_COgreVRApplication_H__
