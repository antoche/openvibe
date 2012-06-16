#include "ovkCMaterial.h"
#include "ovkCOgreWindow.h"
#include "ovkCOgreScene.h"
#include "ovkCOgreResourceGroup.h"
#include "ovkCOgreVisualisation.h"

using namespace std;
using namespace Ogre;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

#define uint64 OpenViBE::uint64

COgreVisualisation::COgreVisualisation(const OpenViBE::Kernel::IKernelContext& rKernelContext)
	:TKernelObject<IObject>(rKernelContext)
	,m_eLogLevel(LogLevel_Trace)
	,m_bOgreInitialised(false)
	,m_bResourcesInitialised(false)
	,m_pRoot(NULL)
	,m_pLog(NULL)
{
}

COgreVisualisation::~COgreVisualisation()
{
	finish();
}

void COgreVisualisation::messageLogged(const String& message, LogMessageLevel lml, bool maskDebug, const String &logName)
{
	switch(lml)
	{
		case Ogre::LML_CRITICAL:
		case Ogre::LML_TRIVIAL:
		case Ogre::LML_NORMAL:
		default:
			this->getLogManager() << m_eLogLevel << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Log> " << message.c_str() << "\n";
			break;
	}
}

boolean COgreVisualisation::initializeOgre(const CString& rPluginsFile, boolean bLogToScreen, const CString& rLogFileName) throw (std::exception)
{
#if 0
	CNameValuePairList l_oOgreParams;
	l_oOgreParams.setValue("PluginsFile", rPluginsFile.toASCIIString());
	l_oOgreParams.setValue("CaptureMessages", "1");
	l_oOgreParams.setValue("OutputCapturedMessages", bLogToScreen ? "1" : "0");
	l_oOgreParams.setValue("OutputFileName", rLogFileName);

	return initializeOgre(l_oOgreParams);
#else
	this->getLogManager() << LogLevel_ImportantWarning << "This function has been removed (" << CString("boolean COgreVisualisation::initializeOgre(const CString&, boolean, const CString&)") << "\n";
	return initializeOgre();
#endif
}

boolean COgreVisualisation::initializeOgre(void) throw (std::exception)
{
	try
	{
		//should Ogre messages be forwarded to OpenViBE's log manager?
		m_eLogLevel=ELogLevel(this->getConfigurationManager().expandAsEnumerationEntryValue("${Kernel_3DVisualisationOgreLogLevel}", OV_TypeId_LogLevel, LogLevel_Trace));

		//configure custom log
		//--------------------
		LogManager* l_pLogManager = new LogManager();
		//create custom log
		m_pLog = l_pLogManager->createLog("" /* log filename */, true /* is default log */, false /* send log messages to debugger */, true /* suppress file log */);
		//get log messages to forward them to OV's log manager, if required
		m_pLog->addListener(this);

		//initialize Ogre
		//---------------

		//configuration file is assumed to be copied to working dir
		CString l_sConfigFile=this->getConfigurationManager().expand("${Kernel_3DVisualisationOgreConfiguration}");
		//CString l_sConfigFile("../share/openvibe-kernel-omk/ogre.cfg");

		this->getLogManager() << LogLevel_Trace << "Ogre configuration file " << l_sConfigFile << "\n";

		//retrieve plugins file
		CString l_sPluginsFile=this->getConfigurationManager().expand("${Kernel_3DVisualisationOgrePlugins}");
		//CString l_sPluginsFile("../../dependencies/ogre/bin/debug/Plugins.cfg");

		this->getLogManager() << LogLevel_Trace << "Ogre plugins file " << l_sPluginsFile << "\n";

		//create Ogre Root. This will cause a crash under Windows if the wrong version (debug/release) of Ogre dlls
		//are referenced in the 'plugins' file!
		this->getLogManager() << LogLevel_Trace << "Creating Ogre root object...\n";
		m_pRoot = new Ogre::Root(l_sPluginsFile.toASCIIString(), l_sConfigFile.toASCIIString(), "");
		this->getLogManager() << LogLevel_Trace << "Ogre root object created!\n";

		//resources file is assumed to be copied to working dir
		CString l_sResourcesFile=this->getConfigurationManager().expand("${Kernel_3DVisualisationOgreResources}");
		//CString l_sResourcesFile("../share/openvibe-kernel-omk/resources.cfg");

		this->getLogManager() << LogLevel_Trace << "Ogre resources file " << l_sResourcesFile << "\n";

		//add resource locations from resources file (if any)
		addResourceLocations(l_sResourcesFile.toASCIIString());

		//init Ogre from last configuration, or show config dialog if none was found
		if((m_pRoot->restoreConfig() == true) || (m_pRoot->showConfigDialog() == true))
		{
			//init Ogre w/o creating a default window
			m_pRoot->initialise(false);
			m_bOgreInitialised = true;
		}
	}
	catch(Ogre::Exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to initialize Ogre : " << e.what() << "\n";
	}

	return m_bOgreInitialised;
}

