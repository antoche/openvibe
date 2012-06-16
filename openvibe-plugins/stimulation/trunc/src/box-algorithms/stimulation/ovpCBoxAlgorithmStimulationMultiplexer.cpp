#include "ovpCBoxAlgorithmStimulationMultiplexer.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

using namespace std;

/*
uint64 CBoxAlgorithmStimulationMultiplexer::getClockFrequency(void)
{
	return 0; // the box clock frequency
}
*/

boolean CBoxAlgorithmStimulationMultiplexer::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_vStreamDecoder.resize(getStaticBoxContext().getInputCount());
	m_vStreamDecoderEndTime.resize(getStaticBoxContext().getInputCount());
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
		m_vStreamDecoder[i]->initialize();
		m_vStreamDecoderEndTime[i]=0;
	}
	m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStreamEncoder->initialize();

	m_ui64LastStartTime=0;
	m_ui64LastEndTime=0;
	m_bHasSentHeader=false;

	return true;
}

boolean CBoxAlgorithmStimulationMultiplexer::uninitialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pStreamEncoder->uninitialize();
	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]->uninitialize();
		getAlgorithmManager().releaseAlgorithm(*m_vStreamDecoder[i]);
	}
	m_vStreamDecoder.clear();

	return true;
}

/*
boolean CBoxAlgorithmStimulationMultiplexer::processEvent(IMessageEvent& rMessageEvent)
{
	// ...

	// getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

/*
boolean CBoxAlgorithmStimulationMultiplexer::processSignal(IMessageSignal& rMessageSignal)
{
	// ...

	// getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

/*
boolean CBoxAlgorithmStimulationMultiplexer::processClock(IMessageClock& rMessageClock)
{
	// ...

	// getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

boolean CBoxAlgorithmStimulationMultiplexer::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmStimulationMultiplexer::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint32 i,j,k;

	TParameterHandler < IMemoryBuffer* > l_opMemoryBuffer(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	if(!m_bHasSentHeader)
	{
		l_opMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
		m_pStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastEndTime, m_ui64LastEndTime);
		m_bHasSentHeader=true;
	}

	uint64 l_ui64ReadChunkMinEndTime=0xffffffffffffffffll;
	for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > l_ipMemoryBuffer(m_vStreamDecoder[i]->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
			l_ipMemoryBuffer=l_rDynamicBoxContext.getInputChunk(i, j);
			m_vStreamDecoder[i]->process();

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
			}

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				TParameterHandler < IStimulationSet* > l_opStimulationSet(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
				for(k=0; k<l_opStimulationSet->getStimulationCount(); k++)
				{
					SStimulation l_oStimulation;
					l_oStimulation.m_ui64Identifier=l_opStimulationSet->getStimulationIdentifier(k);
					l_oStimulation.m_ui64Date=l_opStimulationSet->getStimulationDate(k);
					l_oStimulation.m_ui64Duration=l_opStimulationSet->getStimulationDuration(k);
					m_vStimulation.insert(make_pair(l_oStimulation.m_ui64Date, l_oStimulation));
				}
			}

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}

			m_vStreamDecoderEndTime[i]=l_rDynamicBoxContext.getInputChunkEndTime(i, j);
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}

		if(l_ui64ReadChunkMinEndTime > m_vStreamDecoderEndTime[i])
		{
			l_ui64ReadChunkMinEndTime=m_vStreamDecoderEndTime[i];
		}
	}

	if(l_ui64ReadChunkMinEndTime!=m_ui64LastEndTime)
	{
		multimap < uint64, SStimulation >::iterator it;
		multimap < uint64, SStimulation >::iterator it_backup;
		multimap < uint64, SStimulation > l_vStimulationToSend;
		for(it=m_vStimulation.begin(); it!=m_vStimulation.end(); )
		{
			if(it->first < l_ui64ReadChunkMinEndTime)
			{
				it_backup=it;
				it++;
				l_vStimulationToSend.insert(make_pair(it_backup->first, it_backup->second));
				m_vStimulation.erase(it_backup);
			}
			else
			{
				it++;
			}
		}

		TParameterHandler < IStimulationSet* > l_ipStimulationSet(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
		l_ipStimulationSet->setStimulationCount(l_vStimulationToSend.size());
		for(k=0, it=l_vStimulationToSend.begin(); it!=l_vStimulationToSend.end(); it++, k++)
		{
			l_ipStimulationSet->setStimulationIdentifier(k, it->second.m_ui64Identifier);
			l_ipStimulationSet->setStimulationDate(k, it->second.m_ui64Date);
			l_ipStimulationSet->setStimulationDuration(k, it->second.m_ui64Duration);
		}

		l_opMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
		m_pStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastEndTime, l_ui64ReadChunkMinEndTime);

		m_ui64LastStartTime=m_ui64LastEndTime;
		m_ui64LastEndTime=l_ui64ReadChunkMinEndTime;
	}

	return true;
}
