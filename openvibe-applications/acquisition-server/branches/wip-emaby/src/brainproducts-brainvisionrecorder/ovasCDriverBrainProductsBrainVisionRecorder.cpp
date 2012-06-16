#include "ovasCDriverBrainProductsBrainVisionRecorder.h"
#include "../ovasCConfigurationNetworkBuilder.h"

#include <system/Time.h>

#include <cmath>

#include <iostream>

#include <cstdlib>
#include <cstring>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CDriverBrainProductsBrainVisionRecorder::CDriverBrainProductsBrainVisionRecorder(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_pConnectionClient(NULL)
	,m_sServerHostName("192.168.0.2")
	,m_ui32ServerHostPort(51244)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
{
}

CDriverBrainProductsBrainVisionRecorder::~CDriverBrainProductsBrainVisionRecorder(void)
{
}

const char* CDriverBrainProductsBrainVisionRecorder::getName(void)
{
	return "Brain Products Brainamp Standard (through Vision Recorder)";
}



//___________________________________________________________________//
//                                                                   //

boolean CDriverBrainProductsBrainVisionRecorder::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) { return false; }

	// Initialize GUID value
	DEFINE_GUID(GUID_RDAHeader,
		1129858446, 51606, 19590, char(175), char(74), char(152), char(187), char(246), char(201), char(20), char(80)
	);

	// Builds up client connection
	m_pConnectionClient=Socket::createConnectionClient();

	// Tries to connect to server
	m_pConnectionClient->connect(m_sServerHostName, m_ui32ServerHostPort);

	// Checks if connection is correctly established
	if(!m_pConnectionClient->isConnected())
	{
		// In case it is not, try to reconnect
		m_pConnectionClient->connect(m_sServerHostName, m_ui32ServerHostPort);
	}

	if(!m_pConnectionClient->isConnected())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Connection problem! Tried 2 times without success! :(\n";
		m_rDriverContext.getLogManager() << LogLevel_Error << "Verify port number and/or Hostname...\n";
		return false;
	}

	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Client connected\n";

	
	// Initialize vars for reception
	m_pStructRDA_MessageHeader = NULL;
	RDA_MessageHeader l_structRDA_MessageHeader;
	m_pcharStructRDA_MessageHeader = (char*)&l_structRDA_MessageHeader;

	uint32 l_ui32Received = 0;
	uint32 l_ui32ReqLength = 0;
	uint32 l_ui32Result = 0;
	uint32 l_ui32Datasize = sizeof(l_structRDA_MessageHeader);

	// Receive Header
	while(l_ui32Received < l_ui32Datasize)
	{
		l_ui32ReqLength = l_ui32Datasize -  l_ui32Received;
		l_ui32Result = m_pConnectionClient->receiveBuffer((char*)m_pcharStructRDA_MessageHeader, l_ui32ReqLength);

		l_ui32Received += l_ui32Result;
		m_pcharStructRDA_MessageHeader += l_ui32Result;
	}	

	// Check for correct header GUID.
	if (!COMPARE_GUID(l_structRDA_MessageHeader.guid, GUID_RDAHeader))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "GUID received is not correct!\n";
		return false;
	}

	// Check for correct header nType
	if (l_structRDA_MessageHeader.nType !=1)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "First Message received is not an header!\n";
		m_rDriverContext.getLogManager() << LogLevel_Error << "Try to reconnect....\n";
		return false;
	}

	// Retrieve rest of data
	if (!(*(&m_pStructRDA_MessageHeader) = (RDA_MessageHeader*)malloc(l_structRDA_MessageHeader.nSize)))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Couldn't allocate memory\n";
		return false;
	}
	else
	{
		memcpy(*(&m_pStructRDA_MessageHeader), &l_structRDA_MessageHeader, sizeof(l_structRDA_MessageHeader));
		m_pcharStructRDA_MessageHeader = (char*)(*(&m_pStructRDA_MessageHeader)) + sizeof(l_structRDA_MessageHeader);
		l_ui32Received=0;
		l_ui32ReqLength = 0;
		l_ui32Result = 0;
		l_ui32Datasize = l_structRDA_MessageHeader.nSize - sizeof(l_structRDA_MessageHeader);
		while(l_ui32Received < l_ui32Datasize)
		{
			l_ui32ReqLength = l_ui32Datasize -  l_ui32Received;
			l_ui32Result = m_pConnectionClient->receiveBuffer((char*)m_pcharStructRDA_MessageHeader, l_ui32ReqLength);

			l_ui32Received += l_ui32Result;
			m_pcharStructRDA_MessageHeader += l_ui32Result;
		}
	}

	m_pStructRDA_MessageStart = NULL;
	m_pStructRDA_MessageStart = (RDA_MessageStart*)m_pStructRDA_MessageHeader;

	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";

	// Save Header info into m_oHeader
	//m_oHeader.setExperimentIdentifier();
	//m_oHeader.setExperimentDate();

	//m_oHeader.setSubjectId();
	//m_oHeader.setSubjectName();
	//m_oHeader.setSubjectAge(m_structHeader.subjectAge);
	//m_oHeader.setSubjectGender();

	//m_oHeader.setLaboratoryId();
	//m_oHeader.setLaboratoryName();

	//m_oHeader.setTechnicianId();
	//m_oHeader.setTechnicianName();

	m_oHeader.setChannelCount((uint32)m_pStructRDA_MessageStart->nChannels);

	char* l_pszChannelNames = (char*)m_pStructRDA_MessageStart->dResolutions + (m_pStructRDA_MessageStart->nChannels * sizeof(m_pStructRDA_MessageStart->dResolutions[0]));
	for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
	{
		m_oHeader.setChannelName(i, l_pszChannelNames);
		m_oHeader.setChannelGain(i, (float32)((m_pStructRDA_MessageStart->dResolutions[i])));
		l_pszChannelNames += strlen(l_pszChannelNames) + 1;
	}

	m_oHeader.setSamplingFrequency((uint32)(1000000/m_pStructRDA_MessageStart->dSamplingInterval)); //dSamplingInterval in microseconds

	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	
	m_pCallback=&rCallback;

	m_ui32IndexIn = 0;
	m_ui32IndexOut = 0;
	m_ui32BuffDataIndex = 0;

	m_ui32MarkerCount =0;
	m_ui32NumberOfMarkers = 0;

	return true;
}







