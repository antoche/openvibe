#include "ovpCBoxAlgorithmClassifierAccuracyMeasure.h"

#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

namespace
{
	void reset_scores_button_cb(::GtkToolButton* pButton, gpointer pUserData)
	{
		CBoxAlgorithmClassifierAccuracyMeasure* l_pClassifierAccuracyMeasure=reinterpret_cast<CBoxAlgorithmClassifierAccuracyMeasure*>(pUserData);
		vector<CBoxAlgorithmClassifierAccuracyMeasure::SProgressBar>::iterator l_iter = l_pClassifierAccuracyMeasure->m_vProgressBar.begin();
		for(;l_iter!=l_pClassifierAccuracyMeasure->m_vProgressBar.end();l_iter++)
		{
			l_iter->m_ui32Score = 0;
			l_iter->m_ui32StimulationCount = 0;
		}
	}

	void show_percentages_toggle_button_cb(::GtkToggleToolButton* pButton, gpointer pUserData)
	{
		CBoxAlgorithmClassifierAccuracyMeasure* l_pClassifierAccuracyMeasure=reinterpret_cast<CBoxAlgorithmClassifierAccuracyMeasure*>(pUserData);
		l_pClassifierAccuracyMeasure->m_bShowPercentages=(gtk_toggle_tool_button_get_active(pButton)?true:false);
	}

	void show_scores_toggle_button_cb(::GtkToggleToolButton* pButton, gpointer pUserData)
	{
		CBoxAlgorithmClassifierAccuracyMeasure* l_pClassifierAccuracyMeasure=reinterpret_cast<CBoxAlgorithmClassifierAccuracyMeasure*>(pUserData);
		l_pClassifierAccuracyMeasure->m_bShowScores=(gtk_toggle_tool_button_get_active(pButton)?true:false);
	}
};

boolean CBoxAlgorithmClassifierAccuracyMeasure::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_vProgressBar.resize(getStaticBoxContext().getInputCount()-1); //-1 because the first input is the target
	m_vpClassifierStimulationDecoder.resize(getStaticBoxContext().getInputCount()-1);
	m_vInputClassifierMemoryBuffer.resize(getStaticBoxContext().getInputCount()-1);
	m_vOutputClassifierStimulationSet.resize(getStaticBoxContext().getInputCount()-1);

	//classifier decoders
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount()-1; i++)
	{
		m_vpClassifierStimulationDecoder[i]=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
		m_vpClassifierStimulationDecoder[i]->initialize();
	}

	//targets decoder
	m_pTargetStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pTargetStimulationDecoder->initialize();

	//IO for the classifier MemoryBuffer -> StimulationSet
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount()-1; i++)
	{
		m_vInputClassifierMemoryBuffer[i].initialize(m_vpClassifierStimulationDecoder[i]->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
		m_vOutputClassifierStimulationSet[i].initialize(m_vpClassifierStimulationDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
	}

	//IO for the targets MemoryBuffer -> StimulationSet
	ip_pTargetMemoryBuffer.initialize(m_pTargetStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pTargetStimulationSet.initialize(m_pTargetStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	//widgets
	m_pMainWidgetInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", "classifier-accuracy-measure-table", NULL);
	gtk_builder_add_from_file(m_pMainWidgetInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", NULL);

	m_pToolbarWidgetInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", "classifier-accuracy-measure-toolbar", NULL);
	gtk_builder_add_from_file(m_pToolbarWidgetInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", NULL);

	gtk_builder_connect_signals(m_pMainWidgetInterface, NULL);
	gtk_builder_connect_signals(m_pToolbarWidgetInterface, NULL);

	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "reset-score-button")),                  "clicked",      G_CALLBACK(::reset_scores_button_cb),            this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "show-percentages-toggle-button")),      "toggled",      G_CALLBACK(::show_percentages_toggle_button_cb), this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "show-scores-toggle-button")),           "toggled",      G_CALLBACK(::show_scores_toggle_button_cb), this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pToolbarWidgetInterface, "classifier-accuracy-measure-toolbar")), "delete_event", G_CALLBACK(gtk_widget_hide),                     NULL);

	m_pMainWidget=GTK_WIDGET(gtk_builder_get_object(m_pMainWidgetInterface, "classifier-accuracy-measure-table"));
	m_pToolbarWidget=GTK_WIDGET(gtk_builder_get_object(m_pToolbarWidgetInterface, "classifier-accuracy-measure-toolbar"));

	getVisualisationContext().setWidget(m_pMainWidget);
	getVisualisationContext().setToolbar(m_pToolbarWidget);

	m_bShowPercentages=(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pToolbarWidgetInterface, "show-percentages-toggle-button")))?true:false);
	m_bShowScores=(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pToolbarWidgetInterface, "show-scores-toggle-button")))?true:false);

	return true;
}

