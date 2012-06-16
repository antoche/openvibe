#include "ovpCBoxAlgorithmSignalDecimation.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

#define CBoxAlgorithmSignalDecimation_Debug 0

boolean CBoxAlgorithmSignalDecimation::initialize(void)
{
	m_pStreamDecoder=NULL;
	m_pStreamEncoder=NULL;

	m_i64DecimationFactor=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	if(m_i64DecimationFactor<=1)
	{
		this->getLogManager() << LogLevel_Error << "Decimation factor should be stricly greater than 1 !\n";
		return false;
	}

	m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_ui64SamplingRate.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_ui64SamplingRate.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_ui32ChannelCount=0;
	m_ui32InputSampleIndex=0;
	m_ui32InputSampleCountPerSentBlock=0;
	m_ui32OutputSampleIndex=0;
	m_ui32OutputSampleCountPerSentBlock=0;

	m_ui64TotalSampleCount=0;
	m_ui64LastStartTime=0;

	return true;
}

boolean CBoxAlgorithmSignalDecimation::uninitialize(void)
{
	op_pMemoryBuffer.uninitialize();
	ip_pMatrix.uninitialize();
	ip_ui64SamplingRate.uninitialize();

	if(m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
		m_pStreamEncoder=NULL;
	}

	op_ui64SamplingRate.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	if(m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
		m_pStreamDecoder=NULL;
	}

	return true;
}

boolean CBoxAlgorithmSignalDecimation::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmSignalDecimation::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i, j, k;

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

		uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, i);
		uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, i);

		if(l_ui64StartTime!=m_ui64LastEndTime)
		{
			m_ui64StartTimeBase=l_ui64StartTime;
			m_ui32InputSampleIndex=0;
			m_ui32OutputSampleIndex=0;
			m_ui64TotalSampleCount=0;
		}

		m_ui64LastStartTime=l_ui64StartTime;
		m_ui64LastEndTime=l_ui64EndTime;

		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_ui32InputSampleIndex=0;
			m_ui32InputSampleCountPerSentBlock=op_pMatrix->getDimensionSize(1);
			m_ui64InputSamplingFrequency=op_ui64SamplingRate;

			if(m_ui64InputSamplingFrequency%m_i64DecimationFactor != 0)
			{
				this->getLogManager() << LogLevel_Error << "Input signal sampling frequency " << m_ui64InputSamplingFrequency << " is not a multiple of the decimation factor " << m_i64DecimationFactor << "\n";
				return false;
			}

			m_ui32OutputSampleIndex=0;
			m_ui32OutputSampleCountPerSentBlock=(m_ui32InputSampleCountPerSentBlock/m_i64DecimationFactor);
			m_ui32OutputSampleCountPerSentBlock=(m_ui32OutputSampleCountPerSentBlock?m_ui32OutputSampleCountPerSentBlock:1);
			m_ui64OutputSamplingFrequency=op_ui64SamplingRate/m_i64DecimationFactor;

			m_ui32ChannelCount=op_pMatrix->getDimensionSize(0);
			m_ui64TotalSampleCount=0;

			OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_pMatrix, *op_pMatrix);
			ip_pMatrix->setDimensionSize(1, m_ui32OutputSampleCountPerSentBlock);
			ip_ui64SamplingRate=m_ui64OutputSamplingFrequency;
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
			OpenViBEToolkit::Tools::Matrix::clearContent(*ip_pMatrix);

			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64StartTime); // $$$ supposes we have one node per chunk
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			float64* l_pInputBuffer=op_pMatrix->getBuffer();
			float64* l_pOutputBuffer=ip_pMatrix->getBuffer()+m_ui32OutputSampleIndex;

			for(j=0; j<m_ui32InputSampleCountPerSentBlock; j++)
			{
				float64* l_pInputBufferTmp=l_pInputBuffer;
				float64* l_pOutputBufferTmp=l_pOutputBuffer;
				for(k=0; k<m_ui32ChannelCount; k++)
				{
					*l_pOutputBufferTmp+=*l_pInputBufferTmp;
					l_pOutputBufferTmp+=m_ui32OutputSampleCountPerSentBlock;
					l_pInputBufferTmp+=m_ui32InputSampleCountPerSentBlock;
				}

#if CBoxAlgorithmSignalDecimation_Debug
this->getLogManager() << LogLevel_Info << "Processed input sample\n";
#endif

				m_ui32InputSampleIndex++;
				if(m_ui32InputSampleIndex==m_i64DecimationFactor)
				{
					m_ui32InputSampleIndex=0;
					float64* l_pOutputBufferTmp=l_pOutputBuffer;
					for(k=0; k<m_ui32ChannelCount; k++)
					{
						*l_pOutputBufferTmp/=m_i64DecimationFactor;
						l_pOutputBufferTmp+=m_ui32OutputSampleCountPerSentBlock;
					}

#if CBoxAlgorithmSignalDecimation_Debug
this->getLogManager() << LogLevel_Info << "Fed output sample " << m_ui32OutputSampleIndex << "\n";
#endif

					l_pOutputBuffer++;
					m_ui32OutputSampleIndex++;
					if(m_ui32OutputSampleIndex==m_ui32OutputSampleCountPerSentBlock)
					{

#if CBoxAlgorithmSignalDecimation_Debug
this->getLogManager() << LogLevel_Info << "Built buffer chunk\n";
#endif

						l_pOutputBuffer=ip_pMatrix->getBuffer();
						m_ui32OutputSampleIndex=0;
						m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
						l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64StartTimeBase+(m_ui64TotalSampleCount<<32)/m_ui64OutputSamplingFrequency, m_ui64StartTimeBase+((m_ui64TotalSampleCount+m_ui32OutputSampleCountPerSentBlock)<<32)/m_ui64OutputSamplingFrequency);
						m_ui64TotalSampleCount+=m_ui32OutputSampleCountPerSentBlock;

						OpenViBEToolkit::Tools::Matrix::clearContent(*ip_pMatrix);
					}
				}

				l_pInputBuffer++;
			}

#if CBoxAlgorithmSignalDecimation_Debug
this->getLogManager() << LogLevel_Info << "Finished\n";
#endif

		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64StartTime); // $$$ supposes we have one node per chunk
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}
	return true;
}
