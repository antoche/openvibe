#ifndef __OpenViBE_Kernel_Visualisation_IVisualisationTree_H__
#define __OpenViBE_Kernel_Visualisation_IVisualisationTree_H__

#include "../ovIKernelObject.h"

#include "ovIVisualisationWidget.h"

typedef struct _GtkWidget   GtkWidget;
typedef struct _GtkTreeView GtkTreeView;
typedef struct _GtkTreeIter GtkTreeIter;
typedef struct _GtkTreePath GtkTreePath;

namespace OpenViBE
{
	namespace Kernel
	{
		/// Drag locations around a widget 
		enum EDragDataLocation
		{
			EDragData_Left,
			EDragData_Right,
			EDragData_Top,
			EDragData_Bottom
		};

		/// Column types in a visualisation tree
		enum EVisualisationTreeColumn
		{
			/**
			 * \brief Node name
			 *
			 * Depending on the node type, it refers to :
			 * the name given to the window for EVisualisationTreeNode_VisualisationWindow nodes,
			 * the name of the notebook tab for EVisualisationTreeNode_VisualisationPanel nodes,
			 * the name of the related IBox for EVisualisationTreeNode_VisualisationBox nodes,
			 * "Vertical split" for EVisualisationTreeNode_VerticalSplit nodes,
			 * "Horizontal split" for EVisualisationTreeNode_HorizontalSplit nodes,
			 * "Unaffected display plugins" for the EVisualisationTreeNode_Unaffected node
			 */
			EVisualisationTreeColumn_StringName,
			/**
			 * \brief Icon associated to a node
			 *
			 * Constant except for type EVisualisationTreeNode_VisualisationBox, for which
			 * it can be specified in the plugin descriptor
			 */
			EVisualisationTreeColumn_StringStockIcon,
			/**
			 * \brief EVisualisationTreeNode type
			 */
			EVisualisationTreeColumn_ULongNodeType,
			/**
			 * \brief IVisualisationWidget identifier
			 */
			EVisualisationTreeColumn_StringIdentifier,
			/**
			 * \brief Pointer to the GtkWidget associated to the node (if any)
			 */
			EVisualisationTreeColumn_PointerWidget
		};

		/// Node types in a visualisation tree
		enum EVisualisationTreeNode
		{
			EVisualisationTreeNode_Undefined, /**< May be used for placeholders */
			EVisualisationTreeNode_VPU, /**< Not used yet */
			EVisualisationTreeNode_VisualisationWindow, /**< A top level window */
			EVisualisationTreeNode_VisualisationPanel, /**< A notebook tab inside a window */
			EVisualisationTreeNode_VisualisationBox, /**< A visualisation box (a plugin which displays something)*/
			EVisualisationTreeNode_VerticalSplit, /**< A vertical split widget */
			EVisualisationTreeNode_HorizontalSplit, /**< A horizontal split widget */
			EVisualisationTreeNode_Unaffected /**< A special node parent to all unaffected visualisation boxes */
		};

		class IVisualisationWidget;
		class IVisualisationTree;

		/**
		\brief Tree view interface

		It must be implemented by classes which communicate with the IVisualisationTree class in order
		to display the contents of the visualisation tree.
		*/
		class OV_API ITreeViewCB
		{
		public:

