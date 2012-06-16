#include "ovpCGDFFileReader.h"

#include <system/Memory.h>
#include <cmath>

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

using namespace OpenViBEToolkit;
using namespace std;

// template<> OpenViBE::float64 OpenViBEPlugins::FileIO::CGDFFileReader::GDFTypeToFloat64<OpenViBE::float32>(OpenViBE::float32 val, OpenViBE::uint32 ui32Channel) { std::cout << "specialized 1\n"; return val; }
// template<> OpenViBE::float64 OpenViBEPlugins::FileIO::CGDFFileReader::GDFTypeToFloat64<OpenViBE::float64>(OpenViBE::float64 val, OpenViBE::uint32 ui32Channel) { std::cout << "specialized 2\n"; return val; }

#define _NoValueI_ 0xffffffff
#define _NoValueS_ "_unspecified_"

//Writer Methods
void CGDFFileReader::writeExperimentOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<GDFReader_ExperimentInfoOutput>(pBuffer, ui64BufferSize);
}

void CGDFFileReader::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<GDFReader_SignalOutput>(pBuffer, ui64BufferSize);
}

void CGDFFileReader::writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<GDFReader_StimulationOutput>(pBuffer, ui64BufferSize);
}

//Plugin Methods
CGDFFileReader::CGDFFileReader(void):
	m_bErrorOccured(false),
	m_ui64FileSize(0),
	m_f32FileVersion(-1),
	m_pSignalOutputWriterHelper(NULL),
	m_pExperimentInformationOutputWriterHelper(NULL),
	m_ui32SamplesPerBuffer(0),
	m_ui64NumberOfDataRecords(0),
	m_f64DurationOfDataRecord(0),
	m_ui16NumberOfChannels(0),
	m_ui32NumberOfSamplesPerRecord(0),
	m_pChannelType(NULL),
	m_pChannelDataSize(NULL),
	m_pChannelScale(NULL),
	m_pChannelTranslate(NULL),
	m_ui64DataRecordSize(0),
	m_pDataRecordBuffer(NULL),
	m_pChannelDataInDataRecord(NULL),
	m_pMatrixBuffer(NULL),
	m_ui64MatrixBufferSize(0),
	m_bMatricesSent(false),
	m_ui32SentSampleCount(0),
	m_ui64CurrentDataRecord(0),
	m_ui32CurrentSampleInDataRecord(0),
	m_ui8EventTableMode(0),
	m_ui32NumberOfEvents(0),
	m_pEventsPositionBuffer(NULL),
	m_pEventsTypeBuffer(NULL),
	m_ui32CurrentEvent(0),
	m_bEventsSent(false),
	m_ui64StimulationPerBuffer(32),
	m_pExperimentInfoHeader(NULL),
	m_bExperimentInformationSent(false),
	m_bSignalDescriptionSent(false),
	m_ui64ClockFrequency(0)
{
}

void CGDFFileReader::release(void)
{
}

boolean CGDFFileReader::initialize()
{
	const IBox* l_pBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();

	// Gets the size of output buffers
	CString l_sParam;
	l_pBoxContext->getSettingValue(1, l_sParam);
	m_ui32SamplesPerBuffer = static_cast<uint32>(atoi((const char*)l_sParam));

	//Prepares the writers proxies
	m_pOutputWriterCallbackProxy[GDFReader_ExperimentInfoOutput] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::FileIO::CGDFFileReader>(*this, &CGDFFileReader::writeExperimentOutput);

	m_pOutputWriterCallbackProxy[GDFReader_SignalOutput] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::FileIO::CGDFFileReader>(*this, &CGDFFileReader::writeSignalOutput);

	m_pOutputWriterCallbackProxy[GDFReader_StimulationOutput] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::FileIO::CGDFFileReader>(*this, &CGDFFileReader::writeStimulationOutput);

	for(int i=0 ; i<3 ; i++)
	{
		m_pWriter[i]=EBML::createWriter(*m_pOutputWriterCallbackProxy[i]);
	}

	// Prepares writer helpers
	m_pExperimentInformationOutputWriterHelper=createBoxAlgorithmExperimentInformationOutputWriter();
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pStimulationOutputWriterHelper=createBoxAlgorithmStimulationOutputWriter();

	//allocate the structure used to store the experiment information
	m_pExperimentInfoHeader = new CExperimentInfoHeader;

	// Parses box settings to find filename
	l_pBoxContext->getSettingValue(0, m_sFileName);

	//opens the gdf file
	if(m_sFileName != CString(""))
	{
		m_oFile.open(m_sFileName.toASCIIString(), ios::binary);
		if(!m_oFile.good())
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << m_sFileName << "]\n";
			return false;
		}
	}

	m_oFile.seekg(0, ios::end);
	m_ui64FileSize = (uint64)m_oFile.tellg();
	m_oFile.seekg(0, ios::beg);

	//reads the gdf headers and sends the corresponding buffers
	return readFileHeader();
}

