#include "ovd_base.h"

#include <system/Time.h>

#include <stack>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <cstring>
#include <cstdlib>

#if defined OVD_OS_Linux
 #define _strcmpi strcasecmp
#endif

#define OVD_GUI_File "../share/openvibe-applications/designer/interface.ui"
#define OVD_GUI_Settings_File "../share/openvibe-applications/designer/interface-settings.ui"

#include "ovdCDesignerVisualisation.h"
#include "ovdCPlayerVisualisation.h"
#include "ovdCInterfacedObject.h"
#include "ovdCInterfacedScenario.h"
#include "ovdCApplication.h"
#include "ovdCLogListenerDesigner.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEDesigner;
using namespace std;

namespace
{
	::guint __g_idle_add__(::GSourceFunc fpCallback, ::gpointer pUserData, ::gint iPriority=G_PRIORITY_DEFAULT_IDLE)
	{
		::GSource* l_pSource=g_idle_source_new();
		g_source_set_priority(l_pSource, G_PRIORITY_LOW);
		g_source_set_callback(l_pSource, fpCallback, pUserData, NULL);
		return g_source_attach(l_pSource, NULL);
	}

	::guint __g_timeout_add__(::guint uiInterval, ::GSourceFunc fpCallback, ::gpointer pUserData, ::gint iPriority=G_PRIORITY_DEFAULT)
	{
		::GSource* l_pSource=g_timeout_source_new(uiInterval);
		g_source_set_priority(l_pSource, G_PRIORITY_LOW);
		g_source_set_callback(l_pSource, fpCallback, pUserData, NULL);
		return g_source_attach(l_pSource, NULL);
	}

