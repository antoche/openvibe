#if defined TARGET_HAS_ThirdPartyGUSBampCAPI

#include "ovasCDriverGTecGUSBampMultiamp.h"
#include "ovasCConfigurationGTecGUSBampMultiamp.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>

#include <cmath>
#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <gUSBamp.h>
#define boolean OpenViBE::boolean

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

static const uint32 g_ui32AcquiredChannelCount=16;
//___________________________________________________________________//
//                                                                   //

CDriverGTecGUSBampMultiamp::CDriverGTecGUSBampMultiamp(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_ui32BufferSize(0)
	,m_pSample(NULL)
	,m_pSampleTranspose(NULL)
	,m_ui8CommonGndAndRefBitmap(0)
	,m_i32NotchFilterIndex(-1)
	,m_i32BandPassFilterIndex(-1)
{
	m_oHeader.setSamplingFrequency(512);
	m_oHeader.setChannelCount(16);
	
}

void CDriverGTecGUSBampMultiamp::release(void)
{
	delete this;
}

const char* CDriverGTecGUSBampMultiamp::getName(void)
{
	return "g.Tec gUSBamp - Multi amplifier driver";
}

void CDriverGTecGUSBampMultiamp::closeAllDevices(void)
{
	for(uint32 i = 0; i<m_vDevices.size(); i++)
	{
		if(m_vDevices[i].m_pHandler != NULL) GT_CloseDevice(&(m_vDevices[i].m_pHandler));
		if(m_vDevices[i].m_pEvent != NULL) CloseHandle(m_vDevices[i].m_pEvent);
		delete[] m_vDevices[i].m_pBuffer;
		delete[] m_vDevices[i].m_pOverlapped;
	}
}
//___________________________________________________________________//
//                                                                   //

boolean CDriverGTecGUSBampMultiamp::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) return false;

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		return false;
	}

	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock*m_vDevices.size()];
	
	//For every device:
	//m_ui32BufferSize=(g_ui32AcquiredChannelCount+1)*ui32SampleCountPerSentBlock*sizeof(float)+HEADER_SIZE;
	m_ui32BufferSize=(g_ui32AcquiredChannelCount)*ui32SampleCountPerSentBlock*sizeof(float)+HEADER_SIZE;
	
	// We manage devices one by one. They should be only with a valid Serial, we need to construct everything else
	for(uint32 i = 0; i<m_vDevices.size(); i++)
	{
		// the windows event handler
		void * l_pEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(!l_pEvent)
		{
			m_rDriverContext.getLogManager() <<LogLevel_Error << "Could not create WindowsEvent for "<<(m_vDevices[i].m_bSlave ? "Slave" : "Master") <<" device #"<<i<<" with Serial {" <<m_vDevices[i].m_sSerial<<"}\n";
			closeAllDevices();
			delete[] m_pSample;
			return false;
		}
		m_vDevices[i].m_pEvent = l_pEvent;

		// buffers
		m_vDevices[i].m_pBuffer=new uint8[m_ui32BufferSize];
		if(m_vDevices[i].m_pBuffer == NULL)
		{
			m_rDriverContext.getLogManager() <<LogLevel_Error << "Could not allocate buffer for "<<(m_vDevices[i].m_bSlave ? "Slave" : "Master") <<" Device #"<<i<<" with Serial {" <<m_vDevices[i].m_sSerial<<"}\n";
			closeAllDevices();
			delete[] m_pSample;
			return false;
		}
		::memset(m_vDevices[i].m_pBuffer, 0, m_ui32BufferSize);
		// The SampleTranspose buffer is only the sample part of the packet (without HEADER). It's "transposed" w.r.t. the openvibe buffer structure.
		m_vDevices[i].m_pSampleTranspose=reinterpret_cast<float32*>(m_vDevices[i].m_pBuffer+HEADER_SIZE);
		
		// The overlapped windows structure for acquisition
		m_vDevices[i].m_pOverlapped=new OVERLAPPED;
		if(m_vDevices[i].m_pOverlapped == NULL)
		{
			m_rDriverContext.getLogManager() <<LogLevel_Error << "Could not allocate OVERLAPPED structure for "<<(m_vDevices[i].m_bSlave ? "Slave" : "Master") <<" Device #"<<i<<" with Serial {" <<m_vDevices[i].m_sSerial<<"}\n";
			closeAllDevices();
			delete[] m_pSample;
			return false;
		}
		::memset(((OVERLAPPED*)m_vDevices[i].m_pOverlapped), 0, sizeof(OVERLAPPED));
		((OVERLAPPED*)m_vDevices[i].m_pOverlapped)->hEvent=m_vDevices[i].m_pEvent;

		// the device handler
		// WARNING : we MUST open the slaves before the master device, that's why the last device in the vector must be the master (it should be after configuration).
		m_rDriverContext.getLogManager() <<LogLevel_Trace << "Opening "<<(m_vDevices[i].m_bSlave ? "Slave" : "Master") <<" Device #"<<i<<" with Serial {" <<m_vDevices[i].m_sSerial<<"}\n";
		void* l_pDevice = GT_OpenDeviceEx(const_cast<LPSTR>((const char *) m_vDevices[i].m_sSerial));
		if(l_pDevice != NULL)
		{
			m_vDevices[i].m_pHandler = l_pDevice;
		}
		else
		{
			m_rDriverContext.getLogManager() <<LogLevel_Error << "Could not open "<<(m_vDevices[i].m_bSlave ? "Slave" : "Master") <<" Device #"<<i<<" with Serial {" <<m_vDevices[i].m_sSerial<<"}\n";
			closeAllDevices();
			delete[] m_pSample;
			return false;
		}
	}
	
