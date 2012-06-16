#include "ovd_base.h"
#include "ovdTAttributeHandler.h"
#include "ovdCDesignerVisualisation.h"
#include "ovdCInterfacedScenario.h"
#include "ovdCInputDialog.h"

#include <gdk/gdkkeysyms.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;

static ::GtkTargetEntry targets [] =
{
	{ "STRING", 0, 0 },
	{ "text/plain", 0, 0 },
};

namespace OpenViBEDesigner
{
	void displayErrorDialog(const char* text, const char* secondaryText)
	{
		::GtkWidget* l_pErrorDialog = gtk_message_dialog_new(
						NULL,
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_WARNING,
						GTK_BUTTONS_OK,
						text);

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(l_pErrorDialog), secondaryText);

		gtk_window_set_position(GTK_WINDOW(l_pErrorDialog), GTK_WIN_POS_MOUSE);

		gtk_dialog_run(GTK_DIALOG(l_pErrorDialog));

		gtk_widget_destroy(l_pErrorDialog);
	}

	gint displayQuestionDialog(const char* text, const char* secondaryText)
	{
		::GtkWidget* l_pQuestionDialog = gtk_message_dialog_new(
						NULL,
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_QUESTION,
						GTK_BUTTONS_YES_NO,
						text);

		gtk_message_dialog_format_secondary_text(
						GTK_MESSAGE_DIALOG(l_pQuestionDialog),
						secondaryText);

		gtk_window_set_position(GTK_WINDOW(l_pQuestionDialog), GTK_WIN_POS_MOUSE);

		gint ret = gtk_dialog_run(GTK_DIALOG(l_pQuestionDialog));

		gtk_widget_destroy(l_pQuestionDialog);

		return ret;
	}

	//Table helper function
	//---------------------
	::GtkTableChild* getTableChild(::GtkTable* pTable, int leftAttach, int rightAttach, int topAttach, int bottomAttach)
	{
		GList* pList = pTable->children;
		::GtkTableChild* pTC;

		do
		{
			pTC = (::GtkTableChild*)pList->data;
			if(pTC->left_attach == leftAttach && pTC->right_attach == rightAttach &&
					pTC->top_attach == topAttach && pTC->bottom_attach == bottomAttach)
				return pTC;
			pList = pList->next;
		}while(pList);

		return NULL;
	}
};

//Menus
//-----

static ::GtkItemFactoryEntry unaffected_menu_items[] = {
	{ "/New window", "", (::GtkItemFactoryCallback)CDesignerVisualisation::ask_new_visualisation_window_cb, 1, "<StockItem>", GTK_STOCK_DND_MULTIPLE }
};

static ::GtkItemFactoryEntry visualisation_window_menu_items[] = {
	{ "/New tab", "", (::GtkItemFactoryCallback)CDesignerVisualisation::ask_new_visualisation_panel_cb, 1, "<StockItem>", GTK_STOCK_DND },
	{ "/Rename", "", (::GtkItemFactoryCallback)CDesignerVisualisation::ask_rename_visualisation_window_cb, 1, "<StockItem>", GTK_STOCK_BOLD },
	{ "/Remove", "", (::GtkItemFactoryCallback)CDesignerVisualisation::remove_visualisation_window_cb, 1, "<StockItem>", GTK_STOCK_DELETE }
};

static ::GtkItemFactoryEntry visualisation_panel_menu_items[] = {
	{ "/Rename", "", (::GtkItemFactoryCallback)CDesignerVisualisation::ask_rename_visualisation_panel_cb, 1, "<StockItem>", GTK_STOCK_BOLD },
	{ "/Remove", "", (::GtkItemFactoryCallback)CDesignerVisualisation::remove_visualisation_panel_cb, 1, "<StockItem>", GTK_STOCK_DELETE }
};

static ::GtkItemFactoryEntry visualisation_box_menu_items[] = {
	{ "/Remove", "", (::GtkItemFactoryCallback)CDesignerVisualisation::remove_visualisation_widget_cb, 1, "<StockItem>", GTK_STOCK_DELETE }
};

static ::GtkItemFactoryEntry undefined_widget_menu_items[] = {
	{ "/Remove", "", (::GtkItemFactoryCallback)CDesignerVisualisation::remove_visualisation_widget_cb, 1, "<StockItem>", GTK_STOCK_DELETE }
};

static ::GtkItemFactoryEntry split_widget_menu_items[] = {
	{ "/Remove", "", (::GtkItemFactoryCallback)CDesignerVisualisation::remove_visualisation_widget_cb, 1, "<StockItem>", GTK_STOCK_DELETE }
};

static gint num_unaffected_menu_items           = sizeof (unaffected_menu_items)           / sizeof (unaffected_menu_items[0]);
static gint num_visualisation_window_menu_items = sizeof (visualisation_window_menu_items) / sizeof (visualisation_window_menu_items[0]);
static gint num_visualisation_panel_menu_items  = sizeof (visualisation_panel_menu_items)  / sizeof (visualisation_panel_menu_items[0]);
static gint num_visualisation_box_menu_items    = sizeof (visualisation_box_menu_items)    / sizeof (visualisation_box_menu_items[0]);
static gint num_undefined_widget_menu_items     = sizeof (undefined_widget_menu_items)     / sizeof (undefined_widget_menu_items[0]);
static gint num_split_widget_menu_items         = sizeof (split_widget_menu_items)         / sizeof (split_widget_menu_items[0]);

CDesignerVisualisation::CDesignerVisualisation(const IKernelContext& rKernelContext, IVisualisationTree& rVisualisationTree, CInterfacedScenario& rInterfacedScenario) :
	m_rKernelContext(rKernelContext),
	m_rVisualisationTree(rVisualisationTree),
	m_rInterfacedScenario(rInterfacedScenario),
	m_fpDeleteEventCB(NULL),
	m_pDeleteEventUserData(NULL),
	m_pTreeView(NULL),
	m_pDialog(NULL),
	m_pPane(NULL),
	m_pHighlightedWidget(NULL),
	m_pUnaffectedItemFactory(NULL),
	m_pVisualisationWindowItemFactory(NULL),
	m_pVisualisationPanelItemFactory(NULL),
	m_pVisualisationBoxItemFactory(NULL),
	m_pUndefinedItemFactory(NULL),
	m_pSplitItemFactory(NULL)
{
}

CDesignerVisualisation::~CDesignerVisualisation()
{
	g_signal_handlers_disconnect_by_func(G_OBJECT(m_pDialog), reinterpret_cast<void*>(G_CALLBACK(configure_event_cb)), this);
#ifdef HANDLE_MIN_MAX_EVENTS
	g_signal_handlers_disconnect_by_func(G_OBJECT(m_pDialog), reinterpret_cast<void*>(G_CALLBACK(window_state_event_cb)), this);
#endif
	gtk_widget_destroy(m_pDialog);

	m_rVisualisationTree.setTreeViewCB(NULL);
}

void CDesignerVisualisation::init(std::string guiFile)
{
	m_sGuiFile = guiFile;

	//create tree view
	//----------------

	//register towards tree store
	m_rVisualisationTree.setTreeViewCB(this);

	m_pTreeView = m_rVisualisationTree.createTreeViewWithModel();

	::GtkTreeViewColumn* l_pTreeViewColumnName=gtk_tree_view_column_new();
	::GtkCellRenderer* l_pCellRendererIcon=gtk_cell_renderer_pixbuf_new();
	::GtkCellRenderer* l_pCellRendererName=gtk_cell_renderer_text_new();
	gtk_tree_view_column_set_title(l_pTreeViewColumnName, "Windows for current scenario");
	gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererIcon, FALSE);
	gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererName, TRUE);
	gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererIcon, "stock-id", EVisualisationTreeColumn_StringStockIcon, NULL);
	gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererName, "text", EVisualisationTreeColumn_StringName, NULL);
	//gtk_tree_view_column_set_sizing(l_pTreeViewColumnName, GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_expand(l_pTreeViewColumnName, TRUE/*FALSE*/);
	gtk_tree_view_column_set_resizable(l_pTreeViewColumnName, TRUE);
	gtk_tree_view_column_set_min_width(l_pTreeViewColumnName, 64);
	gtk_tree_view_append_column(m_pTreeView, l_pTreeViewColumnName);

	::GtkTreeViewColumn* l_pTreeViewColumnDesc=gtk_tree_view_column_new();
	gtk_tree_view_append_column(m_pTreeView, l_pTreeViewColumnDesc);

	gtk_tree_view_column_set_visible(l_pTreeViewColumnDesc, 0);
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(m_pTreeView));

	//allow tree items to be dragged
	gtk_drag_source_set(GTK_WIDGET(m_pTreeView), GDK_BUTTON1_MASK, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);

	//require notifications upon tree item dragging, mouse button release, active item change
	g_signal_connect(G_OBJECT(m_pTreeView), "drag_data_get", G_CALLBACK(drag_data_get_from_tree_cb), this);
	g_signal_connect(G_OBJECT(m_pTreeView), "button-release-event", G_CALLBACK(button_release_cb), this);
	g_signal_connect(G_OBJECT(m_pTreeView), "cursor-changed", G_CALLBACK(cursor_changed_cb), this);

	GTK_WIDGET_SET_FLAGS(GTK_WIDGET(m_pTreeView), GDK_KEY_PRESS_MASK);
	g_signal_connect(G_OBJECT(m_pTreeView), "key-press-event", G_CALLBACK(visualisation_widget_key_press_event_cb), this);

	//create main dialog
	//------------------
	m_pDialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_default_size(GTK_WINDOW(m_pDialog), 600, 400);
	gtk_signal_connect(GTK_OBJECT(m_pDialog), "configure_event", G_CALLBACK(configure_event_cb), this);
#ifdef HANDLE_MIN_MAX_EVENTS
	gtk_signal_connect(GTK_OBJECT(m_pDialog), "window_state_event", G_CALLBACK(window_state_event_cb), this);
