#include "ovd_base.h"
#include "ovdTAttributeHandler.h"
#include "ovdCApplication.h"
#include "ovdCInterfacedScenario.h"
#include "ovdCPlayerVisualisation.h"

#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

static ::GtkTargetEntry targets [] =
{
	{ (gchar*)"STRING", 0, 0 },
	{ (gchar*)"text/plain", 0, 0 },
};

static void dummy_callback(::GtkWidget*)
{
}

CPlayerVisualisation::CPlayerVisualisation(const IKernelContext& rKernelContext, IVisualisationTree& rVisualisationTree, CInterfacedScenario& rInterfacedScenario) :
	m_rKernelContext(rKernelContext),
	m_rVisualisationTree(rVisualisationTree),
	m_rInterfacedScenario(rInterfacedScenario),
	m_pActiveToolbarButton(NULL)
{
}

CPlayerVisualisation::~CPlayerVisualisation(void)
{
	hideTopLevelWindows();

	m_pActiveToolbarButton = NULL;

	for(unsigned int i=0; i<m_vWindows.size(); i++)
	{
		g_signal_handlers_disconnect_by_func(G_OBJECT(m_vWindows[i]), G_CALLBACK2(CPlayerVisualisation::configure_event_cb), this);
		gtk_widget_destroy(GTK_WIDGET(m_vWindows[i]));
	}

	m_rVisualisationTree.setTreeViewCB(NULL);
}

void CPlayerVisualisation::init(void)
{
	//empty windows vector
	m_vWindows.clear();

	//empty split widgets map
	m_mSplitWidgets.clear();

	//empty toolbars map
	m_mToolbars.clear();

	//empty plugin widgets map
	m_mPlugins.clear();

	m_pActiveToolbarButton = NULL;

	//register towards tree store
	m_rVisualisationTree.setTreeViewCB(this);

	//rebuild widgets
	m_rVisualisationTree.reloadTree();
}

