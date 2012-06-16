#include "ovpCGDFFileWriter.h"

#include <cmath>
#include <cfloat>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;
using namespace OpenViBEToolkit;
using namespace std;

void CGDFFileWriter::setChannelCount(const uint32 ui32ChannelCount)
{
	m_oFixedHeader.m_ui32NumberOfSignals = ui32ChannelCount;
	m_oFixedHeader.m_i64NumberOfBytesInHeaderRecord=(ui32ChannelCount+1)*256;

	m_oVariableHeader.setChannelCount(ui32ChannelCount);
	m_vSamples.resize(ui32ChannelCount);
	m_vSampleCount.resize(ui32ChannelCount);
}

void CGDFFileWriter::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	//prepares the variable header
	sprintf(m_oVariableHeader[ui32ChannelIndex].m_sLabel, "%s", sChannelName);
	//remove \0 character
	m_oVariableHeader[ui32ChannelIndex].m_sLabel[strlen(sChannelName)] = ' ';

	m_oVariableHeader[ui32ChannelIndex].m_ui32ChannelType=17;                //float64
	m_oVariableHeader[ui32ChannelIndex].m_ui32NumberOfSamplesInEachRecord=1;
	m_oVariableHeader[ui32ChannelIndex].m_f64PhysicalMinimum=+DBL_MAX;       //starting value(to compare later)
	m_oVariableHeader[ui32ChannelIndex].m_f64PhysicalMaximum=-DBL_MAX;       //starting value(to compare later)
	m_oVariableHeader[ui32ChannelIndex].m_i64DigitalMinimum=0x8000000000000000LL;
	m_oVariableHeader[ui32ChannelIndex].m_i64DigitalMaximum=0x7fffffffffffffffLL;

	memcpy(m_oVariableHeader[ui32ChannelIndex].m_sPhysicalDimension, "uV", sizeof("uV"));

}

void CGDFFileWriter::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	m_ui32SamplesPerChannel = ui32SampleCountPerBuffer;

	//save the fixed header
	if(m_oFixedHeader.save(m_oFile))
	{
		//save the variable header
		if(!m_oVariableHeader.save(m_oFile))
		{
			m_bError = true;
		}
	}
	else
	{
		m_bError = true;
	}

	if(m_bError)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error while writing to the output file!\n";
	}
}

void CGDFFileWriter::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_ui64SamplingFrequency = ui32SamplingFrequency;

	m_oFixedHeader.m_ui32DurationOfADataRecordNumerator=1;
	m_oFixedHeader.m_ui32DurationOfADataRecordDenominator=(uint32)m_ui64SamplingFrequency;
}

void CGDFFileWriter::setSampleBuffer(const float64* pBuffer)
{
	float64 l_f64Sample=0;

	//for each channel
	for(uint32 j=0; j<m_oFixedHeader.m_ui32NumberOfSignals ; j++)
	{

		for(uint32 i=0; i<m_ui32SamplesPerChannel; i++)
		{
			//gets a sample value
			l_f64Sample = pBuffer[j*m_ui32SamplesPerChannel+i];

			//actualize channel's digital min/max
			if(fabs(l_f64Sample) > m_oVariableHeader[j].m_f64PhysicalMaximum)
			{
				m_oVariableHeader[j].m_f64PhysicalMaximum = fabs(l_f64Sample+(1/m_f64Precision));
				m_oVariableHeader[j].m_f64PhysicalMinimum = -m_oVariableHeader[j].m_f64PhysicalMaximum;
				m_oVariableHeader[j].m_i64DigitalMaximum=static_cast<int64>(1+(ceil(fabs(l_f64Sample)*m_f64Precision)));
				m_oVariableHeader[j].m_i64DigitalMinimum=-m_oVariableHeader[j].m_i64DigitalMaximum;
			}

			//copy its current sample
			m_vSamples[j].push_back(l_f64Sample*m_f64Precision);
		}

		//updates the sample count
		m_vSampleCount[j]+=m_ui32SamplesPerChannel;
	}

	//save in the file
	saveMatrixData();

	//updates the fixed header
	m_oFixedHeader.m_i64NumberOfDataRecords=m_vSampleCount[0];
	if(m_oFixedHeader.update(m_oFile))
	{
		//updates the variable header
		if(!m_oVariableHeader.update(m_oFile))
		{
			m_bError = true;
		}
	}
	else
	{
		m_bError = true;
	}

	if(m_bError)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error while writing to the output file!\n";
	}

}