#endif
	g_signal_connect(G_OBJECT(m_pDialog), "delete-event", G_CALLBACK(delete_event_cb), this);

	//main pane : tree view to the left, widgets table to the right
	m_pPane = gtk_hpaned_new();
	gtk_container_add(GTK_CONTAINER(m_pDialog), (::GtkWidget*)m_pPane);

	//add tree view to pane
	gtk_paned_add1((::GtkPaned*)m_pPane, GTK_WIDGET(m_pTreeView));

	//create popup menus
	//------------------
	m_pUnaffectedItemFactory = gtk_item_factory_new (GTK_TYPE_MENU, "<unaffected_main>", NULL);
	gtk_item_factory_create_items(m_pUnaffectedItemFactory, num_unaffected_menu_items, unaffected_menu_items, this);

	m_pVisualisationWindowItemFactory = gtk_item_factory_new (GTK_TYPE_MENU, "<visualisation_window_main>", NULL);
	gtk_item_factory_create_items(m_pVisualisationWindowItemFactory, num_visualisation_window_menu_items, visualisation_window_menu_items, this);

	m_pVisualisationPanelItemFactory = gtk_item_factory_new (GTK_TYPE_MENU, "<visualisation_panel_main>", NULL);
	gtk_item_factory_create_items(m_pVisualisationPanelItemFactory, num_visualisation_panel_menu_items, visualisation_panel_menu_items, this);

	m_pVisualisationBoxItemFactory = gtk_item_factory_new (GTK_TYPE_MENU, "<visualisation_box_main>", NULL);
	gtk_item_factory_create_items(m_pVisualisationBoxItemFactory, num_visualisation_box_menu_items, visualisation_box_menu_items, this);

	m_pUndefinedItemFactory = gtk_item_factory_new (GTK_TYPE_MENU, "<undefined_widget_main>", NULL);
	gtk_item_factory_create_items(m_pUndefinedItemFactory, num_undefined_widget_menu_items, undefined_widget_menu_items, this);

	m_pSplitItemFactory = gtk_item_factory_new (GTK_TYPE_MENU, "<split_widget_main>", NULL);
	gtk_item_factory_create_items(m_pSplitItemFactory, num_split_widget_menu_items, split_widget_menu_items, this);

	//create a default window
	newVisualisationWindow("Default window");
}

void CDesignerVisualisation::reset(void)
{
	m_rVisualisationTree.setTreeViewCB(this);

	m_rVisualisationTree.reloadTree();

	gtk_tree_view_expand_all(m_pTreeView);

	setActiveVisualisation(m_oActiveVisualisationWindowName, m_oActiveVisualisationPanelName);
}

void CDesignerVisualisation::show()
{
	gtk_widget_show_all((::GtkWidget*)m_pDialog);
}

void CDesignerVisualisation::hide()
{
	gtk_widget_hide_all((::GtkWidget*)m_pDialog);
}

void CDesignerVisualisation::setDeleteEventCB(fpDesignerVisualisationDeleteEventCB fpDeleteEventCB, gpointer user_data)
{
	m_fpDeleteEventCB = fpDeleteEventCB;
	m_pDeleteEventUserData = user_data;
}

void CDesignerVisualisation::onVisualisationBoxAdded(const IBox* pBox)
{
	CIdentifier l_oVisualisationWidgetIdentifier;
	m_rVisualisationTree.addVisualisationWidget(
		l_oVisualisationWidgetIdentifier,
		pBox->getName(),
		EVisualisationWidget_VisualisationBox,
		OV_UndefinedIdentifier,
		0,
		pBox->getIdentifier(),
		0);

	m_rVisualisationTree.reloadTree();

	//refresh view
	::GtkTreeIter l_oIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, l_oVisualisationWidgetIdentifier);
	refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oIter));
}

void CDesignerVisualisation::onVisualisationBoxRemoved(const CIdentifier& rBoxIdentifier)
{
	IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidgetFromBoxIdentifier(rBoxIdentifier);
	if(l_pVisualisationWidget != NULL)
	{
		//unaffected widget : delete it
		if(l_pVisualisationWidget->getParentIdentifier() == OV_UndefinedIdentifier)
		{
			m_rVisualisationTree.destroyHierarchy(l_pVisualisationWidget->getIdentifier());
		}
		else //simplify tree
		{
			destroyVisualisationWidget(l_pVisualisationWidget->getIdentifier());
		}

		m_rVisualisationTree.reloadTree();

		//refresh view
		refreshActiveVisualisation(NULL);
	}
}

void CDesignerVisualisation::createTreeWidget(IVisualisationWidget* pWidget)
{
	if(pWidget->getType() == EVisualisationWidget_HorizontalSplit || pWidget->getType() == EVisualisationWidget_VerticalSplit)
	{
		TAttributeHandler l_oAttributeHandler(*pWidget);
		l_oAttributeHandler.addAttribute(OVD_AttributeId_VisualisationWidget_IsPanedHorizontal, pWidget->getType() == EVisualisationWidget_HorizontalSplit ? 1 : 0);
		l_oAttributeHandler.addAttribute(OVD_AttributeId_VisualisationWidget_DividerPosition, 1);
		l_oAttributeHandler.addAttribute(OVD_AttributeId_VisualisationWidget_MaxDividerPosition, 2);
	}
}

//need width request of 0 to avoid graphical bugs (label/icon overlapping other widgets) when shrinking buttons
static gint s_labelWidthRequest = 0;
static gint s_iconWidthRequest = 0;
//need expand and fill flags to TRUE to see 0-size-requesting widgets
static gboolean s_labelExpand = TRUE;
static gboolean s_labelFill = TRUE;
static gboolean s_iconExpand = TRUE;
static gboolean s_iconFill = TRUE;

