//#define __OpenViBEPlugins_BoxAlgorithm_IFFTbox_CPP__
// to get ifft:
#include <itpp/itsignal.h>
#include "ovpCBoxAlgorithmIFFTbox.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingBasic;


boolean CBoxAlgorithmIFFTbox::initialize(void)
{
	// Spectrum stream real part decoder
	m_oAlgo0_SpectrumDecoder[0].initialize(*this);
	// Spectrum stream imaginary part decoder
	m_oAlgo0_SpectrumDecoder[1].initialize(*this);
	// Signal stream encoder
	m_oAlgo1_SignalEncoder.initialize(*this);
	
	
	// If you need to, you can manually set the reference targets to link the codecs input and output. To do so, you can use :
	//m_oEncoder.getInputX().setReferenceTarget(m_oDecoder.getOutputX())
	// Where 'X' depends on the codec type. Please refer to the Codec Toolkit Reference Page
	// (http://openvibe.inria.fr/documentation/unstable/Doc_Tutorial_Developer_SignalProcessing_CodecToolkit_Ref.html) for a complete list.
	
	// If you need to retrieve setting values, use the FSettingValueAutoCast function.
	// For example :
	// - CString setting at index 0 in the setting list :
	// CString l_sSettingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	// - unsigned int64 setting at index 1 in the setting list :
	// uint64 l_ui64SettingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	// - float64 setting at index 2 in the setting list :
	// float64 l_f64SettingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	// ...

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmIFFTbox::uninitialize(void)
{

	m_oAlgo0_SpectrumDecoder[0].uninitialize();
	m_oAlgo0_SpectrumDecoder[1].uninitialize();
	m_oAlgo1_SignalEncoder.uninitialize();
	
	return true;
}
/*******************************************************************************/


//boolean CBoxAlgorithmIFFTbox::processClock(IMessageClock& rMessageClock)
//{
//	// some pre-processing code if needed...

//	// ready to process !
//	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

//	return true;
//}
/*******************************************************************************/


//uint64 CBoxAlgorithmIFFTbox::getClockFrequency(void)
//{
//	// Note that the time is coded on a 64 bits unsigned integer, fixed decimal point (32:32)
//	return 1LL<<32LL; // the box clock frequency
//}
/*******************************************************************************/


boolean CBoxAlgorithmIFFTbox::processInput(uint32 ui32InputIndex)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IDynamicBoxContext& l_rDynamicBoxContext=this->getDynamicBoxContext();

	if(l_rDynamicBoxContext.getInputChunkCount(0) == 0)
	{
		return true;
	}
	uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, 0);
	uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, 0);
	for(uint32 i=1; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		if(l_rDynamicBoxContext.getInputChunkCount(i)==0)
		{
			return true;
		}

		boolean l_bValidDates=true;
		if(l_ui64StartTime!=l_rDynamicBoxContext.getInputChunkStartTime(i, 0)) { l_bValidDates=false; }
		if(l_ui64EndTime!=l_rDynamicBoxContext.getInputChunkEndTime(i, 0)) { l_bValidDates=false; }
		if(!l_bValidDates)
		{
			this->getLogManager() << LogLevel_Warning << "Chunk dates mismatch, check stream structure or parameters\n";
			return l_bValidDates;
		}
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmIFFTbox::process(void)
{
	
	// the static box context describes the box inputs, outputs, settings structures
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// the dynamic box context describes the current state of the box inputs and outputs (i.e. the chunks)
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	
	uint32 i;
	uint32 l_ui32HeaderCount=0;
	uint32 l_ui32BufferCount=0;
	uint32 l_ui32EndCount=0;
	
  //uint32 numOutputSamples=0;
  //uint32 numInputSamples=0;
  //float64* bufferInput[]={NULL,NULL};
  //IMatrix* l_pMatrix[]={NULL,NULL};
  uint32 outputNumber=0;

	for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_oAlgo0_SpectrumDecoder[i].decode(i,0);
		if(m_oAlgo0_SpectrumDecoder[i].isHeaderReceived())
		{
			//detect if header of other input is already received 
			if(0==l_ui32HeaderCount)
			{
				// Header received. This happens only once when pressing "play". For example with a StreamedMatrix input, you now know the dimension count, sizes, and labels of the matrix
				// ... maybe do some process ...
				IMatrix* l_pMatrixMinMaxFrequencyBands = m_oAlgo0_SpectrumDecoder[i].getOutputMinMaxFrequencyBands();
				m_samplesNumber=l_pMatrixMinMaxFrequencyBands->getDimensionSize(1);
				m_channelsNumber=m_oAlgo0_SpectrumDecoder[i].getOutputMatrix()->getDimensionSize(0);
			}
			else
			{
				// test headers coherence
				IMatrix* l_pMatrixMinMaxFrequencyBands = m_oAlgo0_SpectrumDecoder[i].getOutputMinMaxFrequencyBands();
				if(m_samplesNumber!=l_pMatrixMinMaxFrequencyBands->getDimensionSize(1))
				{
					// problem!
					this->getLogManager() << LogLevel_Warning << "Samples numbers mismatch, check stream structure or parameters\n";
					return false;
				}
				if(m_channelsNumber!=m_oAlgo0_SpectrumDecoder[i].getOutputMatrix()->getDimensionSize(0))
				{
					// problem!
					this->getLogManager() << LogLevel_Warning << "channels numbers mismatch, check stream structure or parameters\n";
					return false;
				}
			}
			l_ui32HeaderCount++;
		}
		if(m_oAlgo0_SpectrumDecoder[i].isBufferReceived()) l_ui32BufferCount++;
		if(m_oAlgo0_SpectrumDecoder[i].isEndReceived()) l_ui32EndCount++;
	}
	
	if((l_ui32HeaderCount && l_ui32HeaderCount!=l_rStaticBoxContext.getInputCount())
	|| (l_ui32BufferCount && l_ui32BufferCount!=l_rStaticBoxContext.getInputCount())
	|| (l_ui32EndCount && l_ui32EndCount!=l_rStaticBoxContext.getInputCount()))
	{
		this->getLogManager() << LogLevel_Warning << "Stream structure mismatch\n";
		return false;
	}
	
	if(l_ui32HeaderCount)
	{
		m_frequencyBuffer.resize(m_channelsNumber);
		m_signalBuffer.resize(m_channelsNumber);
		for(uint32 channel=0; channel< m_channelsNumber; channel++)
		{
			m_signalBuffer[channel].set_size((m_samplesNumber)*2);
			m_frequencyBuffer[channel].set_size((m_samplesNumber)*2);
		}
		m_oAlgo1_SignalEncoder.getInputSamplingRate()=(m_samplesNumber)*2;
		m_oAlgo1_SignalEncoder.getInputMatrix()->setDimensionCount(2);
		m_oAlgo1_SignalEncoder.getInputMatrix()->setDimensionSize(0,m_channelsNumber);
		m_oAlgo1_SignalEncoder.getInputMatrix()->setDimensionSize(1,(m_samplesNumber)*2);
		for(uint32 channel=0; channel< m_channelsNumber; channel++)
		{
			m_oAlgo1_SignalEncoder.getInputMatrix()->setDimensionLabel(0, channel, 
													m_oAlgo0_SpectrumDecoder[0].getOutputMatrix()->getDimensionLabel(0, channel));
		}

		// Pass the header to the next boxes, by encoding a header on the output 0:
		m_oAlgo1_SignalEncoder.encodeHeader(outputNumber);
		// send the output chunk containing the header. The dates are the same as the input chunk:
		l_rDynamicBoxContext.markOutputAsReadyToSend(outputNumber, l_rDynamicBoxContext.getInputChunkStartTime(i, 0), 
																							l_rDynamicBoxContext.getInputChunkEndTime(i, 0));
																					
	}
	if(l_ui32BufferCount)
	{
			float64* bufferInput0= m_oAlgo0_SpectrumDecoder[0].getOutputMatrix()->getBuffer();
			float64* bufferInput1= m_oAlgo0_SpectrumDecoder[1].getOutputMatrix()->getBuffer();
			for(uint32 channel=0; channel< m_channelsNumber; channel++)
			{
				for(uint32 j=0; j< m_samplesNumber; j++)
				{
					m_frequencyBuffer[channel][j].real(bufferInput0[channel*m_samplesNumber+j]);
					m_frequencyBuffer[channel][j].imag(bufferInput1[channel*m_samplesNumber+j]);
					// implicit with itpp::ifft_real:
					/*m_frequencyBuffer[channel][(m_samplesNumber)*2-j].real(-m_frequencyBuffer[channel][j].real());
					m_frequencyBuffer[channel][(m_samplesNumber)*2-j].imag(m_frequencyBuffer[channel][j].imag());*/
				}
				m_signalBuffer[channel]= itpp::ifft_real(m_frequencyBuffer[channel]);
				float64* bufferOutput=m_oAlgo1_SignalEncoder.getInputMatrix()->getBuffer();
				for(uint32 j=0; j< (m_samplesNumber)*2; j++)
				{
					bufferOutput[channel*(m_samplesNumber)*2+j]= m_signalBuffer[channel][j];
				}
			}
					// Encode the output buffer :
			m_oAlgo1_SignalEncoder.encodeBuffer(outputNumber);
					// and send it to the next boxes :
			l_rDynamicBoxContext.markOutputAsReadyToSend(outputNumber, l_rDynamicBoxContext.getInputChunkStartTime(0, 0), 	
																									 l_rDynamicBoxContext.getInputChunkEndTime(0, 0));
		
	}																										
	if(l_ui32EndCount)
	{
				// End of stream received. This happens only once when pressing "stop". Just pass it to the next boxes so they receive the message :
			m_oAlgo1_SignalEncoder.encodeEnd(outputNumber);
			l_rDynamicBoxContext.markOutputAsReadyToSend(outputNumber, l_rDynamicBoxContext.getInputChunkStartTime(0, 0),
																									 l_rDynamicBoxContext.getInputChunkEndTime(0, 0));
	}																								
																												
		

	// here is some useful functions:
	// - To get input/output/setting count:
	// l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	
	// - To get the number of chunks currently available on a particular input :
	// l_rDynamicBoxContext.getInputChunkCount(input_index)
	
	// - To send an output chunk :
	// l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, chunk_start_time, chunk_end_time);
	
	
	// A typical process iteration may look like this.
	// This example only iterate over the first input of type Signal, and output a modified Signal.
	// thus, the box uses 1 decoder (m_oSignalDecoder) and 1 encoder (m_oSignalEncoder)
	/*
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//iterate over all chunk on input 0
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		// decode the chunk i on input 0
		m_oSignalDecoder.decode(0,i);
		// the decoder may have decoded 3 different parts : the header, a buffer or the end of stream.
		if(m_oSignalDecoder.isHeaderReceived())
		{
			// Header received. This happens only once when pressing "play". For example with a StreamedMatrix input, you now know the dimension count, sizes, and labels of the matrix
			// ... maybe do some process ...
			
			// Pass the header to the next boxes, by encoding a header on the output 0:
			m_oSignalEncoder.encodeHeader(0);
			// send the output chunk containing the header. The dates are the same as the input chunk:
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}
		if(m_oSignalDecoder.isBufferReceived())
		{
			// Buffer received. For example the signal values
			// Access to the buffer can be done thanks to :
			IMatrix* l_pMatrix = m_oSignalDecoder.getOutputMatrix(); // the StreamedMatrix of samples.
			uint64 l_uiSamplingFrequency = m_oSignalDecoder.getOutputSamplingRate(); // the sampling rate of the signal
			
			// ... do some process on the matrix ...

			// Encode the output buffer :
			m_oSignalEncoder.encodeBuffer(0);
			// and send it to the next boxes :
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
			
		}
		if(m_oSignalDecoder.isEndReceived())
		{
			// End of stream received. This happens only once when pressing "stop". Just pass it to the next boxes so they receive the message :
			m_oSignalEncoder.encodeEnd(0);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}

		// The current input chunk has been processed, and automaticcaly discarded.
		// you don't need to call "l_rDynamicBoxContext.markInputAsDeprecated(0, i);"
	}
	*/

	// check the official developer documentation webpage for more example and information :
	
	// Tutorials:
	// http://openvibe.inria.fr/documentation/#Developer+Documentation
	// Codec Toolkit page :
	// http://openvibe.inria.fr/codec-toolkit-references/
	
	// Feel free to ask experienced developers on the forum (http://openvibe.inria.fr/forum) and IRC (#openvibe on irc.freenode.net).

	return true;
}