	void drag_data_get_cb(::GtkWidget* pWidget, ::GdkDragContext* pDragContex, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->dragDataGetCB(pWidget, pDragContex, pSelectionData, uiInfo, uiT);
	}
	void menu_undo_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->undoCB();
	}
	void menu_redo_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->redoCB();
	}
	void menu_focus_search_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		gtk_widget_grab_focus(GTK_WIDGET(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-box_algorithm_searchbox")));
	}
	void menu_copy_selection_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->copySelectionCB();
	}
	void menu_cut_selection_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->cutSelectionCB();
	}
	void menu_paste_selection_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->pasteSelectionCB();
	}
	void menu_delete_selection_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->deleteSelectionCB();
	}
	void menu_preferences_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->preferencesCB();
	}

	void menu_test_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->testCB();
	}
	void menu_new_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->newScenarioCB();
	}
	void menu_open_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->openScenarioCB();
	}
	void menu_save_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->saveScenarioCB();
	}
	void menu_save_scenario_as_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->saveScenarioAsCB();
	}
	void menu_close_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->closeScenarioCB(static_cast<CApplication*>(pUserData)->getCurrentInterfacedScenario());
	}
	void menu_quit_application_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		if(static_cast<CApplication*>(pUserData)->quitApplicationCB())
		{
			gtk_main_quit();
		}
	}

	void menu_about_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->aboutScenarioCB(static_cast<CApplication*>(pUserData)->getCurrentInterfacedScenario());
	}
	void menu_about_openvibe_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->aboutOpenViBECB();
	}
	void menu_browse_documentation_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->browseDocumentationCB();
	}

	void button_new_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->newScenarioCB();
	}
	void button_open_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->openScenarioCB();
	}
	void button_save_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->saveScenarioCB();
	}
	void button_save_scenario_as_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->saveScenarioAsCB();
	}
	void button_close_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->closeScenarioCB(static_cast<CApplication*>(pUserData)->getCurrentInterfacedScenario());
	}

	void delete_designer_visualisation_cb(gpointer user_data)
	{
		static_cast<CApplication*>(user_data)->deleteDesignerVisualisationCB();
	}
	void button_toggle_window_manager_cb(::GtkToggleToolButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->toggleDesignerVisualisationCB();
	}

	void button_comment_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->addCommentCB(static_cast<CApplication*>(pUserData)->getCurrentInterfacedScenario());
	}
	void button_about_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->aboutScenarioCB(static_cast<CApplication*>(pUserData)->getCurrentInterfacedScenario());
	}

	void stop_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->stopScenarioCB();
	}
	void play_pause_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		if(std::string(gtk_tool_button_get_stock_id(GTK_TOOL_BUTTON(pButton)))==GTK_STOCK_MEDIA_PLAY)
		{
			static_cast<CApplication*>(pUserData)->playScenarioCB();
		}
		else
		{
			static_cast<CApplication*>(pUserData)->pauseScenarioCB();
		}
	}
	void next_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->nextScenarioCB();
	}
	void forward_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->forwardScenarioCB();
	}

	gboolean button_quit_application_cb(::GtkWidget* pWidget, ::GdkEvent* pEvent, gpointer pUserData)
	{
		if(static_cast<CApplication*>(pUserData)->quitApplicationCB())
		{
			gtk_main_quit();
			return FALSE;
		}
		return TRUE;
	}

	void log_level_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->logLevelCB();
	}

	void cpu_usage_cb(::GtkToggleButton* pButton, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->CPUUsageCB();
	}

	gboolean change_current_scenario_cb(::GtkNotebook* pNotebook, ::GtkNotebookPage* pNotebookPage, guint uiPageNumber, gpointer pUserData)
	{
		static_cast<CApplication*>(pUserData)->changeCurrentScenario((int32)uiPageNumber);
		return TRUE;
	}

	void box_algorithm_title_button_expand_cb(::GtkButton* pButton, gpointer pUserData)
	{
		gtk_tree_view_expand_all(GTK_TREE_VIEW(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-box_algorithm_tree")));
		gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-resource_notebook")), 0);
	}
	void box_algorithm_title_button_collapse_cb(::GtkButton* pButton, gpointer pUserData)
	{
		gtk_tree_view_collapse_all(GTK_TREE_VIEW(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-box_algorithm_tree")));
		gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-resource_notebook")), 0);
	}

	void algorithm_title_button_expand_cb(::GtkButton* pButton, gpointer pUserData)
	{
		gtk_tree_view_expand_all(GTK_TREE_VIEW(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-algorithm_tree")));
		gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-resource_notebook")), 1);
	}
	void algorithm_title_button_collapse_cb(::GtkButton* pButton, gpointer pUserData)
	{
		gtk_tree_view_collapse_all(GTK_TREE_VIEW(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-algorithm_tree")));
		gtk_notebook_set_current_page(GTK_NOTEBOOK(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-resource_notebook")), 1);
	}

	void clear_messages_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<CLogListenerDesigner*>(pUserData)->clearMessages();
	}

	string strtoupper(string str)
	{
		int leng=str.length();
		for(int i=0; i<leng; i++)
			if (97<=str[i]&&str[i]<=122)//a-z
				str[i]-=32;
		return str;
	}
	static gboolean box_algorithm_search_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer pUserData)
	{
		CApplication* l_pApplication=static_cast<CApplication*>(pUserData);
		/* Visible if row is non-empty and first column is "HI" */


		gboolean l_bVisible = false;
		gboolean l_bShowUnstable = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(static_cast<CApplication*>(pUserData)->m_pBuilderInterface, "openvibe-show_unstable")));

		gchar* l_sHaystackName;
		gchar* l_sHaystackDescription;
		gboolean l_bHaystackUnstable;

		gtk_tree_model_get(model, iter, 0, &l_sHaystackName, 1, &l_sHaystackDescription, 6, &l_bHaystackUnstable, -1);

		// consider only leaf nodes which match the search term
		if (l_sHaystackName!=NULL && l_sHaystackDescription!=NULL)
		{
			if ((l_bShowUnstable || !l_bHaystackUnstable) && (string::npos != strtoupper(l_sHaystackName).find(strtoupper(l_pApplication->m_sSearchTerm)) || string::npos != strtoupper(l_sHaystackDescription).find(strtoupper(l_pApplication->m_sSearchTerm)) || gtk_tree_model_iter_has_child(model, iter)))
			{
				//std::cout << "value : " << l_pApplication->m_sSearchTerm << "\n";
				l_bVisible = true;
			}

			g_free(l_sHaystackName);
			g_free(l_sHaystackDescription);
		}
		else
		{
			l_bVisible = true;
		}

		return l_bVisible;
	}

	static gboolean box_algorithm_prune_empty_folders(GtkTreeModel *model, GtkTreeIter *iter, gpointer pUserData)
	{
		gboolean l_bIsPlugin;
		gtk_tree_model_get(model, iter, 5, &l_bIsPlugin, -1);

		if (gtk_tree_model_iter_has_child(model, iter) || l_bIsPlugin)
		{
			return true;
		}

		return false;
	}

	static gboolean	do_refilter( CApplication *pApplication )
	{
		/*
		if (0 == strcmp(pApplication->m_sSearchTerm, ""))
		{
			// reattach the old model
			gtk_tree_view_set_model(pApplication->m_pBoxAlgorithmTreeView, GTK_TREE_MODEL(pApplication->m_pBoxAlgorithmTreeModel));
		}
		else
		*/
		{
			pApplication->m_pBoxAlgorithmTreeModelFilter = gtk_tree_model_filter_new(GTK_TREE_MODEL(pApplication->m_pBoxAlgorithmTreeModel), NULL);
			pApplication->m_pBoxAlgorithmTreeModelFilter2 = gtk_tree_model_filter_new(GTK_TREE_MODEL(pApplication->m_pBoxAlgorithmTreeModelFilter), NULL);
			pApplication->m_pBoxAlgorithmTreeModelFilter3 = gtk_tree_model_filter_new(GTK_TREE_MODEL(pApplication->m_pBoxAlgorithmTreeModelFilter2), NULL);
			pApplication->m_pBoxAlgorithmTreeModelFilter4 = gtk_tree_model_filter_new(GTK_TREE_MODEL(pApplication->m_pBoxAlgorithmTreeModelFilter3), NULL);
			// detach the normal model from the treeview
			gtk_tree_view_set_model(pApplication->m_pBoxAlgorithmTreeView, NULL);

			// clear the model

			// add a filtering function to the model
			gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(pApplication->m_pBoxAlgorithmTreeModelFilter), box_algorithm_search_func, pApplication, NULL );
			gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(pApplication->m_pBoxAlgorithmTreeModelFilter2), box_algorithm_prune_empty_folders, pApplication, NULL );
			gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(pApplication->m_pBoxAlgorithmTreeModelFilter3), box_algorithm_prune_empty_folders, pApplication, NULL );
			gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(pApplication->m_pBoxAlgorithmTreeModelFilter4), box_algorithm_prune_empty_folders, pApplication, NULL );

			// attach the model to the treeview
			gtk_tree_view_set_model(pApplication->m_pBoxAlgorithmTreeView, GTK_TREE_MODEL(pApplication->m_pBoxAlgorithmTreeModelFilter4));

			if (0 == strcmp(pApplication->m_sSearchTerm, ""))
			{
				gtk_tree_view_collapse_all(pApplication->m_pBoxAlgorithmTreeView);
			}
			else
			{
				gtk_tree_view_expand_all(pApplication->m_pBoxAlgorithmTreeView);
			}
		}

		pApplication->m_giFilterTimeout = 0;

		return false;
	}

	static void	queue_refilter( CApplication* pApplication )
	{
		if( pApplication->m_giFilterTimeout )
			g_source_remove( pApplication->m_giFilterTimeout );

		pApplication->m_giFilterTimeout = g_timeout_add( 300, (GSourceFunc)do_refilter, pApplication );
	}

	void refresh_search_cb(::GtkEntry* pTextfield, CApplication* pApplication)
	{
		pApplication->m_sSearchTerm = gtk_entry_get_text(pTextfield);

		queue_refilter(pApplication);
	}

	void refresh_search_no_data_cb(::GtkToggleButton* pToggleButton, CApplication* pApplication)
	{
		pApplication->m_sSearchTerm = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(pApplication->m_pBuilderInterface, "openvibe-box_algorithm_searchbox")));

		queue_refilter(pApplication);
	}


	static gboolean searchbox_focus_in_cb(::GtkWidget* pWidget, ::GdkEvent* pEvent, CApplication* pApplication)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(pApplication->m_pBuilderInterface, "openvibe-menu_edit")), false);

		return false;
	}

	static gboolean searchbox_focus_out_cb(::GtkWidget* pWidget, ::GdkEvent* pEvent, CApplication* pApplication)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(pApplication->m_pBuilderInterface, "openvibe-menu_edit")), true);

		return false;
	}

	gboolean idle_application_loop(gpointer pUserData)
	{
		CApplication* l_pApplication=static_cast<CApplication*>(pUserData);
		CInterfacedScenario* l_pCurrentInterfacedScenario=l_pApplication->getCurrentInterfacedScenario();
		if(l_pCurrentInterfacedScenario)
		{
			if(l_pApplication->getPlayer() && l_pCurrentInterfacedScenario->m_ePlayerStatus != l_pApplication->getPlayer()->getStatus())
			{
				switch(l_pApplication->getPlayer()->getStatus())
				{
					case PlayerStatus_Stop:    gtk_signal_emit_by_name(GTK_OBJECT(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-button_stop")), "clicked"); break;
					case PlayerStatus_Pause:   while(l_pCurrentInterfacedScenario->m_ePlayerStatus != PlayerStatus_Pause) gtk_signal_emit_by_name(GTK_OBJECT(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-button_play_pause")), "clicked"); break;
					case PlayerStatus_Play:    while(l_pCurrentInterfacedScenario->m_ePlayerStatus != PlayerStatus_Play)  gtk_signal_emit_by_name(GTK_OBJECT(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-button_play_pause")), "clicked"); break;
					case PlayerStatus_Forward: gtk_signal_emit_by_name(GTK_OBJECT(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-button_forward")), "clicked"); break;
					default: std::cout << "unhandled :(\n"; break;
				}
			}
			else
			{
				float64 l_f64Time=(l_pCurrentInterfacedScenario->m_pPlayer?((l_pCurrentInterfacedScenario->m_pPlayer->getCurrentSimulatedTime()>>22)/1024.0):0);
				if(l_pApplication->m_ui64LastTimeRefresh!=l_f64Time)
				{
					l_pApplication->m_ui64LastTimeRefresh=l_f64Time;

					uint32 l_ui32Milli  = ((uint32)(l_f64Time*1000)%1000);
					uint32 l_ui32Seconds=  ((uint32)l_f64Time)%60;
					uint32 l_ui32Minutes= (((uint32)l_f64Time)/60)%60;
					uint32 l_ui32Hours  =((((uint32)l_f64Time)/60)/60);

					float64 l_f64CPUUsage=(l_pCurrentInterfacedScenario->m_pPlayer?l_pCurrentInterfacedScenario->m_pPlayer->getCPUUsage(OV_UndefinedIdentifier):0);

					std::stringstream ss;
					ss << "Time : ";
					if(l_ui32Hours)                                            ss << l_ui32Hours << "h ";
					if(l_ui32Hours||l_ui32Minutes)                             ss << (l_ui32Minutes<10?"0":"") << l_ui32Minutes << "m ";
					if(l_ui32Hours||l_ui32Minutes||l_ui32Seconds)              ss << (l_ui32Seconds<10?"0":"") << l_ui32Seconds << "s ";
					ss << (l_ui32Milli<100?"0":"") << (l_ui32Milli<10?"0":"") << l_ui32Milli << "ms";

					gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-label_current_time")), ss.str().c_str());

					char l_sCPU[1024];
					sprintf(l_sCPU, "%3.01f%%", l_f64CPUUsage);

					gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-progressbar_cpu_usage")), l_f64CPUUsage*.01);
					gtk_progress_bar_set_text(GTK_PROGRESS_BAR(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-progressbar_cpu_usage")), l_sCPU);
					if(l_pCurrentInterfacedScenario->m_pPlayer&&l_pCurrentInterfacedScenario->m_bDebugCPUUsage)
					{
						// redraws scenario
						l_pCurrentInterfacedScenario->redraw();
					}
				}
			}
		}
		else
		{
			gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-label_current_time")), "Time : 000ms");
			gtk_progress_bar_set_text(GTK_PROGRESS_BAR(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-progressbar_cpu_usage")), "");
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gtk_builder_get_object(l_pApplication->m_pBuilderInterface, "openvibe-progressbar_cpu_usage")), 0);
		}

		if(!l_pApplication->hasRunningScenario())
		{
			System::Time::sleep(50);
		}

		return TRUE;
	}

	gboolean idle_scenario_loop(gpointer pUserData)
	{
		CInterfacedScenario* l_pInterfacedScenario=static_cast<CInterfacedScenario*>(pUserData);
		uint64 l_ui64CurrentTime=System::Time::zgetTime();
		l_pInterfacedScenario->m_pPlayer->loop(l_ui64CurrentTime-l_pInterfacedScenario->m_ui64LastLoopTime);
		l_pInterfacedScenario->m_ui64LastLoopTime=l_ui64CurrentTime;
		return TRUE;
	}

	gboolean timeout_application_loop(gpointer pUserData)
	{
		CApplication* l_pApplication=static_cast<CApplication*>(pUserData);
		if(!l_pApplication->hasRunningScenario() && l_pApplication->m_eCommandLineFlags&CommandLineFlag_NoGui)
		{
			l_pApplication->quitApplicationCB();
			gtk_main_quit();
			return FALSE;
		}
		return TRUE;
	}
}

static ::GtkTargetEntry g_vTargetEntry[]= {
	{ (gchar*)"STRING", 0, 0 },
	{ (gchar*)"text/plain", 0, 0 } };

CApplication::CApplication(const IKernelContext& rKernelContext)
	:m_rKernelContext(rKernelContext)
	,m_pPluginManager(NULL)
	,m_pScenarioManager(NULL)
	,m_pVisualisationManager(NULL)
	,m_pClipboardScenario(NULL)
	,m_eCommandLineFlags(CommandLineFlag_None)
	,m_pBuilderInterface(NULL)
	,m_pMainWindow(NULL)
	,m_pScenarioNotebook(NULL)
	,m_pResourceNotebook(NULL)
	,m_pBoxAlgorithmTreeModel(NULL)
	,m_pBoxAlgorithmTreeView(NULL)
	,m_pAlgorithmTreeModel(NULL)
	,m_pAlgorithmTreeView(NULL)
	,m_bIsQuitting(false)
{
	m_pPluginManager=&m_rKernelContext.getPluginManager();
	m_pScenarioManager=&m_rKernelContext.getScenarioManager();
	m_pVisualisationManager=&m_rKernelContext.getVisualisationManager();
}

void CApplication::initialize(ECommandLineFlag eCommandLineFlags)
{
	m_eCommandLineFlags=eCommandLineFlags;
	m_sSearchTerm = "";

	// Prepares scenario clipboard
	CIdentifier l_oClipboardScenarioIdentifier;
	if(m_pScenarioManager->createScenario(l_oClipboardScenarioIdentifier))
	{
		m_pClipboardScenario=&m_pScenarioManager->getScenario(l_oClipboardScenarioIdentifier);
	}

	m_pBuilderInterface=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "openvibe", NULL);
	gtk_builder_add_from_file(m_pBuilderInterface, OVD_GUI_File, NULL);
	gtk_builder_connect_signals(m_pBuilderInterface, NULL);

	m_pMainWindow=GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe"));

	// Catch delete events when close button is clicked
	g_signal_connect(m_pMainWindow, "delete_event", G_CALLBACK(button_quit_application_cb), this);

	// Connects menu actions
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_undo")),        "activate", G_CALLBACK(menu_undo_cb),               this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_redo")),        "activate", G_CALLBACK(menu_redo_cb),               this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_focus_search")),"activate", G_CALLBACK(menu_focus_search_cb),     this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_copy")),        "activate", G_CALLBACK(menu_copy_selection_cb),     this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_cut")),         "activate", G_CALLBACK(menu_cut_selection_cb),      this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_paste")),       "activate", G_CALLBACK(menu_paste_selection_cb),    this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_delete")),      "activate", G_CALLBACK(menu_delete_selection_cb),   this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_preferences")), "activate", G_CALLBACK(menu_preferences_cb),        this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_new")),         "activate", G_CALLBACK(menu_new_scenario_cb),       this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_open")),        "activate", G_CALLBACK(menu_open_scenario_cb),      this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_save")),        "activate", G_CALLBACK(menu_save_scenario_cb),      this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_save_as")),     "activate", G_CALLBACK(menu_save_scenario_as_cb),   this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_close")),       "activate", G_CALLBACK(menu_close_scenario_cb),     this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_quit")),        "activate", G_CALLBACK(menu_quit_application_cb),   this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_about")),          "activate", G_CALLBACK(menu_about_openvibe_cb),  this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_scenario_about")), "activate", G_CALLBACK(menu_about_scenario_cb),  this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_documentation")),  "activate", G_CALLBACK(menu_browse_documentation_cb),   this);

	// g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_test")),        "activate", G_CALLBACK(menu_test_cb),               this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_new")),       "clicked",  G_CALLBACK(button_new_scenario_cb),     this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_open")),      "clicked",  G_CALLBACK(button_open_scenario_cb),    this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_save")),      "clicked",  G_CALLBACK(button_save_scenario_cb),    this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_save_as")),   "clicked",  G_CALLBACK(button_save_scenario_as_cb), this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_close")),     "clicked",  G_CALLBACK(button_close_scenario_cb),   this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_log_level")),     "clicked",  G_CALLBACK(log_level_cb),                    this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), "toggled",  G_CALLBACK(button_toggle_window_manager_cb), this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_comment")),       "clicked", G_CALLBACK(button_comment_cb),        this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_aboutscenario")), "clicked", G_CALLBACK(button_about_scenario_cb), this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),       "clicked",  G_CALLBACK(stop_scenario_cb),          this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), "clicked",  G_CALLBACK(play_pause_scenario_cb),    this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),       "clicked",  G_CALLBACK(next_scenario_cb),          this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),    "clicked",  G_CALLBACK(forward_scenario_cb),       this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-box_algorithm_title_button_expand")),   "clicked", G_CALLBACK(box_algorithm_title_button_expand_cb),   this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-box_algorithm_title_button_collapse")), "clicked", G_CALLBACK(box_algorithm_title_button_collapse_cb), this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-algorithm_title_button_expand")),   "clicked", G_CALLBACK(algorithm_title_button_expand_cb),   this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-algorithm_title_button_collapse")), "clicked", G_CALLBACK(algorithm_title_button_collapse_cb), this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-box_algorithm_searchbox")), "changed", G_CALLBACK(refresh_search_cb), this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-box_algorithm_searchbox")), "focus-in-event", G_CALLBACK(searchbox_focus_in_cb), this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-box_algorithm_searchbox")), "focus-out-event", G_CALLBACK(searchbox_focus_out_cb), this);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-show_unstable")), "toggled", G_CALLBACK(refresh_search_no_data_cb), this);

	__g_idle_add__(idle_application_loop, this);
	__g_timeout_add__(1000, timeout_application_loop, this);

	// Prepares main notebooks
	m_pScenarioNotebook=GTK_NOTEBOOK(gtk_builder_get_object(m_pBuilderInterface, "openvibe-scenario_notebook"));
	g_signal_connect(G_OBJECT(m_pScenarioNotebook), "switch-page", G_CALLBACK(change_current_scenario_cb), this);
	m_pResourceNotebook=GTK_NOTEBOOK(gtk_builder_get_object(m_pBuilderInterface, "openvibe-resource_notebook"));

	// Creates an empty scnenario
	gtk_notebook_remove_page(m_pScenarioNotebook, 0);
	//newScenarioCB();
	{
		// Prepares box algorithm view
		m_pBoxAlgorithmTreeView=GTK_TREE_VIEW(gtk_builder_get_object(m_pBuilderInterface, "openvibe-box_algorithm_tree"));
		::GtkTreeViewColumn* l_pTreeViewColumnName=gtk_tree_view_column_new();
		::GtkTreeViewColumn* l_pTreeViewColumnDesc=gtk_tree_view_column_new();
		::GtkCellRenderer* l_pCellRendererIcon=gtk_cell_renderer_pixbuf_new();
		::GtkCellRenderer* l_pCellRendererName=gtk_cell_renderer_text_new();
		::GtkCellRenderer* l_pCellRendererDesc=gtk_cell_renderer_text_new();
		gtk_tree_view_column_set_title(l_pTreeViewColumnName, "Name");
		gtk_tree_view_column_set_title(l_pTreeViewColumnDesc, "Description");
		gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererIcon, FALSE);
		gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererName, TRUE);
		gtk_tree_view_column_pack_start(l_pTreeViewColumnDesc, l_pCellRendererDesc, TRUE);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererIcon, "stock-id", Resource_StringStockIcon, NULL);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererName, "text", Resource_StringName, "foreground", Resource_StringColor, NULL);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnDesc, l_pCellRendererDesc, "text", Resource_StringShortDescription, "foreground", Resource_StringColor, NULL);
		gtk_tree_view_column_set_sizing(l_pTreeViewColumnName, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_sizing(l_pTreeViewColumnDesc, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_expand(l_pTreeViewColumnName, FALSE);
		gtk_tree_view_column_set_expand(l_pTreeViewColumnDesc, FALSE);
		gtk_tree_view_column_set_resizable(l_pTreeViewColumnName, TRUE);
		gtk_tree_view_column_set_resizable(l_pTreeViewColumnDesc, TRUE);
		gtk_tree_view_column_set_min_width(l_pTreeViewColumnName, 64);
		gtk_tree_view_column_set_min_width(l_pTreeViewColumnDesc, 64);
		gtk_tree_view_column_set_fixed_width(l_pTreeViewColumnName, 256);
		gtk_tree_view_column_set_fixed_width(l_pTreeViewColumnDesc, 512);
		gtk_tree_view_append_column(m_pBoxAlgorithmTreeView, l_pTreeViewColumnName);
		gtk_tree_view_append_column(m_pBoxAlgorithmTreeView, l_pTreeViewColumnDesc);

		// g_signal_connect(G_OBJECT(m_pBoxAlgorithmTreeView), "querry_tooltip", G_CALLBACK(resource_query_tooltip_cb), this);
		//
		// Prepares box algorithm model
		m_pBoxAlgorithmTreeModel=gtk_tree_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN);

		// Tree Storage for the searches
		gtk_tree_view_set_model(m_pBoxAlgorithmTreeView, GTK_TREE_MODEL(m_pBoxAlgorithmTreeModel) );
	}

	{
		// Prepares algorithm view
		m_pAlgorithmTreeView=GTK_TREE_VIEW(gtk_builder_get_object(m_pBuilderInterface, "openvibe-algorithm_tree"));
		::GtkTreeViewColumn* l_pTreeViewColumnName=gtk_tree_view_column_new();
		::GtkTreeViewColumn* l_pTreeViewColumnDesc=gtk_tree_view_column_new();
		::GtkCellRenderer* l_pCellRendererIcon=gtk_cell_renderer_pixbuf_new();
		::GtkCellRenderer* l_pCellRendererName=gtk_cell_renderer_text_new();
		::GtkCellRenderer* l_pCellRendererDesc=gtk_cell_renderer_text_new();
		gtk_tree_view_column_set_title(l_pTreeViewColumnName, "Name");
		gtk_tree_view_column_set_title(l_pTreeViewColumnDesc, "Description");
		gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererIcon, FALSE);
		gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererName, TRUE);
		gtk_tree_view_column_pack_start(l_pTreeViewColumnDesc, l_pCellRendererDesc, TRUE);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererIcon, "stock-id", Resource_StringStockIcon, NULL);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererName, "text", Resource_StringName, "foreground", Resource_StringColor, NULL);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnDesc, l_pCellRendererDesc, "text", Resource_StringShortDescription, "foreground", Resource_StringColor, NULL);

		gtk_tree_view_column_set_sizing(l_pTreeViewColumnName, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_sizing(l_pTreeViewColumnDesc, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_expand(l_pTreeViewColumnName, FALSE);
		gtk_tree_view_column_set_expand(l_pTreeViewColumnDesc, FALSE);
		gtk_tree_view_column_set_resizable(l_pTreeViewColumnName, TRUE);
		gtk_tree_view_column_set_resizable(l_pTreeViewColumnDesc, TRUE);
		gtk_tree_view_column_set_min_width(l_pTreeViewColumnName, 64);
		gtk_tree_view_column_set_min_width(l_pTreeViewColumnDesc, 64);
		gtk_tree_view_column_set_fixed_width(l_pTreeViewColumnName, 256);
		gtk_tree_view_column_set_fixed_width(l_pTreeViewColumnDesc, 512);
		gtk_tree_view_append_column(m_pAlgorithmTreeView, l_pTreeViewColumnName);
		gtk_tree_view_append_column(m_pAlgorithmTreeView, l_pTreeViewColumnDesc);
		// g_signal_connect(G_OBJECT(m_pAlgorithmTreeView), "querry_tooltip", G_CALLBACK(resource_query_tooltip_cb), this);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-show_unstable")), m_rKernelContext.getConfigurationManager().expandAsBoolean("${Designer_ShowUnstable}"));

		// Prepares algorithm model
		m_pAlgorithmTreeModel=gtk_tree_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN);
		gtk_tree_view_set_model(m_pAlgorithmTreeView, GTK_TREE_MODEL(m_pAlgorithmTreeModel));


	}

	// Prepares drag & drop for box creation
	gtk_drag_source_set(GTK_WIDGET(m_pBoxAlgorithmTreeView), GDK_BUTTON1_MASK, g_vTargetEntry, sizeof(g_vTargetEntry)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
	g_signal_connect(
		G_OBJECT(m_pBoxAlgorithmTreeView),
		"drag_data_get",
		G_CALLBACK(drag_data_get_cb),
		this);

	// Shows main window
	gtk_builder_connect_signals(m_pBuilderInterface, NULL);
	if(m_rKernelContext.getConfigurationManager().expandAsBoolean("${Designer_FullscreenEditor}"))
	{
		gtk_window_maximize(GTK_WINDOW(m_pMainWindow));
	}
	if(!m_rKernelContext.getConfigurationManager().expandAsBoolean("${Designer_ShowAlgorithms}"))
	{
		gtk_notebook_remove_page(GTK_NOTEBOOK(gtk_builder_get_object(m_pBuilderInterface, "openvibe-resource_notebook")), 1);
	}

	// gtk_window_set_icon_name(GTK_WINDOW(m_pMainWindow), "ov-logo");
	// gtk_window_set_icon_from_file(GTK_WINDOW(m_pMainWindow), "../share/openvibe-applications/designer/ov-logo.png", NULL);

	if(!(m_eCommandLineFlags&CommandLineFlag_NoManageSession))
	{
		CIdentifier l_oTokenIdentifier;
		char l_sVarName[1024];
		unsigned i=0;
		do
		{
			::sprintf(l_sVarName, "Designer_LastScenarioFilename_%03i", ++i);
			if((l_oTokenIdentifier=m_rKernelContext.getConfigurationManager().lookUpConfigurationTokenIdentifier(l_sVarName))!=OV_UndefinedIdentifier)
			{
				CString l_sFilename;
				l_sFilename=m_rKernelContext.getConfigurationManager().getConfigurationTokenValue(l_oTokenIdentifier);
				l_sFilename=m_rKernelContext.getConfigurationManager().expand(l_sFilename);
				m_rKernelContext.getLogManager() << LogLevel_Info << "Restoring scenario [" << l_sFilename << "]\n";
				if(!this->openScenario(l_sFilename.toASCIIString()))
				{
					m_rKernelContext.getLogManager() << LogLevel_ImportantWarning << "Failed to restore scenario [" << l_sFilename << "]\n";
				}
			}
		}
		while(l_oTokenIdentifier!=OV_UndefinedIdentifier);
	}
	refresh_search_no_data_cb(NULL, this);
	// Add the designer log listener

	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_debug")), m_rKernelContext.getLogManager().isActive(LogLevel_Debug));
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_bench")), m_rKernelContext.getLogManager().isActive(LogLevel_Benchmark));
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_trace")), m_rKernelContext.getLogManager().isActive(LogLevel_Trace));
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_info")), m_rKernelContext.getLogManager().isActive(LogLevel_Info));
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_warning")), m_rKernelContext.getLogManager().isActive(LogLevel_Warning));
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_impwarning")), m_rKernelContext.getLogManager().isActive(LogLevel_ImportantWarning));
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_error")), m_rKernelContext.getLogManager().isActive(LogLevel_Error));
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_fatal")), m_rKernelContext.getLogManager().isActive(LogLevel_Fatal));

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_debug")), m_rKernelContext.getLogManager().isActive(LogLevel_Debug));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_bench")), m_rKernelContext.getLogManager().isActive(LogLevel_Benchmark));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_trace")), m_rKernelContext.getLogManager().isActive(LogLevel_Trace));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_info")), m_rKernelContext.getLogManager().isActive(LogLevel_Info));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_warning")), m_rKernelContext.getLogManager().isActive(LogLevel_Warning));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_impwarning")), m_rKernelContext.getLogManager().isActive(LogLevel_ImportantWarning));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_error")), m_rKernelContext.getLogManager().isActive(LogLevel_Error));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_fatal")), m_rKernelContext.getLogManager().isActive(LogLevel_Fatal));

	if(!(m_eCommandLineFlags&CommandLineFlag_NoGui))
	{
		m_pLogListenerDesigner = new CLogListenerDesigner(m_rKernelContext, m_pBuilderInterface);
		m_rKernelContext.getLogManager().addListener(m_pLogListenerDesigner);
		g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_clear")),       "clicked",  G_CALLBACK(clear_messages_cb), m_pLogListenerDesigner);

		gtk_widget_show(m_pMainWindow);
	}
}

