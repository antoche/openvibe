#include "ovasCDriverMicromedSystemPlusEvolution.h"
#include "../ovasCConfigurationNetworkBuilder.h"

#if defined OVAS_OS_Windows

#define MicromedDLL "\\dllMicromed.dll"

#include <system/Time.h>

#include <cmath>

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <windows.h>
#include <cstring>

#define boolean OpenViBE::boolean
using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//

typedef char*               ( __stdcall * STRUCTHEADER)         ();
typedef int                 ( __stdcall * STRUCTHEADERSIZE)     ();
typedef char*               ( __stdcall * STRUCTHEADERINFO)     ();
typedef int                 ( __stdcall * STRUCTHEADERINFOSIZE) ();
typedef unsigned short int* ( __stdcall * STRUCTBUFFDATA)       ();
typedef int                 ( __stdcall * STRUCTBUFFDATASIZE)   ();
typedef boolean             ( __stdcall * HEADERVALID)          ();
typedef boolean             ( __stdcall * DATAHEADER)           ();
typedef boolean             ( __stdcall * INITHEADER)           ();
typedef unsigned int        ( __stdcall * DATALENGTH)           ();
typedef unsigned int        ( __stdcall * ADDRESSOFDATA)        ();
typedef unsigned int        ( __stdcall * NBOFCHANNELS)         ();
typedef unsigned int        ( __stdcall * MINSAMPLINGRATE)      ();
typedef unsigned int        ( __stdcall * SIZEOFEACHDATAINBYTE) ();
typedef float               ( __stdcall * DATAVALUE)(unsigned short* buffData, int numChannel, int numSample);

// Header  Structure
//---------------------------

//pointer on the header structure. the header structure is send before any HeaderInfo or BuffData structure.
STRUCTHEADER m_oFgetStructHeader;

//give the size of the structHeader
STRUCTHEADERSIZE m_oFgetStructHeaderSize;

//say if the last structHeader is valid.
//must be call just after structHeader.
HEADERVALID m_oFisHeaderValid;

//say if the Header structure is following by a Buffer Data structure
//must be call after structHeader.
DATAHEADER m_oFisDataHeader;

//say if the Header structure is following by a Header Info Structure
//must be call after structHeader.
INITHEADER m_oFisInitHeader;

//give the size of the following data receive
//must be call after structHeader.
DATALENGTH m_oFgetDataLength;

//   Header Info Structure
//---------------------------

//give many information of the device configuration.
STRUCTHEADERINFO m_oFgetStructHeaderInfo;

//give the size of the structHeaderInfo
STRUCTHEADERINFOSIZE m_oFgetStructHeaderInfoSize;

//give address of the first data. the address count start to the beginning of this structure.
//it necessary to receive information between this structure and the first address data before beginning.
//must be call after structHeaderInfo.
ADDRESSOFDATA m_oFgetAddressOfData;

//give the number of channels connected to this device.
//must be call after structHeaderInfo.
NBOFCHANNELS m_oFgetNbOfChannels;

//give the sampling rate of the device
//must be call after structHeaderInfo.
MINSAMPLINGRATE m_oFgetMinimumSamplingRate;

//give the size in byte for one sample of one channels
//must be call after structHeaderInfo.
SIZEOFEACHDATAINBYTE m_oFgetSizeOfEachDataInByte;

//Give the value of the samples and channel specify
DATAVALUE m_oFgetDataValue;

//   Buffer Data Structure
//---------------------------

//give sample of channels
STRUCTBUFFDATA m_oFgetStructBuffData;
//give the size of the Data buffer
STRUCTBUFFDATASIZE m_oFgetStructBuffDataSize;

//lib
HINSTANCE m_oLibMicromed; //Library Handle

//reg key
namespace
{
	char g_sTCPPortNumber[1024];
	char g_sTCPSendAcq[1024];
	char g_sTCPServerName[1024];
	HKEY g_hRegistryKey=NULL;
//	boolean g_bInitializedFromRegistry=false;
	const char* g_sRegisteryKeyName="Software\\VB and VBA Program Settings\\Brain Quick - System 98\\EEG_Settings";
}

