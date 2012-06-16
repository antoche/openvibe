#include "ovpCBoxAlgorithmCrop.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CBoxAlgorithmCrop::initialize(void)
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

	m_pMatrix=new CMatrix();
	TParameterHandler < IMatrix* >(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)).setReferenceTarget(m_pMatrix);
	TParameterHandler < IMatrix* >(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix)).setReferenceTarget(m_pMatrix);

	CString l_sSettingValue;

	getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	m_ui64CropMethod=getTypeManager().getEnumerationEntryValueFromName(OVP_TypeId_CropMethod, l_sSettingValue);

	getStaticBoxContext().getSettingValue(1, l_sSettingValue);
	m_f64MinCropValue=::atof(l_sSettingValue);

	getStaticBoxContext().getSettingValue(2, l_sSettingValue);
	m_f64MaxCropValue=::atof(l_sSettingValue);

	return true;
}

boolean CBoxAlgorithmCrop::uninitialize(void)
{
	delete m_pMatrix;

	m_pStreamEncoder->uninitialize();
	m_pStreamDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

boolean CBoxAlgorithmCrop::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmCrop::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		TParameterHandler < const IMemoryBuffer* > l_oInputMemoryBufferHandle(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		l_oInputMemoryBufferHandle=l_rDynamicBoxContext.getInputChunk(0, i);
		l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(0);

		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			float64* l_pBuffer=m_pMatrix->getBuffer();
			for(uint32 j=0; j<m_pMatrix->getBufferElementCount(); j++, l_pBuffer++)
			{
				if(*l_pBuffer < m_f64MinCropValue && (m_ui64CropMethod==OVP_TypeId_CropMethod_Min || m_ui64CropMethod==OVP_TypeId_CropMethod_MinMax)) *l_pBuffer=m_f64MinCropValue;
				if(*l_pBuffer > m_f64MaxCropValue && (m_ui64CropMethod==OVP_TypeId_CropMethod_Max || m_ui64CropMethod==OVP_TypeId_CropMethod_MinMax)) *l_pBuffer=m_f64MaxCropValue;
			}
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