boolean CApplication::openScenario(const char* sFileName)
{
	CIdentifier l_oScenarioIdentifier;
	if(m_pScenarioManager->createScenario(l_oScenarioIdentifier))
	{
		IScenario& l_rScenario=m_pScenarioManager->getScenario(l_oScenarioIdentifier);

		CMemoryBuffer l_oMemoryBuffer;
		boolean l_bSuccess=false;

		if(::strcmp(sFileName, "-")==0)
		{
			m_rKernelContext.getLogManager() << LogLevel_Trace << "Reading from standard input...\n";
			unsigned int l_uiSize=0;
			FILE* l_pFile=stdin;
			while(1)
			{
				unsigned char c=::fgetc(l_pFile);
				if(::feof(l_pFile)) break;
				l_uiSize++;
				l_oMemoryBuffer.setSize(l_uiSize, false);
				l_oMemoryBuffer[l_uiSize-1]=c;
			}
			m_rKernelContext.getLogManager() << LogLevel_Debug << "Finished reading standard input...\n" << (const char*)&l_oMemoryBuffer[0] << "\n";
		}
		else
		{
			FILE* l_pFile=fopen(sFileName, "rb");
			if(l_pFile)
			{
				::fseek(l_pFile, 0, SEEK_END);
				l_oMemoryBuffer.setSize(::ftell(l_pFile), true);
				::fseek(l_pFile, 0, SEEK_SET);
				::fread(reinterpret_cast<char*>(l_oMemoryBuffer.getDirectPointer()), l_oMemoryBuffer.getSize(), 1, l_pFile);
				::fclose(l_pFile);
			}
		}

		if(l_oMemoryBuffer.getSize())
		{
			CIdentifier l_oImporterIdentifier=m_rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_XMLScenarioImporter);
			if(l_oImporterIdentifier!=OV_UndefinedIdentifier)
			{
				IAlgorithmProxy* l_pImporter=&m_rKernelContext.getAlgorithmManager().getAlgorithm(l_oImporterIdentifier);
				if(l_pImporter)
				{
					m_rKernelContext.getLogManager() << LogLevel_Info << "Importing scenario...\n";

					l_pImporter->initialize();

					TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(l_pImporter->getInputParameter(OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer));
					TParameterHandler < IScenario* > op_pScenario(l_pImporter->getOutputParameter(OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario));

					ip_pMemoryBuffer=&l_oMemoryBuffer;
					op_pScenario=&l_rScenario;

					l_pImporter->process();
					l_pImporter->uninitialize();
					m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*l_pImporter);

					l_bSuccess=true;
				}
			}
		}

		if(l_bSuccess)
		{
			CIdentifier l_oVisualisationWidgetIdentifier;
			CIdentifier l_oBoxIdentifier;

			//ensure visualisation widgets contained in the scenario (if any) appear in the window manager
			//even when the <VisualisationTree> section of a scenario file is missing, erroneous or deprecated
			IVisualisationTree& l_rVisualisationTree = m_rKernelContext.getVisualisationManager().getVisualisationTree(l_rScenario.getVisualisationTreeIdentifier());

			//no visualisation widget was added to visualisation tree : ensure there aren't any in scenario
			while((l_oBoxIdentifier=l_rScenario.getNextBoxIdentifier(l_oBoxIdentifier)) != OV_UndefinedIdentifier)
			{
				if(l_rVisualisationTree.getVisualisationWidgetFromBoxIdentifier(l_oBoxIdentifier)==NULL)
				{
					const IBox* l_pBox = l_rScenario.getBoxDetails(l_oBoxIdentifier);
					CIdentifier l_oAlgorithmIdentifier = l_pBox->getAlgorithmClassIdentifier();
					const IPluginObjectDesc* l_pPOD = m_rKernelContext.getPluginManager().getPluginObjectDescCreating(l_oAlgorithmIdentifier);
					if(l_pPOD != NULL && l_pPOD->hasFunctionality(PluginFunctionality_Visualization))
					{
						//a visualisation widget was found in scenario : manually add it to visualisation tree
						l_rVisualisationTree.addVisualisationWidget(
								l_oVisualisationWidgetIdentifier,
								l_pBox->getName(),
								EVisualisationWidget_VisualisationBox,
								OV_UndefinedIdentifier,
								0,
								l_pBox->getIdentifier(),
								0);
					}
				}
			}

			// Closes first unnamed scenario
			if(m_vInterfacedScenario.size()==1)
			{
				if(m_vInterfacedScenario[0]->m_bHasBeenModified==false && !m_vInterfacedScenario[0]->m_bHasFileName)
				{
					CIdentifier l_oScenarioIdentifier=m_vInterfacedScenario[0]->m_oScenarioIdentifier;
					delete m_vInterfacedScenario[0];
					m_pScenarioManager->releaseScenario(l_oScenarioIdentifier);
					m_vInterfacedScenario.clear();
				}
			}

			// Creates interfaced scenario
			CInterfacedScenario* l_pInterfacedScenario=new CInterfacedScenario(m_rKernelContext, *this, l_rScenario, l_oScenarioIdentifier, *m_pScenarioNotebook, OVD_GUI_File, OVD_GUI_Settings_File);
			if(l_pInterfacedScenario->m_pDesignerVisualisation != NULL)
			{
				l_pInterfacedScenario->m_pDesignerVisualisation->setDeleteEventCB(&::delete_designer_visualisation_cb, this);
				l_pInterfacedScenario->m_pDesignerVisualisation->load();
			}
			l_pInterfacedScenario->snapshotCB();
			l_pInterfacedScenario->m_sFileName=sFileName;
			l_pInterfacedScenario->m_bHasFileName=true;
			l_pInterfacedScenario->m_bHasBeenModified=false;
			l_pInterfacedScenario->updateScenarioLabel();
			m_vInterfacedScenario.push_back(l_pInterfacedScenario);
			gtk_notebook_set_current_page(m_pScenarioNotebook, gtk_notebook_get_n_pages(m_pScenarioNotebook)-1);
			//this->changeCurrentScenario(gtk_notebook_get_n_pages(m_pScenarioNotebook)-1);

			return true;
		}
		else
		{
			m_rKernelContext.getLogManager() << LogLevel_Warning << "Importing scenario failed...\n";

			m_pScenarioManager->releaseScenario(l_oScenarioIdentifier);

			std::stringstream l_oStringStream;
			l_oStringStream << "The requested file: " << sFileName << "\n";
			l_oStringStream << "may either not be an OpenViBE scenario file, \n";
			l_oStringStream << "be corrupted or not be compatible with \n";
			l_oStringStream << "the selected scenario importer...";

			::GtkWidget* l_pErrorDialog=gtk_message_dialog_new(
					NULL,
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_WARNING,
					GTK_BUTTONS_OK,
					"Scenario importation process failed !");
			gtk_message_dialog_format_secondary_text(
					GTK_MESSAGE_DIALOG(l_pErrorDialog), "%s", l_oStringStream.str().c_str());
			gtk_dialog_run(GTK_DIALOG(l_pErrorDialog));
			gtk_widget_destroy(l_pErrorDialog);

		}
	}
	return false;
}

