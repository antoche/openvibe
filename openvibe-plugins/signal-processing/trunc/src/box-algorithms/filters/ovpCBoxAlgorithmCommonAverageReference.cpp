#include "ovpCBoxAlgorithmCommonAverageReference.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CBoxAlgorithmCommonAverageReference::initialize(void)
{
	// CString   l_sSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	// uint64 l_ui64SettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	// float64 l_f64SettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	// ...

	m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_ui64SamplingRate.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	ip_ui64SamplingRate.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	op_pMatrix=&m_oMatrix;
	ip_pMatrix=&m_oMatrix;
	ip_ui64SamplingRate.setReferenceTarget(op_ui64SamplingRate);

	return true;
}

boolean CBoxAlgorithmCommonAverageReference::uninitialize(void)
{
	op_pMemoryBuffer.uninitialize();
	ip_ui64SamplingRate.uninitialize();
	ip_pMatrix.uninitialize();
	m_pStreamEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);

	op_ui64SamplingRate.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();
	m_pStreamDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

boolean CBoxAlgorithmCommonAverageReference::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmCommonAverageReference::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			uint32 i, j;
			uint32 l_ui32ChannelCount=m_oMatrix.getDimensionSize(0);
			uint32 l_ui32SampleCount=m_oMatrix.getDimensionSize(1);
			for(i=0; i<l_ui32SampleCount; i++)
			{
				float64* l_pBufferBase=m_oMatrix.getBuffer()+i;
				float64 l_f64Sum=0;
				float64 l_f64Mean=0;
				for(j=l_ui32ChannelCount; j!=0; j--)
				{
					l_f64Sum+=*l_pBufferBase;
					l_pBufferBase+=l_ui32SampleCount;
				}
				l_f64Mean=l_f64Sum/l_ui32ChannelCount;
				for(j=l_ui32ChannelCount; j!=0; j--)
				{
					l_pBufferBase-=l_ui32SampleCount;
					*l_pBufferBase-=l_f64Mean;
				}
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
