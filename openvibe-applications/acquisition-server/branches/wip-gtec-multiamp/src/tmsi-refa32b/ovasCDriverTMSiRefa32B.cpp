#include "ovasCDriverTMSiRefa32B.h"
#include "ovasCConfigurationTMSIRefa32B.h"

#if defined OVAS_OS_Windows

#include <openvibe-toolkit/ovtk_all.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <system/Time.h>
#include "ovasCConfigurationTMSIRefa32B.h"
#include <Windows.h>
#define boolean OpenViBE::boolean

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________//
//                                                           //

//structure define in the DLL

typedef struct _SP_DEVICE_PATH
{
	DWORD  dwCbSize;
	TCHAR  devicePath[1];
} SP_DEVICE_PATH, *PSP_DEVICE_PATH;

typedef struct _FeatureData
{
	ULONG FeatureId;
	ULONG Info;
} FEATURE_DATA, *PFEATURE_DATA;

typedef struct _SYSTEM_TIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEM_TIME;

typedef struct _SIGNAL_FORMAT
{
	ULONG Size;      // Size of this structure
	ULONG Elements;  // Number of elements in list

	ULONG Type;      // One of the signal types above
	ULONG SubType;   // One of the signal sub-types above
	ULONG Format;    // Float / Integer / Asci / Ect..
	ULONG Bytes;     // Number of bytes per sample including subsignals

	FLOAT UnitGain;
	FLOAT UnitOffSet;
	ULONG UnitId;
	LONG UnitExponent;

	WCHAR Name[SIGNAL_NAME];

	ULONG Port;
	WCHAR PortName[SIGNAL_NAME];
	ULONG SerialNumber;
} SIGNAL_FORMAT, *PSIGNAL_FORMAT;

typedef struct _FeatureMemory{
	FEATURE_DATA Feature;
	ULONG Data[1];
}FEATURE_MEMORY, *PFEATURE_MEMORY;

typedef struct _FeatureMode{
	FEATURE_DATA Feature;
	ULONG Mode;
}FEATURE_MODE,*PFEATURE_MODE;

//___________________________________________________________//
//                                                           //

//methods define in the DLL

typedef HANDLE          ( __stdcall * POPEN)            (PSP_DEVICE_PATH DevicePath);
typedef BOOL            ( __stdcall * PCLOSE)           (HANDLE hHandle);
typedef ULONG           ( __stdcall * PGETDEVICESTATE)  (IN HANDLE Handle);
typedef BOOLEAN         ( __stdcall * PSTART)           (IN HANDLE Handle);
typedef BOOLEAN         ( __stdcall * PRESETDEVICE)     (IN HANDLE Handle);
typedef BOOLEAN         ( __stdcall * PSTOP)            (IN HANDLE Handle);
typedef HANDLE          ( __stdcall * PGETSLAVEHANDLE)  (IN HANDLE Handle);
typedef BOOLEAN         ( __stdcall * PADDSLAVE)        (IN HANDLE Handle, IN HANDLE SlaveHandle);
typedef PSIGNAL_FORMAT  ( __stdcall * PGETSIGNALFORMAT) (IN HANDLE Handle, IN OUT PSIGNAL_FORMAT pSignalFormat);
typedef BOOLEAN         ( __stdcall * PSETSIGNALBUFFER) (IN HANDLE Handle, IN OUT PULONG SampleRate, IN OUT PULONG BufferSize);
typedef ULONG           ( __stdcall * PGETSAMPLES)      (IN HANDLE Handle, OUT PULONG SampleBuffer, IN ULONG Size);
typedef BOOLEAN         ( __stdcall * PGETBUFFERINFO)   (IN HANDLE Handle, OUT PULONG Overflow, OUT PULONG PercentFull);
typedef BOOLEAN         ( __stdcall * PDEVICEFEATURE)   (IN HANDLE Handle, IN LPVOID DataIn, IN DWORD InSize, OUT LPVOID DataOut, IN DWORD OutSize);
typedef PSP_DEVICE_PATH ( __stdcall * PGETINSTANCEID)   (IN LONG DeviceIndex, IN BOOLEAN Present, OUT ULONG  *MaxDevices );
typedef HKEY            ( __stdcall * POPENREGKEY)      (IN PSP_DEVICE_PATH Path );
typedef BOOL            ( __stdcall * PFREE)            (IN VOID *Memory);

