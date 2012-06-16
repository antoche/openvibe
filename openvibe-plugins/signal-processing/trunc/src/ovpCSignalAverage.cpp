#include "ovpCSignalAverage.h"

#include <cmath>
#include <iostream>
#include <sstream>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

void CSignalAverage::setChannelCount(const uint32 ui32ChannelCount)
{
	//gets the channel count
	m_pSignalDescription->m_ui32ChannelCount = ui32ChannelCount;
	m_pSignalDescription->m_pChannelName.resize(ui32ChannelCount);
}

void CSignalAverage::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pSignalDescription->m_pChannelName[ui32ChannelIndex]=sChannelName;
}

void CSignalAverage::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	//gets the sample count
	m_pSignalDescription->m_ui32SampleCount = ui32SampleCountPerBuffer;

	//the matrix buffer hasn't been allocated yet, allocate it
	if(!m_pMatrixBuffer)
	{
		m_ui64MatrixBufferSize = m_pSignalDescription -> m_ui32ChannelCount;

		m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];
	}

	//we have everything needed to send the stream header

	//inferior since we made a temporal average of the samples
	uint32 l_ui32NewSamplingRate =static_cast<uint32>(ceil((float64)m_pSignalDescription->m_ui32SamplingRate/(float64)m_pSignalDescription->m_ui32SampleCount));

	m_pSignalOutputWriterHelper->setSamplingRate(l_ui32NewSamplingRate);

	m_pSignalOutputWriterHelper->setChannelCount(m_pSignalDescription->m_ui32ChannelCount);

	for(uint32 i=0 ; i<m_pSignalDescription->m_ui32ChannelCount ; i++)
	{
		m_pSignalOutputWriterHelper->setChannelName(i, m_pSignalDescription->m_pChannelName[i].c_str());
	}

	//just one sample per buffer since we made an average of the values in each buffer
	m_pSignalOutputWriterHelper->setSampleCountPerBuffer(1);
	m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

	m_pSignalOutputWriterHelper->writeHeader(*m_pWriter);

	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);

}

void CSignalAverage::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_pSignalDescription->m_ui32SamplingRate = ui32SamplingFrequency;
}

void CSignalAverage::setSampleBuffer(const float64* pBuffer)
{
	float64 l_f64SamplesSum = 0;

	//for each channel
	for(uint32 c=0 ; c<m_pSignalDescription->m_ui32ChannelCount ; c++)
	{
		l_f64SamplesSum = 0;

		//sum its samples
		for(uint64 i=0 ; i<m_pSignalDescription->m_ui32SampleCount ; i++)
		{
			l_f64SamplesSum += pBuffer[(c*m_pSignalDescription->m_ui32SampleCount)+i];
		}

		//computes and stores the average for a channel
		m_pMatrixBuffer[c] = l_f64SamplesSum / m_pSignalDescription->m_ui32SampleCount;
	}

	//sends the current signal matrix
	m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);

	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);

}

void CSignalAverage::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

CSignalAverage::CSignalAverage(void)
	: m_pReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_pWriter(NULL),
	m_oSignalOutputWriterCallbackProxy(
		*this,
		&CSignalAverage::writeSignalOutput),
	m_pSignalOutputWriterHelper(NULL),
	m_pSignalDescription(NULL),
	m_ui64MatrixBufferSize(0),
	m_pMatrixBuffer(NULL)
{
}

void CSignalAverage::release(void)
{
}

boolean CSignalAverage::initialize()
{

	//initialises the signal description structure
	m_pSignalDescription = new CSignalDescription();

	// Prepares EBML reader
	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(*this);
	m_pReader=EBML::createReader(*m_pSignalReaderCallBack);

	// Prepares EBML writer
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	return true;
}

boolean CSignalAverage::uninitialize()
{

	if(m_pMatrixBuffer)
	{
		delete[] m_pMatrixBuffer;
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

boolean CSignalAverage::processInput( uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSignalAverage::process()
{
	IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for(uint32 i=0; i<l_pDynamicBoxContext->getInputChunkCount(0); i++)
	{

		uint64 l_ui64ChunkSize;
		const uint8* l_pBuffer;
		l_pDynamicBoxContext->getInputChunk(0, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer);
		l_pDynamicBoxContext->markInputAsDeprecated(0, i);
		m_pReader->processData(l_pBuffer, l_ui64ChunkSize);
	}

	return true;
}