::GtkWidget* CPlayerVisualisation::loadTreeWidget(IVisualisationWidget* pVisualisationWidget)
{
	::GtkWidget* l_pTreeWidget = NULL;

	if(pVisualisationWidget->getType() == EVisualisationWidget_VisualisationPanel)
	{
		//retrieve panel index
		IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(pVisualisationWidget->getParentIdentifier());
		if(l_pVisualisationWindow != NULL)
		{
			uint32 l_ui32PanelIndex;
			l_pVisualisationWindow->getChildIndex(pVisualisationWidget->getIdentifier(), l_ui32PanelIndex);

			//create notebook if this is the first panel
			if(l_ui32PanelIndex == 0)
			{
				l_pTreeWidget = gtk_notebook_new();
			}
			else //otherwise retrieve it from first panel
			{
				CIdentifier l_oFirstPanelIdentifier;
				l_pVisualisationWindow->getChildIdentifier(0, l_oFirstPanelIdentifier);
				GtkTreeIter l_oFirstPanelIter;
				m_rVisualisationTree.findChildNodeFromRoot(&l_oFirstPanelIter, l_oFirstPanelIdentifier);
				void* l_pNotebookWidget = NULL;
				m_rVisualisationTree.getPointerValueFromTreeIter(&l_oFirstPanelIter, l_pNotebookWidget, EVisualisationTreeColumn_PointerWidget);
				l_pTreeWidget = (GtkWidget*)l_pNotebookWidget;
			}
		}
	}
	else if(pVisualisationWidget->getType() == EVisualisationWidget_VerticalSplit ||	pVisualisationWidget->getType() == EVisualisationWidget_HorizontalSplit ||
		pVisualisationWidget->getType() == EVisualisationWidget_Undefined || pVisualisationWidget->getType() == EVisualisationWidget_VisualisationBox)
	{
		if(pVisualisationWidget->getType() == EVisualisationWidget_VisualisationBox)
		{
			if(pVisualisationWidget->getParentIdentifier() != OV_UndefinedIdentifier)
			{
				//dummy widget (actual one will be created at plugin initialization time)
				l_pTreeWidget = gtk_button_new();
			}
			else
			{
				//widget will be added to a top level window in setWidget()
			}
		}
		else if(pVisualisationWidget->getType() == EVisualisationWidget_Undefined)
		{
			l_pTreeWidget = gtk_button_new();
			gtk_button_set_label(GTK_BUTTON(l_pTreeWidget), (const char*)pVisualisationWidget->getName());
		}
		else if(pVisualisationWidget->getType() == EVisualisationWidget_HorizontalSplit || pVisualisationWidget->getType() == EVisualisationWidget_VerticalSplit)
		{
			l_pTreeWidget = (pVisualisationWidget->getType() == EVisualisationWidget_HorizontalSplit) ? gtk_hpaned_new() : gtk_vpaned_new();

			//store paned widget in paned map
			m_mSplitWidgets[GTK_PANED(l_pTreeWidget)] = pVisualisationWidget->getIdentifier();

			//retrieve its attributes
			TAttributeHandler l_oAttributeHandler(*pVisualisationWidget);
			int l_i32HandlePos = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWidget_DividerPosition);

			//initialize paned handle position
			gtk_paned_set_position(GTK_PANED(l_pTreeWidget), l_i32HandlePos);
		}

		//parent widget to its parent
		//---------------------------
		IVisualisationWidget* l_pParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(pVisualisationWidget->getParentIdentifier());

		if(l_pParentVisualisationWidget != NULL) //unparented visualisation boxes don't have a parent
		{
			GtkTreeIter l_oParentIter;
			m_rVisualisationTree.findChildNodeFromRoot(&l_oParentIter, l_pParentVisualisationWidget->getIdentifier());

			if(l_pParentVisualisationWidget->getType() == EVisualisationWidget_VisualisationPanel)
			{
				//parent widget to notebook as a new page
				void* l_pNotebook = NULL;
				m_rVisualisationTree.getPointerValueFromTreeIter(&l_oParentIter, l_pNotebook, EVisualisationTreeColumn_PointerWidget);
				char* l_pVisualisationPanelName = NULL;
				m_rVisualisationTree.getStringValueFromTreeIter(&l_oParentIter, l_pVisualisationPanelName, EVisualisationTreeColumn_StringName);
				gtk_notebook_append_page(GTK_NOTEBOOK(l_pNotebook), l_pTreeWidget, gtk_label_new(l_pVisualisationPanelName));
			}
			else if(l_pParentVisualisationWidget->getType() == EVisualisationWidget_VerticalSplit || l_pParentVisualisationWidget->getType() == EVisualisationWidget_HorizontalSplit)
			{
				//insert widget in parent paned
				void* l_pPaned = NULL;
				m_rVisualisationTree.getPointerValueFromTreeIter(&l_oParentIter, l_pPaned, EVisualisationTreeColumn_PointerWidget);
				if(l_pPaned != NULL && GTK_IS_PANED(l_pPaned))
				{
					uint32 l_ui32ChildIndex;
					if(l_pParentVisualisationWidget->getChildIndex(pVisualisationWidget->getIdentifier(), l_ui32ChildIndex) == true)
					{
						if(l_ui32ChildIndex == 0)
						{
							gtk_paned_pack1(GTK_PANED(l_pPaned), l_pTreeWidget, TRUE, TRUE);
						}
						else
						{
							gtk_paned_pack2(GTK_PANED(l_pPaned), l_pTreeWidget, TRUE, TRUE);
						}
					}
				}
			}
		}
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
			l_pTreeWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			m_vWindows.push_back(GTK_WINDOW(l_pTreeWidget));

			//retrieve its size
			TAttributeHandler l_oAttributeHandler(*pVisualisationWidget);
			gtk_window_set_default_size(
				GTK_WINDOW(l_pTreeWidget),
				l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWindow_Width),
				l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWindow_Height));
			//set its title
			gtk_window_set_title(GTK_WINDOW(l_pTreeWidget), (const char*)pVisualisationWidget->getName());

			//set it transient for main window
			gtk_window_set_transient_for(GTK_WINDOW(l_pTreeWidget), GTK_WINDOW(m_rInterfacedScenario.m_rApplication.m_pMainWindow));

			//centered on the main window
			if(m_rKernelContext.getConfigurationManager().expandAsBoolean("${Designer_WindowManager_Center}", false))
			{
				gtk_window_set_position(GTK_WINDOW(l_pTreeWidget),GTK_WIN_POS_CENTER_ON_PARENT);
			}

			//FIXME wrong spelling (-)
			gtk_signal_connect(GTK_OBJECT(l_pTreeWidget), "configure_event", G_CALLBACK(configure_event_cb), this);
			//FIXME wrong spelling (-)
			g_signal_connect(l_pTreeWidget, "delete_event", G_CALLBACK(dummy_callback), NULL);
		}
	}

	//show newly created widget
	if(l_pTreeWidget != NULL && pVisualisationWidget->getType() != EVisualisationWidget_VisualisationWindow)
	{
		gtk_widget_show(l_pTreeWidget);
	}