boolean COgreVisualisation::ogreInitialized()
{
	return m_bOgreInitialised;
}

boolean COgreVisualisation::initializeResources() throw (Ogre::Exception)
{
	//ensure ogre initialisation hasn't been performed already
	if(m_bResourcesInitialised == true)
	{
		return false;
	}

	//init resources
	try
	{
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
	catch(Ogre::Exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to initialize Ogre resources : " << e.what() << "\n";
	}

	m_bResourcesInitialised = true;

	return true;
}

boolean COgreVisualisation::resourcesInitialized()
{
	return m_bResourcesInitialised;
}

boolean COgreVisualisation::finish()
{
	try
	{
		//destroy windows
		std::map<OpenViBE::CIdentifier, COgreWindow*>::iterator itWindows = m_mOgreWindows.begin();
		while(itWindows != m_mOgreWindows.end())
		{
			delete itWindows->second;
			m_mOgreWindows.erase(itWindows);
			itWindows = m_mOgreWindows.begin();
		}

		//destroy scenes
		std::map<OpenViBE::CIdentifier, COgreScene*>::iterator itScenes = m_mOgreScenes.begin();
		while(itScenes != m_mOgreScenes.end())
		{
			delete itScenes->second;
			m_mOgreScenes.erase(itScenes);
			itScenes = m_mOgreScenes.begin();
		}

		//destroy resources
		std::map<OpenViBE::CIdentifier, COgreResourceGroup*>::iterator itResources = m_mOgreResourceGroups.begin();
		while(itResources != m_mOgreResourceGroups.end())
		{
			delete itResources->second;
			m_mOgreResourceGroups.erase(itResources);
			itResources = m_mOgreResourceGroups.begin();
		}

		if(m_pRoot != NULL)
		{
			delete m_pRoot;
			m_pRoot = NULL;
		}
	}
	catch(Ogre::Exception)
	{
		//
		return false;
	}

	return true;
}

void COgreVisualisation::idle()
{
	//update all windows
	std::map<OpenViBE::CIdentifier, COgreWindow*>::iterator it;

	for(it = m_mOgreWindows.begin(); it != m_mOgreWindows.end(); it++)
	{
		it->second->update();
	}
}

RenderWindow* COgreVisualisation::createRenderWindow(const std::string& rName, const std::string& rExternalHandle, unsigned int uiWidth, unsigned int uiHeight)
{
	RenderWindow* l_pRenderWindow = NULL;

	//store external window handle in struct
	Ogre::NameValuePairList params;
	params["parentWindowHandle"] = rExternalHandle;

	//create render window from settings passed in argument
	try
	{
		l_pRenderWindow = Ogre::Root::getSingleton().createRenderWindow(
			rName,
			uiWidth,
			uiHeight,
			false, //don't use fullscreen with non top level windows
			&params);
	}
	catch(Ogre::Exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to create render window : " << e.what() << "\n";
		return NULL;
	}

	return l_pRenderWindow;
}

COgreResourceGroup* COgreVisualisation::createResourceGroup(CIdentifier& rResourceGroupIdentifier, const std::string& rResourceGroupName)
{
	//check whether a similarly named group was already created by another plugin
	std::map<OpenViBE::CIdentifier, COgreResourceGroup*>::iterator it = m_mOgreResourceGroups.begin();
	while(it != m_mOgreResourceGroups.end())
	{
		if(it->second->getName() == rResourceGroupName)
		{
			this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit
			<< "> A resource group named " << rResourceGroupName.c_str() << " was found! Creation aborted\n";
			rResourceGroupIdentifier = it->first;
			return it->second;
		}
		it++;
	}

	//if a new resource group must be created
	if(rResourceGroupIdentifier == OV_UndefinedIdentifier)
	{
		rResourceGroupIdentifier = getUnusedResourceGroupIdentifier();
		return m_mOgreResourceGroups[rResourceGroupIdentifier] = new COgreResourceGroup(this->getKernelContext(), rResourceGroupName);
	}
	else //a specific ID was set
	{
		//ensure a group with this ID exists
		if(m_mOgreResourceGroups.find(rResourceGroupIdentifier) != m_mOgreResourceGroups.end())
		{
			//ensure group with this ID is named as specified
			if(m_mOgreResourceGroups[rResourceGroupIdentifier]->getName() == rResourceGroupName)
			{
				return m_mOgreResourceGroups[rResourceGroupIdentifier];
			}
		}
		//error
		this->getLogManager()
		<< LogLevel_Warning << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit
		<< "> Can't find resource group with ID " << rResourceGroupIdentifier <<" and name " << rResourceGroupName.c_str() << "\n";
		return NULL;
	}
}

boolean COgreVisualisation::addResourceLocation(const CIdentifier& rResourceGroupIdentifier, const std::string& rPath, const std::string& rType, bool bRecursive)
{
	if(m_mOgreResourceGroups.find(rResourceGroupIdentifier) == m_mOgreResourceGroups.end())
	{
		return false;
	}

	return m_mOgreResourceGroups[rResourceGroupIdentifier]->addResourceLocation(rPath, rType, bRecursive);
}

boolean COgreVisualisation::initializeResourceGroup(const CIdentifier& rResourceGroupIdentifier)
{
	if(m_mOgreResourceGroups.find(rResourceGroupIdentifier) == m_mOgreResourceGroups.end())
	{
		return false;
	}
	return m_mOgreResourceGroups[rResourceGroupIdentifier]->initialize();
}

boolean COgreVisualisation::destroyResourceGroup(const CIdentifier& rResourceGroupIdentifier)
{
	if(m_mOgreResourceGroups.find(rResourceGroupIdentifier) == m_mOgreResourceGroups.end())
	{
		return false;
	}
	return m_mOgreResourceGroups[rResourceGroupIdentifier]->destroy();
}

COgreScene* COgreVisualisation::createScene(const CIdentifier& rSceneIdentifier)
{
	//ensure scene hasn't been created yet
	if(m_mOgreScenes.find(rSceneIdentifier) == m_mOgreScenes.end())
	{
		char l_sBuffer[1024];
		// sprintf(l_sBuffer, "Scene_%u", (unsigned int)m_mOgreScenes.size());
		sprintf(l_sBuffer, "Scene_%s", rSceneIdentifier.toString().toASCIIString());
		m_mOgreScenes[rSceneIdentifier] = new COgreScene(this->getKernelContext(), String(l_sBuffer), this);
	}
	return m_mOgreScenes[rSceneIdentifier];
}

COgreWindow* COgreVisualisation::createWindow(CIdentifier rWindowIdentifier, const string& rWindowName, CIdentifier rSceneIdentifier)
{
	//ensure scene has been created
	if(m_mOgreScenes.find(rSceneIdentifier) == m_mOgreScenes.end())
	{
		return NULL;
	}

	//ensure window hasn't been created yet
	if(m_mOgreWindows.find(rWindowIdentifier) != m_mOgreWindows.end())
	{
		return NULL;
	}

	//create Ogre window
	m_mOgreWindows[rWindowIdentifier] = new COgreWindow(this->getKernelContext(), rWindowName, this, m_mOgreScenes[rSceneIdentifier]);

	return m_mOgreWindows[rWindowIdentifier];
}

Ogre::Root* COgreVisualisation::getOgreRoot()
{
	return m_pRoot;
}

COgreWindow* COgreVisualisation::getOgreWindow(CIdentifier rWindowIdentifier)
{
	if(m_mOgreWindows.find(rWindowIdentifier) == m_mOgreWindows.end())
	{
		return NULL;
	}
	return m_mOgreWindows[rWindowIdentifier];
}

COgreScene* COgreVisualisation::getOgreScene(CIdentifier rSceneIdentifier)
{
	if(m_mOgreScenes.find(rSceneIdentifier) == m_mOgreScenes.end() || m_mOgreScenes[rSceneIdentifier] == NULL)
	{
		return NULL;
	}
	return m_mOgreScenes[rSceneIdentifier];
}

boolean COgreVisualisation::deleteWindow(CIdentifier rWindowIdentifier)
{
	if(m_mOgreWindows.find(rWindowIdentifier) == m_mOgreWindows.end())
	{
		return false;
	}
	this->getLogManager() << LogLevel_Trace << "Destroying 3D window " << m_mOgreWindows[rWindowIdentifier]->getName().c_str() << "\n";
	delete m_mOgreWindows[rWindowIdentifier];
	m_mOgreWindows.erase(rWindowIdentifier);

	return true;
}

boolean COgreVisualisation::deleteScene(CIdentifier rSceneIdentifier)
{
	if(m_mOgreScenes.find(rSceneIdentifier) == m_mOgreScenes.end())
	{
		return false;
	}
	this->getLogManager() << LogLevel_Trace << "Destroying 3D scene " << m_mOgreScenes[rSceneIdentifier]->getName().c_str() << "\n";
	delete m_mOgreScenes[rSceneIdentifier];
	m_mOgreScenes.erase(rSceneIdentifier);
	return true;
}

boolean COgreVisualisation::isOgreStatsOverlayEnabled()
{
	return getConfigurationManager().expandAsBoolean("${Kernel_3DVisualisationOgreStatisticsOverlayEnabled}", false);
}

void COgreVisualisation::addResourceLocations(const std::string& resourcesFile)
{
	try
	{
		//load resource paths from config file
		ConfigFile configFile;
		this->getLogManager() << LogLevel_Trace << "Loading resources file\n";
		configFile.load(resourcesFile);

		String resourceGroupName;
		String resourceTypeName;
		String resourceName;

		//go through all sections
		ConfigFile::SectionIterator sectionIterator(configFile.getSectionIterator());
		while(sectionIterator.hasMoreElements() == true)
		{
			//each section corresponds to a resource group
			resourceGroupName = sectionIterator.peekNextKey();
			ConfigFile::SettingsMultiMap* settings(sectionIterator.getNext());

			//go through all resources
			for(ConfigFile::SettingsMultiMap::iterator i(settings->begin()); i != settings->end(); ++i)
			{
				//each entry is a resource location for the current resource group
				resourceTypeName = i->first;
				resourceName = i->second;
				this->getLogManager() << LogLevel_Trace << "Adding resource location " << resourceName.c_str() << "\n";
				ResourceGroupManager::getSingleton().addResourceLocation(resourceName, resourceTypeName, resourceGroupName);
			}
		}
	}
	catch(Ogre::Exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< e.what() << "\n";
	}
}

CIdentifier COgreVisualisation::getUnusedResourceGroupIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	std::map<CIdentifier, COgreResourceGroup*>::const_iterator it;
	do
	{
		l_oResult=CIdentifier(l_ui64Identifier++);
		it=m_mOgreResourceGroups.find(l_oResult);
	}
	while(it!=m_mOgreResourceGroups.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
