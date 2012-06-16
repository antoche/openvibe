#if defined TARGET_HAS_ThirdPartyUSBFirstAmpAPI

#include "ovasCDriverBrainProductsVAmp.h"
#include "ovasCConfigurationBrainProductsVAmp.h"
#include "ovasCHeaderBrainProductsVAmp.h"

#include <system/Time.h>
#include <windows.h>

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <vector>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

#define boolean OpenViBE::boolean

//___________________________________________________________________//
//                                                                   //

CDriverBrainProductsVAmp::CDriverBrainProductsVAmp(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_bAcquireAuxiliaryAsEEG(rDriverContext.getConfigurationManager().expandAsBoolean("${AcquisitionServer_Driver_VAmpAcquireAuxiliaryAsEEG}", false))
	,m_bAcquireTriggerAsEEG(rDriverContext.getConfigurationManager().expandAsBoolean("${AcquisitionServer_Driver_VAmpAcquireTriggerAsEEG}", false))
	,m_oHeader(
		m_bAcquireAuxiliaryAsEEG,
		m_bAcquireTriggerAsEEG)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_ui32TotalSampleCount(0)
	,m_pSample(NULL)
	,m_bFirstStart(false)
{
	m_oHeader.setAcquisitionMode(AcquisitionMode_VAmp16);
	m_oHeader.setChannelCount(
		m_oHeader.getEEGChannelCount(AcquisitionMode_VAmp16)+
		m_oHeader.getAuxiliaryChannelCount(AcquisitionMode_VAmp16)+
		m_oHeader.getTriggerChannelCount(AcquisitionMode_VAmp16));
	m_oHeader.setSamplingFrequency(2000);

	t_faDataModeSettings l_tVamp4FastSettings;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsPos[0] = 7;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsNeg[0] = -1;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsPos[1] = 8;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsNeg[1] = -1;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsPos[2] = 9;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsNeg[2] = -1;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsPos[3] = 10;
	l_tVamp4FastSettings.Mode20kHz4Channels.ChannelsNeg[3] = -1;

	m_oHeader.setFastModeSettings(l_tVamp4FastSettings);
	m_oHeader.setDeviceId(FA_ID_INVALID);
}

CDriverBrainProductsVAmp::~CDriverBrainProductsVAmp(void)
{
}

const char* CDriverBrainProductsVAmp::getName(void)
{
	return "Brain Products V-Amp / First-Amp";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverBrainProductsVAmp::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "INIT called.\n";
	if(m_rDriverContext.isConnected())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] VAmp Driver: Driver already initialized.\n";
		return false;
	}

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] VAmp Driver: Channel count or frequency not set.\n";
		return false;
	}

	// Builds up a buffer to store acquired samples. This buffer will be sent to the acquisition server later.
	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	if(!m_pSample)
	{
		delete [] m_pSample;
		m_pSample=NULL;
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] VAmp Driver: Samples allocation failed.\n";
		return false;
	}

	int32 l_i32DeviceId = m_oHeader.getDeviceId();

	//__________________________________
	// Hardware initialization

	if(m_bAcquireAuxiliaryAsEEG) m_rDriverContext.getLogManager() << LogLevel_Trace << "[INIT] VAmp Driver: will acquire aux as EEG\n";
	else                         m_rDriverContext.getLogManager() << LogLevel_Trace << "[INIT] VAmp Driver: will NOT acquire aux as EEG\n";
	if(m_bAcquireTriggerAsEEG) m_rDriverContext.getLogManager() << LogLevel_Trace << "[INIT] VAmp Driver: will acquire trigger as EEG\n";
	else                       m_rDriverContext.getLogManager() << LogLevel_Trace << "[INIT] VAmp Driver: will NOT acquire trigger as EEG\n";

	m_ui32AcquisitionMode=m_oHeader.getAcquisitionMode();
	m_ui32EEGChannelCount=m_oHeader.getEEGChannelCount(m_ui32AcquisitionMode);
	m_ui32AuxiliaryChannelCount=m_oHeader.getAuxiliaryChannelCount(m_ui32AcquisitionMode);
	m_ui32TriggerChannelCount=m_oHeader.getTriggerChannelCount(m_ui32AcquisitionMode);

	// if no device selected with the properties dialog
	// we take the last device connected
	if(l_i32DeviceId == FA_ID_INVALID)
	{
		// We try to get the last opened device,
		uint32 l_uint32LastOpenedDeviceID = faGetCount(); // Get the last opened Device id.

		if (l_uint32LastOpenedDeviceID == FA_ID_INVALID) // failed
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] VAmp Driver: faGetCount failed to get last opened device.\n";
			return false;
		}

		l_i32DeviceId = faGetId(l_uint32LastOpenedDeviceID -1);
		m_oHeader.setDeviceId(l_i32DeviceId);
	}

	if (l_i32DeviceId != FA_ID_INVALID)
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "[INIT] VAmp Driver: Active device ID(" << m_oHeader.getDeviceId() << ").\n";
	}
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] VAmp Driver: No device connected !\n";
		return false;
	}

	// Open the device.
	uint32 l_uint32OpenReturn = faOpen(l_i32DeviceId);
	if (l_uint32OpenReturn != FA_ERR_OK)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] VAmp Driver: faOpen(" << l_i32DeviceId << ") FAILED(" << l_uint32OpenReturn << ").\n";
		return false;
	}

	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		faSetDataMode(l_i32DeviceId, dm20kHz4Channels, &(m_oHeader.getFastModeSettings()));
	}
	else
	{
		faSetDataMode(l_i32DeviceId, dmNormal, NULL);
	}

	uint32 l_uint32ErrorCode = faStart(l_i32DeviceId);

	if (l_uint32ErrorCode != FA_ERR_OK)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[START] VAmp Driver: faStart FAILED(" << l_uint32ErrorCode << "). Closing device.\n";
		faClose(l_i32DeviceId);
		return false;
	}

	//__________________________________
	// Saves parameters
	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;

	return true;
}