boolean CGDFFileReader::uninitialize()
{
	// Cleans up EBML writers
	for(int i=0 ; i<3 ; i++)
	{
		delete m_pOutputWriterCallbackProxy[i];
		m_pOutputWriterCallbackProxy[i] = NULL;
		m_pWriter[i]->release();
		m_pWriter[i] = NULL;
	}

	releaseBoxAlgorithmExperimentInformationOutputWriter(m_pExperimentInformationOutputWriterHelper);
	m_pExperimentInformationOutputWriterHelper=NULL;

	//desallocate the signal output writer helper
	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;

	//desallocate the stimulation output writer helper
	releaseBoxAlgorithmStimulationOutputWriter(m_pStimulationOutputWriterHelper);
	m_pStimulationOutputWriterHelper=NULL;

	//desallocate all of the remaining buffers
	if(m_pChannelDataSize) delete[] m_pChannelDataSize;	//can be done before?
	if(m_pChannelType) delete[] m_pChannelType;	//can be done before?
	if(m_pChannelScale) delete[] m_pChannelScale;
	if(m_pChannelTranslate) delete[] m_pChannelTranslate;
	if(m_pDataRecordBuffer) delete[] m_pDataRecordBuffer;	//can be done before??
	if(m_pChannelDataInDataRecord) delete[] m_pChannelDataInDataRecord;
	if(m_pMatrixBuffer) delete[] m_pMatrixBuffer;		//can be done before?
	if(m_pEventsPositionBuffer) delete[] m_pEventsPositionBuffer;
	if(m_pEventsTypeBuffer) delete[] m_pEventsTypeBuffer;

	//Close the GDF file
	if(m_oFile)
	{
		m_oFile.close();
	}

	return true;
}

boolean CGDFFileReader::processClock(CMessageClock& rMessageClock)
{
	if(rMessageClock.getTime() > (((uint64)(m_ui32SentSampleCount+m_pSignalDescription.m_ui32SampleCount))<<32)/m_pSignalDescription.m_ui32SamplingRate)
	{
		getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	}
	return true;
}

