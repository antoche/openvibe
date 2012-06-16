#if 0
#include "ovpCSpatialFilterBoxAlgorithm.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace std;

boolean CSpatialFilterBoxAlgorithm::initialize(void)
{
	CIdentifier l_oInputTypeIdentifier;
	getStaticBoxContext().getInputType(0, l_oInputTypeIdentifier);
	if(l_oInputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	}
	else
	{
		return false;
	}
	m_pStreamDecoder->initialize();
	m_pStreamEncoder->initialize();

	// Apply filter to signal input buffer
	m_pApplySpatialFilter=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ApplySpatialFilter));
	m_pApplySpatialFilter->initialize();

	if(l_oInputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	}

	// compute filter coefs settings
	CString l_oParameters;
	CString l_oNbLine;
	CString l_oNbColumn;
	getStaticBoxContext().getSettingValue(0, l_oParameters);
	getStaticBoxContext().getSettingValue(1, l_oNbLine);
	getStaticBoxContext().getSettingValue(2, l_oNbColumn);

	uint64 l_ui64NbLine = atoi(l_oNbLine);
	uint64 l_ui64NbColumn = atoi(l_oNbColumn);

	istringstream l_oStreamString((const char*)l_oParameters);
	float64 l_f64CurrentValue;

	while(l_oStreamString>>l_f64CurrentValue)
	{
		m_oCoefficients.push_back(l_f64CurrentValue);
	}
	TParameterHandler<IMatrix*> ip_pFilterCoefficientsInputMatrix(m_pApplySpatialFilter->getInputParameter(OVP_Algorithm_ApplySpatialFilter_InputParameterId_FilterCoefficientsMatrix));
	ip_pFilterCoefficientsInputMatrix->setDimensionCount(2);
	ip_pFilterCoefficientsInputMatrix->setDimensionSize(0, (uint32)l_ui64NbLine);
	ip_pFilterCoefficientsInputMatrix->setDimensionSize(1, (uint32)l_ui64NbColumn);
	float64* l_pFilterCoefficientsInput = ip_pFilterCoefficientsInputMatrix->getBuffer();
	for (uint32 i = 0; i<m_oCoefficients.size(); i++)
	{
		l_pFilterCoefficientsInput[i] = m_oCoefficients[i];
	}

	// apply filter settings
	m_pApplySpatialFilter->getInputParameter(OVP_Algorithm_ApplySpatialFilter_InputParameterId_SignalMatrix)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pApplySpatialFilter->getOutputParameter(OVP_Algorithm_ApplySpatialFilter_OutputParameterId_FilteredSignalMatrix));

	return true;
}

boolean CSpatialFilterBoxAlgorithm::uninitialize(void)
{
	m_pApplySpatialFilter->uninitialize();
	m_pStreamEncoder->uninitialize();
	m_pStreamDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pApplySpatialFilter);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

boolean CSpatialFilterBoxAlgorithm::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSpatialFilterBoxAlgorithm::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	IBox& l_rStaticBoxContext=getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > l_oInputMemoryBufferHandle(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
			l_oInputMemoryBufferHandle=l_rDynamicBoxContext.getInputChunk(i, j);
			l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(i);

			m_pStreamDecoder->process();
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_pApplySpatialFilter->process(OVP_Algorithm_ApplySpatialFilter_InputTriggerId_Initialize);
				m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				m_pApplySpatialFilter->process(OVP_Algorithm_ApplySpatialFilter_InputTriggerId_ApplyFilter);
				m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
				m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
			}

			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
#endif
