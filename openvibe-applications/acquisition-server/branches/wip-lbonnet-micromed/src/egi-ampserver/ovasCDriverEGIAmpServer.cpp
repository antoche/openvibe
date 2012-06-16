#include "ovasCDriverEGIAmpServer.h"
#include "ovasCConfigurationEGIAmpServer.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>
#include <system/Memory.h>

#include <cmath>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

//___________________________________________________________________//
//                                                                   //

CDriverEGIAmpServer::CDriverEGIAmpServer(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_ui32SampleIndex(0)
	,m_pBuffer(NULL)
{
	m_sAmpServerHostName="localhost";
	m_ui32CommandPort=9877;
	m_ui32StreamPort=9879;

	m_oHeader.setSamplingFrequency(1000);
	m_oHeader.setChannelCount(280);
}

void CDriverEGIAmpServer::release(void)
{
	delete this;
}

const char* CDriverEGIAmpServer::getName(void)
{
	return "EGI Net Amps 300 (through AmpServer)";
}

//___________________________________________________________________//
//                                                                   //

#define COMMAND_SIZE 4096

namespace OpenViBEAcquisitionServer
{
	class CCommandConnectionHandler
	{
	public:

		CCommandConnectionHandler(CDriverEGIAmpServer& rDriver, Socket::IConnection* pConnection=NULL) : m_rDriver(rDriver), m_rConnection(pConnection?*pConnection:*rDriver.m_pCommand), m_bListenToResponse(pConnection==NULL) { }
		~CCommandConnectionHandler(void) { }

		boolean send(const char* sCommand)
		{
			m_rDriver.m_rDriverContext.getLogManager() << LogLevel_Trace << "Sending command >> [" << CString(sCommand) << "]\n";
			m_rConnection.sendBufferBlocking(sCommand, ::strlen(sCommand));
			m_rConnection.sendBufferBlocking("\n", 1);
			if(m_bListenToResponse)
			{
				int i, l = ::strlen(sCommand);
				char l_sBuffer[COMMAND_SIZE];
				l=0;
				do
				{
					i=m_rConnection.receiveBuffer(const_cast<char*>(l_sBuffer+l), 1);
					if(i>0)
					{
						l+=i;
					}
				}
				while(i==0 || (i>0 && l_sBuffer[l-1] != '\0' && l_sBuffer[l-1] != '\n'));
				l_sBuffer[l>1?l-1:l]='\0';
				m_rDriver.m_rDriverContext.getLogManager() << LogLevel_Trace << "Received answer << [" << CString(l_sBuffer) << "]\n";
			}
			return true;
		}

		CDriverEGIAmpServer& m_rDriver;
		Socket::IConnection& m_rConnection;
		OpenViBE::boolean m_bListenToResponse;
	};

#pragma pack(1)
	typedef struct
	{
		OpenViBE::int64 m_i64AmplifierIdentifier;
		OpenViBE::uint64 m_ui64PacketSize;
	} SAmpServerPacketHeader;
#pragma pack()
};

	// l_oCommandHandler.send("(sendCommand cmd_NumberOfAmps -1 -1 -1)");
	// l_oCommandHandler.send("(sendCommand cmd_SetPower 0 -1 1)");
	// l_oCommandHandler.send("(sendCommand cmd_GetAmpDetails 0 -1 -1)");
	// l_oCommandHandler.send("(sendCommand cmd_GetAmpStatus 0 -1 -1)");
	// l_oCommandHandler.send("(sendCommand cmd_DefaultAcquisitionState 0 -1 -1)");
	// l_oCommandHandler.send("(sendCommand cmd_DefaultSignalGeneration 0 -1 -1)");
	// l_oCommandHandler.send("(sendCommand cmd_SetWaveShape 0 -1 1)");
	// l_oCommandHandler.send("(sendCommand cmd_SetCalibrationSignalRange 0 -1 -1)");
	// l_oCommandHandler.send("(sendCommand cmd_SetCalibrationSignalAmplitude 0 -1 -1)");
	// l_oCommandHandler.send("(sendCommand cmd_SetCalibrationSignalFreq 0 -1 10)");
	// l_oCommandHandler.send("(sendCommand cmd_TurnAllDriveSignals 0 -1 1)");
	// l_oStreamHandler.send("(sendCommand cmd_GetStatus -1 -1 -1)");
	// l_oStreamHandler.send("(sendCommand cmd_InstallEGINA300TestAmp -1 -1 -1)");

	// l_oCommandHandler.send("(sendCommand cmd_SetPower 0 -1 0)");
	// l_oCommandHandler.send("(sendCommand cmd_Exit -1 -1 -1)");

boolean CDriverEGIAmpServer::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) { return false; }

	m_pCommand=Socket::createConnectionClient();
	if(!m_pCommand->connect(m_sAmpServerHostName.toASCIIString(), m_ui32CommandPort))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not connect to AmpServer command port [" << m_sAmpServerHostName << ":" << m_ui32CommandPort << "]\n";
		m_rDriverContext.getLogManager() << LogLevel_Error << "Please check the driver configuration and the firewall configuration\n";
		m_pCommand->close();
		m_pCommand->release();
		m_pCommand=NULL;
		return false;
	}

	m_pStream=Socket::createConnectionClient();
	if(!m_pStream->connect(m_sAmpServerHostName.toASCIIString(), m_ui32StreamPort))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not connect to AmpServer stream port [" << m_sAmpServerHostName << ":" << m_ui32StreamPort << "]\n";
		m_rDriverContext.getLogManager() << LogLevel_Error << "Please check the driver configuration and the firewall configuration\n";
		m_pStream->close();
		m_pStream->release();
		m_pStream=NULL;
		m_pCommand->close();
		m_pCommand->release();
		m_pCommand=NULL;
		return false;
	}

	m_pBuffer=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	if(!m_pBuffer)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Memory allocation error.\n";
		m_pStream->close();
		m_pStream->release();
		m_pStream=NULL;
		m_pCommand->close();
		m_pCommand->release();
		m_pCommand=NULL;
		return false;
	}

	this->exectue("../share/openvibe-applications/acquisition-server/scripts/egi-default-initialize.script");

	CCommandConnectionHandler l_oCommandHandler(*this);
	l_oCommandHandler.send("(sendCommand cmd_Start 0 -1 -1)");

	CCommandConnectionHandler l_oStreamHandler(*this, m_pStream);
	l_oStreamHandler.send("(sendCommand cmd_ListenToAmp 0 -1 -1)");

	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_ui32SampleIndex=0;
	m_ui32ChannelCount=m_oHeader.getChannelCount();

	return true;
}