CString CApplication::getWorkingDirectory(void)
{
	CString l_sWorkingDirectory=m_rKernelContext.getConfigurationManager().expand("${Designer_DefaultWorkingDirectory}");

	CInterfacedScenario* l_pCurrentScenario=this->getCurrentInterfacedScenario();
	if(l_pCurrentScenario)
	{
		if(l_pCurrentScenario->m_bHasFileName)
		{
			char* l_sCurrentDirectory=g_path_get_dirname(l_pCurrentScenario->m_sFileName.c_str());
			l_sWorkingDirectory=l_sCurrentDirectory;
			g_free(l_sCurrentDirectory);
		}
	}

	if(!g_path_is_absolute(l_sWorkingDirectory.toASCIIString()))
	{
		char* l_sCurrentDirectory=g_get_current_dir();
		l_sWorkingDirectory=l_sCurrentDirectory+CString("/")+l_sWorkingDirectory;
		g_free(l_sCurrentDirectory);
	}

	return l_sWorkingDirectory;
}

boolean CApplication::hasRunningScenario(void)
{
	vector<CInterfacedScenario*>::const_iterator itInterfacedScenario;
	for(itInterfacedScenario=m_vInterfacedScenario.begin(); itInterfacedScenario!=m_vInterfacedScenario.end(); itInterfacedScenario++)
	{
		if((*itInterfacedScenario)->m_pPlayer)
		{
			return true;
		}
	}
	return false;
}

