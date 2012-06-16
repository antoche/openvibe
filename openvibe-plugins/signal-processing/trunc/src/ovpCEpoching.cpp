#include "ovpCEpoching.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEPlugins::SignalProcessing::Epoching;
using namespace OpenViBEToolkit;
using namespace std;

void CEpoching::setChannelCount(const uint32 ui32ChannelCount)
{
	//gets the channel count
	m_pSignalDescription->m_ui32ChannelCount = ui32ChannelCount;
	m_pSignalDescription->m_pChannelName.resize(ui32ChannelCount);
}

void CEpoching::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pSignalDescription->m_pChannelName[ui32ChannelIndex]=sChannelName;
}

void CEpoching::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	//gets the sample count
	m_pSignalDescription->m_ui32SampleCount = ui32SampleCountPerBuffer;
	m_ui32SizeInputBuffer = ui32SampleCountPerBuffer;
	m_ui32InputChannelCount = m_pSignalDescription->m_ui32ChannelCount;

	//translates time intervals in samples
	m_ui32SizeSampleOutputBuffer = static_cast<uint32>(floor((float64)m_float64SizeTimeOutputBuffer * (float64)m_pSignalDescription->m_ui32SamplingRate / 1000));

	//the matrix buffer hasn't been allocated yet, allocate it
	if(!m_pMatrixBuffer)
	{
		m_ui64MatrixBufferSize = m_ui32SizeSampleOutputBuffer * m_pSignalDescription -> m_ui32ChannelCount;
		m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];
	}

	//we have everything needed to send the stream header
	m_pSignalOutputWriterHelper->setSamplingRate(m_pSignalDescription->m_ui32SamplingRate);
	m_pSignalOutputWriterHelper->setChannelCount(m_pSignalDescription->m_ui32ChannelCount);

	for(uint32 i=0 ; i<m_pSignalDescription->m_ui32ChannelCount ; i++)
	{
		m_pSignalOutputWriterHelper->setChannelName(i, m_pSignalDescription->m_pChannelName[i].c_str());
	}

	m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_ui32SizeSampleOutputBuffer);
	m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

	m_pSignalOutputWriterHelper->writeHeader(*m_pWriter);

	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
}

void CEpoching::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_pSignalDescription->m_ui32SamplingRate = ui32SamplingFrequency;
}

