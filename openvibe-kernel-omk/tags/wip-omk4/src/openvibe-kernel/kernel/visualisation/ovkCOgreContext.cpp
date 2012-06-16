/*
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
#include "OgrePrerequisites.h"
#include "OgreSingleton.h"
#include "OgreRoot.h"
#include "OgreConfigFile.h"

#include "Ogre.h"
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "ovkCOgreWidget.h"
#include "ovkCOgreContext.h"

#include <direct.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;
using namespace Ogre;


#define ORTHOGONAL_CAMERA_DISTANCE   1000.0
#define ORTHOGONAL_CAMERA_NEAR_CLIP   300.0
#define ORTHOGONAL_CAMERA_FAR_CLIP   5000.0

// Globals
const int nbOgreWins = 4;
enum CameraId {TOP, LEFT, FRONT, PERSPECTIVE};
const char* cameraName[4] = {"Top", "Left", "Front", "Perspective"};
Ogre::Camera* camera[nbOgreWins];

// Application entry point
COgreContext::COgreContext() :
	m_bSceneManagerCreated(false)
{	
}

COgreContext::~COgreContext()
{
	release();
}

void COgreContext::init()
{
  //create Ogre instance
	Ogre::String pluginsfile("../media/plugins.cfg");
	Ogre::String configfile("../media/ogre.cfg");

	m_pRoot = new Ogre::Root(pluginsfile, configfile);	

  if(!m_pRoot->restoreConfig())
    if(!m_pRoot->showConfigDialog())
      throw std::exception();

  m_pRoot->initialise(false);

	//load resource paths from config file
  Ogre::ConfigFile cf;
  cf.load("../media/resources.cfg");
  
  //go through all sections & settings in the file
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
  
  Ogre::String secName, typeName, archName;
  while (seci.hasMoreElements())
  {
    secName = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
	      archName, typeName, secName);
		}
  }
}

boolean COgreContext::newOgreWidget(void* pParentWidget, OpenViBE::uint32 ui32Index)
{
	char buf[MAX_PATH];
	sprintf(buf, "%d", m_vGtkOgre.size());

	GtkOgre* l_pGtkOgre = GTK_OGRE(gtk_ogre_new(buf));
	
	//store widget
	m_vGtkOgre.push_back(l_pGtkOgre);	

	if(GTK_IS_PANED(pParentWidget) != 0)
	{
		if(ui32Index == 0)
			gtk_paned_add1(GTK_PANED(pParentWidget), &l_pGtkOgre->widget);
		else if(ui32Index == 1)
			gtk_paned_add2(GTK_PANED(pParentWidget), &l_pGtkOgre->widget);
		else
			return false;
	}
	else if(GTK_IS_NOTEBOOK(pParentWidget) != 0)
	{
		GtkWidget* l_pChild = gtk_notebook_get_nth_page(GTK_NOTEBOOK(pParentWidget), ui32Index);
		GtkWidget* l_pLabel = gtk_notebook_get_tab_label(GTK_NOTEBOOK(pParentWidget), l_pChild);
		CString oLabel = gtk_label_get_text(GTK_LABEL(l_pLabel));
		gtk_notebook_remove_page(GTK_NOTEBOOK(pParentWidget), ui32Index);				
		gtk_notebook_insert_page(GTK_NOTEBOOK(pParentWidget), &l_pGtkOgre->widget, gtk_label_new((const char*)oLabel), ui32Index);				
	}
	else
	{
		return false;
	}	

	return true;
}

//widgets need to have been realized (==shown) before scenemanager can actually be created
boolean COgreContext::realizeOgreWidgets()
{
	Ogre::SceneNode* sn;

	for(uint32 i=0; i<m_vGtkOgre.size(); i++)
	{
		if(i==0)
		{
			sn = initScene();
			m_bSceneManagerCreated = true;
		}

		GtkOgre* pGtkOgre = m_vGtkOgre[i];

		//create camera
		m_vGtkOgre[i]->mCamera = createCamera(i, m_vGtkOgre[i]);

		m_vGtkOgre[i]->mEntityNode = sn;

		//create viewport
		Ogre::RenderWindow* l_pRenderWindow = gtk_ogre_get_render_window(m_vGtkOgre[i]);
		if(l_pRenderWindow != NULL)
		{
			Ogre::Viewport* l_pViewport = l_pRenderWindow->addViewport(m_vGtkOgre[i]->mCamera);
			l_pViewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));
			m_vGtkOgre[i]->mViewport = l_pViewport;	
		}		
	}

	return true;
}

Ogre::SceneNode* COgreContext::initScene()
{
	Ogre::SceneNode* entityNode;

  // Initialise resource groups, i.e. parse scripts etc.
  // This must be done after the primary window has been created.
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	 m_pSceneManager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);

	// Setup lighting
  m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
  Ogre::Light* light = m_pSceneManager->createLight("MainLight");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  Ogre::Vector3 dir(1, -1, -0.5);
  dir.normalise();
  light->setDirection(dir);
  
	Ogre::Entity* entity;
	
  // Create a floor plane mesh
  //Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0.0);
  //Ogre::MeshManager::getSingleton().createPlane("FloorPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 200000, 200000, 20, 20, true, 1, 500, 500, Ogre::Vector3::UNIT_Z);
  
  // Add a floor to the scene
	entity = m_pSceneManager->createEntity("floor", "FloorPlane");
  entity->setMaterialName("Examples/RustySteel");
  m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(entity);

	entity = m_pSceneManager->createEntity("head0", "head.mesh");
  entityNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0.0, 10.0, 40.0));
	entityNode->attachObject(entity);

  
  // Add the mandatory ogre heads ;)
  //entity = m_pSceneManager->createEntity("head0", "ogrehead.mesh");
  //m_pSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0.0, 10.0, 40.0))->attachObject(entity);
  
  //entity = m_pSceneManager->createEntity("head1", "ogrehead.mesh");
  //m_pSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-50.0, 10.0, -10.0))->attachObject(entity);
  
  //entity = m_pSceneManager->createEntity("head2", "ogrehead.mesh");
  //m_pSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(70.0, 10.0, -40.0))->attachObject(entity);
	
	return entityNode;
}

void COgreContext::release()
{
	for(unsigned int i=0; i<m_vGtkOgre.size(); i++)
	{
		//gtk_widget_destroy(&m_vGtkOgre[i]->widget);
	}

	m_vGtkOgre.clear();

	if(m_pRoot != NULL)
	{
		delete m_pRoot;
		m_pRoot = NULL;
	}
}


//gboolean COgreContext::delete_event_cb(GtkWidget *widget, GdkEvent  *event, gpointer data)
//{
  //Return FALSE to destroy the window on a "delete_event"
  //return FALSE;
//}

//void COgreContext::destroy_cb(GtkWidget *widget, gpointer data)
//{
  //gtk_main_quit();
//}

Ogre::Camera* COgreContext::createCamera(int index, GtkOgre* ogre)
{
	char buf[MAX_PATH];
	sprintf(buf, "Camera%d", index);

	Camera* l_pCamera = m_pSceneManager->createCamera(buf);
  l_pCamera->setAutoAspectRatio(true);
  
	if(index == 0)
	{ 		
		ogre->mCamType = CAM_PERSPECTIVE;
		l_pCamera->setPosition(146.0, 248.0, 440.0);
		l_pCamera->lookAt(-10.0, 20.0, 5.0);
		l_pCamera->setNearClipDistance(5.0);
		l_pCamera->setFarClipDistance(5000.0);	
	}
	else if(index == 1)
	{
		ogre->mCamType = CAM_ORTHO_X;
		l_pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		l_pCamera->setFixedYawAxis(false);
		l_pCamera->setPosition(-ORTHOGONAL_CAMERA_DISTANCE, 150, 150);
		l_pCamera->setDirection(Ogre::Vector3::UNIT_X);
		l_pCamera->setNearClipDistance(ORTHOGONAL_CAMERA_NEAR_CLIP);
		l_pCamera->setFarClipDistance(ORTHOGONAL_CAMERA_FAR_CLIP);
		l_pCamera->setPolygonMode(Ogre::PM_WIREFRAME);
	}
	else if(index == 2)
	{
		ogre->mCamType = CAM_ORTHO_Z;
		l_pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		l_pCamera->setFixedYawAxis(false);
		l_pCamera->setPosition(80, 150, ORTHOGONAL_CAMERA_DISTANCE);
		l_pCamera->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Z);
		l_pCamera->setNearClipDistance(ORTHOGONAL_CAMERA_NEAR_CLIP);
		l_pCamera->setFarClipDistance(ORTHOGONAL_CAMERA_FAR_CLIP);
		l_pCamera->setPolygonMode(Ogre::PM_WIREFRAME);
	}
	else
	{
		ogre->mCamType = CAM_ORTHO_Y;
		l_pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		l_pCamera->setFixedYawAxis(false);
		l_pCamera->setPosition(90, ORTHOGONAL_CAMERA_DISTANCE, 150);
		l_pCamera->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);
		l_pCamera->setNearClipDistance(ORTHOGONAL_CAMERA_NEAR_CLIP);
		l_pCamera->setFarClipDistance(ORTHOGONAL_CAMERA_FAR_CLIP);
		l_pCamera->setPolygonMode(Ogre::PM_WIREFRAME);
	}

	return l_pCamera;
}*/
