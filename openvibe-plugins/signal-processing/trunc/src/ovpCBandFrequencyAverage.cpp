#include "ovpCBandFrequencyAverage.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;
using namespace OpenViBE::Kernel;


CBandFrequencyAverage::CBandFrequencyAverage(void)
	:m_pSpectrumReader(NULL),
	m_pSpectrumReaderCallback(NULL),
	m_oSpectrumReaderCallbackProxy(
		*this,
		&CBandFrequencyAverage::setChannelCount,
		&CBandFrequencyAverage::setChannelName,
		&CBandFrequencyAverage::setFrequencyBandCount,
		&CBandFrequencyAverage::setFrequencyBandName,
		&CBandFrequencyAverage::setFrequencyBandStart,
		&CBandFrequencyAverage::setFrequencyBandStop,
		&CBandFrequencyAverage::setBuffer
	),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_oFeatureVectorOutputWriterCallbackProxy(
		*this,
		&CBandFrequencyAverage::writeFeatureVectorOutput),
	m_pFeatureVectorOutputWriterHelper(NULL),
	m_bCoefComputed(false)
{
	m_pWriter = NULL;
}

void CBandFrequencyAverage::release(void)
{
	delete this;
}

boolean CBandFrequencyAverage::initialize()
{
	//reads the plugin settings
	CString l_oLowFrequency;
	CString l_oHighFrequency;

	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_oLowFrequency);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1, l_oHighFrequency);

	m_ui32LowFrequency= atoi(l_oLowFrequency);
	m_ui32HighFrequency = atoi(l_oHighFrequency);

	// Prepares EBML reader
	m_pSpectrumReaderCallback = createBoxAlgorithmSpectrumInputReaderCallback(m_oSpectrumReaderCallbackProxy);
	m_pSpectrumReader=EBML::createReader(*m_pSpectrumReaderCallback);

	// Prepares EBML writer
	m_pFeatureVectorOutputWriterHelper=createBoxAlgorithmFeatureVectorOutputWriter();
	m_pWriter=EBML::createWriter(m_oFeatureVectorOutputWriterCallbackProxy);

	m_pInputBuffer = NULL;
	m_pOutputBuffer = NULL;

	return true;
}

boolean CBandFrequencyAverage::uninitialize()
{
	// Cleans up EBML writer
	m_pWriter->release();
	m_pWriter=NULL;

	releaseBoxAlgorithmFeatureVectorOutputWriter(m_pFeatureVectorOutputWriterHelper);
	m_pFeatureVectorOutputWriterHelper=NULL;

	// Cleans up EBML reader
	releaseBoxAlgorithmSpectrumInputReaderCallback(m_pSpectrumReaderCallback);
	m_pSpectrumReader->release();
	m_pSpectrumReaderCallback = NULL;
	m_pSpectrumReader=NULL;

	delete[] m_pOutputBuffer;

	return true;
}

boolean CBandFrequencyAverage::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

void CBandFrequencyAverage::setChannelCount(const uint32 ui32ChannelCount)
{
	//gets the channel count
	m_ui32ChannelCount = ui32ChannelCount;
}

void CBandFrequencyAverage::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{

}

void CBandFrequencyAverage::setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount)
{
	m_ui32FrequencyBandCount = ui32FrequencyBandCount;
	m_pFrequencyBandStart = new float64[m_ui32FrequencyBandCount];
	m_pFrequencyBandStop = new float64[m_ui32FrequencyBandCount];
}

void CBandFrequencyAverage::setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName)
{
	//~ cout << "index= " << ui32FrequencyBandIndex << ", sFrequencyBandName= " << sFrequencyBandName << std::endl;
}

void CBandFrequencyAverage::setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart)
{
	m_pFrequencyBandStart[ui32FrequencyBandIndex] = f64FrequencyBandStart;
	//~ cout << "index= " << ui32FrequencyBandIndex << ", f64FrequencyBandStart= " << f64FrequencyBandStart << std::endl;
}

void CBandFrequencyAverage::setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop)
{
	m_pFrequencyBandStop[ui32FrequencyBandIndex] = f64FrequencyBandStop;
	//~ cout << "index= " << ui32FrequencyBandIndex << ",  f64FrequencyBandStop= " <<  f64FrequencyBandStop << std::endl;
}

