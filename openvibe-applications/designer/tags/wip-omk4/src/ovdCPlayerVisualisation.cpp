#include "ovd_base.h"
#include "ovdTAttributeHandler.h"
#include "ovdCPlayerVisualisation.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

#define G_CALLBACK2(x) G_CALLBACK_AUTOCAST(G_CALLBACK(x))

class G_CALLBACK_AUTOCAST
{
public:

	typedef union
	{
		GCallback fp;;
		gpointer p;
	} _data;

	_data m_pData;

	G_CALLBACK_AUTOCAST(gpointer p) { m_pData.p=p; }
	G_CALLBACK_AUTOCAST(GCallback fp) { m_pData.fp=fp; }

	operator gpointer (void) { return m_pData.p; }
	operator GCallback (void) { return m_pData.fp; }

private:

	G_CALLBACK_AUTOCAST(void);
};

static ::GtkTargetEntry targets [] =
{
	{ "STRING", 0, 0 },
	{ "text/plain", 0, 0 },
};

static void dummy_callback(::GtkWidget*)
{
}

CPlayerVisualisation::CPlayerVisualisation(const IKernelContext& rKernelContext, const IScenario& rScenario, IVisualisationTree& rVisualisationTree) :
	m_rKernelContext(rKernelContext),
	m_rScenario(rScenario),
	m_rVisualisationTree(rVisualisationTree),
	m_pActiveToolbarButton(NULL)
{
}

CPlayerVisualisation::~CPlayerVisualisation(void)
{
	release();

	std::map<CIdentifier, ::GtkWindow*>::iterator it = m_mVisualisationWindow.begin();

	while(it != m_mVisualisationWindow.end())
	{
		g_signal_handlers_disconnect_by_func(G_OBJECT(it->second), G_CALLBACK2(CPlayerVisualisation::configure_event_cb), this);
		gtk_widget_destroy(GTK_WIDGET(it->second));
		it++;
	}

	m_rVisualisationTree.setTreeViewCB(NULL);
}

void CPlayerVisualisation::init(void)
{
	//delete ::GtkWidgets
	m_mVisualisationWidget.clear();

	//delete ::GtkWindows
	m_mVisualisationWindow.clear();

	m_pActiveToolbarButton = NULL;

	//register towards tree store
	m_rVisualisationTree.setTreeViewCB(this);

	//rebuild widgets
	m_rVisualisationTree.reloadTree();
}

void CPlayerVisualisation::release(void)
{
}

::GtkWidget* CPlayerVisualisation::loadTreeWidget(IVisualisationWidget* pVisualisationWidget)
{
	::GtkWidget* l_pWidget = NULL;

	if(pVisualisationWidget->getType() == EVisualisationWidget_Undefined)
	{
		l_pWidget = gtk_button_new();
		gtk_button_set_label(GTK_BUTTON(l_pWidget), (const char*)pVisualisationWidget->getName());
	}
	else if(pVisualisationWidget->getType() == EVisualisationWidget_VisualisationPanel)
	{
		l_pWidget = gtk_notebook_new();
	}
	else if(pVisualisationWidget->getType() == EVisualisationWidget_VisualisationBox)
	{
		//widget will be inserted at player start time
	}
	else if(pVisualisationWidget->getType() == EVisualisationWidget_HorizontalSplit || pVisualisationWidget->getType() == EVisualisationWidget_VerticalSplit)
	{
		l_pWidget = (pVisualisationWidget->getType() == EVisualisationWidget_HorizontalSplit) ? gtk_hpaned_new() : gtk_vpaned_new();
		m_mVisualisationWidget[l_pWidget] = pVisualisationWidget->getIdentifier();

		//retrieve its attributes
		TAttributeHandler l_oAttributeHandler(*pVisualisationWidget);
		int l_i32HandlePos = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWidget_DividerPosition);
		gtk_paned_set_position(GTK_PANED(l_pWidget), l_i32HandlePos);
	}
	else if(pVisualisationWidget->getType() == EVisualisationWidget_VisualisationWindow)
	{
		//create this window only if it contains at least one visualisation box
		CIdentifier l_oIdentifier = OV_UndefinedIdentifier;
		boolean l_bCreateWindow = false;

		//for all visualisation boxes
		while(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oIdentifier, EVisualisationWidget_VisualisationBox) == true)
		{
			//retrieve window containing current visualisation box
			CIdentifier l_oParentIdentifier;
			IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oIdentifier);
			while(l_pVisualisationWidget->getParentIdentifier() != OV_UndefinedIdentifier)
			{
				l_oParentIdentifier = l_pVisualisationWidget->getParentIdentifier();
				l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oParentIdentifier);
			}

			//if current box is parented to window passed in parameter, break and create it
			if(m_rVisualisationTree.getVisualisationWidget(l_oParentIdentifier) == pVisualisationWidget)
			{
				l_bCreateWindow = true;
				break;
			}
		}

		if(l_bCreateWindow == true)
		{
			//create new top level window
			l_pWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			m_mVisualisationWindow[pVisualisationWidget->getIdentifier()] = GTK_WINDOW(l_pWidget);

			//retrieve its size
			TAttributeHandler l_oAttributeHandler(*pVisualisationWidget);
			gtk_window_set_default_size(
				GTK_WINDOW(l_pWidget),
				l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWindow_Width),
				l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWindow_Height));

			//set its title
			gtk_window_set_title(GTK_WINDOW(l_pWidget), (const char*)pVisualisationWidget->getName());

			//FIXME wrong spelling (-)
			gtk_signal_connect(GTK_OBJECT(l_pWidget), "configure_event", G_CALLBACK(configure_event_cb), this);
			//FIXME wrong spelling (-)
			g_signal_connect(l_pWidget, "delete_event", G_CALLBACK(dummy_callback), NULL);
		}
	}

