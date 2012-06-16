#include "ovpCBoxAlgorithmSignalProcessingBoxAlgorithm.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

boolean CBoxAlgorithmSignalProcessingBoxAlgorithm::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	CString l_sActive;
	l_rStaticBoxContext.getSettingValue(0, l_sActive);
	m_bActive=this->getConfigurationManager().expandAsBoolean(l_sActive);

	m_pSignalDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pSignalDecoder->initialize();
	ip_pMemoryBufferToDecode.initialize(m_pSignalDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_ui64SamplingRate.initialize(m_pSignalDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	op_pDecodedMatrix.initialize(m_pSignalDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));

	m_pSignalProcessingAlgorithm=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalProcessingAlgorithm));
	m_pSignalProcessingAlgorithm->initialize();
	ip_pSignalProcessingAlgorithmMatrix.initialize(m_pSignalProcessingAlgorithm->getInputParameter(OVP_Algorithm_SignalProcessingAlgorithm_InputParameterId_Matrix));
	op_pSignalProcessingAlgorithmMatrix.initialize(m_pSignalProcessingAlgorithm->getOutputParameter(OVP_Algorithm_SignalProcessingAlgorithm_OutputParameterId_Matrix));

	m_pSignalEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pSignalEncoder->initialize();
	ip_ui64SamplingRate.initialize(m_pSignalEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	ip_pMatrixToEncode.initialize(m_pSignalEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	op_pEncodedMemoryBuffer.initialize(m_pSignalEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	ip_ui64SamplingRate.setReferenceTarget(op_ui64SamplingRate);
	ip_pSignalProcessingAlgorithmMatrix.setReferenceTarget(op_pDecodedMatrix);
	ip_pMatrixToEncode.setReferenceTarget(op_pSignalProcessingAlgorithmMatrix);

	return true;
}

boolean CBoxAlgorithmSignalProcessingBoxAlgorithm::uninitialize(void)
{
	ip_pMatrixToEncode.uninitialize();
	ip_ui64SamplingRate.uninitialize();
	m_pSignalEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pSignalEncoder);

	op_pSignalProcessingAlgorithmMatrix.uninitialize();
	ip_pSignalProcessingAlgorithmMatrix.uninitialize();
	m_pSignalProcessingAlgorithm->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pSignalProcessingAlgorithm);

	op_pDecodedMatrix.uninitialize();
	op_ui64SamplingRate.uninitialize();
	m_pSignalDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pSignalDecoder);

	return true;
}

boolean CBoxAlgorithmSignalProcessingBoxAlgorithm::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmSignalProcessingBoxAlgorithm::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBufferToDecode=l_rDynamicBoxContext.getInputChunk(0, i);
		op_pEncodedMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

		m_pSignalDecoder->process();

		if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pSignalProcessingAlgorithm->process(OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Initialize);
			m_pSignalEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);

			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}
		if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			m_pSignalProcessingAlgorithm->process(OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Process);

			if(m_pSignalProcessingAlgorithm->isOutputTriggerActive(OVP_Algorithm_SignalProcessingAlgorithm_OutputTriggerId_ProcessDone))
			{
				m_pSignalEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);

				l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
			}
		}
		if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pSignalEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);

			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