#if 0
	//*** moved to setWidget() ***
	//resize widgets once they are allocated : this is the case when they are shown on an expose event
	//FIXME : perform resizing only once (when it is done as many times as there are widgets in the tree here)
	if(l_pTreeWidget != NULL)
	{
		gtk_signal_connect(GTK_OBJECT(l_pTreeWidget), "expose-event", G_CALLBACK(widget_expose_event_cb), this);
	}
#endif

	return l_pTreeWidget;
}

void CPlayerVisualisation::endLoadTreeWidget(OpenViBE::Kernel::IVisualisationWidget* pVisualisationWidget)
{
	//retrieve tree widget
	GtkTreeIter l_oIter;
	m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, pVisualisationWidget->getIdentifier());
	void* l_pTreeWidget;
	m_rVisualisationTree.getPointerValueFromTreeIter(&l_oIter, l_pTreeWidget, EVisualisationTreeColumn_PointerWidget);

	if(l_pTreeWidget != NULL && pVisualisationWidget->getType() == EVisualisationWidget_VisualisationWindow)
	{
		//retrieve notebook
		CIdentifier l_oChildIdentifier;
		pVisualisationWidget->getChildIdentifier(0, l_oChildIdentifier);
		GtkTreeIter l_oChildIter;
		m_rVisualisationTree.findChildNodeFromRoot(&l_oChildIter,  l_oChildIdentifier);
		void* l_pChildTreeWidget;
		m_rVisualisationTree.getPointerValueFromTreeIter(&l_oChildIter, l_pChildTreeWidget, EVisualisationTreeColumn_PointerWidget);

		//insert notebook in window
		if(l_pChildTreeWidget != NULL && GTK_IS_NOTEBOOK((GtkWidget*)l_pChildTreeWidget))
		{
			gtk_container_add(GTK_CONTAINER((GtkWidget*)l_pTreeWidget), (GtkWidget*)l_pChildTreeWidget);
		}
	}
}

boolean CPlayerVisualisation::setToolbar(const CIdentifier& rBoxIdentifier, ::GtkWidget* pToolbarWidget)
{
	//retrieve visualisation widget
	IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidgetFromBoxIdentifier(rBoxIdentifier);
	if(l_pVisualisationWidget == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "CPlayerVisualisation::setToolbar FAILED : couldn't retrieve simulated box with identifier " << rBoxIdentifier <<"\n";
		return false;
	}

	//ensure toolbar pointer is not null
	if(pToolbarWidget == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "CPlayerVisualisation::setToolbar FAILED : toolbar pointer is NULL for plugin " << l_pVisualisationWidget->getName() << "\n";
		return false;
	}

	//ensure toolbar pointer is a window
	if(GTK_IS_WINDOW(pToolbarWidget) == false)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "CPlayerVisualisation::setToolbar FAILED : toolbar pointer is not a GtkWindow for plugin " << l_pVisualisationWidget->getName() <<"\n";
		return false;
	}

	//retrieve identifier
	CIdentifier l_oIdentifier = l_pVisualisationWidget->getIdentifier();

	//store toolbar
	m_mPlugins[l_oIdentifier].m_pToolbar = pToolbarWidget;

	//ensure it is open at mouse position
	gtk_window_set_position(GTK_WINDOW(pToolbarWidget), GTK_WIN_POS_MOUSE);

	//if toolbar button has been created, set it sensitive (otherwise it will be set active later)
	if(m_mPlugins[l_oIdentifier].m_pToolbarButton != NULL)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(m_mPlugins[l_oIdentifier].m_pToolbarButton), true);

		//associate toolbar button to toolbar window
		m_mToolbars[m_mPlugins[l_oIdentifier].m_pToolbarButton] = pToolbarWidget;
	}

	//catch delete events
	g_signal_connect(G_OBJECT(pToolbarWidget), "delete-event", G_CALLBACK(toolbar_delete_event_cb), this);

	return true;
}