void CBandFrequencyAverage::setBuffer(const float64* pBuffer)
{
	m_pInputBuffer = pBuffer;
}

void CBandFrequencyAverage::setFeatureVector(const float64* pBuffer)
{
	m_pInputBuffer = pBuffer;
}

void CBandFrequencyAverage::writeFeatureVectorOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

boolean CBandFrequencyAverage::process()
{
	IBoxIO * l_pDynamicContext = getBoxAlgorithmContext()->getDynamicBoxContext();
	uint32 l_ui32InputChunkCount = l_pDynamicContext->getInputChunkCount(0);
	char l_sFeatureVectorLabel[1024];

	for(uint32 i = 0; i < l_ui32InputChunkCount; i++)
    {
		uint64 l_ui64ChunkSize = 0;
		const uint8 * l_pChunkBuffer = NULL;

		if(l_pDynamicContext->getInputChunk(0, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pChunkBuffer))
		{
			m_pSpectrumReader->processData(l_pChunkBuffer, l_ui64ChunkSize);

			if (m_pInputBuffer == NULL)
			{
				if (!m_bCoefComputed)
				{
					// Determine the IndexLowFrequency
					uint32 l_ui32FrequencyBandIndex = 0;
					while (m_pFrequencyBandStart[l_ui32FrequencyBandIndex] < (float64)m_ui32LowFrequency && l_ui32FrequencyBandIndex< m_ui32FrequencyBandCount)
					{
						l_ui32FrequencyBandIndex++;
					}
					m_ui32IndexLowFrequency = l_ui32FrequencyBandIndex-1;
					//~ cout << "m_ui32IndexLowFrequency = " << m_ui32IndexLowFrequency << std::endl;
					// Determine the IndexHighFrequency
					l_ui32FrequencyBandIndex = 0;
					while (m_pFrequencyBandStop[l_ui32FrequencyBandIndex] < (float64)m_ui32HighFrequency&& l_ui32FrequencyBandIndex< m_ui32FrequencyBandCount)
					{
						l_ui32FrequencyBandIndex++;
					}
					m_ui32IndexHighFrequency = l_ui32FrequencyBandIndex;
					//~ cout << "m_ui32IndexHighFrequency = " << m_ui32IndexHighFrequency << std::endl;

					m_ui32VectorSize = 1;
					m_bCoefComputed = true;
				}

				m_pFeatureVectorOutputWriterHelper->setFeatureCount(m_ui32VectorSize);
				for (uint32 j=0; j < m_ui32VectorSize; j++)
				{
					sprintf(l_sFeatureVectorLabel, "%u", j);
					m_pFeatureVectorOutputWriterHelper->setFeatureName(j, l_sFeatureVectorLabel);
				}

				m_pFeatureVectorOutputWriterHelper->writeHeader(*m_pWriter);
				l_pDynamicContext->markOutputAsReadyToSend(0,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				m_pOutputBuffer = new float64[m_ui32VectorSize];
				m_pFeatureVectorOutputWriterHelper->setFeatureVector(m_pOutputBuffer);
			}
			else
			{
				float64 l_f64FrequenciesSum = 0;
				//for each channel
				for(uint32 c=0 ; c<m_ui32ChannelCount ; c++)
				{
					//sum its samples
					for(uint64 i=m_ui32IndexLowFrequency ; i<=m_ui32IndexHighFrequency ; i++)
					{
						l_f64FrequenciesSum += m_pInputBuffer[(c*m_ui32FrequencyBandCount)+i];
					}
				}
				m_pOutputBuffer[0] = l_f64FrequenciesSum / (((float64)m_ui32IndexHighFrequency-(float64)m_ui32IndexLowFrequency+1)*(float64)m_ui32ChannelCount);
				m_pFeatureVectorOutputWriterHelper->writeBuffer(*m_pWriter);

				l_pDynamicContext->markOutputAsReadyToSend(0,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			}
			l_pDynamicContext->markInputAsDeprecated(0,i);
		}
	}
	return true;
}