::GtkWidget* CDesignerVisualisation::loadTreeWidget(IVisualisationWidget* pWidget)
{
	::GtkWidget* l_pTreeWidget = NULL;

	if(pWidget->getType() == EVisualisationWidget_VisualisationPanel)
	{
		//tree widget = visualisation widget = notebook
		l_pTreeWidget = gtk_notebook_new();
	}
	else if(pWidget->getType() == EVisualisationWidget_VerticalSplit ||	pWidget->getType() == EVisualisationWidget_HorizontalSplit)
	{
		//visualisation widget = ::GtkPaned
		//tree widget = table containing event boxes + visualisation widget in the center
		l_pTreeWidget = GTK_WIDGET(newWidgetsTable());
		::GtkWidget* l_pCurrentVisualisationWidget = getVisualisationWidget(l_pTreeWidget);
		if(l_pCurrentVisualisationWidget != NULL)
			gtk_container_remove(GTK_CONTAINER(l_pTreeWidget), l_pCurrentVisualisationWidget);
		::GtkWidget* l_pPaned = (pWidget->getType() == EVisualisationWidget_HorizontalSplit) ? gtk_hpaned_new() : gtk_vpaned_new();
		gtk_table_attach(GTK_TABLE(l_pTreeWidget), l_pPaned, 1, 2, 1, 2,
			::GtkAttachOptions(GTK_EXPAND|GTK_SHRINK|GTK_FILL),
			::GtkAttachOptions(GTK_EXPAND|GTK_SHRINK|GTK_FILL), 0, 0);
	}
	else if(pWidget->getType() == EVisualisationWidget_Undefined || pWidget->getType() == EVisualisationWidget_VisualisationBox)
	{
		//visualisation widget = ::GtkButton (left : icon, right : label)
		//tree widget = table containing event boxes + visualisation widget in the center
		l_pTreeWidget = GTK_WIDGET(newWidgetsTable());
		::GtkWidget* l_pCurrentVisualisationWidget = getVisualisationWidget(l_pTreeWidget);
		if(l_pCurrentVisualisationWidget != NULL)
			gtk_container_remove(GTK_CONTAINER(l_pTreeWidget), l_pCurrentVisualisationWidget);

		//button
		::GtkWidget* l_pButton = gtk_button_new();
		gtk_widget_set_size_request(l_pButton, 0, 0);
		gtk_table_attach(GTK_TABLE(l_pTreeWidget), l_pButton, 1, 2, 1, 2,
			::GtkAttachOptions(GTK_EXPAND|GTK_SHRINK|GTK_FILL),
			::GtkAttachOptions(GTK_EXPAND|GTK_SHRINK|GTK_FILL), 0, 0);

		//box inserted in button
		::GtkBox* l_pBox = GTK_BOX(gtk_vbox_new(FALSE, 0));
		gtk_widget_set_size_request(GTK_WIDGET(l_pBox), 0, 0);

		//icon - actual icon will be loaded in endLoadTreeWidget
		::GtkWidget* l_pIcon = gtk_image_new_from_stock(getTreeWidgetIcon(EVisualisationTreeNode_Undefined), GTK_ICON_SIZE_BUTTON);
		if(s_iconWidthRequest == 0)
			gtk_widget_set_size_request(l_pIcon, 0, 0);
		gtk_box_pack_start(l_pBox, l_pIcon, s_iconExpand, s_iconFill, 0);

		//label
		::GtkWidget* l_pLabel = gtk_label_new((const char*)pWidget->getName());
		if(s_labelWidthRequest == 0)
			gtk_widget_set_size_request(l_pLabel, 0, 0);
		gtk_box_pack_start(l_pBox, l_pLabel, s_labelExpand, s_labelFill, 0);

		//add box to button and button to table
		gtk_container_add(GTK_CONTAINER(l_pButton), GTK_WIDGET(l_pBox));

		//set up button as drag destination
		gtk_drag_dest_set(l_pButton, GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
		g_signal_connect(G_OBJECT(l_pButton), "drag_data_received", G_CALLBACK(drag_data_received_in_widget_cb), this);

		//set up button as drag source as well
		gtk_drag_source_set(l_pButton, GDK_BUTTON1_MASK, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
		g_signal_connect(G_OBJECT(l_pButton), "drag_data_get", G_CALLBACK(drag_data_get_from_widget_cb), this);

		if(pWidget->getType() == EVisualisationWidget_VisualisationBox)
		{
			GTK_WIDGET_SET_FLAGS(l_pButton, GDK_KEY_PRESS_MASK|GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK);
			g_signal_connect(G_OBJECT(l_pButton), "key-press-event", G_CALLBACK(visualisation_widget_key_press_event_cb), this);
			g_signal_connect(G_OBJECT(l_pButton), "enter-notify-event", G_CALLBACK(visualisation_widget_enter_notify_event_cb), this);
			g_signal_connect(G_OBJECT(l_pButton), "leave-notify-event", G_CALLBACK(visualisation_widget_leave_notify_event_cb), this);
		}
	}

	//resize widgets once they are allocated : this is the case when they are shown on an expose event
	//FIXME : perform resizing only once (when it is done as many times as there are widgets in the tree here)
	if(l_pTreeWidget != NULL)
		gtk_signal_connect(GTK_OBJECT(getVisualisationWidget(l_pTreeWidget)), "expose-event", G_CALLBACK(widget_expose_event_cb), this);

	return l_pTreeWidget;
}

void CDesignerVisualisation::endLoadTreeWidget(::GtkWidget* treeWidget)
{
	::GtkWidget* l_pVisualisationWidget = getVisualisationWidget(treeWidget);

	if(GTK_IS_BUTTON(l_pVisualisationWidget) != FALSE)
	{
		//retrieve icon name from tree
		::GtkTreeIter l_oIter;
		m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, treeWidget);
		char* l_pIconString = NULL;
		m_rVisualisationTree.getStringValueFromTreeIter(&l_oIter, l_pIconString, EVisualisationTreeColumn_StringStockIcon);
		//retrieve hbox
		GList* l_pButtonChildren = gtk_container_get_children(GTK_CONTAINER(l_pVisualisationWidget));
		::GtkContainer* l_pBox = GTK_CONTAINER(l_pButtonChildren->data);
		//remove first widget
		GList* l_pBoxChildren = gtk_container_get_children(l_pBox);
		gtk_container_remove(l_pBox, GTK_WIDGET(l_pBoxChildren->data));
		//create new icon
		::GtkWidget* l_pIcon = gtk_image_new_from_stock(l_pIconString, GTK_ICON_SIZE_BUTTON);
		if(s_iconWidthRequest == 0)
			gtk_widget_set_size_request(l_pIcon, 0, 0);
		gtk_box_pack_start(GTK_BOX(l_pBox), l_pIcon, s_iconExpand, s_iconFill, 0);
		//insert it in first position
		gtk_box_reorder_child(GTK_BOX(l_pBox), l_pIcon, 0);
	}
	if(GTK_IS_PANED(l_pVisualisationWidget) != FALSE)
	{
		enablePanedSignals(l_pVisualisationWidget, true);
	}
	else if(GTK_IS_NOTEBOOK(l_pVisualisationWidget) != FALSE)
	{
		enableNotebookSignals(l_pVisualisationWidget, true);
	}
}

::GtkWidget* CDesignerVisualisation::getTreeWidget(::GtkWidget* visualisationWidget)
{
	return gtk_widget_get_parent(visualisationWidget);
}

::GtkWidget* CDesignerVisualisation::getVisualisationWidget(::GtkWidget* pWidget)
{
	if(GTK_IS_TABLE(pWidget))
		return getTableChild(GTK_TABLE(pWidget), 1, 2, 1, 2)->widget;
	else
		return pWidget;
}

const char* CDesignerVisualisation::getTreeWidgetIcon(EVisualisationTreeNode type)
{
	switch(type)
	{
		case EVisualisationTreeNode_Unaffected:
			return GTK_STOCK_DIALOG_QUESTION;
		case EVisualisationTreeNode_Undefined:
			return GTK_STOCK_CANCEL;
		case EVisualisationTreeNode_VisualisationWindow:
			return GTK_STOCK_DND_MULTIPLE;
		case EVisualisationTreeNode_VisualisationPanel:
			return GTK_STOCK_DND;
		case EVisualisationTreeNode_VisualisationBox:
			return GTK_STOCK_EXECUTE; //default (actual icon name may be retrieved from box descriptor)
		case EVisualisationTreeNode_HorizontalSplit:
		case EVisualisationTreeNode_VerticalSplit:
			return GTK_STOCK_ADD;
		default:
			return "";
	}
}

void CDesignerVisualisation::resizePanedWidget(IVisualisationWidget* pVisualisationWidget, ::GtkWidget* pPaned)
{
	enablePanedSignals(pPaned, false);

	//retrieve its attributes
	TAttributeHandler l_oAttributeHandler(*pVisualisationWidget);
	int l_i32HandlePos = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWidget_DividerPosition);
	int l_i32MaxHandlePos = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWidget_MaxDividerPosition);

	if(l_i32MaxHandlePos > 0)
	{
		//retrieve current maximum handle position
		int l_i32CurrentMaxHandlePos = GTK_IS_VPANED(pPaned) ?
			GTK_PANED(pPaned)->container.widget.allocation.height : GTK_PANED(pPaned)->container.widget.allocation.width;

		//set new paned handle position
		gtk_paned_set_position(GTK_PANED(pPaned), l_i32HandlePos * l_i32CurrentMaxHandlePos / l_i32MaxHandlePos);
	}

	enablePanedSignals(pPaned, true);
}

gboolean CDesignerVisualisation::delete_event_cb(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
	return static_cast<CDesignerVisualisation*>(user_data)->deleteEventCB() == true ? TRUE : FALSE;
}

OpenViBE::boolean CDesignerVisualisation::deleteEventCB()
{
	if(m_fpDeleteEventCB != NULL)
	{
		m_fpDeleteEventCB(m_pDeleteEventUserData);
		return true;
	}

	return false;
}

#ifdef HANDLE_MIN_MAX_EVENTS
gboolean CDesignerVisualisation::window_state_event_cb(::GtkWidget* widget,GdkEventWindowState* event, gpointer user_data)
{
	//refresh widgets if window was maximized or minimized
	if(event->changed_mask & GDK_WINDOW_STATE_MAXIMIZED ||
		event->changed_mask & GDK_WINDOW_STATE_ICONIFIED)
	{
		//widgets haven't been reallocated yet, perform resizing only when this happens
		//gtk_signal_connect(GTK_OBJECT(gtk_paned_get_child2(GTK_PANED(m_pPane))), "size-allocate", G_CALLBACK(widget_size_allocate_cb), this);
		gtk_signal_connect(GTK_OBJECT(gtk_paned_get_child2(GTK_PANED(m_pPane))), "expose-event", G_CALLBACK(widget_expose_cb), this);
	}

	return FALSE;
}
#endif

//event generated whenever window changes size, including when it is first created
gboolean CDesignerVisualisation::configure_event_cb(::GtkWidget* widget, GdkEventConfigure* event, gpointer user_data)
{
	static_cast<CDesignerVisualisation*>(user_data)->resizeCB();

	return FALSE;
}

gboolean CDesignerVisualisation::widget_expose_event_cb(::GtkWidget* widget, GdkEventExpose* event, gpointer user_data)
{
	g_signal_handlers_disconnect_by_func(G_OBJECT(widget), reinterpret_cast<void*>(G_CALLBACK(CDesignerVisualisation::widget_expose_event_cb)), user_data);

	static_cast<CDesignerVisualisation*>(user_data)->resizeCB();

	return FALSE;
}

void CDesignerVisualisation::resizeCB()
{
	//get window size and store it for each window
	::GtkWidget* l_pNotebook = gtk_paned_get_child2(GTK_PANED(m_pPane));
	if(l_pNotebook != NULL)
	{
		CIdentifier l_oVisualisationWindowIdentifier = OV_UndefinedIdentifier;

		while(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oVisualisationWindowIdentifier, EVisualisationWidget_VisualisationWindow) == true)
		{
			IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier);

			//store new dimensions
			TAttributeHandler l_oAttributeHandler(*l_pVisualisationWindow);
			l_oAttributeHandler.setAttributeValue(OVD_AttributeId_VisualisationWindow_Width, l_pNotebook->allocation.width);
			l_oAttributeHandler.setAttributeValue(OVD_AttributeId_VisualisationWindow_Height, l_pNotebook->allocation.height);
		}
	}

	//retrieve iterator to active window
	::GtkTreeIter l_oWindowIter;
	if(m_rVisualisationTree.findChildNodeFromRoot(&l_oWindowIter, m_oActiveVisualisationWindowName, EVisualisationTreeNode_VisualisationWindow) == false)
		return;

	//retrieve iterator to active panel
	::GtkTreeIter l_oPanelIter = l_oWindowIter;
	if(m_rVisualisationTree.findChildNodeFromParent(&l_oPanelIter, m_oActiveVisualisationPanelName, EVisualisationTreeNode_VisualisationPanel) == false)
		return;

	//resize panel
	m_rVisualisationTree.resizeVisualisationPanel(&l_oPanelIter);
}

void CDesignerVisualisation::notebook_page_switch_cb(::GtkNotebook* notebook, ::GtkNotebookPage* page, guint pagenum, gpointer user_data)
{
	static_cast<CDesignerVisualisation*>(user_data)->notebookPageSelectedCB(notebook, pagenum);
}

gboolean CDesignerVisualisation::notify_position_paned_cb(::GtkWidget *widget, GParamSpec* spec, gpointer user_data)
{
	static_cast<CDesignerVisualisation*>(user_data)->notifyPositionPanedCB(widget);
	return TRUE;
}

//--------------------------
//Event box table management
//--------------------------