#if 0
	//*** moved to setWidgets() ***
	//resize widgets once they are allocated : this is the case when they are shown on an expose event
	//FIXME : perform resizing only once (when it is done as many times as there are widgets in the tree here)
	if(l_pWidget != NULL)
		gtk_signal_connect(GTK_OBJECT(l_pWidget), "expose-event", G_CALLBACK(widget_expose_event_cb), this);
#endif

	return l_pWidget;
}

boolean CPlayerVisualisation::setWidgets(const CString& rVisualisationBoxName, ::GtkWidget* pWidget, ::GtkWidget* pToolbarWidget)
{
	if(pWidget == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning
			<< "CPlayerVisualisation::setWidgets FAILED : widget pointer is NULL for plugin "
			<< rVisualisationBoxName <<"\n";
		return false;
	}

	//unparent top widget, if necessary
	::GtkWidget* l_pWidgetParent = gtk_widget_get_parent(pWidget);
	if(GTK_IS_CONTAINER(l_pWidgetParent))
	{
		gtk_object_ref(GTK_OBJECT(pWidget));
		gtk_container_remove(GTK_CONTAINER(l_pWidgetParent), pWidget);
	}

	//retrieve iterator to visualisation box
	::GtkTreeIter l_oVisualisationBoxIter;
	if(m_rVisualisationTree.findChildNodeFromRoot(&l_oVisualisationBoxIter, (const char*)rVisualisationBoxName, EVisualisationTreeNode_VisualisationBox) == true)
	{
		CIdentifier l_oVisualisationBoxIdentifier;
		m_rVisualisationTree.getIdentifierFromTreeIter(&l_oVisualisationBoxIter, l_oVisualisationBoxIdentifier, EVisualisationTreeColumn_StringIdentifier);
		IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationBoxIdentifier);
		if(l_pVisualisationWidget != NULL)
		{
			//put visualisation box in a container, with an icon and a label on top
			::GtkBox* l_pVBox = GTK_BOX(gtk_vbox_new(FALSE, 0));
			//gtk_widget_set_size_request(GTK_WIDGET(l_pVBox), 0, 0);

			::GtkToggleButton* l_pButton = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
			{
				//horizontal container : icon + label
				::GtkBox* l_pHBox = GTK_BOX(gtk_hbox_new(FALSE, 0));
				//gtk_widget_set_size_request(GTK_WIDGET(l_pHBox), 0, 0);

				//retrieve icon name
				::GtkTreeIter l_oIter;
				char* l_pIconString = NULL;
				if(m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, (const char*)rVisualisationBoxName, EVisualisationTreeNode_VisualisationBox) == true)
				{
					m_rVisualisationTree.getStringValueFromTreeIter(&l_oIter, l_pIconString, EVisualisationTreeColumn_StringStockIcon);
				}

				//create icon
				::GtkWidget* l_pIcon = gtk_image_new_from_stock(l_pIconString != NULL ? l_pIconString : GTK_STOCK_EXECUTE, GTK_ICON_SIZE_BUTTON);
				//gtk_widget_set_size_request(l_pIcon, 0, 0);
				gtk_box_pack_start(l_pHBox, l_pIcon, TRUE, TRUE, 0);

				//create label
				::GtkWidget* l_pLabel = gtk_label_new((const char*)rVisualisationBoxName);
				//gtk_widget_set_size_request(l_pLabel, 0, 0);
				gtk_box_pack_start(l_pHBox, l_pLabel, TRUE, TRUE, 0);

				//add box to button
				gtk_container_add(GTK_CONTAINER(l_pButton), GTK_WIDGET(l_pHBox));
			}

			g_signal_connect(G_OBJECT(l_pButton), "toggled", G_CALLBACK2(toolbar_button_toggled_cb), this);

			//set up button as drag destination
			gtk_drag_dest_set(GTK_WIDGET(l_pButton), GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
			g_signal_connect(G_OBJECT(l_pButton), "drag_data_received", G_CALLBACK(drag_data_received_in_widget_cb), this);

			//set up button as drag source as well
			gtk_drag_source_set(GTK_WIDGET(l_pButton), GDK_BUTTON1_MASK, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
			g_signal_connect(G_OBJECT(l_pButton), "drag_data_get", G_CALLBACK(drag_data_get_from_widget_cb), this);

			//associate button to its toolbar
			m_mVisualisationWidgetToolbar[l_pButton] = pToolbarWidget;
			//associate widget to its identifier
			m_mVisualisationWidget[pWidget] = l_pVisualisationWidget->getIdentifier();

			//catch delete events
			g_signal_connect(G_OBJECT(pToolbarWidget), "delete-event", G_CALLBACK(toolbar_delete_event_cb), this);

			//vertical container : button on top, visualisation box below
			gtk_box_pack_start(l_pVBox, GTK_WIDGET(l_pButton), FALSE, TRUE, 0);
			gtk_box_pack_start(l_pVBox, pWidget, TRUE, TRUE, 0);

			//if visualisation box is unaffected, open it in its own window
			if(l_pVisualisationWidget->getParentIdentifier() == OV_UndefinedIdentifier)
			{
				::GtkWidget* l_pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
				m_mVisualisationWindow[l_pVisualisationWidget->getIdentifier()] = GTK_WINDOW(l_pWindow);
				//FIXME
				gtk_window_set_default_size(GTK_WINDOW(l_pWindow), 300, 300);
				gtk_window_set_title(GTK_WINDOW(l_pWindow), (const char*)l_pVisualisationWidget->getName());
				gtk_container_add(GTK_CONTAINER(l_pWindow), (::GtkWidget*)l_pVBox);
				//prevent user from closing this window
				g_signal_connect(l_pWindow, "delete_event", G_CALLBACK(dummy_callback), NULL);
				gtk_widget_show_all(l_pWindow);
				return true;
			}
			else //retrieve parent widget in which to insert VBox
			{
				::GtkTreeIter l_oParentIter;
				if(m_rVisualisationTree.findChildNodeFromRoot(&l_oParentIter, l_pVisualisationWidget->getParentIdentifier()) == true)
				{
					void* l_pParentWidget = NULL;
					m_rVisualisationTree.getPointerValueFromTreeIter(&l_oParentIter, l_pParentWidget, EVisualisationTreeColumn_PointerWidget);
					CIdentifier l_oParentIdentifier;
					m_rVisualisationTree.getIdentifierFromTreeIter(&l_oParentIter, l_oParentIdentifier, EVisualisationTreeColumn_StringIdentifier);

					//box is to be parented to a paned widget
					if(GTK_IS_PANED(l_pParentWidget))
					{
						//retrieve index at which to insert child
						IVisualisationWidget* l_pParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oParentIdentifier);
						uint32 l_oVisualisationBoxIndex;
						l_pParentVisualisationWidget->getChildIndex(l_oVisualisationBoxIdentifier, l_oVisualisationBoxIndex);
						//insert visualisation box in paned
						if(l_oVisualisationBoxIndex == 0)
							gtk_paned_pack1(GTK_PANED(l_pParentWidget), GTK_WIDGET(l_pVBox), TRUE, TRUE);
						else
							gtk_paned_pack2(GTK_PANED(l_pParentWidget), GTK_WIDGET(l_pVBox), TRUE, TRUE);
					}
					else if(GTK_IS_NOTEBOOK(l_pParentWidget)) //box to be added to a notebook page
					{
						//retrieve notebook page index
						IVisualisationWidget* l_pParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oParentIdentifier);
						IVisualisationWidget* l_pParentVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_pParentVisualisationWidget->getParentIdentifier());
						uint32 l_oPanelIndex;
						l_pParentVisualisationWindow->getChildIndex(l_pParentVisualisationWidget->getIdentifier(), l_oPanelIndex);

						//insert visualisation box in notebook
						//FIXME : page doesn't exist when a NULL widget was inserted at page creation time
						gtk_notebook_remove_page(GTK_NOTEBOOK(l_pParentWidget), l_oPanelIndex);
						gtk_notebook_insert_page(
							GTK_NOTEBOOK(l_pParentWidget),
							GTK_WIDGET(l_pVBox),
							gtk_label_new((const char*)l_pParentVisualisationWidget->getName()),
							l_oPanelIndex);
					}

					//resize widgets once they are allocated : this is the case when they are shown on an expose event
					//FIXME : perform resizing only once (when it is done as many times as there are widgets in the tree here)
					if(l_pParentWidget != NULL)
					{
						gtk_signal_connect(GTK_OBJECT(l_pParentWidget), "expose-event", G_CALLBACK(widget_expose_event_cb), this);
					}

					gtk_widget_show_all(GTK_WIDGET(l_pVBox));
					return true;
				}
			}
		}
	}

	return false;
}

