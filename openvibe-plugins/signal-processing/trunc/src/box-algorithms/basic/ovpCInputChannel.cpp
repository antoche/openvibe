#include "ovpCInputChannel.h"

#include <iostream>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

namespace
{
	class _AutoCast_
	{
	public:
		_AutoCast_(IBox& rBox, IConfigurationManager& rConfigurationManager, const uint32 ui32Index) : m_rConfigurationManager(rConfigurationManager) { rBox.getSettingValue(ui32Index, m_sSettingValue); }
		operator uint64 (void) { return m_rConfigurationManager.expandAsUInteger(m_sSettingValue); }
		operator int64 (void) { return m_rConfigurationManager.expandAsInteger(m_sSettingValue); }
		operator float64 (void) { return m_rConfigurationManager.expandAsFloat(m_sSettingValue); }
		operator boolean (void) { return m_rConfigurationManager.expandAsBoolean(m_sSettingValue); }
		operator const CString (void) { return m_sSettingValue; }
	protected:
		IConfigurationManager& m_rConfigurationManager;
		CString m_sSettingValue;
	};
};

CInputChannel::~CInputChannel()
{ 
	if(m_oMatrixBuffer[0]) {delete m_oMatrixBuffer[0];}
	if(m_oMatrixBuffer[1]) {delete m_oMatrixBuffer[1];}
}