#define __load_dll_func__(var, type, name) \
	var = (type)::GetProcAddress(m_oLibMicromed, name); \
	if(!var) \
	{ \
		m_rDriverContext.getLogManager() << LogLevel_Error << "Load method " << name << "\n"; \
		m_bValid=false; \
		return; \
	}

CDriverMicromedSystemPlusEvolution::CDriverMicromedSystemPlusEvolution(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_bValid(true)
	,m_pConnectionServer(NULL)
	,m_ui32ServerHostPort(3000)
	,m_pConnection(NULL)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
{
	//load the ddl of the driver
	m_oLibMicromed = NULL ;
	TCHAR l_sPath[1024];

	//Open libratry
	::GetCurrentDirectory(1024, l_sPath);
	::lstrcat(l_sPath, "\\..\\bin");
	::lstrcat(l_sPath, MicromedDLL);
	m_oLibMicromed = ::LoadLibrary(l_sPath);

	//if it can't be open return FALSE;
	if( m_oLibMicromed == NULL)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Couldn't load DLL: " << CString(l_sPath) << "\n";
		return;
	}

	//load the method for initialized the driver
	__load_dll_func__(m_oFgetStructHeader, STRUCTHEADER, "getStructHeader");
	__load_dll_func__(m_oFgetStructHeaderSize, STRUCTHEADERSIZE, "getStructHeaderSize");
	__load_dll_func__(m_oFgetStructHeaderInfo, STRUCTHEADERINFO, "getStructHeaderInfo");
	__load_dll_func__(m_oFgetStructHeaderInfoSize, STRUCTHEADERINFOSIZE, "getStructHeaderInfoSize");
	__load_dll_func__(m_oFgetStructBuffData, STRUCTBUFFDATA, "getStructBuffData");
	__load_dll_func__(m_oFgetStructBuffDataSize, STRUCTBUFFDATASIZE, "getStructBuffDataSize");
	__load_dll_func__(m_oFisHeaderValid, HEADERVALID, "isHeaderValid");
	__load_dll_func__(m_oFisDataHeader, DATAHEADER, "isDataHeader");
	__load_dll_func__(m_oFisInitHeader, INITHEADER, "isInitHeader");
	__load_dll_func__(m_oFgetDataLength, DATALENGTH, "getDataLength");
	__load_dll_func__(m_oFgetAddressOfData, ADDRESSOFDATA, "getAddressOfData");

	__load_dll_func__(m_oFgetNbOfChannels, NBOFCHANNELS, "getNbOfChannels");
	__load_dll_func__(m_oFgetMinimumSamplingRate, MINSAMPLINGRATE, "getMinimumSamplingRate");
	__load_dll_func__(m_oFgetSizeOfEachDataInByte, SIZEOFEACHDATAINBYTE, "getSizeOfEachDataInByte");
	__load_dll_func__(m_oFgetDataValue, DATAVALUE, "getDataValue");

	m_rDriverContext.getLogManager() << LogLevel_Trace << "Succeeded in loading DLL: " << CString(l_sPath) << "\n";
	m_pStructHeader=m_oFgetStructHeader();
	m_pStructHeaderInfo=m_oFgetStructHeaderInfo();
	m_pStructBuffData=m_oFgetStructBuffData();

#if 1
	if(ERROR_SUCCESS!=::RegOpenKeyEx(HKEY_CURRENT_USER, g_sRegisteryKeyName, 0, KEY_QUERY_VALUE, &g_hRegistryKey))
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString(g_sRegisteryKeyName) << " is not initialized\n";
		::strcpy(g_sTCPPortNumber, "");
		::strcpy(g_sTCPSendAcq, "");
		::strcpy(g_sTCPServerName, "");
		return;
	}

	DWORD taille=sizeof(g_sTCPPortNumber);

	if(ERROR_SUCCESS!=::RegQueryValueEx(g_hRegistryKey, "tcpPortNumber", NULL, NULL, (LPBYTE)g_sTCPPortNumber, &taille))
	{
		::strcpy(g_sTCPPortNumber, "");
	}
	else
	{
		m_ui32ServerHostPort=::atoi(g_sTCPPortNumber);
	}

	if(ERROR_SUCCESS!=::RegQueryValueEx(g_hRegistryKey, "tcpSendAcq", NULL, NULL, (LPBYTE)g_sTCPSendAcq, &taille))
	{
		::strcpy(g_sTCPSendAcq, "");
	}

	if(ERROR_SUCCESS!=::RegQueryValueEx(g_hRegistryKey, "tcpServerName", NULL, NULL, (LPBYTE)g_sTCPServerName, &taille))
	{
		::strcpy(g_sTCPServerName, "");
	}

	::RegCloseKey(g_hRegistryKey);
	g_hRegistryKey=NULL;
