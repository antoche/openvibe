#include "ovpCBoxAlgorithmP300SpellerVisualisation.h"

#include <system/Memory.h>

#include <list>
#include <vector>
#include <string>
#include <algorithm>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

namespace
{
	class _AutoCast_
	{
	public:
		_AutoCast_(IBox& rBox, IConfigurationManager& rConfigurationManager, const uint32 ui32Index) : m_rConfigurationManager(rConfigurationManager) { rBox.getSettingValue(ui32Index, m_sSettingValue); }
		operator ::GdkColor (void)
		{
			::GdkColor l_oColor;
			int r=0, g=0, b=0;
			sscanf(m_sSettingValue.toASCIIString(), "%i,%i,%i", &r, &g, &b);
			l_oColor.pixel=0;
			l_oColor.red=(r*65535)/100;
			l_oColor.green=(g*65535)/100;
			l_oColor.blue=(b*65535)/100;
			return l_oColor;
		}
	protected:
		IConfigurationManager& m_rConfigurationManager;
		CString m_sSettingValue;
	};

	static void toggle_button_show_hide_cb(::GtkToggleToolButton* pToggleButton, gpointer pUserData)
	{
		if(gtk_toggle_tool_button_get_active(pToggleButton))
		{
			gtk_widget_show(GTK_WIDGET(pUserData));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(pUserData));
		}
	}
};

