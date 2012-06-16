#include "ovp_gdf_helpers.h"

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;
using namespace std;

#define _NoValueI_ 0xffffffff
#define _NoValueS_ "_unspecified_"

//CFIXEDHEADER methods
uint64 GDF::CFixedGDFHeader::getSubjectIdentifier()
{
	return _NoValueI_;
}

std::string GDF::CFixedGDFHeader::getSubjectName()
{
	return _NoValueS_;
}

uint64 GDF::CFixedGDFHeader::getSubjectSex()
{
	return _NoValueI_;
}

uint64 GDF::CFixedGDFHeader::getSubjectAge()
{
	return _NoValueI_;
}

uint64 GDF::CFixedGDFHeader::getExperimentIdentifier()
{
	return _NoValueI_;
}

std::string GDF::CFixedGDFHeader::getExperimentDate()
{
	return _NoValueS_;
}

uint64 GDF::CFixedGDFHeader::getLaboratoryIdentifier()
{
	return _NoValueI_;
}

uint64 GDF::CFixedGDFHeader::getTechnicianIdentifier()
{
	return _NoValueI_;
}

std::string GDF::CFixedGDFHeader::getLaboratoryName()
{
	return _NoValueS_;
}

std::string GDF::CFixedGDFHeader::getTechnicianName()
{
	return _NoValueS_;
}

GDF::CFixedGDF1Header::CFixedGDF1Header()
	:m_i64NumberOfBytesInHeaderRecord(0)
	,m_ui64EquipmentProviderId(0)
	,m_ui64LaboratoryId(0)
	,m_ui64TechnicianId(0)
	,m_i64NumberOfDataRecords(0)
	,m_ui32DurationOfADataRecordNumerator(0)
	,m_ui32DurationOfADataRecordDenominator(0)
	,m_ui32NumberOfSignals(0)
{
	memset(m_sPatientId, ' ', sizeof(m_sPatientId));
	memset(m_sRecordingId, ' ', sizeof(m_sRecordingId));
	memset(m_sStartDateAndTimeOfRecording, ' ', sizeof(m_sStartDateAndTimeOfRecording));
	memset(m_sReservedSerialNumber, ' ', sizeof(m_sReservedSerialNumber));

	//set version number
	memcpy(m_sVersionId, "GDF 1.25", 8);

	//subjectId unknown by default
	m_sPatientId[0] = 'X';
	//sex unknown by default
	m_sPatientId[17] = 'X';
}

boolean GDF::CFixedGDF1Header::read(std::ifstream& oFile)
{
	uint8 l_pHeaderBuffer[72];

	oFile.read(m_sPatientId, sizeof(m_sPatientId));
	oFile.read(m_sRecordingId, sizeof(m_sRecordingId));
	oFile.read(m_sStartDateAndTimeOfRecording, sizeof(m_sStartDateAndTimeOfRecording));

	oFile.read(reinterpret_cast<char*>(l_pHeaderBuffer), 72);

	System::Memory::littleEndianToHost(l_pHeaderBuffer,reinterpret_cast<uint64*>(&m_i64NumberOfBytesInHeaderRecord));
	System::Memory::littleEndianToHost(l_pHeaderBuffer+8,&m_ui64EquipmentProviderId);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+16,&m_ui64LaboratoryId);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+24,&m_ui64TechnicianId);

	System::Memory::littleEndianToHost(l_pHeaderBuffer+52,reinterpret_cast<uint64*>( &m_i64NumberOfDataRecords));
	System::Memory::littleEndianToHost(l_pHeaderBuffer+60,&m_ui32DurationOfADataRecordNumerator);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+64,&m_ui32DurationOfADataRecordDenominator);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+68,&m_ui32NumberOfSignals);

	if(oFile.bad())
	{
		return false;
	}
	return true;
}