//___________________________________________________________//
//                                                           //

//vars used for load the DLL's methods
POPEN m_oFpOpen;
PCLOSE m_oFpClose;
PGETDEVICESTATE m_oFpGetDeviceState;
PSTART m_oFpStart;
PRESETDEVICE m_oFpReset;
PSTOP m_oFpStop;
PGETSLAVEHANDLE m_oFpGetSlaveHandle;
PADDSLAVE m_oFpAddSlave;
PGETSIGNALFORMAT m_oFpGetSignalFormat;
PSETSIGNALBUFFER m_oFpSetSignalBuffer;
PGETSAMPLES m_oFpGetSamples;
PGETBUFFERINFO m_oFpGetBufferInfo;
PDEVICEFEATURE m_oFpDeviceFeature;
PGETINSTANCEID m_oFpGetInstanceId;
POPENREGKEY m_oFpOpenRegKey;
PFREE m_oFpFree;

//___________________________________________________________//
//                                                           //

//  Handle
//----------

//Device Handle Master
HANDLE m_HandleMaster;

//Device Handle Slave
std::vector <HANDLE> m_vHandleSlaves;

//  lib
//--------

HINSTANCE m_oLibHandle; //Library Handle

//  Buffer for storing the samples
//----------------------------------
ULONG *m_ulSignalBuffer;
LONG *m_lSignalBuffer;
boolean m_bSignalBufferUnsigned;
ULONG m_ulSampleRate ;
ULONG m_ulBufferSize ;

//  device
//----------
map <PSP_DEVICE_PATH,string> m_vDevicePathMap; //m_vDevicePathMap contains all connected devicePath and their name
string m_pDevicePathMaster; //the name of the Master devicePath chosen
vector <string> m_vDevicePathSlave; //a list with the name of the Slave devicePath chosen
ULONG m_lNrOfDevicesConnected; // Number of devices on this PC
ULONG m_lNrOfDevicesOpen; //total of Master/slaves device open

//  store value for calculate the data
//--------------------------------------
vector <LONG > m_vExponentChannel;
vector <FLOAT> m_vUnitGain;
vector <FLOAT> m_vUnitOffSet;

//number of channels
ULONG m_ui32NbTotalChannels;

uint32 m_ui32BufferSize;

#define __load_dll_func__(var, type, name) \
	var = (type)::GetProcAddress(m_oLibHandle, name); \
	if(!var) \
	{ \
		m_rDriverContext.getLogManager() << LogLevel_Error << "Load method " << name << "\n"; \
		m_bValid=false; \
		return; \
	}

CDriverTMSiRefa32B::CDriverTMSiRefa32B(IDriverContext& rDriverContext)
:IDriver(rDriverContext)
 ,m_pCallback(NULL)
 ,m_ui32SampleCountPerSentBlock(0)
 ,m_ui32SampleIndex(0)
 ,m_i32NumOfTriggerChannel(-1)
 ,m_ui32LastTriggerValue(255)
 ,m_bValid(true)
 {
	m_oHeader.setSamplingFrequency(512);
	m_oHeader.setChannelCount(32);

	//load the DLL of the driver
	m_oLibHandle = NULL ;
	m_HandleMaster=NULL;
	TCHAR l_sPath[ 1024 ];

	//Open library
	GetCurrentDirectory(1024, l_sPath);
	::lstrcat(l_sPath,"\\..\\bin");
	::lstrcat(l_sPath,RTLOADER);
	m_oLibHandle = ::LoadLibrary(l_sPath);

	//if it can't be open return FALSE;
	if( m_oLibHandle == NULL)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Couldn't load DLL: " << CString(l_sPath) << "\n";
		return;
	}

	//load DLL methods for initialized the driver
	__load_dll_func__(m_oFpOpen, POPEN, "Open");
	__load_dll_func__(m_oFpClose, PCLOSE, "Close");
	__load_dll_func__(m_oFpGetDeviceState, PGETDEVICESTATE, "GetDeviceState");
	__load_dll_func__(m_oFpStart, PSTART, "Start");
	__load_dll_func__(m_oFpReset, PRESETDEVICE, "ResetDevice");
	__load_dll_func__(m_oFpStop, PSTOP, "Stop");
	__load_dll_func__(m_oFpGetSlaveHandle, PGETSLAVEHANDLE, "GetSlaveHandle");
	__load_dll_func__(m_oFpAddSlave, PADDSLAVE, "AddSlave");
	__load_dll_func__(m_oFpGetSignalFormat, PGETSIGNALFORMAT, "GetSignalFormat");
	__load_dll_func__(m_oFpSetSignalBuffer, PSETSIGNALBUFFER, "SetSignalBuffer");
	__load_dll_func__(m_oFpGetSamples, PGETSAMPLES, "GetSamples");
	__load_dll_func__(m_oFpGetBufferInfo, PGETBUFFERINFO, "GetBufferInfo");
	__load_dll_func__(m_oFpDeviceFeature, PDEVICEFEATURE, "DeviceFeature");

	__load_dll_func__(m_oFpGetInstanceId, PGETINSTANCEID, "GetInstanceId" );
	__load_dll_func__(m_oFpOpenRegKey, POPENREGKEY, "OpenRegKey" );
	__load_dll_func__(m_oFpFree, PFREE, "Free" );

	m_rDriverContext.getLogManager() << LogLevel_Trace << "Succeeded in loading DLL: " << CString(l_sPath) << "\n";
	m_pDevicePathMaster = "";
	m_lNrOfDevicesOpen=0;
	m_bCheckImpedance=m_rDriverContext.getConfigurationManager().expandAsBoolean("${AcquisitionServer_CheckImpedance}", false);
}

