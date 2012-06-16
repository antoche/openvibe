#ifndef __OpenViBEKernel_Kernel_Visualisation_CVisualisationTree_H__
#define __OpenViBEKernel_Kernel_Visualisation_CVisualisationTree_H__

#include "../ovkTKernelObject.h"

#include "../scenario/ovkTAttributable.h"

#include <map>
#include <gtk/gtk.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationTree : public OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IVisualisationTree > >
		{
		public:
			CVisualisationTree(
				const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CVisualisationTree(void);

			OpenViBE::boolean init(
				const OpenViBE::Kernel::IScenario* pScenario);

			/** \name VisualisationWidget management */
			//@{
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
			//@}

			/**
			 * \brief Creates a tree view from the internal tree store/model
			 * \return pointer to newly created tree view
			 */
			::GtkTreeView* createTreeViewWithModel(void);
			OpenViBE::boolean setTreeViewCB(
				OpenViBE::Kernel::ITreeViewCB* pTreeViewCB);

			/**
			 * \brief Recreates Gtk widgets based on the widgets description stored in the internal tree store.
			 * \remarks Side effects : all iterators and GtkWidget pointers are invalidated and replaced with new values
			 * To be called upon widget creation, deletion or repositioning.
			 * \return true if widgets were successfully reloaded, false otherwise
			 */
			OpenViBE::boolean reloadTree(void);

			/** \name Helper functions */
			//@{
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
			//@}

			/** \name Node searching */
			//@{
			/**
			 * \brief Looks for a node of name 'label' and type 'eType' from the root of the internal tree store
			 * \param pIter [out] pointer to node to be retrieved
			 * \param pLabel [in] label of node to be retrieved, as stored in the EVisualisationTreeColumn_StringName column
			 * \param eType [in] type of node to be retrieved, as stored in the EVisualisationTreeColumn_ULongNodeType column
			 * \return true if node was found, false otherwise
			 */
			OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				const char* label,
				OpenViBE::Kernel::EVisualisationTreeNode rType);
			/**
			 * \brief Looks for a node of name 'label' and type 'eType' from a given node in the internal tree store
			 * \param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			 * \param pLabel [in] label of node to be retrieved, as stored in the EVisualisationTreeColumn_StringName column
			 * \param eType [in] type of node to be retrieved, as stored in the EVisualisationTreeColumn_ULongNodeType column
			 * \return true if node was found, false otherwise
			 */
			OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				const char* label,
				OpenViBE::Kernel::EVisualisationTreeNode rType);
			/**
			 * \brief Looks for a node whose associated Gtk widget matches 'pWidget' from the root of the internal tree store
			 * \param pIter [out] pointer to node to be retrieved
			 * \param pWidget [in] pointer to Gtk widget of node to be retrieved, as stored in the EVisualisationTreeColumn_PointerWidget column
			 * \return true if node was found, false otherwise
			 */
			OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				void* pWidget);
			/**
			 * \brief Looks for a node whose associated Gtk widget matches 'pWidget' from a given node in the internal tree store
			 * \param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			 * \param pWidget [in] pointer to Gtk widget of node to be retrieved, as stored in the EVisualisationTreeColumn_PointerWidget column
			 * \return true if node was found, false otherwise
			 */
			OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				void* pWidget);
			/**
			 * \brief Looks for a node whose identifier matches 'oIdentifier' from the root of the internal tree store
			 * \param pIter [out] pointer to node to be retrieved
			 * \param oIdentifier [in] identifier of node to be retrieved, as stored in the EVisualisationTreeColumn_StringIdentifier column
			 * \return true if node was found, false otherwise
			 */
			OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier);
			/**
			 * \brief Looks for a node whose identifier matches 'oIdentifier' from a given node in the internal tree store
			 * \param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			 * \param oIdentifier [in] identifier of node to be retrieved, as stored in the EVisualisationTreeColumn_StringIdentifier column
			 * \return true if node was found, false otherwise
			 */
			OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier);
			/**
			 * \brief Find first parent node of a given type
			 * Looks for first parent node of type 'eType' from a given node in the internal tree store
			 * \param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			 * \param eType [in] type of parent node looked for, as stored in the EVisualisationTreeColumn_ULongNodeType column
			 * \return true if node was found, false otherwise
			 */
			OpenViBE::boolean findParentNode(
				::GtkTreeIter* pIter,
				OpenViBE::Kernel::EVisualisationTreeNode rType);
			//@}

			/** \name Drag n drop functions */
			//@{

			/**
			 * \brief Handles drop of a widget in an existing widget
			 * This operation replaces the existing widget with the one passed in parameter. The existing widget
			 * is unaffected if it is of type EVisualisationTreeNode_VisualisationBox, or destroyed if it is of
			 * type EVisualisationTreeNode_Undefined (placeholder widget).
			 * \param rSrcIdentifier [in] identifier of widget being dropped
			 * \param pDstWidget [in] pointer to widget on which the drop operation is performed
			 * \return true if drop operation was successfully completed, false otherwise
			 */
			OpenViBE::boolean dragDataReceivedInWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget);

			/**
			 * \brief Handles drop of a widget in an event box
			 * This operation adds a widget to the tree and changes the tree structure.
			 * If a widget is moved from one place to the other in the tree, it is automatically
			 * simplified to avoid placeholders creation.
			 * \param rSrcIdentifier [in] identifier of widget being dropped
			 * \param pDstWidget [in] pointer to widget on which the drop operation is performed
			 * \return true if drop operation was successfully completed, false otherwise
			 */
			OpenViBE::boolean dragDataReceivedOutsideWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget,
				OpenViBE::Kernel::EDragDataLocation eLocation);
			//@}

			/**
			 * \name Dynamic widget parenting
			 */
			//@{

			/**
			 * \brief Forward pointer to the toolbar of a visualisation plugin (if any) to the tree view
			 * \param rBoxIdentifier [in] Identifier of IBox whose toolbar is being set
			 * \param pToolbarWidget [in] pointer to toolbar
			 * \return \e true in case of success, \e false otherwise.
			 */
			virtual OpenViBE::boolean setToolbar(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				::GtkWidget* pToolbarWidget);

			/**
			 * \brief Forward pointer to the main widget of a visualisation plugin to the tree view
			 * \param rBoxIdentifier [in] Identifier of IBox whose topmost widget is being set
			 * \param pWidget [in] pointer to main window
			 * \return \e true in case of success, \e false otherwise.
			 */
			virtual OpenViBE::boolean setWidget(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				::GtkWidget* pTopmostWidget);

			//@}

			virtual OpenViBE::boolean acceptVisitor(
				OpenViBE::IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IVisualisationTree > >, OVK_ClassId_Kernel_Visualisation_VisualisationTree)

		private:

			OpenViBE::boolean _destroyHierarchy(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::boolean bDestroyVisualisationBoxes);
			OpenViBE::CIdentifier getUnusedIdentifier(void) const;

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
			OpenViBE::boolean loadVisualisationWidget(
				OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget,
				::GtkTreeIter* pParentIter);

			OpenViBE::boolean visitVisualisationWidget(
				OpenViBE::IObjectVisitor& rObjectVisitor,
				OpenViBE::CIdentifier oVisualisationWidgetIdentifier,
				OpenViBE::boolean bRecurse);

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