//called upon Player start
void CPlayerVisualisation::showTopLevelWindows(void)
{
	map<CIdentifier, ::GtkWindow*>::const_iterator it = m_mVisualisationWindow.begin();

	while(it != m_mVisualisationWindow.end())
	{
		gtk_widget_show_all(GTK_WIDGET(it->second));
		it++;
	}
}

//called upon Player stop
void CPlayerVisualisation::hideTopLevelWindows(void)
{
	map<CIdentifier, ::GtkWindow*>::const_iterator it = m_mVisualisationWindow.begin();

	while(it != m_mVisualisationWindow.end())
	{
		gtk_widget_hide_all(GTK_WIDGET(it->second));
		it++;
	}

	if(m_pActiveToolbarButton != NULL)
	{
		gtk_widget_hide_all(m_mVisualisationWidgetToolbar[m_pActiveToolbarButton]);
		m_pActiveToolbarButton = NULL;
	}
}

//event generated whenever window changes size, including when it is first created
gboolean CPlayerVisualisation::configure_event_cb(::GtkWidget* widget, GdkEventConfigure* event, gpointer user_data)
{
	//paned positions aren't to be saved, they are to be read once only
	g_signal_handlers_disconnect_by_func(G_OBJECT(widget), G_CALLBACK2(CPlayerVisualisation::configure_event_cb), user_data);

	if(GTK_IS_CONTAINER(widget))
	{
		static_cast<CPlayerVisualisation*>(user_data)->resizeCB(GTK_CONTAINER(widget));
	}

	return FALSE;
}

