#include "ovpCTemporalFilterBoxAlgorithm.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace std;

boolean CTemporalFilterBoxAlgorithm::initialize(void)
{
	CIdentifier l_oInputTypeIdentifier;
	getStaticBoxContext().getInputType(0, l_oInputTypeIdentifier);
	if(l_oInputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalDecoder));
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalEncoder));
	}
	else
	{
		return false;
	}
	m_pStreamDecoder->initialize();
	m_pStreamEncoder->initialize();

	ip_pMemoryBufferToDecode.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pEncodedMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Compute filter coeff algorithm
	m_pComputeTemporalFilterCoefficients=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficients));
	m_pComputeTemporalFilterCoefficients->initialize();

	// Apply filter to signal input buffer
	m_pApplyTemporalFilter=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ApplyTemporalFilter));
	m_pApplyTemporalFilter->initialize();

	if(l_oInputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	}

	// compute filter coefs settings
	CString l_oNameFilter;
	CString l_oKindFilter;
	CString l_oFilterOrder;
	CString l_oLowPassBandEdge;
	CString l_oHighPassBandEdge;
	CString l_oPassBandRipple;

	getStaticBoxContext().getSettingValue(0, l_oNameFilter);
	getStaticBoxContext().getSettingValue(1, l_oKindFilter);
	getStaticBoxContext().getSettingValue(2, l_oFilterOrder);
	getStaticBoxContext().getSettingValue(3, l_oLowPassBandEdge);
	getStaticBoxContext().getSettingValue(4, l_oHighPassBandEdge);
	getStaticBoxContext().getSettingValue(5, l_oPassBandRipple);

	TParameterHandler<uint64> ip_ui64NameFilter(m_pComputeTemporalFilterCoefficients->getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterMethod));
	ip_ui64NameFilter=this->getTypeManager().getEnumerationEntryValueFromName(OVP_TypeId_FilterMethod, l_oNameFilter);

	TParameterHandler<uint64> ip_ui64KindFilter(m_pComputeTemporalFilterCoefficients->getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterType));
	ip_ui64KindFilter=this->getTypeManager().getEnumerationEntryValueFromName(OVP_TypeId_FilterType, l_oKindFilter);

	TParameterHandler<uint64> ip_ui64FilterOrder(m_pComputeTemporalFilterCoefficients->getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterOrder));
	ip_ui64FilterOrder=atoi(l_oFilterOrder);

	TParameterHandler<float64> ip_f64LowCutFrequency(m_pComputeTemporalFilterCoefficients->getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_LowCutFrequency));
	ip_f64LowCutFrequency=atof(l_oLowPassBandEdge);

	TParameterHandler<float64> ip_f64HighCutFrequency(m_pComputeTemporalFilterCoefficients->getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_HighCutFrequency));
	ip_f64HighCutFrequency=atof(l_oHighPassBandEdge);

	TParameterHandler<float64> ip_f64PassBandRipple(m_pComputeTemporalFilterCoefficients->getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_BandPassRipple));
	ip_f64PassBandRipple=atof(l_oPassBandRipple);

	m_pComputeTemporalFilterCoefficients->getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_SamplingFrequency)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	// apply filter settings
	m_pApplyTemporalFilter->getInputParameter(OVP_Algorithm_ApplyTemporalFilter_InputParameterId_SignalMatrix)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	m_pApplyTemporalFilter->getInputParameter(OVP_Algorithm_ApplyTemporalFilter_InputParameterId_FilterCoefficientsMatrix)->setReferenceTarget(m_pComputeTemporalFilterCoefficients->getOutputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_OutputParameterId_Matrix));

	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pApplyTemporalFilter->getOutputParameter(OVP_Algorithm_ApplyTemporalFilter_OutputParameterId_FilteredSignalMatrix));
	return true;
}

boolean CTemporalFilterBoxAlgorithm::uninitialize(void)
{
	m_pApplyTemporalFilter->uninitialize();
	m_pComputeTemporalFilterCoefficients->uninitialize();
	m_pStreamEncoder->uninitialize();
	m_pStreamDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pApplyTemporalFilter);
	getAlgorithmManager().releaseAlgorithm(*m_pComputeTemporalFilterCoefficients);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

boolean CTemporalFilterBoxAlgorithm::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CTemporalFilterBoxAlgorithm::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	IBox& l_rStaticBoxContext=getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
//			TParameterHandler < const IMemoryBuffer* > l_oInputMemoryBufferHandle(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
//			TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
//			l_oInputMemoryBufferHandle=l_rDynamicBoxContext.getInputChunk(i, j);
//			l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(i);
			ip_pMemoryBufferToDecode=l_rDynamicBoxContext.getInputChunk(i, j);
			op_pEncodedMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(i);
			uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(i, j);
			uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(i, j);

			m_pStreamDecoder->process();
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_pComputeTemporalFilterCoefficients->process(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_Initialize);
				m_pComputeTemporalFilterCoefficients->process(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_ComputeCoefficients);
				m_pApplyTemporalFilter->process(OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_Initialize);
				m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_ui64StartTime, l_ui64EndTime);
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				if (m_ui64LastEndTime==l_ui64StartTime)
				{
					m_pApplyTemporalFilter->process(OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilterWithHistoric);
				}
				else
				{
					m_pApplyTemporalFilter->process(OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilter);
				}
				m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_ui64StartTime, l_ui64EndTime);
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
				m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_ui64StartTime, l_ui64EndTime);
			}

//			m_ui64LastStartTime=l_ui64StartTime;
			m_ui64LastEndTime=l_ui64EndTime;
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
