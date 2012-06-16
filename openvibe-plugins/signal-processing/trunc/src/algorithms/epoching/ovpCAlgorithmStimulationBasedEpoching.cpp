#include "ovpCAlgorithmStimulationBasedEpoching.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CAlgorithmStimulationBasedEpoching::initialize(void)
{
	m_pInputSignal.initialize(getInputParameter(OVP_Algorithm_StimulationBasedEpoching_InputParameterId_InputSignal));
	m_ui64OffsetSampleCount.initialize(getInputParameter(OVP_Algorithm_StimulationBasedEpoching_InputParameterId_OffsetSampleCount));
	m_pOutputSignal.initialize(getOutputParameter(OVP_Algorithm_StimulationBasedEpoching_OutputParameterId_OutputSignal));
	m_ui64EndTimeChunkToProcess.initialize(getInputParameter(OVP_Algorithm_StimulationBasedEpoching_InputParameterId_EndTimeChunkToProcess));

	return true;
}

boolean CAlgorithmStimulationBasedEpoching::uninitialize(void)
{
	m_pOutputSignal.uninitialize();
	m_ui64OffsetSampleCount.uninitialize();
	m_pInputSignal.uninitialize();
	m_ui64EndTimeChunkToProcess.uninitialize();

	return true;
}

boolean CAlgorithmStimulationBasedEpoching::process(void)
{
	if(isInputTriggerActive(OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_Reset))
	{
		m_ui64ReceivedSamples=0;
		m_ui64TimeLastProcessedChunk = m_ui64EndTimeChunkToProcess-1;
		m_ui64SamplesToSkip=m_ui64OffsetSampleCount;
	}

	if(isInputTriggerActive(OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_PerformEpoching) &&
			(m_ui64TimeLastProcessedChunk<m_ui64EndTimeChunkToProcess) )
	{
		m_ui64TimeLastProcessedChunk = m_ui64EndTimeChunkToProcess;
		uint32 l_ui32InputSampleCount=m_pInputSignal->getDimensionSize(1);
		uint32 l_ui32OutputSampleCount=m_pOutputSignal->getDimensionSize(1);

		if(m_ui64SamplesToSkip!=0)
		{
			if(m_ui64SamplesToSkip>=l_ui32InputSampleCount)
			{
				m_ui64SamplesToSkip-=l_ui32InputSampleCount;
				return true;
			}
		}

		uint32 l_ui32SamplesToCopy=(uint32)(l_ui32InputSampleCount-m_ui64SamplesToSkip);
		if(l_ui32SamplesToCopy>=l_ui32OutputSampleCount-m_ui64ReceivedSamples)
		{
			l_ui32SamplesToCopy=(uint32)(l_ui32OutputSampleCount-m_ui64ReceivedSamples);
			this->activateOutputTrigger(OVP_Algorithm_StimulationBasedEpoching_OutputTriggerId_EpochingDone, true);
		}

		if(l_ui32SamplesToCopy)
		{
			for(uint32 i=0; i<m_pInputSignal->getDimensionSize(0); i++)
			{
				System::Memory::copy(
					m_pOutputSignal->getBuffer()+i*l_ui32OutputSampleCount+m_ui64ReceivedSamples,
					m_pInputSignal->getBuffer()+i*l_ui32InputSampleCount+m_ui64SamplesToSkip,
					l_ui32SamplesToCopy*sizeof(float64));
			}
		}

		m_ui64ReceivedSamples+=l_ui32SamplesToCopy;
		m_ui64SamplesToSkip=0;
	}

	return true;
}