gboolean CPlayerVisualisation::widget_expose_event_cb(::GtkWidget* widget, GdkEventExpose* event, gpointer user_data)
{
	g_signal_handlers_disconnect_by_func(G_OBJECT(widget), G_CALLBACK2(CPlayerVisualisation::widget_expose_event_cb), user_data);
/*
	//retrieve topmost widget
	while(gtk_widget_get_parent(widget) != NULL && GTK_IS_CONTAINER(gtk_widget_get_parent(widget)))
		widget = gtk_widget_get_parent(widget);
*/
	if(GTK_IS_CONTAINER(widget))
	{
		static_cast<CPlayerVisualisation*>(user_data)->resizeCB(GTK_CONTAINER(widget));
	}

	return FALSE;
}

void CPlayerVisualisation::resizeCB(::GtkContainer* container)
{
	if(GTK_IS_WINDOW(container))
	{
		gpointer data = g_list_first(gtk_container_get_children(container))->data;

		if(GTK_IS_CONTAINER(data))
		{
			resizeCB(GTK_CONTAINER(data));
		}
	}
	else if(GTK_IS_NOTEBOOK(container))
	{
		::GtkNotebook* l_pNotebook = GTK_NOTEBOOK(container);

		for(int i=0; i<gtk_notebook_get_n_pages(l_pNotebook); i++)
		{
			::GtkWidget* l_pWidget = gtk_notebook_get_nth_page(l_pNotebook, i);
			if(GTK_IS_CONTAINER(l_pWidget))
			{
				resizeCB(GTK_CONTAINER(l_pWidget));
			}
		}
	}
	else if(GTK_IS_PANED(container))
	{
		::GtkPaned* l_pPaned = GTK_PANED(container);
		CIdentifier& l_oPanedIdentifier = m_mVisualisationWidget[GTK_WIDGET(l_pPaned)];
		IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oPanedIdentifier);

		//retrieve its attributes
		TAttributeHandler l_oAttributeHandler(*l_pVisualisationWidget);
		int l_i32HandlePos = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWidget_DividerPosition);
		int l_i32MaxHandlePos = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWidget_MaxDividerPosition);

		if(l_i32MaxHandlePos > 0)
		{
			//retrieve current maximum handle position
			int l_i32CurrentMaxHandlePos = GTK_IS_VPANED(l_pPaned) ? l_pPaned->container.widget.allocation.height : l_pPaned->container.widget.allocation.width;

			//set new paned handle position
			gtk_paned_set_position(l_pPaned, l_i32HandlePos * l_i32CurrentMaxHandlePos / l_i32MaxHandlePos);
		}

		//go down each child
		::GtkWidget* l_pChild = gtk_paned_get_child1(l_pPaned);
		if(GTK_IS_CONTAINER(l_pChild))
		{
			resizeCB(GTK_CONTAINER(l_pChild));
		}

		l_pChild = gtk_paned_get_child2(l_pPaned);
		if(GTK_IS_CONTAINER(l_pChild))
		{
			resizeCB(GTK_CONTAINER(l_pChild));
		}
	}
}

