#include "ovkCVisualisationTree.h"
#include "ovkCVisualisationManager.h"
#include "../player/ovkCOgreVisualisation.h"
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
using namespace OpenViBE::Tools;

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

CVisualisationManager::CVisualisationManager(const IKernelContext& rKernelContext)
	:TKernelObject<IVisualisationManager>(rKernelContext)
	,m_pPrimaryRenderWindowWidget(NULL)
	,m_pOgreVisualisation(NULL)
{
}

CVisualisationManager::~CVisualisationManager()
{
	if(m_pPrimaryRenderWindowWidget != NULL)
	{
		//destroy Gtk widget only - the primary RenderWindow it contains will be deleted by Ogre
		gtk_widget_destroy(GTK_WIDGET(m_pPrimaryRenderWindowWidget));
	}

	delete m_pOgreVisualisation;
}

boolean CVisualisationManager::initialize3DContext(void)
{
	try
	{
		//initialize Ogre
		//---------------
		m_pOgreVisualisation = new COgreVisualisation(getKernelContext());
		this->getLogManager() << LogLevel_Trace << "Initializing Ogre\n";
		if(m_pOgreVisualisation->initializeOgre() == false)
		{
			this->getLogManager() << LogLevel_Warning << "Ogre initialization failed, 3D rendering disabled!\n";
			return false;
		}
	}
	catch(std::exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to initialize Ogre : " << e.what() << "\n";
		return false;
	}

	try
	{
		this->getLogManager() << LogLevel_Trace << "Creating primary render window\n";
		//create primary render window
		//----------------------------
		//create custom Gtk widget
		GtkWidget* l_pOVCustomWidget = gtk_ov_custom_new(this);
		//add it to a top level container
		m_pPrimaryRenderWindowWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_container_add(GTK_CONTAINER(m_pPrimaryRenderWindowWidget), l_pOVCustomWidget);
		//show container to realize widget, which is when the render window can be created
		gtk_widget_show_all(m_pPrimaryRenderWindowWidget);
		//hide window from now on
		gtk_widget_hide_all(m_pPrimaryRenderWindowWidget);
	}
	catch(std::exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to create primary render window : " << e.what() << "\n";
		return false;
	}

	return true;
}

boolean CVisualisationManager::createResourceGroup(CIdentifier& rResourceGroupIdentifier, const CString& rResourceGroupName)
{
	if(m_pOgreVisualisation == NULL)
	{
		return false;
	}
	m_pOgreVisualisation->createResourceGroup(rResourceGroupIdentifier, rResourceGroupName.toASCIIString());
	return true;
}

boolean CVisualisationManager::addResourceLocation(const CIdentifier& rResourceGroupIdentifier, const CString& rPath, EResourceType type,	bool bRecursive)
{
	if(m_pOgreVisualisation == NULL)
	{
		return false;
	}
	switch(type)
	{
		case ResourceType_File:
		case ResourceType_Directory:
			return m_pOgreVisualisation->addResourceLocation(rResourceGroupIdentifier, rPath.toASCIIString(), std::string("FileSystem"), bRecursive);
			break;
		case ResourceType_ZipArchive:
			return m_pOgreVisualisation->addResourceLocation(rResourceGroupIdentifier, rPath.toASCIIString(), std::string("Zip"), bRecursive);
			break;
	}
	return false;
}

boolean CVisualisationManager::initializeResourceGroup(const CIdentifier& rResourceGroupIdentifier)
{
	if(m_pOgreVisualisation == NULL || m_pOgreVisualisation->ogreInitialized() == false)
	{
		return false;
	}
	return m_pOgreVisualisation->initializeResourceGroup(rResourceGroupIdentifier);
}

boolean CVisualisationManager::destroyResourceGroup(const CIdentifier& rResourceGroupIdentifier)
{
	if(m_pOgreVisualisation == NULL || m_pOgreVisualisation->ogreInitialized() == false)
	{
		return false;
	}
	return m_pOgreVisualisation->destroyResourceGroup(rResourceGroupIdentifier);
}

COgreVisualisation* CVisualisationManager::getOgreVisualisation()
{
	return m_pOgreVisualisation;
}

