#include "ovasCDriverMicromedIntraEEG.h"
#include "../ovasCConfigurationNetworkGlade.h"

#include <system/Time.h>

#include <cmath>

#include <iostream>
#include <cstring>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CDriverMicromedIntraEEG::CDriverMicromedIntraEEG(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pConnectionServer(NULL)
	,m_ui32ServerHostPort(3000)
	,m_pConnection(NULL)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
{
}

CDriverMicromedIntraEEG::~CDriverMicromedIntraEEG(void)
{
	if(m_pConnectionServer)
	{
		m_pConnectionServer->release();
		m_pConnectionServer=NULL;
	}

}

const char* CDriverMicromedIntraEEG::getName(void)
{
	return "Micromed Intracranial EEG";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMicromedIntraEEG::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) { return false; }

	// Initialize var for connection
	uint32 l_ui32Listen = 0;

	// Builds up server connection
	m_pConnectionServer=Socket::createConnectionServer();
	if (m_pConnectionServer)
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace <<"> Server is on \n";
		// Server start listening on defined port
		l_ui32Listen = m_pConnectionServer->listen(m_ui32ServerHostPort);

		if (l_ui32Listen)
		{
			m_rDriverContext.getLogManager() << LogLevel_Trace <<"> Server is listening on port : " <<  m_ui32ServerHostPort << "\n";
			// Accept new client
			m_pConnection=m_pConnectionServer->accept();

			// Initialize vars for reception
			uint32 l_ui32Received = 0;
			uint32 l_ui32ReqLength = 0;
			uint32 l_ui32Result = 0;

			m_pStructHeader = (char *)&m_structHeader;

			// Receive Header
			while(l_ui32Received < sizeof(m_structHeader))
			{
				l_ui32ReqLength =  sizeof(m_structHeader) -  l_ui32Received;
				l_ui32Result = m_pConnection->receiveBuffer((char*)m_pStructHeader, l_ui32ReqLength);

				l_ui32Received += l_ui32Result;
				m_pStructHeader += l_ui32Result;
				m_rDriverContext.getLogManager() << LogLevel_Trace << "> Receiving Header....\n";
			}

			// Verify header validity
			if (strcmp(m_structHeader.fixCode,"MICM"))
			{
				m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with fixCode\n";
				return false;
			}
			if (m_structHeader.infoType!=0)
			{
				m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with infoType\n";
				return false;
			}
			if (m_structHeader.lenData <= 0)
			{
				m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with lenData\n";
				return false;
			}

			// Initialize vars for reception
			l_ui32Received = 0;
			l_ui32ReqLength = 0;
			l_ui32Result = 0;
			m_pStructHeaderInfo = (char *)&m_structHeaderInfo;

			// Receive Header
			while(l_ui32Received < sizeof(m_structHeaderInfo))
			{
				l_ui32ReqLength =  sizeof(m_structHeaderInfo) -  l_ui32Received;
				l_ui32Result = m_pConnection->receiveBuffer((char*)m_pStructHeaderInfo, l_ui32ReqLength);

				l_ui32Received += l_ui32Result;
				m_pStructHeaderInfo += l_ui32Result;
				m_rDriverContext.getLogManager() << LogLevel_Trace << "> Receiving Header Info...\n";
			}
			m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";

			// Initialize vars for reception
			l_ui32Received = 0;
			l_ui32ReqLength = 0;
			l_ui32Result = 0;
			uint32 l_ui32RestOfDataSize = m_structHeaderInfo.addressOfData -  sizeof(m_structHeaderInfo);
			char* l_charHdBuffer=new char[l_ui32RestOfDataSize];
			char* l_pcharHdBuffer=l_charHdBuffer;

			// Receive rest of the header not used yet maybe one day...
			while(l_ui32Received < l_ui32RestOfDataSize)
			{
				l_ui32ReqLength =  l_ui32RestOfDataSize -  l_ui32Received;
				l_ui32Result = m_pConnection->receiveBuffer(l_pcharHdBuffer, l_ui32ReqLength);

				l_ui32Received += l_ui32Result;
				l_pcharHdBuffer += l_ui32Result;
				m_rDriverContext.getLogManager() << LogLevel_Trace << "> Receiving Header Info...\n";
			}
			m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";
			m_rDriverContext.getLogManager() << LogLevel_Trace << "m_structHeaderInfo.positiveInputLabel = " << m_structHeaderInfo.positiveInputLabel << "\n";
			m_rDriverContext.getLogManager() << LogLevel_Trace << "m_structHeaderInfo.negativeInputLabel = " << m_structHeaderInfo.negativeInputLabel << "\n";
			m_rDriverContext.getLogManager() << LogLevel_Trace << "SSStartOffsetCode = " << (uint32)m_structHeaderInfo.SSStartOffsetCode << "\n";
			delete [] l_charHdBuffer;

			// Save Header info into m_oHeader
			//m_oHeader.setExperimentIdentifier();
			//m_oHeader.setExperimentDate();

			//m_oHeader.setSubjectId();
			//m_oHeader.setSubjectName();
			//m_oHeader.setSubjectAge();
			//m_oHeader.setSubjectGender();

			//m_oHeader.setLaboratoryId();
			//m_oHeader.setLaboratoryName();

			//m_oHeader.setTechnicianId();
			//m_oHeader.setTechnicianName();

			m_oHeader.setChannelCount(m_structHeaderInfo.nbOfChannels);

			//~ for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
			//~ {
				//~ m_oHeader.setChannelName(i, m_structHeader.channelLabel[i]);
				//~ m_oHeader.setChannelGain(i, (m_structHeader.qGain[i]*m_structHeader.properGain[i]));
			//~ }

			m_oHeader.setSamplingFrequency((uint32)m_structHeaderInfo.minimumSamplingRate);

			//m_oHeader.setSampleCount(ui32SampleCountPerSentBlock);

			m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;

			m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];

			if(!m_pSample)
			{
				delete [] m_pSample;
				m_pSample=NULL;
				return false;
			}

			m_pCallback=&rCallback;

			m_ui32IndexIn = 0;
			m_ui32IndexOut = 0;
			m_ui32BuffDataIndex = 0;

			return true;
		}
	}

	return false;
}