void CPlayerVisualisation::drag_data_get_from_widget_cb(::GtkWidget* pSrcWidget, GdkDragContext* pDragContext, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiTime, gpointer pData)
{
	char l_sString[1024];
	sprintf(l_sString, "%i", (int)pSrcWidget);
	gtk_selection_data_set_text(pSelectionData, l_sString, strlen(l_sString));
}

void CPlayerVisualisation::drag_data_received_in_widget_cb(::GtkWidget* pDstWidget, GdkDragContext* pDragContext,gint iX,gint iY,::GtkSelectionData* pSelectionData,guint uiInfo,guint uiTime,gpointer pData)
{
	void* l_pSrcWidget = NULL;
	sscanf((const char*)gtk_selection_data_get_text(pSelectionData), "%i", reinterpret_cast<int*>(&l_pSrcWidget));

	//retrieve source box and parent widgets
	::GtkWidget* l_pSrcBoxWidget = NULL;
	do
	{
		l_pSrcBoxWidget = gtk_widget_get_parent(GTK_WIDGET(l_pSrcWidget));
	}
	while(l_pSrcBoxWidget!=NULL && !GTK_IS_VBOX(l_pSrcBoxWidget));

	if(l_pSrcBoxWidget == NULL)
	{
		return;
	}

	::GtkWidget* l_pSrcParentWidget = gtk_widget_get_parent(l_pSrcBoxWidget);

	if(l_pSrcParentWidget == NULL)
	{
		return;
	}

	//retrieve dest box and parent widgets
	::GtkWidget* l_pDstBoxWidget = NULL;
	do
	{
		l_pDstBoxWidget = gtk_widget_get_parent(pDstWidget);
	}
	while(l_pDstBoxWidget!=NULL && !GTK_IS_VBOX(l_pDstBoxWidget));

	if(l_pDstBoxWidget == NULL)
	{
		return;
	}

	::GtkWidget* l_pDstParentWidget = gtk_widget_get_parent(l_pDstBoxWidget);

	if(l_pDstParentWidget == NULL)
	{
		return;
	}

	//remove src box from parent
	int l_i32SrcIndex;
	::GtkWidget* l_pSrcTabLabel=NULL;

	if(GTK_IS_WINDOW(l_pSrcParentWidget))
	{
		l_i32SrcIndex = 0;
	}
	else if(GTK_IS_NOTEBOOK(l_pSrcParentWidget))
	{
		l_i32SrcIndex = gtk_notebook_page_num(GTK_NOTEBOOK(l_pSrcParentWidget), l_pSrcBoxWidget);
		l_pSrcTabLabel = gtk_label_new(gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(l_pSrcParentWidget), l_pSrcBoxWidget));
	}
	else if(GTK_IS_PANED(l_pSrcParentWidget))
	{
		l_i32SrcIndex = ((::GtkPaned*)l_pSrcParentWidget)->child1 == l_pSrcBoxWidget ? 1 : 2;
	}
	else
	{
		return;
	}

	//remove dst box from parent
	int l_i32DstIndex;
	::GtkWidget* l_pDstTabLabel=NULL;
	if(GTK_IS_WINDOW(l_pDstParentWidget))
	{
		l_i32DstIndex = 0;
	}
	else if(GTK_IS_NOTEBOOK(l_pDstParentWidget))
	{
		l_i32DstIndex = gtk_notebook_page_num(GTK_NOTEBOOK(l_pDstParentWidget), l_pDstBoxWidget);
		l_pDstTabLabel = gtk_label_new(gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(l_pDstParentWidget), l_pDstBoxWidget));
	}
	else if(GTK_IS_PANED(l_pDstParentWidget))
	{
		l_i32DstIndex = ((::GtkPaned*)l_pDstParentWidget)->child1 == l_pDstBoxWidget ? 1 : 2;
	}
	else
	{
		return;
	}

	gtk_object_ref(GTK_OBJECT(l_pSrcBoxWidget));
	gtk_container_remove(GTK_CONTAINER(l_pSrcParentWidget), l_pSrcBoxWidget);

	gtk_object_ref(GTK_OBJECT(l_pDstBoxWidget));
	gtk_container_remove(GTK_CONTAINER(l_pDstParentWidget), l_pDstBoxWidget);

	//parent src box to dst parent
	if(GTK_IS_WINDOW(l_pDstParentWidget))
	{
		gtk_container_add(GTK_CONTAINER(l_pDstParentWidget), l_pSrcBoxWidget);
	}
	else if(GTK_IS_NOTEBOOK(l_pDstParentWidget))
	{
		gtk_notebook_insert_page(GTK_NOTEBOOK(l_pDstParentWidget), l_pSrcBoxWidget, l_pDstTabLabel, l_i32DstIndex);
	}
	else //dst parent is a paned
	{
		if(l_i32DstIndex == 1)
		{
			gtk_paned_pack1(GTK_PANED(l_pDstParentWidget), l_pSrcBoxWidget, TRUE, TRUE);
		}
		else
		{
			gtk_paned_pack2(GTK_PANED(l_pDstParentWidget), l_pSrcBoxWidget, TRUE, TRUE);
		}
	}

	//parent dst box to src parent
	if(GTK_IS_WINDOW(l_pSrcParentWidget))
	{
		gtk_container_add(GTK_CONTAINER(l_pSrcParentWidget), l_pDstBoxWidget);
	}
	else if(GTK_IS_NOTEBOOK(l_pSrcParentWidget))
	{
		gtk_notebook_insert_page(GTK_NOTEBOOK(l_pSrcParentWidget), l_pDstBoxWidget, l_pSrcTabLabel, l_i32SrcIndex);
	}
	else //src parent is a paned
	{
		if(l_i32SrcIndex == 1)
		{
			gtk_paned_pack1(GTK_PANED(l_pSrcParentWidget), l_pDstBoxWidget, TRUE, TRUE);
		}
		else
		{
			gtk_paned_pack2(GTK_PANED(l_pSrcParentWidget), l_pDstBoxWidget, TRUE, TRUE);
		}
	}
}