boolean CBoxAlgorithmP300SpellerVisualisation::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pMainWidgetInterface=NULL;
	m_pToolbarWidgetInterface=NULL;
	m_pFlashFontDescription=NULL;
	m_pNoFlashFontDescription=NULL;
	m_pTargetFontDescription=NULL;
	m_pSelectedFontDescription=NULL;

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------

	m_sInterfaceFilename         =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64RowStimulationBase     =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_ui64ColumnStimulationBase  =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	m_oFlashBackgroundColor      =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 3);
	m_oFlashForegroundColor      =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 4);
	m_ui64FlashFontSize          =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);
	m_oNoFlashBackgroundColor    =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 6);
	m_oNoFlashForegroundColor    =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 7);
	m_ui64NoFlashFontSize        =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 8);
	m_oTargetBackgroundColor     =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 9);
	m_oTargetForegroundColor     =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 10);
	m_ui64TargetFontSize         =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 11);
	m_oSelectedBackgroundColor   =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 12);
	m_oSelectedForegroundColor   =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 13);
	m_ui64SelectedFontSize       =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 14);

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------

	m_pSequenceStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pSequenceStimulationDecoder->initialize();

	m_pTargetStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pTargetStimulationDecoder->initialize();

	m_pTargetFlaggingStimulationEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pTargetFlaggingStimulationEncoder->initialize();

	m_pRowSelectionStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pRowSelectionStimulationDecoder->initialize();

	m_pColumnSelectionStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pColumnSelectionStimulationDecoder->initialize();

	ip_pSequenceMemoryBuffer.initialize(m_pSequenceStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pSequenceStimulationSet.initialize(m_pSequenceStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	ip_pTargetMemoryBuffer.initialize(m_pTargetStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pTargetStimulationSet.initialize(m_pTargetStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	ip_pTargetFlaggingStimulationSet.initialize(m_pTargetFlaggingStimulationEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	op_pTargetFlaggingMemoryBuffer.initialize(m_pTargetFlaggingStimulationEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_ui64LastTime=0;

	m_pMainWidgetInterface=gtk_builder_new(); // glade_xml_new(m_sInterfaceFilename.toASCIIString(), "p300-speller-main", NULL);
	if(!gtk_builder_add_from_file(m_pMainWidgetInterface, m_sInterfaceFilename.toASCIIString(), NULL))
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Could not load interface file [" << m_sInterfaceFilename << "]\n";
		this->getLogManager() << LogLevel_ImportantWarning << "The file may be missing. However, the interface files now use gtk-builder instead of glade. Did you update your files ?\n";
		return false;
	}

	m_pToolbarWidgetInterface=gtk_builder_new(); // glade_xml_new(m_sInterfaceFilename.toASCIIString(), "p300-speller-toolbar", NULL);
	gtk_builder_add_from_file(m_pToolbarWidgetInterface, m_sInterfaceFilename.toASCIIString(), NULL);

	m_pMainWindow=GTK_WIDGET(gtk_builder_get_object(m_pMainWidgetInterface, "p300-speller-main"));
	m_pToolbarWidget=GTK_WIDGET(gtk_builder_get_object(m_pToolbarWidgetInterface, "p300-speller-toolbar"));
	m_pTable=GTK_TABLE(gtk_builder_get_object(m_pMainWidgetInterface, "p300-speller-table"));
	m_pResult=GTK_LABEL(gtk_builder_get_object(m_pMainWidgetInterface, "label-result"));
	m_pTarget=GTK_LABEL(gtk_builder_get_object(m_pMainWidgetInterface, "label-target"));

	gtk_builder_connect_signals(m_pMainWidgetInterface, NULL);
	gtk_builder_connect_signals(m_pToolbarWidgetInterface, NULL);

	g_signal_connect(gtk_builder_get_object(m_pToolbarWidgetInterface, "toolbutton-show_target_text"),             "toggled", G_CALLBACK(toggle_button_show_hide_cb), gtk_builder_get_object(m_pMainWidgetInterface, "label-target"));
	g_signal_connect(gtk_builder_get_object(m_pToolbarWidgetInterface, "toolbutton-show_target_text"),             "toggled", G_CALLBACK(toggle_button_show_hide_cb), gtk_builder_get_object(m_pMainWidgetInterface, "label-target-title"));
	g_signal_connect(gtk_builder_get_object(m_pToolbarWidgetInterface, "toolbutton-show_result_text"),             "toggled", G_CALLBACK(toggle_button_show_hide_cb), gtk_builder_get_object(m_pMainWidgetInterface, "label-result"));
	g_signal_connect(gtk_builder_get_object(m_pToolbarWidgetInterface, "toolbutton-show_result_text"),             "toggled", G_CALLBACK(toggle_button_show_hide_cb), gtk_builder_get_object(m_pMainWidgetInterface, "label-result-title"));

	getVisualisationContext().setWidget(m_pMainWindow);
	getVisualisationContext().setToolbar(m_pToolbarWidget);

	guint l_uiRowCount=0;
	guint l_uiColumnCount=0;
	g_object_get(m_pTable, "n-rows", &l_uiRowCount, NULL);
	g_object_get(m_pTable, "n-columns", &l_uiColumnCount, NULL);

	m_ui64RowCount=l_uiRowCount;
	m_ui64ColumnCount=l_uiColumnCount;

	::PangoFontDescription* l_pMaxFontDescription=pango_font_description_copy(pango_context_get_font_description(gtk_widget_get_pango_context(m_pMainWindow)));
	m_pFlashFontDescription=pango_font_description_copy(pango_context_get_font_description(gtk_widget_get_pango_context(m_pMainWindow)));
	m_pNoFlashFontDescription=pango_font_description_copy(pango_context_get_font_description(gtk_widget_get_pango_context(m_pMainWindow)));
	m_pTargetFontDescription=pango_font_description_copy(pango_context_get_font_description(gtk_widget_get_pango_context(m_pMainWindow)));
	m_pSelectedFontDescription=pango_font_description_copy(pango_context_get_font_description(gtk_widget_get_pango_context(m_pMainWindow)));

	uint64 l_ui64MaxSize=0;
	l_ui64MaxSize=std::max(l_ui64MaxSize, m_ui64FlashFontSize);
	l_ui64MaxSize=std::max(l_ui64MaxSize, m_ui64NoFlashFontSize);
	l_ui64MaxSize=std::max(l_ui64MaxSize, m_ui64TargetFontSize);
	l_ui64MaxSize=std::max(l_ui64MaxSize, m_ui64SelectedFontSize);

	pango_font_description_set_size(l_pMaxFontDescription, l_ui64MaxSize * PANGO_SCALE);
	pango_font_description_set_size(m_pFlashFontDescription, m_ui64FlashFontSize * PANGO_SCALE);
	pango_font_description_set_size(m_pNoFlashFontDescription, m_ui64NoFlashFontSize * PANGO_SCALE);
	pango_font_description_set_size(m_pTargetFontDescription, m_ui64TargetFontSize * PANGO_SCALE);
	pango_font_description_set_size(m_pSelectedFontDescription, m_ui64SelectedFontSize * PANGO_SCALE);

	this->_cache_build_from_table_(m_pTable);
	this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_, &m_oNoFlashBackgroundColor);
	this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_, &m_oNoFlashForegroundColor);
	this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_, l_pMaxFontDescription);

	pango_font_description_free(l_pMaxFontDescription);

	m_iLastTargetRow=-1;
	m_iLastTargetColumn=-1;
	m_iTargetRow=-1;
	m_iTargetColumn=-1;
	m_iSelectedRow=-1;
	m_iSelectedColumn=-1;

	m_bTableInitialized=false;

	return true;
}

boolean CBoxAlgorithmP300SpellerVisualisation::uninitialize(void)
{
	if(m_pSelectedFontDescription)
	{
		pango_font_description_free(m_pSelectedFontDescription);
		m_pSelectedFontDescription=NULL;
	}

	if(m_pTargetFontDescription)
	{
		pango_font_description_free(m_pTargetFontDescription);
		m_pTargetFontDescription=NULL;
	}

	if(m_pNoFlashFontDescription)
	{
		pango_font_description_free(m_pNoFlashFontDescription);
		m_pNoFlashFontDescription=NULL;
	}

	if(m_pFlashFontDescription)
	{
		pango_font_description_free(m_pFlashFontDescription);
		m_pFlashFontDescription=NULL;
	}

	if(m_pToolbarWidgetInterface)
	{
		g_object_unref(m_pToolbarWidgetInterface);
		m_pToolbarWidgetInterface=NULL;
	}

	if(m_pMainWidgetInterface)
	{
		g_object_unref(m_pMainWidgetInterface);
		m_pMainWidgetInterface=NULL;
	}

	ip_pTargetFlaggingStimulationSet.uninitialize();
	op_pTargetFlaggingMemoryBuffer.uninitialize();

	op_pTargetStimulationSet.uninitialize();
	ip_pTargetMemoryBuffer.uninitialize();

	op_pSequenceStimulationSet.uninitialize();
	ip_pSequenceMemoryBuffer.uninitialize();

	if(m_pColumnSelectionStimulationDecoder)
	{
		m_pColumnSelectionStimulationDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pColumnSelectionStimulationDecoder);
		m_pColumnSelectionStimulationDecoder=NULL;
	}

	if(m_pRowSelectionStimulationDecoder)
	{
		m_pRowSelectionStimulationDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pRowSelectionStimulationDecoder);
		m_pRowSelectionStimulationDecoder=NULL;
	}

	if(m_pTargetFlaggingStimulationEncoder)
	{
		m_pTargetFlaggingStimulationEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pTargetFlaggingStimulationEncoder);
		m_pTargetFlaggingStimulationEncoder=NULL;
	}

	if(m_pTargetStimulationDecoder)
	{
		m_pTargetStimulationDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pTargetStimulationDecoder);
		m_pTargetStimulationDecoder=NULL;
	}

	if(m_pSequenceStimulationDecoder)
	{
		m_pSequenceStimulationDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pSequenceStimulationDecoder);
		m_pSequenceStimulationDecoder=NULL;
	}

	return true;
}