boolean CDriverMicromedIntraEEG::start(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }
	return true;
}

boolean CDriverMicromedIntraEEG::loop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return true; }

	// Initialize vars for reception
	uint32 l_ui32Received = 0;
	uint32 l_ui32ReqLength = 0;
	uint32 l_ui32Result = 0;
	m_pStructHeader = (char *)&m_structHeader;

	// Receive Header
	while(l_ui32Received < sizeof(m_structHeader))
	{
		l_ui32ReqLength =  sizeof(m_structHeader) -  l_ui32Received;
		l_ui32Result = m_pConnection->receiveBuffer((char*)m_pStructHeader, l_ui32ReqLength);

		l_ui32Received += l_ui32Result;
		m_pStructHeader += l_ui32Result;
	}
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";

	m_rDriverContext.getLogManager() << LogLevel_Trace << "m_structHeader.fixCode = " << m_structHeader.fixCode << "\n";
	m_rDriverContext.getLogManager() << LogLevel_Trace << "m_structHeader.infoType = " << m_structHeader.infoType << "\n";
	m_rDriverContext.getLogManager() << LogLevel_Trace << "m_structHeader.lenData = " << m_structHeader.lenData << "\n";

	// Verify header validity
	//~ if (strcmp(m_structHeader.fixCode,"MICM"))
	//~ {
		//~ m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with fixCode\n";
		//~ return false;
	//~ }
	if (m_structHeader.infoType!=1)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with infoType\n";
		return false;
	}
	if (m_structHeader.lenData <= 0)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with lenData\n";
		return false;
	}

	// Initialize var to receive buffer of data
	l_ui32Received = 0;
	l_ui32ReqLength = 0;
	l_ui32Result = 0;
	m_ui32BuffDataIndex++;

	uint32 l_ui32DataBufferSize= m_structHeader.lenData;
	m_pStructBuffData = (char *)&m_structBuffData;

	// Read a first buffer of data
	while(l_ui32Received < l_ui32DataBufferSize)
	{
		l_ui32ReqLength = l_ui32DataBufferSize -  l_ui32Received;
		l_ui32Result = m_pConnection->receiveBuffer((char*)m_pStructBuffData, l_ui32ReqLength);
		l_ui32Received += l_ui32Result;
		m_pStructBuffData += l_ui32Result;
	}
	m_rDriverContext.getLogManager() << LogLevel_Trace << " > Buffer of data, nb: " << m_ui32BuffDataIndex << " received\n";

	// if input flow is equal to output one
	if (m_ui32SampleCountPerSentBlock == (uint32)m_structHeader.lenData/m_structHeaderInfo.nbOfChannels)
	{
		for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
		{
			for (uint32 j=0; j < m_ui32SampleCountPerSentBlock; j++)
			{
				m_pSample[j + i*m_ui32SampleCountPerSentBlock] = (float32)(m_structBuffData.data[m_oHeader.getChannelCount()*j + i]);
			}
		}
		// send data
		m_pCallback->setSamples(m_pSample);
	}
	else
	{
		// if output flow is bigger
		if (m_ui32SampleCountPerSentBlock > (uint32)m_structHeader.lenData/m_structHeaderInfo.nbOfChannels)
		{
			while (m_ui32IndexOut + (uint32)m_structHeader.lenData/m_structHeaderInfo.nbOfChannels < m_ui32SampleCountPerSentBlock)
			{
				for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
				{
					for (uint32 j=0; j < (uint32)m_structHeader.lenData/m_structHeaderInfo.nbOfChannels; j++)
					{
						m_pSample[m_ui32IndexOut + j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i];
					}
				}

				m_ui32IndexOut = m_ui32IndexOut + (uint32)m_structHeader.lenData/m_structHeaderInfo.nbOfChannels;

				// Initialize vars for reception
				l_ui32Received = 0;
				l_ui32ReqLength = 0;
				l_ui32Result = 0;
				m_pStructHeader = (char *)&m_structHeader;

				// Receive Header
				while(l_ui32Received < sizeof(m_structHeader))
				{
					l_ui32ReqLength =  sizeof(m_structHeader) -  l_ui32Received;
					l_ui32Result = m_pConnection->receiveBuffer((char*)m_pStructHeader, l_ui32ReqLength);

					l_ui32Received += l_ui32Result;
					m_pStructHeader += l_ui32Result;
				}
				m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";

				// Verify header validity
				//~ if (strcmp(m_structHeader.fixCode,"MICM"))
				//~ {
					//~ m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with fixCode\n";
					//~ return false;
				//~ }
				if (m_structHeader.infoType!=1)
				{
					m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with infoType\n";
					return false;
				}
				if (m_structHeader.lenData <= 0)
				{
					m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with lenData\n";
					return false;
				}

				// Initialize var to receive buffer of data
				l_ui32Received = 0;
				l_ui32ReqLength = 0;
				l_ui32Result = 0;
				m_ui32BuffDataIndex++;

				l_ui32DataBufferSize= m_structHeader.lenData;
				m_pStructBuffData = (char *)&m_structBuffData;

				// Read a first buffer of data
				while(l_ui32Received < l_ui32DataBufferSize)
				{
					l_ui32ReqLength = l_ui32DataBufferSize -  l_ui32Received;
					l_ui32Result = m_pConnection->receiveBuffer((char*)m_pStructBuffData, l_ui32ReqLength);
					l_ui32Received += l_ui32Result;
					m_pStructBuffData += l_ui32Result;
				}
				m_rDriverContext.getLogManager() << LogLevel_Trace << " > Buffer of data, nb: " << m_ui32BuffDataIndex << " received\n";

			}

			// Finishing filling up
			for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
			{
				for (uint32 j=0; j < m_ui32SampleCountPerSentBlock-m_ui32IndexOut ; j++)
				{
					m_pSample[m_ui32IndexOut + j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i];
				}
			}

			// send data
			m_pCallback->setSamples(m_pSample);

			// Reset index out because new output
			m_ui32IndexIn = m_ui32SampleCountPerSentBlock-m_ui32IndexOut;
			m_ui32IndexOut = 0;

			// Save the rest of the buffer input
			for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
			{
				for (uint32 j=0; j < (uint32)m_structHeader.lenData/m_structHeaderInfo.nbOfChannels - m_ui32IndexIn ; j++)
				{
					m_pSample[j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_ui32IndexIn *m_oHeader.getChannelCount() + m_oHeader.getChannelCount()*j + i];
				}
			}

			m_ui32IndexOut = (uint32)m_structHeader.lenData/m_structHeaderInfo.nbOfChannels-m_ui32IndexIn;
			m_ui32IndexIn = 0;

		}

		//~ else
		//~ {
			//~ // if output flow is smaller
			//~ while (m_ui32IndexIn + (m_ui32SampleCountPerSentBlock - m_ui32IndexOut)< (uint32)m_structHeaderInfo.nbOfChannels)
			//~ {
				//~ for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
				//~ {
					//~ for (uint32 j=0; j < m_ui32SampleCountPerSentBlock - m_ui32IndexOut; j++)
					//~ {
						//~ if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
						//~ {
							//~ m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
						//~ }
						//~ else
						//~ {
							//~ m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
						//~ }
					//~ }
				//~ }
				//~ //send data
				//~ m_pCallback->setSamples(m_pSample);

				//~ m_ui32IndexIn = m_ui32IndexIn + (m_ui32SampleCountPerSentBlock - m_ui32IndexOut);
				//~ m_ui32IndexOut = 0;
			//~ }

			//~ // save the rest of buff data
			//~ for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
			//~ {
				//~ for (uint32 j=0; j <  (uint32)m_structHeaderInfo.nbOfChannels - m_ui32IndexIn; j++)
				//~ {
					//~ if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
					//~ {
						//~ m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
					//~ }
					//~ else
					//~ {
						//~ m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
					//~ }
				//~ }
			//~ }

			//~ m_ui32IndexOut = (uint32)m_structHeaderInfo.nbOfChannels - m_ui32IndexIn;
			//~ m_ui32IndexIn = 0;
		//~ }

	}

	return true;

}

boolean CDriverMicromedIntraEEG::stop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Server stopped\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }
	return true;
}

boolean CDriverMicromedIntraEEG::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	delete [] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;

	// Cleans up server connection
	m_pConnectionServer->close();
	m_pConnectionServer->release();
	m_pConnectionServer=NULL;

	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Server disconnected\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMicromedIntraEEG::isConfigurable(void)
{
	return true;
}

boolean CDriverMicromedIntraEEG::configure(void)
{
	CConfigurationNetworkGlade l_oConfiguration("../share/openvibe-applications/acquisition-server/interface-Micromed-IntraEEG.glade");

	l_oConfiguration.setHostPort(m_ui32ServerHostPort);

	if(l_oConfiguration.configure(m_oHeader))
	{
		m_ui32ServerHostPort=l_oConfiguration.getHostPort();
		return true;
	}

	return false;
}
