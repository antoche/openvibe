#include "ovasCDriverCtfVsmMeg.h"
#include "../ovasCConfigurationNetworkBuilder.h"

#include <system/Time.h>

#include <cmath>

#include <iostream>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CDriverCtfVsmMeg::CDriverCtfVsmMeg(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pConnectionClient(NULL)
	,m_sServerHostName("localhost")
	,m_ui32ServerHostPort(9999)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
{
}

CDriverCtfVsmMeg::~CDriverCtfVsmMeg(void)
{
}

const char* CDriverCtfVsmMeg::getName(void)
{
	return "CTF/VSM MEG";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverCtfVsmMeg::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) { return false; }

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
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "> Client connected\n";
		// Initialize vars for reception
		uint32 l_ui32Received = 0;
		uint32 l_ui32ReqLength = 0;
		uint32 l_ui32Result = 0;
		m_pStructHeader = (char*) &m_structHeader;
		// Receive Header
		while(l_ui32Received < sizeof(m_structHeader))
		{
			l_ui32ReqLength = sizeof(m_structHeader) -  l_ui32Received;
			l_ui32Result = m_pConnectionClient->receiveBuffer((char*)m_pStructHeader, l_ui32ReqLength);

			l_ui32Received += l_ui32Result;
			m_pStructHeader += l_ui32Result;
			m_rDriverContext.getLogManager() << LogLevel_Trace << "> Receiving Header....\n";
		}
		m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";

		// Save Header info into m_oHeader
		//m_oHeader.setExperimentIdentifier();
		//m_oHeader.setExperimentDate();

		//m_oHeader.setSubjectId();
		//m_oHeader.setSubjectName();
		m_oHeader.setSubjectAge(m_structHeader.subjectAge);
		//m_oHeader.setSubjectGender();

		//m_oHeader.setLaboratoryId();
		//m_oHeader.setLaboratoryName();

		//m_oHeader.setTechnicianId();
		//m_oHeader.setTechnicianName();

		m_oHeader.setChannelCount(m_structHeader.numberOfChannels);
		for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
		{
			m_oHeader.setChannelName(i, m_structHeader.channelLabel[i]);
			m_oHeader.setChannelGain(i, (m_structHeader.qGain[i]*m_structHeader.properGain[i]));
		}

		m_oHeader.setSamplingFrequency((uint32)m_structHeader.samplingRate);

		//m_oHeader.setSampleCount(ui32SampleCountPerSentBlock);

		m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];

		if(!m_pSample)
		{
			delete [] m_pSample;
			m_pSample=NULL;
			return false;
		}

		m_pCallback=&rCallback;

		m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;

		m_ui32IndexIn = 0;
		m_ui32IndexOut = 0;
		m_ui32SocketFlag = 1976;
		m_ui32BuffDataIndex = 0;

		return true;
	}

}

boolean CDriverCtfVsmMeg::start(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	if (m_ui32BuffDataIndex==0)
	{
		// Initialize var to send flag
		uint32 l_ui32Sent = 0;
		uint32 l_ui32ReqLength = 0;
		uint32 l_ui32Result = 0;
		uint32* l_pSocketFlag = (uint32*) &m_ui32SocketFlag;

		// Send flag to server
		while(l_ui32Sent < sizeof(m_ui32SocketFlag))
		{
			l_ui32ReqLength = sizeof(m_ui32SocketFlag) -  l_ui32Sent;
			l_ui32Result = m_pConnectionClient->sendBuffer((uint32*)l_pSocketFlag, l_ui32ReqLength);
			l_ui32Sent += l_ui32Result;
			m_rDriverContext.getLogManager() << LogLevel_Trace << " > Sending flag to start....\n";
		}
		m_rDriverContext.getLogManager() << LogLevel_Trace << "> Flag sent\n";
	}

	return true;
}

