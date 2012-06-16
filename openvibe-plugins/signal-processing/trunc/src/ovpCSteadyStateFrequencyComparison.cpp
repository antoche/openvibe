#include "ovpCSteadyStateFrequencyComparison.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;
using namespace OpenViBE::Kernel;



CSteadyStateFrequencyComparison::CSteadyStateFrequencyComparison()
	:m_pFeatureVectorReader(NULL),
	m_pFeatureVectorInputReaderCallback(NULL),
	m_oFeatureVectorReaderCallbackProxy(
		*this,
		&CSteadyStateFrequencyComparison::setFeatureCount,
		&CSteadyStateFrequencyComparison::setFeatureName,
		&CSteadyStateFrequencyComparison::setFeatureVector
	),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_oAmplitudeOutputWriterCallbackProxy(
		*this,
		&CSteadyStateFrequencyComparison::writeToAmplitudeOutput),
	m_pStreamedMatrixOutputWriterHelper(NULL),
	m_bError(false),
	m_ui32CurrentInput(0)
{
	m_pWriter = NULL;
}

void CSteadyStateFrequencyComparison::release(void)
{
	delete this;
}

boolean CSteadyStateFrequencyComparison::initialize()
{
	//reads the plugin settings
	CString l_oComparisonKind;

	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_oComparisonKind);
	m_bRatio = false;
	m_bSubstraction = false;
	m_bLateralityIndex = false;
	if (!strcmp(l_oComparisonKind, "Ratio"))
	{
		m_bRatio = true;
	}
	else if (!strcmp(l_oComparisonKind, "Substraction"))
	{
		m_bSubstraction = true;
	}
	else if (!strcmp(l_oComparisonKind, "Laterality Index"))
	{
		m_bLateralityIndex = true;
	}
	m_ui32NumberOfInput = getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount();

	// Prepares EBML reader
	m_pFeatureVectorInputReaderCallback = createBoxAlgorithmFeatureVectorInputReaderCallback(m_oFeatureVectorReaderCallbackProxy);
	m_pFeatureVectorReader=EBML::createReader(*m_pFeatureVectorInputReaderCallback);

	// Prepares EBML writer
	m_pStreamedMatrixOutputWriterHelper=createBoxAlgorithmStreamedMatrixOutputWriter();
	m_pWriter=EBML::createWriter(m_oAmplitudeOutputWriterCallbackProxy);

	m_pInputBuffer = NULL;
	m_pOutputBuffer = NULL;

	//Sends the output header
	m_pStreamedMatrixOutputWriterHelper->setDimmensionCount(1);
	m_pStreamedMatrixOutputWriterHelper->setDimmensionSize(0,1);
	m_pStreamedMatrixOutputWriterHelper->setDimmensionLabel(0, 0, "Amplitude");
	m_pStreamedMatrixOutputWriterHelper->writeHeader(*m_pWriter);
	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);

	return true;
}

OpenViBE::boolean CSteadyStateFrequencyComparison::uninitialize()
{
	// Cleans up EBML writer
	m_pWriter->release();
	m_pWriter=NULL;

	releaseBoxAlgorithmStreamedMatrixOutputWriter(m_pStreamedMatrixOutputWriterHelper);
	m_pStreamedMatrixOutputWriterHelper=NULL;

	// Cleans up EBML reader
	releaseBoxAlgorithmFeatureVectorInputReaderCallback(m_pFeatureVectorInputReaderCallback);
	m_pFeatureVectorReader->release();
	m_pFeatureVectorInputReaderCallback = NULL;
	m_pFeatureVectorReader=NULL;

	delete[] m_pOutputBuffer;

	return true;
}

