#if defined TARGET_HAS_ThirdPartyEmotivAPI

#include "ovasCDriverEmotivEPOC.h"
#include "ovasCConfigurationEmotivEPOC.h"

#include "edk.h"

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

static const EE_DataChannel_t g_ChannelList[] = 
{
	ED_AF3, ED_F7, ED_F3, ED_FC5, ED_T7, ED_P7, ED_O1, ED_O2, ED_P8, ED_T8, ED_FC6, ED_F4, ED_F8, ED_AF4, 
	ED_GYROX, ED_GYROY,
	ED_COUNTER,
	ED_TIMESTAMP, 
	ED_FUNC_ID, ED_FUNC_VALUE, 
	ED_MARKER, 
	ED_SYNC_SIGNAL
};

CDriverEmotivEPOC::CDriverEmotivEPOC(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_ui32TotalSampleCount(0)
	,m_pSample(NULL)
{
	m_oHeader.setChannelCount(14); // 14 + 2 REF electrodes

	m_oHeader.setChannelName(0,  "AF3");
	m_oHeader.setChannelName(1,  "F7");
	m_oHeader.setChannelName(2,  "F3");
	m_oHeader.setChannelName(3,  "FC5");
	m_oHeader.setChannelName(4,  "T7");
	m_oHeader.setChannelName(5,  "P7");
	m_oHeader.setChannelName(6,  "O1");
	m_oHeader.setChannelName(7,  "O2");
	m_oHeader.setChannelName(8,  "P8");
	m_oHeader.setChannelName(9,  "T8");
	m_oHeader.setChannelName(10, "FC6");
	m_oHeader.setChannelName(11, "F4");
	m_oHeader.setChannelName(12, "F8");
	m_oHeader.setChannelName(13, "AF4");

	m_oHeader.setSamplingFrequency(128); // let's hope so...

	m_ui32UserID = 0;
	m_bReadyToCollect = false;
	m_bFirstStart = true;
}

CDriverEmotivEPOC::~CDriverEmotivEPOC(void)
{
}

const char* CDriverEmotivEPOC::getName(void)
{
	return "Emotiv EPOC";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverEmotivEPOC::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "INIT called.\n";
	if(m_rDriverContext.isConnected())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Driver already initialized.\n";
		return false;
	}

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Channel count or frequency not set.\n";
		return false;
	}

	//---------------------------------------------------------
	// Builds up a buffer to store acquired samples. This buffer will be sent to the acquisition server later.

	m_pSample=new float32[m_oHeader.getChannelCount()];
	m_pBuffer=new float64[m_oHeader.getChannelCount()];
	if(!m_pSample || !m_pBuffer)
	{
		delete [] m_pSample;
		delete [] m_pBuffer;
		m_pSample=NULL;
		m_pBuffer=NULL;
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Samples allocation failed.\n";
		return false;
	}

	//__________________________________
	// Hardware initialization

	m_bReadyToCollect = false;
	m_tEEEventHandle = EE_EmoEngineEventCreate();
	int32 l_i32ErrorCode = EE_EngineConnect();
	if (l_i32ErrorCode != EDK_OK) {
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Can't connect to EmoEngine (error code "<<l_i32ErrorCode<<").\n";
		return false;
	}
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "[INIT] Emotiv Driver: Connection to EmoEngine successful.\n";
	}

	//__________________________________
	// Saves parameters

	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;

	return true;
}

boolean CDriverEmotivEPOC::start(void)
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

	//TODO
	m_tDataHandle = EE_DataCreate();
	EE_DataSetBufferSizeInSec(1);
	m_rDriverContext.getLogManager() << LogLevel_Trace << "Data Handle created. Buffer size set to 1 sec.\n";

	return true;
}

boolean CDriverEmotivEPOC::loop(void)
{
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	if(m_rDriverContext.isStarted())
	{
		if(EE_EngineGetNextEvent(m_tEEEventHandle) == EDK_OK)
		{
			EE_Event_t l_tEventType = EE_EmoEngineEventGetType(m_tEEEventHandle);
			EE_EmoEngineEventGetUserId(m_tEEEventHandle, &m_ui32UserID);

			if (l_tEventType == EE_UserAdded)
			{
				m_rDriverContext.getLogManager() << LogLevel_Trace << "User #" << m_ui32UserID << " registered.\n";
				EE_DataAcquisitionEnable(m_ui32UserID, true);
				m_bReadyToCollect = true;
			}
		}

		if(m_bReadyToCollect)
		{
			uint32 l_ui32nSamplesTaken=0;

			EE_DataUpdateHandle(m_ui32UserID, m_tDataHandle);
			EE_DataGetNumberOfSample(m_tDataHandle, &l_ui32nSamplesTaken);

			// warning :  if you connect/disconnect then reconnect, the internal buffer may be full of samples, thus maybe l_ui32nSamplesTaken > m_ui32SampleCountPerSentBlock
			for(uint32 j=0; j<l_ui32nSamplesTaken; j++)
			{
				for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
				{
					EE_DataGet(m_tDataHandle, g_ChannelList[i], m_pBuffer, m_oHeader.getChannelCount());
					m_pSample[i] = float32(m_pBuffer[j]); /* *m_oHeader.getChannelGain(i); */
				}
				m_pCallback->setSamples(m_pSample, 1);
			}

			m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
		}
	}

	return true;
}

boolean CDriverEmotivEPOC::stop(void)
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

	EE_DataFree(m_tDataHandle);

	return true;
}

boolean CDriverEmotivEPOC::uninitialize(void)
{
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	if(m_rDriverContext.isStarted())
	{
		return false;
	}

	EE_EngineDisconnect();
	EE_EmoEngineEventFree(m_tEEEventHandle);
		
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverEmotivEPOC::isConfigurable(void)
{
	return true;
}

boolean CDriverEmotivEPOC::configure(void)
{
	CConfigurationEmotivEPOC m_oConfiguration(m_rDriverContext, "../share/openvibe-applications/acquisition-server/interface-Emotiv-EPOC.ui"); 

	if(!m_oConfiguration.configure(m_oHeader)) 
	{
		return false;
	}


	return true;
}

#endif // TARGET_HAS_ThirdPartyEmotivAPI
