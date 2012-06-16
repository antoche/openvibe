#include "ovpCBoxAlgorithmUnivariateStatistics.h"
#include "ovpCAlgorithmUnivariateStatistics.h"
#include <cstdlib>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CBoxUnivariateStatistic::initialize(void)
{
	//initialise en/decoder function of the input type
	this->getStaticBoxContext().getInputType(0, m_oInputTypeIdentifier);

#if 0 // this is not needed as you know you always habe signal
	if(m_oInputTypeIdentifier==OV_TypeId_StreamedMatrix)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		m_pStreamEncoderMean=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_pStreamEncoderVariance=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_pStreamEncoderRange=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_pStreamEncoderMedian=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_pStreamEncoderIQR=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_pStreamEncoderPercentile=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	}
	else if(m_oInputTypeIdentifier==OV_TypeId_FeatureVector)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
		m_pStreamEncoderMean=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_pStreamEncoderVariance=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_pStreamEncoderRange=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_pStreamEncoderMedian=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_pStreamEncoderIQR=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_pStreamEncoderPercentile=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
	}
	else if(m_oInputTypeIdentifier==OV_TypeId_Signal)
	{
#endif
	m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pStreamEncoderMean=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoderVariance=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoderRange=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoderMedian=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoderIQR=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoderPercentile=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
#if 0 // this is not needed as you know you always habe signal
	}
	else if(m_oInputTypeIdentifier==OV_TypeId_Spectrum)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
		m_pStreamEncoderMean=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_pStreamEncoderVariance=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_pStreamEncoderRange=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_pStreamEncoderMedian=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_pStreamEncoderIQR=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_pStreamEncoderPercentile=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
	}
	else
	{
		this->getLogManager() << LogLevel_Debug << "Input type is not planned : no matrix base. This box can't work, so it is disabled\n";
		return false;
	}
#endif

	m_pStreamDecoder->initialize();
	m_pStreamEncoderMean->initialize();
	m_pStreamEncoderVariance->initialize();
	m_pStreamEncoderRange->initialize();
	m_pStreamEncoderMedian->initialize();
	m_pStreamEncoderIQR->initialize();
	m_pStreamEncoderPercentile->initialize();

	//initialize the real algorithm this box encapsulate
	m_pMatrixStatistic=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_AlgoUnivariateStatistic));
	m_pMatrixStatistic->initialize();

	//initialize all handlers
	m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_Matrix)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
	m_pStreamEncoderMean->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pMatrixStatistic->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Mean));
	m_pStreamEncoderVariance->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pMatrixStatistic->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Var));
	m_pStreamEncoderRange->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pMatrixStatistic->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Range));
	m_pStreamEncoderMedian->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pMatrixStatistic->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Med));
	m_pStreamEncoderIQR->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pMatrixStatistic->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_IQR));
	m_pStreamEncoderPercentile->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pMatrixStatistic->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Percent));

#if 0 // this is not needed as you know you always habe signal

	/// specific connection for what is different of matrix base
	if(m_oInputTypeIdentifier==OV_TypeId_Signal)
	{
#endif
	op_ui64SamplingRate.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	m_pStreamEncoderMean->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	m_pStreamEncoderVariance->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	m_pStreamEncoderRange->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	m_pStreamEncoderMedian->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	m_pStreamEncoderIQR->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	m_pStreamEncoderPercentile->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
#if 0 // this is not needed as you know you always habe signal
	}
	else if(m_oInputTypeIdentifier==OV_TypeId_Spectrum)
	{
		m_pStreamEncoderMean->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
		m_pStreamEncoderVariance->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
		m_pStreamEncoderRange->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
		m_pStreamEncoderMedian->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
		m_pStreamEncoderIQR->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
		m_pStreamEncoderPercentile->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
	}
#endif

	op_fCompression.initialize(m_pMatrixStatistic->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Compression));

	ip_bStatisticMeanActive.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_MeanActive));
	ip_bStatisticVarianceActive.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_VarActive));
	ip_bStatisticRangeActive.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_RangeActive));
	ip_bStatisticMedianActive.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_MedActive));
	ip_bStatisticIQRActive.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_IQRActive));
	ip_bStatisticPercentileActive.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentActive));

	//get dis/enabled output wanted
