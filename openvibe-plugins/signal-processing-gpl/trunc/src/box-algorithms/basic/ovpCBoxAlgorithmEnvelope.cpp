// to get fft and ifft:
#include <itpp/itsignal.h>
#include "ovpCBoxAlgorithmEnvelope.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingBasic;



boolean CBoxAlgorithmEnvelope::initialize(void)
{
	// Signal stream decoder
	m_oSignalDecoder.initialize(*this);
	// Signal stream encoder
	m_oSignalEncoder.initialize(*this);
	m_oSignalEncoder.getInputSamplingRate().setReferenceTarget(m_oSignalDecoder.getOutputSamplingRate());
	m_oSignalEncoder.getInputMatrix().setReferenceTarget(m_oSignalDecoder.getOutputMatrix());
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

boolean CBoxAlgorithmEnvelope::uninitialize(void)
{
	m_oSignalDecoder.uninitialize();
	m_oSignalEncoder.uninitialize();

	return true;
}
/*******************************************************************************/


//boolean CBoxAlgorithmEnvelope::processClock(IMessageClock& rMessageClock)
//{
//	// some pre-processing code if needed...

//	// ready to process !
//	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

//	return true;
//}
/*******************************************************************************/


//uint64 CBoxAlgorithmEnvelope::getClockFrequency(void)
//{
//	// Note that the time is coded on a 64 bits unsigned integer, fixed decimal point (32:32)
//	return 1LL<<32LL; // the box clock frequency
//}
/*******************************************************************************/


boolean CBoxAlgorithmEnvelope::processInput(uint32 ui32InputIndex)
{
	// some pre-processing code if needed...

	// ready to process !
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmEnvelope::process(void)
{
	
	// the static box context describes the box inputs, outputs, settings structures
	//IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// the dynamic box context describes the current state of the box inputs and outputs (i.e. the chunks)
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

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
	
	//IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

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
			IMatrix* l_pMatrix = m_oSignalDecoder.getOutputMatrix();
			m_channelsNumber=l_pMatrix->getDimensionSize(0);
			m_samplesNumber = l_pMatrix->getDimensionSize(1);
			m_frequencyBuffer.resize(m_channelsNumber);
			m_signalBuffer.resize(m_channelsNumber);
			for(uint32 channel=0; channel< m_channelsNumber; channel++)
			{
				m_signalBuffer[channel].set_size(m_samplesNumber+20);
				m_frequencyBuffer[channel].set_size(m_samplesNumber+20);
			}
			
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
			float64* bufferInput= l_pMatrix->getBuffer();
			// Computing hilbert transform for all channels
			for(uint32 channel=0; channel< m_channelsNumber; channel++)
			{
				// copy signal chunk on itpp buffer
				for(uint32 j=0; j< m_samplesNumber; j++)
				{
					m_signalBuffer[channel][j+10].real(bufferInput[channel*m_samplesNumber+j]);
					m_signalBuffer[channel][j+10].imag(0.0);
				}
				for(uint32 j=0; j< 10; j++)
				{
					m_signalBuffer[channel][j]=m_signalBuffer[channel][10];
					m_signalBuffer[channel][j+10+m_samplesNumber]=m_signalBuffer[channel][10+m_samplesNumber-1];
				}
				// compute Fast Fourier Transform of signal chunk
				itpp::fft(m_signalBuffer[channel],m_frequencyBuffer[channel]);
				// Apply hilbert transform in frequency space
				if ((m_samplesNumber+20)%2 == 0)
				{
					for(uint32 j=1; j< (m_samplesNumber+20)/2; j++)
						m_frequencyBuffer[channel][j]*=2.0;
					for(uint32 j=(m_samplesNumber+20)/2+1; j< (m_samplesNumber+20); j++)
						m_frequencyBuffer[channel][j]=0.0;
				}
				else
				{				
					for(uint32 j=1; j< ((m_samplesNumber+20)+1)/2; j++)
						m_frequencyBuffer[channel][j]*=2.0;
					for(uint32 j=((m_samplesNumber+20)+1)/2+1; j< (m_samplesNumber+20); j++)
						m_frequencyBuffer[channel][j]=0.0;
				}
				// compute Inverse Fast Fourier Transform of frequency
				itpp::ifft(m_frequencyBuffer[channel],m_signalBuffer[channel]);
				// copy itpp buffer into bufferInput (=bufferOutput) 
				for(uint32 j=0; j< m_samplesNumber; j++)
				{
					bufferInput[channel*m_samplesNumber+j]=abs(m_signalBuffer[channel][j+10]);
				}
			}

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
	

	// check the official developer documentation webpage for more example and information :
	
	// Tutorials:
	// http://openvibe.inria.fr/documentation/#Developer+Documentation
	// Codec Toolkit page :
	// http://openvibe.inria.fr/codec-toolkit-references/
	
	// Feel free to ask experienced developers on the forum (http://openvibe.inria.fr/forum) and IRC (#openvibe on irc.freenode.net).

	return true;
}