CDriverTMSiRefa32B::~CDriverTMSiRefa32B(void)
{
	this->uninitialize();
}

void CDriverTMSiRefa32B::release(void)
{
	delete this;
}

const char* CDriverTMSiRefa32B::getName(void)
{
	return "TMSi Refa32B";
}

boolean CDriverTMSiRefa32B::initialize(
		const uint32 ui32SampleCountPerSentBlock,
		IDriverCallback& rCallback)
{
	if(!m_bValid) { return false; }
	if(m_rDriverContext.isConnected()) { return false; }

	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Initialized TMSI\n";

	if(m_HandleMaster != NULL)
	{
		m_oFpClose(m_HandleMaster);
		m_HandleMaster = NULL;
	}

	//Refresh information about connected device
	if(!refreshDevicePath())
	{
		return false;
	}

	if(m_lNrOfDevicesConnected==0)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "There is no device connected to the PC\n";
		return false;
	}

	//open master
	map<PSP_DEVICE_PATH, string>::iterator iter=m_vDevicePathMap.begin();
	bool l_bMasterFind=false;
	while(m_pDevicePathMaster.compare("")!=0 && !l_bMasterFind && iter!=m_vDevicePathMap.end())
	{
		if((*iter).second.compare(m_pDevicePathMaster)==0)
		{
			l_bMasterFind=true;
			m_HandleMaster = m_oFpOpen((*iter).first);
			if(!m_HandleMaster)
			{
				m_rDriverContext.getLogManager() << LogLevel_Trace << "Open Driver\n";
				return false;
			}
			m_lNrOfDevicesOpen++;
		}
		iter++;
	}
	if(m_HandleMaster == NULL && m_lNrOfDevicesConnected>0){
		m_HandleMaster=m_oFpOpen((*m_vDevicePathMap.begin()).first);
		if(!m_HandleMaster)
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Open Driver\n";
			return false;
		}
		m_lNrOfDevicesOpen++;
	}

	//open slave
	for(uint32 i=0;i<m_vDevicePathSlave.size();i++)
	{
		map<PSP_DEVICE_PATH, string>::iterator j=m_vDevicePathMap.begin();
		bool find=false;
		while(!find&&j!=m_vDevicePathMap.end())
		{
			if((*j).second.compare(m_pDevicePathMaster)!=0&&
					m_vDevicePathSlave[i].compare((*j).second)==0)
			{
				find=true;

				//open slave driver
				m_vHandleSlaves.push_back(m_oFpOpen((*j).first));
				m_oFpAddSlave(m_HandleMaster,m_vHandleSlaves[m_vHandleSlaves.size()-1]);
				m_lNrOfDevicesOpen++;
			}
			j++;
		}
	}

	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">size for 1 channel, 1 block: "<<m_ui32SampleCountPerSentBlock<<"\n";


	//initialized the buffer
	m_ulSampleRate = m_oHeader.getSamplingFrequency()*1000;
	m_ulBufferSize = MAX_BUFFER_SIZE;
	if(!m_oFpSetSignalBuffer(m_HandleMaster,&m_ulSampleRate,&m_ulBufferSize)){
		m_rDriverContext.getLogManager() << LogLevel_Error << "For allocate the buffer\n";
		return false;
	}
	m_oHeader.setSamplingFrequency(m_ulSampleRate/1000);
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Maximum sample rate =" << (uint32)(m_ulSampleRate  / 1000 ) << "Hz\n";
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Maximum Buffer size =" << (uint32)(m_ulBufferSize) << "\n";

	BOOLEAN start=m_oFpStart(m_HandleMaster);
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Start handle state: " << (uint32)m_oFpGetDeviceState(m_HandleMaster) << "\n";
	if(!start)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Start handle failed\n";
		this->uninitialize();
		return false;
	}

	//get informations of the format signal for all channels of the Master Handle
	PSIGNAL_FORMAT l_pSignalFormat=m_oFpGetSignalFormat(m_HandleMaster,NULL) ;

	if(l_pSignalFormat!=NULL)
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << ">Master device name: " << (char*)l_pSignalFormat[0].PortName << "\n";
		m_rDriverContext.getLogManager() << LogLevel_Trace << ">Nb channels: " << (uint32)l_pSignalFormat[0].Elements << "\n\n";
		m_ui32NbTotalChannels=l_pSignalFormat[0].Elements;
		m_bSignalBufferUnsigned=l_pSignalFormat[0].Format==0;
		for(uint32 i = 0 ; i < l_pSignalFormat[0].Elements; i++ )
		{
			m_vExponentChannel.push_back(l_pSignalFormat[i].UnitExponent+6/*changed measure unit in µV*/);
			m_vUnitGain.push_back(l_pSignalFormat[i].UnitGain);
			m_vUnitOffSet.push_back(l_pSignalFormat[i].UnitOffSet);
			m_rDriverContext.getLogManager() << LogLevel_Debug << "channel[" << i << "]: Exponent=" << m_vExponentChannel[i] <<
					" unitGain=" << m_vUnitGain[i] <<
					" offSet=" << m_vUnitOffSet[i] <<
					" format data signed=" << ((uint32)l_pSignalFormat[i].Format) <<
					" type="<< (uint32)l_pSignalFormat[i].Type<<
					" sub type="<< (uint32)l_pSignalFormat[i].SubType<<
					" unit id="<< (uint32)l_pSignalFormat[i].UnitId<<"\n";
			//if no trigger channel was found and this channel is a digital input, this channel contains information about Trigger
			if(m_i32NumOfTriggerChannel==-1 && l_pSignalFormat[i].Type==4)
			{
				m_i32NumOfTriggerChannel=i;
			}
		}

		for(uint32 j = 0; j < m_vHandleSlaves.size() ; j++)
		{
			l_pSignalFormat = m_oFpGetSignalFormat(m_vHandleSlaves[j],NULL) ;

			if(l_pSignalFormat!=NULL)
			{
				m_rDriverContext.getLogManager() << LogLevel_Trace << ">Slave device n°" << (uint32)j <<
						" name: " << (char*)l_pSignalFormat[0].PortName << "\n";
				m_rDriverContext.getLogManager() << LogLevel_Trace << ">Nb channels: " << (uint32)l_pSignalFormat[0].Elements << "\n\n";
				m_ui32NbTotalChannels += l_pSignalFormat[0].Elements;
				for(uint32 i = 0 ; i < l_pSignalFormat[0].Elements ; i++)
				{
					m_vExponentChannel.push_back(l_pSignalFormat[i].UnitExponent);
					m_vUnitGain.push_back(l_pSignalFormat[i].UnitGain);
					m_vUnitOffSet.push_back(l_pSignalFormat[i].UnitOffSet);
				}
			}
		}
	}
	//m_oHeader.setChannelCount(m_ui32NbTotalChannels);
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Number of Channels: " << (uint32)m_oHeader.getChannelCount() << "\n";
	m_pSample=new float32[m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock*2] ;

	m_ui32SampleIndex=0;
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Sample driver size " << (uint32)(m_ui32NbTotalChannels*4) << "\n";
	m_ui32BufferSize = (m_ulBufferSize<(m_ui32SampleCountPerSentBlock*m_ui32NbTotalChannels*4))?m_ulBufferSize:(m_ui32SampleCountPerSentBlock*m_ui32NbTotalChannels*32);
	m_ulSignalBuffer=new ULONG[m_ui32BufferSize];
	if(!m_bSignalBufferUnsigned)
	{
		m_lSignalBuffer=new LONG[m_ui32BufferSize];
	}

	//activate the mode Impedance of the device
	if(m_bCheckImpedance)
	{
		if(!this->measureMode(MEASURE_MODE_IMPEDANCE, IC_OHM_005)){
			m_rDriverContext.getLogManager() << LogLevel_Error << "Error impedance measure mode ic_ohm_005\n";
			return false;
		}
	}
	return true;
}

