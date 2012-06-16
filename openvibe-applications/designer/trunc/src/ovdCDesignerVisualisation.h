#ifndef __OpenViBEDesigner_CDesignerVisualisation_H__
#define __OpenViBEDesigner_CDesignerVisualisation_H__

#include <string>
#include <vector>

namespace OpenViBE
{
	namespace Kernel
	{
		class IVisualisationTree;
		struct ITreeViewCB;
	};
};

namespace OpenViBEDesigner
{
	typedef void (*fpDesignerVisualisationDeleteEventCB)(gpointer user_data);

	class CInterfacedScenario;

	class CDesignerVisualisation : public OpenViBE::Kernel::ITreeViewCB
	{
	public:
		CDesignerVisualisation(
			const OpenViBE::Kernel::IKernelContext& rKernelContext,
			OpenViBE::Kernel::IVisualisationTree& rVisualisationTree,
			CInterfacedScenario& rInterfacedScenario);

		virtual ~CDesignerVisualisation();

		void init(
			std::string guiFile);
		void load();
		void show();
		void hide();

		void setDeleteEventCB(fpDesignerVisualisationDeleteEventCB fpDeleteEventCB, gpointer user_data);

		void onVisualisationBoxAdded(
			const OpenViBE::Kernel::IBox* pBox);
		void onVisualisationBoxRemoved(
			const OpenViBE::CIdentifier& rBoxIdentifier);
		void onVisualisationBoxRenamed(
			const OpenViBE::CIdentifier& rBoxIdentifier);

		//ITreeViewCB callbacks overloading
		void createTreeWidget(
			OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget);
		GtkWidget* loadTreeWidget(
			OpenViBE::Kernel::IVisualisationWidget*);
		void endLoadTreeWidget(
			OpenViBE::Kernel::IVisualisationWidget*);
		GtkWidget* getTreeWidget(
			GtkWidget* visualisationWidget);
		GtkWidget* getVisualisationWidget(
			GtkWidget* treeWidget);
		const char* getTreeWidgetIcon(
			OpenViBE::Kernel::EVisualisationTreeNode);

		//callbacks for dialog
#ifdef HANDLE_MIN_MAX_EVENTS
		static gboolean window_state_event_cb(
			GtkWidget* widget,
			GdkEventWindowState* event,
      gpointer user_data);
#endif
		static gboolean configure_event_cb(
			GtkWidget* widget,
			GdkEventConfigure* event,
			gpointer user_data);
		static gboolean widget_expose_event_cb(
			GtkWidget* widget,
			GdkEventExpose* event,
			gpointer user_data);
		void resizeCB(
			OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget);

		static void notebook_page_switch_cb(
			GtkNotebook* notebook,
			GtkNotebookPage* page,
			guint pagenum,
			gpointer user_data);

		//callback for paned handle position changes
		static gboolean notify_position_paned_cb(
			GtkWidget* widget,
			GParamSpec* spec,
			gpointer user_data);

		static void ask_new_visualisation_window_cb(
			gpointer pUserData,
			guint callback_action,
			GtkWidget* pWidget);
		static void new_visualisation_window_cb(
			GtkWidget* pWidget,
			gpointer pUserData);
		static void ask_rename_visualisation_window_cb(
			gpointer pUserData,
			guint callback_action,
			GtkWidget* pWidget);
		static void rename_visualisation_window_cb(
			GtkWidget* pWidget,
			gpointer pUserData);
		static void remove_visualisation_window_cb(
			gpointer pUserData,
			guint callback_action,
			GtkWidget* pWidget);

		static void ask_new_visualisation_panel_cb(
			gpointer pUserData,
			guint callback_action,
			GtkWidget* pWidget);
		static void new_visualisation_panel_cb(
			GtkWidget* pWidget,
			gpointer pUserData);
		static void ask_rename_visualisation_panel_cb(
			gpointer pUserData,
			guint callback_action,
			GtkWidget* pWidget);
		static void rename_visualisation_panel_cb(
			GtkWidget* pWidget,
			gpointer pUserData);
		static void remove_visualisation_panel_cb(
			gpointer pUserData,
			guint callback_action,
			GtkWidget* pWidget);

		static void remove_visualisation_widget_cb(
			gpointer pUserData,
			guint callback_action,
			GtkWidget* pWidget);

	private:
		static gboolean delete_event_cb(
			GtkWidget* widget,
      GdkEvent* event,
      gpointer user_data);
		OpenViBE::boolean deleteEventCB();

		void refreshActiveVisualisation(
			GtkTreePath* pSelectedItemPath);

		void setActiveVisualisation(
			const char* activeWindow,
			const char* activePanel);

		GtkTable* newWidgetsTable();
		void setupNewEventBoxTable(
			GtkBuilder* xml);

		//visualisation windows
		void askNewVisualisationWindow(void);
	public:
		OpenViBE::boolean newVisualisationWindow(
			const char* label);
	private:
		void askRenameVisualisationWindow(void);
		OpenViBE::boolean renameVisualisationWindow(
			const char* label);
		OpenViBE::boolean	removeVisualisationWindow(void);

		//visualisation panels
		void askNewVisualisationPanel(void);
		OpenViBE::boolean newVisualisationPanel(
			const char* label);
		void askRenameVisualisationPanel(void);
		OpenViBE::boolean renameVisualisationPanel(
			const char* label);
		OpenViBE::boolean removeVisualisationPanel(void);