void CEpoching::setSampleBuffer(const float64* pBuffer)
{
	//translates time intervals in samples
	m_ui32IndexLoopBuffer++;
	if (m_ui32IndexLoopBuffer==1)
	{
		m_ui32SizeSampleIntervalInterTrigger = static_cast<uint32>(
				floor((float64)m_float64SizeTimeIntervalInterTrigger*(float64)m_ui32IndexLoopBuffer* (float64)m_pSignalDescription->m_ui32SamplingRate / 1000)
		);
	}
	else
	{
		m_ui32SizeSampleIntervalInterTrigger = static_cast<uint32>(
				floor((float64)m_float64SizeTimeIntervalInterTrigger*(float64)m_ui32IndexLoopBuffer* (float64)m_pSignalDescription->m_ui32SamplingRate / 1000)
			-	floor((float64)m_float64SizeTimeIntervalInterTrigger*(float64)(m_ui32IndexLoopBuffer-1)* (float64)m_pSignalDescription->m_ui32SamplingRate / 1000)
		);
	}

	if (m_ui32SizeSampleIntervalInterTrigger == m_ui32SizeInputBuffer && m_ui32SizeSampleOutputBuffer <= m_ui32SizeInputBuffer)
	{
		if (m_ui32SizeSampleOutputBuffer < m_ui32SizeInputBuffer)
		{
			// send one buffer smaller than input
			for (uint32 i=0; i < m_ui32InputChannelCount ; i++)
			{
				for (uint32 j=0; j < m_ui32SizeSampleOutputBuffer; j++)
				{
					m_pMatrixBuffer[j + i*m_ui32SizeSampleOutputBuffer] = pBuffer[j + i*m_ui32SizeInputBuffer];
				}
			}

			// compute timestart
			m_ui32IndexInGlobal += m_ui32SizeSampleIntervalInterTrigger;
			m_ui64LastChunkStartTime = ((m_ui32IndexInGlobal) << 32) / m_pSignalDescription->m_ui32SamplingRate;

			// compute timestop
			m_ui64LastChunkEndTime = ((m_ui32IndexInGlobal + m_ui32SizeSampleOutputBuffer) << 32) / m_pSignalDescription->m_ui32SamplingRate;

			//sends the current signal matrix
			m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
		}
		else if (m_ui32SizeSampleOutputBuffer == m_ui32SizeInputBuffer)
		{
			System::Memory::copy(m_pMatrixBuffer, pBuffer, m_ui64MatrixBufferSize*sizeof(float64));

			//sends the current signal matrix
			m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
		}
	}
	else
	{
		if (m_ui32SizeSampleIntervalInterTrigger == m_ui32SizeInputBuffer)
		{
			if (m_ui32SizeSampleOutputBuffer > m_ui32SizeInputBuffer)
			{
				m_ui32SizeTempMatrixBuffer = static_cast<uint32>(floor((float64)m_ui32SizeSampleOutputBuffer/(float64)m_ui32SizeInputBuffer)+1);
			}
		}
		else if (m_ui32SizeSampleIntervalInterTrigger > m_ui32SizeInputBuffer)
		{
			if (m_ui32SizeSampleOutputBuffer < m_ui32SizeInputBuffer)
			{
				m_ui32SizeTempMatrixBuffer = 2;
			}
			else if (m_ui32SizeSampleOutputBuffer == m_ui32SizeInputBuffer)
			{
				m_ui32SizeTempMatrixBuffer = 2;
			}
			else if (m_ui32SizeSampleOutputBuffer > m_ui32SizeInputBuffer)
			{
				m_ui32SizeTempMatrixBuffer = static_cast<uint32>(floor(((float64)m_ui32SizeSampleOutputBuffer-1)/(float64)m_ui32SizeInputBuffer)+2);
			}
		}
		else if (m_ui32SizeSampleIntervalInterTrigger < m_ui32SizeInputBuffer)
		{
			if (m_ui32SizeSampleOutputBuffer < m_ui32SizeInputBuffer)
			{
				m_ui32SizeTempMatrixBuffer = 2;
			}
			else if (m_ui32SizeSampleOutputBuffer == m_ui32SizeInputBuffer)
			{
				m_ui32SizeTempMatrixBuffer = 2;
			}
			else if (m_ui32SizeSampleOutputBuffer > m_ui32SizeInputBuffer)
			{
				m_ui32SizeTempMatrixBuffer = static_cast<uint32>(floor(((float64)m_ui32SizeSampleOutputBuffer-1)/(float64)m_ui32SizeInputBuffer) +2);
			}
		}

		if (m_bFlagFirstTime)
		{
			// historic of data with size equal to m_ui32SizeTempMatrixBuffer times of input buffer size
			m_pTempMatrixBuffer = new OpenViBE::float64[(size_t)m_ui32SizeTempMatrixBuffer*m_ui32SizeInputBuffer*m_ui32InputChannelCount];
			m_pTempMatrixDbBuffer  = new OpenViBE::float64[(size_t)(m_ui32SizeTempMatrixBuffer-1)*m_ui32SizeInputBuffer*m_ui32InputChannelCount];
			//~ m_pTempMatrixBuffer.resize(m_ui32SizeTempMatrixBuffer*m_ui32SizeInputBuffer*m_ui32InputChannelCount, 0);
			//~ m_pTempMatrixDbBuffer.resize((m_ui32SizeTempMatrixBuffer-1)*m_ui32SizeInputBuffer*m_ui32InputChannelCount, 0);

			for(uint64 i=0; i<m_ui32SizeTempMatrixBuffer*m_ui32SizeInputBuffer*m_ui32InputChannelCount ; i++)
			{
				m_pTempMatrixBuffer[i] = 0;
			}
			for(uint64 i=0; i<(m_ui32SizeTempMatrixBuffer-1)*m_ui32SizeInputBuffer*m_ui32InputChannelCount ; i++)
			{
				m_pTempMatrixDbBuffer[i] = 0;
			}
			m_ui32IndexIn = (m_ui32SizeTempMatrixBuffer-1)*m_ui32SizeInputBuffer;
			m_ui32IndexTempMatrixBuffer=0;
			m_bFlagFirstTime = false;
		}

		// save data into last part of historic buffer
		for(uint64 i=0; i<m_ui32SizeInputBuffer*m_ui32InputChannelCount ; i++)
		{
			m_pTempMatrixBuffer[i + (m_ui32SizeTempMatrixBuffer-1)*m_ui32SizeInputBuffer*m_ui32InputChannelCount] = pBuffer[i];
		}

		// send all the buffer you can
		float64 l_float64FracPart = 0;
		float64 l_float64IntPart = 0;

		while ((m_ui32IndexIn + m_ui32SizeSampleOutputBuffer) < (m_ui32SizeTempMatrixBuffer*m_ui32SizeInputBuffer))
		{
			for (uint32 i=0; i < m_ui32InputChannelCount ; i++)
			{
				for (uint32 j=0; j < m_ui32SizeSampleOutputBuffer; j++)
				{
					l_float64FracPart = (float64)((j + m_ui32IndexIn)%m_ui32SizeInputBuffer);
					l_float64IntPart = floor((float64)((m_ui32IndexIn + j)/m_ui32SizeInputBuffer));

					m_ui32IndexTempMatrixBuffer = static_cast<uint32>(
						l_float64FracPart +
						(float64)(i*m_ui32SizeInputBuffer) +
						(float64)(l_float64IntPart*m_ui32SizeInputBuffer*m_ui32InputChannelCount)
					);

					m_pMatrixBuffer[j + i*m_ui32SizeSampleOutputBuffer] = m_pTempMatrixBuffer[m_ui32IndexTempMatrixBuffer];
				}
			}

			// compute timestop
			m_ui64LastChunkEndTime = ((m_ui32IndexInGlobal + m_ui32SizeSampleOutputBuffer) << 32) / m_pSignalDescription->m_ui32SamplingRate;

			//sends the current signal matrix
			m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);

			// compute timestart
			m_ui32IndexInGlobal += m_ui32SizeSampleIntervalInterTrigger;
			m_ui64LastChunkStartTime = ((m_ui32IndexInGlobal) << 32) / m_pSignalDescription->m_ui32SamplingRate;

			//~ cout << "m_ui64LastChunkStartTime= "<<m_ui64LastChunkStartTime<< std::endl;
			//~ cout << "m_ui64LastChunkEndTime = " <<m_ui64LastChunkEndTime << std::endl;

			//~ if(m_pMatrixBuffer)
			//~ {
				//~ delete[] m_pMatrixBuffer;
				//~ m_ui64MatrixBufferSize = m_ui32SizeSampleOutputBuffer * m_pSignalDescription -> m_ui32ChannelCount;
				//~ m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];
			//~ }

			m_ui32IndexIn = m_ui32IndexIn + m_ui32SizeSampleIntervalInterTrigger;
		}

		// save data into 1st part of historic buffer
		for(uint64 i=0; i<(m_ui32SizeTempMatrixBuffer-1)*m_ui32SizeInputBuffer*m_ui32InputChannelCount ; i++)
		{
			m_pTempMatrixDbBuffer[i] = m_pTempMatrixBuffer[i+m_ui32SizeInputBuffer*m_ui32InputChannelCount];
		}
		for(uint64 i=0; i<(m_ui32SizeTempMatrixBuffer-1)*m_ui32SizeInputBuffer*m_ui32InputChannelCount ; i++)
		{
			m_pTempMatrixBuffer[i] = m_pTempMatrixDbBuffer[i];
		}

		m_ui32IndexIn = m_ui32IndexIn - m_ui32SizeInputBuffer;
	}
}