boolean CPlayerVisualisation::setWidget(const CIdentifier& rBoxIdentifier, ::GtkWidget* pWidget)
{
	//retrieve visualisation widget
	IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidgetFromBoxIdentifier(rBoxIdentifier);
	if(l_pVisualisationWidget == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "CPlayerVisualisation::setWidget FAILED : couldn't retrieve simulated box with identifier " << rBoxIdentifier <<"\n";
		return false;
	}

	//ensure widget pointer is not null
	if(pWidget == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "CPlayerVisualisation::setWidget FAILED : widget pointer is NULL for plugin " << l_pVisualisationWidget->getName() << "\n";
		return false;
	}

	//unparent top widget, if necessary
	::GtkWidget* l_pWidgetParent = gtk_widget_get_parent(pWidget);
	if(GTK_IS_CONTAINER(l_pWidgetParent))
	{
		gtk_object_ref(GTK_OBJECT(pWidget));
		gtk_container_remove(GTK_CONTAINER(l_pWidgetParent), pWidget);
	}

	//create a box to store toolbar button and plugin widget
	::GtkBox* l_pVBox = GTK_BOX(gtk_vbox_new(FALSE, 0));
	//gtk_widget_set_size_request(GTK_WIDGET(l_pVBox), 0, 0);

	//create toolbar button
	::GtkToggleButton* l_pButton = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
	{
		//horizontal container : icon + label
		::GtkBox* l_pHBox = GTK_BOX(gtk_hbox_new(FALSE, 0));
		//gtk_widget_set_size_request(GTK_WIDGET(l_pHBox), 0, 0);

		//retrieve icon name
		::GtkTreeIter l_oIter;
		char* l_pIconString = NULL;
		if(m_rVisualisationTree.findChildNodeFromRoot(&l_oIter, (const char*)l_pVisualisationWidget->getName(), EVisualisationTreeNode_VisualisationBox) == true)
		{
			m_rVisualisationTree.getStringValueFromTreeIter(&l_oIter, l_pIconString, EVisualisationTreeColumn_StringStockIcon);
		}

		//create icon
		::GtkWidget* l_pIcon = gtk_image_new_from_stock(l_pIconString != NULL ? l_pIconString : GTK_STOCK_EXECUTE, GTK_ICON_SIZE_BUTTON);
		//gtk_widget_set_size_request(l_pIcon, 0, 0);
		gtk_box_pack_start(l_pHBox, l_pIcon, TRUE, TRUE, 0);

		//create label
		::GtkWidget* l_pLabel = gtk_label_new((const char*)l_pVisualisationWidget->getName());
		//gtk_widget_set_size_request(l_pLabel, 0, 0);
		gtk_box_pack_start(l_pHBox, l_pLabel, TRUE, TRUE, 0);

		//add box to button
		gtk_container_add(GTK_CONTAINER(l_pButton), GTK_WIDGET(l_pHBox));
	}

	//detect toolbar button toggle events
	g_signal_connect(G_OBJECT(l_pButton), "toggled", G_CALLBACK(toolbar_button_toggled_cb), this);

	//set up toolbar button as drag destination
	gtk_drag_dest_set(GTK_WIDGET(l_pButton), GTK_DEST_DEFAULT_ALL, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	g_signal_connect(G_OBJECT(l_pButton), "drag_data_received", G_CALLBACK(drag_data_received_in_widget_cb), this);

	//set up toolbar button as drag source as well
	gtk_drag_source_set(GTK_WIDGET(l_pButton), GDK_BUTTON1_MASK, targets, sizeof(targets)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	g_signal_connect(G_OBJECT(l_pButton), "drag_data_get", G_CALLBACK(drag_data_get_from_widget_cb), this);

	//store plugin widget and toolbar button
	CIdentifier l_oIdentifier = l_pVisualisationWidget->getIdentifier();
	m_mPlugins[l_oIdentifier].m_pWidget = pWidget;
	m_mPlugins[l_oIdentifier].m_pToolbarButton = l_pButton;

	//if a toolbar was registered for this widget, set its button sensitive
	if(m_mPlugins[l_oIdentifier].m_pToolbar != NULL)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(l_pButton), true);

		//associate toolbar button to toolbar window
		m_mToolbars[l_pButton] = m_mPlugins[l_oIdentifier].m_pToolbar;
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(l_pButton), false);
	}

	//vertical container : button on top, visualisation box below
	gtk_box_pack_start(l_pVBox, GTK_WIDGET(l_pButton), FALSE, TRUE, 0);
	gtk_box_pack_start(l_pVBox, pWidget, TRUE, TRUE, 0);

	//show vbox hierarchy
	gtk_widget_show_all(GTK_WIDGET(l_pVBox));

	//parent box at the appropriate location
	parentWidgetBox(l_pVisualisationWidget, l_pVBox);

	return true;
}