void CDesignerVisualisation::setupNewEventBoxTable(GladeXML* xml)
{
	//set up event boxes as drag targets
	gtk_drag_dest_set(glade_xml_get_widget(xml, "eventbox2"), GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	gtk_drag_dest_set(glade_xml_get_widget(xml, "eventbox4"), GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	gtk_drag_dest_set(glade_xml_get_widget(xml, "eventbox6"), GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	gtk_drag_dest_set(glade_xml_get_widget(xml, "eventbox8"), GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);

	//set up event boxes callbacks for drag data received events
	char buf[256];
	sprintf(buf, "%i %s", (int)this, "top");
	m_sTopEventBoxData = buf;
	g_signal_connect(G_OBJECT(glade_xml_get_widget(xml, "eventbox2")), "drag_data_received", G_CALLBACK(drag_data_received_in_event_box_cb),
		gpointer(m_sTopEventBoxData.c_str()));
	sprintf(buf, "%i %s", (int)this, "left");
	m_sLeftEventBoxData = buf;
	g_signal_connect(G_OBJECT(glade_xml_get_widget(xml, "eventbox4")), "drag_data_received", G_CALLBACK(drag_data_received_in_event_box_cb),
		gpointer(m_sLeftEventBoxData.c_str()));
	sprintf(buf, "%i %s", (int)this, "right");
	m_sRightEventBoxData = buf;
	g_signal_connect(G_OBJECT(glade_xml_get_widget(xml, "eventbox6")), "drag_data_received", G_CALLBACK(drag_data_received_in_event_box_cb),
		gpointer(m_sRightEventBoxData.c_str()));
	sprintf(buf, "%i %s", (int)this, "bottom");
	m_sBottomEventBoxData = buf;
	g_signal_connect(G_OBJECT(glade_xml_get_widget(xml, "eventbox8")), "drag_data_received", G_CALLBACK(drag_data_received_in_event_box_cb),
		gpointer(m_sBottomEventBoxData.c_str()));
}

void CDesignerVisualisation::refreshActiveVisualisation(::GtkTreePath* pSelectedItemPath)
{
	//show tree
	gtk_tree_view_expand_all(m_pTreeView);

	//select item
	if(pSelectedItemPath != NULL)
	{
		gtk_tree_view_set_cursor(m_pTreeView, pSelectedItemPath, NULL, false);
	}
	else //select previous visualisation tab again (or another tab if it doesn't exist anymore)
	{
		setActiveVisualisation(m_oActiveVisualisationWindowName, m_oActiveVisualisationPanelName);
	}
}

void CDesignerVisualisation::setActiveVisualisation(const char* _activeWindow, const char* _activePanel)
{
	//ensures correct behavior when _active[Window/Panel] point to m_oActiveVisualisation[Window/Panel]Name.m_pSecretImplementation
	CString activeWindow = _activeWindow;
	CString activePanel = _activePanel;

	//clear active window/panel names
	m_oActiveVisualisationWindowName = "";
	m_oActiveVisualisationPanelName = "";

	//retrieve active window
	::GtkTreeIter l_oWindowIter;

	if(m_rVisualisationTree.findChildNodeFromRoot(&l_oWindowIter, activeWindow, EVisualisationTreeNode_VisualisationWindow) == true)
	{
		m_oActiveVisualisationWindowName = activeWindow;
	}
	else
	{
		//pick first window if previously active window doesn't exist anymore
		CIdentifier l_oIdentifier = OV_UndefinedIdentifier;

		if(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oIdentifier, EVisualisationWidget_VisualisationWindow) == true)
		{
			m_oActiveVisualisationWindowName = m_rVisualisationTree.getVisualisationWidget(l_oIdentifier)->getName();
			m_rVisualisationTree.findChildNodeFromRoot(&l_oWindowIter, (const char*)m_oActiveVisualisationWindowName, EVisualisationTreeNode_VisualisationWindow);
		}
		else //no windows left
		{
			gtk_container_remove(GTK_CONTAINER(m_pPane), gtk_paned_get_child2(GTK_PANED(m_pPane)));
			return;
		}
	}

	//retrieve active panel
	::GtkTreeIter l_oPanelIter = l_oWindowIter;
	if(m_rVisualisationTree.findChildNodeFromParent(&l_oPanelIter, activePanel, EVisualisationTreeNode_VisualisationPanel) == true)
	{
		m_oActiveVisualisationPanelName = activePanel;
	}
	else //couldn't find panel : select first one
	{
		CIdentifier l_oWindowIdentifier;
		m_rVisualisationTree.getIdentifierFromTreeIter(&l_oWindowIter, l_oWindowIdentifier, EVisualisationTreeColumn_StringIdentifier);
		IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oWindowIdentifier);
		CIdentifier l_oPanelIdentifier;
		if(l_pVisualisationWindow->getChildIdentifier(0, l_oPanelIdentifier) == true)
		{
			l_oPanelIter = l_oWindowIter;
			m_rVisualisationTree.findChildNodeFromParent(&l_oPanelIter, l_oPanelIdentifier);
			char* l_pString = NULL;
			m_rVisualisationTree.getStringValueFromTreeIter(&l_oPanelIter, l_pString, EVisualisationTreeColumn_StringName);
			m_oActiveVisualisationPanelName = l_pString;
		}
		else //no panel in window
		{
			::GtkWidget* l_pCurrentNotebook = gtk_paned_get_child2(GTK_PANED(m_pPane));
			if(l_pCurrentNotebook != NULL)
			{
				gtk_object_ref(GTK_OBJECT(l_pCurrentNotebook));
				gtk_container_remove(GTK_CONTAINER(m_pPane), l_pCurrentNotebook);
			}
			return;
		}
	}

	//retrieve notebook	and set it visible
	void* l_pNotebook = NULL;
	m_rVisualisationTree.getPointerValueFromTreeIter(&l_oPanelIter, l_pNotebook, EVisualisationTreeColumn_PointerWidget);
	::GtkWidget* l_pCurrentNotebook = gtk_paned_get_child2(GTK_PANED(m_pPane));
	if(l_pCurrentNotebook != GTK_WIDGET(l_pNotebook))
	{
		if(l_pCurrentNotebook != NULL)
		{
			//FIXME : don't ref previous notebook if parent window doesn't exist anymore
			gtk_object_ref(GTK_OBJECT(l_pCurrentNotebook));
			gtk_container_remove(GTK_CONTAINER(m_pPane), l_pCurrentNotebook);
		}
		gtk_paned_add2(GTK_PANED(m_pPane), GTK_WIDGET(l_pNotebook));
		//gtk_object_unref(l_pCurrentNotebook);
	}

	//disable switch page notifications
	enableNotebookSignals(GTK_WIDGET(l_pNotebook), false);

	//set active panel visible
	int i;
	for(i=0; i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(l_pNotebook)); i++)
	{
		if(strcmp(gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(l_pNotebook),
			gtk_notebook_get_nth_page(GTK_NOTEBOOK(l_pNotebook), i)), m_oActiveVisualisationPanelName) == 0)
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(l_pNotebook), i);
			break;
		}
	}

	//if active page couldn't be found
	if(i == gtk_notebook_get_n_pages(GTK_NOTEBOOK(l_pNotebook)))
	{
		//error!
		//pick first page if it exists
		if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(l_pNotebook)) > 0)
		{
			m_oActiveVisualisationPanelName = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(l_pNotebook),
				gtk_notebook_get_nth_page(GTK_NOTEBOOK(l_pNotebook), 0));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(l_pNotebook), 0);
		}
		else //error : no pages in notebook, clear panel name
		{
			m_oActiveVisualisationPanelName = "";
		}
	}

	//enable switch page notifications
	enableNotebookSignals(GTK_WIDGET(l_pNotebook), true);

	//refresh display
	gtk_widget_show_all(m_pPane);
}

//creates a new widgets table and sets it as current
::GtkTable* CDesignerVisualisation::newWidgetsTable()
{
	GladeXML* pGladeXMLTable = glade_xml_new(m_sGuiFile.c_str(), "table", NULL);

	//set up event boxes
	setupNewEventBoxTable(pGladeXMLTable);

	::GtkTable* pTable = GTK_TABLE(glade_xml_get_widget(pGladeXMLTable, "table"));

	//clear central button label
	::GtkTableChild* pTC = getTableChild(pTable, 1, 2, 1, 2);
	::GtkButton* pButton = GTK_BUTTON(pTC->widget);
	gtk_button_set_label(pButton, "");

	//set it up as drag destination
	gtk_drag_dest_set(GTK_WIDGET(pButton), GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	g_signal_connect(G_OBJECT(pButton), "drag_data_received", G_CALLBACK(drag_data_received_in_widget_cb), this);

	//set it up as drag source as well
	gtk_drag_source_set(GTK_WIDGET(pButton), GDK_BUTTON1_MASK, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	g_signal_connect(G_OBJECT(pButton), "drag_data_get", G_CALLBACK(drag_data_get_from_widget_cb), this);

	return pTable;
}

void CDesignerVisualisation::askNewVisualisationWindow()
{
	//show dialog
	CInputDialog id(m_sGuiFile.c_str(), &CDesignerVisualisation::new_visualisation_window_cb, this, "New window", "Please enter name of new window : ");

	id.run();
}

boolean CDesignerVisualisation::newVisualisationWindow(const char* label)
{
	//ensure name is unique
	IVisualisationWidget* l_pVisualisationWindow;
	CIdentifier l_oVisualisationWindowIdentifier = OV_UndefinedIdentifier;

	while(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oVisualisationWindowIdentifier, EVisualisationWidget_VisualisationWindow) == true)
	{
		l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier);

		if(strcmp((const char*)l_pVisualisationWindow->getName(), label) == 0)
		{
			displayErrorDialog("Window creation failed !", "An existing window already uses this name. Please choose another name.");
			return false;
		}
	}

	//proceed with window creation
	//m_rVisualisationTree.addVisualisationWindow(l_oVisualisationWindowIdentifier, CString(label));
	m_rVisualisationTree.addVisualisationWidget(
		l_oVisualisationWindowIdentifier,
		CString(label),
		EVisualisationWidget_VisualisationWindow,
		OV_UndefinedIdentifier,
		0,
		OV_UndefinedIdentifier,
		0);

	l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier);

	//add attributes
	TAttributeHandler l_oAttributeHandler(*l_pVisualisationWindow);
	l_oAttributeHandler.addAttribute(OVD_AttributeId_VisualisationWindow_Width, 1);
	l_oAttributeHandler.addAttribute(OVD_AttributeId_VisualisationWindow_Height, 1);

	//create default visualisation panel as well
	CIdentifier l_oChildIdentifier;
	CString l_oChildName = "Default tab";

	m_rVisualisationTree.addVisualisationWidget(
		l_oChildIdentifier,
		l_oChildName,
		EVisualisationWidget_VisualisationPanel,
		l_oVisualisationWindowIdentifier,
		0,
		OV_UndefinedIdentifier,
		1);

	m_rVisualisationTree.reloadTree();

	//refresh view
	::GtkTreeIter l_oChildIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oChildIter, l_oChildIdentifier);
	refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oChildIter));

	return true;
}