boolean CApplication::hasUnsavedScenario(void)
{
	vector<CInterfacedScenario*>::const_iterator itInterfacedScenario;
	for(itInterfacedScenario=m_vInterfacedScenario.begin(); itInterfacedScenario!=m_vInterfacedScenario.end(); itInterfacedScenario++)
	{
		if((*itInterfacedScenario)->m_bHasBeenModified)
		{
			return true;
		}
	}
	return false;
}

CInterfacedScenario* CApplication::getCurrentInterfacedScenario(void)
{
	uint32 l_ui32Index=(uint32)gtk_notebook_get_current_page(m_pScenarioNotebook);
	if(l_ui32Index<m_vInterfacedScenario.size())
	{
		return m_vInterfacedScenario[l_ui32Index];
	}
	return NULL;
}

void CApplication::dragDataGetCB(::GtkWidget* pWidget, ::GdkDragContext* pDragContex, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "dragDataGetCB\n";

	::GtkTreeView* l_pTreeView=GTK_TREE_VIEW(gtk_builder_get_object(m_pBuilderInterface, "openvibe-box_algorithm_tree"));
	::GtkTreeSelection* l_pTreeSelection=gtk_tree_view_get_selection(l_pTreeView);
	::GtkTreeModel* l_pTreeModel=NULL;
	::GtkTreeIter l_oTreeIter;
	if(gtk_tree_selection_get_selected(l_pTreeSelection, &l_pTreeModel, &l_oTreeIter))
	{
		const char* l_sBoxAlgorithmIdentifier=NULL;
		gtk_tree_model_get(
				l_pTreeModel, &l_oTreeIter,
				Resource_StringIdentifier, &l_sBoxAlgorithmIdentifier,
				-1);
		if(l_sBoxAlgorithmIdentifier)
		{
			gtk_selection_data_set(
					pSelectionData,
					GDK_SELECTION_TYPE_STRING,
					8,
					(const guchar*)l_sBoxAlgorithmIdentifier,
					strlen(l_sBoxAlgorithmIdentifier)+1);
		}
	}
}

void CApplication::undoCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "undoCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=this->getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->undoCB();
	}
}

void CApplication::redoCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "redoCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=this->getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->redoCB();
	}
}

void CApplication::copySelectionCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "copySelectionCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=this->getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->copySelection();
	}
}

void CApplication::cutSelectionCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "cutSelectionCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=this->getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->cutSelection();
	}
}

void CApplication::pasteSelectionCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "pasteSelectionCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=this->getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->pasteSelection();
	}
}

void CApplication::deleteSelectionCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "deleteSelectionCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=this->getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->deleteSelection();
	}
}

void CApplication::preferencesCB(void)
{
	enum
	{
		Resource_TokenName,
		Resource_TokenValue,
		Resource_TokenExpand,
	};

	m_rKernelContext.getLogManager() << LogLevel_Trace << "preferencesCB\n";
	::GtkBuilder* l_pBuilderInterface=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "configuration_manager", NULL);
	gtk_builder_add_from_file(l_pBuilderInterface, OVD_GUI_File, NULL);
	gtk_builder_connect_signals(l_pBuilderInterface, NULL);

	::GtkWidget* l_pConfigurationManager=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "configuration_manager"));
	::GtkTreeView* l_pConfigurationManagerTreeView=GTK_TREE_VIEW(gtk_builder_get_object(l_pBuilderInterface, "configuration_manager-treeview"));

	// Prepares tree view
	::GtkTreeViewColumn* l_pTreeViewColumnTokenName=gtk_tree_view_column_new();
	::GtkTreeViewColumn* l_pTreeViewColumnTokenValue=gtk_tree_view_column_new();
	::GtkTreeViewColumn* l_pTreeViewColumnTokenExpand=gtk_tree_view_column_new();
	::GtkCellRenderer* l_pCellRendererTokenName=gtk_cell_renderer_text_new();
	::GtkCellRenderer* l_pCellRendererTokenValue=gtk_cell_renderer_text_new();
	::GtkCellRenderer* l_pCellRendererTokenExpand=gtk_cell_renderer_text_new();
	gtk_tree_view_column_set_title(l_pTreeViewColumnTokenName, "Token name");
	gtk_tree_view_column_set_title(l_pTreeViewColumnTokenValue, "Token value");
	gtk_tree_view_column_set_title(l_pTreeViewColumnTokenExpand, "Expanded token value");
	gtk_tree_view_column_pack_start(l_pTreeViewColumnTokenName, l_pCellRendererTokenName, TRUE);
	gtk_tree_view_column_pack_start(l_pTreeViewColumnTokenValue, l_pCellRendererTokenValue, TRUE);
	gtk_tree_view_column_pack_start(l_pTreeViewColumnTokenExpand, l_pCellRendererTokenExpand, TRUE);
	gtk_tree_view_column_set_attributes(l_pTreeViewColumnTokenName, l_pCellRendererTokenName, "text", Resource_TokenName, NULL);
	gtk_tree_view_column_set_attributes(l_pTreeViewColumnTokenValue, l_pCellRendererTokenValue, "text", Resource_TokenValue, NULL);
	gtk_tree_view_column_set_attributes(l_pTreeViewColumnTokenExpand, l_pCellRendererTokenExpand, "text", Resource_TokenExpand, NULL);
	gtk_tree_view_column_set_sort_column_id(l_pTreeViewColumnTokenName, Resource_TokenName);
	gtk_tree_view_column_set_sort_column_id(l_pTreeViewColumnTokenValue, Resource_TokenValue);
	gtk_tree_view_column_set_sort_column_id(l_pTreeViewColumnTokenExpand, Resource_TokenExpand);
	gtk_tree_view_column_set_sizing(l_pTreeViewColumnTokenName, GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_sizing(l_pTreeViewColumnTokenValue, GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_sizing(l_pTreeViewColumnTokenExpand, GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_expand(l_pTreeViewColumnTokenName, TRUE);
	gtk_tree_view_column_set_expand(l_pTreeViewColumnTokenValue, TRUE);
	gtk_tree_view_column_set_expand(l_pTreeViewColumnTokenExpand, TRUE);
	gtk_tree_view_column_set_resizable(l_pTreeViewColumnTokenName, TRUE);
	gtk_tree_view_column_set_resizable(l_pTreeViewColumnTokenValue, TRUE);
	gtk_tree_view_column_set_resizable(l_pTreeViewColumnTokenExpand, TRUE);
	gtk_tree_view_column_set_min_width(l_pTreeViewColumnTokenName, 256);
	gtk_tree_view_column_set_min_width(l_pTreeViewColumnTokenValue, 256);
	gtk_tree_view_column_set_min_width(l_pTreeViewColumnTokenExpand, 256);
	gtk_tree_view_append_column(l_pConfigurationManagerTreeView, l_pTreeViewColumnTokenName);
	gtk_tree_view_append_column(l_pConfigurationManagerTreeView, l_pTreeViewColumnTokenValue);
	gtk_tree_view_append_column(l_pConfigurationManagerTreeView, l_pTreeViewColumnTokenExpand);
	gtk_tree_view_column_set_sort_indicator(l_pTreeViewColumnTokenName, TRUE);

	// Prepares tree model
	CIdentifier l_oTokenIdentifier;
	::GtkTreeStore* l_pConfigurationManagerTreeModel=gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	while((l_oTokenIdentifier=m_rKernelContext.getConfigurationManager().getNextConfigurationTokenIdentifier(l_oTokenIdentifier))!=OV_UndefinedIdentifier)
	{
		::GtkTreeIter l_oGtkIterChild;
		CString l_sTokenName=m_rKernelContext.getConfigurationManager().getConfigurationTokenName(l_oTokenIdentifier);
		CString l_sTokenValue=m_rKernelContext.getConfigurationManager().getConfigurationTokenValue(l_oTokenIdentifier);
		CString l_sTokenExpand=m_rKernelContext.getConfigurationManager().expand(l_sTokenValue);
		gtk_tree_store_append(
				l_pConfigurationManagerTreeModel,
				&l_oGtkIterChild,
				NULL);
		gtk_tree_store_set(
				l_pConfigurationManagerTreeModel,
				&l_oGtkIterChild,
				Resource_TokenName, l_sTokenName.toASCIIString(),
				Resource_TokenValue, l_sTokenValue.toASCIIString(),
				Resource_TokenExpand, l_sTokenExpand.toASCIIString(),
				-1);
	}
	gtk_tree_view_set_model(l_pConfigurationManagerTreeView, GTK_TREE_MODEL(l_pConfigurationManagerTreeModel));
	g_signal_emit_by_name(l_pTreeViewColumnTokenName, "clicked");

	gtk_dialog_run(GTK_DIALOG(l_pConfigurationManager));
	gtk_widget_destroy(l_pConfigurationManager);

	g_object_unref(l_pConfigurationManagerTreeModel);
	g_object_unref(l_pBuilderInterface);
}

void CApplication::testCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "testCB\n";
}

void CApplication::newScenarioCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "newScenarioCB\n";

	CIdentifier l_oScenarioIdentifier;
	if(m_pScenarioManager->createScenario(l_oScenarioIdentifier))
	{
		IScenario& l_rScenario=m_pScenarioManager->getScenario(l_oScenarioIdentifier);
		CInterfacedScenario* l_pInterfacedScenario=new CInterfacedScenario(m_rKernelContext, *this, l_rScenario, l_oScenarioIdentifier, *m_pScenarioNotebook, OVD_GUI_File, OVD_GUI_Settings_File);
		if(l_pInterfacedScenario->m_pDesignerVisualisation != NULL)
		{
			l_pInterfacedScenario->m_pDesignerVisualisation->setDeleteEventCB(&::delete_designer_visualisation_cb, this);
			l_pInterfacedScenario->m_pDesignerVisualisation->newVisualisationWindow("Default window");
		}
		l_pInterfacedScenario->updateScenarioLabel();
		m_vInterfacedScenario.push_back(l_pInterfacedScenario);
		gtk_notebook_set_current_page(m_pScenarioNotebook, gtk_notebook_get_n_pages(m_pScenarioNotebook)-1);
		//this->changeCurrentScenario(gtk_notebook_get_n_pages(m_pScenarioNotebook)-1);
	}
}