//	g_bInitializedFromRegistry=true;
#endif
}

CDriverMicromedSystemPlusEvolution::~CDriverMicromedSystemPlusEvolution(void)
{
	if(m_pConnectionServer)
	{
		m_pConnectionServer->release();
		m_pConnectionServer=NULL;
	}

/**
 * Is it necessary to restore the
 * registry keys here ?
 */

#if 0
	if(ERROR_SUCCESS!=::RegOpenKeyEx(HKEY_CURRENT_USER, g_sRegisteryKeyName, 0, KEY_WRITE, &g_hRegistryKey))
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString(g_sRegisteryKeyName) << " not restored\n";
	}

	if(g_sTCPPortNumber!=std::string(""))
	{
		if(ERROR_SUCCESS!=::RegSetValueEx(g_hRegistryKey, "tcpPortNumber", 0, REG_SZ, (LPBYTE)g_sTCPPortNumber, ::strlen(g_sTCPPortNumber)))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpPortNumber") << " not restored\n";
		}
	}
	else
	{
		if(ERROR_SUCCESS!=::RegDeleteValue(g_hRegistryKey, "tcpPortNumber"))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpPortNumber") << " not deleted\n";
		}
	}

	if(g_sTCPSendAcq!=std::string(""))
	{
		if(ERROR_SUCCESS!=::RegSetValueEx(g_hRegistryKey, "tcpSendAcq", 0, REG_SZ, (LPBYTE)g_sTCPSendAcq, ::strlen(g_sTCPSendAcq)))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpSendAcq") << " not restored\n";
		}
	}
	else
	{
		if(ERROR_SUCCESS!=::RegDeleteValue(g_hRegistryKey, "tcpSendAcq"))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpSendAcq") << " not deleted\n";
		}
	}

	if(g_sTCPServerName!=std::string(""))
	{
		if(ERROR_SUCCESS!=::RegSetValueEx(g_hRegistryKey, "tcpServerName", 0, REG_SZ, (LPBYTE)g_sTCPServerName, ::strlen(g_sTCPServerName)))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpServerName") << " not restored\n";
		}
	}
	else
	{
		if(ERROR_SUCCESS!=::RegDeleteValue(g_hRegistryKey, "tcpServerName"))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpServerName") << " not deleted\n";
		}
	}

	::RegCloseKey(g_hRegistryKey);
	g_hRegistryKey=NULL;
#endif
}

const char* CDriverMicromedSystemPlusEvolution::getName(void)
{
	return "Micromed SD LTM (through SystemPlus Evolution)";
}