boolean CDriverBrainProductsVAmp::start(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "START called.\n";
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	if(m_rDriverContext.isStarted())
	{
		return false;
	}

	m_bFirstStart = true;

	//The bonus...
	HBITMAP l_bitmap = (HBITMAP) LoadImage(NULL, "../share/openvibe-applications/acquisition-server/vamp.bmp",IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(l_bitmap == NULL || faSetBitmap(m_oHeader.getDeviceId(),l_bitmap ) != FA_ERR_OK)
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "[START] VAmp Driver: BMP load failed.\n";
	}

	return true;

}

boolean CDriverBrainProductsVAmp::loop(void)
{
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	t_faDataModel16 l_DataBufferVAmp16; // buffer for the next block in normal mode
	uint32 l_uint32ReadLengthVAmp16 = sizeof(t_faDataModel16);

	t_faDataModel8 l_DataBufferVAmp8; // buffer for the next block in normal mode
	uint32 l_uint32ReadLengthVAmp8 = sizeof(t_faDataModel8);

	t_faDataFormatMode20kHz l_DataBufferVamp4Fast; // buffer for fast mode acquisition
	uint32 l_uint32ReadLengthVamp4Fast = sizeof(t_faDataFormatMode20kHz);

	int32 l_i32DeviceId = m_oHeader.getDeviceId();

	if(m_rDriverContext.isStarted())
	{
		uint32 l_i32ReceivedSamples=0;
#if DEBUG
		uint32 l_uint32ReadErrorCount = 0;
		uint32 l_uint32ReadSuccessCount = 0;
		uint32 l_uint32ReadZeroCount = 0;
#endif

		while(l_i32ReceivedSamples < m_ui32SampleCountPerSentBlock)
		{
			// we need to "getData" with the right output structure according to acquisition mode

			int32 l_i32ReturnLength = 0;
			signed int* l_pEEGArray=NULL;
			signed int* l_pAuxiliaryArray=NULL;
			unsigned int l_uiStatus=0;
			switch(m_ui32AcquisitionMode)
			{
				case AcquisitionMode_VAmp16:
					l_i32ReturnLength = faGetData(l_i32DeviceId, &l_DataBufferVAmp16, l_uint32ReadLengthVAmp16);
					l_pEEGArray=l_DataBufferVAmp16.Main;
					l_pAuxiliaryArray=l_DataBufferVAmp16.Aux;
					l_uiStatus=l_DataBufferVAmp16.Status;
					break;

				case AcquisitionMode_VAmp8:
					l_i32ReturnLength = faGetData(l_i32DeviceId, &l_DataBufferVAmp8, l_uint32ReadLengthVAmp8);
					l_pEEGArray=l_DataBufferVAmp8.Main;
					l_pAuxiliaryArray=l_DataBufferVAmp8.Aux;
					l_uiStatus=l_DataBufferVAmp8.Status;
					break;

				case AcquisitionMode_VAmp4Fast:
					l_i32ReturnLength = faGetData(l_i32DeviceId, &l_DataBufferVamp4Fast, l_uint32ReadLengthVamp4Fast);
					l_pEEGArray=l_DataBufferVamp4Fast.Main;
					// l_pAuxiliaryArray=l_DataBufferVamp4Fast.Aux;
					l_uiStatus=l_DataBufferVamp4Fast.Status;
					break;
			}

			if(l_i32ReturnLength > 0)
			{
				uint32 i;
#if DEBUG
				l_uint32ReadSuccessCount++;
#endif
				for(i=0; i < m_ui32EEGChannelCount; i++)
				{
					m_pSample[i*m_ui32SampleCountPerSentBlock+l_i32ReceivedSamples] = (float32)(l_pEEGArray[i]*m_oHeader.getChannelGain(i));
				}
				for(i=0; i < m_ui32AuxiliaryChannelCount; i++)
				{
					m_pSample[(m_ui32EEGChannelCount+i)*m_ui32SampleCountPerSentBlock+l_i32ReceivedSamples] = (float32)(l_pAuxiliaryArray[i]);
				}
				for(i=0; i < m_ui32TriggerChannelCount; i++)
				{
					m_pSample[(m_ui32EEGChannelCount+m_ui32AuxiliaryChannelCount+i)*m_ui32SampleCountPerSentBlock+l_i32ReceivedSamples] = (float32)(l_uiStatus);
				}

				l_i32ReceivedSamples++;
			}
			else if(l_i32ReturnLength==0)
			{
				System::Time::sleep(2);
			}
#if DEBUG
			if(l_i32ReturnLength < 0)
			{
				l_uint32ReadErrorCount++;
			}
			if(l_i32ReturnLength == 0)
			{
				l_uint32ReadZeroCount++;
			}
#endif
		}// while received < m_ui32SampleCountPerSentBlock
#if DEBUG
        m_rDriverContext.getLogManager() << LogLevel_Debug << "[LOOP] VAmp Driver: stats for the current block : Success="<<l_uint32ReadSuccessCount<<" Error="<<l_uint32ReadErrorCount<<" Zero="<<l_uint32ReadZeroCount<<"\n";
#endif
		//____________________________

		m_pCallback->setSamples(m_pSample);
		m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
	}

	return true;
}