boolean GDF::CFixedGDF1Header::save(std::ofstream& oFile)
{
	if(oFile.is_open())
	{
		uint8 l_pTempBuffer[sizeof(uint64)];

		oFile.seekp(0, ios::beg);

		oFile.write(m_sVersionId, sizeof(m_sVersionId));
		oFile.write(m_sPatientId, sizeof(m_sPatientId));
		oFile.write(m_sRecordingId, sizeof(m_sRecordingId));
		oFile.write(m_sStartDateAndTimeOfRecording, sizeof(m_sStartDateAndTimeOfRecording));

		System::Memory::hostToLittleEndian(static_cast<uint64>(m_i64NumberOfBytesInHeaderRecord), l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(int64));

		System::Memory::hostToLittleEndian(static_cast<uint64>(m_ui64EquipmentProviderId), l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint64));

		System::Memory::hostToLittleEndian(static_cast<uint64>(m_ui64LaboratoryId), l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint64));

		System::Memory::hostToLittleEndian(static_cast<uint64>(m_ui64TechnicianId), l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint64));

		oFile.write(m_sReservedSerialNumber, sizeof(m_sReservedSerialNumber));

		System::Memory::hostToLittleEndian(static_cast<uint64>(m_i64NumberOfDataRecords), l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(int64));

		System::Memory::hostToLittleEndian(m_ui32DurationOfADataRecordNumerator, l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint32));

		System::Memory::hostToLittleEndian(m_ui32DurationOfADataRecordDenominator, l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint32));

		System::Memory::hostToLittleEndian(m_ui32NumberOfSignals, l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint32));

		if(oFile.bad())
		{
			//cout<<"Error while writing to the output file"<<endl;
			return false;
		}

		return true;
	}

	return false;
}

boolean GDF::CFixedGDF1Header::update(std::ofstream& oFile)
{
	if(oFile.is_open())
	{
		uint8 l_pTempBuffer[sizeof(uint64)];

		uint64 l_oBackupPosition = oFile.tellp();
		oFile.seekp(236, ios::beg);

		System::Memory::hostToLittleEndian(static_cast<uint64>(m_i64NumberOfDataRecords), l_pTempBuffer);
		oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(int64));

		oFile.seekp((std::streamsize)l_oBackupPosition, ios::beg);


		if(oFile.bad())
		{
			//cout<<"Error while writing to the output file"<<endl;
			return false;
		}
		return true;
	}
	return false;
}

std::string GDF::CFixedGDF1Header::getSubjectName()
{
	// extracts the PID and Patient name from the m_sPatientId
	char * l_pToken = strtok(m_sPatientId, " ");
	if(l_pToken)
	{
		//The PId is not a numerical value in GDF, it is useless for us
		l_pToken = strtok(NULL, " ");
	}

	if(l_pToken)
	{
		return l_pToken;
	}
	else
	{
		return _NoValueS_;
	}
}

uint64 GDF::CFixedGDF1Header::getLaboratoryIdentifier()
{
	uint8 * l_pTemp = reinterpret_cast<uint8*>(&m_ui64LaboratoryId);
	boolean l_bBlank = true;
	for(int i=0 ; i<8 && l_bBlank; i++)
	{
		if(l_pTemp[i] != 0x20)
		{
			l_bBlank = false;
		}
	}

	if(l_bBlank)
	{
		return _NoValueI_;
	}
	else
	{
		return m_ui64LaboratoryId;
	}

}

uint64 GDF::CFixedGDF1Header::getTechnicianIdentifier()
{
	uint8 * l_pTemp = reinterpret_cast<uint8*>(&m_ui64TechnicianId);
	boolean l_bBlank = true;
	for(int i=0 ; i<8 && l_bBlank; i++)
	{
		if(l_pTemp[i] != 0x20)
		{
			l_bBlank = false;
		}
	}

	if(l_bBlank)
	{
		return _NoValueI_;
	}
	else
	{
		return m_ui64TechnicianId;
	}
}

float64 GDF::CFixedGDF1Header::getDataRecordDuration()
{
	return static_cast<float64>(m_ui32DurationOfADataRecordNumerator) / static_cast<float64>(m_ui32DurationOfADataRecordDenominator);
}

uint64 GDF::CFixedGDF1Header::getNumberOfDataRecords()
{
	return m_i64NumberOfDataRecords;
}