void CApplication::openScenarioCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "openScenarioCB\n";

	::GtkFileFilter* l_pFileFilterXML=gtk_file_filter_new();
	::GtkFileFilter* l_pFileFilterAll=gtk_file_filter_new();
	gtk_file_filter_set_name(l_pFileFilterXML, "OpenViBE XML scenario");
	gtk_file_filter_add_pattern(l_pFileFilterXML, "*.xml");
	gtk_file_filter_set_name(l_pFileFilterAll, "All files");
	gtk_file_filter_add_pattern(l_pFileFilterAll, "*");

	::GtkWidget* l_pWidgetDialogOpen=gtk_file_chooser_dialog_new(
			"Select scenario to open...",
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_pFileFilterXML);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_pFileFilterAll);
	gtk_file_chooser_set_current_folder(
			GTK_FILE_CHOOSER(l_pWidgetDialogOpen),
			this->getWorkingDirectory().toASCIIString());

	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(l_pWidgetDialogOpen),true);

	if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogOpen))==GTK_RESPONSE_ACCEPT)
	{
		//char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));
		GSList * l_pFile, *l_pList;
		l_pFile = l_pList = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));
		while(l_pFile)
		{
			char* l_sFileName = (char*)l_pFile->data;
			char* l_pBackslash = NULL;
			while((l_pBackslash = ::strchr(l_sFileName, '\\'))!=NULL)
			{
				*l_pBackslash = '/';
			}
			this->openScenario(l_sFileName);
			g_free(l_pFile->data);
			l_pFile=l_pFile->next;
		}
		g_slist_free(l_pList);
	}
	gtk_widget_destroy(l_pWidgetDialogOpen);
}

void CApplication::saveScenarioCB(CInterfacedScenario* pScenario)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "saveScenarioCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=pScenario?pScenario:getCurrentInterfacedScenario();
	if(!l_pCurrentInterfacedScenario)
	{
		return;
	}
	if(!l_pCurrentInterfacedScenario->m_bHasFileName)
	{
		saveScenarioAsCB(pScenario);
	}
	else
	{
		boolean l_bSuccess=false;
		CMemoryBuffer l_oMemoryBuffer;
		CIdentifier l_oClassIdentifier(l_pCurrentInterfacedScenario->m_oExporterIdentifier);

		CIdentifier l_oExporterIdentifier=m_rKernelContext.getAlgorithmManager().createAlgorithm(l_oClassIdentifier!=OV_UndefinedIdentifier?l_oClassIdentifier:OVP_GD_ClassId_Algorithm_XMLScenarioExporter);
		if(l_oExporterIdentifier!=OV_UndefinedIdentifier)
		{
			IAlgorithmProxy* l_pExporter=&m_rKernelContext.getAlgorithmManager().getAlgorithm(l_oExporterIdentifier);
			if(l_pExporter)
			{
				m_rKernelContext.getLogManager() << LogLevel_Info << "Exporting scenario...\n";

				l_pExporter->initialize();

				TParameterHandler < const IScenario* > ip_pScenario(l_pExporter->getInputParameter(OVTK_Algorithm_ScenarioExporter_InputParameterId_Scenario));
				TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(l_pExporter->getOutputParameter(OVTK_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer));

				ip_pScenario=&l_pCurrentInterfacedScenario->m_rScenario;
				op_pMemoryBuffer=&l_oMemoryBuffer;

				l_pExporter->process();
				l_pExporter->uninitialize();
				m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*l_pExporter);

				l_bSuccess=(l_oMemoryBuffer.getSize()!=0);

				l_pCurrentInterfacedScenario->snapshotCB();
				l_pCurrentInterfacedScenario->m_bHasFileName=true;
				l_pCurrentInterfacedScenario->m_bHasBeenModified=false;
				l_pCurrentInterfacedScenario->updateScenarioLabel();

				std::ofstream l_oFile(l_pCurrentInterfacedScenario->m_sFileName.c_str(), ios::binary);
				if(l_oFile.good())
				{
					l_oFile.write(reinterpret_cast<const char*>(l_oMemoryBuffer.getDirectPointer()), l_oMemoryBuffer.getSize());
					l_oFile.close();
				}
			}
		}

		if(!l_bSuccess)
		{
			m_rKernelContext.getLogManager() << LogLevel_Warning << "Exporting scenario failed...\n";
		}
	}
}

void CApplication::saveScenarioAsCB(CInterfacedScenario* pScenario)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "saveScenarioAsCB\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=pScenario?pScenario:getCurrentInterfacedScenario();
	if(!l_pCurrentInterfacedScenario)
	{
		return;
	}

	::GtkFileFilter* l_pFileFilterXML=gtk_file_filter_new();
	// ::GtkFileFilter* l_pFileFilterSVG=gtk_file_filter_new();
	::GtkFileFilter* l_pFileFilterAll=gtk_file_filter_new();
	gtk_file_filter_set_name(l_pFileFilterXML, "OpenViBE XML scenario");
	gtk_file_filter_add_pattern(l_pFileFilterXML, "*.xml");
	// gtk_file_filter_set_name(l_pFileFilterSVG, "SVG image");
	// gtk_file_filter_add_pattern(l_pFileFilterSVG, "*.svg");
	gtk_file_filter_set_name(l_pFileFilterAll, "All files");
	gtk_file_filter_add_pattern(l_pFileFilterAll, "*");

	::GtkWidget* l_pWidgetDialogSaveAs=gtk_file_chooser_dialog_new(
			"Select scenario to save...",
			NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs), l_pFileFilterXML);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs), l_pFileFilterAll);
	// gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs), true);
	if(l_pCurrentInterfacedScenario->m_bHasFileName)
	{
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs), l_pCurrentInterfacedScenario->m_sFileName.c_str());
	}
	else
	{
		gtk_file_chooser_set_current_folder(
				GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs),
				this->getWorkingDirectory().toASCIIString());
	}
	if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogSaveAs))==GTK_RESPONSE_ACCEPT)
	{
		//ensure file extension is added after filename
		char* l_sTempFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs));
		char* l_pBackslash = NULL;
		while((l_pBackslash = ::strchr(l_sTempFileName, '\\'))!=NULL)
		{
			*l_pBackslash = '/';
		}

		char l_sFileName[1024];
		::sprintf(l_sFileName, "%s", l_sTempFileName);
		g_free(l_sTempFileName);

		::GtkFileFilter* l_pFileFilter=gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs));
		if(l_pFileFilter == l_pFileFilterXML)
		{
			if(::strlen(l_sFileName) > 4 && ::_strcmpi(l_sFileName+strlen(l_sFileName)-4, ".xml")!=0)
			{
				::strcat(l_sFileName, ".xml");
			}
		}

		l_pCurrentInterfacedScenario->m_sFileName=l_sFileName;
		l_pCurrentInterfacedScenario->m_bHasFileName=true;
		l_pCurrentInterfacedScenario->m_bHasBeenModified=false;
		l_pCurrentInterfacedScenario->updateScenarioLabel();

		if(true /* l_pFileFilter==l_pFileFilterSVG */)
		{
			l_pCurrentInterfacedScenario->m_oExporterIdentifier=OVP_GD_ClassId_Algorithm_XMLScenarioExporter;
		}
		else
		{
			l_pCurrentInterfacedScenario->m_oExporterIdentifier=OVP_GD_ClassId_Algorithm_XMLScenarioExporter;
		}

		saveScenarioCB(l_pCurrentInterfacedScenario);
	}
	gtk_widget_destroy(l_pWidgetDialogSaveAs);
}

void CApplication::closeScenarioCB(CInterfacedScenario* pInterfacedScenario)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "closeScenarioCB\n";

	if(!pInterfacedScenario)
	{
		return;
	}
	if(pInterfacedScenario->isLocked())
	{
		::GtkBuilder* l_pBuilder=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "about", NULL);
		gtk_builder_add_from_file(l_pBuilder, OVD_GUI_File, NULL);
		gtk_builder_connect_signals(l_pBuilder, NULL);

		::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilder, "dialog_running_scenario"));
		gtk_builder_connect_signals(l_pBuilder, NULL);
		// gtk_dialog_set_response_sensitive(GTK_DIALOG(l_pDialog), GTK_RESPONSE_CLOSE, true);
		gtk_dialog_run(GTK_DIALOG(l_pDialog));
		gtk_widget_destroy(l_pDialog);
		g_object_unref(l_pBuilder);
		return;
	}
	if(pInterfacedScenario->m_bHasBeenModified)
	{
		gint l_iResponseId;

		::GtkBuilder* l_pBuilder=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "about", NULL);
		gtk_builder_add_from_file(l_pBuilder, OVD_GUI_File, NULL);
		gtk_builder_connect_signals(l_pBuilder, NULL);

		::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilder, "dialog_unsaved_scenario"));
		gtk_builder_connect_signals(l_pBuilder, NULL);
		// gtk_dialog_set_response_sensitive(GTK_DIALOG(l_pDialog), GTK_RESPONSE_CLOSE, true);
		l_iResponseId=gtk_dialog_run(GTK_DIALOG(l_pDialog));
		gtk_widget_destroy(l_pDialog);
		g_object_unref(l_pBuilder);

		switch(l_iResponseId)
		{
			case GTK_RESPONSE_OK:
				this->saveScenarioCB(pInterfacedScenario);
				if(pInterfacedScenario->m_bHasBeenModified)
				{
					return;
				}
				break;
			case GTK_RESPONSE_DELETE_EVENT:
			case GTK_RESPONSE_CANCEL:
				return;
			default:
				break;
		}
	}

	vector<CInterfacedScenario*>::iterator i=m_vInterfacedScenario.begin();
	while(i!=m_vInterfacedScenario.end() && *i!=pInterfacedScenario) i++;
	if(i!=m_vInterfacedScenario.end())
	{
		CIdentifier l_oScenarioIdentifier=pInterfacedScenario->m_oScenarioIdentifier;
		delete pInterfacedScenario;
		m_pScenarioManager->releaseScenario(l_oScenarioIdentifier);
		m_vInterfacedScenario.erase(i);
		//when closing last open scenario, no "switch-page" event is triggered so we manually handle this case
		if(m_vInterfacedScenario.empty() == true)
		{
			changeCurrentScenario(-1);
		}
	}
}

