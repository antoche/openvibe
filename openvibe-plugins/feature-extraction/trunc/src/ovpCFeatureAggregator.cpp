#include "ovpCFeatureAggregator.h"

#include <system/Memory.h>

#include <iostream>
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FeatureExtraction;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace FeatureExtraction
	{

		void CFeatureAggregator::setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)
		{
			//resizes to the number of dimmensions for the current input
			m_oDimmensionSize[m_ui32CurrentInput].resize(ui32DimmensionCount);
			m_oFeatureNames[m_ui32CurrentInput].resize(ui32DimmensionCount);

			// initilizes the total input buffer size
			m_oInputBufferSizes[m_ui32CurrentInput] = 1;
		}

		void CFeatureAggregator::setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)
		{
			//sets this dimmension size
			m_oDimmensionSize[m_ui32CurrentInput][ui32DimmensionIndex] = ui32DimmensionSize;

			m_oFeatureNames[m_ui32CurrentInput][ui32DimmensionIndex].resize(ui32DimmensionSize);

			// taking the dimmension size into account in the input buffer size
			m_oInputBufferSizes[m_ui32CurrentInput] *= ui32DimmensionSize;


			//if it's the last input
			if(m_ui32CurrentInput == m_ui32NumberOfInput-1 && ui32DimmensionIndex == m_oDimmensionSize[m_ui32CurrentInput].size()-1)
			{
				//computes the feature vector size
				for(uint32 i=0 ; i<m_oInputBufferSizes.size() ; i++)
				{
					m_ui32VectorSize+=(OpenViBE::uint32)m_oInputBufferSizes[i];
				}

				//allocates the vector
				m_pVectorBuffer = new float64[m_ui32VectorSize];

				//sends the header
				m_pFeatureVectorOutputWriterHelper->setFeatureCount(m_ui32VectorSize);

				//generates features names based on inputs dimmensions' names
				uint32 l_ui32CurrentFeature = 0;
				//for each input
				for(uint32 i=0 ; i<m_ui32NumberOfInput ; i++)
				{
					//for all the elements from that input
					for(uint32 elt=0 ; elt<m_oInputBufferSizes[i] ; elt++, l_ui32CurrentFeature++)
					{
						//creates feature's name by concatenating labels
						string l_oLabel;
						uint32 l_ui32ElementIndex = elt;

						for(int32 k=m_oDimmensionSize[i].size()-1 ; k>=0 ; k--)
						{
							l_oLabel += m_oFeatureNames[(size_t)i][(size_t)(m_oDimmensionSize[i].size()-k-1)][(size_t)(l_ui32ElementIndex / m_oDimmensionSize[i][k])];
							l_ui32ElementIndex = l_ui32ElementIndex % m_oDimmensionSize[i][k];
						}
						m_pFeatureVectorOutputWriterHelper->setFeatureName(l_ui32CurrentFeature, l_oLabel.c_str());
					}
				}
				//writes the header
				m_pFeatureVectorOutputWriterHelper->writeHeader(*m_pWriter);
				getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			}
		}

		void CFeatureAggregator::setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
		{
			m_oFeatureNames[m_ui32CurrentInput][ui32DimmensionIndex][ui32DimmensionEntryIndex] = sDimmensionLabel;
		}

		void CFeatureAggregator::setMatrixBuffer(const OpenViBE::float64* pBuffer)
		{
			float64 * l_pDestinationAddress = (m_ui32CurrentInput==0)
				? m_pVectorBuffer
				: m_pVectorBuffer + (m_ui32CurrentInput * m_oInputBufferSizes[m_ui32CurrentInput-1]);

			System::Memory::copy(l_pDestinationAddress, pBuffer, m_oInputBufferSizes[m_ui32CurrentInput] * sizeof(float64));

			//if it's the last input
			if(m_ui32CurrentInput == m_ui32NumberOfInput-1)
			{
				//sends vector
				m_pFeatureVectorOutputWriterHelper->setFeatureVector(m_pVectorBuffer);
				m_pFeatureVectorOutputWriterHelper->writeBuffer(*m_pWriter);
				getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			}
		}

		void CFeatureAggregator::writeFeatureVectorOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			appendOutputChunkData<0>(pBuffer, ui64BufferSize);
		}

		CFeatureAggregator::CFeatureAggregator(void) :
			m_pWriter(NULL),
			m_pOutputWriterCallbackProxy(NULL),
			m_pFeatureVectorOutputWriterHelper(NULL),
			m_pReader(NULL),
			m_pMatrixReaderCallBack(NULL),
			m_ui64LastChunkStartTime(0),
			m_ui64LastChunkEndTime(0),
			m_ui32CurrentInput(0),
			m_pVectorBuffer(NULL),
			m_ui32VectorSize(0),
			m_bError(false)
		{
		}

		OpenViBE::boolean CFeatureAggregator::initialize()
		{

			// Prepares EBML reader
			m_pMatrixReaderCallBack = createBoxAlgorithmStreamedMatrixInputReaderCallback(*this);
			m_pReader=EBML::createReader(*m_pMatrixReaderCallBack);

			//EBML writer, ...
			m_pOutputWriterCallbackProxy = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::FeatureExtraction::CFeatureAggregator>(*this, &CFeatureAggregator::writeFeatureVectorOutput);

			m_pWriter=EBML::createWriter(*m_pOutputWriterCallbackProxy);

			m_pFeatureVectorOutputWriterHelper=createBoxAlgorithmFeatureVectorOutputWriter();

			m_ui32NumberOfInput = getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount();

			//resizes everything as needed
			m_oInputBufferSizes.resize(m_ui32NumberOfInput);
			m_oDimmensionSize.resize(m_ui32NumberOfInput);
			m_oFeatureNames.resize(m_ui32NumberOfInput);

			return true;
		}

		OpenViBE::boolean CFeatureAggregator::uninitialize()
		{
			//Cleans up the writer ...
			delete m_pOutputWriterCallbackProxy;
			m_pOutputWriterCallbackProxy= NULL;

			if(m_pWriter)
			{
				m_pWriter->release();
				m_pWriter = NULL;
			}

			if(m_pFeatureVectorOutputWriterHelper)
			{
				releaseBoxAlgorithmFeatureVectorOutputWriter(m_pFeatureVectorOutputWriterHelper);
				m_pFeatureVectorOutputWriterHelper=NULL;
			}

			// Cleans up EBML reader
			releaseBoxAlgorithmStreamedMatrixInputReaderCallback(m_pMatrixReaderCallBack);

			m_pReader->release();
			m_pReader=NULL;

			//deletes the vector
			delete[] m_pVectorBuffer;
			m_pVectorBuffer = NULL;

			return true;
		}

		boolean CFeatureAggregator::processInput(uint32 ui32InputIndex)
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

		OpenViBE::boolean CFeatureAggregator::process()
		{
			IBoxIO* l_pBoxIO=getBoxAlgorithmContext()->getDynamicBoxContext();

			//process the first buffer of every input. We are sure there is one else process wouldn't have been called
			for(m_ui32CurrentInput=0 ; m_ui32CurrentInput<m_ui32NumberOfInput ; m_ui32CurrentInput++)
			{
					uint64 l_ui64ChunkSize;
					const uint8* l_pBuffer;

					l_pBoxIO->getInputChunk(m_ui32CurrentInput, 0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer);
					l_pBoxIO->markInputAsDeprecated(m_ui32CurrentInput, 0);
					m_pReader->processData(l_pBuffer, l_ui64ChunkSize);
			}
			return true;
		}
	};
};