boolean CDriverBrainProductsBrainVisionRecorder::start(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }
	return true;

}



boolean CDriverBrainProductsBrainVisionRecorder::loop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return true; }


	DEFINE_GUID(GUID_RDAHeader,
		1129858446, 51606, 19590, char(175), char(74), char(152), char(187), char(246), char(201), char(20), char(80)
	);


	// Initialize var to receive buffer of data
	m_pStructRDA_MessageHeader = NULL;
	RDA_MessageHeader l_structRDA_MessageHeader;
	m_pcharStructRDA_MessageHeader = (char*)&l_structRDA_MessageHeader;
	uint32 l_ui32Received = 0;
	uint32 l_ui32ReqLength = 0;
	uint32 l_ui32Result = 0;
	uint32 l_ui32Datasize = sizeof(l_structRDA_MessageHeader);
			
	// Receive Header
	while(l_ui32Received < l_ui32Datasize)
	{	

		l_ui32ReqLength = l_ui32Datasize -  l_ui32Received;
		l_ui32Result = m_pConnectionClient->receiveBuffer((char*)m_pcharStructRDA_MessageHeader, l_ui32ReqLength);
		l_ui32Received += l_ui32Result;
		m_pcharStructRDA_MessageHeader += l_ui32Result;
	}

		

		
	// Check for correct header nType
	if (l_structRDA_MessageHeader.nType == 1)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Message received is a header!\n";
		return false;
	}
	if (l_structRDA_MessageHeader.nType == 3)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Message received is a STOP!\n";
		return false;
	}
	if (l_structRDA_MessageHeader.nType !=4)
	{
		return true;
	}

	// Check for correct header GUID.
	if (!COMPARE_GUID(l_structRDA_MessageHeader.guid, GUID_RDAHeader))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "GUID received is not correct!\n";
		return false;
	}

	// Retrieve rest of block.
	if (!(*(&m_pStructRDA_MessageHeader) = (RDA_MessageHeader*)malloc(l_structRDA_MessageHeader.nSize)))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Couldn't allocate memory\n";
		return false;
	}
	else
	{
		memcpy(*(&m_pStructRDA_MessageHeader), &l_structRDA_MessageHeader, sizeof(l_structRDA_MessageHeader));
		m_pcharStructRDA_MessageHeader = (char*)(*(&m_pStructRDA_MessageHeader)) + sizeof(l_structRDA_MessageHeader);
		l_ui32Received=0;
		l_ui32ReqLength = 0;
		l_ui32Result = 0;
		l_ui32Datasize = l_structRDA_MessageHeader.nSize - sizeof(l_structRDA_MessageHeader);
		while(l_ui32Received < l_ui32Datasize)
		{
			l_ui32ReqLength = l_ui32Datasize -  l_ui32Received;
			l_ui32Result = m_pConnectionClient->receiveBuffer((char*)m_pcharStructRDA_MessageHeader, l_ui32ReqLength);

			l_ui32Received += l_ui32Result;
			m_pcharStructRDA_MessageHeader += l_ui32Result;
		}
		m_ui32BuffDataIndex++;
	}

	// Put the data into MessageData32 structure
	m_pStructRDA_MessageData32 = NULL;
	m_pStructRDA_MessageData32 = (RDA_MessageData32*)m_pStructRDA_MessageHeader;

	//////////////////////
	//Markers
	if (m_pStructRDA_MessageData32->nMarkers > 0)
	{
// 		if (m_pStructRDA_MessageData32->nMarkers == 0)
// 		{
// 			return true;
// 		}

		m_pStructRDA_Marker = (RDA_Marker*)((char*)m_pStructRDA_MessageData32->fData + m_pStructRDA_MessageData32->nPoints * m_oHeader.getChannelCount() * sizeof(m_pStructRDA_MessageData32->fData[0]));

		m_ui32NumberOfMarkers = m_pStructRDA_MessageData32->nMarkers;

		m_vStimulationIdentifier.assign(m_ui32NumberOfMarkers, 0);
		m_vStimulationDate.assign(m_ui32NumberOfMarkers, 0);
		m_vStimulationSample.assign(m_ui32NumberOfMarkers, 0);

		for (uint32 i = 0; i < m_pStructRDA_MessageData32->nMarkers; i++)
		{
			char* pszType = m_pStructRDA_Marker->sTypeDesc;
			char* pszDescription = pszType + strlen(pszType) + 1;

			// m_rDriverContext.getLogManager() << LogLevel_Info << "Stim " << m_ui32MarkerCount + i + 1 << ", " << atoi(strtok (pszDescription,"S")) << ", " << m_pStructRDA_Marker->nPosition<< "\n";

			m_vStimulationIdentifier[i] = atoi(strtok (pszDescription,"S"));
			m_vStimulationDate[i] = (((uint64)(m_pStructRDA_Marker->nPosition )) << 32) / m_oHeader.getSamplingFrequency();
			m_vStimulationSample[i] = m_pStructRDA_Marker->nPosition;
			m_pStructRDA_Marker = (RDA_Marker*)((char*)m_pStructRDA_Marker + m_pStructRDA_Marker->nSize);
		}

		m_ui32MarkerCount += m_pStructRDA_MessageData32->nMarkers;
	}

	m_pSample=new float32[m_oHeader.getChannelCount()*(uint32)m_pStructRDA_MessageData32->nPoints];

	
		for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
		{
			for (uint32 j=0; j < (uint32)m_pStructRDA_MessageData32->nPoints; j++)
			{
					m_pSample[j + (i*(uint32)m_pStructRDA_MessageData32->nPoints)] = (float32)m_pStructRDA_MessageData32->fData[(m_oHeader.getChannelCount()*j) + i]*m_oHeader.getChannelGain(i);	
			}

		}


		// send data
		CStimulationSet l_oStimulationSet;
		l_oStimulationSet.setStimulationCount(m_ui32NumberOfMarkers);
		for (uint32 i = 0; i < m_ui32NumberOfMarkers; i++)
		{
			l_oStimulationSet.setStimulationIdentifier(i, OVTK_StimulationId_Label(m_vStimulationIdentifier[i]));
			l_oStimulationSet.setStimulationDate(i, m_vStimulationDate[i]);
			l_oStimulationSet.setStimulationDuration(i, 0);
 m_rDriverContext.getLogManager() << LogLevel_Info << "Stim " <<m_ui32MarkerCount+ i  << ", " << m_vStimulationIdentifier[i] << ", " << m_vStimulationDate[i]<< "\n";

		}

		m_pCallback->setSamples(m_pSample,(uint32)m_pStructRDA_MessageData32->nPoints);

		m_pCallback->setStimulationSet(l_oStimulationSet);
	//m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

		m_ui32NumberOfMarkers = 0;


	return true;

}