boolean CVisualisationManager::handleRealizeEvent(::GtkWidget* pOVCustomWidget)
{
	//ensure Ogre could be initialized
	if(m_pOgreVisualisation->ogreInitialized() == false)
	{
		return false;
	}

	//added for Ogre 1.7
	gtk_widget_realize(pOVCustomWidget);
	gtk_widget_set_double_buffered(pOVCustomWidget, FALSE);

	//create primary render window
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
		m_pOgreVisualisation->createRenderWindow(
			"PrimaryRenderWindow",
			l_sExternalHandle,
			pOVCustomWidget->allocation.width,
			pOVCustomWidget->allocation.height);
		this->getLogManager() << LogLevel_Trace << "Ogre render window created\n";
	}
	catch(std::exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Primary render window couldn't be created : " << e.what() << "\n";
	}

	//complete Ogre initialisation (this must be done once after primary window realization)
	try
	{
		m_pOgreVisualisation->initializeResources();
		this->getLogManager() << LogLevel_Trace << "Ogre resources initialized\n";
	}
	catch(std::exception& e)
	{
		this->getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Ogre resources initialisation failure : " << e.what() << "\n";
	}

	return true;
}

boolean CVisualisationManager::createVisualisationTree(CIdentifier& rVisualisationTreeIdentifier)
{
	//create visualisation tree
	IVisualisationTree* l_pVisualisationTree =
		CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<IVisualisationTree*>(OV_ClassId_Kernel_Visualisation_VisualisationTree);

	//generate an identifier for visualisation tree
	rVisualisationTreeIdentifier=getUnusedIdentifier();

	//store pointer to visualisation tree
	m_vVisualisationTree[rVisualisationTreeIdentifier] = l_pVisualisationTree;

	return true;
}

boolean CVisualisationManager::releaseVisualisationTree(const CIdentifier& rVisualisationTreeIdentifier)
{
	map<CIdentifier, IVisualisationTree*>::iterator it = m_vVisualisationTree.find(rVisualisationTreeIdentifier);
	if(it != m_vVisualisationTree.end())
	{
		CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(it->second);
		m_vVisualisationTree.erase(it);
		return true;
	}

	return false;
}

IVisualisationTree& CVisualisationManager::getVisualisationTree(const CIdentifier& rVisualisationTreeIdentifier)
{
	map<CIdentifier, IVisualisationTree*>::const_iterator it = m_vVisualisationTree.find(rVisualisationTreeIdentifier);
	if(it == m_vVisualisationTree.end())
	{
		this->getLogManager() << LogLevel_Fatal << "Visualisation Tree " << rVisualisationTreeIdentifier << " does not exist !\n";
	}
	return *it->second;
}

boolean CVisualisationManager::enumerateVisualisationTrees(IVisualisationManager::IVisualisationTreeEnum& rCallback) const
{
	map<CIdentifier, IVisualisationTree*>::const_iterator it;
	for(it = m_vVisualisationTree.begin(); it != m_vVisualisationTree.end(); it++)
	{
		if(!rCallback.callback(it->first, *it->second))
		{
			return true;
		}
	}
	return true;
}

boolean CVisualisationManager::setToolbar(const CIdentifier& rVisualisationTreeIdentifier, const CIdentifier& rBoxIdentifier, ::GtkWidget* pToolbar)
{
	IVisualisationTree& l_rVisualisationTree = getVisualisationTree(rVisualisationTreeIdentifier);

	l_rVisualisationTree.setToolbar(rBoxIdentifier, pToolbar);

	return true;
}

boolean CVisualisationManager::setWidget(const CIdentifier& rVisualisationTreeIdentifier, const CIdentifier& rBoxIdentifier, ::GtkWidget* pTopmostWidget)
{
	IVisualisationTree& l_rVisualisationTree = getVisualisationTree(rVisualisationTreeIdentifier);

	l_rVisualisationTree.setWidget(rBoxIdentifier, pTopmostWidget);

	return true;
}

CIdentifier CVisualisationManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, IVisualisationTree*>::const_iterator it;
	do
	{
		l_oResult=CIdentifier(l_ui64Identifier++);
		it=m_vVisualisationTree.find(l_oResult);
	}
	while(it!=m_vVisualisationTree.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
