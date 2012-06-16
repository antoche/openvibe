#include "ovpCAlgorithmLevelMeasure.h"

#include <string>
#include <sstream>
#include <iomanip>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

namespace
{
	void reset_scores_button_cb(::GtkToolButton* pButton, gpointer pUserData)
	{
		CAlgorithmLevelMeasure* l_pLevelMeasure=reinterpret_cast<CAlgorithmLevelMeasure*>(pUserData);
		std::vector < CAlgorithmLevelMeasure::SProgressBar >::iterator it;
		for(it=l_pLevelMeasure->m_vProgressBar.begin(); it!=l_pLevelMeasure->m_vProgressBar.end(); it++)
		{
			it->m_ui32Score=0;
		}
	}

	void threshold_spinbutton_cb(::GtkSpinButton* pButton, gpointer pUserData)
	{
		CAlgorithmLevelMeasure* l_pLevelMeasure=reinterpret_cast<CAlgorithmLevelMeasure*>(pUserData);
		l_pLevelMeasure->m_f64Threshold=.01*gtk_spin_button_get_value(pButton);
	}

	void show_percentages_toggle_button_cb(::GtkToggleToolButton* pButton, gpointer pUserData)
	{
		CAlgorithmLevelMeasure* l_pLevelMeasure=reinterpret_cast<CAlgorithmLevelMeasure*>(pUserData);
		l_pLevelMeasure->m_bShowPercentages=(gtk_toggle_tool_button_get_active(pButton)?true:false);
	}
};

boolean CAlgorithmLevelMeasure::initialize(void)
{
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_LevelMeasure_InputParameterId_Matrix));

	op_pMainWidget.initialize(getOutputParameter(OVP_Algorithm_LevelMeasure_OutputParameterId_MainWidget));
	op_pToolbarWidget.initialize(getOutputParameter(OVP_Algorithm_LevelMeasure_OutputParameterId_ToolbarWidget));

	m_pMainWidgetInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-LevelMeasure.ui", "level-measure-table", NULL);
	gtk_builder_add_from_file(m_pMainWidgetInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-LevelMeasure.ui", NULL);

	m_pToolbarWidgetInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-LevelMeasure.ui", "level-measure-toolbar", NULL);
	gtk_builder_add_from_file(m_pToolbarWidgetInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-LevelMeasure.ui", NULL);

	gtk_builder_connect_signals(m_pMainWidgetInterface, NULL);
	gtk_builder_connect_signals(m_pToolbarWidgetInterface, NULL);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "reset-score-button")),             "clicked",       G_CALLBACK(::reset_scores_button_cb),            this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "show-percentages-toggle-button")), "toggled",       G_CALLBACK(::show_percentages_toggle_button_cb), this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "threshold-spinbutton")),           "value-changed", G_CALLBACK(::threshold_spinbutton_cb),           this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "level-measure-toolbar")),          "delete_event",  G_CALLBACK(gtk_widget_hide),                     NULL);

	m_pMainWindow=GTK_WIDGET(gtk_builder_get_object(m_pMainWidgetInterface, "level-measure-table"));
	m_pToolbarWidget=GTK_WIDGET(gtk_builder_get_object(m_pToolbarWidgetInterface, "level-measure-toolbar"));

	m_bShowPercentages=(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pToolbarWidgetInterface, "show-percentages-toggle-button")))?true:false);
	m_f64Threshold=.01*gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(m_pToolbarWidgetInterface, "threshold-spinbutton")));

	return true;
}

boolean CAlgorithmLevelMeasure::uninitialize(void)
{
	g_object_unref(m_pToolbarWidgetInterface);
	m_pToolbarWidgetInterface=NULL;

	g_object_unref(m_pMainWidgetInterface);
	m_pMainWidgetInterface=NULL;

	op_pToolbarWidget.uninitialize();
	op_pMainWidget.uninitialize();

	ip_pMatrix.uninitialize();

	return true;
}