boolean CDriverEGIAmpServer::start(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	this->exectue("../share/openvibe-applications/acquisition-server/scripts/egi-default-start.script");

	return true;
}

boolean CDriverEGIAmpServer::loop(void)
{
	uint32 i, j;
	boolean l_bCompletedBuffer=false;

	if(!m_rDriverContext.isConnected()) { return false; }
	// if(!m_rDriverContext.isStarted()) { return true; }

	SAmpServerPacketHeader l_oHeader;
	SAmpServerPacketHeader l_oHeaderSwap;

	while(m_pStream->isReadyToReceive() && !l_bCompletedBuffer)
	{
		m_pStream->receiveBufferBlocking(reinterpret_cast<char*>(&l_oHeaderSwap), sizeof(l_oHeaderSwap));
		System::Memory::bigEndianToHost(reinterpret_cast<const uint8*>(&l_oHeaderSwap.m_i64AmplifierIdentifier), &l_oHeader.m_i64AmplifierIdentifier);
		System::Memory::bigEndianToHost(reinterpret_cast<const uint8*>(&l_oHeaderSwap.m_ui64PacketSize), &l_oHeader.m_ui64PacketSize);

		if(l_oHeader.m_ui64PacketSize)
		{
			float32* l_pBufferSwap=new float32[l_oHeader.m_ui64PacketSize/sizeof(float32)];
			m_pStream->receiveBufferBlocking(reinterpret_cast<char*>(l_pBufferSwap), l_oHeader.m_ui64PacketSize);

			if(m_rDriverContext.isStarted())
			{
				uint32 l_ui32SampleCount=l_oHeader.m_ui64PacketSize/1152;
				for(i=0; i<l_ui32SampleCount; i++)
				{
					for(j=0; j<m_ui32ChannelCount; j++)
					{
						System::Memory::bigEndianToHost(reinterpret_cast<const uint8*>(l_pBufferSwap+8+i*288+j), m_pBuffer+j*m_ui32SampleCountPerSentBlock+m_ui32SampleIndex);
					}

					m_ui32SampleIndex++;
					if(m_ui32SampleIndex==m_ui32SampleCountPerSentBlock)
					{
						l_bCompletedBuffer=true;
						m_ui32SampleIndex=0;
						m_pCallback->setSamples(m_pBuffer);
						m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
					}
				}
			}

			delete [] l_pBufferSwap;
		}
	}

	return true;
}

boolean CDriverEGIAmpServer::stop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

	this->exectue("../share/openvibe-applications/acquisition-server/scripts/egi-default-stop.script");

	return true;
}

boolean CDriverEGIAmpServer::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	delete [] m_pBuffer;
	m_pBuffer=NULL;

	if(m_pStream)
	{
		CCommandConnectionHandler l_oStreamHandler(*this, m_pStream);
		l_oStreamHandler.send("(sendCommand cmd_StopListeningToAmp 0 -1 -1)");

		m_pStream->close();
		m_pStream->release();
		m_pStream=NULL;
	}

	if(m_pCommand)
	{
		CCommandConnectionHandler l_oCommandHandler(*this);
		l_oCommandHandler.send("(sendCommand cmd_Stop 0 -1 -1)");

		this->exectue("../share/openvibe-applications/acquisition-server/scripts/egi-default-uninitialize.script");

		m_pCommand->close();
		m_pCommand->release();
		m_pCommand=NULL;
	}

	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverEGIAmpServer::isConfigurable(void)
{
	return true;
}

boolean CDriverEGIAmpServer::configure(void)
{
	CConfigurationEGIAmpServer m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-egi-ampserver.ui");

	m_oConfiguration.setHostName(m_sAmpServerHostName);
	m_oConfiguration.setCommandPort(m_ui32CommandPort);
	m_oConfiguration.setStreamPort(m_ui32StreamPort);

	if(m_oConfiguration.configure(m_oHeader))
	{
		m_sAmpServerHostName=m_oConfiguration.getHostName();
		m_ui32CommandPort=m_oConfiguration.getCommandPort();
		m_ui32StreamPort=m_oConfiguration.getStreamPort();
		return true;
	}
	return false;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverEGIAmpServer::exectue(const char* sScriptFilename)
{
	FILE* l_pFile=::fopen(sScriptFilename, "rb");
	if(!l_pFile)
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Could not find script [" << CString(sScriptFilename) << "]\n";
		return false;
	}

	CCommandConnectionHandler l_oCommandHandler(*this);
	while(!::feof(l_pFile))
	{
		char l_sLineBuffer[4096];
		char* l_sLine=::fgets(l_sLineBuffer, sizeof(l_sLineBuffer), l_pFile);
		if(l_sLine)
		{
			l_sLine[::strlen(l_sLine)-1]='\0'; // replaces \n with \0 (\n is automatically added by the command handler)
			l_oCommandHandler.send(l_sLine);
		}
	}

	return true;
}