void CDesignerVisualisation::askRenameVisualisationWindow()
{
	//show dialog
	CInputDialog id(m_sGuiFile.c_str(), &CDesignerVisualisation::rename_visualisation_window_cb, this, "Rename window", "Please enter new name of window : ");

	id.run();
}

boolean CDesignerVisualisation::renameVisualisationWindow(const char* pNewVisualisationWindowName)
{
	//retrieve visualisation window
	::GtkTreeIter l_oIter;
	if(m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, m_oActiveVisualisationWindowName, EVisualisationTreeNode_VisualisationWindow) == false)
	{
		displayErrorDialog("Window renaming failed !", "Couldn't retrieve window.");
		return false;
	}

	CIdentifier l_oWindowIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oWindowIdentifier, EVisualisationTreeColumn_StringIdentifier);
	IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oWindowIdentifier);
	if(l_pVisualisationWindow == NULL)
	{
		displayErrorDialog("Window renaming failed !", "Couldn't retrieve window.");
		return false;
	}

	//if trying to set identical name, return
	CString l_oNewWindowName = pNewVisualisationWindowName;
	if(l_pVisualisationWindow->getName() == l_oNewWindowName)
		return true;

	//ensure name is unique
	CIdentifier l_oVisualisationWindowIdentifier = OV_UndefinedIdentifier;
	while(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oVisualisationWindowIdentifier, EVisualisationWidget_VisualisationWindow) == true)
	{
		//name already in use : warn user
		if(m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier)->getName() == l_oNewWindowName)
		{
			displayErrorDialog("Window renaming failed !", "An existing window already uses this name. Please choose another name.");
			return false;
		}
	}

	//change its name
	l_pVisualisationWindow->setName(l_oNewWindowName);

	m_rVisualisationTree.reloadTree();

	//refresh view
	m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, l_oWindowIdentifier);
	refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oIter));

	return true;
}

boolean CDesignerVisualisation::removeVisualisationWindow()
{
	//retrieve visualisation window
	CIdentifier l_oVisualisationWindowIdentifier = OV_UndefinedIdentifier;
	while(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oVisualisationWindowIdentifier, EVisualisationWidget_VisualisationWindow) == true)
		if(m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier)->getName() == m_oActiveVisualisationWindowName)
			break;

	//return if window was not found
	if(l_oVisualisationWindowIdentifier == OV_UndefinedIdentifier)
	{
		displayErrorDialog("Window removal failed !", "Couldn't retrieve window.");
		return false;
	}

	//destroy hierarchy but only unaffect visualisation boxes
	m_rVisualisationTree.destroyHierarchy(l_oVisualisationWindowIdentifier, false);

	m_rVisualisationTree.reloadTree();

	//refresh view
	refreshActiveVisualisation(NULL);

	return true;
}

void CDesignerVisualisation::askNewVisualisationPanel()
{
	//show dialog
	CInputDialog id(m_sGuiFile.c_str(), &CDesignerVisualisation::new_visualisation_panel_cb, this, "New tab", "Please enter name of new tab : ");

	id.run();
}

boolean CDesignerVisualisation::newVisualisationPanel(const char* label)
{
	//retrieve visualisation window
	IVisualisationWidget* l_pVisualisationWindow=NULL;
	CIdentifier l_oVisualisationWindowIdentifier = OV_UndefinedIdentifier;

	while(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oVisualisationWindowIdentifier, EVisualisationWidget_VisualisationWindow) == true)
	{
		l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier);
		if(l_pVisualisationWindow->getName() == m_oActiveVisualisationWindowName)
			break;
	}

	//return if parent window was not found
	if(l_oVisualisationWindowIdentifier == OV_UndefinedIdentifier || l_pVisualisationWindow==NULL)
	{
		displayErrorDialog("Tab creation failed !", "Couldn't retrieve parent window.");
		return false;
	}

	CIdentifier l_oChildIdentifier;
	CString l_oNewChildName = label;

	//ensure visualisation panel name is unique in this window
	for(uint32 i=0; i<l_pVisualisationWindow->getNbChildren(); i++)
	{
		l_pVisualisationWindow->getChildIdentifier(i, l_oChildIdentifier);
		if(m_rVisualisationTree.getVisualisationWidget(l_oChildIdentifier)->getName() == l_oNewChildName)
		{
			displayErrorDialog("Tab creation failed !", "An existing tab already uses this name. Please choose another name.");
			return false;
		}
	}

	//proceed with panel creation
	m_rVisualisationTree.addVisualisationWidget(
		l_oChildIdentifier,
		l_oNewChildName,
		EVisualisationWidget_VisualisationPanel,
		l_oVisualisationWindowIdentifier,
		l_pVisualisationWindow->getNbChildren(),
		OV_UndefinedIdentifier,
		1);

	m_rVisualisationTree.reloadTree();

	//refresh view
	::GtkTreeIter l_oIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, l_oChildIdentifier);
	refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oIter));

	return true;
}

void CDesignerVisualisation::askRenameVisualisationPanel()
{
	//show dialog
	CInputDialog id(m_sGuiFile.c_str(), &CDesignerVisualisation::rename_visualisation_panel_cb, this, "Rename tab", "Please enter new name of tab : ");

	id.run();
}

boolean CDesignerVisualisation::renameVisualisationPanel(const char* pNewVisualisationPanelName)
{
	//retrieve visualisation window
	::GtkTreeIter l_oIter;
	if(m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, (const char*)m_oActiveVisualisationWindowName, EVisualisationTreeNode_VisualisationWindow) == false)
	{
		displayErrorDialog("Tab renaming failed !", "Couldn't retrieve parent window.");
		return false;
	}
	CIdentifier l_oVisualisationWindowIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oVisualisationWindowIdentifier, EVisualisationTreeColumn_StringIdentifier);
	IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier);
	if(l_pVisualisationWindow == NULL)
	{
		displayErrorDialog("Tab renaming failed !", "Couldn't retrieve parent window.");
		return false;
	}

	//retrieve visualisation panel
	if(m_rVisualisationTree.findChildNodeFromParent(&l_oIter, (const char*)m_oActiveVisualisationPanelName, EVisualisationTreeNode_VisualisationPanel) == false)
	{
		displayErrorDialog("Tab renaming failed !", "Couldn't retrieve tab.");
		return false;
	}

	CIdentifier l_oVisualisationPanelIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oVisualisationPanelIdentifier, EVisualisationTreeColumn_StringIdentifier);
	IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationPanelIdentifier);
	if(l_pVisualisationWidget == NULL)
	{
		displayErrorDialog("tab renaming failed !", "Couldn't retrieve tab.");
		return false;
	}

	//if trying to set identical name, return
	CString l_oNewPanelName = pNewVisualisationPanelName;
	if(l_pVisualisationWidget->getName() == l_oNewPanelName)
		return true;

	//ensure visualisation panel name is unique in this window
	CIdentifier l_oChildIdentifier;
	for(uint32 i=0; i<l_pVisualisationWindow->getNbChildren(); i++)
	{
		l_pVisualisationWindow->getChildIdentifier(i, l_oChildIdentifier);
		if(m_rVisualisationTree.getVisualisationWidget(l_oChildIdentifier)->getName() == l_oNewPanelName)
		{
			displayErrorDialog("Tab renaming failed !", "An existing tab already uses this name. Please choose another name.");
			return false;
		}
	}

	l_pVisualisationWidget->setName(l_oNewPanelName);

	m_rVisualisationTree.reloadTree();

	//refresh view
	m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, l_oVisualisationPanelIdentifier);
	refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oIter));

	return true;
}

boolean CDesignerVisualisation::removeVisualisationPanel()
{
	//retrieve visualisation window
	::GtkTreeIter l_oIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, (const char*)m_oActiveVisualisationWindowName, EVisualisationTreeNode_VisualisationWindow);

	//retrieve visualisation panel
	m_rVisualisationTree.findChildNodeFromParent(&l_oIter, (const char*)m_oActiveVisualisationPanelName, EVisualisationTreeNode_VisualisationPanel);
	CIdentifier l_oVisualisationPanelIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oVisualisationPanelIdentifier, EVisualisationTreeColumn_StringIdentifier);

	//destroy hierarchy but only unaffect visualisation boxes (as opposed to destroying them)
	if(m_rVisualisationTree.destroyHierarchy(m_rVisualisationTree.getVisualisationWidget(l_oVisualisationPanelIdentifier)->getIdentifier(), false) == false)
	{
		displayErrorDialog("Tab removal failed !", "An error occured while destroying widget hierarchy.");
		return false;
	}

	m_rVisualisationTree.reloadTree();

	//refresh view
	refreshActiveVisualisation(NULL);

	return true;
}

boolean CDesignerVisualisation::removeVisualisationWidget()
{
	//retrieve widget
	::GtkTreeIter l_oIter;
	if(m_rVisualisationTree.getTreeSelection(m_pTreeView, &l_oIter) == false)
		return false;
	CIdentifier l_oIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oIdentifier, EVisualisationTreeColumn_StringIdentifier);
	return removeVisualisationWidget(l_oIdentifier);
}

