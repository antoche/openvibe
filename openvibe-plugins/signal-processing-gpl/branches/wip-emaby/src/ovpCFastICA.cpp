#include "ovpCFastICA.h"

#include <iostream>
#include <sstream>

#include <itpp/itstat.h>
#include <itpp/itsignal.h>

using namespace itpp;

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace OpenViBEPlugins::SignalProcessingGpl::FastICA;
using namespace OpenViBEToolkit;
using namespace std;

void CFastICA::setChannelCount(const uint32 ui32ChannelCount)
{
	//gets the channel count
	m_pSignalDescription->m_ui32ChannelCount = ui32ChannelCount;
	m_pSignalDescription->m_pChannelName.resize(ui32ChannelCount);
}

void CFastICA::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pSignalDescription->m_pChannelName[ui32ChannelIndex]=sChannelName;
}

void CFastICA::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
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

void CFastICA::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_pSignalDescription->m_ui32SamplingRate = ui32SamplingFrequency;
}

void CFastICA::setSampleBuffer(const float64* pBuffer)
{
	mat sources(m_pSignalDescription->m_ui32ChannelCount, m_pSignalDescription->m_ui32SampleCount);
	mat ICs(m_pSignalDescription->m_ui32ChannelCount, m_pSignalDescription->m_ui32SampleCount);

	for (uint64 i=0;  i < m_pSignalDescription->m_ui32ChannelCount; i++)
	{
		for(uint64 j=0 ; j<m_pSignalDescription->m_ui32SampleCount ; j++)
		{
			sources(i, j) =  (double)pBuffer[i*m_pSignalDescription->m_ui32SampleCount+j];
		}
	}

	Fast_ICA fastica(sources);
	fastica.set_nrof_independent_components(sources.rows());
	fastica.set_non_linearity(FICA_NONLIN_TANH);
	fastica.set_approach(FICA_APPROACH_DEFL);
	fastica.separate();
	ICs = fastica.get_independent_components();

	for (uint64 i=0;  i < m_pSignalDescription->m_ui32ChannelCount; i++)
	{
		for(uint64 j=0 ; j<m_pSignalDescription->m_ui32SampleCount ; j++)
		{
			m_pMatrixBuffer[i*m_pSignalDescription->m_ui32SampleCount+j] = ICs(i,j);
		}
	}

	//sends the current signal matrix
	m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);
	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
}

void CFastICA::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

CFastICA::CFastICA(void)
	:m_pReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_pWriter(NULL),
	m_oSignalOutputWriterCallbackProxy(
		*this,
		&CFastICA::writeSignalOutput),
	m_pSignalOutputWriterHelper(NULL),
	m_pSignalDescription(NULL),
	m_ui64MatrixBufferSize(0),
	m_pMatrixBuffer(NULL)
{
}

void CFastICA::release(void)
{
	delete this;
}

boolean CFastICA::initialize()
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

boolean CFastICA::uninitialize()
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

boolean CFastICA::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CFastICA::process()
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