//#define m_pOverlapped ((::OVERLAPPED*)m_pOverlapped)

	m_ui32ActualImpedanceIndex=0;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_pCallback=&rCallback;

	return true;
}

boolean CDriverGTecGUSBampMultiamp::start(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	// The UCHAR table of channels
	UCHAR l_oChannel[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	// The amplifier is divided in 4 blocks, A to D
	// each one has its own Ref/gnd connections,
	// user can specify whether or not to connect the block to the common ground and reference of the amplifier.
	// SAME CONFIGURATION FOR EVERY DEVICE
	GND l_oGround;
	l_oGround.GND1=(m_ui8CommonGndAndRefBitmap&1);
	l_oGround.GND2=(m_ui8CommonGndAndRefBitmap&(1<<1));
	l_oGround.GND3=(m_ui8CommonGndAndRefBitmap&(1<<2));
	l_oGround.GND4=(m_ui8CommonGndAndRefBitmap&(1<<3));

	REF l_oReference;
	l_oReference.ref1=(m_ui8CommonGndAndRefBitmap&(1<<4));
	l_oReference.ref2=(m_ui8CommonGndAndRefBitmap&(1<<5));
	l_oReference.ref3=(m_ui8CommonGndAndRefBitmap&(1<<6));
	l_oReference.ref4=(m_ui8CommonGndAndRefBitmap&(1<<7));

	// From syncdemo : UNIPOLAR derivation for every device
	CHANNEL l_oBipolarSettings;
	l_oBipolarSettings.Channel1 = 0;
	l_oBipolarSettings.Channel2 = 0;
	l_oBipolarSettings.Channel3 = 0;
	l_oBipolarSettings.Channel4 = 0;
	l_oBipolarSettings.Channel5 = 0;
	l_oBipolarSettings.Channel6 = 0;
	l_oBipolarSettings.Channel7 = 0;
	l_oBipolarSettings.Channel8 = 0;
	l_oBipolarSettings.Channel9 = 0;
	l_oBipolarSettings.Channel10 = 0;
	l_oBipolarSettings.Channel11 = 0;
	l_oBipolarSettings.Channel12 = 0;
	l_oBipolarSettings.Channel13 = 0;
	l_oBipolarSettings.Channel14 = 0;
	l_oBipolarSettings.Channel15 = 0;
	l_oBipolarSettings.Channel16 = 0;

	// We iterate over the devices. Master is last.
	for(uint32 i = 0; i < m_vDevices.size(); i++)
	{
		// should be 16 channels acquired anyway
		if(!GT_SetChannels(m_vDevices[i].m_pHandler, l_oChannel, sizeof(l_oChannel)/sizeof(UCHAR)))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetChannels\n";
		
		// The samplig rate is (must be) the same for every device
		if(!GT_SetSampleRate(m_vDevices[i].m_pHandler, m_oHeader.getSamplingFrequency()))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetSampleRate\n";

		//From syncDemo: disable trigger line
		if(!GT_EnableTriggerLine(m_vDevices[i].m_pHandler, false))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_EnableTriggerLine\n";// false
		//if(!GT_EnableTriggerLine(m_vDevices[i].m_pHandler, true)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_EnableTriggerLine\n";// false

		//from syncdemo : 512 HZ -> recommends 16 scans. But it seems like we don't follow the rules anyway... Yep we're crazy.
		//if(!GT_SetBufferSize(m_vDevices[i].m_pHandler, 16)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBufferSize\n";//NUMBER_OF_SCAN for 512 Hz acquisition.
		if(!GT_SetBufferSize(m_vDevices[i].m_pHandler, m_ui32SampleCountPerSentBlock))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBufferSize\n";
		
		for(uint32 c=0; c<g_ui32AcquiredChannelCount; c++)
		{
			if(!GT_SetBandPass(m_vDevices[i].m_pHandler, c+1, m_i32BandPassFilterIndex))
				m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBandPass for channel " << i << "\n";
			if(!GT_SetNotch(m_vDevices[i].m_pHandler, c+1, m_i32NotchFilterIndex))
				m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetNotch for channel " << i << "\n";
		}
		
		if(!GT_SetSlave(m_vDevices[i].m_pHandler, m_vDevices[i].m_bSlave))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetSlave\n";
		
		//disable shortcut function
		if(!GT_EnableSC(m_vDevices[i].m_pHandler, false))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_EnableSC\n";

		// GT_SetBipolar --> sync demo : unipolar derivation for every device
		if(!GT_SetBipolar(m_vDevices[i].m_pHandler, l_oBipolarSettings))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBipolar\n";

		// NORMAL MODE
		if(!GT_SetMode(m_vDevices[i].m_pHandler, M_NORMAL))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetMode\n";
		
		// Common REF and GND : user defined, same for every device
		if(!GT_SetReference(m_vDevices[i].m_pHandler, l_oReference))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetReference\n";
		if(!GT_SetGround(m_vDevices[i].m_pHandler, l_oGround))
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetGround\n";

		// Now we start the device (master should be last).
		if(!GT_Start(m_vDevices[i].m_pHandler))
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Could not start Device #"<<i<<" with Serial {" <<m_vDevices[i].m_sSerial<<"}\n";
			return false;
		}
	}
	return true;
}

boolean CDriverGTecGUSBampMultiamp::loop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted())
	{
		// The devices are internally stored as {SLAVE_3; SLAVE_2; SLAVE_1; MASTER}
		// The order for the openvibe streams is opposite, ie {MASTER 1-16 ; SLAVE_1 17-32 ; SLAVE_2 33-48 ; SLAVE_3 49-64}
		int32 l_ui32DeviceIndex = m_vDevices.size() -1;
		uint32 l_ui32SampleBlockIndex = 0;
		while(l_ui32DeviceIndex >= 0)
		{
			if(GT_GetData(m_vDevices[l_ui32DeviceIndex].m_pHandler, m_vDevices[l_ui32DeviceIndex].m_pBuffer, m_ui32BufferSize, (OVERLAPPED*)m_vDevices[l_ui32DeviceIndex].m_pOverlapped))
			{
				if(WaitForSingleObject(((OVERLAPPED*)m_vDevices[l_ui32DeviceIndex].m_pOverlapped)->hEvent, 2000)==WAIT_OBJECT_0)
				{
					DWORD l_dwByteCount=0;
					GetOverlappedResult(m_vDevices[l_ui32DeviceIndex].m_pHandler, (OVERLAPPED*)m_vDevices[l_ui32DeviceIndex].m_pOverlapped, &l_dwByteCount, FALSE);
					if(l_dwByteCount==m_ui32BufferSize)
					{
						for(uint32 i=0; i<g_ui32AcquiredChannelCount; i++)
						{
							for(uint32 j=0; j<m_ui32SampleCountPerSentBlock; j++)
							{
								m_pSample[(l_ui32SampleBlockIndex*g_ui32AcquiredChannelCount+i)*m_ui32SampleCountPerSentBlock+j]=m_vDevices[l_ui32DeviceIndex].m_pSampleTranspose[j*(g_ui32AcquiredChannelCount)+i];
							}
						}
						//We send the samples if we got all samples blocks, i.e this is the last device
						if(l_ui32DeviceIndex == 0)
						{
							m_pCallback->setSamples(m_pSample);
						}
						m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

						// TODO manage stims
					}
					else
					{
						m_rDriverContext.getLogManager() << LogLevel_Trace << "Received Byte count and buffer size differs : " << (uint32)l_dwByteCount << "/" << m_ui32BufferSize << "(header size is " << HEADER_SIZE << ")\n";
					}
				}
				else
				{
					// TIMEOUT
					m_rDriverContext.getLogManager() << LogLevel_Error << "timeout on WaitForSingleObject\n";
				}
			}
			else
			{
				// TIMEOUT
				m_rDriverContext.getLogManager() << LogLevel_Error << "GT_GetData not successful\n";
			}
			l_ui32DeviceIndex--;
			l_ui32SampleBlockIndex++;
		}
	}
	else
	{
		{
			System::Time::sleep(20);
		}
	}

	return true;
}