uint64 GDF::CFixedGDF1Header::getChannelCount()
{
	return m_ui32NumberOfSignals;
}

GDF::CFixedGDF2Header::CFixedGDF2Header()
	:m_ui8HealthInformation(0)
	,m_ui8Weight(0)
	,m_ui8Height(0)
	,m_ui8SubjectInformation(0)
	,m_ui16NumberOfBlocksInHeader(0)
	,m_ui64EquipmentProviderId(0)
	,m_i64NumberOfDataRecords(0)
	,m_ui32DurationOfADataRecordNumerator(0)
	,m_ui32DurationOfADataRecordDenominator(0)
	,m_ui16NumberOfSignals(0)
	,m_ui16Reserved3(0)
{
	System::Memory::set(&m_sVersionId, sizeof(m_sVersionId), 0);
	System::Memory::set(&m_sPatientId, sizeof(m_sPatientId), 0);
	System::Memory::set(&m_ui8Reserved, sizeof(m_ui8Reserved), 0);
	System::Memory::set(&m_sRecordingId, sizeof(m_sRecordingId), 0);
	System::Memory::set(&m_ui32RecordingLocation, sizeof(m_ui32RecordingLocation), 0);
	System::Memory::set(&m_ui32StartDateAndTimeOfRecording, sizeof(m_ui32StartDateAndTimeOfRecording), 0);
	System::Memory::set(&m_ui32Birthday, sizeof(m_ui32Birthday), 0);
	System::Memory::set(&m_ui8Reserved2, sizeof(m_ui8Reserved2), 0);
	System::Memory::set(&m_ui8IPAdress, sizeof(m_ui8IPAdress), 0);
	System::Memory::set(&m_ui16HeadSize, sizeof(m_ui16HeadSize), 0);
	System::Memory::set(&m_f32PositionReferenceElectrode, sizeof(m_f32PositionReferenceElectrode), 0);
	System::Memory::set(&m_f32GroundElectrode, sizeof(m_f32GroundElectrode), 0);
}

boolean GDF::CFixedGDF2Header::read(std::ifstream& oFile)
{
	uint8 l_pHeaderBuffer[104];

	oFile.read(m_sPatientId, 66);

	oFile.read(reinterpret_cast<char*>(l_pHeaderBuffer), 14);
	m_ui8HealthInformation = l_pHeaderBuffer[10];
	m_ui8Weight = l_pHeaderBuffer[11];
	m_ui8Height = l_pHeaderBuffer[12];
	m_ui8SubjectInformation = l_pHeaderBuffer[13];

	oFile.read(m_sRecordingId, 64);

	oFile.read(reinterpret_cast<char*>(l_pHeaderBuffer), 102);

	for(int i=0 ; i<4 ; i++)
	{
		System::Memory::littleEndianToHost(l_pHeaderBuffer+i*sizeof(uint32), &m_ui32RecordingLocation[i]);
	}

	System::Memory::littleEndianToHost(l_pHeaderBuffer+16, &m_ui32StartDateAndTimeOfRecording[0]);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+20, &m_ui32StartDateAndTimeOfRecording[1]);

	System::Memory::littleEndianToHost(l_pHeaderBuffer+24, &m_ui32Birthday[0]);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+28, &m_ui32Birthday[1]);

	System::Memory::littleEndianToHost(l_pHeaderBuffer+32, &m_ui16NumberOfBlocksInHeader);

	System::Memory::littleEndianToHost(l_pHeaderBuffer+40, &m_ui64EquipmentProviderId);

	for(int i=0 ; i<6 ; i++)
	{
		m_ui8IPAdress[i] = l_pHeaderBuffer[48+i];
	}

	for(int i=0 ; i<3 ; i++)
	{
		System::Memory::littleEndianToHost(l_pHeaderBuffer+54+i*sizeof(uint16), &m_ui16HeadSize[i]);
		System::Memory::littleEndianToHost(l_pHeaderBuffer+60+i*sizeof(float32), &m_f32PositionReferenceElectrode[i]);
		System::Memory::littleEndianToHost(l_pHeaderBuffer+72+i*sizeof(float32), &m_f32GroundElectrode[i]);
	}

	System::Memory::littleEndianToHost(l_pHeaderBuffer+84, reinterpret_cast<uint64*>(&m_i64NumberOfDataRecords));
	System::Memory::littleEndianToHost(l_pHeaderBuffer+92, &m_ui32DurationOfADataRecordNumerator);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+96, &m_ui32DurationOfADataRecordDenominator);
	System::Memory::littleEndianToHost(l_pHeaderBuffer+100, &m_ui16NumberOfSignals);

	if(oFile.bad())
	{
		return false;
	}
	return true;
}