boolean CPlayerVisualisation::parentWidgetBox(IVisualisationWidget* pWidget, ::GtkBox* pWidgetBox)
{
	//if widget is unaffected, open it in its own window
	if(pWidget->getParentIdentifier() == OV_UndefinedIdentifier)
	{
		//create a top level window
		::GtkWidget* l_pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		m_vWindows.push_back(GTK_WINDOW(l_pWindow));
#if 1
		uint64 l_ui64DefaultWidth = m_rKernelContext.getConfigurationManager().expandAsUInteger("${Designer_UnaffectedVisualisationWindowWidth}", 400);
		uint64 l_ui64DefaultHeight = m_rKernelContext.getConfigurationManager().expandAsUInteger("${Designer_UnaffectedVisualisationWindowHeight}", 400);
#else
		//one could also choose to initialize unaffected windows with the size of user-created windows
		//=>look for an existing window and get its size from its attributes
		CIdentifier l_oVisualisationWindowIdentifier;
		if(m_rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oVisualisationWindowIdentifier, EVisualisationWidget_VisualisationWindow) == true)
		{
			IVisualisationWidget* l_pVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_oVisualisationWindowIdentifier);
			TAttributeHandler l_oAttributeHandler(*l_pVisualisationWindow);
			l_ui64DefaultWidth = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWindow_Width);
			l_ui64DefaultHeight = l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_VisualisationWindow_Height);
		}