boolean CBoxAlgorithmClassifierAccuracyMeasure::uninitialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	// IO
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount()-1; i++)
	{
		m_vOutputClassifierStimulationSet[i].uninitialize();
		m_vInputClassifierMemoryBuffer[i].uninitialize();
	}
	op_pTargetStimulationSet.uninitialize();
	ip_pTargetMemoryBuffer.uninitialize();

	//decoders
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount()-1; i++)
	{
		m_vpClassifierStimulationDecoder[i]->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*(m_vpClassifierStimulationDecoder[i]));
	}
	m_pTargetStimulationDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pTargetStimulationDecoder);

	//widgets
	g_object_unref(m_pToolbarWidgetInterface);
	m_pToolbarWidgetInterface=NULL;

	g_object_unref(m_pMainWidgetInterface);
	m_pMainWidgetInterface=NULL;

	return true;
}

boolean CBoxAlgorithmClassifierAccuracyMeasure::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmClassifierAccuracyMeasure::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//input chunk 0 = targets
	// we iterate over the "target" chunks and update the timeline
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pTargetMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pTargetStimulationDecoder->process();

		if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			//header received
			//adding the progress bars to the window
			::GtkTable* l_pTable=GTK_TABLE(gtk_builder_get_object(m_pMainWidgetInterface, "classifier-accuracy-measure-table"));
			gtk_table_resize(l_pTable, 1, l_rStaticBoxContext.getInputCount()-1);

			for(uint32 i=0; i<l_rStaticBoxContext.getInputCount()-1; i++)
			{
				::GtkBuilder* l_pGtkBuilderBar=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", "progress-bar-classifier-accuracy", NULL);
				gtk_builder_add_from_file(l_pGtkBuilderBar, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", NULL);

				::GtkBuilder* l_pGtkBuilderLabel=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", "label-classifier-name", NULL);
				gtk_builder_add_from_file(l_pGtkBuilderLabel, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-ClassifierAccuracyMeasure.ui", NULL);

				::GtkWidget* l_pWidgetBar=GTK_WIDGET(gtk_builder_get_object(l_pGtkBuilderBar, "progress-bar-classifier-accuracy"));
				::GtkWidget* l_pWidgetLabel=GTK_WIDGET(gtk_builder_get_object(l_pGtkBuilderLabel, "label-classifier-name"));

				gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pWidgetBar)), l_pWidgetBar);
				gtk_table_attach(
					l_pTable, l_pWidgetBar,
					i, i+1, 0, 6,
					(::GtkAttachOptions)(GTK_EXPAND|GTK_FILL),
					(::GtkAttachOptions)(GTK_EXPAND|GTK_FILL),
					0, 0);
				gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pWidgetLabel)), l_pWidgetLabel);
				gtk_table_attach(
					l_pTable, l_pWidgetLabel,
					i, i+1, 6, 7,
					(::GtkAttachOptions)(GTK_EXPAND|GTK_FILL),
					(::GtkAttachOptions)(GTK_EXPAND|GTK_FILL),
					0, 0);

				g_object_unref(l_pGtkBuilderBar);
				g_object_unref(l_pGtkBuilderLabel);

				CBoxAlgorithmClassifierAccuracyMeasure::SProgressBar l_oProgressBar;
				l_oProgressBar.m_pProgressBar=GTK_PROGRESS_BAR(l_pWidgetBar);
				l_oProgressBar.m_ui32Score=0;
				l_oProgressBar.m_ui32StimulationCount=0;
				l_oProgressBar.m_pLabelClassifier = GTK_LABEL(l_pWidgetLabel);

				gtk_progress_bar_set_fraction(l_oProgressBar.m_pProgressBar, 0);
				CString l_sInputName;
				l_rStaticBoxContext.getInputName(i+1, l_sInputName);
				gtk_progress_bar_set_text(l_oProgressBar.m_pProgressBar, l_sInputName.toASCIIString());
				gtk_label_set_text(l_oProgressBar.m_pLabelClassifier, l_sInputName.toASCIIString());
				m_vProgressBar[i] = (l_oProgressBar);
			}

			m_ui64CurrentProcessingTimeLimit = 0;
		}

		if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			//buffer received
			//A new target comes, let's update the timeline with it
			for(uint32 s=0; s<op_pTargetStimulationSet->getStimulationCount(); s++)
			{
				uint64 l_ui64StimulationIdentifier = op_pTargetStimulationSet->getStimulationIdentifier(s);
				uint64 l_ui64StimulationDate = op_pTargetStimulationSet->getStimulationDate(s);
				m_mTargetsTimeLine.insert(std::pair<uint64,uint64>(l_ui64StimulationDate,l_ui64StimulationIdentifier));
				getLogManager() << LogLevel_Trace << "New target inserted ("<< l_ui64StimulationIdentifier <<","<< time64(l_ui64StimulationDate) <<")\n";
			}

			//we updtae the time limit for processing classifier stim
			uint64 l_ui64ChunkEndTime = l_rDynamicBoxContext.getInputChunkEndTime(0, i);
			m_ui64CurrentProcessingTimeLimit = MAX(l_ui64ChunkEndTime, m_ui64CurrentProcessingTimeLimit);
		}

		if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	//input index 1-n = n classifier results
	for(uint32 ip=1; ip<l_rStaticBoxContext.getInputCount(); ip++)
	{
		for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(ip); i++)
		{
			m_vInputClassifierMemoryBuffer[ip-1] = l_rDynamicBoxContext.getInputChunk(ip, i);

			// lets get the chunck end time
			uint64 l_ui64ChunkEndTime = l_rDynamicBoxContext.getInputChunkEndTime(ip,i);
			// if the incoming chunk is in the timeline
			if(l_ui64ChunkEndTime <= m_ui64CurrentProcessingTimeLimit)
			{
				if(m_mTargetsTimeLine.size() != 0)
				{
					// we can process it
					m_vpClassifierStimulationDecoder[ip-1]->process();

					if(m_vpClassifierStimulationDecoder[ip-1]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
					{
						//header received
					}

					if(m_vpClassifierStimulationDecoder[ip-1]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
					{
						//buffer received
						for(uint32 s=0; s<m_vOutputClassifierStimulationSet[ip-1]->getStimulationCount(); s++)
						{
							//We need to locate the stimulation on the timeline
							uint64 l_ui64StimulationFromClassifierIdentifier = m_vOutputClassifierStimulationSet[ip-1]->getStimulationIdentifier(s);
							uint64 l_ui64StimulationFromClassifierDate = m_vOutputClassifierStimulationSet[ip-1]->getStimulationDate(s);

							getLogManager() << LogLevel_Trace << "New Classifier state received ("<< l_ui64StimulationFromClassifierIdentifier <<","<< time64(l_ui64StimulationFromClassifierDate) <<") from Classifier "<< ip <<"\n";

							std::map<uint64,uint64>::iterator l_itTarget = m_mTargetsTimeLine.begin();
							std::map<uint64,uint64>::iterator l_itNextTarget;
							boolean l_bContinue = true;
							while(l_itTarget != m_mTargetsTimeLine.end() && l_bContinue)
							{
								l_itNextTarget = l_itTarget;
								l_itNextTarget++;
								if((l_itNextTarget == m_mTargetsTimeLine.end() || l_ui64StimulationFromClassifierDate < l_itNextTarget->first)
								 && l_ui64StimulationFromClassifierDate > l_itTarget->first)
								{
									if(l_ui64StimulationFromClassifierIdentifier == l_itTarget->second)
									{
										//+1 for this classifier !
										m_vProgressBar[ip-1].m_ui32Score++;
									}
									m_vProgressBar[ip-1].m_ui32StimulationCount++;
									l_bContinue = false;
								}
								l_itTarget++;
							}

							//std::map<uint64,uint64>::iterator l_itTarget = m_mTargetsTimeLine.lower_bound(l_ui64StimulationFromClassifierDate);
						}

						std::stringstream ss;
						ss << std::fixed;
						ss << std::setprecision(2);
						if(m_bShowScores)
						{
							ss << "score : " << m_vProgressBar[ip-1].m_ui32Score << "/" << m_vProgressBar[ip-1].m_ui32StimulationCount << "\n";
						}
						float64 l_f64Percent = 0.0;
						if(m_vProgressBar[ip-1].m_ui32StimulationCount != 0)
						{
							l_f64Percent = m_vProgressBar[ip-1].m_ui32Score * 1. / m_vProgressBar[ip-1].m_ui32StimulationCount;
						}
						if(m_bShowPercentages)
						{
							ss << l_f64Percent*100 << "%\n";
						}

						gtk_progress_bar_set_fraction(m_vProgressBar[ip-1].m_pProgressBar, l_f64Percent);
						gtk_progress_bar_set_text(m_vProgressBar[ip-1].m_pProgressBar, ss.str().c_str());
					}

					if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
					{
					}
				}

				l_rDynamicBoxContext.markInputAsDeprecated(ip, i);
			}
		}
	}

	return true;
}