			virtual ~ITreeViewCB(void) { }
			/** Notifies the tree view that a new widget is being created 
			\param pVisualisationWidget [in] pointer to the newly created widget
			*/
			virtual void              createTreeWidget      (OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget)                            { };
			///Notifies the tree view that a widget is being loaded. 
			/** This method must return a Gtk widget that matches the description passed in parameter, or NULL if 
			it doesn't wish to support a specific kind of widget (e.g. top level windows are not instantiated by the
			offline window manager). The widget pointer returned is stored in the EVisualisationTreeColumn_PointerWidget
			column of the corresponding tree node.
			\param pVisualisationWidget [in] pointer to loaded visualisation widget
			\return GtkWidget* pointer to Gtk widget associated to visualisation widget (possibly NULL)
			*/
			virtual ::GtkWidget*      loadTreeWidget        (OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget)                            { return NULL; }
			///Notifies the tree view that a widget hierarchy has been loaded
			/** This method lets the tree view perform additional loading operations at the end of the loading
			process 
			\param pTreeWidget [in] pointer to the GtkWidget whose loading is finished
			*/
			virtual void              endLoadTreeWidget     (::GtkWidget* pTreeWidget)                                                                { }
			///Asks for the visualisation (visible) widget associated to the tree widget passed in parameter
			/**
			In some cases the Gtk 'tree widget' associated to a tree node is a table comprising several widgets. 
			For example, in the offline window manager, a widget is often stored in the central cell of a 3x3 table and is surrounded 
			by transparent event boxes for drag n' drop purposes. 
			This callback returns the central 'visualisation widget' (the visible widget) contained in such a table if any, otherwise 
			it returns the tree widget itself.
			\param pTreeWidget [in] pointer to the Gtk widget associated to a tree node (as stored in the EVisualisationTreeColumn_PointerWidget
			column of a tree store)
			\return pointer to the visible Gtk widget if pTreeWidget is a table, pTreeWidget itself otherwise(default)
			\sa getTreeWidget()
			*/
			virtual ::GtkWidget*      getVisualisationWidget(::GtkWidget* pTreeWidget)                                                                { return pTreeWidget; }
			///Asks for the tree widget associated to a visualisation widget
			/**
			In some cases a Gtk widget is inserted in a 3x3 table for drag n' drop purposes. In such cases, a distinction is
			made between the 'visualisation widget' (the actual visible widget) and the widget stored in the EVisualisationTreeColumn_PointerWidget
			column of a tree store.
			\param pVisualisationWidget [in] pointer to visualisation widget
			\return parent table if any, pVisualisationWidget otherwise (default)
			\sa getVisualisationWidget()
			*/
			virtual ::GtkWidget*      getTreeWidget         (::GtkWidget* pVisualisationWidget)                                                       { return pVisualisationWidget; }
			///Icon associated to a visualisation tree node
			/**			
			Each node has a Gtk icon. It is defined by the tree view, but can be customised for nodes of type EVisualisationTreeNode_VisualisationBox. 
			This method asks the tree view what icon is to be used for a given type of node.
			\param eVisualisationTreeNode [in] type of node whose icon name is to be retrieved
			\return name of stock icon to be associated to the type of node passed in parameter
			*/
			virtual const char*       getTreeWidgetIcon     (OpenViBE::Kernel::EVisualisationTreeNode eVisualisationTreeNode)                         { return ""; }
			///Notifies the tree view that a paned widget is being resized
			/**
			When a window is resized, the new size information is being forwarded to the whole widgets hierarchy. In the case of 
			paned widgets (vertical and horizontal splits), this information is forwarded to the tree view (which might
			want to save the paned widget's new size, and/or reposition the paned handle).
			\param pVisualisationWidget [in] pointer to visualisation widget of type EVisualisationTreeNode_VerticalSplit or EVisualisationTreeNode_HorizontalSplit that is being resized
			\param pPanedWidget [in] pointer to associated Gtk widget			
			*/
			virtual void              resizePanedWidget     (OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget, ::GtkWidget* pPanedWidget) { }
			///Notifies tree view that a visualisation box is being instantiated in online mode
			/**
			Forwards pointers to the main window and the toolbar of a visualisation box as the player is launched and each visualisation
			box contained in the active scenario is being instantiated. Used to position each box at the appropriate position, as defined
		  using the offline window manager.
			\param rName [in] name (as stored in EVisualisationTreeColumn_StringName column of tree store) of visualisation box whose widgets are to be sent to tree view
			\param pWidget [in] pointer to main window of visualisation box
			\param pToolbarWidget [in] pointer to toolbar of visualisation box
			\return true if widgets were successfully registered, false otherwise
			*/
			virtual OpenViBE::boolean setWidgets            (const OpenViBE::CString& rName, ::GtkWidget* pWidget, ::GtkWidget* pToolbarWidget)       { return false; }
		};

