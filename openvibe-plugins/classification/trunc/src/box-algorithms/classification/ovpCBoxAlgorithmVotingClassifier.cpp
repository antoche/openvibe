#include "ovpCBoxAlgorithmVotingClassifier.h"

#include <system/Memory.h>

#include <list>
#include <vector>
#include <string>
#include <algorithm>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Classification;

boolean CBoxAlgorithmVotingClassifier::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i;

	m_pClassificationChoiceEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pClassificationChoiceEncoder->initialize();

	ip_pClassificationChoiceStimulationSet.initialize(m_pClassificationChoiceEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	op_pClassificationChoiceMemoryBuffer.initialize(m_pClassificationChoiceEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	CIdentifier l_oTypeIdentifier;
	l_rStaticBoxContext.getInputType(0, l_oTypeIdentifier);
	m_bMatrixBased=(l_oTypeIdentifier==OV_TypeId_StreamedMatrix);

	for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		SInput& l_rInput=m_vClassificationResults[i];
		if(m_bMatrixBased)
		{
			l_rInput.m_pDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
			l_rInput.m_pDecoder->initialize();

			l_rInput.ip_pMemoryBuffer.initialize(l_rInput.m_pDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
			l_rInput.op_pMatrix.initialize(l_rInput.m_pDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

			m_oStreamDecoder_OutputTriggerId_ReceivedHeader=OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader;
			m_oStreamDecoder_OutputTriggerId_ReceivedBuffer=OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer;
			m_oStreamDecoder_OutputTriggerId_ReceivedEnd=OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd;
		}
		else
		{
			l_rInput.m_pDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
			l_rInput.m_pDecoder->initialize();

			l_rInput.ip_pMemoryBuffer.initialize(l_rInput.m_pDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
			l_rInput.op_pStimulationSet.initialize(l_rInput.m_pDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

			m_oStreamDecoder_OutputTriggerId_ReceivedHeader=OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader;
			m_oStreamDecoder_OutputTriggerId_ReceivedBuffer=OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer;
			m_oStreamDecoder_OutputTriggerId_ReceivedEnd=OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd;
		}
	}

	m_ui64NumberOfRepetitions  =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64TargetClassLabel     =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_ui64NonTargetClassLabel  =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_ui64RejectClassLabel     =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	m_ui64ResultClassLabelBase =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
	m_bChooseOneIfExAequo      =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);

	m_ui64LastTime=0;

	m_pClassificationChoiceEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
	l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastTime, this->getPlayerContext().getCurrentTime());

	return true;
}

boolean CBoxAlgorithmVotingClassifier::uninitialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i;

	for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		SInput& l_rInput=m_vClassificationResults[i];
		l_rInput.m_pDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*l_rInput.m_pDecoder);
	}

	m_pClassificationChoiceEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pClassificationChoiceEncoder);

	return true;
}

boolean CBoxAlgorithmVotingClassifier::processInput(uint32 ui32Index)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmVotingClassifier::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i, j, k;

	boolean l_bCanChoose=true;

	for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		SInput& l_rInput=m_vClassificationResults[i];
		for(j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			l_rInput.ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(i, j);
			l_rInput.m_pDecoder->process();
			if(l_rInput.m_pDecoder->isOutputTriggerActive(m_oStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				if(m_bMatrixBased)
				{
					if(l_rInput.op_pMatrix->getBufferElementCount() != 1)
					{
						this->getLogManager() << LogLevel_ImportantWarning << "Input matrix should have a single value\n";
						return false;
					}
				}
			}
			if(l_rInput.m_pDecoder->isOutputTriggerActive(m_oStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				if(m_bMatrixBased)
				{
					l_rInput.m_vScore.push_back(std::pair<float64, uint64>(-l_rInput.op_pMatrix->getBuffer()[0], l_rDynamicBoxContext.getInputChunkEndTime(i, j)));
				}
				else
				{
					for(k=0; k<l_rInput.op_pStimulationSet->getStimulationCount(); k++)
					{
						uint64 l_ui64StimulationIdentifier=l_rInput.op_pStimulationSet->getStimulationIdentifier(k);
						if(l_ui64StimulationIdentifier == m_ui64TargetClassLabel || l_ui64StimulationIdentifier == m_ui64NonTargetClassLabel || l_ui64StimulationIdentifier == m_ui64RejectClassLabel)
						{
							l_rInput.m_vScore.push_back(std::pair<float64, uint64>(l_ui64StimulationIdentifier == m_ui64TargetClassLabel ? 1 : 0, l_rInput.op_pStimulationSet->getStimulationDate(k)));
						}
					}
				}
			}
			if(l_rInput.m_pDecoder->isOutputTriggerActive(m_oStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}

		if(l_rInput.m_vScore.size() < m_ui64NumberOfRepetitions)
		{
			l_bCanChoose=false;
		}
	}

	if(l_bCanChoose)
	{
		float64 l_f64ResultScore=-1E100;
		uint64 l_ui64ResultClassLabel = m_ui64RejectClassLabel;
		uint64 l_ui64Time = 0;

		std::map < uint32, float64 > l_vScore;
		for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			SInput& l_rInput=m_vClassificationResults[i];
			l_vScore[i]=0;
			for(j=0; j<m_ui64NumberOfRepetitions; j++)
			{
				l_vScore[i]+=l_rInput.m_vScore[j].first;
			}

			if(l_vScore[i] > l_f64ResultScore)
			{
				l_f64ResultScore = l_vScore[i];
				l_ui64ResultClassLabel = m_ui64ResultClassLabelBase + i;
				l_ui64Time = l_rInput.m_vScore[m_ui64NumberOfRepetitions-1].second;
			}
			else if(l_vScore[i] == l_f64ResultScore)
			{
				if(!m_bChooseOneIfExAequo)
				{
					l_f64ResultScore = l_vScore[i];
					l_ui64ResultClassLabel = m_ui64RejectClassLabel;
					l_ui64Time = l_rInput.m_vScore[m_ui64NumberOfRepetitions-1].second;
				}
			}

			l_rInput.m_vScore.erase(l_rInput.m_vScore.begin(), l_rInput.m_vScore.begin()+m_ui64NumberOfRepetitions);

			this->getLogManager() << LogLevel_Trace << "Input " << i << " got score " << l_vScore[i] << "\n";
		}

		if(l_ui64ResultClassLabel != m_ui64RejectClassLabel)
		{
			this->getLogManager() << LogLevel_Trace << "Chosed " << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, l_ui64ResultClassLabel) << " with score " << l_f64ResultScore << "\n";
		}
		else
		{
			this->getLogManager() << LogLevel_Trace << "Chosed rejection " << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, l_ui64ResultClassLabel) << "\n";
		}

		CStimulationSet l_oStimulationSet;
		l_oStimulationSet.appendStimulation(l_ui64ResultClassLabel, l_ui64Time, 0);
		ip_pClassificationChoiceStimulationSet=&l_oStimulationSet;
		op_pClassificationChoiceMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
		m_pClassificationChoiceEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastTime, l_ui64Time);
		m_ui64LastTime=l_ui64Time;
	}

	return true;
}