void CGDFFileReader::GDFBufferToFloat64Buffer(float64 * out, void * in, uint64 inputBufferSize, uint32 ui32Channel)
{
	switch (m_pChannelType[ui32Channel])
	{
		case GDF::ChannelType_int8 :
			GDFTypeBufferToFloat64Buffer<int8>(out, reinterpret_cast<int8 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_uint8 :
			GDFTypeBufferToFloat64Buffer<uint8>(out, reinterpret_cast<uint8 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_int16 :
			GDFTypeBufferToFloat64Buffer<int16>(out, reinterpret_cast<int16 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_uint16 :
			GDFTypeBufferToFloat64Buffer<uint16>(out, reinterpret_cast<uint16 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_int32 :
			GDFTypeBufferToFloat64Buffer<int32>(out, reinterpret_cast<int32 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_uint32 :
			GDFTypeBufferToFloat64Buffer<uint32>(out, reinterpret_cast<uint32 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_int64 :
			GDFTypeBufferToFloat64Buffer<int64>(out, reinterpret_cast<int64 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_uint64 :
			GDFTypeBufferToFloat64Buffer<uint64>(out, reinterpret_cast<uint64 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_float32 :
			GDFTypeBufferToFloat64Buffer<float32>(out, reinterpret_cast<float32 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_float64 :
			GDFTypeBufferToFloat64Buffer<float64>(out, reinterpret_cast<float64 *>(in), inputBufferSize, ui32Channel);
		break;

		case GDF::ChannelType_float128 :
		{
			//Not handled
			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "This data type is currently not handled : float128.\n";
			m_bErrorOccured = true;
		}
		break;

		case GDF::ChannelType_int24 :
		{
			uint8 * l_pIn = reinterpret_cast<uint8 *>(in);

			for(uint64 i = 0 ; i<(inputBufferSize*3) ; i+=3)
			{
				out[i] = GDFTypeToFloat64(l_pIn[i] + (l_pIn[i+1]<<8) + (l_pIn[i+2]<<16), ui32Channel);
			}
		}
		break;

		case GDF::ChannelType_uint24 :
		{
			int8 * l_pIn = reinterpret_cast<int8 *>(in);

			for(uint64 i = 0 ; i<(inputBufferSize*3) ; i+=3)
			{
				out[i] =GDFTypeToFloat64(l_pIn[i] + (l_pIn[i+1]<<8) + (l_pIn[i+2]<<16), ui32Channel);
			}
		}
		break;

		default:
			//not handled
			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Invalid GDF data type!\n";
			m_bErrorOccured = true;
			break;
	}
}

boolean CGDFFileReader::readFileHeader()
{

	IBoxIO * l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

	if(!m_bExperimentInformationSent)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Trace <<"Reading experiment information\n";

		//First reads the file type
		char l_pFileType[3];
		char l_pFileVersion[5];
		m_oFile.read(l_pFileType, 3);

		//if not a gdf file
		if(strncmp(l_pFileType,"GDF", 3) != 0)
		{
			//Handle error
			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning <<"This is not a valid GDF File!\n";
			m_bErrorOccured = true;
			return false;
		}

		m_oFile.read(l_pFileVersion, 5);
		m_f32FileVersion = (float32)atof(l_pFileVersion);

		if(m_oFile.bad())
		{
			//Handle error
			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error while reading file.\n";
			m_bErrorOccured = true;
			return false;
		}

		if(m_f32FileVersion < 3)
		{
			GDF::CFixedGDFHeader * l_oFixedHeader = NULL;

			if(m_f32FileVersion > 1.90)
			{
				l_oFixedHeader = new GDF::CFixedGDF2Header;
			}
			else
			{
				l_oFixedHeader = new GDF::CFixedGDF1Header;
			}

			if(!l_oFixedHeader->read(m_oFile))
			{
				m_bErrorOccured = true;
				delete l_oFixedHeader;
				return false;
			}

			m_pExperimentInfoHeader->m_ui64ExperimentId = l_oFixedHeader->getExperimentIdentifier();
			m_pExperimentInfoHeader->m_sExperimentDate = l_oFixedHeader->getExperimentDate();
			m_pExperimentInfoHeader->m_ui64SubjectId = l_oFixedHeader->getSubjectIdentifier();
			m_pExperimentInfoHeader->m_sSubjectName = l_oFixedHeader->getSubjectName();
			m_pExperimentInfoHeader->m_ui64SubjectAge = l_oFixedHeader->getSubjectAge();
			m_pExperimentInfoHeader->m_ui64SubjectSex = l_oFixedHeader->getSubjectSex();

			m_pExperimentInfoHeader->m_ui64LaboratoryId = l_oFixedHeader->getLaboratoryIdentifier();
			m_pExperimentInfoHeader->m_sLaboratoryName = l_oFixedHeader->getLaboratoryName();
			m_pExperimentInfoHeader->m_ui64TechnicianId = l_oFixedHeader->getTechnicianIdentifier();
			m_pExperimentInfoHeader->m_sTechnicianName = l_oFixedHeader->getTechnicianName();

			//Experiment header ready to send now
			m_pExperimentInfoHeader->m_bReadyToSend = true;

			m_f64DurationOfDataRecord = l_oFixedHeader->getDataRecordDuration();
			m_ui64NumberOfDataRecords= l_oFixedHeader->getNumberOfDataRecords();

			//this information is related to the signal
			m_ui16NumberOfChannels = (uint16)l_oFixedHeader->getChannelCount();
			m_pSignalDescription.m_ui32ChannelCount = m_ui16NumberOfChannels;

			//Send the header
			writeExperimentInformation();

			l_pBoxIO->markOutputAsReadyToSend(GDFReader_ExperimentInfoOutput, 0, 0);
			m_bExperimentInformationSent=true;

			//not needed anymore
			delete l_oFixedHeader;
			delete m_pExperimentInfoHeader;
			m_pExperimentInfoHeader=NULL;
		}
		else
		{
			//Not a known GDF File version
			//Error handling
			m_bErrorOccured = true;
		}
	}//END of ExperimentHeader

	if (!m_bSignalDescriptionSent)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Trace <<"Reading signal description\n";

		//reads the whole variable header
		char * l_pVariableHeaderBuffer = new char[m_ui16NumberOfChannels*256];
		m_oFile.read(l_pVariableHeaderBuffer, m_ui16NumberOfChannels*256);

		if(m_oFile.bad())
		{
			//Handle error
			m_bErrorOccured = true;
			return false;
		}

		m_pSignalDescription.m_pChannelName.resize(m_ui16NumberOfChannels);

		//channel's signal gain/translation
		m_pChannelScale = new float64[m_ui16NumberOfChannels];
		m_pChannelTranslate = new float64[m_ui16NumberOfChannels];

		float64 * l_pPhysicalMinimun = reinterpret_cast<float64*>(l_pVariableHeaderBuffer+(104*m_ui16NumberOfChannels));

		float64 * l_pPhysicalMaximun = reinterpret_cast<float64*>(l_pVariableHeaderBuffer+(112*m_ui16NumberOfChannels));

		int64 * l_pDigitalMinimun = reinterpret_cast<int64*>(l_pVariableHeaderBuffer+(120*m_ui16NumberOfChannels));

		int64 * l_pDigitalMaximun = reinterpret_cast<int64*>(l_pVariableHeaderBuffer+(128*m_ui16NumberOfChannels));

		for(int i=0 ; i<m_ui16NumberOfChannels ; i++)
		{
			m_pChannelScale[i] = (l_pPhysicalMaximun[i]-l_pPhysicalMinimun[i])/(l_pDigitalMaximun[i]-l_pDigitalMinimun[i]);

			m_pChannelTranslate[i] = (l_pPhysicalMaximun[i]+l_pPhysicalMinimun[i])/2;
		}

		//Check if all the channels have the same sampling rate
		uint32 * l_pNumberOfSamplesPerRecordArray = reinterpret_cast<uint32*>(l_pVariableHeaderBuffer+(216*m_ui16NumberOfChannels));

		m_ui32NumberOfSamplesPerRecord = l_pNumberOfSamplesPerRecordArray[0];

		for(int i=1 ; i<m_ui16NumberOfChannels ; i++)
		{
			//If all the channels don't have the same sampling rate
			if(m_ui32NumberOfSamplesPerRecord != l_pNumberOfSamplesPerRecordArray[i])
			{
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Can't handle GDF files with channels having different sampling rates!\n";

				m_bErrorOccured = true;
				return false;
			}
		}

		//type of the channels' data
		m_pChannelType = new uint32[m_ui16NumberOfChannels];
		memcpy(m_pChannelType, l_pVariableHeaderBuffer+ (220*m_ui16NumberOfChannels), m_ui16NumberOfChannels * 4);

		m_pChannelDataSize = new uint16[m_ui16NumberOfChannels];

		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Debug <<"Found " << m_ui16NumberOfChannels << " channels...\n";
		for(int i=0 ; i<m_ui16NumberOfChannels ; i++)
		{
			//Find the data size for each channel
			//TODO use enum to specify each type's name
			m_pChannelDataSize[i] = GDF::GDFDataSize(m_pChannelType[i]);

			//Here, we can compute the size of a data record, based on the type of each channel
			m_ui64DataRecordSize += m_ui32NumberOfSamplesPerRecord*m_pChannelDataSize[i];

			//reads the channels names
			m_pSignalDescription.m_pChannelName[i].assign(l_pVariableHeaderBuffer + (16*i), 16);

			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Debug <<" * Channel " << (uint32)i << " : " << CString(m_pSignalDescription.m_pChannelName[i].c_str()) << "\n";
		}

		//This parameter is defined by the user of the plugin
		m_pSignalDescription.m_ui32SampleCount = static_cast<EBML::uint32>(m_ui32SamplesPerBuffer);

		//needs to be computed based on the duration of a data record and the number of samples in one of those data records
		m_pSignalDescription.m_ui32SamplingRate = static_cast<EBML::uint32>(0.5 + (m_ui32NumberOfSamplesPerRecord/m_f64DurationOfDataRecord));

		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Debug <<"Sample count per buffer : " << m_ui32SamplesPerBuffer << "\n";
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Debug <<"Sampling rate : " << m_pSignalDescription.m_ui32SamplingRate << "\n";

		//computes clock frequency
		if(m_ui32SamplesPerBuffer <= m_pSignalDescription.m_ui32SamplingRate)
		{
			if(m_pSignalDescription.m_ui32SamplingRate % m_ui32SamplesPerBuffer != 0)
			{
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning
					<< "The sampling rate isn't a multiple of the buffer size\n";
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning
					<< "Please consider adjusting the GDFReader settings to correct this!\n";
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning
					<< "Sampling rate was " << m_pSignalDescription.m_ui32SamplingRate << "\n";
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning
					<< "Buffer size was " << m_ui32SamplesPerBuffer << "\n";
			}

			m_ui64ClockFrequency = ( ((uint64)m_pSignalDescription.m_ui32SamplingRate<<32) / (uint64)m_ui32SamplesPerBuffer);
		}

		//Send the data to the output
		writeSignalInformation();
		l_pBoxIO->markOutputAsReadyToSend(GDFReader_SignalOutput, 0, 0);

		delete[] l_pVariableHeaderBuffer;

		m_bSignalDescriptionSent = true;

	}//END of SignalHeader

	return true;
}

void CGDFFileReader::writeExperimentInformation()
{
	if(m_pExperimentInfoHeader->m_ui64ExperimentId != _NoValueI_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_ExperimentIdentifier, (uint32)m_pExperimentInfoHeader->m_ui64ExperimentId);
	}

	if(m_pExperimentInfoHeader->m_sExperimentDate != _NoValueS_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_ExperimentDate, m_pExperimentInfoHeader->m_sExperimentDate.c_str());
	}

	if(m_pExperimentInfoHeader->m_ui64SubjectId != _NoValueI_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectIdentifier, (uint32) m_pExperimentInfoHeader->m_ui64SubjectId);
	}

	if(m_pExperimentInfoHeader->m_sSubjectName != _NoValueS_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectName, m_pExperimentInfoHeader->m_sSubjectName.c_str());
	}

	if(m_pExperimentInfoHeader->m_ui64SubjectAge != _NoValueI_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectAge, (uint32)m_pExperimentInfoHeader->m_ui64SubjectAge);
	}

	if(m_pExperimentInfoHeader->m_ui64SubjectSex != _NoValueI_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectSex, (uint32)m_pExperimentInfoHeader->m_ui64SubjectSex);
	}

	if(m_pExperimentInfoHeader->m_ui64LaboratoryId != _NoValueI_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_LaboratoryIdentifier, (uint32)m_pExperimentInfoHeader->m_ui64LaboratoryId);
	}

	if(m_pExperimentInfoHeader->m_sLaboratoryName != _NoValueS_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_LaboratoryName, m_pExperimentInfoHeader->m_sLaboratoryName.c_str());
	}

	if(m_pExperimentInfoHeader->m_ui64TechnicianId != _NoValueI_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_TechnicianIdentifier, (uint32)m_pExperimentInfoHeader->m_ui64TechnicianId);
	}

	if(m_pExperimentInfoHeader->m_sTechnicianName != _NoValueS_)
	{
		m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_TechnicianName, m_pExperimentInfoHeader->m_sTechnicianName.c_str());
	}

	m_pExperimentInformationOutputWriterHelper->writeHeader(*m_pWriter[GDFReader_ExperimentInfoOutput]);
}