boolean CDriverTMSiRefa32B::start(void)
{
	if(!m_bValid) { return false; }
	if(!m_rDriverContext.isConnected()){ return false;}
	if(m_rDriverContext.isStarted()){ return false;}

	m_rDriverContext.getLogManager() << LogLevel_Trace << ">start TMSI\n";
	if(m_bCheckImpedance)
	{
		this->measureMode(MEASURE_MODE_NORMAL, 0);
	}
	m_ui32SampleIndex=0;
	m_ui32TotalSampleReceived=0;
	return true;
}

boolean CDriverTMSiRefa32B::loop(void)
{
	if(!m_bValid) { return false; }
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted())
	{
		//get size of data receive
		ULONG l_lsize;

		//get data receive by the driver
		if(m_oFpGetSamples==NULL)
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "m_oFpGetSample not load\n";
			return false;
		}
		uint32 l_ui32NextSampleIndex=0;

		//uint64 l_ui64ElapsedTime=System::Time::zgetTime()-m_i64StartTime;
		l_lsize=m_oFpGetSamples(m_HandleMaster, (m_bSignalBufferUnsigned) ? (PULONG)m_ulSignalBuffer : (PULONG)m_lSignalBuffer, m_ui32BufferSize);

		if(l_lsize<1)
		{
			return true;
		}

		//number of samples contains in the data receive
		uint32 l_ui32NumSamples = l_lsize/(m_ui32NbTotalChannels*4);

		m_rDriverContext.getLogManager() << LogLevel_Debug << "size=" << (uint32)l_lsize << " ;;number of sample received=" << (uint32)l_ui32NumSamples << " ;; Samp["<<0<<"]=" <<
			(uint32)((m_bSignalBufferUnsigned)?m_ulSignalBuffer[0]:m_lSignalBuffer[0]) << ";; " << (uint32)((m_bSignalBufferUnsigned) ? m_ulSignalBuffer[1] : m_lSignalBuffer[1]) << "\n";

		//index of the data buffer
		uint32 l_ui32IndexBuffer=0;

		while(l_ui32IndexBuffer<l_ui32NumSamples)//-m_i64AutoRemovedSampleCount)
		{
			//take the minimum value between the size for complete the current block and the size of data received
			ULONG l_lmin;
			if(m_ui32SampleCountPerSentBlock-m_ui32SampleIndex<(l_ui32NumSamples-l_ui32IndexBuffer)){
				l_lmin=m_ui32SampleCountPerSentBlock-m_ui32SampleIndex;
			}
			else
			{
				l_lmin=l_ui32NumSamples-l_ui32IndexBuffer;
			}

			//loop on the channel
			for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
			{
				//loop on the samples by channel
				for(uint32 j=0; j<l_lmin; j++)
				{
					// save the data of one sample for one channel on the table
					if(m_ui32NbTotalChannels<=i)
					{
						m_pSample[m_ui32SampleIndex+j + i*m_ui32SampleCountPerSentBlock] =0;
					}
					else if(m_bSignalBufferUnsigned)
					{
						m_pSample[m_ui32SampleIndex+j + i*m_ui32SampleCountPerSentBlock] =(((float32)m_ulSignalBuffer[(l_ui32IndexBuffer+j)*m_ui32NbTotalChannels +i])*m_vUnitGain[i]+m_vUnitOffSet[i])*pow(10.,(double)m_vExponentChannel[i]);
					}
					else
					{
						m_pSample[m_ui32SampleIndex+j + i*m_ui32SampleCountPerSentBlock] =(((float32)m_lSignalBuffer[(l_ui32IndexBuffer+j)*m_ui32NbTotalChannels +i])*m_vUnitGain[i]+m_vUnitOffSet[i])*pow(10.,(double)m_vExponentChannel[i]);
					}

				}
			}

			for(uint32 j=0; j<l_lmin; j++)
			{
				uint32 l_ui32Trigger;
				if(m_bSignalBufferUnsigned)
				{
					l_ui32Trigger=m_ulSignalBuffer[(l_ui32IndexBuffer+j)*m_ui32NbTotalChannels +m_i32NumOfTriggerChannel];
				}
				else
				{
					l_ui32Trigger=m_lSignalBuffer[(l_ui32IndexBuffer+j)*m_ui32NbTotalChannels +m_i32NumOfTriggerChannel];
				}
				//std::cout<<l_ui32Trigger<<" ";
				l_ui32Trigger&=255;

				if(m_ui32LastTriggerValue!=l_ui32Trigger)
				{
					uint32 l_ui32IndexStimulation=m_oStimulationSet.getStimulationCount();
					m_oStimulationSet.appendStimulation(l_ui32Trigger, (uint64(m_ui32SampleIndex+j)<<32)/m_oHeader.getSamplingFrequency(), 0);
					m_ui32LastTriggerValue=l_ui32Trigger;
				}
			}
			//Calculate the number of index receive on the block
			m_ui32SampleIndex+=l_lmin;
			l_ui32IndexBuffer+=l_lmin;
			m_ui32TotalSampleReceived+=l_lmin;
			//see if the block is complete
			if(m_ui32SampleIndex>=m_ui32SampleCountPerSentBlock)
			{
				//sent the data block
				m_pCallback->setSamples(m_pSample);
				m_pCallback->setStimulationSet(m_oStimulationSet);
				m_oStimulationSet.clear();
				m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

				//calculate the index of the new block
				m_ui32SampleIndex-=m_ui32SampleCountPerSentBlock;
			}
		}
	}
	else if(m_bCheckImpedance)
	{

		//get Impedance value
		//get size of data receive
		ULONG l_lsize=0;
		l_lsize=m_oFpGetSamples(m_HandleMaster, (PULONG)m_ulSignalBuffer, m_ui32BufferSize);
		for(uint32 i=0;i<l_lsize/sizeof(ULONG)&&i<m_ui32NbTotalChannels;i++)
		{
			m_rDriverContext.updateImpedance(i, m_ulSignalBuffer[i]*1000);
		}
	}
	else
	{
		m_oFpGetSamples(m_HandleMaster, (PULONG)m_ulSignalBuffer, m_ui32BufferSize);
	}
	return true;
}