boolean CDriverCtfVsmMeg::loop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return true; }

	// Initialize var to receive buffer of data
	uint32 l_ui32Received = 0;
	uint32 l_ui32ReqLength = 0;
	uint32 l_ui32Result = 0;
	m_pStructBuffData = (char*) &m_structBuffData;

	// Read a first buffer of data
	m_ui32BuffDataIndex++;
	while(l_ui32Received < sizeof(m_structBuffData))
	{
		l_ui32ReqLength = sizeof(m_structBuffData) -  l_ui32Received;
		l_ui32Result = m_pConnectionClient->receiveBuffer((char*)m_pStructBuffData, l_ui32ReqLength);
		l_ui32Received += l_ui32Result;
		m_pStructBuffData += l_ui32Result;
		m_rDriverContext.getLogManager() << LogLevel_Trace << " > Receiving buffer of data....nb: " << m_ui32BuffDataIndex << "\n";
	}

	// if input flow is equal to output one
	if (m_ui32SampleCountPerSentBlock == (uint32)m_structBuffData.nbSamplesPerChanPerBlock)
	{
		for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
		{
			for (uint32 j=0; j < m_ui32SampleCountPerSentBlock; j++)
			{
				if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
				{
					m_pSample[j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
				}
				else
				{
					m_pSample[j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
				}
			}
		}
		// send data
		m_pCallback->setSamples(m_pSample);
	}
	else
	{
		// if output flow is bigger
		if (m_ui32SampleCountPerSentBlock > (uint32)m_structBuffData.nbSamplesPerChanPerBlock)
		{
			while (m_ui32IndexOut + (uint32)m_structBuffData.nbSamplesPerChanPerBlock < m_ui32SampleCountPerSentBlock)
			{
				for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
				{
					for (uint32 j=0; j < (uint32)m_structBuffData.nbSamplesPerChanPerBlock; j++)
					{
						if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
						{
							m_pSample[m_ui32IndexOut + j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
						}
						else
						{
							m_pSample[m_ui32IndexOut + j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
						}
					}
				}

				m_ui32IndexOut = m_ui32IndexOut + (uint32)m_structBuffData.nbSamplesPerChanPerBlock;

				// Ask for another buffer
				l_ui32Received = 0;
				m_pStructBuffData = (char*)&m_structBuffData;

				while(l_ui32Received < sizeof(m_structBuffData))
				{
					l_ui32ReqLength = sizeof(m_structBuffData) -  l_ui32Received;
					l_ui32Result = m_pConnectionClient->receiveBuffer((char*)m_pStructBuffData, l_ui32ReqLength);

					l_ui32Received += l_ui32Result;
					m_pStructBuffData += l_ui32Result;
					m_rDriverContext.getLogManager() << LogLevel_Trace << " > Receiving buffer of data....nb: " << m_ui32BuffDataIndex << "\n";
				}

			}

			// Finishing filling up
			for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
			{
				for (uint32 j=0; j < m_ui32SampleCountPerSentBlock-m_ui32IndexOut ; j++)
				{
					if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
					{
						m_pSample[m_ui32IndexOut + j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
					}
					else
					{
						m_pSample[m_ui32IndexOut + j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
					}
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
				for (uint32 j=0; j < (uint32)m_structBuffData.nbSamplesPerChanPerBlock - m_ui32IndexIn ; j++)
				{
					if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
					{
						m_pSample[j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_ui32IndexIn *m_oHeader.getChannelCount() + m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
					}
					else
					{
						m_pSample[j + i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_ui32IndexIn *m_oHeader.getChannelCount() + m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
					}
				}
			}

			m_ui32IndexOut = (uint32)m_structBuffData.nbSamplesPerChanPerBlock-m_ui32IndexIn;
			m_ui32IndexIn = 0;

		}
		else
		{
			// if output flow is smaller
			while (m_ui32IndexIn + (m_ui32SampleCountPerSentBlock - m_ui32IndexOut)< (uint32)m_structBuffData.nbSamplesPerChanPerBlock)
			{
				for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
				{
					for (uint32 j=0; j < m_ui32SampleCountPerSentBlock - m_ui32IndexOut; j++)
					{
						if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
						{
							m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
						}
						else
						{
							m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
						}
					}
				}
				//send data
				m_pCallback->setSamples(m_pSample);

				m_ui32IndexIn = m_ui32IndexIn + (m_ui32SampleCountPerSentBlock - m_ui32IndexOut);
				m_ui32IndexOut = 0;
			}

			// save the rest of buff data
			for (uint32 i=0; i < m_oHeader.getChannelCount(); i++)
			{
				for (uint32 j=0; j <  (uint32)m_structBuffData.nbSamplesPerChanPerBlock - m_ui32IndexIn; j++)
				{
					if ((m_structHeader.channelTypeIndex[i]==0) | (m_structHeader.channelTypeIndex[i]==1) |(m_structHeader.channelTypeIndex[i]==5))
					{
						m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] * 1e15 / m_oHeader.getChannelGain(i);
					}
					else
					{
						m_pSample[j + m_ui32IndexOut+ i*m_ui32SampleCountPerSentBlock] = (float32)m_structBuffData.data[m_oHeader.getChannelCount() *  m_ui32IndexIn + m_oHeader.getChannelCount()*j + i] / m_oHeader.getChannelGain(i);
					}
				}
			}

			m_ui32IndexOut = (uint32)m_structBuffData.nbSamplesPerChanPerBlock - m_ui32IndexIn;
			m_ui32IndexIn = 0;
		}

	}

	return true;

}

boolean CDriverCtfVsmMeg::stop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Connection stopped\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }
	return true;
}

boolean CDriverCtfVsmMeg::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

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

boolean CDriverCtfVsmMeg::isConfigurable(void)
{
	return true;
}

boolean CDriverCtfVsmMeg::configure(void)
{
	CConfigurationNetworkBuilder l_oConfiguration("../share/openvibe-applications/acquisition-server/interface-CtfVsm-Meg.ui");

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