OpenViBE::boolean CSteadyStateFrequencyComparison::processInput(OpenViBE::uint32 ui32InputIndex)
{
	if(m_bError)
	{
		return false;
	}

	IBoxIO* l_pBoxIO=getBoxAlgorithmContext()->getDynamicBoxContext();

	uint64 l_ui64LastBufferChunkSize;
	const uint8* l_pLastBuffer;

	uint64 l_ui64CurrentBufferChunkSize;
	const uint8* l_pCurrentBuffer;

	//gets the first buffer from the concerned input
	l_pBoxIO->getInputChunk(ui32InputIndex, 0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64LastBufferChunkSize, l_pLastBuffer);

	uint64 l_ui64StartTime = 0;
	uint64 l_ui64EndTime = 0;

	boolean l_bReadyToProcess = true;

	//checks every input's first chunk's dates
	for(uint32 i=0 ; i<m_ui32NumberOfInput && l_bReadyToProcess ; i++)
	{
		if(l_pBoxIO->getInputChunkCount(i) != 0)
		{
			l_pBoxIO->getInputChunk(i, 0, l_ui64StartTime, l_ui64EndTime, l_ui64CurrentBufferChunkSize, l_pCurrentBuffer);
			//if the first buffers don't have the same starting/ending dates, stop
			if(l_ui64StartTime != m_ui64LastChunkStartTime || l_ui64EndTime != m_ui64LastChunkEndTime)
			{
				l_bReadyToProcess = false;
			}

			//checks for problems, buffer lengths differents...
			if(l_ui64EndTime-l_ui64StartTime != m_ui64LastChunkEndTime-m_ui64LastChunkStartTime)
			{
				//marks everything as deprecated and sends a warning
				for(uint32 input=0 ; input<m_ui32NumberOfInput ; input++)
				{
					for(uint32 chunk=0 ; chunk<l_pBoxIO->getInputChunkCount(input) ; chunk++)
					{
						l_pBoxIO->markInputAsDeprecated(input, chunk);
					}
				}

				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Problem with incoming input chunks' time lengths (different)\n";

				l_bReadyToProcess = false;
				m_bError = true;
				return false;
			}
		}
		else
		{
			l_bReadyToProcess = false;
		}
	}

	//If there is one buffer of the same time period per input, process
	if(l_bReadyToProcess)
	{
		getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	}

	return true;
}

void CSteadyStateFrequencyComparison::setFeatureCount(const OpenViBE::uint32 ui32FeatureCount)
{

}

void CSteadyStateFrequencyComparison::setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName)
{
	/* No need for this */
}

void CSteadyStateFrequencyComparison::setFeatureVector(const OpenViBE::float64* pFeatureBuffer)
{
	if(m_bError)
	{
		return;
	}

	if (m_ui32CurrentInput == 0)
	{
		m_float64Amplitude = pFeatureBuffer[0];
	}

	if (m_ui32CurrentInput == m_ui32NumberOfInput-1)
	{
		float64 l_float64Amplitude = m_float64Amplitude;
		//~ cout << "l_float64Amplitude1 = " << l_float64Amplitude << ", l_float64Amplitude2 = " << pFeatureBuffer[0]<< std::endl;
		if (m_bSubstraction)
		{
			m_float64Amplitude = (pFeatureBuffer[0]-l_float64Amplitude);
		}
		else if (m_bRatio)
		{
			m_float64Amplitude = (pFeatureBuffer[0]/l_float64Amplitude);
		}
		else if (m_bLateralityIndex)
		{
			m_float64Amplitude = (l_float64Amplitude-pFeatureBuffer[0])/(pFeatureBuffer[0]+l_float64Amplitude);
		}
		//~ cout << "m_float64Amplitude = " << m_float64Amplitude<< std::endl;
		m_pStreamedMatrixOutputWriterHelper->setBuffer(&m_float64Amplitude);
		m_pStreamedMatrixOutputWriterHelper->writeBuffer(*m_pWriter);
		getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
	}
}

void CSteadyStateFrequencyComparison::writeToAmplitudeOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

OpenViBE::boolean CSteadyStateFrequencyComparison::process()
{
	IBoxIO* l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

	//process the first buffer of every input. We are sure there is one else process wouldn't have been called
	for(m_ui32CurrentInput=0 ; m_ui32CurrentInput<m_ui32NumberOfInput ; m_ui32CurrentInput++)
	{
		uint64 l_ui64ChunkSize;
		const uint8* l_pBuffer;

		l_pBoxIO->getInputChunk(m_ui32CurrentInput, 0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer);
			l_pBoxIO->markInputAsDeprecated(m_ui32CurrentInput, 0);
			m_pFeatureVectorReader->processData(l_pBuffer, l_ui64ChunkSize);
	}

	return true;
}