/*
* Experiment callback
*
*/
void CGDFFileWriter::setValue(const uint32 ui32ValueIdentifier, const uint64 ui64Value)
{
	switch(ui32ValueIdentifier)
	{
		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_ExperimentIdentifier:

			sprintf(m_oFixedHeader.m_sRecordingId, "0x%08X", (unsigned int)ui64Value);
			m_oFixedHeader.m_sRecordingId[10] = ' ';
			break;

		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectIdentifier:

			sprintf(m_oFixedHeader.m_sPatientId, "0x%08X ", (unsigned int)ui64Value);
			m_oFixedHeader.m_sPatientId[11] = ' ';
			break;

		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectAge:
			// TODO using the experiment date, compute the birthdate?
			break;

		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectSex:

			switch(ui64Value)
			{
				case OVTK_Value_Sex_Female:
					m_oFixedHeader.m_sPatientId[17] = 'F';
					break;

				case OVTK_Value_Sex_Male:
					m_oFixedHeader.m_sPatientId[17] = 'M';
					break;

				case OVTK_Value_Sex_Unknown:
				case OVTK_Value_Sex_NotSpecified:
				default:
					m_oFixedHeader.m_sPatientId[17] = 'X';
					break;
			}

			m_oFixedHeader.m_sPatientId[18] = ' ';
			break;

		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_LaboratoryIdentifier:
			m_oFixedHeader.m_ui64LaboratoryId = ui64Value;
			break;

		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_TechnicianIdentifier:
			m_oFixedHeader.m_ui64TechnicianId = ui64Value;
			if(!m_oFixedHeader.save(m_oFile))
			{
				m_bError = true;

				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error while writing to the output file!\n";
			}
			break;

	}
}

void CGDFFileWriter::setValue(const uint32 ui32ValueIdentifier, const char* sValue)
{
	switch(ui32ValueIdentifier)
	{
		/*
		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_ExperimentDate:
			break;
		*/

		case IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectName:
		{
			if (strlen(sValue)>0)
			{
				char * l_pFormattedSubjectName = new char[strlen(sValue)];

				strcpy(l_pFormattedSubjectName, sValue);

				char * l_pSpaceInSubjectName;
				//replaces all spaces by underscores
				while( (l_pSpaceInSubjectName = strchr(l_pFormattedSubjectName , ' ')) != NULL)
				{
					*l_pSpaceInSubjectName = '_';
				}

				sprintf(m_oFixedHeader.m_sPatientId + 31, "%s", l_pFormattedSubjectName);

				delete[] l_pFormattedSubjectName;
			}
		}
		break;
	}
}

void CGDFFileWriter::setStimulationCount(const uint32 ui32StimulationCount)
{
}

void CGDFFileWriter::setStimulation(const uint32 ui32StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate)
{
	m_oEvents.push_back(pair<uint64,uint64>(ui64StimulationIdentifier, ui64StimulationDate));
}

CGDFFileWriter::CGDFFileWriter(void) :
	m_bError(false)
{
	for(int i=0 ; i<3 ; i++)
	{
		m_pReaderCallBack[i]=NULL;
		m_pReader[i] = NULL;
	}
}

boolean CGDFFileWriter::initialize()
{
	const IBox* l_pBox=getBoxAlgorithmContext()->getStaticBoxContext();

	// Prepares EBML readers
	m_pExperimentInformationReaderCallBack=createBoxAlgorithmExperimentInformationInputReaderCallback(*this);
	m_pReader[0]=EBML::createReader(*m_pExperimentInformationReaderCallBack);

	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(*this);
	m_pReader[1]=EBML::createReader(*m_pSignalReaderCallBack);

	m_pStimulationReaderCallBack=createBoxAlgorithmStimulationInputReaderCallback(*this);
	m_pReader[2]=EBML::createReader(*m_pStimulationReaderCallBack);

	// Parses box settings to find filename
	l_pBox->getSettingValue(0, m_sFileName);

	if(!m_oFile.is_open())
	{
		m_oFile.open(m_sFileName, ios::binary | ios::trunc);

		if(!m_oFile.good())
		{
			m_bError = true;

			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << m_sFileName << "]\n";
			return false;
		}
	}

	m_f64Precision = 1000.0;

	return true;
}

boolean CGDFFileWriter::uninitialize()
{
	//update the fixed header
	if(m_vSampleCount.size() != 0)
	{
		m_oFixedHeader.m_i64NumberOfDataRecords=m_vSampleCount[0];
	}

	if(m_oFixedHeader.update(m_oFile))
	{
		//To save the Physical/Digital max/min values
		if(!m_oVariableHeader.update(m_oFile))
		{
			m_bError = true;
		}
	}
	else
	{
		m_bError = true;
	}

	//write events
	if(!m_oEvents.empty())
	{
		this->getLogManager() << LogLevel_Trace << "Saving " << (uint32)m_oEvents.size() << " events\n";
		saveEvents();
	}

	if(m_bError)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error while writing to the output file!\n";
	}

	if(m_oFile.is_open())
	{
		m_oFile.close();
	}

	releaseBoxAlgorithmExperimentInformationInputReaderCallback(m_pExperimentInformationReaderCallBack);
	releaseBoxAlgorithmSignalInputReaderCallback(m_pSignalReaderCallBack);
	releaseBoxAlgorithmStimulationInputReaderCallback(m_pStimulationReaderCallBack);

	// Cleans up EBML reader
	for(int i=0 ; i<3 ; i++)
	{
		m_pReader[i]->release();
		m_pReader[i]=NULL;
	}

	return true;
}

boolean CGDFFileWriter::processInput(uint32 ui32InputIndex)
{
	if(m_bError)
	{
		return false;
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

void CGDFFileWriter::saveMatrixData()
{
	if(!m_oFile.is_open())
	{
		m_oFile.open(m_sFileName, ios::binary | ios::trunc);

		if(!m_oFile.good())
		{
			m_bError = true;

			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << m_sFileName << "]\n";
			return;
		}
	}

	size_t i,j;
	m_oFile.seekp(0, ios::end);

	//to "convert" (if needed) the float64 in little endian format
	uint8 l_pLittleEndianBuffer[8];

	for(i=0; i<m_vSamples[0].size(); i++)
	{
		for(j=0; j<m_vSamples.size(); j++)
		{
			float64 v=m_vSamples[j][i];

			System::Memory::hostToLittleEndian(v, l_pLittleEndianBuffer);
			m_oFile.write(reinterpret_cast<char *>(l_pLittleEndianBuffer), sizeof(l_pLittleEndianBuffer));
		}
	}
	for(j=0; j<m_vSamples.size(); j++)
	{
		m_vSamples[j].clear();
	}
}

void CGDFFileWriter::saveEvents()
{
	if(!m_oFile.is_open())
	{
		m_oFile.open(m_sFileName, ios::binary | ios::trunc);

		if(!m_oFile.good())
		{
			m_bError = true;

			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << m_sFileName << "]\n";
			return;
		}
	}

	m_oFile.seekp(0, ios::end);

	//event mode
	m_oFile.put(1);

	//sample rate associated with event positions
	m_oFile.put(0);
	m_oFile.put(0);
	m_oFile.put(0);

	//number of events
	uint8 l_pLittleEndianBuffer[4];
	System::Memory::hostToLittleEndian((uint32)m_oEvents.size(), l_pLittleEndianBuffer);
	m_oFile.write(reinterpret_cast<char *>(l_pLittleEndianBuffer), sizeof(l_pLittleEndianBuffer));

	uint32 l_ui32Position;
	uint16 l_ui16Type;

	//events
	vector<pair<uint64, uint64> >::iterator l_oIterator;
	for(l_oIterator=m_oEvents.begin() ; l_oIterator!=m_oEvents.end() ; l_oIterator++)
	{
		l_ui32Position = (uint32)(((l_oIterator->second + 1) * m_ui64SamplingFrequency - 1)>>32);

		System::Memory::hostToLittleEndian(l_ui32Position+1, l_pLittleEndianBuffer);
		m_oFile.write(reinterpret_cast<char *>(l_pLittleEndianBuffer), 4);

	}

	for(l_oIterator=m_oEvents.begin() ; l_oIterator!=m_oEvents.end() ; l_oIterator++)
	{
		//Force to use only 16bits stimulations IDs
		l_ui16Type = (uint16)(l_oIterator->first&0xFFFF);

		System::Memory::hostToLittleEndian(l_ui16Type, l_pLittleEndianBuffer);
		m_oFile.write(reinterpret_cast<char *>(l_pLittleEndianBuffer), 2);
	}
}

boolean CGDFFileWriter::process()
{
	IBoxIO* l_pBoxIO=getBoxAlgorithmContext()->getDynamicBoxContext();

	for(uint32 j=0 ; j<getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount() ; j++)
	{
		for(uint32 i=0; i<l_pBoxIO->getInputChunkCount(j); i++)
		{
			uint64 l_ui64StartTime;
			uint64 l_ui64EndTime;
			uint64 l_ui64ChunkSize;
			const uint8* l_pChunkBuffer=NULL;

			if(l_pBoxIO->getInputChunk(j, i, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer))
			{
				m_pReader[j]->processData(l_pChunkBuffer, l_ui64ChunkSize);
			}
			l_pBoxIO->markInputAsDeprecated(j, i);
		}
	}

	return true;
}