boolean GDF::CFixedGDF2Header::save(std::ofstream& oFile)
{
	return false;
	//TODO complete
	/*
	uint8 l_pTempBuffer[sizeof(uint64)];

	oFile.seekp(0,ios::beg);

	oFile<<m_sVersionId<<m_sPatientId;
	*/
}

std::string GDF::CFixedGDF2Header::getSubjectName()
{
	// extracts the PID and Patient name from the m_sPatientId
	char * l_pToken = strtok(m_sPatientId, " ");
	if(l_pToken)
	{
		//The PId is not a numerical value in GDF, it is useless for us
		l_pToken = strtok(NULL, " ");
	}

	if(l_pToken)
	{
		return l_pToken;
	}
	else
	{
		return _NoValueS_;
	}
}

uint64 GDF::CFixedGDF2Header::getSubjectSex()
{
	return m_ui8SubjectInformation & 0x03;
}

std::string GDF::CFixedGDF2Header::getExperimentDate()
{
	//computes the experiment date
	//uint64 l_ui32TempDate= *(reinterpret_cast<uint64*>(m_ui32StartDateAndTimeOfRecording));

	//time_t l_sStartDateAndTimeOfRecordingInSeconds = ((l_ui32TempDate/2^32) - 719529) * (3600*24);
	//tm * l_sStartDateAndTimeOfRecording = gmtime(&l_sStartDateAndTimeOfRecordingInSeconds);

	//TODO check how date is coded in openvibe Date not good?
	//(l_sStartDateAndTimeOfRecording->mon+1)<<8 + (l_sStartDateAndTimeOfRecording->day)

	return _NoValueS_;
}

uint64 GDF::CFixedGDF2Header::getSubjectAge()
{
	return static_cast<uint64>( floor(static_cast<double>((m_ui32StartDateAndTimeOfRecording[1] - m_ui32Birthday[1]) / 365.242189813)));
}

float64 GDF::CFixedGDF2Header::getDataRecordDuration()
{
	return static_cast<float64>(m_ui32DurationOfADataRecordNumerator) / static_cast<float64>(m_ui32DurationOfADataRecordDenominator);
}

uint64 GDF::CFixedGDF2Header::getNumberOfDataRecords()
{
	return m_i64NumberOfDataRecords;
}

uint64 GDF::CFixedGDF2Header::getChannelCount()
{
	return m_ui16NumberOfSignals;
}

//VARIABLE HEADER
GDF::CVariableGDF1Header::CVariableGDF1HeaderPerChannel::CVariableGDF1HeaderPerChannel()
	:m_f64PhysicalMinimum(0)
	,m_f64PhysicalMaximum(0)
	,m_i64DigitalMinimum(0)
	,m_i64DigitalMaximum(0)
	,m_ui32NumberOfSamplesInEachRecord(0)
	,m_ui32ChannelType(0)
{
	memset(m_sLabel, ' ', sizeof(m_sLabel));
	memset(m_sTranducerType, ' ', sizeof(m_sTranducerType));
	memset(m_sPhysicalDimension, ' ', sizeof(m_sPhysicalDimension));
	memset(m_sPreFiltering, ' ', sizeof(m_sPreFiltering));
	memset(m_sReserved, ' ', sizeof(m_sReserved));

}