boolean CAlgorithmLevelMeasure::process(void)
{
	if(this->isInputTriggerActive(OVP_Algorithm_LevelMeasure_InputTriggerId_Reset))
	{
		if(ip_pMatrix->getDimensionCount()!=1 && ip_pMatrix->getDimensionCount()!=2)
		{
			getLogManager() << LogLevel_ImportantWarning << "Input matrix does not have 1 or 2 dimensions (" << ip_pMatrix->getDimensionCount() << ")\n";
			return false;
		}

		uint32 l_ui32RowCount=(ip_pMatrix->getDimensionCount()==2?ip_pMatrix->getDimensionSize(0):1);
		uint32 l_ui32ColumnCount=(ip_pMatrix->getDimensionCount()==2?ip_pMatrix->getDimensionSize(1):ip_pMatrix->getDimensionSize(0));

		::GtkTable* l_pTable=GTK_TABLE(gtk_builder_get_object(m_pMainWidgetInterface, "level-measure-table"));
		gtk_table_resize(l_pTable, l_ui32RowCount, l_ui32ColumnCount);

		for(uint32 i=0; i<l_ui32RowCount; i++)
		{
			for(uint32 j=0; j<l_ui32ColumnCount; j++)
			{
				::GtkBuilder* l_pGtkBuilder=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-LevelMeasure.ui", "progress-bar-level", NULL);
				gtk_builder_add_from_file(l_pGtkBuilder, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-LevelMeasure.ui", NULL);

				::GtkWidget* l_pWidget=GTK_WIDGET(gtk_builder_get_object(l_pGtkBuilder, "progress-bar-level"));
				gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pWidget)), l_pWidget);
				gtk_table_attach(
					l_pTable, l_pWidget,
					j, j+1, i, i+1,
					(::GtkAttachOptions)(GTK_EXPAND|GTK_FILL),
					(::GtkAttachOptions)(GTK_EXPAND|GTK_FILL),
					0, 0);
				g_object_unref(l_pGtkBuilder);

				CAlgorithmLevelMeasure::SProgressBar l_oProgressBar;
				l_oProgressBar.m_pProgressBar=GTK_PROGRESS_BAR(l_pWidget);
				l_oProgressBar.m_ui32Score=0;
				l_oProgressBar.m_bLastWasOverThreshold=false;
				m_vProgressBar.push_back(l_oProgressBar);
			}
		}

		op_pMainWidget=m_pMainWindow;
		op_pToolbarWidget=m_pToolbarWidget;
	}

	if(this->isInputTriggerActive(OVP_Algorithm_LevelMeasure_InputTriggerId_Refresh))
	{
		std::vector < CAlgorithmLevelMeasure::SProgressBar >::iterator it=m_vProgressBar.begin();

		float64* l_pInBuffer=ip_pMatrix->getBuffer();
		float64 l_f64Percent;

		uint32 l_ui32ElementCount=ip_pMatrix->getBufferElementCount();
		while(l_ui32ElementCount--)
		{
			l_f64Percent=*l_pInBuffer;
			if(l_f64Percent>1) l_f64Percent=1;
			if(l_f64Percent<0) l_f64Percent=0;

			if(l_f64Percent>m_f64Threshold && !it->m_bLastWasOverThreshold)
			{
				it->m_ui32Score++;
				it->m_bLastWasOverThreshold=true;
			}
			if(l_f64Percent<=m_f64Threshold)
			{
				it->m_bLastWasOverThreshold=false;
			}

			std::stringstream ss;
			ss << std::fixed;
			ss << std::setprecision(2);
			ss << "score : " << it->m_ui32Score << "\n";

			if(m_bShowPercentages)
			{
				ss << "level : " << l_f64Percent*100 << "%\n";
			}

			gtk_progress_bar_set_fraction(it->m_pProgressBar, l_f64Percent);
			gtk_progress_bar_set_text(it->m_pProgressBar, ss.str().c_str());

			l_pInBuffer++;
			it++;
		}

		this->activateOutputTrigger(OVP_Algorithm_LevelMeasure_OutputTriggerId_Refreshed, true);
	}

	return true;
}