#if 0 // this can be replaced by later auto-cast code
	CString l_sSettings;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sSettings);
	ip_bStatisticMeanActive=(l_sSettings == CString("true")? true : false);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1, l_sSettings);
	ip_bStatisticVarianceActive =(l_sSettings == CString("true")? true : false);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(2, l_sSettings);
	ip_bStatisticRangeActive =(l_sSettings == CString("true")? true : false);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(3, l_sSettings);
	ip_bStatisticMedianActive =(l_sSettings == CString("true")? true : false);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(4, l_sSettings);
	ip_bStatisticIQRActive =(l_sSettings == CString("true")? true : false);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(5, l_sSettings);
	ip_bStatisticPercentileActive =(l_sSettings == CString("true")? true : false);
#else
	ip_bStatisticMeanActive      =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	ip_bStatisticVarianceActive  =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	ip_bStatisticRangeActive     =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	ip_bStatisticMedianActive    =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	ip_bStatisticIQRActive       =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
	ip_bStatisticPercentileActive=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);
#endif

	//the percentile value
#if 0 // this can be replaced by later auto-cast code
	getStaticBoxContext().getSettingValue(6, l_sSettings);
	ip_ui64StatisticParameterValue.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentValue));
	ip_ui64StatisticParameterValue=uint32(::atoi(l_sSettings));
#else
	ip_ui64StatisticParameterValue.initialize(m_pMatrixStatistic->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentValue));
	ip_ui64StatisticParameterValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 6);
#endif

	return true;
}

boolean CBoxUnivariateStatistic::uninitialize(void)
{
#if 0 
	if(m_oInputTypeIdentifier==OV_TypeId_Signal)
	{
#endif
		op_ui64SamplingRate.uninitialize();
#if 0 
	}
#endif

	ip_ui64StatisticParameterValue.uninitialize();

	m_pMatrixStatistic->uninitialize();
	m_pStreamEncoderMean->uninitialize();
	m_pStreamEncoderVariance->uninitialize();
	m_pStreamEncoderRange->uninitialize();
	m_pStreamEncoderMedian->uninitialize();
	m_pStreamEncoderIQR->uninitialize();
	m_pStreamEncoderPercentile->uninitialize();
	m_pStreamDecoder->uninitialize();

	this->getAlgorithmManager().releaseAlgorithm(*m_pMatrixStatistic);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderMean);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderVariance);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderRange);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderMedian);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderIQR);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoderPercentile);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