void CGDFFileReader::writeSignalInformation()
{
	m_pSignalOutputWriterHelper->setSamplingRate(m_pSignalDescription.m_ui32SamplingRate);
	m_pSignalOutputWriterHelper->setChannelCount(m_pSignalDescription.m_ui32ChannelCount);

	for(uint32 i=0 ; i<m_pSignalDescription.m_ui32ChannelCount ; i++)
	{
		m_pSignalOutputWriterHelper->setChannelName(i, m_pSignalDescription.m_pChannelName[i].c_str());
	}

	m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_pSignalDescription.m_ui32SampleCount);

	m_pSignalOutputWriterHelper->writeHeader(*m_pWriter[GDFReader_SignalOutput]);
}

void CGDFFileReader::writeEvents()
{
	m_pStimulationOutputWriterHelper->setStimulationCount(m_oEvents.size());

	uint64 l_ui64EventDate = 0;

	for(size_t i=0 ; i<m_oEvents.size() ; i++)
	{
		//compute date
		l_ui64EventDate = ( ((uint64)(m_oEvents[i].m_ui32Position)) <<32)/m_pSignalDescription.m_ui32SamplingRate;
		m_pStimulationOutputWriterHelper->setStimulation(i, m_oEvents[i].m_ui16Type, l_ui64EventDate);
	}

	m_pStimulationOutputWriterHelper->writeBuffer(*m_pWriter[GDFReader_StimulationOutput]);
}

