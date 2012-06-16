#include "ovpCSignChangeDetector.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

boolean CSignChangeDetector::initialize(void)
{
	CString l_sSettingValue;

	// we read the settings:
	// The stimulations names:
	getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	m_ui64OnStimulationId=getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sSettingValue);
	getStaticBoxContext().getSettingValue(1, l_sSettingValue);
	m_ui64OffStimulationId=getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sSettingValue);

	m_ui64ChannelIndex=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	// The refractory periode

	m_f64Lastsample=0;
	m_bNextStimIsOn=true;

	m_pStreamedMatrixDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
	m_pStreamedMatrixDecoder->initialize();
	op_pMatrix.initialize(m_pStreamedMatrixDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

	m_pStimulationEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStimulationEncoder->initialize();
	ip_pStimulationSet.initialize(m_pStimulationEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));

	m_bError=false;
	return true;
}

boolean CSignChangeDetector::uninitialize(void)
{
	op_pMatrix.uninitialize();
	m_pStreamedMatrixDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamedMatrixDecoder);

	ip_pStimulationSet.uninitialize();
	m_pStimulationEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationEncoder);

	return true;
}

boolean CSignChangeDetector::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSignChangeDetector::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(m_pStimulationEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

	// We decode the stream matrix
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, i);
		uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, i);

		TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_pStreamedMatrixDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStreamedMatrixDecoder->process();

		// if  we received the header
		if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			//we analyse the header (meaning the input matrix size)
			if (op_pMatrix->getDimensionCount() != 2)
			{
				this->getLogManager() << LogLevel_ImportantWarning << "Streamed matrix must have exactly 2 dimensions\n";
				return false;
			}
			else
			{
				if (m_ui64ChannelIndex > op_pMatrix->getDimensionSize(0) - 1)
				{
					this->getLogManager() << LogLevel_Info << "Channel Index out of bounds. Incoming matrix has fewer channels than specified index.";
					return false;
				}

				m_ui64SamplesPerChannel = op_pMatrix->getDimensionSize(1);
			}

			// we send a header on the stimulation output:
			m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime,l_ui64EndTime );
		}


		// if we received a buffer
		if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			ip_pStimulationSet->setStimulationCount(0);
			float64* l_pData = op_pMatrix->getBuffer();
			// for each data sample of the buffer we look for sign change

			for (uint32 j=0;j< op_pMatrix->getDimensionSize(1);j++)
			{
				// if m_bNextStimIsOn==true we look for the signal getting positive.
				if ( m_bNextStimIsOn && ( ( (j==0) ? (m_f64Lastsample) : (l_pData[(m_ui64ChannelIndex * m_ui64SamplesPerChannel) + j-1]) ) <0) && (l_pData[(m_ui64ChannelIndex * m_ui64SamplesPerChannel) + j]>=0) )
				{
					uint64 l_ui64Time = l_ui64StartTime + (l_ui64EndTime-l_ui64StartTime)*j/m_ui64SamplesPerChannel;

					ip_pStimulationSet->appendStimulation(m_ui64OnStimulationId, l_ui64Time, 0);
					m_bNextStimIsOn=false;
				}

				// if m_bNextStimIsOn==false we look for the signal getting negative.
				if ( !m_bNextStimIsOn && ( ( (j==0) ? (m_f64Lastsample) : (l_pData[(m_ui64ChannelIndex * m_ui64SamplesPerChannel) + j-1]) ) >=0) && (l_pData[(m_ui64ChannelIndex * m_ui64SamplesPerChannel) + j]<0) )
				{
					uint64 l_ui64Time = l_ui64StartTime + (l_ui64EndTime-l_ui64StartTime)*j/m_ui64SamplesPerChannel;
					ip_pStimulationSet->appendStimulation(m_ui64OffStimulationId, l_ui64Time, 0);
					m_bNextStimIsOn=true;
				}
			}

			m_f64Lastsample=l_pData[(m_ui64ChannelIndex * m_ui64SamplesPerChannel) +m_ui64SamplesPerChannel-1]; // we save the last sample of the buffer to compare it to the first one of the next buffer
			m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
		}

		// if we received the End
		if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			// we send the End signal to the stimulation output:
			m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd);
		}

		// The stream matrix chunk i has been process
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
