#include "ovpCBoxAlgorithmPlayerController.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

boolean CBoxAlgorithmPlayerController::initialize(void)
{
	m_ui64StimulationIdentifier=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64ActionIdentifier     =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pStimulationSet.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	return true;
}

boolean CBoxAlgorithmPlayerController::uninitialize(void)
{
	op_pStimulationSet.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	if(m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
		m_pStreamDecoder=NULL;
	}

	return true;
}

boolean CBoxAlgorithmPlayerController::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmPlayerController::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			IStimulationSet* l_pStimulationSet=op_pStimulationSet;
			for(uint32 j=0; j<l_pStimulationSet->getStimulationCount(); j++)
			{
				if(l_pStimulationSet->getStimulationIdentifier(j) == m_ui64StimulationIdentifier)
				{
					this->getLogManager()
						<< LogLevel_Trace << "Received stimulation ["
						<< this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, m_ui64StimulationIdentifier) << "] causing action ["
						<< this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_PlayerAction, m_ui64ActionIdentifier) << "]\n";

					boolean l_bResult=true;
					if(m_ui64ActionIdentifier == OV_TypeId_PlayerAction_Play)
					{
						l_bResult=this->getPlayerContext().play();
					}
					if(m_ui64ActionIdentifier == OV_TypeId_PlayerAction_Stop)
					{
						l_bResult=this->getPlayerContext().stop();
					}
					if(m_ui64ActionIdentifier == OV_TypeId_PlayerAction_Pause)
					{
						l_bResult=this->getPlayerContext().pause();
					}
					if(m_ui64ActionIdentifier == OV_TypeId_PlayerAction_Forward)
					{
						l_bResult=this->getPlayerContext().forward();
					}

					if(!l_bResult)
					{
						this->getLogManager() << LogLevel_Warning << "Failed to request player action [" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_PlayerAction, m_ui64ActionIdentifier) << "]\n";
					}
				}
			}
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	// ...

	// l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	// l_rStaticBoxContext.getSettingCount();

	// l_rDynamicBoxContext.getInputChunkCount()
	// l_rDynamicBoxContext.getInputChunk(i, )
	// l_rDynamicBoxContext.getOutputChunk(i, )

	return true;
}