boolean CBoxUnivariateStatistic::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxUnivariateStatistic::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	// IBox& l_rStaticBoxContext=getStaticBoxContext();

	//for each input, calculate statistics and return the value
	for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(0); j++)
	{
		TParameterHandler < const IMemoryBuffer* > l_oInputMemoryBufferHandle(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle_Mean(m_pStreamEncoderMean->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle_Var(m_pStreamEncoderVariance->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle_Range(m_pStreamEncoderRange->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle_Median(m_pStreamEncoderMedian->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle_IQR(m_pStreamEncoderIQR->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle_Percent(m_pStreamEncoderPercentile->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		l_oInputMemoryBufferHandle=l_rDynamicBoxContext.getInputChunk(0, j);
		l_oOutputMemoryBufferHandle_Mean=l_rDynamicBoxContext.getOutputChunk(0);
		l_oOutputMemoryBufferHandle_Var=l_rDynamicBoxContext.getOutputChunk(1);
		l_oOutputMemoryBufferHandle_Range=l_rDynamicBoxContext.getOutputChunk(2);
		l_oOutputMemoryBufferHandle_Median=l_rDynamicBoxContext.getOutputChunk(3);
		l_oOutputMemoryBufferHandle_IQR=l_rDynamicBoxContext.getOutputChunk(4);
		l_oOutputMemoryBufferHandle_Percent=l_rDynamicBoxContext.getOutputChunk(5);

		m_pStreamDecoder->process();

		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pMatrixStatistic->process(OVP_Algorithm_UnivariateStatistic_InputTriggerId_Initialize);

#if 0 // this is not needed as you know you always habe signal
			if(m_oInputTypeIdentifier==OV_TypeId_FeatureVector)
			{
			}
			if(m_oInputTypeIdentifier==OV_TypeId_Signal)
			{
#endif
			this->getLogManager() << LogLevel_Debug << "DownSampling information : " << op_ui64SamplingRate << "*" << op_fCompression << "=>" << op_ui64SamplingRate*op_fCompression << "\n";
			op_ui64SamplingRate=op_ui64SamplingRate*op_fCompression;
			if(op_ui64SamplingRate==0)
			{
				this->getLogManager() << LogLevel_Warning <<"Output sampling Rate is null, it could produce problem in next boxes \n";
			}
#if 0 // this is not needed as you know you always habe signal
			}
			else if(m_oInputTypeIdentifier==OV_TypeId_Spectrum)
			{
			}
#endif

			if(ip_bStatisticMeanActive)
			{
				m_pStreamEncoderMean->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticVarianceActive)
			{
				m_pStreamEncoderVariance->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticRangeActive)
			{
				m_pStreamEncoderRange->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(2, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticMedianActive)
			{
				m_pStreamEncoderMedian->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(3, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticIQRActive)
			{
				m_pStreamEncoderIQR->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(4, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticPercentileActive)
			{
				m_pStreamEncoderPercentile->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(5, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
		}//end header
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			m_pMatrixStatistic->process(OVP_Algorithm_UnivariateStatistic_InputTriggerId_Process);
			if(m_pMatrixStatistic->isOutputTriggerActive(OVP_Algorithm_UnivariateStatistic_OutputTriggerId_ProcessDone))
			{
#if 0
				if(m_oInputTypeIdentifier==OV_TypeId_FeatureVector)
				{
				}
				if(m_oInputTypeIdentifier==OV_TypeId_Signal)
				{
				}
				else if(m_oInputTypeIdentifier==OV_TypeId_Spectrum)
				{
				}
#endif
				if(ip_bStatisticMeanActive)
				{
					m_pStreamEncoderMean->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
				}
				if(ip_bStatisticVarianceActive)
				{
					m_pStreamEncoderVariance->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
				}
				if(ip_bStatisticRangeActive)
				{
					m_pStreamEncoderRange->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(2, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
				}
				if(ip_bStatisticMedianActive)
				{
					m_pStreamEncoderMedian->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(3, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
				}
				if(ip_bStatisticIQRActive)
				{
					m_pStreamEncoderIQR->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(4, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
				}
				if(ip_bStatisticPercentileActive)
				{
					m_pStreamEncoderPercentile->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(5, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
				}
			}
			else
			{
				this->getLogManager() << LogLevel_Debug << "Process not activated\n";
			}
		}//end buffer
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
#if 0
			if(m_oInputTypeIdentifier==OV_TypeId_FeatureVector)
			{
			}
			if(m_oInputTypeIdentifier==OV_TypeId_Signal)
			{
			}
			else if(m_oInputTypeIdentifier==OV_TypeId_Spectrum)
			{
			}
#endif
			if(ip_bStatisticMeanActive)
			{
				m_pStreamEncoderMean->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticVarianceActive)
			{
				m_pStreamEncoderVariance->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticRangeActive)
			{
				m_pStreamEncoderRange->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(2, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticMedianActive)
			{
				m_pStreamEncoderMedian->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(3, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticIQRActive)
			{
				m_pStreamEncoderIQR->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(4, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
			if(ip_bStatisticPercentileActive)
			{
				m_pStreamEncoderPercentile->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(5, l_rDynamicBoxContext.getInputChunkStartTime(0, j), l_rDynamicBoxContext.getInputChunkEndTime(0, j));
			}
		}//end ender

		l_rDynamicBoxContext.markInputAsDeprecated(0, j);
	}
	return true;
}