//TODO : move this to CVisualisationTree?
boolean CDesignerVisualisation::removeVisualisationWidget(const CIdentifier& rIdentifier)
{
	IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(rIdentifier);
	if(l_pVisualisationWidget == NULL)
		return false;

	IVisualisationWidget* l_pParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_pVisualisationWidget->getParentIdentifier());

	//unparent or destroy widget
	uint32 l_ui32ChildIndex;
	m_rVisualisationTree.unparentVisualisationWidget(rIdentifier, l_ui32ChildIndex);
	if(l_pVisualisationWidget->getType() != EVisualisationWidget_VisualisationBox)
		m_rVisualisationTree.destroyHierarchy(rIdentifier, false);

	//reparent other child widget, if any
	if(l_pParentVisualisationWidget->getType() != EVisualisationWidget_VisualisationPanel)
	{
		//retrieve parent's other widget
		CIdentifier l_oOtherVisualisationWidgetIdentifier;
		l_pParentVisualisationWidget->getChildIdentifier(1-l_ui32ChildIndex, l_oOtherVisualisationWidgetIdentifier);

		//unparent parent
		uint32 l_ui32ParentIndex;
		CIdentifier l_oParentParentIdentifier = l_pParentVisualisationWidget->getParentIdentifier();
		m_rVisualisationTree.unparentVisualisationWidget(l_pParentVisualisationWidget->getIdentifier(), l_ui32ParentIndex);

		//reparent other widget to its grandparent
		m_rVisualisationTree.unparentVisualisationWidget(l_oOtherVisualisationWidgetIdentifier, l_ui32ChildIndex);
		m_rVisualisationTree.parentVisualisationWidget(l_oOtherVisualisationWidgetIdentifier, l_oParentParentIdentifier, l_ui32ParentIndex);

		//destroy parent
		m_rVisualisationTree.destroyHierarchy(l_pParentVisualisationWidget->getIdentifier(), false);
	}

	m_rVisualisationTree.reloadTree();

	//refresh view
	refreshActiveVisualisation(NULL);

	return true;
}

boolean CDesignerVisualisation::destroyVisualisationWidget(const CIdentifier& rIdentifier)
{
	boolean b = removeVisualisationWidget(rIdentifier);
	m_rVisualisationTree.destroyHierarchy(rIdentifier, true);
	return b;
}

//CALLBACKS
//---------

void CDesignerVisualisation::notebookPageSelectedCB(::GtkNotebook* pNotebook, guint pagenum)
{
	::GtkTreeIter l_oIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, (void*)pNotebook);
	CIdentifier l_oIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oIdentifier, EVisualisationTreeColumn_StringIdentifier);
	IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oIdentifier);
	if(l_pVisualisationWidget != NULL)
	{
		IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_pVisualisationWidget->getParentIdentifier());
		if(l_pVisualisationWindow != NULL)
		{
			l_pVisualisationWindow->getChildIdentifier(pagenum, l_oIdentifier);
			if(m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, l_oIdentifier) == true)
				refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oIter));
		}
	}
}

void CDesignerVisualisation::enableNotebookSignals(::GtkWidget* pNotebook, boolean b)
{
	if(b)
	{
		g_signal_connect(G_OBJECT(pNotebook), "switch-page", G_CALLBACK(notebook_page_switch_cb), this);
	}
	else
	{
		g_signal_handlers_disconnect_by_func(G_OBJECT(pNotebook), reinterpret_cast<void*>(G_CALLBACK(notebook_page_switch_cb)), this);
	}
}

void CDesignerVisualisation::notifyPositionPanedCB(::GtkWidget* pWidget)
{
	::GtkPaned* l_pPaned = GTK_PANED(pWidget);

	//return if handle pos was changed because parent window was resized
	int l_iPos = gtk_paned_get_position(l_pPaned);
	int l_iMaxPos = GTK_IS_VPANED(l_pPaned) ? l_pPaned->container.widget.allocation.height : l_pPaned->container.widget.allocation.width;
	int l_iHandleThickness = GTK_IS_VPANED(l_pPaned) ? l_pPaned->handle_pos.height : l_pPaned->handle_pos.width;

	if(l_iPos + l_iHandleThickness == l_iMaxPos)
		return;

	//look for widget in tree
	::GtkWidget* l_pTreeWidget = getTreeWidget(pWidget);
	::GtkTreeIter l_oIter;
	if(m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, l_pTreeWidget) == true)
	{
		CIdentifier l_oIdentifier;
		m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oIdentifier, EVisualisationTreeColumn_StringIdentifier);
		TAttributeHandler l_oAttributeHandler(*m_rVisualisationTree.getVisualisationWidget(l_oIdentifier));

		//store new position and max position
		l_oAttributeHandler.setAttributeValue(OVD_AttributeId_VisualisationWidget_DividerPosition, l_iPos);
		l_oAttributeHandler.setAttributeValue(OVD_AttributeId_VisualisationWidget_MaxDividerPosition, l_iMaxPos);
	}
}

void CDesignerVisualisation::enablePanedSignals(::GtkWidget* pPaned, boolean b)
{
	if(b)
		g_signal_connect(G_OBJECT(pPaned), "notify::position", G_CALLBACK(notify_position_paned_cb), this);
	else
		g_signal_handlers_disconnect_by_func(G_OBJECT(pPaned), reinterpret_cast<void*>(G_CALLBACK(notify_position_paned_cb)), this);
}

void CDesignerVisualisation::ask_new_visualisation_window_cb(gpointer pUserData, guint callback_action, ::GtkWidget* pWidget)
{
	static_cast<CDesignerVisualisation*>(pUserData)->askNewVisualisationWindow();
}

void CDesignerVisualisation::new_visualisation_window_cb(::GtkWidget* pWidget, gpointer pUserData)
{
	CInputDialog* l_pInputDialog = static_cast<CInputDialog*>(pUserData);

	if(l_pInputDialog->getUserData() != NULL)
		static_cast<CDesignerVisualisation*>(l_pInputDialog->getUserData())->newVisualisationWindow(l_pInputDialog->getEntry());
}

void CDesignerVisualisation::ask_rename_visualisation_window_cb(gpointer pUserData, guint callback_action, ::GtkWidget* pWidget)
{
	static_cast<CDesignerVisualisation*>(pUserData)->askRenameVisualisationWindow();
}

void CDesignerVisualisation::rename_visualisation_window_cb(::GtkWidget* pWidget, gpointer pUserData)
{
	CInputDialog* l_pInputDialog = static_cast<CInputDialog*>(pUserData);

	if(l_pInputDialog->getUserData() != NULL)
		static_cast<CDesignerVisualisation*>(l_pInputDialog->getUserData())->renameVisualisationWindow(l_pInputDialog->getEntry());
}

void CDesignerVisualisation::remove_visualisation_window_cb(gpointer pUserData, guint callback_action, ::GtkWidget* pWidget)
{
	static_cast<CDesignerVisualisation*>(pUserData)->removeVisualisationWindow();
}

void CDesignerVisualisation::ask_new_visualisation_panel_cb(gpointer pUserData, guint callback_action, ::GtkWidget* pWidget)
{
	static_cast<CDesignerVisualisation*>(pUserData)->askNewVisualisationPanel();
}

void CDesignerVisualisation::new_visualisation_panel_cb(::GtkWidget* pWidget, gpointer pUserData)
{
	CInputDialog* l_pInputDialog = static_cast<CInputDialog*>(pUserData);

	if(l_pInputDialog->getUserData() != NULL)
		static_cast<CDesignerVisualisation*>(l_pInputDialog->getUserData())->newVisualisationPanel(l_pInputDialog->getEntry());
}

void CDesignerVisualisation::ask_rename_visualisation_panel_cb(gpointer pUserData, guint callback_action, ::GtkWidget* pWidget)
{
	static_cast<CDesignerVisualisation*>(pUserData)->askRenameVisualisationPanel();
}

void CDesignerVisualisation::rename_visualisation_panel_cb(::GtkWidget* pWidget, gpointer pUserData)
{
	CInputDialog* l_pInputDialog = static_cast<CInputDialog*>(pUserData);

	if(l_pInputDialog->getUserData() != NULL)
		static_cast<CDesignerVisualisation*>(l_pInputDialog->getUserData())->renameVisualisationPanel(l_pInputDialog->getEntry());
}

void CDesignerVisualisation::remove_visualisation_panel_cb(gpointer pUserData, guint callback_action, ::GtkWidget* pWidget)
{
	static_cast<CDesignerVisualisation*>(pUserData)->removeVisualisationPanel();
}

void CDesignerVisualisation::remove_visualisation_widget_cb(gpointer pUserData, guint callback_action, ::GtkWidget* pWidget)
{
	static_cast<CDesignerVisualisation*>(pUserData)->removeVisualisationWidget();
}

void CDesignerVisualisation::visualisation_widget_key_press_event_cb(::GtkWidget* pWidget, GdkEventKey* pEvent, gpointer pUserData)
{
	static_cast<CDesignerVisualisation*>(pUserData)->visualisationWidgetKeyPressEventCB(pWidget, pEvent);
}

void CDesignerVisualisation::visualisationWidgetKeyPressEventCB(::GtkWidget*, GdkEventKey* pEventKey)
{
	//remove widget
	if(pEventKey->keyval==GDK_Delete || pEventKey->keyval==GDK_KP_Delete)
	{
		if(m_pHighlightedWidget != NULL)
		{
			::GtkTreeIter l_oIter;
			if(m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, getTreeWidget(m_pHighlightedWidget)) == true)
			{
				CIdentifier l_oIdentifier;
				m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oIdentifier, EVisualisationTreeColumn_StringIdentifier);
				removeVisualisationWidget(l_oIdentifier);
			}
		}
	}
}