boolean CDriverTMSiRefa32B::stop(void)
{
	if(!m_rDriverContext.isConnected()){ return false;}
	if(!m_rDriverContext.isStarted()){ return false;}
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Stop TMSI\n";

	if(m_bCheckImpedance)
	{
		this->measureMode(MEASURE_MODE_IMPEDANCE, IC_OHM_005);
	}
	m_ui32SampleIndex=0;
	m_ui32TotalSampleReceived=0;
	return true;
}

boolean CDriverTMSiRefa32B::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()){ return false;}
	if(m_rDriverContext.isStarted()){ return false;}
	m_rDriverContext.getLogManager() << LogLevel_Trace << ">Uninit TMSI\n";

	//stop the driver
	BOOLEAN stop=TRUE;

	for(uint32 i=0;i<m_vHandleSlaves.size();i++)
	{
		stop=stop&&m_oFpStop(m_vHandleSlaves[i]);
	}
	stop=stop&&m_oFpStop(m_HandleMaster);
	if(!stop)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Stop handler\n";
		this->uninitialize();
		return false;
	}
	for(uint32 i=0;i<m_vHandleSlaves.size();i++)
	{
		m_oFpClose(m_vHandleSlaves[i]);
	}
	m_vHandleSlaves.clear();
	m_oFpClose(m_HandleMaster);
	m_HandleMaster = NULL;
	for(uint32 i=0;i<m_vHandleSlaves.size();i++)
	{
		m_oFpClose(m_vHandleSlaves[i]);
	}
	m_vDevicePathSlave.clear();
	m_pDevicePathMaster= "";
	m_vDevicePathMap.clear();
	delete[] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverTMSiRefa32B::isConfigurable(void)
{
	return true;
}