boolean CDriverGTecGUSBampMultiamp::stop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return false;
	for(uint32 i = 0; i < m_vDevices.size(); i++)
	{
		GT_Stop(m_vDevices[i].m_pHandler);
		GT_ResetTransfer(m_vDevices[i].m_pHandler);
	}

	return true;
}

boolean CDriverGTecGUSBampMultiamp::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	closeAllDevices();
	delete [] m_pSample;
	
	return true;
}

//___________________________________________________________________//
//                                                                   //
boolean CDriverGTecGUSBampMultiamp::isConfigurable(void)
{
	return true;
}

boolean CDriverGTecGUSBampMultiamp::configure(void)
{
	m_ui8CommonGndAndRefBitmap = 0;
	m_i32NotchFilterIndex = -1;
	m_i32BandPassFilterIndex = -1;
	vector<CString> l_vSerials;
	for(uint32 i = 0; i < m_vDevices.size(); i++)
	{
		l_vSerials.push_back(m_vDevices[i].m_sSerial);
	}
	CConfigurationGTecGUSBampMultiamp m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-GTec-GUSBamp-multiamp.ui", l_vSerials);
	
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}

	m_vDevices.clear();
	for(uint32 i = 0; i < l_vSerials.size(); i++)
	{
		SDevice l_oDevice;
		l_oDevice.m_pEvent = NULL;
		l_oDevice.m_pHandler = NULL;
		l_oDevice.m_pBuffer = NULL;
		l_oDevice.m_pSampleTranspose = NULL;
		l_oDevice.m_pOverlapped = NULL;
		l_oDevice.m_sSerial = l_vSerials[i];
		l_oDevice.m_bSlave = (i != l_vSerials.size()-1);
		m_vDevices.push_back(l_oDevice);
	}

	for(uint32 i = 0; i < m_vDevices.size(); i++)
	{
		m_rDriverContext.getLogManager() <<LogLevel_Info << "Adding "<<(m_vDevices[i].m_bSlave ? "Slave" : "Master") <<" Device #"<<i<<" with Serial {" <<m_vDevices[i].m_sSerial<<"}\n";
	}

	m_oHeader.setChannelCount(m_oHeader.getChannelCount() * l_vSerials.size());

	return true;
}

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI
