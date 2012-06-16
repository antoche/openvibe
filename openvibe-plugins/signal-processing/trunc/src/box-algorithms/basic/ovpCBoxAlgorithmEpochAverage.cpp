#include "ovpCBoxAlgorithmEpochAverage.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CEpochAverage::initialize(void)
{
	CIdentifier l_oInputTypeIdentifier;
	getStaticBoxContext().getInputType(0, l_oInputTypeIdentifier);
	if(l_oInputTypeIdentifier==OV_TypeId_StreamedMatrix)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	}
	else if(l_oInputTypeIdentifier==OV_TypeId_FeatureVector)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
	}
	else if(l_oInputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	}
	else if(l_oInputTypeIdentifier==OV_TypeId_Spectrum)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
	}
	else
	{
		return false;
	}
	m_pStreamDecoder->initialize();
	m_pStreamEncoder->initialize();

	m_pMatrixAverage=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_MatrixAverage));
	m_pMatrixAverage->initialize();

	if(l_oInputTypeIdentifier==OV_TypeId_StreamedMatrix)
	{
	}
	else if(l_oInputTypeIdentifier==OV_TypeId_FeatureVector)
	{
	}
	else if(l_oInputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	}
	else if(l_oInputTypeIdentifier==OV_TypeId_Spectrum)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
	}

	ip_ui64AveragingMethod.initialize(m_pMatrixAverage->getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod));
	ip_ui64MatrixCount.initialize(m_pMatrixAverage->getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount));

	ip_ui64AveragingMethod=(uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	ip_ui64MatrixCount=(uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_pMatrixAverage->getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pMatrixAverage->getOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix));

	if(ip_ui64MatrixCount<=0)
	{
		getLogManager() << LogLevel_Error << "You should provide a positive number of epochs better than " << ip_ui64MatrixCount << "\n";
		return false;
	}

	return true;
}

boolean CEpochAverage::uninitialize(void)
{
	ip_ui64AveragingMethod.uninitialize();
	ip_ui64MatrixCount.uninitialize();

	m_pMatrixAverage->uninitialize();
	m_pStreamEncoder->uninitialize();
	m_pStreamDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pMatrixAverage);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

boolean CEpochAverage::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CEpochAverage::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	IBox& l_rStaticBoxContext=getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > l_oInputMemoryBufferHandle(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
			l_oInputMemoryBufferHandle=l_rDynamicBoxContext.getInputChunk(i, j);
			l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(i);

			m_pStreamDecoder->process();
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_pMatrixAverage->process(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset);
				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				m_pMatrixAverage->process(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix);
				if(m_pMatrixAverage->isOutputTriggerActive(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed))
				{
					m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
				}
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
			}

			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
