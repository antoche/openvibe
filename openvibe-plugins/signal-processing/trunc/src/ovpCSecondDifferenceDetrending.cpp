#include "ovpCSecondDifferenceDetrending.h"

using namespace OpenViBE;
using namespace OpenViBEPlugins;
using namespace OpenViBEToolkit;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBE::Kernel;

CSecondDifferenceDetrending::CSecondDifferenceDetrending(void) :
	m_pSignalReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_oSignalReaderCallBackProxy(*this,
		&CSecondDifferenceDetrending::setChannelCount,
		&CSecondDifferenceDetrending::setChannelName,
		&CSecondDifferenceDetrending::setSampleCountPerBuffer,
		&CSecondDifferenceDetrending::setSamplingRate,
		&CSecondDifferenceDetrending::setSampleBuffer),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_pSignalOutputWriter(NULL),
	m_oSignalOutputWriterCallbackProxy(*this,
		&CSecondDifferenceDetrending::appendSignalOutputChunkData),
	m_pSignalOutputWriterHelper(NULL),
	m_ui32ChannelCount(0),
	m_ui32SamplePerBufferCount(0),
	m_pLastSamples(NULL),
	m_pLastDifferences(NULL)
{
}

void CSecondDifferenceDetrending::release(void)
{
	delete this;
}

boolean CSecondDifferenceDetrending::initialize(void)
{
	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(m_oSignalReaderCallBackProxy);
	m_pSignalReader = EBML::createReader(*m_pSignalReaderCallBack);
	m_pSignalOutputWriter = EBML::createWriter(m_oSignalOutputWriterCallbackProxy);
	m_pSignalOutputWriterHelper = createBoxAlgorithmSignalOutputWriter();
	m_pInputBuffer = NULL;
	m_pOutputBuffer = NULL;
	m_pLastSamples = NULL;
	m_pLastDifferences = NULL;
	m_bAreLastSamplesValid=false;
	m_bAreLastDifferencesValid=false;
	return true;
}

boolean CSecondDifferenceDetrending::uninitialize(void)
{
	releaseBoxAlgorithmSignalInputReaderCallback(m_pSignalReaderCallBack);
	m_pSignalReader->release();
	m_pSignalOutputWriter->release();
	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalReaderCallBack = NULL;
	m_pSignalReader = NULL;
	m_pSignalOutputWriter = NULL;
	m_pSignalOutputWriterHelper = NULL;
	delete[] m_pOutputBuffer;
	delete[] m_pLastSamples;
	delete[] m_pLastDifferences;
	return true;
}