void CPlayerVisualisation::toolbar_button_toggled_cb(::GtkToggleButton* pButton, gpointer user_data)
{
	static_cast<CPlayerVisualisation*>(user_data)->toggleToolbarCB(pButton);
}

OpenViBE::boolean CPlayerVisualisation::toggleToolbarCB(::GtkToggleButton* pToolbarButton)
{
	//retrieve toolbar widget
	if(m_mVisualisationWidgetToolbar.find(pToolbarButton) == m_mVisualisationWidgetToolbar.end())
	{
		return false;
	}
	::GtkWidget* l_pToolbar = m_mVisualisationWidgetToolbar[pToolbarButton];

	//if current toolbar is toggled on or off, update toolbar state accordingly
	if(pToolbarButton == m_pActiveToolbarButton)
	{
		if(gtk_toggle_button_get_active(pToolbarButton) == FALSE)
		{
			gtk_widget_hide_all(l_pToolbar);
			m_pActiveToolbarButton = NULL;
		}
		else
		{
			gtk_widget_show_all(l_pToolbar);
		}
	}
	else //a new toolbar is to be shown
	{
		//hide previously active toolbar, if any
		if(m_pActiveToolbarButton != NULL)
		{
			gtk_widget_hide_all(m_mVisualisationWidgetToolbar[m_pActiveToolbarButton]);

			g_signal_handlers_disconnect_by_func(m_pActiveToolbarButton, G_CALLBACK2(toolbar_button_toggled_cb), this);
			gtk_toggle_button_set_active(m_pActiveToolbarButton, FALSE);
			g_signal_connect(m_pActiveToolbarButton, "toggled", G_CALLBACK(toolbar_button_toggled_cb), this);
		}
/*
		//set toolbar transient for plugin window
		::GtkWidget* l_pWidget = GTK_WIDGET(pToolbarButton);
		while(l_pWidget!=NULL && !GTK_IS_WINDOW(l_pWidget))
		{
			l_pWidget = gtk_widget_get_parent(l_pWidget);
		}
		if(l_pWidget != NULL && GTK_IS_WINDOW(l_pToolbar))
		{
			gtk_window_set_transient_for(GTK_WINDOW(l_pToolbar), GTK_WINDOW(l_pWidget));
		}
*/
		//show new toolbar
		gtk_widget_show_all(l_pToolbar);

		//update active toolbar button
		m_pActiveToolbarButton = pToolbarButton;
	}

	return true;
}

gboolean CPlayerVisualisation::toolbar_delete_event_cb(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
	if(user_data != NULL)
		static_cast<CPlayerVisualisation*>(user_data)->deleteToolbarCB(widget);

	return TRUE;
}

OpenViBE::boolean CPlayerVisualisation::deleteToolbarCB(GtkWidget* pToolbarWidget)
{
	if(m_pActiveToolbarButton == NULL || m_mVisualisationWidgetToolbar[m_pActiveToolbarButton] != pToolbarWidget)
	{
		//error : active toolbar isn't the one registered as such
		gtk_widget_hide_all(pToolbarWidget);
		return FALSE;
	}

	//toggle toolbar button off
	g_signal_handlers_disconnect_by_func(m_pActiveToolbarButton, G_CALLBACK2(toolbar_button_toggled_cb), this);
	gtk_toggle_button_set_active(m_pActiveToolbarButton, FALSE);
	g_signal_connect(m_pActiveToolbarButton, "toggled", G_CALLBACK(toolbar_button_toggled_cb), this);

	//hide toolbar
	gtk_widget_hide_all(pToolbarWidget);

	//clear active toolbar button pointer
	m_pActiveToolbarButton = NULL;

	return TRUE;
}
