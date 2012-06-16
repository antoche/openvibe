#ifndef __OpenViBEKernel_Kernel_Player_COgreVisualisation_H__
#define __OpenViBEKernel_Kernel_Player_COgreVisualisation_H__

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

#include <Ogre.h>

class COgreScene;
class COgreWindow;
class COgreResourceGroup;

/**
 * \class COgreVisualisation
 * \author Vincent Delannoy (INRIA/IRISA)
 * \date 2008-06
 * \brief High level Ogre initialisation & management
 * This class initializes the Ogre library, creates 3D scenes, windows (views
 of 3D scenes) and objects that populate such scenes.
 */
class COgreVisualisation : public OpenViBE::Kernel::TKernelObject<OpenViBE::IObject>, public Ogre::LogListener
{
public:
	/**
	 * \brief Constructor
	 * \param[in] rKernelContext OpenViBE kernel context
	 */
	COgreVisualisation(const OpenViBE::Kernel::IKernelContext& rKernelContext);

	/**
	 * \brief Destructor
	 */
	~COgreVisualisation();

	/**
   * \brief This is called whenever the log recieves a message and is about to write it out
   * \param message The message to be logged
   * \param lml The message level the log is using
   * \param maskDebug If we are printing to the console or not
   * \param logName
   * \logName the name of this log
   */
	virtual void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName);

	/**
	 * \brief Load configuration files and initialise Ogre
	 * Creates Ogre root object, parses resource locations and initialises Ogre from last
	 * configuration (if any) or by displaying the Ogre configuration dialog if no
	 * ogre.cfg is available
	 * \param rPluginsFile Ogre plugins file
	 * \param rLogFileName If not empty, Ogre log dumped to this file
	 * \return True if Ogre was initialised, false otherwise
	 */
	OpenViBE::boolean initializeOgre(
		const OpenViBE::CString& rPluginsFile,
		OpenViBE::boolean bLogToScreen,
		const OpenViBE::CString& rLogFileName) throw (std::exception);

	/**
	 * \brief Load configuration files and initialise Ogre
	 * Creates Ogre root object, parses resource locations and initialises Ogre from last
	 * configuration (if any) or by displaying the Ogre configuration dialog if no
	 * ogre.cfg is available
	 * \return True if Ogre was initialised, false otherwise
	 */
	OpenViBE::boolean initializeOgre(void) throw (std::exception);

	/**
	 * \brief Determine whether Ogre has been initialized yet
	 * \return True if Ogre was initialized, false otherwise
	 */
	OpenViBE::boolean ogreInitialized();

	/**
	 * \brief Complete Ogre initialisation once primary RenderWindow has been created
	 * This function initialises all resource groups. Additionally, render states may be
	 * initialized here if needed.
	 * \return True if Ogre resources could be initialized, false otherwise
	 */
	OpenViBE::boolean initializeResources() throw (Ogre::Exception);

	/**
	 * \brief Determine whether resources have been initialized yet
	 * \return True if resources were initialized, false otherwise
	 */
	OpenViBE::boolean resourcesInitialized();

	/**
	 * \brief Delete all windows, scenes and the Ogre root
	 * \return True if all Ogre resources were successfully released, false otherwise
	 */
	OpenViBE::boolean finish();

	/**
	 * \brief Refreshes all windows
	 */
	void idle();

	/**
	 * \brief Create a RenderWindow from an external window handle
	 * \remarks This function is used to create the primary render window which
	 * is then hidden and destroyed only when the application is shut down.
	 * Secondary windows should be created by COgreWindow objects
	 * \return Pointer to RenderWindow or NULL if it couldn't be created
	 */
	Ogre::RenderWindow* createRenderWindow(
		const std::string& rName,
		const std::string& rExternalHandle,
		unsigned int uiWidth,
		unsigned int uiHeight); // throw (/*Ogre::Exception*/);

	/**
		\name Resource groups management
	 */
	//@{

	/**
	 * \brief Create a resource group
	 * \param[in/out] rResourceGroupIdentifier Must be OV_UndefinedIdentifier if a new
	 * resource group is to be created. Otherwise the method looks for the resource group
	 * with the identifier passed in parameter, returning NULL if none matches this ID.
	 * \param[in] rResourceGroupName If rResourceGroupIdentifier is OV_UndefinedIdentifier
	 * and a group named rResourceGroupName already exists, rResourceGroupIdentifier
	 * will be set to the identifier of this group. Otherwise, a new group named
	 * rResourceGroupName will be created.
   * \return Pointer to resource group or NULL if creation failed
	 */
	COgreResourceGroup* createResourceGroup(
		OpenViBE::CIdentifier& rResourceGroupIdentifier,
		const std::string& rResourceGroupName);

	/**
	 * \brief Add a resource location
	 * \remarks Resources can't be added once Resource Group has been initialized
	 * \param rResourceGroupIdentifier Resource group identifier
	 * \param rPath Resources location path
	 * \param type Resources type (path or zip file)
	 * \param bRecursive Resources are added recursively from location passed in parameter if true
	 * \return True if resource location could be added, false otherwise
	 */
	OpenViBE::boolean addResourceLocation(
		const OpenViBE::CIdentifier& rResourceGroupIdentifier,
		const std::string& rPath,
		const std::string& rType,
		bool bRecursive);

	/**
	 * \brief Initialize resource group
	 * \remarks Each call to this method increments the group's internal reference counter.
	 * When it is set to one (upon first call to this method) the group is actually initialized, but isn't upon
	 * subsequent calls. It has to be destroyed (its ref count set back to 0) and recreated to be reinitialized.
	 * \param rResourceGroupIdentifier Resource group identifier
	 * \return True if resource group could be initialized (or was initialized already), false if an error occured
	 */
	OpenViBE::boolean initializeResourceGroup(
		const OpenViBE::CIdentifier& rResourceGroupIdentifier);

	/**
	 * \brief Destroy resource group
	 * \remarks Each call to this method decrements the group's internal reference counter.
	 * Only when this counter reaches 0 is the group actually destroyed.
	 * \param rResourceGroupIdentifier Resource group identifier
	 * \return True if resource group could be destroyed (or was initialized already), false if an error occured
	 */
	OpenViBE::boolean destroyResourceGroup(
		const OpenViBE::CIdentifier& rResourceGroupIdentifier);
	//@}

	/**
	 * \brief Create a COgreScene object
	 * At least one such object should have been created before createWindow() may be called
	 * \param rSceneIdentifier identifier to give to scene to be created
	 * \return Pointer to newly created COgreScene object
	 */
	COgreScene* createScene(
		const OpenViBE::CIdentifier& rSceneIdentifier);

	/**
	 * \brief Create a COgreWindow object
	 * A window is a view of a 3D scene. The COgreScene object referenced by rSceneIdentifier
	 * should have been created beforehand
	 * \param rWindowIdentifier identifier to give to window to be created
	 * \param rWindowName name to give to window to be created
	 * \param rSceneIdentifier identifier of scene this window is a view of. Must reference an existing scene!
	 * \return Pointer to newly created COgreWindow object if creation succeeded, false otherwise
	 */
	COgreWindow* createWindow(
		OpenViBE::CIdentifier rWindowIdentifier,
		const std::string& rWindowName,
		OpenViBE::CIdentifier rSceneIdentifier);

	/**
	 * \brief Return Ogre root
	 * \return Pointer to Ogre root or NULL if it wasn't created
	 */
	Ogre::Root* getOgreRoot();

	/**
	 * \brief Return a window
	 * \param rWindowIdentifier Identifier of window to be retrieved
	 * \return Pointer to COgreWindow if it could be retrieved, NULL otherwise
	 */
	COgreWindow* getOgreWindow(
		OpenViBE::CIdentifier rWindowIdentifier);

	/**
	 * \brief Return a scene
	 * \param rSceneIdentifier Identifier of scene to be retrieved
	 * \return Pointer to COgreScene if it could be retrieved, NULL otherwise
	 */
	COgreScene* getOgreScene(
		OpenViBE::CIdentifier rSceneIdentifier);

	/**
	 * \brief Delete a window
	 * \param rWindowIdentifier Identifier of window to be deleted
	 * \return True if window was deleted, NULL otherwise
	 */
	OpenViBE::boolean deleteWindow(
		OpenViBE::CIdentifier rWindowIdentifier);

	/**
	 * \remarks Windows using this scene should be deleted first
	 */
	OpenViBE::boolean deleteScene(
		OpenViBE::CIdentifier rSceneIdentifier);

	/**
	 * \brief Checks whether Ogre statistics overlay should be enabled in 3d windows
	 * \return True if stats overlay should be enabled, false otherwise
	 */
	OpenViBE::boolean isOgreStatsOverlayEnabled();

	_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::IObject>, OV_UndefinedIdentifier);