#endif
		gtk_window_set_default_size(GTK_WINDOW(l_pWindow), (gint)l_ui64DefaultWidth, (gint)l_ui64DefaultHeight);
		//set its title
		gtk_window_set_title(GTK_WINDOW(l_pWindow), (const char*)pWidget->getName());
		//set it transient for main window
		gtk_window_set_transient_for(GTK_WINDOW(l_pWindow), GTK_WINDOW(m_rInterfacedScenario.m_rApplication.m_pMainWindow));
		//insert box in top level window
		gtk_container_add(GTK_CONTAINER(l_pWindow), (::GtkWidget*)pWidgetBox);
		//prevent user from closing this window
		g_signal_connect(l_pWindow, "delete_event", G_CALLBACK(dummy_callback), NULL);

		//position: centered in the main window
		if(m_rKernelContext.getConfigurationManager().expandAsBoolean("${Designer_WindowManager_Center}", false))
		{
			gtk_window_set_position(GTK_WINDOW(l_pWindow), GTK_WIN_POS_CENTER_ON_PARENT);
		}

		//show window (and realize widget in doing so)
		gtk_widget_show(l_pWindow);
	}
	else //retrieve parent widget in which to insert current widget
	{
		::GtkTreeIter l_oParentIter;
		if(m_rVisualisationTree.findChildNodeFromRoot(&l_oParentIter, pWidget->getParentIdentifier()) == true)
		{
			void* l_pParentWidget = NULL;
			m_rVisualisationTree.getPointerValueFromTreeIter(&l_oParentIter, l_pParentWidget, EVisualisationTreeColumn_PointerWidget);
			CIdentifier l_oParentIdentifier;
			m_rVisualisationTree.getIdentifierFromTreeIter(&l_oParentIter, l_oParentIdentifier, EVisualisationTreeColumn_StringIdentifier);

			//widget is to be parented to a paned widget
			if(GTK_IS_PANED(l_pParentWidget))
			{
				//retrieve index at which to insert child
				IVisualisationWidget* l_pParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oParentIdentifier);
				uint32 l_oVisualisationBoxIndex;
				l_pParentVisualisationWidget->getChildIndex(pWidget->getIdentifier(), l_oVisualisationBoxIndex);
				//insert visualisation box in paned
				if(l_oVisualisationBoxIndex == 0)
				{
					gtk_container_remove(GTK_CONTAINER(l_pParentWidget), gtk_paned_get_child1(GTK_PANED(l_pParentWidget)));
					gtk_paned_pack1(GTK_PANED(l_pParentWidget), GTK_WIDGET(pWidgetBox), TRUE, TRUE);
				}
				else
				{
					gtk_container_remove(GTK_CONTAINER(l_pParentWidget), gtk_paned_get_child2(GTK_PANED(l_pParentWidget)));
					gtk_paned_pack2(GTK_PANED(l_pParentWidget), GTK_WIDGET(pWidgetBox), TRUE, TRUE);
				}
			}
			else if(GTK_IS_NOTEBOOK(l_pParentWidget)) //widget is to be added to a notebook page
			{
				//retrieve notebook page index
				IVisualisationWidget* l_pParentVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oParentIdentifier);
				IVisualisationWidget* l_pParentVisualisationWindow = m_rVisualisationTree.getVisualisationWidget(l_pParentVisualisationWidget->getParentIdentifier());
				uint32 l_oPanelIndex;
				l_pParentVisualisationWindow->getChildIndex(l_pParentVisualisationWidget->getIdentifier(), l_oPanelIndex);

				//remove temporary page
				gtk_notebook_remove_page(GTK_NOTEBOOK(l_pParentWidget), l_oPanelIndex);

				//insert final page
				gtk_notebook_insert_page(
					GTK_NOTEBOOK(l_pParentWidget),
					GTK_WIDGET(pWidgetBox),
					gtk_label_new((const char*)l_pParentVisualisationWidget->getName()),
					l_oPanelIndex);
			}

			//resize widgets once they are allocated : this is the case when they are shown on an expose event
			//FIXME : perform resizing only once (when it is done as many times as there are widgets in the tree here)
			if(l_pParentWidget != NULL)
			{
				gtk_signal_connect(GTK_OBJECT(l_pParentWidget), "expose-event", G_CALLBACK(widget_expose_event_cb), this);
			}

			//show window (and realize widget if it owns a 3D context)
			//--------------------------------------------------------
			//get panel containing widget
			GtkTreeIter l_oPanelIter = l_oParentIter;
			if(m_rVisualisationTree.findParentNode(&l_oPanelIter, EVisualisationTreeNode_VisualisationPanel) == true)
			{
				//get panel identifier
				CIdentifier l_oPanelIdentifier;
				m_rVisualisationTree.getIdentifierFromTreeIter(&l_oPanelIter, l_oPanelIdentifier, EVisualisationTreeColumn_StringIdentifier);

				//get panel index in window
				uint32 l_ui32PanelIndex;
				m_rVisualisationTree.getVisualisationWidgetIndex(l_oPanelIdentifier, l_ui32PanelIndex);

				//get notebook pointer
				void* l_pPanelWidget = NULL;
				m_rVisualisationTree.getPointerValueFromTreeIter(&l_oPanelIter, l_pPanelWidget, EVisualisationTreeColumn_PointerWidget);

				//get parent window
				GtkTreeIter l_oWindowIter = l_oPanelIter;
				if(m_rVisualisationTree.findParentNode(&l_oWindowIter, EVisualisationTreeNode_VisualisationWindow) == true)
				{
					//get parent window pointer
					void* l_pWindowWidget = NULL;
					m_rVisualisationTree.getPointerValueFromTreeIter(&l_oWindowIter, l_pWindowWidget, EVisualisationTreeColumn_PointerWidget);

					//show parent window
					gtk_widget_show(GTK_WIDGET(l_pWindowWidget));
					// gtk_widget_realize(GTK_WIDGET(l_pWindowWidget));
					// gdk_flush();

					//set panel containing widget as current (this realizes the widget)
					gtk_notebook_set_current_page(GTK_NOTEBOOK(l_pPanelWidget), l_ui32PanelIndex);

					//then reset first panel as current
					gtk_notebook_set_current_page(GTK_NOTEBOOK(l_pPanelWidget), 0);
				}
			}
		}
	}

	return true;
}