		/**
		 \class IVisualisationTree
		 \author Vincent Delannoy (INRIA/IRISA)
		 \date 2007-11
		 \brief Handles visualisation widgets and their arrangement in space
		 
		 This interface lets applications configure visualisation widgets by adding them to windows and 
		 tabs, positioning them next to each other using paned widgets, and resizing them as the user 
		 sees fit. All widgets are saved in a symbolic way as IVisualisationWidget instances referenced in a tree store. 
		 Methods of this class are essentially meant to be called from an external class inheriting from the ITreeViewCB 
		 interface, for offline design or online use of the widgets.
		 */
		class OV_API IVisualisationTree : public OpenViBE::Kernel::IAttributable
		{
		public:

			///Initializes the visualisation tree
			/**
			This method registers the scenario associated to this tree and creates a tree store.
			\param pScenario [in] scenario associated to this tree store
			\return true if tree was successfully initialized, false otherwise
			*/
			virtual OpenViBE::boolean init(
				const OpenViBE::Kernel::IScenario* pScenario)=0;

			/** \name IVisualisationWidget management */
			//@{

			///Iterates through IVisualisationWidget instances managed by this tree
			/**
			Upon first call, this method should be passed an identifier initialized with OV_Undefined.
			It is modified at each subsequent call until all widgets have been returned (in which 
			case the identifier is reset to OV_Undefined)
			\param rIdentifier [in/out] identifier of current visualisation widget when calling the method and of next widget upon return
			\return true if a widget was found, false if past beyond last one
			*/
			virtual OpenViBE::boolean getNextVisualisationWidgetIdentifier(
				OpenViBE::CIdentifier& rIdentifier) const=0;
			///Iterates through IVisualisationWidget instances of type oType managed by this tree
			/**
			\param rIdentifier [in/out] identifier of current visualisation widget when calling the method and of next widget upon return
			\param oType [in] restricts search to IVisualisationWidget instances of type oType
			\return true if a widget was found, false if past beyond last one
			\sa getNextVisualisationWidgetIdentifier()
			*/
			virtual OpenViBE::boolean getNextVisualisationWidgetIdentifier(
				OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::Kernel::EVisualisationWidgetType oType) const=0;
			///Tests whether an identifier corresponds to an IVisualisationWidget instance
			/**
			This method browses the internal tree store to look for a node whose EVisualisationTreeColumn_StringIdentifier field
			equals 'rIdentifier' once converted to CString format.
			\param rIdentifier [in] identifier to look for in the internal tree store
			\return true if rIdentifier corresponds to an IVisualisationWidget instance stored in this tree, false otherwise
			*/
			virtual OpenViBE::boolean	isVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier) const=0;
			///Returns the IVisualisationWidget instance whose identifier is passed in parameter
			/**
			\param rIdentifier [in] identifier to look for in the internal tree store
			\return pointer to IVisualisationWidget whose identifier matches 'rIdentifier' if any, NULL otherwise
			*/
			virtual OpenViBE::Kernel::IVisualisationWidget* getVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier) const=0;
			///Returns the IVisualisationWidget instance whose corresponding IBox identifier matches 'rBoxIdentifier'
			/**
			This method inherently restricts the search to nodes of type EVisualisationTreeNode_VisualisationBox and
			since only these are associated to an IBox instance.
			\param rBoxIdentifier [in] identifier of IBox whose associated IVisualisationWidget is to be retrieved
			\return IVisualisationWidget whose associated IBox identifier matches 'rBoxIdentifier' if any, NULL otherwise
			*/
			virtual OpenViBE::Kernel::IVisualisationWidget* getVisualisationWidgetFromBoxIdentifier(
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;
			///Adds an IVisualisationWidget instance to the tree
			/**
			Seven parameters describe the IVisualisationWidget to be created. Not all are relevant for every type of widget.
			\param rIdentifier [in] identifier of the widget to be created
			\param rName [in] name of the widget
			\param oType [in] type of the widget
			\param rParentIdentifier [in] parent widget identifier (OV_Undefined for top-level widgets)
			\param ui32ParentIndex [in] index where this widget is to be parented (irrelevant for top-level widgets)
			\param rBoxIdentifier [in] identifier of associated IBox (for widgets of type EVisualisationWidget_VisualisationBox only)
			\param ui32NbChildren [in] number of children of this widget (none for a visualisation box, 1 for a visualisation panel, 2 for split widgets, variable number for windows)
			\return true if widget successfully added to the internal tree store, false otherwise
			*/
			virtual OpenViBE::boolean	addVisualisationWidget(
				OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rName,
				OpenViBE::Kernel::EVisualisationWidgetType oType,
				const OpenViBE::CIdentifier& rParentIdentifier,
				OpenViBE::uint32 ui32ParentIndex,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				OpenViBE::uint32 ui32NbChildren)=0;
			///Returns the index where a widget is parented
			/**
			Irrelevant for top level windows.
			\param rIdentifier [in] identifier of the IVisualisationWidget whose index is to be retrieved
			\param ui32Index [out] index where the widget is parented
			\return true if widget index could be determined, false otherwise
			*/
			virtual OpenViBE::boolean	getVisualisationWidgetIndex(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32Index) const=0;
			///Unparents a widget from its parent, if any
			/**
			\param rIdentifier [in] identifier of widget to be unparented
			\param ui32Index [out] index where this widget was parented
			\return true if widget could be removed from its parent, false otherwise
			*/
			virtual OpenViBE::boolean	unparentVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32Index)=0;
			///Parents a widget to a parent widget
			/**
			\param rIdentifier [in] identifier of widget to be parented
			\param rParentIdentifier [in] identifier of parent widget 
			\param ui32Index [in] index where widget is to be parented
			\return true if widget could be parented as desired, false otherwise
			*/
			virtual OpenViBE::boolean	parentVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CIdentifier& rParentIdentifier,
				OpenViBE::uint32 ui32Index)=0;
			///Destroys a widget hierarchy
			/**
			\param rIdentifier [in] identifier of widget that is to be destroyed, along with all widgets in its subtree
			\param bDestroyVisualisationBoxes [in] if false, widgets of type EVisualisationTreeNode_VisualisationBox are unaffected only (as opposed to destroyed)
			\return true if hierarchy was successfully destroyed, false otherwise
			*/
			virtual OpenViBE::boolean	destroyHierarchy(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::boolean bDestroyVisualisationBoxes=true)=0;
			//@}

			/** \name Tree view creation/registration */
			//@{			

			/**
			Creates a tree view from the internal tree store/model
			\return pointer to newly created tree view
			*/
			virtual ::GtkTreeView* createTreeViewWithModel(void)=0;
			/**
			Sets the instance implementing the ITreeViewCB interface that is to be used with the visualisation tree
			\param pTreeViewCB [in] pointer to an implementation of the ITreeViewCB interface
			\return true if tree view was successfully registered, false otherwise
			\sa ITreeViewCB
			*/
			virtual OpenViBE::boolean	setTreeViewCB(
				OpenViBE::Kernel::ITreeViewCB* pTreeViewCB)=0;
			//@}

			///Recreates Gtk widgets based on the widgets description stored in the internal tree store.
			/**To be called upon widget creation, deletion or repositioning. Side effects : all identifiers
			and Gtk widget pointers are invalidated and replaced with new values.
			\return true if widgets were successfully reloaded, false otherwise
			*/
			virtual OpenViBE::boolean reloadTree(void)=0;
			
			///Resizes a visualisation panel and its hierarchy
			//FIXME : is this still needed?
			/**Called internally upon tree reloading (for each visualisation panel). 
			Should be called externally whenever a dialog is resized (for the active visualisation panel only)
			\param pVisualisationPanelIter [in] pointer to a node of type EVisualisationTreeNode_VisualisationPanel when its 
			associated widget is resized.
			\return true if panel was successfully resized, false otherwise
			*/
			virtual OpenViBE::boolean resizeVisualisationPanel(
				::GtkTreeIter* pVisualisationPanelIter)=0;

			/** \name Helper functions */
			//@{			

			///Returns currently selected node
			/**
			\param pTreeView [in] pointer to tree view whose selected node is to be retrieved
			\param pIter [out] pointer to selected node 
			\return true if a node is selected, false otherwise
			*/
			virtual OpenViBE::boolean getTreeSelection(
				::GtkTreeView* pTreeView,
				::GtkTreeIter* pIter)=0;
			///Returns tree path of a given node
			/**
			\param pTreeIter [in] pointer to node whose path is to be retrieved
			\return pointer to path of node
			*/
			virtual GtkTreePath* getTreePath(
				::GtkTreeIter* pTreeIter) const=0;
			///Returns unsigned long value stored in the 'eVisualisationTreeColumn' column of node 'pTreeIter'
			/**
			\param pTreeIter [in] pointer to node
			\param eVisualisationTreeColumn [in] index of column where unsigned long value is stored
			\return unsigned long value retrieved at the specified column of the specified node, if any, 0 otherwise.
			*/
			virtual unsigned long getULongValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const=0;
			///Returns string stored in the 'eVisualisationTreeColumn' column of node 'pTreeIter'
			/**
			\param pTreeIter [in] pointer to node
			\param rString [out] string to be retrieved
			\param eVisualisationTreeColumn [in] index of column where string is stored
			\return true if string was successfully retrieved at the specified column of the specified node, 0 otherwise.
			*/
			virtual OpenViBE::boolean getStringValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				char*& rString,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const=0;
			///Returns pointer stored in the 'eVisualisationTreeColumn' column of node 'pTreeIter'
			/**
			\param pTreeIter [in] pointer to node
			\param rPointer [out] pointer to be retrieved
			\param eVisualisationTreeColumn [in] index of column where pointer is stored
			\return true if pointer was successfully retrieved at the specified column of the specified node, 0 otherwise.
			*/
			virtual OpenViBE::boolean	getPointerValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				void*& rPointer,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const=0;
			///Returns identifier stored in the 'eVisualisationTreeColumn' column of node 'pTreeIter'
			/**
			\param pTreeIter [in] pointer to node
			\param rIdentifier [out] identifier to be retrieved
			\param eVisualisationTreeColumn [in] index of column where identifier is stored
			\return true if identifier was successfully retrieved at the specified column of the specified node, 0 otherwise.
			*/
			virtual OpenViBE::boolean getIdentifierFromTreeIter(
				::GtkTreeIter* pTreeIter,
				OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const=0;
			//@}

			/** \name Search functions */
			//@{			

			///Looks for a node of name 'label' and type 'eType' from the root of the internal tree store
			/**
			\param pIter [out] pointer to node to be retrieved
			\param pLabel [in] label of node to be retrieved, as stored in the EVisualisationTreeColumn_StringName column
			\param eType [in] type of node to be retrieved, as stored in the EVisualisationTreeColumn_ULongNodeType column
			\return true if node was found, false otherwise
			*/
			virtual OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				const char* pLabel,
				OpenViBE::Kernel::EVisualisationTreeNode eType)=0;
			///Looks for a node of name 'label' and type 'eType' from a given node in the internal tree store
			/**
			\param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			\param pLabel [in] label of node to be retrieved, as stored in the EVisualisationTreeColumn_StringName column
			\param eType [in] type of node to be retrieved, as stored in the EVisualisationTreeColumn_ULongNodeType column
			\return true if node was found, false otherwise
			*/
			virtual OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				const char* pLabel,
				OpenViBE::Kernel::EVisualisationTreeNode eType)=0;

			///Looks for a node whose associated Gtk widget matches 'pWidget' from the root of the internal tree store
			/**
			\param pIter [out] pointer to node to be retrieved
			\param pWidget [in] pointer to Gtk widget of node to be retrieved, as stored in the EVisualisationTreeColumn_PointerWidget column
			\return true if node was found, false otherwise
			*/			
			virtual OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				void* pWidget)=0;
			///Looks for a node whose associated Gtk widget matches 'pWidget' from a given node in the internal tree store
			/**
			\param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			\param pWidget [in] pointer to Gtk widget of node to be retrieved, as stored in the EVisualisationTreeColumn_PointerWidget column
			\return true if node was found, false otherwise
			*/
			virtual OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				void* pWidget)=0;

			///Looks for a node whose identifier matches 'oIdentifier' from the root of the internal tree store
			/**
			\param pIter [out] pointer to node to be retrieved
			\param oIdentifier [in] identifier of node to be retrieved, as stored in the EVisualisationTreeColumn_StringIdentifier column
			\return true if node was found, false otherwise
			*/
			virtual OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier)=0;
			///Looks for a node whose identifier matches 'oIdentifier' from a given node in the internal tree store
			/**
			\param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			\param oIdentifier [in] identifier of node to be retrieved, as stored in the EVisualisationTreeColumn_StringIdentifier column
			\return true if node was found, false otherwise
			*/
			virtual OpenViBE::boolean	findChildNodeFromParent(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier)=0;

			//find first parent node of a given type
			///Looks for first parent node of type 'eType' from a given node in the internal tree store
			/**
			\param pIter [in/out] pointer to node from which to start searching when calling function and to node found upon return
			\param eType [in] type of parent node looked for, as stored in the EVisualisationTreeColumn_ULongNodeType column
			\return true if node was found, false otherwise
			*/
			virtual OpenViBE::boolean	findParentNode(
				::GtkTreeIter* pIter,
				OpenViBE::Kernel::EVisualisationTreeNode eType)=0;

			//@}
			
			/** \name Drag n' drop functions */
			//@{			

			///Handles drop of a widget in an existing widget
			/**
			This operation replaces the existing widget with the one passed in parameter. The existing widget
			is unaffected if it is of type EVisualisationTreeNode_VisualisationBox, or destroyed if it is of 
			type EVisualisationTreeNode_Undefined (placeholder widget).
			\param rSrcIdentifier [in] identifier of widget being dropped
			\param pDstWidget [in] pointer to widget on which the drop operation is performed
			\return true if drop operation was successfully completed, false otherwise
			*/
			virtual OpenViBE::boolean dragDataReceivedInWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget)=0;
			///Handles drop of a widget in an event box
			/**
			This operation adds a widget to the tree and changes the tree structure.
			If a widget is moved from one place to the other in the tree, it is automatically 
			simplified to avoid placeholders creation.
			\param rSrcIdentifier [in] identifier of widget being dropped
			\param pDstWidget [in] pointer to widget on which the drop operation is performed
			\return true if drop operation was successfully completed, false otherwise
			*/
			virtual OpenViBE::boolean dragDataReceivedOutsideWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget,
				OpenViBE::Kernel::EDragDataLocation eLocation)=0;

			//@}

			///Forwards pointers to the Gtk widgets of the main window and toolbar of a widget to the tree view
			/**
			\param rVisualisationBoxName [in] name of visualisation box whose widgets are passed in parameter
			\param pWidget [in] pointer to main window
			\param pToolbarWidget [in] pointer to toolbar
			\return \e true in case of success, \e false otherwise.
			*/
			virtual OpenViBE::boolean setWidgets(
				const OpenViBE::CString& rVisualisationBoxName,
				::GtkWidget* pWidget,
				::GtkWidget* pToolbarWidget)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Visualisation_VisualisationTree)
		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_IVisualisationTree_H__