short CDriverMicromedSystemPlusEvolution::MyReceive(char* buf, long dataLen)
{
	long nDati=0;
	long recByte;
	while (nDati < dataLen)
	{
		recByte =m_pConnection->receiveBuffer((&buf[nDati]), dataLen - nDati);
		if (recByte == 0)
		{
			return -1;
		}

		nDati += recByte;
		m_rDriverContext.getLogManager() << LogLevel_Debug << "Received Data: = " << nDati << " /" << dataLen << "\n";
	}
	return 0;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMicromedSystemPlusEvolution::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(!m_bValid) { return false; }
	if(m_rDriverContext.isConnected()) { return false; }

	// Initialize var for connection
	uint32 l_ui32Listen = 0;

	//update register key
	if(ERROR_SUCCESS!=::RegOpenKeyEx(HKEY_CURRENT_USER, g_sRegisteryKeyName, 0, KEY_WRITE, &g_hRegistryKey))
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key not initialized\n";
	}
	else
	{
		char l_sServerHostPort[1024];
		::sprintf(l_sServerHostPort, "%i", m_ui32ServerHostPort);

		if(ERROR_SUCCESS!=::RegSetValueEx(g_hRegistryKey, "tcpPortNumber", 0, REG_SZ, (LPBYTE)l_sServerHostPort, ::strlen(l_sServerHostPort)))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpPortNumber") << " not initialized to '" << m_ui32ServerHostPort << "'\n";
		}

		if(g_sTCPSendAcq==std::string())
		{
			char* l_sAcq="1";
			if(ERROR_SUCCESS!=::RegSetValueEx(g_hRegistryKey, "tcpSendAcq", 0, REG_SZ, (LPBYTE)l_sAcq, 1))
			{
				m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpSendAcq") << " not initialized to '1'\n";
			}
		}

		if(g_sTCPServerName==std::string())
		{
			::strcpy(g_sTCPServerName, "localhost");
			if(ERROR_SUCCESS!=::RegSetValueEx(g_hRegistryKey, "tcpServerName", 0, REG_SZ, (LPBYTE)g_sTCPServerName, ::strlen(g_sTCPServerName)))
			{
				m_rDriverContext.getLogManager() << LogLevel_Warning << "Registery key " << CString("tcpServerName") << " not initialized to '" << CString(g_sTCPServerName) << "'\n";
			}
		}

		::RegCloseKey(g_hRegistryKey);
		g_hRegistryKey=NULL;
	}

	m_rDriverContext.getLogManager() << LogLevel_Trace << "Configure Register key\n";

	// Builds up server connection
	m_pConnectionServer=Socket::createConnectionServer();

	if(!m_pConnectionServer)
	{
		m_rDriverContext.getLogManager() << LogLevel_ImportantWarning << "> Could not create server socket\n";
		return false;
	}
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "> Server is on \n";

		// Server start listening on defined port
		if(!m_pConnectionServer->listen(m_ui32ServerHostPort))
		{
			m_rDriverContext.getLogManager() << LogLevel_ImportantWarning << "> Could not listen TCP port " << m_ui32ServerHostPort << "\n";

			// Cleans up server connection
			m_pConnectionServer->close();
			m_pConnectionServer->release();
			m_pConnectionServer=NULL;

			return false;
		}
	}

	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_pCallback=&rCallback;

	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Server is listening on port : " << m_ui32ServerHostPort << "\n";

	uint32 l_ui32TimeOutMilliseconds=m_rDriverContext.getConfigurationManager().expandAsInteger("${AcquisitionServer_Driver_MicromedTimeOut}", 5000);
	if(m_pConnectionServer->isReadyToReceive(l_ui32TimeOutMilliseconds))
	{
		// Accept new client
		m_pConnection=m_pConnectionServer->accept();
	}
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "> Time out after " << l_ui32TimeOutMilliseconds << " milliseconds\n";

		// Cleans up server connection
		m_pConnectionServer->close();
		m_pConnectionServer->release();
		m_pConnectionServer=NULL;

		return false;
	}

	// Receive Header
	this->MyReceive(m_pStructHeader, m_oFgetStructHeaderSize());
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Receiving Header....\n";

	// Verify header validity
	if (!m_oFisHeaderValid())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb with fixCode\n";
		return false;
	}

	if (!m_oFisInitHeader())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb not receive Init information\n";
		return false;
	}

	if(m_oFgetStructHeaderInfoSize()!=m_oFgetDataLength())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : pb the data header Info hasn't the good size\n the structure size:" << m_oFgetStructHeaderInfoSize() << "the size of data received:" << m_oFgetDataLength() << "\n";
		return false;
	}

	// Receive Header
	this->MyReceive(m_pStructHeaderInfo, m_oFgetStructHeaderInfoSize());
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";

	m_oHeader.setChannelCount(m_oFgetNbOfChannels());

	m_oHeader.setSamplingFrequency((uint32)m_oFgetMinimumSamplingRate());

	m_rDriverContext.getLogManager() << LogLevel_Trace << "size for 1 channel, 1 block: " << m_ui32SampleCountPerSentBlock << "\n";
	m_rDriverContext.getLogManager() << LogLevel_Trace << "number of channels: " << m_oHeader.getChannelCount() << "\n";
	m_rDriverContext.getLogManager() << LogLevel_Trace << "Maximum sample rate =" << m_oHeader.getSamplingFrequency() << " Hz" << "\n";
	m_pSample=new float32[m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock];
	m_rDriverContext.getLogManager() << LogLevel_Trace << "size of m_pSample=" << (m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock*sizeof(float32)) << "\n";
	m_rDriverContext.getLogManager() << LogLevel_Trace << "Maximum Buffer size =" << (m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock*sizeof(signed short int)) << " Samples" << "\n";

	if(!m_pSample)
	{
		m_rDriverContext.getLogManager() << LogLevel_ImportantWarning << "Could not allocate sample buffer\n";
		uninitialize();
		return false;
	}
	m_ui32BuffDataIndex = 0;

	return true;
}

