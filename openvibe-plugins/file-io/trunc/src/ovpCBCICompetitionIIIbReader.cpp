#include "ovpCBCICompetitionIIIbReader.h"

#include <iostream>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;
using namespace OpenViBEToolkit;

using namespace std;

#define BCICompetitionIIIbReader_UndefinedClass 0xFFFFFFFFFFLL
#if defined OVP_OS_Windows
#define isnan _isnan
#endif

namespace OpenViBEPlugins
{
	namespace FileIO
	{

		//Writer Methods
		void CBCICompetitionIIIbReader::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			appendOutputChunkData<0>(pBuffer, ui64BufferSize);
		}

		void CBCICompetitionIIIbReader::writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			appendOutputChunkData<1>(pBuffer, ui64BufferSize);
		}

		CBCICompetitionIIIbReader::CBCICompetitionIIIbReader()
			: m_bErrorOccured(false),
			m_ui64FileSize(0),
			m_pSignalOutputWriterHelper(NULL),
			m_pStimulationOutputWriterHelper(NULL),
			m_ui64ClockFrequency(100LL<<32),
			m_ui32SamplesPerBuffer(0),
			m_ui32SamplingRate(125),
			m_ui32SentSampleCount(0),
			m_bEndOfFile(false),
			m_ui32CurrentTrial(0)
		{
			/*TODO*/
		}

		void CBCICompetitionIIIbReader::writeSignalInformation()
		{
			m_pSignalOutputWriterHelper->setSamplingRate(125);
			m_pSignalOutputWriterHelper->setChannelCount(2);

			m_pSignalOutputWriterHelper->setChannelName(0, "+C3a-C3p");
			m_pSignalOutputWriterHelper->setChannelName(1, "+C4a-C4p");

			m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_ui32SamplesPerBuffer);

			m_pSignalOutputWriterHelper->writeHeader(*m_pWriter[0]);

			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);
		}

		OpenViBE::boolean CBCICompetitionIIIbReader::initialize()
		{
			const IBox* l_pBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();

			CString l_oParameter;

			// Parses box settings to find filename
			l_pBoxContext->getSettingValue(0, l_oParameter);

			//opens the file
			if(l_oParameter)
			{
				m_oSignalFile.open(l_oParameter);
			}
			if(!m_oSignalFile.good())
			{
				this->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << l_oParameter << "]\n";
				return false;
			}

			m_oSignalFile.seekg(0, ios::end);
			m_ui64FileSize = (uint64)m_oSignalFile.tellg();
			m_oSignalFile.seekg(0, ios::beg);

			std::string l_oLine;
			std::istringstream l_oStringStream;

			//get trial length
			l_pBoxContext->getSettingValue(10, l_oParameter);
			m_f64TrialLength = atof((const char*) l_oParameter);

			//get CUE display start
			l_pBoxContext->getSettingValue(11, l_oParameter);
			m_f64CueDisplayStart = atof((const char*) l_oParameter);

			//get feedback start
			l_pBoxContext->getSettingValue(12, l_oParameter);
			m_f64FeedbackStart = atof((const char*) l_oParameter);

			//read triggers
			std::ifstream l_oTriggerFile;
			l_pBoxContext->getSettingValue(1, l_oParameter);

			if(l_oParameter)
			{
				l_oTriggerFile.open(l_oParameter);
			}

			uint64 l_ui64Trigger;
			while(getline(l_oTriggerFile, l_oLine))
			{
				l_oStringStream.clear();
				l_oStringStream.str(l_oLine);
				l_oStringStream>>l_ui64Trigger;

				m_oTriggerTime.push_back(l_ui64Trigger);
				m_oCueDisplayStart.push_back(l_ui64Trigger + (uint64)floor(m_ui32SamplingRate * m_f64CueDisplayStart));
				m_oFeedbackStart.push_back(l_ui64Trigger + (uint64)floor(m_ui32SamplingRate * m_f64FeedbackStart));
				m_oEndOfTrial.push_back(l_ui64Trigger + (uint64)floor(m_ui32SamplingRate * m_f64TrialLength));
			}
			l_oTriggerFile.close();

			//read labels
			std::ifstream l_oLabelsFile;
			l_pBoxContext->getSettingValue(2, l_oParameter);

			if(l_oParameter)
			{
				l_oLabelsFile.open(l_oParameter);
			}

			uint64 l_ui64Label;
			while(getline(l_oLabelsFile, l_oLine))
			{
				if(l_oLine.compare(0, 3, "NaN", 0, 3) == 0)
				{
					m_oClassLabels.push_back(BCICompetitionIIIbReader_UndefinedClass);
				}
				else
				{
					l_oStringStream.clear();
					l_oStringStream.str(l_oLine);
					l_oStringStream>>l_ui64Label;
					m_oClassLabels.push_back(l_ui64Label);
				}
			}
			l_oLabelsFile.close();

			//read artifacts
			std::ifstream l_oArtifactFile;
			l_pBoxContext->getSettingValue(3, l_oParameter);

			if(l_oParameter)
			{
				l_oArtifactFile.open(l_oParameter);
			}

			uint64 l_ui64Artifact;
			while(getline(l_oArtifactFile, l_oLine))
			{
				l_oStringStream.clear();
				l_oStringStream.str(l_oLine);
				l_oStringStream>>l_ui64Artifact;
				m_oArtifacts.push_back(l_ui64Artifact == 1);
			}
			l_oArtifactFile.close();

			//read true labels
			std::ifstream l_oTrueLabelsFile;
			l_pBoxContext->getSettingValue(4, l_oParameter);
			if(l_oParameter)
			{
				l_oTrueLabelsFile.open(l_oParameter);
			}
			while(getline(l_oTrueLabelsFile, l_oLine))
			{
				l_oStringStream.clear();
				l_oStringStream.str(l_oLine);
				l_oStringStream>>l_ui64Label;
				m_oTrueLabels.push_back(l_ui64Label);
			}
			l_oTrueLabelsFile.close();

			// Gets the size of output buffers
			l_pBoxContext->getSettingValue(5, l_oParameter);
			m_ui32SamplesPerBuffer = static_cast<uint32>(atoi((const char*)l_oParameter));

			//Offline/Online
			l_pBoxContext->getSettingValue(6, l_oParameter);
			if(l_oParameter == CString("false"))
			{
				//computes clock frequency
				if(m_ui32SamplesPerBuffer <= m_ui32SamplingRate)
				{
					if(m_ui32SamplingRate % m_ui32SamplesPerBuffer != 0)
					{
						getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning <<
							"The sampling rate isn't a multiple of the buffer size\n" <<
							"Please consider adjusting the BCI Competition IIIb reader settings to correct this!\n";
					}

					m_ui64ClockFrequency = ( ((uint64)m_ui32SamplingRate<<32) / (uint64)m_ui32SamplesPerBuffer);
				}

			}

			//Test/Training
			l_pBoxContext->getSettingValue(7, l_oParameter);
			m_bKeepTrainingSamples = (l_oParameter == CString("true"));
			l_pBoxContext->getSettingValue(8, l_oParameter);
			m_bKeepTestSamples = (l_oParameter == CString("true"));
			l_pBoxContext->getSettingValue(9, l_oParameter);
			m_bKeepArtifactSamples = (l_oParameter == CString("true"));

			//Prepares the writers proxies
			m_pOutputWriterCallbackProxy[0] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::FileIO::CBCICompetitionIIIbReader>(*this, &CBCICompetitionIIIbReader::writeSignalOutput);

			m_pOutputWriterCallbackProxy[1] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::FileIO::CBCICompetitionIIIbReader>(*this, &CBCICompetitionIIIbReader::writeStimulationOutput);

			for(int i=0 ; i<2 ; i++)
			{
				m_pWriter[i]=EBML::createWriter(*m_pOutputWriterCallbackProxy[i]);
			}

			// Prepares writer helpers
			m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
			m_pStimulationOutputWriterHelper=createBoxAlgorithmStimulationOutputWriter();

			writeSignalInformation();

			m_pStimulationOutputWriterHelper->writeHeader(*m_pWriter[1]);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(1, 0, 0);

			m_oMatrixBuffer.resize(2*m_ui32SamplesPerBuffer);
			m_pSignalOutputWriterHelper->setSampleBuffer(&m_oMatrixBuffer[0]);

			return true;
		}

		OpenViBE::boolean CBCICompetitionIIIbReader::uninitialize()
		{
			// Cleans up EBML writers
			for(int i=0 ; i<2 ; i++)
			{
				delete m_pOutputWriterCallbackProxy[i];
				m_pOutputWriterCallbackProxy[i] = NULL;
				m_pWriter[i]->release();
				m_pWriter[i] = NULL;
			}

			//desallocate the signal output writer helper
			releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
			m_pSignalOutputWriterHelper=NULL;

			//desallocate the stimulation output writer helper
			releaseBoxAlgorithmStimulationOutputWriter(m_pStimulationOutputWriterHelper);
			m_pStimulationOutputWriterHelper=NULL;

			if(m_oSignalFile)
			{
				m_oSignalFile.close();
			}

			return true;
		}

		OpenViBE::boolean CBCICompetitionIIIbReader::processClock(CMessageClock& rMessageClock)
		{
			if(!m_bEndOfFile)
			{
				getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			}

			return true;
		}

		OpenViBE::boolean CBCICompetitionIIIbReader::process()
		{
			if(m_bErrorOccured)
			{
				return false;
			}

			uint64 l_ui64StartTime=0;
			uint64 l_ui64EndTime=0;

			IBoxIO * l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

			//reading signal
			//reset vector
			m_oMatrixBuffer.assign(2*m_ui32SamplesPerBuffer, 0);
			uint32 i;
			std::istringstream l_oStringStream;
			std::string l_oLine;
			float64 l_f64Sample;

			for(i=0 ; i<m_ui32SamplesPerBuffer && !m_bEndOfFile; i++)
			{
				m_bEndOfFile = (getline(m_oSignalFile, l_oLine) == NULL);

				l_oStringStream.clear();
				l_oStringStream.str(l_oLine);

				l_oStringStream>>l_f64Sample;
				if(isnan(l_f64Sample))
				{
					m_oMatrixBuffer[i] = (i != 0) ? m_oMatrixBuffer[i-1] : 0.0;
				}
				else
				{
					m_oMatrixBuffer[i] = l_f64Sample;
				}

				l_oStringStream>>l_f64Sample;
				if(isnan(l_f64Sample))
				{
					m_oMatrixBuffer[i+m_ui32SamplesPerBuffer] = (i != 0) ? m_oMatrixBuffer[i+m_ui32SamplesPerBuffer-1] : 0.0;
				}
				else
				{
					m_oMatrixBuffer[i+m_ui32SamplesPerBuffer] = l_f64Sample;
				}
			}

			m_ui32SentSampleCount += i;

			//A signal matrix is ready to be output
			m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter[0]);

			l_ui64StartTime=(((uint64)(m_ui32SentSampleCount - i))<<32)/m_ui32SamplingRate;
			l_ui64EndTime  =(((uint64)(m_ui32SentSampleCount))<<32)/m_ui32SamplingRate;

			l_pBoxIO->markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
			//////

			//Stimulations
			vector<std::pair<OpenViBE::uint64, OpenViBE::uint64> > l_oEvents;
			boolean l_bChanged = true;

			while(l_bChanged)
			{
				l_bChanged = false;

				boolean l_bKeepCurrentTrial =
					((m_oArtifacts[m_ui32CurrentTrial] && m_bKeepArtifactSamples) || !m_oArtifacts[m_ui32CurrentTrial]) &&
					((m_oClassLabels[m_ui32CurrentTrial] == BCICompetitionIIIbReader_UndefinedClass && m_bKeepTestSamples) ||
					 (m_oClassLabels[m_ui32CurrentTrial] != BCICompetitionIIIbReader_UndefinedClass && m_bKeepTrainingSamples));

				if(m_oTriggerTime[m_ui32CurrentTrial]>(m_ui32SentSampleCount-i) &&
						m_oTriggerTime[m_ui32CurrentTrial]<=m_ui32SentSampleCount
				     )
				{
					if(l_bKeepCurrentTrial)
					{
						//start of trial
						l_oEvents.push_back(pair<uint64,uint64>(0x300,m_oTriggerTime[m_ui32CurrentTrial]));

						//display cross
						l_oEvents.push_back(pair<uint64,uint64>(0x312,m_oTriggerTime[m_ui32CurrentTrial]));
					}
				}

				//send CUE stimulation
				if(m_oCueDisplayStart[m_ui32CurrentTrial]>(m_ui32SentSampleCount-i) &&
						m_oCueDisplayStart[m_ui32CurrentTrial]<=m_ui32SentSampleCount
				     )
				{
					if(l_bKeepCurrentTrial)
					{
						if(m_oClassLabels[m_ui32CurrentTrial] != BCICompetitionIIIbReader_UndefinedClass)
						{
							//send class label
							l_oEvents.push_back(pair<uint64,uint64>(0x300+m_oClassLabels[m_ui32CurrentTrial], m_oCueDisplayStart[m_ui32CurrentTrial]));
						}
						else
						{
							//send true label
							l_oEvents.push_back(pair<uint64,uint64>(0x300+m_oTrueLabels[m_ui32CurrentTrial], m_oCueDisplayStart[m_ui32CurrentTrial]));
						}
					}
				}

				//send feedback start stimulation
				if(m_oFeedbackStart[m_ui32CurrentTrial]>(m_ui32SentSampleCount-i) &&
						m_oFeedbackStart[m_ui32CurrentTrial]<=m_ui32SentSampleCount
				     )
				{
					if(l_bKeepCurrentTrial)
					{
						l_oEvents.push_back(pair<uint64,uint64>(0x30D, m_oFeedbackStart[m_ui32CurrentTrial]));
					}
				}

				//send end of trial stimulation
				if(m_oEndOfTrial[m_ui32CurrentTrial]>(m_ui32SentSampleCount-i) &&
						m_oEndOfTrial[m_ui32CurrentTrial]<=m_ui32SentSampleCount
				     )
				{
					if(l_bKeepCurrentTrial)
					{
						l_oEvents.push_back(pair<uint64,uint64>(0x320, m_oEndOfTrial[m_ui32CurrentTrial]));
					}
					m_ui32CurrentTrial++;
					l_bChanged=true;
				}
			}

			if(!l_oEvents.empty() || m_bEndOfFile)
			{
				m_pStimulationOutputWriterHelper->setStimulationCount(l_oEvents.size() + ((m_bEndOfFile)? 1 : 0) );

				uint64 l_ui64EventDate = 0;

				for(size_t j=0 ; j<l_oEvents.size() ; j++)
				{
					//compute date
					l_ui64EventDate = ( ((uint64)l_oEvents[j].second)<<32)/m_ui32SamplingRate;
					m_pStimulationOutputWriterHelper->setStimulation(j, l_oEvents[j].first, l_ui64EventDate);
				}

				//add the ending stim
				if(m_bEndOfFile)
				{
					//compute date
					l_ui64EventDate = (((uint64)m_ui32SentSampleCount)<<32) / m_ui32SamplingRate;
					m_pStimulationOutputWriterHelper->setStimulation(l_oEvents.size(), 0x3FF, l_ui64EventDate);
				}

				m_pStimulationOutputWriterHelper->writeBuffer(*m_pWriter[1]);

				l_pBoxIO->markOutputAsReadyToSend(1, l_ui64StartTime, l_ui64EndTime);
			}

			return true;
		}
	};
};
