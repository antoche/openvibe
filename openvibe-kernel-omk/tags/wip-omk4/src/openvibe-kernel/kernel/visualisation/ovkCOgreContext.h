/*
#ifndef __OgreContext_H__
#define __OgreContext_H__

#include "../ovkTKernelObject.h"
#include "ovkCOgreWidget.h"

#include <vector>


namespace Ogre
{
	class SceneManager;
	class Root;
	class Camera;
	class Viewport;
	class SceneNode;
};

namespace OpenViBE
{
	namespace Kernel
	{
		class COgreContext
		{
		public:
			COgreContext();	
			virtual ~COgreContext();
			
			void init();
			void release();

			OpenViBE::boolean newOgreWidget(void* pParenGtkWidget, OpenViBE::uint32 ui32Index);
			
			OpenViBE::boolean realizeOgreWidgets();
			
			//static gboolean delete_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data);
			//static void destroy_cb(GtkWidget *widget, gpointer data);
			
		protected:			
			//to be called once at least one RenderWindow has been created
			Ogre::SceneNode* initScene();
			Ogre::Camera* createCamera(int index, GtkOgre*);

			OpenViBE::boolean m_bSceneManagerCreated;
			Ogre::SceneManager* m_pSceneManager;
			Ogre::Root* m_pRoot;

			std::vector<GtkOgre*> m_vGtkOgre;			
		};
	};
};

#endif // __OgreApplication_H__
*/