void CApplication::deleteDesignerVisualisationCB()
{
	//untoggle window manager button when its associated dialog is closed
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), FALSE);

	CInterfacedScenario* l_pCurrentInterfacedScenario = getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->snapshotCB();
	}
}

void CApplication::toggleDesignerVisualisationCB()
{
	CInterfacedScenario* l_pCurrentInterfacedScenario = getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario != NULL && l_pCurrentInterfacedScenario->isLocked() == false)
	{
		uint32 l_ui32Index=(uint32)gtk_notebook_get_current_page(m_pScenarioNotebook);
		if(l_ui32Index<m_vInterfacedScenario.size())
		{
			m_vInterfacedScenario[l_ui32Index]->toggleDesignerVisualisation();
		}
	}
}

void CApplication::aboutOpenViBECB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "CApplication::aboutOpenViBECB\n";
	::GtkBuilder* l_pBuilder=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "about", NULL);
	gtk_builder_add_from_file(l_pBuilder, OVD_GUI_File, NULL);
	gtk_builder_connect_signals(l_pBuilder, NULL);

	::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilder, "about"));
	gtk_builder_connect_signals(l_pBuilder, NULL);
	gtk_dialog_set_response_sensitive(GTK_DIALOG(l_pDialog), GTK_RESPONSE_CLOSE, true);
	gtk_dialog_run(GTK_DIALOG(l_pDialog));
	gtk_widget_destroy(l_pDialog);
	g_object_unref(l_pBuilder);
}

void CApplication::aboutScenarioCB(CInterfacedScenario* pScenario)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "CApplication::aboutScenarioCB\n";
	if(pScenario && !pScenario->isLocked())
	{
		pScenario->contextMenuScenarioAboutCB();
	}
}

void CApplication::browseDocumentationCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "CApplication::browseDocumentationCB\n";
	CString l_sCommand = m_rKernelContext.getConfigurationManager().expand("${Designer_WebBrowserCommand} \"${Designer_WebBrowserOpenViBEHomepage}/documentation-index\"");
	int l_iResult = system(l_sCommand.toASCIIString());

	if(l_iResult<0)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Could not launch command " << l_sCommand << "\n";
	}
}

void CApplication::addCommentCB(
		CInterfacedScenario* pScenario)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "CApplication::addCommentCB\n";
	if(pScenario && !pScenario->isLocked())
	{
		pScenario->addCommentCB();
	}
}

IPlayer* CApplication::getPlayer(void)
{
	CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
	return (l_pCurrentInterfacedScenario?l_pCurrentInterfacedScenario->m_pPlayer:NULL);
}

void CApplication::createPlayer(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "createPlayer\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario && !l_pCurrentInterfacedScenario->m_pPlayer)
	{
		// create a snapshot so settings override does not modify the scenario !
		l_pCurrentInterfacedScenario->snapshotCB(false);

		// generate player windows
		l_pCurrentInterfacedScenario->createPlayerVisualisation();

		m_rKernelContext.getPlayerManager().createPlayer(l_pCurrentInterfacedScenario->m_oPlayerIdentifier);
		CIdentifier l_oScenarioIdentifier=l_pCurrentInterfacedScenario->m_oScenarioIdentifier;
		CIdentifier l_oPlayerIdentifier=l_pCurrentInterfacedScenario->m_oPlayerIdentifier;
		l_pCurrentInterfacedScenario->m_pPlayer=&m_rKernelContext.getPlayerManager().getPlayer(l_oPlayerIdentifier);
		l_pCurrentInterfacedScenario->m_pPlayer->setScenario(l_oScenarioIdentifier);
		l_pCurrentInterfacedScenario->m_pPlayer->initialize();
		l_pCurrentInterfacedScenario->m_ui64LastLoopTime=System::Time::zgetTime();

		//set up idle function
		__g_idle_add__(idle_scenario_loop, l_pCurrentInterfacedScenario);

		// redraws scenario
		l_pCurrentInterfacedScenario->redraw();
	}
}

void CApplication::releasePlayer(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "releasePlayer\n";

	CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario && l_pCurrentInterfacedScenario->m_pPlayer)
	{
		// removes idle function
		g_idle_remove_by_data(l_pCurrentInterfacedScenario);

		l_pCurrentInterfacedScenario->m_pPlayer->uninitialize();

		m_rKernelContext.getPlayerManager().releasePlayer(l_pCurrentInterfacedScenario->m_oPlayerIdentifier);

		l_pCurrentInterfacedScenario->m_oPlayerIdentifier=OV_UndefinedIdentifier;
		l_pCurrentInterfacedScenario->m_pPlayer=NULL;

		// restore the snapshot so settings override does not modify the scenario !
		l_pCurrentInterfacedScenario->undoCB(false);

		// destroy player windows
		l_pCurrentInterfacedScenario->releasePlayerVisualisation();

		// redraws scenario
		l_pCurrentInterfacedScenario->redraw();
	}
}

void CApplication::stopScenarioCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "stopScenarioCB\n";

	if(this->getCurrentInterfacedScenario()->m_ePlayerStatus == PlayerStatus_Play || this->getCurrentInterfacedScenario()->m_ePlayerStatus == PlayerStatus_Pause || this->getCurrentInterfacedScenario()->m_ePlayerStatus == PlayerStatus_Forward)
	{
		this->getPlayer()->stop();
		this->getCurrentInterfacedScenario()->m_ePlayerStatus=this->getPlayer()->getStatus();
		this->releasePlayer();

		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),          false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")),    true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),          true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),       true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), true);
		gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), GTK_STOCK_MEDIA_PLAY);
	}
}

void CApplication::pauseScenarioCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "pauseScenarioCB\n";

	this->createPlayer();
	this->getPlayer()->pause();
	this->getCurrentInterfacedScenario()->m_ePlayerStatus=this->getPlayer()->getStatus();

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")),    true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),       true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), false);
	gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), GTK_STOCK_MEDIA_PLAY);
}

void CApplication::nextScenarioCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "nextScenarioCB\n";

	this->createPlayer();
	this->getPlayer()->step();
	this->getCurrentInterfacedScenario()->m_ePlayerStatus=this->getPlayer()->getStatus();

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")),    true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),       true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), false);
	gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), GTK_STOCK_MEDIA_PLAY);
}

void CApplication::playScenarioCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "playScenarioCB\n";

	this->createPlayer();
	this->getPlayer()->play();
	this->getCurrentInterfacedScenario()->m_ePlayerStatus=this->getPlayer()->getStatus();

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")),    true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),       true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), false);
	gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), GTK_STOCK_MEDIA_PAUSE);
}

void CApplication::forwardScenarioCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "forwardScenarioCB\n";

	this->createPlayer();
	this->getPlayer()->forward();
	this->getCurrentInterfacedScenario()->m_ePlayerStatus=this->getPlayer()->getStatus();

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")),    true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),          true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),       false);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), false);
	gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), GTK_STOCK_MEDIA_PLAY);
}

