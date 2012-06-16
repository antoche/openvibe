//include OpenViBE
#include "ovpCBoxAlgorithmQuadraticForm.h"

//include C++ STL
#include <iostream>
#include <sstream>
#include <string>

//atoi
#include <cstdlib>

using namespace std;

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

uint64 CBoxAlgorithmQuadraticForm::getClockFrequency(void)
{
	return 0; // the box clock frequency
}

boolean CBoxAlgorithmQuadraticForm::initialize(void)
{
	//the algorithms that decode and encode the signals
	m_pDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pDecoder->initialize();
	m_pEncoder->initialize();
	m_pDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate)->setReferenceTarget(m_pEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));

	//connecting the decoding and encoding the parameters
	m_oEBMLMemoryBufferHandleInput.initialize(m_pDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	m_oMatrixInputHandle.initialize(m_pDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
	m_oMatrixOutputHandle.initialize(m_pEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	m_oEBMLMemoryBufferHandleOutput.initialize(m_pEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	//end and start time
	m_ui64StartTime=0;
	m_ui64EndTime=0;

	//reading the quadratic operator (matrix) values

	//the number of rows/columns
	CString l_sRowsNumber;
	getStaticBoxContext().getSettingValue(1, l_sRowsNumber);
	uint32 l_ui32RowsNumber;
	l_ui32RowsNumber = ::atoi(l_sRowsNumber);

	//setting the size of the matrix
	m_oQuadraticOperator.setDimensionCount(2);
	m_oQuadraticOperator.setDimensionSize(0, l_ui32RowsNumber);
	m_oQuadraticOperator.setDimensionSize(1, l_ui32RowsNumber);

	//the coefficients
	CString l_sMatrixCoefficients;
	getStaticBoxContext().getSettingValue(0, l_sMatrixCoefficients);
	const char * l_sCMatrixCoefficients = l_sMatrixCoefficients.toASCIIString();
	float64 * l_pMatrixBuffer = m_oQuadraticOperator.getBuffer();

	istringstream l_oStreamString(l_sCMatrixCoefficients); //the stream for parsing the matrix coefficient
	float64 l_f64CurrentValue = 0.0; //the current coefficient being read

	for(uint32 i=0; i < l_ui32RowsNumber; i++)
	{
		for(uint32 j = 0; j < l_ui32RowsNumber; j++)
		{
			//actual parsing, checking and storing value if everything is OK
			if(!(l_oStreamString>>l_f64CurrentValue))
			{
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Error << "Error reading quadratic operator coefficients\n The coefficients or the number of coefficient must be wrong\n";
				return false;
			}
			else
			{
				l_pMatrixBuffer[i * l_ui32RowsNumber + j] = l_f64CurrentValue;
			}
		}
	}

	if(l_oStreamString>>l_f64CurrentValue)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "There may be more coefficients specified in the setting 'Matrix values' than the number of rows/columns can allow\n";
	}

	return true;
}

boolean CBoxAlgorithmQuadraticForm::uninitialize(void)
{
	//uninitializing algorithms and parameters handlers
	m_pDecoder->uninitialize();
	m_pEncoder->uninitialize();
	m_oEBMLMemoryBufferHandleInput.uninitialize();
	m_oEBMLMemoryBufferHandleOutput.uninitialize();
	m_oMatrixInputHandle.uninitialize();
	m_oMatrixOutputHandle.uninitialize();

	//releasing algorithms
	getAlgorithmManager().releaseAlgorithm(*m_pDecoder);
	getAlgorithmManager().releaseAlgorithm(*m_pEncoder);

	return true;
}

boolean CBoxAlgorithmQuadraticForm::processEvent(IMessageEvent& rMessageEvent)
{
	return true;
}

boolean CBoxAlgorithmQuadraticForm::processSignal(IMessageSignal& rMessageSignal)
{
	return true;
}

boolean CBoxAlgorithmQuadraticForm::processClock(IMessageClock& rMessageClock)
{
	return true;
}

boolean CBoxAlgorithmQuadraticForm::processInput(uint32 ui32InputIndex)
{
	//if input is arrived, processing it, i.e., computing the corresponding quadratic forms
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmQuadraticForm::process(void)
{
	//IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//prcessing the input buffers
	for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(0); j++)
	{
		//decoding the input signal
		m_oEBMLMemoryBufferHandleInput=l_rDynamicBoxContext.getInputChunk(0, j);
		m_pDecoder->process();
		//storing the start and end time of the chunk
		m_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(0, j);
		m_ui64EndTime = l_rDynamicBoxContext.getInputChunkEndTime(0, j);

		//deal with the header if needed (initializations)
		if(m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			//getting some input matrix properties
			m_ui32NbChannels = m_oMatrixInputHandle->getDimensionSize(0);
			m_ui32NbSamplesPerBuffer = m_oMatrixInputHandle->getDimensionSize(1);

			//checking that the number of channels is compatible with the quadratic operator size
			if(m_ui32NbChannels != m_oQuadraticOperator.getDimensionSize(0))
			{
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Error << "The number of input channels is not compatible with the number of rows/columns of the quadratic operator matrix. This number of rows/columns must be equal to the number of input channels\n";
				return false;
			}

			//setting the size of the output buffer
			m_oMatrixOutputHandle->setDimensionCount(2);
			m_oMatrixOutputHandle->setDimensionSize(0,1);
			m_oMatrixOutputHandle->setDimensionSize(1,m_ui32NbSamplesPerBuffer);

			m_oEBMLMemoryBufferHandleOutput=l_rDynamicBoxContext.getOutputChunk(0);
			//encoding the output
			m_pEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
			//sending the output
			l_rDynamicBoxContext.markOutputAsReadyToSend (0, m_ui64StartTime, m_ui64EndTime);
		}

		//applying the quadratic operator
		if(m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			//the A (quadratic operator) matrix coefficients
			float64 * l_pMatrixBuffer = m_oQuadraticOperator.getBuffer();

			//the input vectors m
			float64* l_pBufferInput = m_oMatrixInputHandle->getBuffer();

			//the output matrix
			float64* l_pBufferOutput = m_oMatrixOutputHandle->getBuffer();

			//applying the quadratic operator for each sample: o = m^T * A * m
			for(uint32 i=0; i < m_ui32NbSamplesPerBuffer; i++)
			{

				//performing m' = A * m (intermediate step 1)
				vector<float64> l_omPrime(m_ui32NbChannels);

				//initializing with zeros
				for(uint32 k = 0; k < l_omPrime.size(); k++)
				{
					l_omPrime[k] = 0.0;
				}

				//browsing the quadratic operator matrix (A) rows
				for(uint32 k = 0; k < m_oQuadraticOperator.getDimensionSize(0); k++)
				{
					//browsing the quadratic operator matrix (A) columns
					for(uint32 l = 0; l < m_oQuadraticOperator.getDimensionSize(1); l++)
					{
						l_omPrime[k] += l_pMatrixBuffer[k * m_oQuadraticOperator.getDimensionSize(0) + l] * l_pBufferInput[l * m_ui32NbChannels + i];
					}
				}

				//performing o = m^T * m' (intermediate step 2)
				float64 l_f64Output = 0.0;
				for(uint32 k=0; k < l_omPrime.size(); k++)
				{
					l_f64Output += l_pBufferInput[k * m_ui32NbChannels + i] * l_omPrime[k];
				}

				l_pBufferOutput[i] = l_f64Output;
			}

			l_rDynamicBoxContext.markInputAsDeprecated(0, j);
			//setting the output
			m_oEBMLMemoryBufferHandleOutput=l_rDynamicBoxContext.getOutputChunk(0);
			//encoding the output
			m_pEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
			//sending the output
			l_rDynamicBoxContext.markOutputAsReadyToSend (0, m_ui64StartTime, m_ui64EndTime);
		}

	}

	return true;
}