boolean CDriverMicromedSystemPlusEvolution::start(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "start device\n";
	if(!m_bValid) { return false; }
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }
	if(!m_pConnection) { return false; }
	return true;
}

boolean CDriverMicromedSystemPlusEvolution::dropData(void)
{
	//drop data
	uint32 l_ui32TotalReceived=0;
	do
	{
		uint32 l_ui32MaxByteRecv=min(m_oFgetStructBuffDataSize(), m_oFgetDataLength()-l_ui32TotalReceived);
		this->MyReceive((char*)m_pStructBuffData, l_ui32MaxByteRecv);
		l_ui32TotalReceived+=l_ui32MaxByteRecv;
	}
	while(l_ui32TotalReceived<m_oFgetDataLength());
	return true;
}

boolean CDriverMicromedSystemPlusEvolution::loop(void)
{
	if(!m_bValid) { return false; }
	if(!m_rDriverContext.isConnected()) { return false; }

	if(m_pConnection)
	{
		// Receive Header
		this->MyReceive(m_pStructHeader, m_oFgetStructHeaderSize());
		m_rDriverContext.getLogManager() << LogLevel_Trace << "> Header received\n";

		// Verify header validity
		if (!m_oFisHeaderValid())
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form\n";
			return false;
		}
		if (!m_oFisDataHeader())
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Header received not in correct form : problem with infoType\n";
			return false;
		}

		// Receive Data
		uint32 l_ui32MaxByteRecv=0;
		uint32 l_ui32TotalReceived=0;
		uint32 l_ui32nbSamplesBlock=m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock;
		uint32 l_ui32DataSizeInByte=m_oFgetSizeOfEachDataInByte();
		uint32 l_ui32ReceivedSampleCount=0;
		uint32 l_ui32BuffSize=m_oFgetStructBuffDataSize();

		//if the device is not start or the first block after start haven't been received, data will be dropped
		if(!m_rDriverContext.isStarted())
		{
			dropData();
			m_rDriverContext.getLogManager() << LogLevel_Debug << "Device not started, dropped data: data.len = " << m_oFgetDataLength() << "\n";
			return true;
		}

		if(m_rDriverContext.isStarted())
		{
			do
			{
				l_ui32MaxByteRecv=min(l_ui32BuffSize, min(m_oFgetDataLength()-l_ui32TotalReceived, (l_ui32nbSamplesBlock-m_ui32BuffDataIndex*m_oHeader.getChannelCount())*l_ui32DataSizeInByte));
				this->MyReceive((char*)m_pStructBuffData, l_ui32MaxByteRecv);
				l_ui32ReceivedSampleCount=l_ui32MaxByteRecv/(l_ui32DataSizeInByte*m_oHeader.getChannelCount());
				m_rDriverContext.getLogManager() << LogLevel_Debug << "Number of Samples Received:" << l_ui32ReceivedSampleCount << "\n";

				for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
				{
					m_rDriverContext.getLogManager() << LogLevel_Debug << "channel[" << i << "]={" ;
					for(uint32 j=0; j<l_ui32ReceivedSampleCount; j++)
					{
						m_pSample[m_ui32BuffDataIndex+j + i*m_ui32SampleCountPerSentBlock] = (float32)m_oFgetDataValue(m_pStructBuffData, i, j);
						m_rDriverContext.getLogManager() << LogLevel_Debug << m_pSample[m_ui32BuffDataIndex+j + i*m_ui32SampleCountPerSentBlock] << "; ";
					}
					m_rDriverContext.getLogManager() << LogLevel_Debug << "}" << "\n";
				}

				m_rDriverContext.getLogManager() << LogLevel_Debug << "Convert Data: dataConvert = " << l_ui32TotalReceived << "/" << m_oFgetDataLength() << "\n";
				l_ui32TotalReceived+=l_ui32MaxByteRecv;
				m_ui32BuffDataIndex+=l_ui32ReceivedSampleCount;
				m_rDriverContext.getLogManager() << LogLevel_Debug << "Convert Data: dataConvert = " << l_ui32TotalReceived << "/" << m_oFgetDataLength() << "\n";

				if(l_ui32nbSamplesBlock<m_ui32BuffDataIndex)
				{
					m_rDriverContext.getLogManager() << LogLevel_Error << "Data not received in correct form : problem with lenData\n";
					return false;
				}

				if(l_ui32nbSamplesBlock==m_ui32BuffDataIndex*m_oHeader.getChannelCount())
				{
					m_pCallback->setSamples(m_pSample);
					m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
					m_rDriverContext.getLogManager() << LogLevel_Debug << "Send samples back to CAcquisitionServer: samples.len = " << m_ui32BuffDataIndex << "\n";
					m_ui32BuffDataIndex=0;
				}
			} while(l_ui32TotalReceived<m_oFgetDataLength());
		}
	}
	return true;

}