boolean CDriverBrainProductsBrainVisionRecorder::stop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Connection stopped\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }
	return true;
}

boolean CDriverBrainProductsBrainVisionRecorder::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	if (m_pcharStructRDA_MessageHeader!=NULL) m_pcharStructRDA_MessageHeader=NULL;
	if (m_pStructRDA_MessageHeader!=NULL) m_pStructRDA_MessageHeader= NULL;
	if (m_pStructRDA_MessageStart!=NULL) m_pStructRDA_MessageStart=NULL;
	if (m_pStructRDA_MessageStop!=NULL) m_pStructRDA_MessageStop=NULL;
	if (m_pStructRDA_MessageData32!=NULL) m_pStructRDA_MessageData32=NULL;
	if (m_pStructRDA_Marker!=NULL) m_pStructRDA_Marker=NULL;

	delete [] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;

	// Cleans up client connection
	m_pConnectionClient->close();
	m_pConnectionClient->release();
	m_pConnectionClient=NULL;
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Client disconnected\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverBrainProductsBrainVisionRecorder::isConfigurable(void)
{
	return true;
}

boolean CDriverBrainProductsBrainVisionRecorder::configure(void)
{
	CConfigurationNetworkBuilder l_oConfiguration("../share/openvibe-applications/acquisition-server/interface-BrainProducts-BrainVisionRecorder.ui");

	l_oConfiguration.setHostName(m_sServerHostName);
	l_oConfiguration.setHostPort(m_ui32ServerHostPort);

	if(l_oConfiguration.configure(m_oHeader))
	{
		m_sServerHostName=l_oConfiguration.getHostName();
		m_ui32ServerHostPort=l_oConfiguration.getHostPort();
		return true;
	}

	return false;
}