		//visualisation widgets
		OpenViBE::boolean removeVisualisationWidget(void);
		OpenViBE::boolean removeVisualisationWidget(
			const OpenViBE::CIdentifier& rIdentifier);
		OpenViBE::boolean destroyVisualisationWidget(
			const OpenViBE::CIdentifier& rIdentifier);

		void enableNotebookSignals(
			GtkWidget* pNotebook,
			OpenViBE::boolean b);
		void notebookPageSelectedCB(
			GtkNotebook* notebook,
			guint pagenum);

		virtual void enablePanedSignals(
			GtkWidget* pPaned,
			OpenViBE::boolean b);
		void notifyPositionPanedCB(
			GtkWidget* widget);

		//Mouse/Key event callbacks
		static void visualisation_widget_key_press_event_cb(
			GtkWidget* pWidget,
			GdkEventKey* pEvent,
			gpointer pUserData);
		void visualisationWidgetKeyPressEventCB(
			GtkWidget* pWidget,
			GdkEventKey* pEventKey);
		static gboolean visualisation_widget_enter_notify_event_cb(
			GtkWidget* pWidget,
      GdkEventCrossing* pEventCrossing,
      gpointer pUserData);
		void visualisationWidgetEnterNotifyEventCB(
			GtkWidget* pWidget,
			GdkEventCrossing* pEventCrossing);
		static gboolean visualisation_widget_leave_notify_event_cb(
			GtkWidget* pWidget,
      GdkEventCrossing* pEventCrossing,
      gpointer pUserData);
		void visualisationWidgetLeaveNotifyEventCB(
			GtkWidget* pWidget,
			GdkEventCrossing* pEventCrossing);

		//Tree management callbacks
		static gboolean button_release_cb(
			GtkWidget* pWidget,
			GdkEventButton *pEvent,
			gpointer pUserData);
		static void cursor_changed_cb(
			GtkTreeView* pTreeView,
			gpointer pUserData);

		//Tree management methods
		void buttonReleaseCB(
			GtkWidget* pWidget,
			GdkEventButton *pEvent);
		void cursorChangedCB(
			GtkTreeView* pTreeView);

		//Drag methods
		static void drag_data_get_from_tree_cb(
			GtkWidget* pSrcWidget,
			GdkDragContext* pDragContex,
			GtkSelectionData* pSelectionData,
			guint uiInfo,
			guint uiT,
			gpointer pData);
		void dragDataGetFromTreeCB(
			GtkWidget* pSrcWidget,
			GtkSelectionData* pSelectionData);
		static void drag_data_get_from_widget_cb(
			GtkWidget* pSrcWidget,
			GdkDragContext* pDC,
			GtkSelectionData* pSelectionData,
			guint uiInfo,
			guint uiTime,
			gpointer pData);
		void dragDataGetFromWidgetCB(
			GtkWidget* pSrcWidget,
			GtkSelectionData* pSelectionData);

		//Drop methods
		static void drag_data_received_in_widget_cb(
			GtkWidget* pDstWidget,
			GdkDragContext*,
			gint,
			gint,
			GtkSelectionData* pSelectionData,
			guint,
			guint,
			gpointer pData);
		void dragDataReceivedInWidgetCB(
			GtkWidget* pDstWidget,
			GtkSelectionData* pSelectionData);
		static void drag_data_received_in_event_box_cb(
			GtkWidget* pDstWidget,
			GdkDragContext* pDC,
			gint iX,
			gint iY,
			GtkSelectionData* pSelectionData,
			guint uiInfo,
			guint uiTime,
			gpointer pData);
		void dragDataReceivedInEventBoxCB(
			GtkWidget* pDstWidget,
			GtkSelectionData* pSelectionData,
			OpenViBE::Kernel::EDragDataLocation oLocation);

	private:

		const OpenViBE::Kernel::IKernelContext&	m_rKernelContext;
		OpenViBE::Kernel::IVisualisationTree& m_rVisualisationTree;
		OpenViBEDesigner::CInterfacedScenario& m_rInterfacedScenario;
		fpDesignerVisualisationDeleteEventCB m_fpDeleteEventCB;
		gpointer m_pDeleteEventUserData;
		std::string m_sGuiFile;
		GtkTreeView* m_pTreeView;
		GtkWidget* m_pDialog;
		GtkWidget* m_pPane;
		//highlighted widget
		GtkWidget* m_pHighlightedWidget;
		//active items
		OpenViBE::CString m_oActiveVisualisationWindowName, m_oActiveVisualisationPanelName;
		OpenViBE::CIdentifier m_oActiveVisualisationBoxIdentifier;
		//preview window visibility flag
		OpenViBE::boolean m_bPreviewWindowVisible;
		OpenViBE::uint32 m_ui32PreviewWindowWidth;
		OpenViBE::uint32 m_ui32PreviewWindowHeight;
		//factories used to build contextual menus
		GtkItemFactory *m_pUnaffectedItemFactory, *m_pVisualisationWindowItemFactory, *m_pVisualisationPanelItemFactory;
		GtkItemFactory *m_pVisualisationBoxItemFactory, *m_pUndefinedItemFactory, *m_pSplitItemFactory;
		//strings identifying top, left, right and bottom event boxes
		std::string m_sTopEventBoxData, m_sLeftEventBoxData, m_sRightEventBoxData, m_sBottomEventBoxData;
	};
};

#endif // __OpenViBEDesigner_CDesignerVisualisation_H__
