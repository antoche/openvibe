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

#define GTEC_NUM_CHANNELS 16

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

/*
	This driver always reads 17 channels: 16 + 1
	16 are EEG channels
	1 is the last channel that provides triggers from the parallel port of the GTEC
	Although 17 channels are read only "m_ui32AcquiredChannelCount" + 1 (if m_pTriggerInputEnabled==true) are displayed.
	If m_ui32AcquiredChannelCount=6 and m_pTriggerInputEnabled=true then the output in OpenVibe is 7 channels. If m_pTriggerInputEnabled=false then 6.
	"m_ui32AcquiredChannelCount" is a user modifiable variable with default value 16
*/

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
	,m_ui8CommonGndAndRefBitmap(0)
	,m_i32NotchFilterIndex(-1)
	,m_i32BandPassFilterIndex(-1)
	,m_bTriggerInputEnabled(false)
	,m_ui32AcquiredChannelCount(GTEC_NUM_CHANNELS)
{
	m_oHeader.setSamplingFrequency(512);
	m_oHeader.setChannelCount(GTEC_NUM_CHANNELS);
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

OpenViBE::boolean CDriverGTecGUSBamp::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) return false;

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		return false;
	}

	if (m_bTriggerInputEnabled)
	{
		m_oHeader.setChannelCount(m_ui32AcquiredChannelCount+1);
		m_oHeader.setChannelName(m_ui32AcquiredChannelCount, "CH_Event");
	}
	else
	{
		m_oHeader.setChannelCount(m_ui32AcquiredChannelCount);
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

	//allocate buffers
	m_ui32BufferSize=(GTEC_NUM_CHANNELS + 1)*ui32SampleCountPerSentBlock*sizeof(float)+HEADER_SIZE;//+1 channel for trigger
	m_pBuffer=new uint8[m_ui32BufferSize];

	if (m_bTriggerInputEnabled)
	{
		m_pSample=new float32[(m_ui32AcquiredChannelCount+1) * ui32SampleCountPerSentBlock];
	}
	else
	{
		m_pSample=new float32[m_ui32AcquiredChannelCount * ui32SampleCountPerSentBlock];
	}
	
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

OpenViBE::boolean CDriverGTecGUSBamp::start(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	::UCHAR l_oChannel[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	// The amplifier is divided in 4 blocks, A to D
	// each one has its own Ref/gnd connections,
	// user can specify whether or not to connect the block to the common ground and reference of the amplifier.
	::GND l_oGround;
	l_oGround.GND1=(m_ui8CommonGndAndRefBitmap&1);
	l_oGround.GND2=(m_ui8CommonGndAndRefBitmap&(1<<1));
	l_oGround.GND3=(m_ui8CommonGndAndRefBitmap&(1<<2));
	l_oGround.GND4=(m_ui8CommonGndAndRefBitmap&(1<<3));

	::REF l_oReference;
	l_oReference.ref1=(m_ui8CommonGndAndRefBitmap&(1<<4));
	l_oReference.ref2=(m_ui8CommonGndAndRefBitmap&(1<<5));
	l_oReference.ref3=(m_ui8CommonGndAndRefBitmap&(1<<6));
	l_oReference.ref4=(m_ui8CommonGndAndRefBitmap&(1<<7));

	if(!::GT_SetMode(m_pDevice, M_NORMAL)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetMode\n";
	if(!::GT_SetBufferSize(m_pDevice, m_ui32SampleCountPerSentBlock)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBufferSize\n";
	if(!::GT_SetChannels(m_pDevice, l_oChannel, sizeof(l_oChannel)/sizeof(::UCHAR))) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetChannels\n";
	if(!::GT_SetSlave(m_pDevice, FALSE)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetSlave\n";
	
	if(!::GT_EnableTriggerLine(m_pDevice, TRUE)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_EnableTriggerLine - the extra input trigger channel is disabled\n";
	// GT_EnableSC
    // GT_SetBipolar

	for(uint32 i=0; i<m_ui32AcquiredChannelCount; i++)
	{
		if(!::GT_SetBandPass(m_pDevice, i+1, m_i32BandPassFilterIndex)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetBandPass for channel " << i << "\n";
		if(!::GT_SetNotch(m_pDevice, i+1, m_i32NotchFilterIndex)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetNotch for channel " << i << "\n";
	}

	if(!::GT_SetSampleRate(m_pDevice, m_oHeader.getSamplingFrequency())) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetSampleRate\n";

	if(!::GT_SetReference(m_pDevice, l_oReference)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetReference\n";
	if(!::GT_SetGround(m_pDevice, l_oGround)) m_rDriverContext.getLogManager() << LogLevel_Error << "Unexpected error while calling GT_SetGround\n";

	m_ui32LastStimulation = STIMULATION_0;
	m_ui32TotalHardwareStimulations = 0;
	m_ui32TotalDriverChunksLost = 0;

	::GT_Start(m_pDevice);

	return true;
}

OpenViBE::boolean CDriverGTecGUSBamp::loop(void)
{
	CStimulationSet   l_oStimulationSet;

	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted())
	{

		if (::GT_GetData(m_pDevice, m_pBuffer, m_ui32BufferSize, m_pOverlapped))
			{
				if(::WaitForSingleObject(m_pOverlapped->hEvent, 1000)==WAIT_OBJECT_0)
				{
					DWORD l_dwByteCount=0;
				
					::GetOverlappedResult(m_pDevice, m_pOverlapped, &l_dwByteCount, FALSE);

					if(l_dwByteCount==m_ui32BufferSize)
					{

							for(uint32 i=0; i<m_ui32AcquiredChannelCount; i++) 
							{
								for(uint32 j=0; j<m_ui32SampleCountPerSentBlock; j++)
								{
									m_pSample[i*m_ui32SampleCountPerSentBlock+j]=m_pSampleTranspose[j*(GTEC_NUM_CHANNELS + 1)+i];
								}
							}

							if (m_bTriggerInputEnabled)
							{
								int l_oStimulationChannel = m_oHeader.getChannelCount()-1;
        
								for(uint32 iSample=0; iSample<m_ui32SampleCountPerSentBlock; iSample++)
								{
									OpenViBE::uint32 l_ui32StimCode = m_pSampleTranspose[iSample*(GTEC_NUM_CHANNELS + 1) + GTEC_NUM_CHANNELS];
									m_pSample[m_ui32AcquiredChannelCount*m_ui32SampleCountPerSentBlock+iSample]=l_ui32StimCode;

									if ( (l_ui32StimCode != STIMULATION_0) //this means that the user sends 0 after each stimulatuion and in the beginning
											&& (l_ui32StimCode != m_ui32LastStimulation) 
										)
									{
										OpenViBE::uint64 identifier;
										switch (l_ui32StimCode)
										{
											case STIMULATION_64  : identifier = OVTK_StimulationId_Label_01; break;
											case STIMULATION_128 : identifier = OVTK_StimulationId_Label_02; break;
											case STIMULATION_192 : identifier = OVTK_StimulationId_Label_03; break;
											default: identifier = OVTK_StimulationId_Label_07;
										}

										l_oStimulationSet.appendStimulation(identifier,( uint64(iSample) << 32) / m_oHeader.getSamplingFrequency(),0);
										m_ui32TotalHardwareStimulations++;
									}

									m_ui32LastStimulation = l_ui32StimCode;
								}     
							}

							m_pCallback->setSamples(m_pSample);	
							m_pCallback->setStimulationSet(l_oStimulationSet);
							m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());						
					}
					else
					{
						m_ui32TotalDriverChunksLost++;
						//m_rDriverContext.getLogManager() << LogLevel_Error << "l_dwByteCount and m_ui32BufferSize differs : " << l_dwByteCount << "/" << m_ui32BufferSize << "(header size is " << HEADER_SIZE << ")\n";
						/*m_rDriverContext.getLogManager() << LogLevel_Warning 
						<< "Returned data is different than expected. Total chunks lost: " << m_totalDriverChunksLost 
						<< ", Total samples lost: " << m_ui32SampleCountPerSentBlock * m_totalDriverChunksLost
						<< "\n";*/
					}
				}
				else
				{
					// TIMEOUT
					//m_rDriverContext.getLogManager() << LogLevel_Error << "timeout 1\n";
				}
			}
			else
			{
				//m_rDriverContext.getLogManager() << LogLevel_Error << "tError on GT_GetData.\n";
			}
	}
	else
	{
		if(m_rDriverContext.isImpedanceCheckRequested())
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

OpenViBE::boolean CDriverGTecGUSBamp::stop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return false;

	//stop device
	::GT_Stop(m_pDevice);
	::GT_ResetTransfer(m_pDevice);

	m_rDriverContext.getLogManager() << LogLevel_Debug << "Total number of hardware stimulations acquired: " << m_ui32TotalHardwareStimulations << "\n";
	m_rDriverContext.getLogManager() << LogLevel_Trace << "Total chunks lost: " << m_ui32TotalDriverChunksLost << "\n";
	
	return true;
}

OpenViBE::boolean CDriverGTecGUSBamp::uninitialize(void)
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
OpenViBE::boolean CDriverGTecGUSBamp::isConfigurable(void)
{
	return true;
}

OpenViBE::boolean CDriverGTecGUSBamp::configure(void)
{
	CConfigurationGTecGUSBamp m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-GTec-GUSBamp.ui", m_ui32DeviceIndex, m_ui8CommonGndAndRefBitmap, m_i32NotchFilterIndex,m_i32BandPassFilterIndex,m_bTriggerInputEnabled);

	m_oHeader.setChannelCount(m_ui32AcquiredChannelCount);
	
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}

	this->m_ui32AcquiredChannelCount = m_oHeader.getChannelCount();

	return true;
}

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI
