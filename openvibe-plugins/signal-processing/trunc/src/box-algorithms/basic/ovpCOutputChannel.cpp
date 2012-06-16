#include "ovpCOutputChannel.h"

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;


boolean COutputChannel::initialize(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm>* pTBoxAlgorithm)
{
	m_pTBoxAlgorithm            = pTBoxAlgorithm;


	m_pStreamEncoderSignal      = &m_pTBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pTBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoderSignal->initialize();
	op_pMemoryBufferSignal.initialize(m_pStreamEncoderSignal->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	ip_pMatrixSignal.initialize(m_pStreamEncoderSignal->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	ip_ui64SamplingRateSignal.initialize(m_pStreamEncoderSignal->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));


	m_pStreamEncoderStimulation = &m_pTBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pTBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStreamEncoderStimulation->initialize();
	op_pMemoryBufferStimulation.initialize(m_pStreamEncoderStimulation->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	ip_pStimulationSetStimulation.initialize(m_pStreamEncoderStimulation->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	
	return true;
}

boolean COutputChannel::uninitialize()
{	
	ip_ui64SamplingRateSignal.uninitialize();
	ip_pMatrixSignal.uninitialize();
	op_pMemoryBufferSignal.uninitialize();
	m_pStreamEncoderSignal->uninitialize();
	m_pTBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderSignal);

	op_pMemoryBufferStimulation.uninitialize();
	ip_pStimulationSetStimulation.uninitialize();
	m_pStreamEncoderStimulation->uninitialize();
	m_pTBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderStimulation);

	return true;
}

void COutputChannel::sendStimulation(IStimulationSet* stimset, OpenViBE::uint64 startTimestamp, OpenViBE::uint64 endTimestamp)
{
	IBoxIO& l_rDynamicBoxContext   = m_pTBoxAlgorithm->getDynamicBoxContext();

	for(uint32 j=0; j < stimset->getStimulationCount(); j++)
	
	for(uint32 j=0; j < stimset->getStimulationCount(); j++)
	{
		if(stimset->getStimulationDate(j) < m_ui64TimeStimulationPosition)
		{
			stimset->removeStimulation(j);
			j--;
		}
		else
		{
			stimset->setStimulationDate(j, stimset->getStimulationDate(j) - m_ui64TimeStimulationPosition);
		}
	}

	ip_pStimulationSetStimulation  = stimset;
	op_pMemoryBufferStimulation    = l_rDynamicBoxContext.getOutputChunk(STIMULATION_CHANNEL);
	m_pStreamEncoderStimulation->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
	l_rDynamicBoxContext.markOutputAsReadyToSend(STIMULATION_CHANNEL, startTimestamp - m_ui64TimeStimulationPosition, endTimestamp - m_ui64TimeStimulationPosition);
}

void COutputChannel::sendHeader(OpenViBE::uint64 samplingRate, OpenViBE::CMatrix* pMatrix)
{
	IBoxIO& l_rDynamicBoxContext=m_pTBoxAlgorithm->getDynamicBoxContext();

	m_oMatrixBuffer           = pMatrix;
	m_ui64SamplingRate        = samplingRate;

	op_pMemoryBufferSignal    = l_rDynamicBoxContext.getOutputChunk(SIGNAL_CHANNEL);
	ip_pMatrixSignal          = m_oMatrixBuffer;
	ip_ui64SamplingRateSignal = m_ui64SamplingRate;
	m_pStreamEncoderSignal->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
	l_rDynamicBoxContext.markOutputAsReadyToSend(SIGNAL_CHANNEL, 0, 0);
}

void COutputChannel::sendSignal(OpenViBE::CMatrix* pMatrix, OpenViBE::uint64 startTimestamp, OpenViBE::uint64 endTimestamp)
{
	IBoxIO& l_rDynamicBoxContext   = m_pTBoxAlgorithm->getDynamicBoxContext();

	op_pMemoryBufferSignal         = l_rDynamicBoxContext.getOutputChunk(SIGNAL_CHANNEL);
	ip_pMatrixSignal               = pMatrix;
	ip_ui64SamplingRateSignal      = m_ui64SamplingRate;
	m_pStreamEncoderSignal->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
	l_rDynamicBoxContext.markOutputAsReadyToSend(SIGNAL_CHANNEL, startTimestamp - m_ui64TimeSignalPosition, endTimestamp - m_ui64TimeSignalPosition);
}

void COutputChannel::processSynchroSignal(OpenViBE::uint64 stimulationPosition, OpenViBE::uint64 signalPosition)
{
	m_ui64TimeStimulationPosition = stimulationPosition;
	m_ui64TimeSignalPosition      = signalPosition;
}