gboolean CDesignerVisualisation::visualisation_widget_enter_notify_event_cb(::GtkWidget* pWidget, GdkEventCrossing* pEventCrossing, gpointer pUserData)
{
	static_cast<CDesignerVisualisation*>(pUserData)->visualisationWidgetEnterNotifyEventCB(pWidget, pEventCrossing);
	return FALSE;
}

void CDesignerVisualisation::visualisationWidgetEnterNotifyEventCB(::GtkWidget* pWidget, GdkEventCrossing* pEventCrossing)
{
	m_pHighlightedWidget = pWidget;
}

gboolean CDesignerVisualisation::visualisation_widget_leave_notify_event_cb(::GtkWidget* pWidget, GdkEventCrossing* pEventCrossing, gpointer pUserData)
{
	static_cast<CDesignerVisualisation*>(pUserData)->visualisationWidgetLeaveNotifyEventCB(pWidget, pEventCrossing);
	return FALSE;
}

void CDesignerVisualisation::visualisationWidgetLeaveNotifyEventCB(::GtkWidget* pWidget, GdkEventCrossing* pEventCrossing)
{
	m_pHighlightedWidget = NULL;
}

gboolean CDesignerVisualisation::button_release_cb(::GtkWidget* pWidget, GdkEventButton *pEvent, gpointer pUserData)
{
	static_cast<CDesignerVisualisation*>(pUserData)->buttonReleaseCB(pWidget, pEvent);

	return FALSE;
}

void CDesignerVisualisation::buttonReleaseCB(::GtkWidget* pWidget, GdkEventButton* pEvent)
{
	if(GTK_IS_TREE_VIEW(pWidget))
	{
		if(pEvent->button == 3) //right button
		{
			if(pEvent->type != GDK_BUTTON_PRESS)
			{
				::GtkTreeIter l_oIter;

				if(m_rVisualisationTree.getTreeSelection(m_pTreeView, &l_oIter) == false)
					return;

				unsigned long l_ulType = m_rVisualisationTree.getULongValueFromTreeIter(&l_oIter, EVisualisationTreeColumn_ULongNodeType);

				if(l_ulType == EVisualisationTreeNode_Unaffected)
				{
					gtk_menu_popup(GTK_MENU(gtk_item_factory_get_widget(m_pUnaffectedItemFactory, "<unaffected_main>")),NULL,NULL,NULL,NULL,pEvent->button,pEvent->time);
				}
				else if(l_ulType == EVisualisationTreeNode_VisualisationWindow)
				{
					gtk_menu_popup(GTK_MENU(gtk_item_factory_get_widget(m_pVisualisationWindowItemFactory, "<visualisation_window_main>")),NULL,NULL,NULL,NULL,pEvent->button,pEvent->time);
				}
				else if(l_ulType == EVisualisationTreeNode_VisualisationPanel)
				{
					gtk_menu_popup(GTK_MENU(gtk_item_factory_get_widget(m_pVisualisationPanelItemFactory, "<visualisation_panel_main>")),NULL,NULL,NULL,NULL,pEvent->button,pEvent->time);
				}
				else if(l_ulType == EVisualisationTreeNode_HorizontalSplit || l_ulType == EVisualisationTreeNode_VerticalSplit)
				{
					gtk_menu_popup(GTK_MENU(gtk_item_factory_get_widget(m_pSplitItemFactory, "<split_widget_main>")),NULL,NULL,NULL,NULL,pEvent->button,pEvent->time);
				}
				else if(l_ulType == EVisualisationTreeNode_VisualisationBox)
				{
					//ensure visualisation box is parented to a tab
					if(m_rVisualisationTree.findParentNode(&l_oIter, EVisualisationTreeNode_VisualisationPanel) == true)
						gtk_menu_popup(GTK_MENU(gtk_item_factory_get_widget(m_pVisualisationBoxItemFactory, "<visualisation_box_main>")),NULL,NULL,NULL,NULL,pEvent->button,pEvent->time);
				}
				else if(l_ulType == EVisualisationTreeNode_Undefined)
				{
					//ensure empty plugin is not parented to a panel (because an empty widget is always present in an empty panel)
					CIdentifier l_oIdentifier;
					m_rVisualisationTree.getIdentifierFromTreeIter(&l_oIter, l_oIdentifier, EVisualisationTreeColumn_StringIdentifier);
					IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oIdentifier);
					if(l_pVisualisationWidget != NULL)
					{
						IVisualisationWidget* l_pParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_pVisualisationWidget->getParentIdentifier());
						if(l_pParentVisualisationWidget != NULL)
						{
							if(l_pParentVisualisationWidget->getType() != EVisualisationWidget_VisualisationPanel)
							{
								gtk_menu_popup(GTK_MENU(gtk_item_factory_get_widget(m_pUndefinedItemFactory, "<undefined_widget_main>")),NULL,NULL,NULL,NULL,pEvent->button,pEvent->time);
							}
						}
					}
				}
			}
		}
	}
}

void CDesignerVisualisation::cursor_changed_cb(::GtkTreeView* pTreeView, gpointer pUserData)
{
	static_cast<CDesignerVisualisation*>(pUserData)->cursorChangedCB(pTreeView);
}

void CDesignerVisualisation::cursorChangedCB(::GtkTreeView* pTreeView)
{
	//retrieve selection
	::GtkTreeIter l_oSelectionIter;
	if(m_rVisualisationTree.getTreeSelection(pTreeView, &l_oSelectionIter) == false)
		return;

	//save active item
	if(m_rVisualisationTree.getULongValueFromTreeIter(&l_oSelectionIter, EVisualisationTreeColumn_ULongNodeType) == EVisualisationTreeNode_VisualisationBox)
	{
		char* l_pString = NULL;
		m_rVisualisationTree.getStringValueFromTreeIter(&l_oSelectionIter, l_pString, EVisualisationTreeColumn_StringName);
		m_oActiveVisualisationBoxName = l_pString;
	}

	::GtkTreeIter l_oVisualisationPanelIter = l_oSelectionIter;

	//if selection lies in a visualisation panel subtree, display this subtree
	if(m_rVisualisationTree.findParentNode(&l_oVisualisationPanelIter, EVisualisationTreeNode_VisualisationPanel) == true)
	{
		//get visualisation panel name
		char* l_pVisualisationPanelName = NULL;
		m_rVisualisationTree.getStringValueFromTreeIter(&l_oVisualisationPanelIter, l_pVisualisationPanelName, EVisualisationTreeColumn_StringName);

		//retrieve visualisation window that contains selection
		::GtkTreeIter l_oVisualisationWindowIter = l_oVisualisationPanelIter;
		if(m_rVisualisationTree.findParentNode(&l_oVisualisationWindowIter, EVisualisationTreeNode_VisualisationWindow) == true)
		{
			//get its name
			char* l_pVisualisationWindowName = NULL;
			m_rVisualisationTree.getStringValueFromTreeIter(&l_oVisualisationWindowIter, l_pVisualisationWindowName, EVisualisationTreeColumn_StringName);

			//set active visualisation
			setActiveVisualisation(l_pVisualisationWindowName, l_pVisualisationPanelName);
		}
	}
	else
	{
		::GtkTreeIter l_oVisualisationWindowIter = l_oSelectionIter;

		//if selection is a visualisation window, display it
		if(m_rVisualisationTree.findParentNode(&l_oVisualisationWindowIter, EVisualisationTreeNode_VisualisationWindow) == true)
		{
			//retrieve visualisation window
			CIdentifier l_oVisualisationWindowIdentifier;
			m_rVisualisationTree.getIdentifierFromTreeIter(&l_oVisualisationWindowIter, l_oVisualisationWindowIdentifier, EVisualisationTreeColumn_StringIdentifier);
			IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier);

			//if window has at least one panel
			if(l_pVisualisationWindow->getNbChildren() > 0)
			{
				//retrieve first panel
				CIdentifier l_oVisualisationPanelIdentifier;
				l_pVisualisationWindow->getChildIdentifier(0, l_oVisualisationPanelIdentifier);
				m_rVisualisationTree.findChildNodeFromParent(&l_oVisualisationPanelIter, l_oVisualisationPanelIdentifier);

				//retrieve notebook
				void* l_pNotebook = NULL;
				m_rVisualisationTree.getPointerValueFromTreeIter(&l_oVisualisationPanelIter, l_pNotebook, EVisualisationTreeColumn_PointerWidget);

				//get label of its active tab
				::GtkWidget* l_pCurrentPageLabel =
					gtk_notebook_get_tab_label(GTK_NOTEBOOK(l_pNotebook),
						gtk_notebook_get_nth_page(GTK_NOTEBOOK(l_pNotebook),
							gtk_notebook_get_current_page(GTK_NOTEBOOK(l_pNotebook))));

				//set active visualisation
				if(l_pCurrentPageLabel != NULL)
					setActiveVisualisation((const char*)l_pVisualisationWindow->getName(), gtk_label_get_text(GTK_LABEL(l_pCurrentPageLabel)));
				else
					setActiveVisualisation((const char*)l_pVisualisationWindow->getName(), NULL);
			}
			else //window has no panels
			{
				setActiveVisualisation((const char*)l_pVisualisationWindow->getName(), NULL);
			}
		}
		else
		{
			//refresh active visualisation (::GtkWidgets may have changed if tree was reloaded)
			setActiveVisualisation(m_oActiveVisualisationWindowName, m_oActiveVisualisationPanelName);
		}
	}
}

void CDesignerVisualisation::drag_data_get_from_tree_cb(::GtkWidget* pSrcWidget, ::GdkDragContext* pDragContex, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT, gpointer pData)
{
	static_cast<CDesignerVisualisation*>(pData)->dragDataGetFromTreeCB(pSrcWidget, pSelectionData);
}