boolean CDriverMicromedSystemPlusEvolution::stop(void)
{
	if(!m_bValid) { return false; }
	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Server stopped\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }
	return true;
}

boolean CDriverMicromedSystemPlusEvolution::uninitialize(void)
{
	if(!m_bValid) { return false; }
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	if(m_pSample)
	{
		delete [] m_pSample;
		m_pSample=NULL;
		m_pCallback=NULL;
	}

	// Cleans up client connection
	if(m_pConnection)
	{
		m_pConnection->close();
		m_pConnection->release();
		m_pConnection=NULL;
	}

	// Cleans up server connection
	if(m_pConnectionServer)
	{
		m_pConnectionServer->close();
		m_pConnectionServer->release();
		m_pConnectionServer=NULL;
	}

	m_rDriverContext.getLogManager() << LogLevel_Trace << "> Server disconnected\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMicromedSystemPlusEvolution::isConfigurable(void)
{
	return true;
}

boolean CDriverMicromedSystemPlusEvolution::configure(void)
{
	CConfigurationNetworkBuilder l_oConfiguration("../share/openvibe-applications/acquisition-server/interface-Micromed-SystemPlusEvolution.ui");

	l_oConfiguration.setHostPort(m_ui32ServerHostPort);

	if(l_oConfiguration.configure(m_oHeader))
	{
		m_ui32ServerHostPort=l_oConfiguration.getHostPort();
		return true;
	}

	return false;
}
#endif
