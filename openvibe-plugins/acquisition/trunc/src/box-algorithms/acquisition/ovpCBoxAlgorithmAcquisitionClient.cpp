#include "ovpCBoxAlgorithmAcquisitionClient.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Acquisition;

uint64 CBoxAlgorithmAcquisitionClient::getClockFrequency(void)
{
	return 64LL<<32;
}

boolean CBoxAlgorithmAcquisitionClient::initialize(void)
{
	m_pAcquisitionStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_AcquisitionStreamDecoder));

	m_pAcquisitionStreamDecoder->initialize();

	ip_pAcquisitionMemoryBuffer.initialize(m_pAcquisitionStreamDecoder->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_ui64BufferDuration.initialize(m_pAcquisitionStreamDecoder->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration));
	op_pExperimentInformationMemoryBuffer.initialize(m_pAcquisitionStreamDecoder->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream));
	op_pSignalMemoryBuffer.initialize(m_pAcquisitionStreamDecoder->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream));
	op_pStimulationMemoryBuffer.initialize(m_pAcquisitionStreamDecoder->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream));
	op_pChannelLocalisationMemoryBuffer.initialize(m_pAcquisitionStreamDecoder->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream));

	m_ui64LastChunkStartTime=0;
	m_ui64LastChunkEndTime=0;
	m_pConnectionClient=NULL;

	return true;
}

boolean CBoxAlgorithmAcquisitionClient::uninitialize(void)
{
	m_pConnectionClient->close();
	m_pConnectionClient->release();
	m_pConnectionClient=NULL;

	op_pChannelLocalisationMemoryBuffer.uninitialize();
	op_pStimulationMemoryBuffer.uninitialize();
	op_pSignalMemoryBuffer.uninitialize();
	op_pExperimentInformationMemoryBuffer.uninitialize();
	op_ui64BufferDuration.uninitialize();
	ip_pAcquisitionMemoryBuffer.uninitialize();

	m_pAcquisitionStreamDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pAcquisitionStreamDecoder);

	m_pAcquisitionStreamDecoder=NULL;

	return true;
}

boolean CBoxAlgorithmAcquisitionClient::processClock(IMessageClock& rMessageClock)
{
	if(!m_pConnectionClient)
	{
		CString l_sSettingValue;

		getStaticBoxContext().getSettingValue(0, l_sSettingValue);
		CString l_sServerName=this->getConfigurationManager().expand(l_sSettingValue);

		getStaticBoxContext().getSettingValue(1, l_sSettingValue);
		uint32 l_ui32ServerPort=::atoi(l_sSettingValue.toASCIIString());

		m_pConnectionClient=Socket::createConnectionClient();
		m_pConnectionClient->connect(l_sServerName, l_ui32ServerPort);
		if(!m_pConnectionClient->isConnected())
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Could not connect to server " << l_sServerName << ":" << l_ui32ServerPort << "\n";
			return true;
		}
	}

	if(m_pConnectionClient && m_pConnectionClient->isReadyToReceive() /* && rMessageClock.getTime()>m_ui64LastChunkEndTime */)
	{
		getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	}

	return true;
}

boolean CBoxAlgorithmAcquisitionClient::process(void)
{
	if(!m_pConnectionClient || !m_pConnectionClient->isConnected())
	{
		return false;
	}

	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	op_pExperimentInformationMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
	op_pSignalMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(1);
	op_pStimulationMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(2);
	op_pChannelLocalisationMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(3);

	while(m_pConnectionClient->isReadyToReceive())
	{
		uint64 l_ui64MemoryBufferSize=0;
		if(!m_pConnectionClient->receiveBufferBlocking(&l_ui64MemoryBufferSize, sizeof(l_ui64MemoryBufferSize)))
		{
			getLogManager() << LogLevel_ImportantWarning << "Could not receive memory buffer size\n";
			return false;
		}
		if(!ip_pAcquisitionMemoryBuffer->setSize(l_ui64MemoryBufferSize, true))
		{
			getLogManager() << LogLevel_ImportantWarning << "Could not re allocate memory buffer with size " << l_ui64MemoryBufferSize << "\n";
			return false;
		}
		if(!m_pConnectionClient->receiveBufferBlocking(ip_pAcquisitionMemoryBuffer->getDirectPointer(), static_cast<uint32>(l_ui64MemoryBufferSize)))
		{
			getLogManager() << LogLevel_ImportantWarning << "Could not receive memory buffer content of size " << l_ui64MemoryBufferSize << "\n";
			return false;
		}

		m_pAcquisitionStreamDecoder->process();

		if(m_pAcquisitionStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedHeader)
		 ||m_pAcquisitionStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedBuffer)
		 ||m_pAcquisitionStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			l_rDynamicBoxContext.markOutputAsReadyToSend(1, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			l_rDynamicBoxContext.markOutputAsReadyToSend(2, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			l_rDynamicBoxContext.markOutputAsReadyToSend(3, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			m_ui64LastChunkStartTime=m_ui64LastChunkEndTime;
			m_ui64LastChunkEndTime+=op_ui64BufferDuration;
			float64 l_f64Latency=(int64(m_ui64LastChunkEndTime-this->getPlayerContext().getCurrentTime())/(1LL<<22))/1024.;
			this->getLogManager() << LogLevel_Debug << "Acquisition inner latency : " << l_f64Latency << "\n";
		}
	}

	return true;
}