boolean GDF::CVariableGDF1Header::update(std::ofstream& oFile)
{
	if(oFile.is_open())
	{
		uint8 l_pTempBuffer[sizeof(uint64)];
		uint64 l_oBackupPosition = oFile.tellp();

		uint32 l_ui32ChannelCount = m_vVariableHeader.size();

		oFile.seekp(0x100 + 104*l_ui32ChannelCount, ios::beg);

		std::vector<CVariableGDF1HeaderPerChannel>::iterator i;
		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(i->m_f64PhysicalMinimum, l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(float64));
		}
		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(i->m_f64PhysicalMaximum, l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(float64));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(static_cast<uint64>(i->m_i64DigitalMinimum), l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(int64));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(static_cast<uint64>(i->m_i64DigitalMaximum), l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(int64));
		}

		oFile.seekp((std::streamsize)l_oBackupPosition, ios::beg);

		if(oFile.bad())
		{
			//cout<<"Error while writing to the output file"<<endl;
			return false;
		}
		return true;
	}
	return false;
}

void GDF::CVariableGDF1Header::setChannelCount(uint32 ui32ChannelCount)
{
	m_vVariableHeader.resize(ui32ChannelCount);
}

GDF::CVariableGDF1Header::CVariableGDF1HeaderPerChannel& GDF::CVariableGDF1Header::operator[](uint32 ui32Channel)
{
	return m_vVariableHeader[ui32Channel];
}

boolean GDF::CVariableGDF1Header::save(std::ofstream& oFile)
{
	if(oFile.is_open())
	{
		uint8 l_pTempBuffer[sizeof(uint64)];

		oFile.seekp(0x100, ios::beg);

		std::vector<CVariableGDF1HeaderPerChannel>::iterator i;

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			oFile.write(i->m_sLabel, sizeof(i->m_sLabel));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			oFile.write(i->m_sTranducerType, sizeof(i->m_sTranducerType));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			oFile.write(i->m_sPhysicalDimension, sizeof(i->m_sPhysicalDimension));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(i->m_f64PhysicalMinimum, l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(float64));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(i->m_f64PhysicalMaximum, l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(float64));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(static_cast<uint64>(i->m_i64DigitalMinimum), l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(int64));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(static_cast<uint64>(i->m_i64DigitalMaximum), l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(int64));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			oFile.write(i->m_sPreFiltering, sizeof(i->m_sPreFiltering));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(i->m_ui32NumberOfSamplesInEachRecord, l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint32));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			System::Memory::hostToLittleEndian(i->m_ui32ChannelType, l_pTempBuffer);
			oFile.write(reinterpret_cast<char *>(l_pTempBuffer), sizeof(uint32));
		}

		for(i=m_vVariableHeader.begin() ; i!=m_vVariableHeader.end() ; i++)
		{
			oFile.write(i->m_sReserved, sizeof(i->m_sReserved));
		}

		if(oFile.bad())
		{
			//cout<<"Error while writing to the output file"<<endl;
			return false;
		}
		return true;
	}
	return false;
}

//HELPER METHODS
uint16 GDF::GDFDataSize(uint32 ui32ChannelType)
{
	uint16 l_ui16Size=0;
	switch(ui32ChannelType)
	{
		case GDF::ChannelType_int8 : l_ui16Size = 1; break;
		case GDF::ChannelType_uint8 : l_ui16Size = 1; break;
		case GDF::ChannelType_int16 : l_ui16Size = 2; break;
		case GDF::ChannelType_uint16 : l_ui16Size = 2; break;
		case GDF::ChannelType_int32 : l_ui16Size = 4; break;
		case GDF::ChannelType_uint32 : l_ui16Size = 4; break;
		case GDF::ChannelType_int64 : l_ui16Size = 8; break;
		case GDF::ChannelType_uint64 : l_ui16Size = 8; break;

		case GDF::ChannelType_float32 : l_ui16Size = 4; break;
		case GDF::ChannelType_float64 : l_ui16Size = 8; break;
		case GDF::ChannelType_float128 : l_ui16Size = 16; break;

		case GDF::ChannelType_int24 : l_ui16Size = 3; break;
		case GDF::ChannelType_uint24 : l_ui16Size = 3; break;

	}
	return l_ui16Size;
}