boolean CBoxAlgorithmP300SpellerVisualisation::processInput(uint32 ui32Index)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	if(!m_bTableInitialized)
	{
		this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_, &m_oNoFlashBackgroundColor);
		this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_, &m_oNoFlashForegroundColor);
		this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_, m_pNoFlashFontDescription);
		m_bTableInitialized=true;
	}

	return true;
}

boolean CBoxAlgorithmP300SpellerVisualisation::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint32 i, j, k;

	// --- Sequence stimulations

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		CStimulationSet l_oFlaggingStimulationSet;

		ip_pSequenceMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		ip_pTargetFlaggingStimulationSet=&l_oFlaggingStimulationSet;
		op_pTargetFlaggingMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

		m_pSequenceStimulationDecoder->process();

		m_ui64LastTime=l_rDynamicBoxContext.getInputChunkEndTime(0, i);

		if(m_pSequenceStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pTargetFlaggingStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
		}

		if(m_pSequenceStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			IStimulationSet* l_pStimulationSet=op_pSequenceStimulationSet;
			for(j=0; j<l_pStimulationSet->getStimulationCount(); j++)
			{
				uint64 l_ui64StimulationIdentifier=l_pStimulationSet->getStimulationIdentifier(j);
				boolean l_bFlash=false;
				int l_iRow=-1;
				int l_iColumn=-1;
				if(l_ui64StimulationIdentifier >= m_ui64RowStimulationBase && l_ui64StimulationIdentifier < m_ui64RowStimulationBase+m_ui64RowCount)
				{
					l_iRow=l_ui64StimulationIdentifier-m_ui64RowStimulationBase;
					l_bFlash=true;
					if(l_iRow==m_iLastTargetRow)
					{
						l_oFlaggingStimulationSet.appendStimulation(OVTK_StimulationId_Target, l_pStimulationSet->getStimulationDate(j), 0);
					}
					else
					{
						l_oFlaggingStimulationSet.appendStimulation(OVTK_StimulationId_NonTarget, l_pStimulationSet->getStimulationDate(j), 0);
					}
				}
				if(l_ui64StimulationIdentifier >= m_ui64ColumnStimulationBase && l_ui64StimulationIdentifier < m_ui64ColumnStimulationBase+m_ui64ColumnCount)
				{
					l_iColumn=l_ui64StimulationIdentifier-m_ui64ColumnStimulationBase;
					l_bFlash=true;
					if(l_iColumn==m_iLastTargetColumn)
					{
						l_oFlaggingStimulationSet.appendStimulation(OVTK_StimulationId_Target, l_pStimulationSet->getStimulationDate(j), 0);
					}
					else
					{
						l_oFlaggingStimulationSet.appendStimulation(OVTK_StimulationId_NonTarget, l_pStimulationSet->getStimulationDate(j), 0);
					}
				}
				if(l_ui64StimulationIdentifier == OVTK_StimulationId_VisualStimulationStop)
				{
					this->getLogManager() << LogLevel_Debug << "Received OVTK_StimulationId_VisualStimulationStop - resets grid\n";
					this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_, &m_oNoFlashBackgroundColor);
					this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_, &m_oNoFlashForegroundColor);
					this->_cache_for_each_(&CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_, m_pNoFlashFontDescription);
				}
				if(l_ui64StimulationIdentifier == OVTK_StimulationId_Reset)
				{
					gtk_label_set_text(m_pTarget, "");
					gtk_label_set_text(m_pResult, "");
				}

				if(l_bFlash)
				{
					this->_cache_for_each_if_(
						l_iRow,
						l_iColumn,
						&CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_,
						&CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_,
						&m_oFlashBackgroundColor,
						&m_oNoFlashBackgroundColor);
					this->_cache_for_each_if_(
						l_iRow,
						l_iColumn,
						&CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_,
						&CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_,
						&m_oFlashForegroundColor,
						&m_oNoFlashForegroundColor);
					this->_cache_for_each_if_(
						l_iRow,
						l_iColumn,
						&CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_,
						&CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_,
						m_pFlashFontDescription,
						m_pNoFlashFontDescription);
				}
			}
			m_pTargetFlaggingStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		if(m_pSequenceStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pTargetFlaggingStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd);
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	// --- Target stimulations

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(1); i++)
	{
		if(m_ui64LastTime>=l_rDynamicBoxContext.getInputChunkStartTime(1, i))
		{
			ip_pTargetMemoryBuffer=l_rDynamicBoxContext.getInputChunk(1, i);
			m_pTargetStimulationDecoder->process();

			if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
			}

			if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				IStimulationSet* l_pStimulationSet=op_pTargetStimulationSet;
				for(j=0; j<l_pStimulationSet->getStimulationCount(); j++)
				{
					uint64 l_ui64StimulationIdentifier=l_pStimulationSet->getStimulationIdentifier(j);
					boolean l_bTarget=false;
					if(l_ui64StimulationIdentifier >= m_ui64RowStimulationBase && l_ui64StimulationIdentifier < m_ui64RowStimulationBase+m_ui64RowCount)
					{
						this->getLogManager() << LogLevel_Debug << "Received Target Row " << l_ui64StimulationIdentifier << "\n";
						m_iTargetRow=l_ui64StimulationIdentifier-m_ui64RowStimulationBase;
						l_bTarget=true;
					}
					if(l_ui64StimulationIdentifier >= m_ui64ColumnStimulationBase && l_ui64StimulationIdentifier < m_ui64ColumnStimulationBase+m_ui64ColumnCount)
					{
						this->getLogManager() << LogLevel_Debug << "Received Target Column " << l_ui64StimulationIdentifier << "\n";
						m_iTargetColumn=l_ui64StimulationIdentifier-m_ui64ColumnStimulationBase;
						l_bTarget=true;
					}

					if(l_bTarget && m_iTargetRow!=-1 && m_iTargetColumn!=-1)
					{
						this->getLogManager() << LogLevel_Debug << "Displays Target Cell\n";
						this->_cache_for_each_if_(
							m_iTargetRow,
							m_iTargetColumn,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_change_null_cb_,
							&m_oTargetBackgroundColor,
							NULL);
						this->_cache_for_each_if_(
							m_iTargetRow,
							m_iTargetColumn,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_change_null_cb_,
							&m_oTargetForegroundColor,
							NULL);
						this->_cache_for_each_if_(
							m_iTargetRow,
							m_iTargetColumn,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_change_null_cb_,
							m_pTargetFontDescription,
							NULL);

						std::vector < ::GtkWidget* > l_vWidgets;
						this->_cache_for_each_if_(
							m_iTargetRow,
							m_iTargetColumn,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_collect_child_widget_cb_,
							&CBoxAlgorithmP300SpellerVisualisation::_cache_collect_child_widget_cb_,
							&l_vWidgets,
							NULL);

						if(l_vWidgets.size() == 1)
						{
							if(GTK_IS_LABEL(l_vWidgets[0]))
							{
								std::string l_sString;
								l_sString=gtk_label_get_text(m_pTarget);
								l_sString+=gtk_label_get_text(GTK_LABEL(l_vWidgets[0]));
								gtk_label_set_text(m_pTarget, l_sString.c_str());
							}
							else
							{
								this->getLogManager() << LogLevel_Warning << "Expected label class widget... could not find a valid text to append\n";
							}
						}
						else
						{
							this->getLogManager() << LogLevel_Warning << "Did not find a unique widget at row:" << (uint32)m_iTargetRow << " column:" << (uint32) m_iTargetColumn << "\n";
						}

						m_vTargetHistory.push_back(std::make_pair(m_iTargetRow, m_iTargetColumn));
						m_iLastTargetRow=m_iTargetRow;
						m_iLastTargetColumn=m_iTargetColumn;
						m_iTargetRow=-1;
						m_iTargetColumn=-1;
					}
				}
			}

			if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}

			l_rDynamicBoxContext.markInputAsDeprecated(1, i);
		}
	}

	// --- Selection stimulations

	for(k=2; k<4; k++)
	{
		IAlgorithmProxy* l_pSelectionStimulationDecoder=(k==2?m_pRowSelectionStimulationDecoder:m_pColumnSelectionStimulationDecoder);
		TParameterHandler < const IMemoryBuffer* > ip_pSelectionMemoryBuffer(l_pSelectionStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler < IStimulationSet* > op_pSelectionStimulationSet(l_pSelectionStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

		for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(k); i++)
		{
			if(m_ui64LastTime>=l_rDynamicBoxContext.getInputChunkStartTime(k, i))
			{
				ip_pSelectionMemoryBuffer=l_rDynamicBoxContext.getInputChunk(k, i);
				l_pSelectionStimulationDecoder->process();

				if(l_pSelectionStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
				{
				}

				if(l_pSelectionStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
				{
					IStimulationSet* l_pStimulationSet=op_pSelectionStimulationSet;
					for(j=0; j<l_pStimulationSet->getStimulationCount(); j++)
					{
						uint64 l_ui64StimulationIdentifier=l_pStimulationSet->getStimulationIdentifier(j);
						boolean l_bSelected=false;
						if(l_ui64StimulationIdentifier >= m_ui64RowStimulationBase && l_ui64StimulationIdentifier < m_ui64RowStimulationBase+m_ui64RowCount)
						{
							this->getLogManager() << LogLevel_Debug << "Received Selected Row " << l_ui64StimulationIdentifier << "\n";
							m_iSelectedRow=l_ui64StimulationIdentifier-m_ui64RowStimulationBase;
							l_bSelected=true;
						}
						if(l_ui64StimulationIdentifier >= m_ui64ColumnStimulationBase && l_ui64StimulationIdentifier < m_ui64ColumnStimulationBase+m_ui64RowCount)
						{
							this->getLogManager() << LogLevel_Debug << "Received Selected Column " << l_ui64StimulationIdentifier << "\n";
							m_iSelectedColumn=l_ui64StimulationIdentifier-m_ui64ColumnStimulationBase;
							l_bSelected=true;
						}
						if(l_ui64StimulationIdentifier == OVTK_StimulationId_Label_00)
						{
							if(k==2) m_iSelectedRow=-2;
							if(k==3) m_iSelectedColumn=-2;
							l_bSelected=true;
						}
						if(l_bSelected && m_iSelectedRow!=-1 && m_iSelectedColumn!=-1)
						{
							if(m_iSelectedRow>=0 && m_iSelectedColumn>=0)
							{
								this->getLogManager() << LogLevel_Debug << "Displays Selected Cell\n";
								this->_cache_for_each_if_(
									m_iSelectedRow,
									m_iSelectedColumn,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_change_null_cb_,
									&m_oSelectedBackgroundColor,
									NULL);
								this->_cache_for_each_if_(
									m_iSelectedRow,
									m_iSelectedColumn,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_change_null_cb_,
									&m_oSelectedForegroundColor,
									NULL);
								this->_cache_for_each_if_(
									m_iSelectedRow,
									m_iSelectedColumn,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_change_null_cb_,
									m_pSelectedFontDescription,
									NULL);

								std::vector < ::GtkWidget* > l_vWidgets;
								this->_cache_for_each_if_(
									m_iSelectedRow,
									m_iSelectedColumn,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_collect_child_widget_cb_,
									&CBoxAlgorithmP300SpellerVisualisation::_cache_collect_child_widget_cb_,
									&l_vWidgets,
									NULL);

								if(l_vWidgets.size() == 1)
								{
									if(GTK_IS_LABEL(l_vWidgets[0]))
									{
										std::string l_sString;
										l_sString=gtk_label_get_text(GTK_LABEL(l_vWidgets[0]));
										if(m_vTargetHistory.size())
										{
											std::list < std::pair < int, int > >::const_iterator it=m_vTargetHistory.begin();
											boolean l_bCorrect=(it->first==m_iSelectedRow && it->second==m_iSelectedColumn);
											boolean l_bHalfCorrect=(it->first==m_iSelectedRow || it->second==m_iSelectedColumn);
											m_vTargetHistory.pop_front();
											if(l_bCorrect)
											{
												l_sString="<span color=\"darkgreen\">" + l_sString + "</span>";
											}
											else if (l_bHalfCorrect)
											{
												l_sString="<span color=\"darkorange\">" + l_sString + "</span>";
											}
										}
										l_sString=std::string(gtk_label_get_label(m_pResult))+l_sString;
										gtk_label_set_markup(m_pResult, l_sString.c_str());
									}
									else
									{
										this->getLogManager() << LogLevel_Warning << "Expected label class widget... could not find a valid text to append\n";
									}
								}
								else
								{
									this->getLogManager() << LogLevel_Warning << "Did not find a unique widget at row : " << (uint32)m_iSelectedRow << " column : " << (uint32) m_iSelectedColumn << "\n";
								}
							}
							else
							{
								this->getLogManager() << LogLevel_Trace << "Selection Rejected !\n";
								std::string l_sString;
								l_sString=gtk_label_get_text(m_pResult);
								l_sString+="*";
								gtk_label_set_text(m_pResult, l_sString.c_str());
							}

							m_iSelectedRow=-1;
							m_iSelectedColumn=-1;
						}
					}
				}

				if(l_pSelectionStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
				{
				}

				l_rDynamicBoxContext.markInputAsDeprecated(k, i);
			}
		}
	}

	return true;
}

// _________________________________________________________________________________________________________________________________________________________
//

void CBoxAlgorithmP300SpellerVisualisation::_cache_build_from_table_(::GtkTable* pTable)
{
	if(pTable)
	{
		::GdkColor l_oWhite;
		l_oWhite.red=65535;
		l_oWhite.green=65535;
		l_oWhite.blue=65535;
		l_oWhite.pixel=65535;

		::GtkTableChild* l_pTableChild=NULL;
		::GList* l_pList=NULL;
		for(l_pList=pTable->children; l_pList; l_pList=l_pList->next)
		{
			l_pTableChild=(::GtkTableChild*)l_pList->data;

			for(unsigned long i=l_pTableChild->top_attach; i<l_pTableChild->bottom_attach; i++)
			{
				for(unsigned long j=l_pTableChild->left_attach; j<l_pTableChild->right_attach; j++)
				{
					CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& l_rWidgetStyle=m_vCache[i][j];
					l_rWidgetStyle.pWidget=l_pTableChild->widget;
					l_rWidgetStyle.pChildWidget=gtk_bin_get_child(GTK_BIN(l_pTableChild->widget));
					l_rWidgetStyle.oBackgroundColor=l_oWhite;
					l_rWidgetStyle.oForegroundColor=l_oWhite;
					l_rWidgetStyle.pFontDescription=NULL;
				}
			}
		}
	}
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_for_each_(_cache_callback_ fpCallback, void* pUserData)
{
	std::map < unsigned long, std::map < unsigned long, CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle > >::iterator i;
	std::map < unsigned long, CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle >::iterator j;

	for(i=m_vCache.begin(); i!=m_vCache.end(); i++)
	{
		for(j=i->second.begin(); j!=i->second.end(); j++)
		{
			(this->*fpCallback)(j->second, pUserData);
		}
	}
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_for_each_if_(int iLine, int iColumn, _cache_callback_ fpIfCallback, _cache_callback_ fpElseCallback, void* pIfUserData, void* pElseUserData)
{
	std::map < unsigned long, std::map < unsigned long, CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle > >::iterator i;
	std::map < unsigned long, CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle >::iterator j;

	for(i=m_vCache.begin(); i!=m_vCache.end(); i++)
	{
		for(j=i->second.begin(); j!=i->second.end(); j++)
		{
			boolean l_bLine=(iLine!=-1);
			boolean l_bColumn=(iColumn!=-1);
			boolean l_bInLine=false;
			boolean l_bInColumn=false;
			boolean l_bIf;

			if(l_bLine && (unsigned long)iLine==i->first)
			{
				l_bInLine=true;
			}
			if(l_bColumn && (unsigned long)iColumn==j->first)
			{
				l_bInColumn=true;
			}

			if(l_bLine && l_bColumn)
			{
				l_bIf=l_bInLine && l_bInColumn;
			}
			else
			{
				l_bIf=l_bInLine || l_bInColumn;
			}

			if(l_bIf)
			{
				(this->*fpIfCallback)(j->second, pIfUserData);
			}
			else
			{
				(this->*fpElseCallback)(j->second, pElseUserData);
			}
		}
	}
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_change_null_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_change_background_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
	::GdkColor oColor=*(::GdkColor*)pUserData;
	if(!System::Memory::compare(&rWidgetStyle.oBackgroundColor, &oColor, sizeof(::GdkColor)))
	{
		gtk_widget_modify_bg(rWidgetStyle.pWidget, GTK_STATE_NORMAL, &oColor);
		rWidgetStyle.oBackgroundColor=oColor;
	}
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_change_foreground_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
	::GdkColor oColor=*(::GdkColor*)pUserData;
	if(!System::Memory::compare(&rWidgetStyle.oForegroundColor, &oColor, sizeof(::GdkColor)))
	{
		gtk_widget_modify_fg(rWidgetStyle.pChildWidget, GTK_STATE_NORMAL, &oColor);
		rWidgetStyle.oForegroundColor=oColor;
	}
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_change_font_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
	::PangoFontDescription* pFontDescription=(::PangoFontDescription*)pUserData;
	if(rWidgetStyle.pFontDescription!=pFontDescription)
	{
		gtk_widget_modify_font(rWidgetStyle.pChildWidget, pFontDescription);
		rWidgetStyle.pFontDescription=pFontDescription;
	}
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_collect_widget_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
	if(pUserData)
	{
		((std::vector < ::GtkWidget* >*)pUserData)->push_back(rWidgetStyle.pWidget);
	}
}

void CBoxAlgorithmP300SpellerVisualisation::_cache_collect_child_widget_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
	if(pUserData)
	{
		((std::vector < ::GtkWidget* >*)pUserData)->push_back(rWidgetStyle.pChildWidget);
	}
}