void CEpoching::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

CEpoching::CEpoching(void)
	:m_pReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_pWriter(NULL),
	m_oSignalOutputWriterCallbackProxy(
		*this,
		&CEpoching::writeSignalOutput),
	m_pSignalOutputWriterHelper(NULL),
	m_pSignalDescription(NULL),
	m_ui64MatrixBufferSize(0),
	m_pMatrixBuffer(NULL),
	m_pTempMatrixBuffer(NULL),
	m_pTempMatrixDbBuffer(NULL),
	m_bFlagFirstTime(true)
{
}

void CEpoching::release(void)
{
	delete this;
}

boolean CEpoching::initialize()
{
	//reads the plugin settings
	CString l_oSizeTimeOutputBuffer;
	CString l_oSizeTimeIntervalInterTrigger;

	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_oSizeTimeIntervalInterTrigger);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1, l_oSizeTimeOutputBuffer);

	m_float64SizeTimeIntervalInterTrigger = static_cast<float64>(atof(l_oSizeTimeIntervalInterTrigger));
	m_float64SizeTimeOutputBuffer = static_cast<float64>(atof(l_oSizeTimeOutputBuffer));

	//initalises the signal description structure
	m_pSignalDescription = new CSignalDescription();

	// Prepares EBML reader
	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(*this);
	m_pReader=EBML::createReader(*m_pSignalReaderCallBack);

	// Prepares EBML writer
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	m_ui32IndexIn = 0;
	m_ui32IndexInGlobal = 0;
	m_ui32IndexLoopBuffer = 0;

	return true;
}

boolean CEpoching::uninitialize()
{
	if(m_pMatrixBuffer)
	{
		delete[] m_pMatrixBuffer;
	}

	if(m_pTempMatrixBuffer)
	{
		delete[] m_pTempMatrixBuffer;
	}

	if(m_pTempMatrixDbBuffer)
	{
		delete[] m_pTempMatrixDbBuffer;
	}

	// Cleans up EBML writer
	m_pWriter->release();
	m_pWriter=NULL;

	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;

	// Cleans up EBML reader
	releaseBoxAlgorithmSignalInputReaderCallback(m_pSignalReaderCallBack);

	m_pReader->release();
	m_pReader=NULL;

	delete m_pSignalDescription;
	m_pSignalDescription = NULL;

	return true;
}

boolean CEpoching::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CEpoching::process()
{
	IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	//reset the OutPut chunk
	l_pDynamicBoxContext->setOutputChunkSize(0, 0);

	// Process input data
	for(uint32 i=0; i<l_pDynamicBoxContext->getInputChunkCount(0); i++)
	{
		uint64 l_ui64ChunkSize;
		const uint8* l_pBuffer;
		uint64 l_ui64LastChunkStartTime;
		uint64 l_ui64LastChunkEndTime;

		l_pDynamicBoxContext->getInputChunk(0, i, l_ui64LastChunkStartTime, l_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer);
		l_pDynamicBoxContext->markInputAsDeprecated(0, i);
		m_pReader->processData(l_pBuffer, l_ui64ChunkSize);
	}

	return true;
}