private:
	/// add the resources path given in the resources.cfg file

	/**
	 * \brief Notify Ogre of resources locations.
	 * \remarks Actual parsing (performed in completeOgreInitialisation) should not
	 * take place before the first RenderWindow is created.
	 * \param resourcesFile File containing paths to Ogre resources
	 */
	void addResourceLocations(
		const std::string& resourcesFile);

	OpenViBE::CIdentifier getUnusedResourceGroupIdentifier() const;

private:

	/// Ogre messages log level
	OpenViBE::Kernel::ELogLevel m_eLogLevel;
	/// Flag set to true upon Ogre initialisation
	OpenViBE::boolean m_bOgreInitialised;
	/// Flag set to true upon resources initialisation (post primary render window creation)
	OpenViBE::boolean m_bResourcesInitialised;
	/// Ogre3D root
	Ogre::Root* m_pRoot;
	/// Ogre log
	Ogre::Log* m_pLog;
	/// Maps scene Ids to scene pointers
	std::map<OpenViBE::CIdentifier, COgreScene*> m_mOgreScenes;
	/// Maps window Ids to window pointers
	std::map<OpenViBE::CIdentifier, COgreWindow*> m_mOgreWindows;
	/// Maps resource group Ids to resource groups
	std::map<OpenViBE::CIdentifier, COgreResourceGroup*> m_mOgreResourceGroups;
};

#endif // __OpenViBEKernel_Kernel_Player_COgreVisualisation_H__