boolean CApplication::quitApplicationCB(void)
{
	std::vector < CInterfacedScenario* >::iterator it;

	CIdentifier l_oIdentifier;
	m_rKernelContext.getLogManager() << LogLevel_Trace << "quitApplicationCB\n";

	// can't quit while scenarios are running
	if(this->hasRunningScenario() == true)
	{
		::GtkBuilder* l_pBuilder=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "about", NULL);
		gtk_builder_add_from_file(l_pBuilder, OVD_GUI_File, NULL);
		gtk_builder_connect_signals(l_pBuilder, NULL);

		::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilder, "dialog_running_scenario_global"));
		gtk_builder_connect_signals(l_pBuilder, NULL);
		// gtk_dialog_set_response_sensitive(GTK_DIALOG(l_pDialog), GTK_RESPONSE_CLOSE, true);
		gtk_dialog_run(GTK_DIALOG(l_pDialog));
		gtk_widget_destroy(l_pDialog);
		g_object_unref(l_pBuilder);

		// prevent Gtk from handling delete_event and killing app
		return false;
	}

	// can't quit while scenarios are unsaved
	if(this->hasUnsavedScenario() == true)
	{
		gint l_iResponseId;

		::GtkBuilder* l_pBuilder=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "about", NULL);
		gtk_builder_add_from_file(l_pBuilder, OVD_GUI_File, NULL);
		gtk_builder_connect_signals(l_pBuilder, NULL);

		::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilder, "dialog_unsaved_scenario_global"));
		gtk_builder_connect_signals(l_pBuilder, NULL);
		l_iResponseId=gtk_dialog_run(GTK_DIALOG(l_pDialog));
		gtk_widget_destroy(l_pDialog);
		g_object_unref(l_pBuilder);

		switch(l_iResponseId)
		{
			case GTK_RESPONSE_OK:
				for(std::vector < CInterfacedScenario* >::iterator i=m_vInterfacedScenario.begin(); i!=m_vInterfacedScenario.end(); i++)
				{
					this->saveScenarioCB(*i);
				}
				if(this->hasUnsavedScenario())
				{
					// prevent Gtk from handling delete_event and killing app
					return false;
				}
				break;
			case GTK_RESPONSE_DELETE_EVENT:
			case GTK_RESPONSE_CANCEL:
				// prevent Gtk from handling delete_event and killing app
				return false;
			default:
				break;
		}
	}

	// Switch to quitting mode
	m_bIsQuitting=true;

	// Saves opened scenarios
	if(!(m_eCommandLineFlags&CommandLineFlag_NoManageSession))
	{
		FILE* l_pFile=::fopen(m_rKernelContext.getConfigurationManager().expand("${CustomConfigurationApplication}").toASCIIString(), "wt");
		if(l_pFile)
		{
			unsigned int i=1;
			::fprintf(l_pFile, "# This file is generated\n");
			::fprintf(l_pFile, "# Do not modify\n");
			::fprintf(l_pFile, "\n");
			::fprintf(l_pFile, "Designer_ShowUnstable = %s\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-show_unstable")))?"True":"False");
			::fprintf(l_pFile, "# Last files opened in the designer\n");
			for(it=m_vInterfacedScenario.begin(); it!=m_vInterfacedScenario.end(); it++)
			{
				if((*it)->m_sFileName != "")
				{
					::fprintf(l_pFile, "Designer_LastScenarioFilename_%03i = %s\n", i, (*it)->m_sFileName.c_str());
					i++;
				}
			}
			::fclose(l_pFile);
		}
	}

	// Clears all existing interfaced scenarios
	for(it=m_vInterfacedScenario.begin(); it!=m_vInterfacedScenario.end(); it++)
	{
		delete *it;
	}

	// Clears all existing scenarios
	vector < CIdentifier > l_vScenarioIdentifiers;
	while((l_oIdentifier=m_rKernelContext.getScenarioManager().getNextScenarioIdentifier(l_oIdentifier))!=OV_UndefinedIdentifier)
	{
		l_vScenarioIdentifiers.push_back(l_oIdentifier);
	}
	for(vector < CIdentifier > ::iterator i=l_vScenarioIdentifiers.begin(); i!=l_vScenarioIdentifiers.end(); i++)
	{
		m_rKernelContext.getScenarioManager().releaseScenario(*i);
	}

	// release the log manager and free the memory

	/*
	   m_rKernelContext.getLogManager().removeListener( m_pLogListenerDesigner );
	   delete m_pLogListenerDesigner;
	   */

	// OK to kill app
	return true;
}

void CApplication::logLevelCB(void)
{
	// Loads log level dialog
	::GtkBuilder* l_pBuilderInterface=gtk_builder_new(); // glade_xml_new(OVD_GUI_File, "loglevel", NULL);
	gtk_builder_add_from_file(l_pBuilderInterface, OVD_GUI_File, NULL);
	gtk_builder_connect_signals(l_pBuilderInterface, NULL);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_fatal")),             m_rKernelContext.getLogManager().isActive(LogLevel_Fatal));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_error")),             m_rKernelContext.getLogManager().isActive(LogLevel_Error));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_important_warning")), m_rKernelContext.getLogManager().isActive(LogLevel_ImportantWarning));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_warning")),           m_rKernelContext.getLogManager().isActive(LogLevel_Warning));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_info")),              m_rKernelContext.getLogManager().isActive(LogLevel_Info));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_trace")),             m_rKernelContext.getLogManager().isActive(LogLevel_Trace));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_benchmark")),         m_rKernelContext.getLogManager().isActive(LogLevel_Benchmark));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_debug")),             m_rKernelContext.getLogManager().isActive(LogLevel_Debug));

	::GtkDialog* l_pLogLevelDialog=GTK_DIALOG(gtk_builder_get_object(l_pBuilderInterface, "loglevel"));
	gint l_iResult=gtk_dialog_run(l_pLogLevelDialog);
	if(l_iResult==GTK_RESPONSE_APPLY)
	{
		m_rKernelContext.getLogManager().activate(LogLevel_Fatal,            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_fatal")))?true:false);
		m_rKernelContext.getLogManager().activate(LogLevel_Error,            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_error")))?true:false);
		m_rKernelContext.getLogManager().activate(LogLevel_ImportantWarning, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_important_warning")))?true:false);
		m_rKernelContext.getLogManager().activate(LogLevel_Warning,          gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_warning")))?true:false);
		m_rKernelContext.getLogManager().activate(LogLevel_Info,             gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_info")))?true:false);
		m_rKernelContext.getLogManager().activate(LogLevel_Trace,            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_trace")))?true:false);
		m_rKernelContext.getLogManager().activate(LogLevel_Benchmark,        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_benchmark")))?true:false);
		m_rKernelContext.getLogManager().activate(LogLevel_Debug,            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "loglevel-checkbutton_loglevel_debug")))?true:false);

		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_debug")), m_rKernelContext.getLogManager().isActive(LogLevel_Debug));
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_bench")), m_rKernelContext.getLogManager().isActive(LogLevel_Benchmark));
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_trace")), m_rKernelContext.getLogManager().isActive(LogLevel_Trace));
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_info")), m_rKernelContext.getLogManager().isActive(LogLevel_Info));
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_warning")), m_rKernelContext.getLogManager().isActive(LogLevel_Warning));
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_impwarning")), m_rKernelContext.getLogManager().isActive(LogLevel_ImportantWarning));
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_error")), m_rKernelContext.getLogManager().isActive(LogLevel_Error));
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-messages_tb_fatal")), m_rKernelContext.getLogManager().isActive(LogLevel_Fatal));
	}

	gtk_widget_destroy(GTK_WIDGET(l_pLogLevelDialog));
	g_object_unref(l_pBuilderInterface);
}

void CApplication::CPUUsageCB(void)
{
	CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
	if(l_pCurrentInterfacedScenario)
	{
		l_pCurrentInterfacedScenario->m_bDebugCPUUsage=(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")))?true:false);
		l_pCurrentInterfacedScenario->redraw();
	}
}

void CApplication::changeCurrentScenario(int32 i32PageIndex)
{
	if(m_bIsQuitting) return;

	//hide window manager of previously active scenario, if any
	int i = gtk_notebook_get_current_page(m_pScenarioNotebook);
	if(i >= 0 && i < (int)m_vInterfacedScenario.size())
	{
		m_vInterfacedScenario[i]->hideCurrentVisualisation();
	}

	//closing last open scenario
	if(i32PageIndex == -1)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),       false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),       false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),    false);
		gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), GTK_STOCK_MEDIA_PLAY);

		g_signal_handlers_disconnect_by_func(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), G_CALLBACK2(cpu_usage_cb), this);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), false);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), false);
		g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), "toggled", G_CALLBACK(cpu_usage_cb), this);

		//toggle off window manager button
		GtkWidget* l_pWindowManagerButton=GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager"));
		g_signal_handlers_disconnect_by_func(l_pWindowManagerButton, G_CALLBACK2(button_toggle_window_manager_cb), this);
		gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(l_pWindowManagerButton), false);
		g_signal_connect(l_pWindowManagerButton, "toggled", G_CALLBACK(button_toggle_window_manager_cb), this);
	}
	//switching to an existing scenario
	else if(i32PageIndex<(int32)m_vInterfacedScenario.size())
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=m_vInterfacedScenario[i32PageIndex];
		EPlayerStatus l_ePlayerStatus=(l_pCurrentInterfacedScenario->m_pPlayer?l_pCurrentInterfacedScenario->m_pPlayer->getStatus():PlayerStatus_Stop);

		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),       l_ePlayerStatus!=PlayerStatus_Stop);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),       true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),    l_ePlayerStatus!=PlayerStatus_Forward);
		gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), (l_ePlayerStatus==PlayerStatus_Stop || l_ePlayerStatus==PlayerStatus_Pause || l_ePlayerStatus==PlayerStatus_Uninitialized) ? GTK_STOCK_MEDIA_PLAY : GTK_STOCK_MEDIA_PAUSE);

		g_signal_handlers_disconnect_by_func(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), G_CALLBACK2(cpu_usage_cb), this);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), l_ePlayerStatus==PlayerStatus_Stop);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), l_pCurrentInterfacedScenario->m_bDebugCPUUsage);
		g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), "toggled", G_CALLBACK(cpu_usage_cb), this);

		// gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_save")), l_pCurrentInterfacedScenario->m_bHasFileName && l_pCurrentInterfacedScenario->m_bHasBeenModified);
		// gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-menu_save")),   l_pCurrentInterfacedScenario->m_bHasFileName && l_pCurrentInterfacedScenario->m_bHasBeenModified);

		//don't show window manager if in offline mode and it is toggled off
		if(l_ePlayerStatus==PlayerStatus_Stop && m_vInterfacedScenario[i32PageIndex]->isDesignerVisualisationToggled() == false)
		{
			m_vInterfacedScenario[i32PageIndex]->hideCurrentVisualisation();
		}
		else
		{
			m_vInterfacedScenario[i32PageIndex]->showCurrentVisualisation();
		}

		//update window manager button state
		GtkWidget* l_pWindowManagerButton=GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager"));
		g_signal_handlers_disconnect_by_func(l_pWindowManagerButton, G_CALLBACK2(button_toggle_window_manager_cb), this);
		gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(l_pWindowManagerButton), m_vInterfacedScenario[i32PageIndex]->isDesignerVisualisationToggled() ? true : false);
		g_signal_connect(l_pWindowManagerButton, "toggled", G_CALLBACK(button_toggle_window_manager_cb), this);
	}
	//first scenario is created (or a scenario is opened and replaces first unnamed unmodified scenario)
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_stop")),       false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_next")),       true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_forward")),    true);
		gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_play_pause")), GTK_STOCK_MEDIA_PLAY);

		g_signal_handlers_disconnect_by_func(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), G_CALLBACK2(cpu_usage_cb), this);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager")), true);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), false);
		g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "openvibe-togglebutton_cpu_usage")), "toggled", G_CALLBACK(cpu_usage_cb), this);

		//toggle off window manager button
		GtkWidget* l_pWindowManagerButton=GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "openvibe-button_windowmanager"));
		g_signal_handlers_disconnect_by_func(l_pWindowManagerButton, G_CALLBACK2(button_toggle_window_manager_cb), this);
		gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(l_pWindowManagerButton), false);
		g_signal_connect(l_pWindowManagerButton, "toggled", G_CALLBACK(button_toggle_window_manager_cb), this);
	}
}
