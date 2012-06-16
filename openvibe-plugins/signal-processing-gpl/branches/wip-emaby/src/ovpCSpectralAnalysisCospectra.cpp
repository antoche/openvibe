#include "ovpCSpectralAnalysisCospectra.h"

#include <iostream>
#include <sstream>

#include <itpp/itstat.h>
#include <itpp/itsignal.h>

using namespace itpp;

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace OpenViBEPlugins::SignalProcessingGpl::SpectralAnalysisCospectra;
using namespace OpenViBEToolkit;
using namespace std;

void CSpectralAnalysisCospectra::setChannelCount(const uint32 ui32ChannelCount)
{
	//gets the channel count
	m_pSignalDescription->m_ui32ChannelCount = ui32ChannelCount;
	m_pSignalDescription->m_pChannelName.resize(ui32ChannelCount);
}

void CSpectralAnalysisCospectra::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pSignalDescription->m_pChannelName[ui32ChannelIndex]=sChannelName;
}

void CSpectralAnalysisCospectra::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	//gets the sample count
	m_pSignalDescription->m_ui32SampleCount = ui32SampleCountPerBuffer;

	//the matrix buffer hasn't been allocated yet, allocate it
	if(!m_pMatrixBuffer)
	{
		m_ui64MatrixBufferSize = m_pSignalDescription -> m_ui32SampleCount * m_pSignalDescription -> m_ui32ChannelCount;

		m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];
	}

	//we have everything needed to send the stream header
	m_pSignalOutputWriterHelper->setSamplingRate(m_pSignalDescription->m_ui32SamplingRate);
	m_pSignalOutputWriterHelper->setChannelCount(m_pSignalDescription->m_ui32ChannelCount);

	for(uint32 i=0 ; i<m_pSignalDescription->m_ui32ChannelCount ; i++)
	{
		m_pSignalOutputWriterHelper->setChannelName(i, m_pSignalDescription->m_pChannelName[i].c_str());
	}

	m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_pSignalDescription->m_ui32SampleCount);
	m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

	m_pSignalOutputWriterHelper->writeHeader(*m_pWriter);

	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
}

void CSpectralAnalysisCospectra::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_pSignalDescription->m_ui32SamplingRate = ui32SamplingFrequency;
}

void CSpectralAnalysisCospectra::setSampleBuffer(const float64* pBuffer)
{
	vec x1(m_pSignalDescription->m_ui32SampleCount), x2(m_pSignalDescription->m_ui32SampleCount);
	vec a1(m_pSignalDescription->m_ui32SampleCount), a2(m_pSignalDescription->m_ui32SampleCount);
	vec b1(m_pSignalDescription->m_ui32SampleCount), b2(m_pSignalDescription->m_ui32SampleCount);
	cvec y1(m_pSignalDescription->m_ui32SampleCount), y2(m_pSignalDescription->m_ui32SampleCount);

	uint32 l_ui32Index = 0;

	while (l_ui32Index + m_ui32WindowSize < m_pSignalDescription->m_ui32SampleCount)
	{
		for (uint64 i=0;  i < m_pSignalDescription->m_ui32ChannelCount; i++)
		{
			for(uint64 j=0 ; j<m_ui32WindowSize ; j++)
			{
				x1[j] =  (double)pBuffer[i*(m_ui32WindowSize+l_ui32Index)+j];
			}
			y1 = fft_real(x1, m_ui32WindowSize);
			a1 = real(y1);
			b1 = imag(y1);

			for (uint64 k=0;  k < m_pSignalDescription->m_ui32ChannelCount; k++)
			{
				for(uint64 l=0 ; l<m_ui32WindowSize ; l++)
				{
					x2[l] =  (double)pBuffer[k*(m_ui32WindowSize+l_ui32Index)+l];
				}
				y2 = fft_real(x2, m_ui32WindowSize);
				a2 = real(y2);
				b2 = imag(y2);
			}
		}
		l_ui32Index += m_ui32WindowSize;
	}

	m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);
	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
}

void CSpectralAnalysisCospectra::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

CSpectralAnalysisCospectra::CSpectralAnalysisCospectra(void)
	:m_pReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_pWriter(NULL),
	m_oSignalOutputWriterCallbackProxy(
		*this,
		&CSpectralAnalysisCospectra::writeSignalOutput),
	m_pSignalOutputWriterHelper(NULL),
	m_pSignalDescription(NULL),
	m_ui64MatrixBufferSize(0),
	m_pMatrixBuffer(NULL)
{
}

void CSpectralAnalysisCospectra::release(void)
{
	delete this;
}

boolean CSpectralAnalysisCospectra::initialize()
{
	//initSquareRootWindow;ialises the signal description structure
	m_pSignalDescription = new CSignalDescription();

	// Prepares EBML reader
	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(*this);
	m_pReader=EBML::createReader(*m_pSignalReaderCallBack);

	// Prepares EBML writer
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	return true;
}

boolean CSpectralAnalysisCospectra::uninitialize()
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

boolean CSpectralAnalysisCospectra::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSpectralAnalysisCospectra::process()
{
	IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	//reset the OutPut chunk
	l_pDynamicBoxContext->setOutputChunkSize(0, 0);

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