//called upon Player start
void CPlayerVisualisation::showTopLevelWindows(void)
{
	for(unsigned int i=0; i<m_vWindows.size(); i++)
	{
		gtk_widget_show(GTK_WIDGET(m_vWindows[i]));
	}
	if(m_pActiveToolbarButton != NULL)
	{
		//show active toolbar
		gtk_widget_show(m_mToolbars[m_pActiveToolbarButton]);
	}
	std::map < OpenViBE::CIdentifier, CPlayerVisualisation::CPluginWidgets >::iterator it=m_mPlugins.begin();
	while(it!=m_mPlugins.end())
	{
		if(GTK_IS_WIDGET(it->second.m_pWidget))
		{
			gtk_widget_show(it->second.m_pWidget);
		}
		it++;
	}
}

//called upon Player stop
void CPlayerVisualisation::hideTopLevelWindows(void)
{
	std::map < OpenViBE::CIdentifier, CPlayerVisualisation::CPluginWidgets >::iterator it=m_mPlugins.begin();
	while(it!=m_mPlugins.end())
	{
		if(GTK_IS_WIDGET(it->second.m_pWidget))
		{
			gtk_widget_hide(it->second.m_pWidget);
		}
		it++;
	}
	for(unsigned int i=0; i<m_vWindows.size(); i++)
	{
		gtk_widget_hide(GTK_WIDGET(m_vWindows[i]));
	}
	if(m_pActiveToolbarButton != NULL)
	{
		//hide active toolbar
		gtk_widget_hide(m_mToolbars[m_pActiveToolbarButton]);
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

		//retrieve paned identifier from paned map
		CIdentifier& l_oPanedIdentifier = m_mSplitWidgets[GTK_PANED(l_pPaned)];

		//retrieve its attributes
		IVisualisationWidget* l_pVisualisationWidget = m_rVisualisationTree.getVisualisationWidget(l_oPanedIdentifier);
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
	sprintf(l_sString, "%p", pSrcWidget);
	gtk_selection_data_set_text(pSelectionData, l_sString, strlen(l_sString));
}

void CPlayerVisualisation::drag_data_received_in_widget_cb(::GtkWidget* pDstWidget, GdkDragContext* pDragContext,gint iX,gint iY,::GtkSelectionData* pSelectionData,guint uiInfo,guint uiTime,gpointer pData)
{
	void* l_pSrcWidget = NULL;
	sscanf((const char*)gtk_selection_data_get_text(pSelectionData), "%p", &l_pSrcWidget);

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

	//ensure src and dst widgets are different
	if(l_pSrcBoxWidget == l_pDstBoxWidget)
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

boolean CPlayerVisualisation::toggleToolbarCB(::GtkToggleButton* pToolbarButton)
{
	//retrieve toolbar
	if(m_mToolbars.find(pToolbarButton) == m_mToolbars.end())
	{
		return false;
	}
	::GtkWidget* l_pToolbar = m_mToolbars[pToolbarButton];

	//if current toolbar is toggled on or off, update toolbar state accordingly
	if(pToolbarButton == m_pActiveToolbarButton)
	{
		//hiding active toolbar
		if(gtk_toggle_button_get_active(pToolbarButton) == FALSE)
		{
			gtk_widget_hide(l_pToolbar);
			m_pActiveToolbarButton = NULL;
		}
		else //showing active toolbar
		{
			gtk_widget_show(l_pToolbar);
		}
	}
	else //a new toolbar is to be shown
	{
		//hide previously active toolbar, if any
		if(m_pActiveToolbarButton != NULL)
		{
			gtk_widget_hide(m_mToolbars[m_pActiveToolbarButton]);

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
		gtk_widget_show(l_pToolbar);

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

boolean CPlayerVisualisation::deleteToolbarCB(GtkWidget* pToolbarWidget)
{
	if(m_pActiveToolbarButton == NULL || m_mToolbars[m_pActiveToolbarButton] != pToolbarWidget)
	{
		//error : active toolbar isn't the one registered as such
		gtk_widget_hide(pToolbarWidget);
		return FALSE;
	}

	//toggle toolbar button off
	g_signal_handlers_disconnect_by_func(m_pActiveToolbarButton, G_CALLBACK2(toolbar_button_toggled_cb), this);
	gtk_toggle_button_set_active(m_pActiveToolbarButton, FALSE);
	g_signal_connect(m_pActiveToolbarButton, "toggled", G_CALLBACK(toolbar_button_toggled_cb), this);

	//hide toolbar
	gtk_widget_hide(pToolbarWidget);

	//clear active toolbar button pointer
	m_pActiveToolbarButton = NULL;

	return TRUE;
}
