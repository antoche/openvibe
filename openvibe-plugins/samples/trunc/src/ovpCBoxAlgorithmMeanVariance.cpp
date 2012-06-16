#include "ovpCBoxAlgorithmMeanVariance.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace std;

uint64 CBoxAlgorithmMeanVariance::getClockFrequency(void)
{
	return 0; // the box clock frequency
}

boolean CBoxAlgorithmMeanVariance::initialize(void)
{
	//the algorithms that decode and encode the signals
	//algorithm creation
	m_pDecoderInput1=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pEncoderOutput1=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pEncoderOutput2=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));

	//initialization
	m_pDecoderInput1->initialize();
	m_pEncoderOutput1->initialize();
	m_pEncoderOutput2->initialize();

	//m_pDecoderInput1->getOutputParameter(OVP_SignalDecoder_SamplingRate_OutParameterId)->setReferenceTarget(m_pEncoder->getInputParameter(OVP_SignalEncoder_SamplingRate_InParameterId));

	//connecting the decoding and encoding parameters
	m_oEBMLMemoryBufferHandleInput1.initialize(m_pDecoderInput1->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	m_oEBMLMemoryBufferHandleOutput1.initialize(m_pEncoderOutput1->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	m_oEBMLMemoryBufferHandleOutput2.initialize(m_pEncoderOutput2->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_oMatrixInput1Handle.initialize(m_pDecoderInput1->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	m_oMatrixOutput1Handle.initialize(m_pEncoderOutput1->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	m_oMatrixOutput2Handle.initialize(m_pEncoderOutput2->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));

	getStaticBoxContext().getSettingValue(0, m_sDSPConfigurationFilename);
	getStaticBoxContext().getSettingValue(1, m_sMinMaxConfigurationFilename);

	CString l_sSettingValue;
	getStaticBoxContext().getSettingValue(2, l_sSettingValue);
	m_ui64ActivityStartTime=(uint64)((1LL<<32)*::atof(l_sSettingValue.toASCIIString()));
	getStaticBoxContext().getSettingValue(3, l_sSettingValue);
	m_ui64ActivityEndTime=(uint64)((1LL<<32)*::atof(l_sSettingValue.toASCIIString()));
	getStaticBoxContext().getSettingValue(4, l_sSettingValue);
	m_ui32StandardDeviationFactor = ::atoi(l_sSettingValue);
	//end and start time
	m_ui64StartTime=0;
	m_ui64EndTime=0;
	m_bFinished=false;

	return true;
}

boolean CBoxAlgorithmMeanVariance::uninitialize(void)
{
	//uninitializing parameters and algorithms
	m_pDecoderInput1->uninitialize();
	m_pEncoderOutput1->uninitialize();
	m_pEncoderOutput2->uninitialize();
	m_oEBMLMemoryBufferHandleInput1.uninitialize();
	m_oEBMLMemoryBufferHandleOutput1.uninitialize();
	m_oEBMLMemoryBufferHandleOutput2.uninitialize();
	m_oMatrixInput1Handle.uninitialize();
	m_oMatrixOutput1Handle.uninitialize();
	m_oMatrixOutput2Handle.uninitialize();

	//releasing algorithms
	getAlgorithmManager().releaseAlgorithm(*m_pDecoderInput1);
	getAlgorithmManager().releaseAlgorithm(*m_pEncoderOutput1);
	getAlgorithmManager().releaseAlgorithm(*m_pEncoderOutput2);

	return true;
}
boolean CBoxAlgorithmMeanVariance::processEvent(IMessageEvent& rMessageEvent)
{
	return true;
}

boolean CBoxAlgorithmMeanVariance::processSignal(IMessageSignal& rMessageSignal)
{
	return true;
}

boolean CBoxAlgorithmMeanVariance::processClock(IMessageClock& rMessageClock)
{
	return true;
}

boolean CBoxAlgorithmMeanVariance::processInput(uint32 ui32InputIndex)
{
	//if input is arrived, processing it
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmMeanVariance::process(void)
{
	//IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//decoding input
	for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(0); j++)
	{
		m_oEBMLMemoryBufferHandleInput1=l_rDynamicBoxContext.getInputChunk(0, j);
		m_pDecoderInput1->process();
		//storing the start and end time of the chunk
		m_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(0, j);
		m_ui64EndTime = l_rDynamicBoxContext.getInputChunkEndTime(0, j);

		//deal with the header if needed (initializations)
		if(m_pDecoderInput1->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_bFinished=false;

			m_oMatrixOutput1Handle->setDimensionCount(2);
			m_oMatrixOutput2Handle->setDimensionCount(2);

			// getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Debug << "input dimension count: " << m_oMatrixInput1Handle->getDimensionCount() << "\n";

			m_ui32NbChannels = m_oMatrixInput1Handle->getDimensionSize(0);
			m_ui32NbSamplesPerBuffer = m_oMatrixInput1Handle->getDimensionSize(1);

			m_f64S1List.resize(m_ui32NbChannels);
			m_f64S2List.resize(m_ui32NbChannels);
			m_ui64N = 0;

			//for(uint32 i = 0; i < m_oMatrixInput1Handle->getDimensionCount(); i++)
			//{
			//getLogManager() << LogLevel_Debug << "input dimension size 0: " << m_oMatrixInput1Handle->getDimensionSize(0) << "\n";
			//getLogManager() << LogLevel_Debug << "input dimension size 1: " << m_oMatrixInput1Handle->getDimensionSize(1) << "\n";

			m_oMatrixOutput1Handle->setDimensionSize(0,m_ui32NbChannels);
			m_oMatrixOutput2Handle->setDimensionSize(0,m_ui32NbChannels);
			m_oMatrixOutput1Handle->setDimensionSize(1,1);
			m_oMatrixOutput2Handle->setDimensionSize(1,1);

			//}

			m_oEBMLMemoryBufferHandleOutput1=l_rDynamicBoxContext.getOutputChunk(0);
			m_oEBMLMemoryBufferHandleOutput2=l_rDynamicBoxContext.getOutputChunk(1);
			// m_pEncoderOutput1->process(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeHeader);
			// m_pEncoderOutput2->process(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeHeader);
			// l_rDynamicBoxContext.markOutputAsReadyToSend (0, m_ui64StartTime, m_ui64EndTime);
			// l_rDynamicBoxContext.markOutputAsReadyToSend (1, m_ui64StartTime, m_ui64EndTime);
		}

		//perform the mean and variance computations
		if(m_pDecoderInput1->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			if(m_ui64StartTime >= m_ui64ActivityStartTime)
			{
				float64* l_pBufferInput1 = m_oMatrixInput1Handle->getBuffer();
				float64* l_pBufferOuput1 = m_oMatrixOutput1Handle->getBuffer();
				float64* l_pBufferOuput2 = m_oMatrixOutput2Handle->getBuffer();

				if(m_ui64EndTime <= m_ui64ActivityEndTime)
				{
					// uint32 l_ui32SizeBuffer = m_oMatrixInput1Handle->getBufferElementCount();
					// getLogManager() << LogLevel_Debug << "sizeBuffer: " << l_ui32SizeBuffer << "\n";

					//updating S1, S2 and N with incoming data
					uint32 l_ui32Offset=0;
					for(uint32 i = 0; i < m_ui32NbChannels; i++)
					{
						for(uint32 j = 0; j < m_ui32NbSamplesPerBuffer; j++)
						{
							float64 l_f64Input = l_pBufferInput1[l_ui32Offset++];
							m_f64S1List[i] += l_f64Input;
							m_f64S2List[i] += l_f64Input * l_f64Input;
						}
					}

					m_ui64N += m_ui32NbSamplesPerBuffer;

					//computing new mean and new variance for each channel
					for(uint32 i = 0; i < m_ui32NbChannels; i++)
					{
						l_pBufferOuput1[i] = m_f64S1List[i] / (float64)m_ui64N; //mean
						l_pBufferOuput2[i] = (m_f64S2List[i] / (float64)m_ui64N) - ((m_f64S1List[i] * m_f64S1List[i]) / ((float64)m_ui64N * (float64)m_ui64N)); //variance
					}

					//encode the result on the output
					m_oEBMLMemoryBufferHandleOutput1=l_rDynamicBoxContext.getOutputChunk(0);
					m_oEBMLMemoryBufferHandleOutput2=l_rDynamicBoxContext.getOutputChunk(1);
					// m_pEncoderOutput1->process(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeBuffer);
					// m_pEncoderOutput2->process(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeBuffer);
					// l_rDynamicBoxContext.markOutputAsReadyToSend (0, m_ui64StartTime, m_ui64EndTime);
					// l_rDynamicBoxContext.markOutputAsReadyToSend (1, m_ui64StartTime, m_ui64EndTime);
				}
				else
				{
					if(!m_bFinished)
					{
						for(uint32 i = 0; i < m_ui32NbChannels; i++)
						{
							getLogManager() << LogLevel_Info << "Channel " << i << ": mean: " << l_pBufferOuput1[i] << ", standard deviation: " << ::sqrt(l_pBufferOuput2[i]) << "\n";
						}
						m_bFinished=true;

						FILE* l_pFile;

						l_pFile=fopen(m_sDSPConfigurationFilename.toASCIIString(), "wt");
						if(!l_pFile)
						{
							this->getLogManager() << LogLevel_Warning << "Unable to write dsp config [" << m_sDSPConfigurationFilename << "]\n";
						}
						else
						{
							fprintf(l_pFile, "<OpenViBE-SettingsOverride>\n");
							fprintf(l_pFile, "\t<SettingValue>x-%f</SettingValue>\n", l_pBufferOuput1[0]+m_ui32StandardDeviationFactor*::sqrt(l_pBufferOuput2[0]));
							fprintf(l_pFile, "</OpenViBE-SettingsOverride>\n");
							fclose(l_pFile);
						}

						l_pFile=fopen(m_sMinMaxConfigurationFilename.toASCIIString(), "wt");
						if(!l_pFile)
						{
							this->getLogManager() << LogLevel_Warning << "Unable to write minmax config [" << m_sMinMaxConfigurationFilename << "]\n";
						}
						else
						{
							fprintf(l_pFile, "<OpenViBE-SettingsOverride>\n");
							fprintf(l_pFile, "\t<SettingValue>Min/Max</SettingValue>\n");
							fprintf(l_pFile, "\t<SettingValue>%f</SettingValue>\n", l_pBufferOuput1[0]);
							fprintf(l_pFile, "\t<SettingValue>%f</SettingValue>\n", l_pBufferOuput1[0]+2*m_ui32StandardDeviationFactor*::sqrt(l_pBufferOuput2[0]));
							fprintf(l_pFile, "</OpenViBE-SettingsOverride>\n");
							fclose(l_pFile);
						}
					}
				}
			}
		}
		l_rDynamicBoxContext.markInputAsDeprecated(0, j);
	}

	// ...

	// l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	// l_rStaticBoxContext.getSettingCount();

	// l_rDynamicBoxContext.getInputChunkCount()
	// l_rDynamicBoxContext.getInputChunk(i, )
	// l_rDynamicBoxContext.getOutputChunk(i, )

	return true;
}
