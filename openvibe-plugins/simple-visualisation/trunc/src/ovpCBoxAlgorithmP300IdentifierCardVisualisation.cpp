#include "ovpCBoxAlgorithmP300IdentifierCardVisualisation.h"

#include <system/Memory.h>

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

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
		_AutoCast_(IBoxAlgorithmContext& rBoxAlgorithtmContext, const uint32 ui32Index) : m_rBoxAlgorithmContext(rBoxAlgorithtmContext) { m_rBoxAlgorithmContext.getStaticBoxContext()->getSettingValue(ui32Index, m_sSettingValue); }
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
		IBoxAlgorithmContext& m_rBoxAlgorithmContext;
		CString m_sSettingValue;
	};
};
boolean CBoxAlgorithmP300IdentifierCardVisualisation::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pMainWidgetInterface=NULL;

	//get value of settings given in the configuration box
	m_sInterfaceFilename      =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_oBackgroundColor        =_AutoCast_(*this->getBoxAlgorithmContext(), 1);
	m_oTargetBackgroundColor  =_AutoCast_(*this->getBoxAlgorithmContext(), 2);
	m_oSelectedBackgroundColor=_AutoCast_(*this->getBoxAlgorithmContext(), 3);
	m_ui64CardStimulationBase =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------

	m_pSequenceStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pSequenceStimulationDecoder->initialize();

	m_pTargetStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pTargetStimulationDecoder->initialize();

	m_pTargetFlaggingStimulationEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pTargetFlaggingStimulationEncoder->initialize();

	m_pCardSelectionStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pCardSelectionStimulationDecoder->initialize();

	ip_pSequenceMemoryBuffer.initialize(m_pSequenceStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pSequenceStimulationSet.initialize(m_pSequenceStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	ip_pTargetMemoryBuffer.initialize(m_pTargetStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pTargetStimulationSet.initialize(m_pTargetStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	ip_pTargetFlaggingStimulationSet.initialize(m_pTargetFlaggingStimulationEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	op_pTargetFlaggingMemoryBuffer.initialize(m_pTargetFlaggingStimulationEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_ui64LastTime=0;
	m_pMainWidgetInterface=gtk_builder_new(); // glade_xml_new(m_sInterfaceFilename.toASCIIString(), "p300-Identifier-card-main", NULL);
	if(!gtk_builder_add_from_file(m_pMainWidgetInterface, m_sInterfaceFilename.toASCIIString(), NULL))
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Could not load interface file [" << m_sInterfaceFilename << "]\n";
		this->getLogManager() << LogLevel_ImportantWarning << "The file may be missing. However, the interface files now use gtk-builder instead of glade. Did you update your files ?\n";
		return false;
	}

	// m_pToolbarWidgetInterface=glade_xml_new(m_sInterfaceFilename.toASCIIString(), "p300-Identifier-card-toolbar", NULL);
	m_pMainWindow=GTK_WIDGET(gtk_builder_get_object(m_pMainWidgetInterface, "p300-Identifier-card-main"));
	// m_pToolbarWidget=gtk_builder_get_object(m_pToolbarWidgetInterface, "p300-Identifier-card-toolbar");
	m_pTable=GTK_TABLE(gtk_builder_get_object(m_pMainWidgetInterface, "p300-Identifier-card-table"));
	gtk_widget_modify_bg(m_pMainWindow, GTK_STATE_NORMAL, &m_oBackgroundColor);
	
	m_oTargetLabel=GTK_LABEL(gtk_builder_get_object(m_pMainWidgetInterface, "labelTarget"));
	char l_oLabelTarget[80];
	sprintf(l_oLabelTarget, "<span weight=\"bold\" size=\"xx-large\" color=\"#%2x%2x%2x\">Target</span>", (m_oTargetBackgroundColor.red),(m_oTargetBackgroundColor.green),(m_oTargetBackgroundColor.blue));
	gtk_label_set_label(m_oTargetLabel,l_oLabelTarget);
	m_oSelectedLabel=GTK_LABEL(gtk_builder_get_object(m_pMainWidgetInterface, "labelResult"));
	char l_oLabelSelected[80];
	sprintf(l_oLabelSelected, "<span weight=\"bold\" size=\"xx-large\" color=\"#%2x%2x%2x\">Selected</span>", (m_oSelectedBackgroundColor.red),(m_oSelectedBackgroundColor.green),(m_oSelectedBackgroundColor.blue));
	gtk_label_set_label(m_oSelectedLabel,l_oLabelSelected);

	gtk_builder_connect_signals(m_pMainWidgetInterface, NULL);
	// gtk_builder_connect_signals(m_pToolbarWidgetInterface, NULL);

	getVisualisationContext().setWidget(m_pMainWindow);
	// getVisualisationContext().setToolbar(m_pToolbarWidget);

	m_ui64CardCount=0;
	m_iTargetCard=-1;

	m_bTableInitialized=false;
	this->_cache_build_from_table_(m_pTable);
	GtkRequisition size;

	gtk_widget_size_request(GTK_WIDGET(m_vCache[1].pWidget), &size);

	gint widthWork=size.width;
	gint heightWork=size.height;

	for(uint32 i=6; i<l_rStaticBoxContext.getSettingCount(); i++)
		{
			CString l_sForegroundImageFilename=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i);
			if(l_sForegroundImageFilename!=(CString)""){
				::GtkWidget* l_pForegroundImageTarget=gtk_image_new_from_file(l_sForegroundImageFilename.toASCIIString());
				gtk_widget_show(l_pForegroundImageTarget);
				g_object_ref(l_pForegroundImageTarget);
				m_vForegroundImageTarget.push_back(l_pForegroundImageTarget);

				::GtkWidget* l_pForegroundImageWork=gtk_image_new_from_file(l_sForegroundImageFilename.toASCIIString());
				gtk_widget_show(l_pForegroundImageWork);
				g_object_ref(l_pForegroundImageWork);
				GdkPixbuf *pSrcPixbuf, *pDestPixbuf;
				pSrcPixbuf = gtk_image_get_pixbuf(GTK_IMAGE(l_pForegroundImageWork));
				pDestPixbuf = gdk_pixbuf_scale_simple(pSrcPixbuf, widthWork, heightWork, GDK_INTERP_HYPER);
				gtk_image_set_from_pixbuf(GTK_IMAGE(l_pForegroundImageWork),pDestPixbuf);
				m_vForegroundImageWork.push_back(l_pForegroundImageWork);
				
				::GtkWidget* l_pForegroundImageResult=gtk_image_new_from_file(l_sForegroundImageFilename.toASCIIString());
				gtk_widget_show(l_pForegroundImageResult);
				g_object_ref(l_pForegroundImageResult);
				m_vForegroundImageResult.push_back(l_pForegroundImageResult);
				m_ui64CardCount++;
			}
		}

		CString l_sBackgroundImageFilename=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);
		m_pBackgroundImageTarget=gtk_image_new_from_file((l_sBackgroundImageFilename+CString("-offscreen")).toASCIIString());
		gtk_widget_show(m_pBackgroundImageTarget);
		g_object_ref(m_pBackgroundImageTarget);

		m_pBackgroundImageWork=gtk_image_new_from_file((l_sBackgroundImageFilename+CString("-offscreen")).toASCIIString());
		gtk_widget_show(m_pBackgroundImageWork);
		g_object_ref(m_pBackgroundImageWork);
		GdkPixbuf *pSrcPixbuf, *pDestPixbuf;
		pSrcPixbuf = gtk_image_get_pixbuf(GTK_IMAGE(m_pBackgroundImageWork));
		pDestPixbuf = gdk_pixbuf_scale_simple(pSrcPixbuf, widthWork, heightWork, GDK_INTERP_HYPER);
		gtk_image_set_from_pixbuf(GTK_IMAGE(m_pBackgroundImageWork),pDestPixbuf);

		m_pBackgroundImageResult=gtk_image_new_from_file((l_sBackgroundImageFilename+CString("-offscreen")).toASCIIString());
		gtk_widget_show(m_pBackgroundImageResult);
		g_object_ref(m_pBackgroundImageResult);

	this->_cache_change_image_cb_(m_vCache[0], m_pBackgroundImageTarget);
	this->_cache_change_image_cb_(m_vCache[1], m_pBackgroundImageWork);
	this->_cache_change_image_cb_(m_vCache[2], m_pBackgroundImageResult);
	this->_cache_for_each_(&CBoxAlgorithmP300IdentifierCardVisualisation::_cache_change_background_cb_, &m_oBackgroundColor);
	return true;
}

boolean CBoxAlgorithmP300IdentifierCardVisualisation::uninitialize(void)
{
	// g_object_unref(m_pToolbarWidgetInterface);
	// m_pToolbarWidgetInterface=NULL;

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

	if(m_pCardSelectionStimulationDecoder)
	{
		m_pCardSelectionStimulationDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pCardSelectionStimulationDecoder);
		m_pCardSelectionStimulationDecoder=NULL;
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

boolean CBoxAlgorithmP300IdentifierCardVisualisation::processInput(uint32 ui32Index)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	if(!m_bTableInitialized)
	{
		this->_cache_change_image_cb_(m_vCache[0], m_pBackgroundImageTarget);
		this->_cache_change_image_cb_(m_vCache[1], m_pBackgroundImageWork);
		this->_cache_change_image_cb_(m_vCache[2], m_pBackgroundImageResult);
		m_bTableInitialized=true;
	}

	return true;
}


boolean CBoxAlgorithmP300IdentifierCardVisualisation::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint32 i, j;

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

				if(l_ui64StimulationIdentifier >= m_ui64CardStimulationBase && l_ui64StimulationIdentifier < m_ui64CardStimulationBase+m_ui64CardCount)
				{
					int l_iCard=(int)(l_ui64StimulationIdentifier-m_ui64CardStimulationBase);
					if(l_iCard==m_iTargetCard)
					{
						l_oFlaggingStimulationSet.appendStimulation(OVTK_StimulationId_Target, l_pStimulationSet->getStimulationDate(j), 0);
					}
					else
					{
						l_oFlaggingStimulationSet.appendStimulation(OVTK_StimulationId_NonTarget, l_pStimulationSet->getStimulationDate(j), 0);
					}
					this->_cache_change_image_cb_(m_vCache[1],m_vForegroundImageWork[l_iCard]);

				}
				else if(l_ui64StimulationIdentifier == OVTK_StimulationId_ExperimentStart)
				{
					this->getLogManager() << LogLevel_Debug << "Received OVTK_StimulationId_ExperimentStart - resets grid\n";
					this->_cache_change_image_cb_(m_vCache[0],m_pBackgroundImageTarget);
					this->_cache_change_background_cb_(m_vCache[0],&m_oBackgroundColor);
					this->_cache_change_image_cb_(m_vCache[1],m_pBackgroundImageWork);
					this->_cache_change_background_cb_(m_vCache[1],&m_oBackgroundColor);
					this->_cache_change_image_cb_(m_vCache[2],m_pBackgroundImageResult);
					this->_cache_change_background_cb_(m_vCache[2],&m_oBackgroundColor);
				}else if(l_ui64StimulationIdentifier == OVTK_StimulationId_VisualStimulationStop)
				{
					this->getLogManager() << LogLevel_Debug << "Received OVTK_StimulationId_VisualStimulationStop - resets grid\n";
					//this->_cache_change_image_cb_(m_vCache[1], m_pBackgroundImageWork);
					::GtkContainer* l_pContainer=GTK_CONTAINER(m_vCache[1].pWidget);
					gtk_container_remove(l_pContainer, m_vCache[1].pImage);
					m_vCache[1].pImage=NULL;
				}else if(l_ui64StimulationIdentifier == OVTK_StimulationId_SegmentStop)
				{
					this->getLogManager() << LogLevel_Debug << "Received OVTK_StimulationId_SegmentStop - resets grid\n";
					this->_cache_change_image_cb_(m_vCache[1], m_pBackgroundImageWork);
				}else if(l_ui64StimulationIdentifier == OVTK_StimulationId_ExperimentStop)
				{
					this->getLogManager() << LogLevel_Debug << "Received OVTK_StimulationId_ExperimentStop - resets grid\n";
					this->_cache_change_image_cb_(m_vCache[0],m_pBackgroundImageTarget);
					this->_cache_change_image_cb_(m_vCache[1],m_pBackgroundImageWork);
					this->_cache_change_image_cb_(m_vCache[2],m_pBackgroundImageResult);
					this->_cache_change_background_cb_(m_vCache[0],&m_oBackgroundColor);
					this->_cache_change_background_cb_(m_vCache[1],&m_oBackgroundColor);
					this->_cache_change_background_cb_(m_vCache[2],&m_oBackgroundColor);
				}else if(l_ui64StimulationIdentifier == OVTK_StimulationId_RestStop)
				{
					this->getLogManager() << LogLevel_Debug << "Received OVTK_StimulationId_RestStop - resets grid\n";
					this->_cache_change_image_cb_(m_vCache[2],m_pBackgroundImageResult);
					this->_cache_change_background_cb_(m_vCache[2],&m_oBackgroundColor);

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
					if(l_ui64StimulationIdentifier >= m_ui64CardStimulationBase && l_ui64StimulationIdentifier < m_ui64CardStimulationBase+m_ui64CardCount)
					{
						this->getLogManager() << LogLevel_Debug << "Received Target Card " << l_ui64StimulationIdentifier << "\n";
						m_iTargetCard=(int)(l_ui64StimulationIdentifier-m_ui64CardStimulationBase);

						this->getLogManager() << LogLevel_Debug << "Displays Target Cell\n";
						this->_cache_change_image_cb_(m_vCache[0],m_vForegroundImageTarget[m_iTargetCard]);
						this->_cache_change_background_cb_(m_vCache[0],&m_oTargetBackgroundColor);
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

	TParameterHandler < const IMemoryBuffer* > ip_pSelectionMemoryBuffer(m_pCardSelectionStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	TParameterHandler < IStimulationSet* > op_pSelectionStimulationSet(m_pCardSelectionStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(2); i++)
	{
		if(m_ui64LastTime>=l_rDynamicBoxContext.getInputChunkStartTime(2, i))
		{
			ip_pSelectionMemoryBuffer=l_rDynamicBoxContext.getInputChunk(2, i);
			m_pCardSelectionStimulationDecoder->process();

			if(m_pCardSelectionStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
			}

			if(m_pCardSelectionStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				IStimulationSet* l_pStimulationSet=op_pSelectionStimulationSet;
				for(j=0; j<l_pStimulationSet->getStimulationCount(); j++)
				{
					uint64 l_ui64StimulationIdentifier=l_pStimulationSet->getStimulationIdentifier(j);
					if(l_ui64StimulationIdentifier >= m_ui64CardStimulationBase && l_ui64StimulationIdentifier < m_ui64CardStimulationBase+m_ui64CardCount)
					{
						this->getLogManager() << LogLevel_Debug << "Received Selected Card " << l_ui64StimulationIdentifier << "\n";
						int l_iSelectedCard=(int)(l_ui64StimulationIdentifier-m_ui64CardStimulationBase);

						this->getLogManager() << LogLevel_Debug << "Displays Selected Cell\n";

						this->_cache_change_image_cb_(m_vCache[1],m_pBackgroundImageWork);
						this->_cache_change_image_cb_(m_vCache[2],m_vForegroundImageResult[l_iSelectedCard]);
						this->_cache_change_background_cb_(m_vCache[2],&m_oSelectedBackgroundColor);
					}
					if(l_ui64StimulationIdentifier == OVTK_StimulationId_Label_00)
					{
						this->getLogManager() << LogLevel_Trace << "Selection Rejected !\n";
						std::string l_sString;
						l_sString=gtk_label_get_text(m_pResult);
						l_sString+="*";
						gtk_label_set_text(m_pResult, l_sString.c_str());
					}
				}
			}

			if(m_pCardSelectionStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}

			l_rDynamicBoxContext.markInputAsDeprecated(2, i);
		}
	}
	return true;
}

// _________________________________________________________________________________________________________________________________________________________
//

void CBoxAlgorithmP300IdentifierCardVisualisation::_cache_build_from_table_(::GtkTable* pTable)
{
	if(pTable)
	{
		::GtkTableChild* l_pTableChild=NULL;
		::GList* l_pList=NULL;
		for(l_pList=pTable->children; l_pList; l_pList=l_pList->next)
		{
			l_pTableChild=(::GtkTableChild*)l_pList->data;

			if(l_pTableChild->top_attach!=0){
				int l_iIndex=0;
				for(unsigned long i=l_pTableChild->top_attach; i<l_pTableChild->bottom_attach; i++)
				{
					for(unsigned long j=l_pTableChild->left_attach; j<l_pTableChild->right_attach; j++)
					{
						l_iIndex++;
						SWidgetStyle l_rWidgetStyle;
						l_rWidgetStyle.iIndex=l_iIndex;
						l_rWidgetStyle.pParent=l_pTableChild->widget;
						l_rWidgetStyle.pWidget=gtk_bin_get_child(GTK_BIN(l_rWidgetStyle.pParent));
						l_rWidgetStyle.pImage=gtk_bin_get_child(GTK_BIN(l_rWidgetStyle.pWidget));
						m_vCache.push_back(l_rWidgetStyle);
					}
				}
			}
		}
	}
}

void CBoxAlgorithmP300IdentifierCardVisualisation::_cache_for_each_(_cache_callback_ fpCallback, void* pUserData)
{

	for(uint32 i=0; i<m_vCache.size(); i++)
	{
		(this->*fpCallback)(m_vCache[i], pUserData);
	}
}

void CBoxAlgorithmP300IdentifierCardVisualisation::_cache_for_each_if_(int iCard, _cache_callback_ fpIfCallback, _cache_callback_ fpElseCallback, void* pIfUserData, void* pElseUserData)
{

	for(uint32 i=0; i<m_vCache.size(); i++)
	{
		if(iCard==m_vCache[i].iIndex)
		{
			(this->*fpIfCallback)(m_vCache[i], pIfUserData);
		}
		else
		{
			(this->*fpElseCallback)(m_vCache[i], pElseUserData);
		}
	}
}

void CBoxAlgorithmP300IdentifierCardVisualisation::_cache_change_null_cb_(CBoxAlgorithmP300IdentifierCardVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
}

void CBoxAlgorithmP300IdentifierCardVisualisation::_cache_change_image_cb_(CBoxAlgorithmP300IdentifierCardVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
	::GtkContainer* m_pContainer=GTK_CONTAINER(rWidgetStyle.pWidget);
	::GtkWidget* l_pImage=(::GtkWidget*) pUserData;

	if(rWidgetStyle.pImage!=l_pImage)
	{
		if(rWidgetStyle.pImage)
		{
			gtk_container_remove(m_pContainer, rWidgetStyle.pImage);
		}
		gtk_container_add(m_pContainer, l_pImage);
		rWidgetStyle.pImage=l_pImage;
	}
}

void CBoxAlgorithmP300IdentifierCardVisualisation::_cache_change_background_cb_(CBoxAlgorithmP300IdentifierCardVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData)
{
	::GdkColor oColor=*(::GdkColor*)pUserData;
	if(!System::Memory::compare(&rWidgetStyle.oBackgroundColor, &oColor, sizeof(::GdkColor)))
	{
		gtk_widget_modify_bg(rWidgetStyle.pParent, GTK_STATE_NORMAL, &oColor);
		gtk_widget_modify_bg(rWidgetStyle.pWidget, GTK_STATE_NORMAL, &oColor);
		gtk_widget_modify_bg(rWidgetStyle.pImage, GTK_STATE_NORMAL, &oColor);
		rWidgetStyle.oBackgroundColor=oColor;
	}
}