boolean CGDFFileReader::process()
{
	//Don't do anything if an error as occured while reading the input file
	//for instance, if the file has channels with different sampling rates
	if(m_bErrorOccured)
	{
		return false;
	}

	uint64 l_ui64StartTime=0;
	uint64 l_ui64EndTime=0;

	IBoxIO * l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process Matrices
	if(m_bSignalDescriptionSent && !m_bMatricesSent)
	{

		//If the matrix buffer is not allocated yet
		//"first time"
		if(!m_pMatrixBuffer)
		{
			//output matrix buffer
			m_ui64MatrixBufferSize = m_pSignalDescription.m_ui32SampleCount*m_pSignalDescription.m_ui32ChannelCount;
			m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];

			//Associate this buffer to the signal output writer helper
			m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

			//We also have to read the first data record
			m_pDataRecordBuffer = new uint8[(size_t)m_ui64DataRecordSize];
			m_oFile.read(reinterpret_cast<char*>(m_pDataRecordBuffer), (std::streamsize)m_ui64DataRecordSize);

			if(m_oFile.bad())
			{
				//Handle error
				m_bErrorOccured = true;
				return false;
			}

			//initialize subpointers
			m_pChannelDataInDataRecord = new uint8*[m_ui16NumberOfChannels];

			m_pChannelDataInDataRecord[0] = m_pDataRecordBuffer;
			for(int i=1 ; i<m_ui16NumberOfChannels ; i++)
			{
				m_pChannelDataInDataRecord[i] = m_pChannelDataInDataRecord[i-1] + m_pChannelDataSize[i-1]*m_ui32NumberOfSamplesPerRecord;
			}
		}

		uint32 l_ui32CurrentSampleInOutputMatrix  = 0;
		boolean l_bMatrixReadyToSend = false;

		while(!l_bMatrixReadyToSend)
		{
			//there is the same number of samples
			uint64 l_ui64SamplesRemainingInDataRecord = m_ui32NumberOfSamplesPerRecord - m_ui32CurrentSampleInDataRecord;

			//If there is enough data in the current data record to read a matrix buffer
			if((m_ui32SamplesPerBuffer-l_ui32CurrentSampleInOutputMatrix) <= l_ui64SamplesRemainingInDataRecord)
			{
				for(int i=0 ; i<m_ui16NumberOfChannels ; i++)
				{
					//reads m_ui32SamplesPerBuffer samples and converts/writes them in output buffer
					GDFBufferToFloat64Buffer(m_pMatrixBuffer + (i*m_ui32SamplesPerBuffer) +l_ui32CurrentSampleInOutputMatrix,
							m_pChannelDataInDataRecord[i]+(m_ui32CurrentSampleInDataRecord*m_pChannelDataSize[i]),
							m_ui32SamplesPerBuffer - l_ui32CurrentSampleInOutputMatrix,
							i);
				}

				m_ui32CurrentSampleInDataRecord += (uint32)(m_ui32SamplesPerBuffer - l_ui32CurrentSampleInOutputMatrix);

				//Prepares for the next matrix
				l_ui32CurrentSampleInOutputMatrix = 0;

				//We can send the matrix
				l_bMatrixReadyToSend = true;
			}
			//Not enough data in the current data record. Read the remaining samples, then load a new data record and read the rest
			else
			{
				//copy what is remaining in the current buffer
				for(int i=0 ; i<m_ui16NumberOfChannels ; i++)
				{
					GDFBufferToFloat64Buffer(m_pMatrixBuffer + (i*m_ui32SamplesPerBuffer) + l_ui32CurrentSampleInOutputMatrix,
							m_pChannelDataInDataRecord[i]+(m_ui32CurrentSampleInDataRecord*m_pChannelDataSize[i]),
							l_ui64SamplesRemainingInDataRecord,
							i);
				}

				//Updates the index in the output matrix
				l_ui32CurrentSampleInOutputMatrix += (uint32)l_ui64SamplesRemainingInDataRecord;

				//reads the next data record if there is one
				if(m_ui64CurrentDataRecord < m_ui64NumberOfDataRecords-1)
				{
					//reads a data record
					m_oFile.read(reinterpret_cast<char*>(m_pDataRecordBuffer), (std::streamsize)m_ui64DataRecordSize);

					if(m_oFile.bad())
					{
						//Handle error
						m_bErrorOccured = true;
						return false;
					}

					m_ui32CurrentSampleInDataRecord = 0;
					m_ui64CurrentDataRecord++;
				}
				//if there is now more data record
				else
				{
					//we can (for instance) pad the rest of the matrix with 0s
					for(int i=0 ; i<m_ui16NumberOfChannels ; i++)
					{
						memset(m_pMatrixBuffer + (((i*m_ui32SamplesPerBuffer) +l_ui32CurrentSampleInOutputMatrix)),
								0,
								(uint32)(m_ui32SamplesPerBuffer - l_ui32CurrentSampleInOutputMatrix) * sizeof(float64));
					}

					//We can send the matrix
					l_bMatrixReadyToSend = true;
					//No more data after that
					m_bMatricesSent = true;
				}

			}

			//Check if we have finished the current data record
			if(m_ui32CurrentSampleInDataRecord == m_ui32NumberOfSamplesPerRecord)
			{
				m_ui32CurrentSampleInDataRecord = 0;
				m_ui64CurrentDataRecord++;

				//if there are no more data records
				if(m_ui64CurrentDataRecord == m_ui64NumberOfDataRecords-1)
				{
					//We don't have to read data records anymore
					m_bMatricesSent = true;
				}
				else
				{
					//reads a data record
					m_oFile.read(reinterpret_cast<char*>(m_pDataRecordBuffer), (std::streamsize)m_ui64DataRecordSize);

					if(m_oFile.bad())
					{
						//Handle error
						m_bErrorOccured = true;
						return false;
					}
				}
			}
		}

		m_ui32SentSampleCount += m_pSignalDescription.m_ui32SampleCount;

		//A signal matrix is ready to be output
		m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter[GDFReader_SignalOutput]);

		l_ui64StartTime=(((uint64)(m_ui32SentSampleCount - m_pSignalDescription.m_ui32SampleCount))<<32)/m_pSignalDescription.m_ui32SamplingRate;

		l_ui64EndTime  =(((uint64)(m_ui32SentSampleCount))<<32)/m_pSignalDescription.m_ui32SamplingRate;

		l_pBoxIO->markOutputAsReadyToSend(GDFReader_SignalOutput, l_ui64StartTime, l_ui64EndTime);

		l_bMatrixReadyToSend = false;
	}

	//Events
	if(m_bSignalDescriptionSent && !m_bEventsSent)
	{
		//reads the events table header if it hasn't been done already
		if(!m_pEventsPositionBuffer)
		{
			std::streamoff l_oBackupPosition = m_oFile.tellg();

			std::streamoff l_oEventDataPosition = (std::streamoff)((256 * (m_ui16NumberOfChannels+1)) + (m_ui64NumberOfDataRecords*m_ui64DataRecordSize));

			//checks if there are event information
			if((uint64)l_oEventDataPosition+1 < m_ui64FileSize)
			{
				m_oFile.seekg(l_oEventDataPosition);

				//reads the event table mode
				m_oFile>>m_ui8EventTableMode;
			}
			//no event information
			else
			{
				m_oFile.seekg(l_oBackupPosition);

				m_bEventsSent = true;
				return true;
			}

			uint32 l_pEventTableHeaderMain[7];
			uint8* l_pEventTableHeader=reinterpret_cast<uint8*>(l_pEventTableHeaderMain);
			System::Memory::set(l_pEventTableHeaderMain, sizeof(l_pEventTableHeaderMain), 0);
			m_oFile.read(reinterpret_cast<char*>(l_pEventTableHeader), 7);

			if(m_f32FileVersion > 1.90)
			{
				m_ui32NumberOfEvents = *(reinterpret_cast<uint32*>(l_pEventTableHeader +0));
			}
			else
			{
				m_ui32NumberOfEvents = *(reinterpret_cast<uint32*>(l_pEventTableHeader +3));
			}

			m_pEventsPositionBuffer = new uint32[m_ui32NumberOfEvents * 4];
			m_pEventsTypeBuffer = new uint16[m_ui32NumberOfEvents * 2];

			//we have to read all the events' position and type
			m_oFile.read(reinterpret_cast<char*>(m_pEventsPositionBuffer), m_ui32NumberOfEvents * 4);
			m_oFile.read(reinterpret_cast<char*>(m_pEventsTypeBuffer), m_ui32NumberOfEvents * 2);

			m_oFile.seekg(l_oBackupPosition);

			m_pStimulationOutputWriterHelper->writeHeader(*m_pWriter[GDFReader_StimulationOutput]);
			l_pBoxIO->markOutputAsReadyToSend(GDFReader_StimulationOutput, 0, 0);
		}

		GDF::CGDFEvent l_oEvent;

		//todo check inclusive/exclusive conditions
		while( (m_ui32CurrentEvent != m_ui32NumberOfEvents)  &&
			(m_pEventsPositionBuffer[m_ui32CurrentEvent]>=m_ui32SentSampleCount-m_pSignalDescription.m_ui32SampleCount) &&
			(m_pEventsPositionBuffer[m_ui32CurrentEvent]<m_ui32SentSampleCount))
		{
			//reads an event
			l_oEvent.m_ui32Position = m_pEventsPositionBuffer[m_ui32CurrentEvent];
			l_oEvent.m_ui16Type = m_pEventsTypeBuffer[m_ui32CurrentEvent];

			if(l_oEvent.m_ui32Position > 0) l_oEvent.m_ui32Position--;

			//adds it to the list of events
			m_oEvents.push_back(l_oEvent);

			m_ui32CurrentEvent++;
		}

		//if we just read the last event
		if(m_ui32CurrentEvent == m_ui32NumberOfEvents)
		{
			m_bEventsSent = true;
			delete [] m_pEventsPositionBuffer;
			m_pEventsPositionBuffer = NULL;
			delete [] m_pEventsTypeBuffer;
			m_pEventsTypeBuffer = NULL;

			//creates an end of file event
			l_oEvent.m_ui32Position = m_ui32SentSampleCount;
			l_oEvent.m_ui16Type = OVTK_StimulationId_EndOfFile;

			//adds it to the list of events
			m_oEvents.push_back(l_oEvent);
		}

		//if there is at least one event, sends it
		if(m_oEvents.size() != 0)
		{
			writeEvents();
			m_oEvents.clear();
		}

		l_pBoxIO->markOutputAsReadyToSend(GDFReader_StimulationOutput, l_ui64StartTime, l_ui64EndTime);
	}

	return true;
}