void CDesignerVisualisation::dragDataGetFromTreeCB(::GtkWidget* pSrcWidget, ::GtkSelectionData* pSelectionData)
{
	char l_sString[1024];
	sprintf(l_sString, "%i", (int)pSrcWidget);
	gtk_selection_data_set_text(pSelectionData, l_sString, strlen(l_sString));
}

void CDesignerVisualisation::drag_data_get_from_widget_cb(::GtkWidget* pSrcWidget, GdkDragContext* pDragContext, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiTime, gpointer pData)
{
	static_cast<CDesignerVisualisation*>(pData)->dragDataGetFromWidgetCB(pSrcWidget, pSelectionData);
}

void CDesignerVisualisation::dragDataGetFromWidgetCB(::GtkWidget* pSrcWidget, ::GtkSelectionData* pSelectionData)
{
	char l_sString[1024];
	sprintf(l_sString, "%i", (int)pSrcWidget);
	gtk_selection_data_set_text(pSelectionData, l_sString, strlen(l_sString));
}

void CDesignerVisualisation::drag_data_received_in_widget_cb(::GtkWidget* dstWidget, GdkDragContext* pDragContext,gint iX,gint iY,::GtkSelectionData* pSelectionData,guint uiInfo,guint uiTime,gpointer pData)
{
	static_cast<CDesignerVisualisation*>(pData)->dragDataReceivedInWidgetCB(dstWidget, pSelectionData);
}

void CDesignerVisualisation::dragDataReceivedInWidgetCB(::GtkWidget* pDstWidget, ::GtkSelectionData* pSelectionData)
{
	void* l_pSrcWidget = NULL;
	sscanf((const char*)gtk_selection_data_get_text(pSelectionData), "%i", reinterpret_cast<int*>(&l_pSrcWidget));
	::GtkTreeIter l_oSrcIter;

	//retrieve source widget iterator
	if(GTK_IS_TREE_VIEW(l_pSrcWidget))
	{
		//ensure dragged widget is a visualisation box
		if(m_rVisualisationTree.findChildNodeFromRoot(&l_oSrcIter, (const char*)m_oActiveVisualisationBoxName, EVisualisationTreeNode_VisualisationBox) == false)
			return;
	}
	else if(GTK_IS_BUTTON(l_pSrcWidget))
	{
		if(l_pSrcWidget == pDstWidget)
			return;
		m_rVisualisationTree.findChildNodeFromRoot(&l_oSrcIter, getTreeWidget(GTK_WIDGET(l_pSrcWidget)));
	}
	else
	{
		return;
	}

	//retrieve src widget identifier
	CIdentifier l_oSrcIdentifier = OV_UndefinedIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oSrcIter, l_oSrcIdentifier, EVisualisationTreeColumn_StringIdentifier);

	//retrieve dest widget type
	::GtkTreeIter l_oDstIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oDstIter, getTreeWidget(pDstWidget));

	//if src widget is unaffected or if dest widget is a visualisation box, perform the drop operation directly
	if(m_rVisualisationTree.getVisualisationWidget(l_oSrcIdentifier)->getParentIdentifier() == OV_UndefinedIdentifier ||
		m_rVisualisationTree.getULongValueFromTreeIter(&l_oDstIter, EVisualisationTreeColumn_ULongNodeType) == EVisualisationTreeNode_VisualisationBox)
	{
		m_rVisualisationTree.dragDataReceivedInWidgetCB(l_oSrcIdentifier, pDstWidget);
	}
	else //dest widget is a dummy : unaffect src widget and simplify the tree before performing the drop operation
	{
		//save dest widget identifier
		CIdentifier l_oDstIdentifier;
		m_rVisualisationTree.getIdentifierFromTreeIter(&l_oDstIter, l_oDstIdentifier, EVisualisationTreeColumn_StringIdentifier);

		//unaffect src widget, so that tree is simplified
		removeVisualisationWidget(l_oSrcIdentifier);

		//then drop it
		m_rVisualisationTree.findChildNodeFromRoot(&l_oDstIter, l_oDstIdentifier);
		void* l_pNewDstTreeWidget = NULL;
		m_rVisualisationTree.getPointerValueFromTreeIter(&l_oDstIter, l_pNewDstTreeWidget, EVisualisationTreeColumn_PointerWidget);
		m_rVisualisationTree.dragDataReceivedInWidgetCB(l_oSrcIdentifier, getVisualisationWidget(GTK_WIDGET(l_pNewDstTreeWidget)));
	}

	//refresh view
	::GtkTreeIter l_oDraggedIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oDraggedIter, l_oSrcIdentifier);
	refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oDraggedIter));
}

void CDesignerVisualisation::drag_data_received_in_event_box_cb(::GtkWidget* pDstWidget,GdkDragContext*,gint,gint,::GtkSelectionData* pSelectionData,guint,guint,gpointer pData)
{
	char buf[10];
	void* pDesignerVisualisation = NULL;
	sscanf((const char*)pData, "%i %s", reinterpret_cast<int*>(&pDesignerVisualisation), buf);

	EDragDataLocation l_oLocation;
	if(strcmp(buf, "left") == 0)
		l_oLocation = EDragData_Left;
	else if(strcmp(buf, "right")==0)
		l_oLocation = EDragData_Right;
	else if(strcmp(buf, "top")==0)
		l_oLocation = EDragData_Top;
	else
		l_oLocation = EDragData_Bottom;

	((CDesignerVisualisation*)pDesignerVisualisation)->dragDataReceivedInEventBoxCB(pDstWidget, pSelectionData, l_oLocation);
}

void CDesignerVisualisation::dragDataReceivedInEventBoxCB(::GtkWidget* pDstWidget, ::GtkSelectionData* pSelectionData, EDragDataLocation l_oLocation)
{
	void* l_pSrcWidget = NULL;
	sscanf((const char*)gtk_selection_data_get_text(pSelectionData), "%i", reinterpret_cast<int*>(&l_pSrcWidget));
	::GtkTreeIter l_oSrcIter;

	//get iterator to src widget
	if(GTK_IS_TREE_VIEW(l_pSrcWidget))
	{
		if(m_rVisualisationTree.findChildNodeFromRoot(&l_oSrcIter, (const char*)m_oActiveVisualisationBoxName, EVisualisationTreeNode_VisualisationBox) == false)
			return;
		//get actual src widget (item being dropped) and ensure it isn't being dropped in its own table
		m_rVisualisationTree.getPointerValueFromTreeIter(&l_oSrcIter, l_pSrcWidget, EVisualisationTreeColumn_PointerWidget);
		if(l_pSrcWidget == gtk_widget_get_parent(pDstWidget))
			return;
	}
	else if(GTK_IS_BUTTON(l_pSrcWidget))
	{
		//ensure src widget isn't being dropped in its own table
		if(gtk_widget_get_parent(GTK_WIDGET(l_pSrcWidget)) == gtk_widget_get_parent(pDstWidget))
			return;
		m_rVisualisationTree.findChildNodeFromRoot(&l_oSrcIter, getTreeWidget(GTK_WIDGET(l_pSrcWidget)));
	}
	else
	{
		return;
	}

	//ensure src widget is a visualisation box
	if(m_rVisualisationTree.getULongValueFromTreeIter(&l_oSrcIter, EVisualisationTreeColumn_ULongNodeType) != EVisualisationTreeNode_VisualisationBox)
		return;

	//retrieve src widget identifier
	CIdentifier l_oSrcIdentifier;
	m_rVisualisationTree.getIdentifierFromTreeIter(&l_oSrcIter, l_oSrcIdentifier, EVisualisationTreeColumn_StringIdentifier);

	//if widget is unaffected, just drag n drop it
	::GtkTreeIter l_oUnaffectedIter = l_oSrcIter;
	if(m_rVisualisationTree.findParentNode(&l_oUnaffectedIter, EVisualisationTreeNode_Unaffected) == true)
	{
		m_rVisualisationTree.dragDataReceivedOutsideWidgetCB(l_oSrcIdentifier, pDstWidget, l_oLocation);
	}
	else
	{
		//save dest widget identifier
		::GtkTreeIter l_oDstIter;
		m_rVisualisationTree.findChildNodeFromRoot(&l_oDstIter, getTreeWidget(pDstWidget));
		CIdentifier l_oDstIdentifier;
		m_rVisualisationTree.getIdentifierFromTreeIter(&l_oDstIter, l_oDstIdentifier, EVisualisationTreeColumn_StringIdentifier);

		//if dest widget is src widget's parent (paned widget), drop src widget in corresponding event box of parent's other child
		//(otherwise, DND will fail due to parent's removal during tree simplification process)
		IVisualisationWidget* l_pSrcVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oSrcIdentifier);
		if(l_pSrcVisualisationWidget->getParentIdentifier() == l_oDstIdentifier)
		{
			IVisualisationWidget* l_pSrcParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_pSrcVisualisationWidget->getParentIdentifier());
			l_pSrcParentVisualisationWidget->getChildIdentifier(0, l_oDstIdentifier);
			if(l_oSrcIdentifier == l_oDstIdentifier)
				l_pSrcParentVisualisationWidget->getChildIdentifier(1, l_oDstIdentifier);
		}

		//unaffect src widget, so that tree is simplified
		removeVisualisationWidget(l_oSrcIdentifier);

		//then drop it
		m_rVisualisationTree.findChildNodeFromRoot(&l_oDstIter, l_oDstIdentifier);
		void* l_pNewDstTreeWidget = NULL;
		m_rVisualisationTree.getPointerValueFromTreeIter(&l_oDstIter, l_pNewDstTreeWidget, EVisualisationTreeColumn_PointerWidget);
		m_rVisualisationTree.dragDataReceivedOutsideWidgetCB(l_oSrcIdentifier, getVisualisationWidget(GTK_WIDGET(l_pNewDstTreeWidget)), l_oLocation);
	}

	//refresh view
	::GtkTreeIter l_oDraggedIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oDraggedIter, l_oSrcIdentifier);
	refreshActiveVisualisation(m_rVisualisationTree.getTreePath(&l_oDraggedIter));
}