boolean CInputChannel::initialize(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm>* pTBoxAlgorithm)
{
	m_ui32Status                    = 0;
	m_oMatrixBuffer[0]              = 0;
	m_oMatrixBuffer[1]              = 0;

	m_ui64TimeStimulationPosition   = 0;
	m_ui64TimeStampStartStimulation = 0;
	m_ui64TimeStampEndStimulation   = 0;
	m_bFirstStimulation             = false;

	m_ui64TimeSignalPosition        = 0;
	m_ui64TimeStampStartSignal      = 0;
	m_ui64TimeStampEndSignal        = 0;
	
	m_oIStimulationSet              = 0;
	m_pTBoxAlgorithm                = pTBoxAlgorithm;
	m_ui64PtrMatrixIndex            = 0;

	m_ui64SynchroStimulation        = m_pTBoxAlgorithm->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, _AutoCast_(m_pTBoxAlgorithm->getStaticBoxContext(), m_pTBoxAlgorithm->getConfigurationManager(), 0));

	m_pStreamDecoderSignal          = &m_pTBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pTBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pStreamDecoderSignal->initialize();
	ip_pMemoryBufferSignal.initialize(m_pStreamDecoderSignal->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrixSignal.initialize(m_pStreamDecoderSignal->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_ui64SamplingRateSignal.initialize(m_pStreamDecoderSignal->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	m_pStreamDecoderStimulation     = &m_pTBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pTBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStreamDecoderStimulation->initialize();
	ip_pMemoryBufferStimulation.initialize(m_pStreamDecoderStimulation->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pStimulationSetStimulation.initialize(m_pStreamDecoderStimulation->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	return true;
}

boolean CInputChannel::uninitialize()
{
	op_pStimulationSetStimulation.uninitialize();
	ip_pMemoryBufferStimulation.uninitialize();
	m_pStreamDecoderStimulation->uninitialize();
	m_pTBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoderStimulation);

	
	op_ui64SamplingRateSignal.uninitialize();
	op_pMatrixSignal.uninitialize();
	ip_pMemoryBufferSignal.uninitialize();
	m_pStreamDecoderSignal->uninitialize();
	m_pTBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoderSignal);


	return true;
}

boolean CInputChannel::waitForSignalHeader()
{
	IBoxIO& l_rDynamicBoxContext=m_pTBoxAlgorithm->getDynamicBoxContext();

	if(l_rDynamicBoxContext.getInputChunkCount(SIGNAL_CHANNEL))
	{
		ip_pMemoryBufferSignal    = l_rDynamicBoxContext.getInputChunk(SIGNAL_CHANNEL, 0);
		m_pStreamDecoderSignal->process();

		if(m_pStreamDecoderSignal->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_ui32Status          |= SIGNAL_HEADER_DETECTED;

			if(m_oMatrixBuffer[0]) {delete m_oMatrixBuffer[0];}
			m_oMatrixBuffer[0] = new CMatrix();
			if(m_oMatrixBuffer[1]) {delete m_oMatrixBuffer[1];}
			m_oMatrixBuffer[1] = new CMatrix();

			OpenViBEToolkit::Tools::Matrix::copyDescription(*m_oMatrixBuffer[0], *op_pMatrixSignal);
			OpenViBEToolkit::Tools::Matrix::copyDescription(*m_oMatrixBuffer[1], *op_pMatrixSignal);
			l_rDynamicBoxContext.markInputAsDeprecated(SIGNAL_CHANNEL, 0);

			return true;
		}
	}

	return false;
}

void CInputChannel::waitForSynchro()
{
	waitForSynchroStimulation();
	waitForSynchroSignal();
}

void CInputChannel::waitForSynchroStimulation()
{
	if(hasSynchroStimulation())
		return;

	IBoxIO& l_rDynamicBoxContext=m_pTBoxAlgorithm->getDynamicBoxContext();

	for(uint32 i=0; i < l_rDynamicBoxContext.getInputChunkCount(STIMULATION_CHANNEL); i++) //Stimulation de l'input 1
	{
		ip_pMemoryBufferStimulation       = l_rDynamicBoxContext.getInputChunk(STIMULATION_CHANNEL, i);
		m_pStreamDecoderStimulation->process();
		m_oIStimulationSet=op_pStimulationSetStimulation;

		m_ui64TimeStampStartStimulation   = l_rDynamicBoxContext.getInputChunkStartTime(STIMULATION_CHANNEL, i);
		m_ui64TimeStampEndStimulation     = l_rDynamicBoxContext.getInputChunkEndTime(STIMULATION_CHANNEL, i);
		
		for(uint32 j=0; j < m_oIStimulationSet->getStimulationCount(); j++)
		{
			if(m_oIStimulationSet->getStimulationIdentifier(j) == m_ui64SynchroStimulation)
			{
				m_ui32Status                     |= STIMULATION_SYNCHRO_DETECTED;
				m_ui64TimeStimulationPosition     = m_oIStimulationSet->getStimulationDate(j);
				m_pTBoxAlgorithm->getLogManager() << LogLevel_Info << "Get Synchronisation Stimulation at channel "<< STIMULATION_CHANNEL << "\n";
				
				return;
			}
		}
		l_rDynamicBoxContext.markInputAsDeprecated(STIMULATION_CHANNEL, i);
	}
}

void CInputChannel::waitForSynchroSignal()
{
	if(m_ui64TimeStampStartStimulation == 0)
		return;

	if(hasSynchroSignal())
		return;

	IBoxIO& l_rDynamicBoxContext=m_pTBoxAlgorithm->getDynamicBoxContext();

	if(hasSynchroStimulation())
	{
		for(uint32 i=0; i < l_rDynamicBoxContext.getInputChunkCount(SIGNAL_CHANNEL); i++) //Stimulation de l'input 1
		{
			m_ui64TimeStampStartSignal = l_rDynamicBoxContext.getInputChunkStartTime(SIGNAL_CHANNEL, i);
			m_ui64TimeStampEndSignal   = l_rDynamicBoxContext.getInputChunkEndTime(SIGNAL_CHANNEL, i);
			if((m_ui64TimeStimulationPosition >= m_ui64TimeStampStartSignal) && (m_ui64TimeStimulationPosition < m_ui64TimeStampEndSignal))
			{
				processSynchroSignal();
			}
			l_rDynamicBoxContext.markInputAsDeprecated(SIGNAL_CHANNEL, i);

			if(hasSynchroSignal())
				break;
		}
	}
	else
	{
		for(uint32 i=0; i < l_rDynamicBoxContext.getInputChunkCount(SIGNAL_CHANNEL); i++) //Stimulation de l'input 1
		{
			m_ui64TimeStampEndSignal = l_rDynamicBoxContext.getInputChunkEndTime(SIGNAL_CHANNEL, i);
			if(m_ui64TimeStampEndSignal < m_ui64TimeStampStartStimulation)
			{
				l_rDynamicBoxContext.markInputAsDeprecated(SIGNAL_CHANNEL, i);
			}
		}
	}
}

void CInputChannel::processSynchroSignal()
{
	m_ui32Status            |= SIGNAL_SYNCHRO_DETECTED;
	m_ui64NbChannels         = m_oMatrixBuffer[0]->getDimensionSize(0);
	m_ui64NbSamples          = m_oMatrixBuffer[0]->getDimensionSize(1);
	m_ui64FirstBlock         = uint64(double(m_ui64NbSamples*(m_ui64TimeStimulationPosition - m_ui64TimeStampStartSignal))/double(m_ui64TimeStampEndSignal - m_ui64TimeStampStartSignal));
	m_ui64SecondBlock        = m_ui64NbSamples - m_ui64FirstBlock;
	m_ui64TimeSignalPosition = m_ui64TimeStampEndSignal;

	copyData(false, m_ui64PtrMatrixIndex);

	m_pTBoxAlgorithm->getLogManager() << LogLevel_Info << "Cutting parameter for both part :  " << m_ui64FirstBlock<<"+"<<m_ui64SecondBlock<< "\n";
}

OpenViBE::uint32 CInputChannel::getNbOfStimulationBuffers()
{
	IBoxIO& l_rDynamicBoxContext  = m_pTBoxAlgorithm->getDynamicBoxContext();

	return l_rDynamicBoxContext.getInputChunkCount(STIMULATION_CHANNEL);
}

OpenViBE::uint32 CInputChannel::getNbOfSignalBuffers()
{
	IBoxIO& l_rDynamicBoxContext  = m_pTBoxAlgorithm->getDynamicBoxContext();

	return l_rDynamicBoxContext.getInputChunkCount(SIGNAL_CHANNEL);
}

OpenViBE::IStimulationSet* CInputChannel::getStimulation(OpenViBE::uint64& startTimestamp, OpenViBE::uint64& endTimestamp, const OpenViBE::uint32 stimulationIndex)
{
	IBoxIO& l_rDynamicBoxContext  = m_pTBoxAlgorithm->getDynamicBoxContext();

	ip_pMemoryBufferStimulation   = l_rDynamicBoxContext.getInputChunk(STIMULATION_CHANNEL, stimulationIndex);
	m_pStreamDecoderStimulation->process();
	m_oIStimulationSet            = op_pStimulationSetStimulation;

	startTimestamp                = m_bFirstStimulation ? l_rDynamicBoxContext.getInputChunkStartTime(STIMULATION_CHANNEL, stimulationIndex) : m_ui64TimeStimulationPosition;
	endTimestamp                  = l_rDynamicBoxContext.getInputChunkEndTime(STIMULATION_CHANNEL, stimulationIndex);
	m_bFirstStimulation           = true;

	l_rDynamicBoxContext.markInputAsDeprecated(STIMULATION_CHANNEL, stimulationIndex);

	return m_oIStimulationSet;
}


OpenViBE::CMatrix* CInputChannel::getSignal(OpenViBE::uint64& startTimestamp, OpenViBE::uint64& endTimestamp, const OpenViBE::uint32 signalIndex)
{
	IBoxIO& l_rDynamicBoxContext  = m_pTBoxAlgorithm->getDynamicBoxContext();
	ip_pMemoryBufferSignal        = l_rDynamicBoxContext.getInputChunk(SIGNAL_CHANNEL, signalIndex);
	m_pStreamDecoderSignal->process();
	if(!m_pStreamDecoderSignal->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		return 0;

	startTimestamp                = l_rDynamicBoxContext.getInputChunkStartTime(SIGNAL_CHANNEL, signalIndex);
	endTimestamp                  = l_rDynamicBoxContext.getInputChunkEndTime(SIGNAL_CHANNEL, signalIndex);

	copyData(true, m_ui64PtrMatrixIndex);
	copyData(false, m_ui64PtrMatrixIndex + 1);
	
	l_rDynamicBoxContext.markInputAsDeprecated(SIGNAL_CHANNEL, signalIndex);

	return getMatrix();
}

OpenViBE::CMatrix* CInputChannel::getMatrix()
{
	return m_oMatrixBuffer[m_ui64PtrMatrixIndex++ & 1];
}

OpenViBE::CMatrix* CInputChannel::getMatrixPtr()
{
	return m_oMatrixBuffer[m_ui64PtrMatrixIndex & 1];
}

void CInputChannel::copyData(const OpenViBE::boolean copyFirstBlock, OpenViBE::uint64 matrixIndex)
{
	OpenViBE::CMatrix*&    l_pMatrixBuffer = m_oMatrixBuffer[matrixIndex & 1];

	OpenViBE::float64*     l_pSrcData = op_pMatrixSignal->getBuffer() + (copyFirstBlock ? 0 : m_ui64FirstBlock);
	OpenViBE::float64*     l_pDstData = l_pMatrixBuffer->getBuffer()  + (copyFirstBlock ? m_ui64SecondBlock : 0);
	OpenViBE::uint64       l_ui64Size = (copyFirstBlock ? m_ui64FirstBlock : m_ui64SecondBlock)*sizeof(OpenViBE::float64);

	for(OpenViBE::uint64 i=0; i < m_ui64NbChannels; i++, l_pSrcData+=m_ui64NbSamples, l_pDstData+=m_ui64NbSamples)
	{
		memcpy(l_pDstData, l_pSrcData, size_t (l_ui64Size));
	}
}