boolean CDriverTMSiRefa32B::configure(void)
{
	//refresh the information of the device connected
	this->refreshDevicePath();

	CConfigurationTMSIRefa32B l_oConfiguration("../share/openvibe-applications/acquisition-server/interface-TMSI-Refa32B.ui");
	//create a vector with all name of device connected
	std::vector<string> l_vDevicePath;
	for(map<PSP_DEVICE_PATH, std::string>::iterator  i=m_vDevicePathMap.begin();i!=m_vDevicePathMap.end();i++)
	{
		l_vDevicePath.push_back((*i).second);
	}

	//call configuration frame
	l_oConfiguration.setDeviceList(l_vDevicePath, &m_pDevicePathMaster, &m_vDevicePathSlave);
	bool result=l_oConfiguration.configure(m_oHeader);
	return result;
}

boolean CDriverTMSiRefa32B::refreshDevicePath(void)
{
	m_vDevicePathMap.clear();

	//get the number of devices connected
	ULONG l_MaxDevices = 0;
	if(m_oFpGetInstanceId== NULL)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Initialized the device, m_oFpGetNrDevice not load\n";
		return false;
	}
	m_oFpGetInstanceId(0, TRUE, &l_MaxDevices);

	for(uint32 i=0;i<l_MaxDevices;i++)
	{
		HKEY hKey;
		TCHAR deviceName[40] ="Unknown Device";
		ULONG serialNumber = 0;

		// get the device path connected
		PSP_DEVICE_PATH device=m_oFpGetInstanceId(i , TRUE, &l_MaxDevices);

		// get the name corresponding to this device
		hKey=m_oFpOpenRegKey(device);
		if( hKey != INVALID_HANDLE_VALUE )
		{
			ULONG sizeSerial;
			ULONG sizeDesc;

			//get the serial number of the device
			sizeSerial = sizeof( serialNumber );
			::RegQueryValueEx(hKey, "DeviceSerialNumber", NULL, NULL, (PBYTE)&serialNumber, &sizeSerial);

			//get the name of the device
			sizeDesc = sizeof( deviceName );
			::RegQueryValueEx(hKey , "DeviceDescription", NULL, NULL, (PBYTE)&deviceName[0], &sizeDesc);
			char l_sBuffer[1024];
			::sprintf(l_sBuffer, "%s %d", deviceName, serialNumber);

			//put the device path and it name in the map m_vDevicePathMap
			m_vDevicePathMap[device]=(char*)l_sBuffer;
			::RegCloseKey(hKey);
		}
	}

	//verify if the device Master is connected
	string l_sDevicePathMaster=m_pDevicePathMaster;
	m_pDevicePathMaster="";
	if(l_sDevicePathMaster.compare("")!=0)
	{
		map<PSP_DEVICE_PATH,string>::iterator index=m_vDevicePathMap.begin();
		while(m_pDevicePathMaster.compare("")==0&&index!=m_vDevicePathMap.end())
		{
			m_pDevicePathMaster=(l_sDevicePathMaster.compare((*index).second)==0) ? l_sDevicePathMaster : NULL;
			index++;
		}
	}

	//verify if all device slaves are connected
	std::vector<string> l_vDevicePathSlave;
	for(uint32 i=0;i<m_vDevicePathSlave.size();i++)
	{
		for(map<PSP_DEVICE_PATH,string>::iterator j=m_vDevicePathMap.begin() ; j!=m_vDevicePathMap.end() ; j++)
		{
			if((*j).second.compare(m_vDevicePathSlave[i])==0)
			{
				l_vDevicePathSlave.push_back(m_vDevicePathSlave[i]);
				break;
			}
		}
	}
	m_vDevicePathSlave.clear();
	m_vDevicePathSlave=l_vDevicePathSlave;
	m_lNrOfDevicesConnected=l_MaxDevices;
	return true;
}

boolean CDriverTMSiRefa32B::measureMode(uint32 mode, uint32 info )
{
	FEATURE_MODE fMode;
	fMode.Feature.FeatureId = DEVICE_FEATURE_MODE;
	fMode.Feature.Info = info;
	fMode.Mode = mode;
	if(!m_oFpDeviceFeature(m_HandleMaster, &fMode, sizeof( FEATURE_MODE  ), NULL, 0 ))return false;
	return true;
}

#endif // OVAS_OS_Windows
