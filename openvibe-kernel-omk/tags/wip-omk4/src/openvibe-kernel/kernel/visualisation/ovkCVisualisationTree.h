#ifndef __OpenViBEKernel_Kernel_Visualisation_CVisualisationTree_H__
#define __OpenViBEKernel_Kernel_Visualisation_CVisualisationTree_H__

#include "../ovkTKernelObject.h"
#include "../scenario/ovkTAttributable.h"

#include <string>
#include <vector>
#include <map>
#include <gtk/gtk.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationTree : virtual public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::IVisualisationTree> >
		{
		public:
			CVisualisationTree(
				const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CVisualisationTree(void);

			OpenViBE::boolean	init(
				const OpenViBE::Kernel::IScenario* pScenario);
			//VisualisationWidget management
			OpenViBE::boolean getNextVisualisationWidgetIdentifier(
				OpenViBE::CIdentifier& rIdentifier) const;
			OpenViBE::boolean getNextVisualisationWidgetIdentifier(
				OpenViBE::CIdentifier& rIdentifier,
				EVisualisationWidgetType oType) const;
			OpenViBE::boolean isVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier) const;
			OpenViBE::Kernel::IVisualisationWidget* getVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier) const;
			OpenViBE::Kernel::IVisualisationWidget* getVisualisationWidgetFromBoxIdentifier(
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			OpenViBE::boolean addVisualisationWidget(
				OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rName,
				OpenViBE::Kernel::EVisualisationWidgetType oType,
				const OpenViBE::CIdentifier& rParentIdentifier,
				OpenViBE::uint32 ui32ParentIndex,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				OpenViBE::uint32 ui32NbChildren);
			OpenViBE::boolean getVisualisationWidgetIndex(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& rIndex) const;
			OpenViBE::boolean unparentVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32Index);
			OpenViBE::boolean parentVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CIdentifier& rParentIdentifier,
				OpenViBE::uint32 ui32Index);
			OpenViBE::boolean destroyHierarchy(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::boolean bDestroyVisualisationBoxes);

			//tree view creation/registration
			::GtkTreeView* createTreeViewWithModel(void);
			OpenViBE::boolean setTreeViewCB(
				OpenViBE::Kernel::ITreeViewCB* pTreeViewCB);

			//to be called whenever tree model is changed
			//side effects : all iterators and GtkWidget pointers are invalidated and replaced with new values
			OpenViBE::boolean reloadTree(void);

			//called upon tree reloading (for each visualisation panel)
			//and whenever dialog is resized (for active visualisation panel only)
			OpenViBE::boolean resizeVisualisationPanel(
				::GtkTreeIter* pVisualisationPanelIter);

			//helper functions
			OpenViBE::boolean getTreeSelection(
				::GtkTreeView* pTreeView,
				::GtkTreeIter* pIter);
			::GtkTreePath* getTreePath(
				::GtkTreeIter* pTreeIter) const;
			unsigned long getULongValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const;
			OpenViBE::boolean getStringValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				char*& rString,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const;
			OpenViBE::boolean getPointerValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				void*& rPointer,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const;
			OpenViBE::boolean getIdentifierFromTreeIter(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const;

			//find a node by its label and type
			OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				const char* label,
				OpenViBE::Kernel::EVisualisationTreeNode rType);
			OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				const char* label,
				OpenViBE::Kernel::EVisualisationTreeNode rType);

			//find a node by its pointer to a Gtk widget
			OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				void* pWidget);
			OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				void* pWidget);

			//find a node by its identifier
			OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier);
			OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier);

			//find first parent node of a given type
			OpenViBE::boolean findParentNode(
				::GtkTreeIter* pIter,
				OpenViBE::Kernel::EVisualisationTreeNode rType);

			//drag n drop
			OpenViBE::boolean dragDataReceivedInWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget);
			OpenViBE::boolean dragDataReceivedOutsideWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget,
				OpenViBE::Kernel::EDragDataLocation eLocation);

			//dynamic widget parenting
			OpenViBE::boolean setWidgets(
				const OpenViBE::CString& rVisualisationBoxName,
				::GtkWidget* pWidget,
				::GtkWidget* pToolbarWidget);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::IVisualisationTree> >, OVK_ClassId_Kernel_Visualisation_VisualisationTree)

		private:

			OpenViBE::boolean _destroyHierarchy(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::boolean bDestroyVisualisationBoxes);
			OpenViBE::CIdentifier getUnusedIdentifier(void) const;

			OpenViBE::boolean createTreeWidget(
				OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget);
			::GtkWidget* loadTreeWidget(
				OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget);
			::GtkWidget* getVisualisationWidget(
				::GtkWidget* pTreeWidget);
			::GtkWidget* getTreeWidget(
				::GtkWidget* pVisualisationWidget);
			const char* getTreeWidgetIcon(
				OpenViBE::Kernel::EVisualisationTreeNode);
			OpenViBE::boolean resizePanedWidget(
				OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget,
				::GtkWidget* pPanedWidget);

			OpenViBE::boolean _findChildNodeFromParent(
				::GtkTreeIter* pIter,
				const char* label,
				OpenViBE::Kernel::EVisualisationTreeNode rType);
			OpenViBE::boolean _findChildNodeFromParent(
				::GtkTreeIter* pIter,
				void* pWidget);
			OpenViBE::boolean _findChildNodeFromParent(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier);

			//tree loading
			OpenViBE::boolean loadVisualisationWindow(
				OpenViBE::Kernel::IVisualisationWidget* pVisualisationWindow);
			OpenViBE::boolean loadVisualisationPanel(
				::GtkTreeIter* pParentIter,
				::GtkWidget*& pNotebook,
				OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget);
			OpenViBE::boolean loadVisualisationWidget(
				::GtkTreeIter* pParentIter,
				OpenViBE::uint32 ui32Index,
				OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget);

		private:

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::IVisualisationWidget*> m_vVisualisationWidget;
			OpenViBE::CIdentifier m_rScenarioIdentifier;
			const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
			const OpenViBE::Kernel::IScenario* m_pScenario;
			::GtkTreeStore* m_pTreeStore;
			::GtkTreeIter m_oInternalTreeNode;
			OpenViBE::Kernel::ITreeViewCB* m_pTreeViewCB;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Visualisation_CVisualisationTree_H__
