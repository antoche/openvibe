#include "ovpCReferenceChannel.h"

#include <system/Memory.h>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;

void CReferenceChannelOld::setChannelCount(const uint32 ui32ChannelCount)
{
	m_oChannelNames.resize(ui32ChannelCount);

	if(m_ui32ReferenceChannel >= ui32ChannelCount)
	{
		m_bError = true;
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning <<"Channel index exceeds input channel count\n";
	}
}

void CReferenceChannelOld::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_oChannelNames[ui32ChannelIndex] = sChannelName;
}

void CReferenceChannelOld::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	m_ui32SamplesPerBuffer = ui32SampleCountPerBuffer;

	//allocate matrix
	m_ui64MatrixBufferSize = m_ui32SamplesPerBuffer * (uint32)m_oChannelNames.size();
	m_pMatrixBuffer = new float64[static_cast<size_t>(m_ui64MatrixBufferSize)];

	//send header
	m_pSignalOutputWriterHelper->setSamplingRate(m_ui32SamplingFrequency);
	m_pSignalOutputWriterHelper->setChannelCount((uint32)m_oChannelNames.size());

	for(size_t i=0 ; i<m_oChannelNames.size(); i++)
	{
		m_pSignalOutputWriterHelper->setChannelName((uint32)i, m_oChannelNames[i].c_str());
	}

	m_pSignalOutputWriterHelper->setSampleCountPerBuffer((uint32) m_ui32SamplesPerBuffer);
	m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

	m_pSignalOutputWriterHelper->writeHeader(*m_pWriter);

	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);

}

void CReferenceChannelOld::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_ui32SamplingFrequency = ui32SamplingFrequency;
}

void CReferenceChannelOld::setSampleBuffer(const float64* pBuffer)
{
	if(!m_bError)
	{
		const float64 * l_pReference = pBuffer + (m_ui32ReferenceChannel * m_ui32SamplesPerBuffer);

		for(size_t i=0 ; i<m_oChannelNames.size() ; i++)
		{
			//substract
			uint64 l_ui64Index = i*m_ui32SamplesPerBuffer;
			for(uint32 j=0 ; j<m_ui32SamplesPerBuffer ; j++, l_ui64Index++)
			{
				m_pMatrixBuffer[l_ui64Index] = pBuffer[l_ui64Index] - l_pReference[j];
			}
		}

		//the matrix is ready to be sent
		m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);
		getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
	}
}

void CReferenceChannelOld::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

CReferenceChannelOld::CReferenceChannelOld() :
	m_pReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_pWriter(NULL),
	m_oSignalOutputWriterCallbackProxy(
			*this,
			&CReferenceChannelOld::writeSignalOutput),
	m_pSignalOutputWriterHelper(NULL),
	m_ui32SamplingFrequency(0),
	m_ui32SamplesPerBuffer(0),
	m_ui32ReferenceChannel(0),
	m_bError(false),
	m_pMatrixBuffer(NULL),
	m_ui64MatrixBufferSize(0)
{
}

boolean CReferenceChannelOld::initialize()
{
	CString l_sSettings;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sSettings);
	m_ui32ReferenceChannel = (uint32)atoi((const char*)l_sSettings);

	// Prepares EBML reader
	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(*this);
	m_pReader=EBML::createReader(*m_pSignalReaderCallBack);

	// Prepares EBML writer
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	return true;
}

boolean CReferenceChannelOld::uninitialize()
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

	return true;
}

boolean CReferenceChannelOld::processInput(uint32 ui32InputIndex)
{
	//don't process anything if an error has occured
	if(m_bError)
	{
		return false;
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CReferenceChannelOld::process()
{
	IBoxIO* l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for(uint32 i=0; i<l_pBoxIO->getInputChunkCount(0); i++)
	{

		uint64 l_ui64ChunkSize;
		const uint8* l_pBuffer;
		l_pBoxIO->getInputChunk(0, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer);
		l_pBoxIO->markInputAsDeprecated(0, i);
		m_pReader->processData(l_pBuffer, l_ui64ChunkSize);
	}
	return true;
}