boolean CDriverBrainProductsVAmp::stop(void)
{

	m_rDriverContext.getLogManager() << LogLevel_Trace << "STOP called.\n";
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	if(!m_rDriverContext.isStarted())
	{
		return false;
	}

	m_bFirstStart = false;

	return true;
}

boolean CDriverBrainProductsVAmp::uninitialize(void)
{
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	if(m_rDriverContext.isStarted())
	{
		return false;
	}

	uint32 l_uint32ErrorCode = faStop(m_oHeader.getDeviceId());
	if (l_uint32ErrorCode != FA_ERR_OK)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[STOP] VAmp Driver: faStop FAILED(" << l_uint32ErrorCode << ").\n";
		faClose(m_oHeader.getDeviceId());
		return false;
	}

	m_rDriverContext.getLogManager() << LogLevel_Trace << "Uninitialize called. Closing the device.\n";

	l_uint32ErrorCode = faClose(m_oHeader.getDeviceId());
	if (l_uint32ErrorCode != FA_ERR_OK)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[UINIT] VAmp Driver: faClose FAILED(" << l_uint32ErrorCode << ").\n";
		faClose(m_oHeader.getDeviceId());
		return false;
	}
	delete [] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverBrainProductsVAmp::isConfigurable(void)
{
	return true;
}

boolean CDriverBrainProductsVAmp::configure(void)
{
	CConfigurationBrainProductsVAmp m_oConfiguration(m_rDriverContext, "../share/openvibe-applications/acquisition-server/interface-BrainProducts-VAmp.ui", &m_oHeader); // the specific header is passed into the specific configuration

	if(!m_oConfiguration.configure(*(m_oHeader.getBasicHeader()))) // the basic configure will use the basic header
	{
		return false;
	}

	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "Pair names :\n";
		for(uint32 i = 0; i < m_oHeader.getPairCount();i++)
		{
			m_rDriverContext.getLogManager() << LogLevel_Trace << "  Pair " << i << " > " << m_oHeader.getPairName(i) << "\n";
		}
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyUSBFirstAmpAPI
