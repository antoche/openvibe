#if defined TARGET_HAS_ThirdPartyGUSBampCAPI

#include "ovasCDriverGTecGUSBamp.h"
#include "ovasCConfigurationGTecGUSBamp.h"

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

CDriverGTecGUSBamp::CDriverGTecGUSBamp(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_ui32DeviceIndex(uint32(-1))
	,m_ui32BufferSize(0)
	,m_pBuffer(NULL)
	,m_pSample(NULL)
	,m_pDevice(NULL)
	,m_pEvent(NULL)
	,m_pOverlapped(NULL)
{
	m_oHeader.setSamplingFrequency(512);
	m_oHeader.setChannelCount(16);

	m_bImpedanceCheck=m_rDriverContext.getConfigurationManager().expandAsBoolean("${AcquisitionServer_CheckImpedance}", false);
	m_rDriverContext.getLogManager() << LogLevel_Trace << (m_bImpedanceCheck?"Impedance will be checked":"Impedance won't be checked") << "\n";
}

void CDriverGTecGUSBamp::release(void)
{
	delete this;
}

const char* CDriverGTecGUSBamp::getName(void)
{
	return "g.Tec gUSBamp";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverGTecGUSBamp::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) return false;

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		return false;
	}

	// If device has not been selected, try to find a device
	uint32 i=0;
	m_ui32ActualDeviceIndex=m_ui32DeviceIndex;
	while(i < 11 && m_ui32ActualDeviceIndex==uint32(-1))
	{
		::HANDLE l_pHandle=::GT_OpenDevice(i);
		if(l_pHandle)
		{
			::GT_CloseDevice(&l_pHandle);
			m_ui32ActualDeviceIndex=i;
		}
		i++;
	}

	if(m_ui32ActualDeviceIndex==uint32(-1))
	{
		return false;
	}

	m_pEvent=::CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_pEvent)
	{
		return false;
	}

	m_ui32BufferSize=(g_ui32AcquiredChannelCount+1)*ui32SampleCountPerSentBlock*sizeof(float)+HEADER_SIZE;
	m_pBuffer=new uint8[m_ui32BufferSize];
	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	if(!m_pBuffer || !m_pSample)
	{
		delete [] m_pBuffer;
		delete [] m_pSample;
		::CloseHandle(m_pEvent);
		return false;
	}
	::memset(m_pBuffer, 0, m_ui32BufferSize);
	m_pSampleTranspose=reinterpret_cast<float32*>(m_pBuffer+HEADER_SIZE);
	m_pOverlapped=new ::OVERLAPPED;
	if(!m_pOverlapped)
	{
		delete [] m_pBuffer;
		delete [] m_pSample;
		::CloseHandle(m_pEvent);
		return false;
	}
#define m_pOverlapped ((::OVERLAPPED*)m_pOverlapped)
	::memset(m_pOverlapped, 0, sizeof(::OVERLAPPED));
	m_pOverlapped->hEvent=m_pEvent;

	m_pDevice=::GT_OpenDevice(m_ui32ActualDeviceIndex);
	if(!m_pDevice)
	{
		delete m_pOverlapped;
		delete [] m_pBuffer;
		delete [] m_pSample;
		::CloseHandle(m_pEvent);
		return false;
	}

	m_ui32ActualImpedanceIndex=0;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_pCallback=&rCallback;

	return true;
}