boolean CSecondDifferenceDetrending::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSecondDifferenceDetrending::process(void)
{
	IBoxIO * l_pDynamicContext = getBoxAlgorithmContext()->getDynamicBoxContext();
	uint32 l_ui32InputChunkCount = l_pDynamicContext->getInputChunkCount(0);
	for(uint32 i = 0; i < l_ui32InputChunkCount; i++)
    {
		uint64 l_ui64ChunkSize = 0;
		const uint8 * l_pChunkBuffer = NULL;
		if(l_pDynamicContext->getInputChunk(0, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pChunkBuffer))
		{
			m_pSignalReader->processData(l_pChunkBuffer, l_ui64ChunkSize);

			if(m_pInputBuffer == NULL) //dealing with the signal header
			{
				m_pSignalOutputWriterHelper->writeHeader(*m_pSignalOutputWriter);
				m_pOutputBuffer = new float64[m_ui32SamplePerBufferCount*m_ui32ChannelCount];
				m_pLastSamples = new float64[m_ui32ChannelCount];
				m_pLastDifferences = new float64[m_ui32ChannelCount];
				m_bAreLastSamplesValid=false;
				m_bAreLastDifferencesValid=false;
				m_pSignalOutputWriterHelper->setSampleBuffer(m_pOutputBuffer);
			}
			else
			{ //doing the processing

				//writing the output buffer for debug
				/*
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "inputBuffer after first differencing: ";
				for(uint32 d = 0; d < m_ui32SamplePerBufferCount; d++)
				{
					getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << "inputBuffer[" << d << "][0]: " << m_pInputBuffer[d] <<" ";
				}
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << "\n";
				*/

				//computing the first difference...
				//dealing with the left boundary
				if(m_bAreLastSamplesValid)
				{
					for(uint32 k = 0; k < m_ui32ChannelCount; k++)
					{
						m_pOutputBuffer[k*m_ui32SamplePerBufferCount] = m_pInputBuffer[k*m_ui32SamplePerBufferCount] - m_pLastSamples[k];
					}
				}
				else
				{
					for(uint32 k = 0; k < m_ui32ChannelCount; k++)
					{
						m_pOutputBuffer[k*m_ui32SamplePerBufferCount] = 0;
					}
				}

				//dealing with the rest of the input buffer
				for(uint32 j = 1; j < m_ui32SamplePerBufferCount; j++)
				{
					for(uint32 k = 0; k < m_ui32ChannelCount; k++)
					{
						uint32 l_ui32index = (k*m_ui32SamplePerBufferCount) + j;
						m_pOutputBuffer[l_ui32index] = m_pInputBuffer[l_ui32index] - m_pInputBuffer[l_ui32index - 1];
					}
				}

				//storing the last samples of the current buffer
				for(uint32 k = 0; k < m_ui32ChannelCount; k++)
				{
					m_pLastSamples[k] = m_pInputBuffer[k*m_ui32SamplePerBufferCount + m_ui32SamplePerBufferCount - 1];
				}
				m_bAreLastSamplesValid=true;

				//writing the output buffer for debug
				/*
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "outputBuffer after first differencing: ";
				for(uint32 d = 0; d < m_ui32SamplePerBufferCount; d++)
				{
					getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << "outputBuffer[" << d << "][0]: " << m_pOutputBuffer[d] << " ";
				}
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << "\n";
				*/

				//computing the second difference
				//dealing with the main part of the buffer
				for(uint32 j = m_ui32SamplePerBufferCount - 1; j >= 1; j--)
				{
					for(uint32 k = 0; k < m_ui32ChannelCount; k++)
					{
						uint32 l_ui32index = (k*m_ui32SamplePerBufferCount) + j;
						m_pOutputBuffer[l_ui32index] = m_pOutputBuffer[l_ui32index] - m_pOutputBuffer[l_ui32index - 1];
					}
				}

				//dealing with the left boundary
				if(m_bAreLastDifferencesValid)
				{
					for(uint32 k = 0; k < m_ui32ChannelCount; k++)
					{
						m_pOutputBuffer[k*m_ui32SamplePerBufferCount] = m_pOutputBuffer[k*m_ui32SamplePerBufferCount] - m_pLastDifferences[k];
					}
				}
				else
				{
					for(uint32 k = 0; k < m_ui32ChannelCount; k++)
					{
						m_pOutputBuffer[k*m_ui32SamplePerBufferCount] = 0;
						m_pOutputBuffer[k*m_ui32SamplePerBufferCount+1] = 0;
					}
				}

				//storing the last differences of the current buffer
				for(uint32 k = 0; k < m_ui32ChannelCount; k++)
				{
					m_pLastDifferences[k] = m_pInputBuffer[k*m_ui32SamplePerBufferCount + m_ui32SamplePerBufferCount - 1] - m_pInputBuffer[k*m_ui32SamplePerBufferCount + m_ui32SamplePerBufferCount - 2];
				}
				m_bAreLastDifferencesValid=true;

				/*
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "outputBuffer after second differencing: ";
				for(uint32 d = 0; d < m_ui32SamplePerBufferCount; d++)
				{
					getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << "outputBuffer[" << d << "][0]: " << m_pOutputBuffer[d] << " ";
				}
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << "\n";
				*/

				m_pSignalOutputWriterHelper->writeBuffer(*m_pSignalOutputWriter);
			}
			l_pDynamicContext->markOutputAsReadyToSend(0,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			l_pDynamicContext->markInputAsDeprecated(0,i);
		}
		else
		{
			//I should insult the user...
		}
    }
	return true;
}

void CSecondDifferenceDetrending::setChannelCount(const uint32 ui32ChannelCount)
{
	m_ui32ChannelCount = ui32ChannelCount;
	m_pSignalOutputWriterHelper->setChannelCount(ui32ChannelCount);
}

void CSecondDifferenceDetrending::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pSignalOutputWriterHelper->setChannelName(ui32ChannelIndex, sChannelName);
}

void CSecondDifferenceDetrending::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	m_ui32SamplePerBufferCount = ui32SampleCountPerBuffer;
	m_pSignalOutputWriterHelper->setSampleCountPerBuffer(ui32SampleCountPerBuffer);
}

void CSecondDifferenceDetrending::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_pSignalOutputWriterHelper->setSamplingRate(ui32SamplingFrequency);
}

void CSecondDifferenceDetrending::setSampleBuffer(const float64* pBuffer)
{
	m_pInputBuffer = pBuffer;
}

