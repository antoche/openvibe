#include "ovpCBoxAlgorithmSpectrumAverage.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CBoxAlgorithmSpectrumAverage::initialize(void)
{
	m_bZeroCare=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
	m_pStreamDecoder->initialize();

	m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix));
	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	return true;
}

boolean CBoxAlgorithmSpectrumAverage::uninitialize(void)
{
	ip_pMatrix.uninitialize();
	op_pMatrix.uninitialize();

	if(m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
		m_pStreamEncoder=NULL;
	}

	if(m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
		m_pStreamDecoder=NULL;
	}

	return true;
}

boolean CBoxAlgorithmSpectrumAverage::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmSpectrumAverage::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_pMatrix, *op_pMatrix);
			ip_pMatrix->setDimensionSize(1, 1);

			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			float64* l_pInputMatrix=ip_pMatrix->getBuffer();
			float64* l_pOutputMatrix=op_pMatrix->getBuffer();
			uint32 l_ui32ChannelCount=op_pMatrix->getDimensionSize(0);
			uint32 l_ui32BandCount=op_pMatrix->getDimensionSize(1);
			for(uint32 j=0; j<l_ui32ChannelCount; j++)
			{
				float64 l_f64Mean=0;
				uint32 l_ui32Count=0;
				for(uint32 k=0; k<l_ui32BandCount; k++)
				{
					l_f64Mean+=*l_pOutputMatrix;
					l_ui32Count+=(m_bZeroCare || *l_pOutputMatrix != 0)?1:0;
					l_pOutputMatrix++;
				}
				*l_pInputMatrix=(l_ui32Count==0?0:l_f64Mean/l_ui32Count);
				l_pInputMatrix++;
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
		}

		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
