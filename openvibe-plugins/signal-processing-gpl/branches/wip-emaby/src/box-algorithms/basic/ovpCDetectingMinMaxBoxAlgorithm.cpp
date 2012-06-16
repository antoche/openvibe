#include "ovpCDetectingMinMaxBoxAlgorithm.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace std;

boolean CDetectingMinMaxBoxAlgorithm::initialize(void)
{
	CIdentifier l_oInputTypeIdentifier;
	getStaticBoxContext().getInputType(0, l_oInputTypeIdentifier);
	if(l_oInputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	}
	else
	{
		return false;
	}
	m_pStreamDecoder->initialize();
	m_pStreamEncoder->initialize();

	// Detects MinMax of signal input buffer
	m_pDetectingMinMax=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_DetectingMinMax));
	m_pDetectingMinMax->initialize();

	// compute filter coefs settings
	CString l_oMinMax;
	CString l_oTimeWindowStart;
	CString l_oTimeWindowEnd;
	m_bMaxFlag = false;
	m_bMinFlag = false;

	getStaticBoxContext().getSettingValue(0, l_oMinMax);
	if(this->getTypeManager().getEnumerationEntryValueFromName(OVP_TypeId_MinMax, l_oMinMax)==OVP_TypeId_MinMax_Min)
	{
		m_bMinFlag = true;
	}
	if(this->getTypeManager().getEnumerationEntryValueFromName(OVP_TypeId_MinMax, l_oMinMax)==OVP_TypeId_MinMax_Max)
	{
		m_bMaxFlag = true;
	}

	getStaticBoxContext().getSettingValue(1, l_oTimeWindowStart);
	float64 l_f64TimeWindowStart=atof(l_oTimeWindowStart);
	getStaticBoxContext().getSettingValue(2, l_oTimeWindowEnd);
	float64 l_f64TimeWindowEnd=atoi(l_oTimeWindowEnd);

	// DetectingMinMax settings
	m_pDetectingMinMax->getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_SignalMatrix)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

	m_pDetectingMinMax->getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_SamplingFrequency)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	m_pDetectingMinMax->getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowStart)->setValue(&l_f64TimeWindowStart);
	m_pDetectingMinMax->getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowEnd)->setValue(&l_f64TimeWindowEnd);

	// encoder settings
	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pDetectingMinMax->getOutputParameter(OVP_Algorithm_DetectingMinMax_OutputParameterId_SignalMatrix));

	m_ui64LastStartTime=0;
	m_ui64LastEndTime=0;
	return true;
}

boolean CDetectingMinMaxBoxAlgorithm::uninitialize(void)
{
	m_pStreamEncoder->uninitialize();
	m_pStreamDecoder->uninitialize();
	m_pDetectingMinMax->uninitialize();
	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
	getAlgorithmManager().releaseAlgorithm(*m_pDetectingMinMax);

	return true;
}

boolean CDetectingMinMaxBoxAlgorithm::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CDetectingMinMaxBoxAlgorithm::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	IBox& l_rStaticBoxContext=getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > l_oInputMemoryBufferHandle(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
			l_oInputMemoryBufferHandle=l_rDynamicBoxContext.getInputChunk(i, j);
			l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(i);
			uint64 l_ui64EndTime=m_ui64LastStartTime+l_rDynamicBoxContext.getInputChunkEndTime(i, j)-l_rDynamicBoxContext.getInputChunkStartTime(i, j);

			m_pStreamDecoder->process();
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_pDetectingMinMax->process(OVP_Algorithm_DetectingMinMax_InputTriggerId_Initialize);
				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, m_ui64LastStartTime, l_ui64EndTime);
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				if (m_bMinFlag)
				{
					m_pDetectingMinMax->process(OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMin);
				}
				if (m_bMaxFlag)

				{
					m_pDetectingMinMax->process(OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMax);
				}

				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, m_ui64LastStartTime, l_ui64EndTime);
			}
			if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i, m_ui64LastStartTime, l_ui64EndTime);
			}

			m_ui64LastStartTime=l_rDynamicBoxContext.getInputChunkStartTime(i, j);
			m_ui64LastEndTime=l_rDynamicBoxContext.getInputChunkEndTime(i, j);
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