boolean CDriverGTecGUSBamp::start(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

#if 1
	::UCHAR l_oChannel[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	::REF l_oReference;
	l_oReference.ref1=FALSE;
	l_oReference.ref2=FALSE;
	l_oReference.ref3=FALSE;
	l_oReference.ref4=FALSE;

	::GND l_oGround;
	l_oGround.GND1=FALSE;
	l_oGround.GND2=FALSE;
	l_oGround.GND3=FALSE;
	l_oGround.GND4=FALSE;

	if(!::GT_SetMode(m_pDevice, M_NORMAL)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetMode\n";
	if(!::GT_SetBufferSize(m_pDevice, m_ui32SampleCountPerSentBlock)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBufferSize\n";
	if(!::GT_SetChannels(m_pDevice, l_oChannel, sizeof(l_oChannel)/sizeof(::UCHAR))) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetChannels\n";
	if(!::GT_SetSlave(m_pDevice, FALSE)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetSlave\n";
	if(!::GT_EnableTriggerLine(m_pDevice, TRUE)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_EnableTriggerLine\n";
// GT_EnableSC
// GT_SetBipolar
/* */
	for(uint32 i=0; i<g_ui32AcquiredChannelCount; i++)
	{
		if(!::GT_SetBandPass(m_pDevice, i+1, -1)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBandPass for channel " << i << "\n";
		if(!::GT_SetNotch(m_pDevice, i+1, -1)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetNotch for channel " << i << "\n";
	}
/* */
	if(!::GT_SetSampleRate(m_pDevice, m_oHeader.getSamplingFrequency())) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetSampleRate\n";
#if 1 // most probably not necessary with g.GAMMAbox
	if(!::GT_SetReference(m_pDevice, l_oReference)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetReference\n";
	if(!::GT_SetGround(m_pDevice, l_oGround)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetGround\n";
#endif
#endif

	::GT_Start(m_pDevice);

	return true;
}

boolean CDriverGTecGUSBamp::loop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted())
	{
		if(::GT_GetData(m_pDevice, m_pBuffer, m_ui32BufferSize, m_pOverlapped))
		{
			if(::WaitForSingleObject(m_pOverlapped->hEvent, 2000)==WAIT_OBJECT_0)
			{
				DWORD l_dwByteCount=0;
				::GetOverlappedResult(m_pDevice, m_pOverlapped, &l_dwByteCount, FALSE);
				if(l_dwByteCount==m_ui32BufferSize)
				{
					for(uint32 i=0; i<m_oHeader.getChannelCount() && i<g_ui32AcquiredChannelCount; i++)
					{
						for(uint32 j=0; j<m_ui32SampleCountPerSentBlock; j++)
						{
							m_pSample[i*m_ui32SampleCountPerSentBlock+j]=m_pSampleTranspose[j*(g_ui32AcquiredChannelCount+1)+i];
						}
					}
					m_pCallback->setSamples(m_pSample);

					m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

					// TODO manage stims
				}
				else
				{
					// m_rDriverContext.getLogManager() << LogLevel_Error << "l_dwByteCount and m_ui32BufferSize differs : " << l_dwByteCount << "/" << m_ui32BufferSize << "(header size is " << HEADER_SIZE << ")\n";
				}
			}
			else
			{
				// TIMEOUT
				// m_rDriverContext.getLogManager() << LogLevel_Error << "timeout 1\n";
			}
		}
		else
		{
			// TIMEOUT
			// m_rDriverContext.getLogManager() << LogLevel_Error << "timeout 2\n";
		}
	}
	else
	{
		if(m_bImpedanceCheck)
		{
			double l_dImpedance=0;
			::GT_GetImpedance(m_pDevice, m_ui32ActualImpedanceIndex+1, &l_dImpedance);
			if(l_dImpedance<0) l_dImpedance*=-1;

			m_rDriverContext.updateImpedance(m_ui32ActualImpedanceIndex, l_dImpedance);

			m_rDriverContext.getLogManager() << LogLevel_Trace << "Channel " << m_ui32ActualImpedanceIndex << " - " << CString(m_oHeader.getChannelName(m_ui32ActualImpedanceIndex)) << " : " << l_dImpedance << "\n";

			m_ui32ActualImpedanceIndex++;
			m_ui32ActualImpedanceIndex%=m_oHeader.getChannelCount();

			m_rDriverContext.updateImpedance(m_ui32ActualImpedanceIndex, -1);
		}
		else
		{
			System::Time::sleep(20);
		}
	}

	return true;
}

boolean CDriverGTecGUSBamp::stop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return false;

	::GT_Stop(m_pDevice);
	::GT_ResetTransfer(m_pDevice);

	return true;
}

boolean CDriverGTecGUSBamp::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	::GT_CloseDevice(&m_pDevice);
	::CloseHandle(m_pEvent);
	delete [] m_pBuffer;
	delete [] m_pSample;
	delete m_pOverlapped;
	m_pDevice=NULL;
	m_pEvent=NULL;
	m_pBuffer=NULL;
	m_pSample=NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //
boolean CDriverGTecGUSBamp::isConfigurable(void)
{
	return true;
}

boolean CDriverGTecGUSBamp::configure(void)
{
	CConfigurationGTecGUSBamp m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-GTec-GUSBamp.ui", m_ui32DeviceIndex);
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}
	return true;
}

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI
