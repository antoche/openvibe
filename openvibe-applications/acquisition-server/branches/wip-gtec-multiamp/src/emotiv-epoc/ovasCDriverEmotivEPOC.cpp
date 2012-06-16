#if defined TARGET_HAS_ThirdPartyEmotivAPI

#include "ovasCDriverEmotivEPOC.h"
#include "ovasCConfigurationEmotivEPOC.h"

#include "edk.h"

#include <system/Time.h>

#include <system/Memory.h>

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
	m_bUseGyroscope = false;
	if(m_rDriverContext.getConfigurationManager().lookUpConfigurationTokenIdentifier("AcquisitionServer_PathToEmotivResearchSDK") == OV_UndefinedIdentifier)
	{
		//it's possible at first start of the acquisition server/ emotiv driver selection
		m_rDriverContext.getLogManager() << LogLevel_Trace << "[CONS] {AcquisitionServer_PathToEmotivResearchSDK} token unset. Ignore this message if you never used the Emotiv Driver.\n";
		m_sPathToEmotivSDK = "";
	}
	else
	{
		m_sPathToEmotivSDK = m_rDriverContext.getConfigurationManager().expand("${AcquisitionServer_PathToEmotivResearchSDK}");
	}
	m_sCommandForPathModification = "";

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

boolean CDriverEmotivEPOC::buildPath(void)
{
	char * l_sPath = getenv("PATH");
	if(l_sPath == NULL)
	{
		return false;
	}
	m_sCommandForPathModification = l_sPath + m_sPathToEmotivSDK+ ";";
	return true;
}

//___________________________________________________________________//
//                                                                   //
boolean CDriverEmotivEPOC::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	//we need to add the path to Emotiv SDK to PATH: done in external function 
	//because SEH (__try/__except) does not allow the use of local variables with destructor.
	if(!this->buildPath())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Failed to get the environment PATH.\n";
		return false;
	}
		
	if(_putenv_s("PATH",m_sCommandForPathModification) != 0)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Failed to modify the environment PATH with the Emotiv SDK path.\n";
		return false;
	}
	
	m_oHeader.setChannelCount(14);
	if(m_bUseGyroscope)
	{
		m_oHeader.setChannelCount(16); // 14 + 2 REF + 2 Gyro 
	}
	
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

	if(m_bUseGyroscope)
	{
		m_oHeader.setChannelName(14, "Gyro-X");
		m_oHeader.setChannelName(15, "Gyro-Y");
	}

	m_oHeader.setSamplingFrequency(128); // let's hope so...

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
	//m_pBuffer=new float64[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	if(!m_pSample /*|| !m_pBuffer*/)
	{
		delete [] m_pSample;
		//delete [] m_pBuffer;
		m_pSample=NULL;
		//m_pBuffer=NULL;
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Samples allocation failed.\n";
		return false;
	}

	//__________________________________
	// Hardware initialization

	m_bReadyToCollect = false;

	// First call to a function from EDK.DLL: guard the call with __try/__except clauses.
	__try
	{
		m_tEEEventHandle = EE_EmoEngineEventCreate();
		m_ui32EDK_LastErrorCode = EE_EngineConnect();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: First call to 'edk.dll' failed.\n"
			<< "\tTo use this driver you must have the Emotiv SDK Research Edition (or above) installed on your computer.\n"
			<< "\tAt first use please provide in the driver properties the path to your Emotiv SDK (root directory)\n"
			<< "\te.g. \"C:\\Program Files (x86)\\Emotiv Research Edition SDK_v1.0.0.4-PREMIUM\"\n"
			<< "\tThis path will be saved for further use automatically.\n";

		return false;
	}

	if (m_ui32EDK_LastErrorCode != EDK_OK)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emotiv Driver: Can't connect to EmoEngine. EDK Error Code [" << m_ui32EDK_LastErrorCode << "]\n";
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

	m_tDataHandle = EE_DataCreate();
	//float l_fBufferSizeInSeconds = (float32)m_ui32SampleCountPerSentBlock/(float32)m_oHeader.getSamplingFrequency();
	float l_fBufferSizeInSeconds = 1;
	m_ui32EDK_LastErrorCode = EE_DataSetBufferSizeInSec(l_fBufferSizeInSeconds);
	if (m_ui32EDK_LastErrorCode != EDK_OK) {
		m_rDriverContext.getLogManager() << LogLevel_Error << "[START] Emotiv Driver: Set buffer size to ["<<l_fBufferSizeInSeconds<< "] sec failed. EDK Error Code [" << m_ui32EDK_LastErrorCode << "]\n";
		return false;
	}
	m_rDriverContext.getLogManager() << LogLevel_Trace << "[START] Emotiv Driver: Data Handle created. Buffer size set to ["<<l_fBufferSizeInSeconds<< "] sec.\n";

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
				m_rDriverContext.getLogManager() << LogLevel_Trace << "[LOOP] Emotiv Driver: User #" << m_ui32UserID << " registered.\n";
				m_ui32EDK_LastErrorCode = EE_DataAcquisitionEnable(m_ui32UserID, true);
				if (m_ui32EDK_LastErrorCode != EDK_OK) 
				{
					m_rDriverContext.getLogManager() << LogLevel_Error << "[LOOP] Emotiv Driver: Enabling acquisition failed. EDK Error Code [" << m_ui32EDK_LastErrorCode << "]\n";
					return false;
				}
				m_bReadyToCollect = true;
			}
		}

		if(m_bReadyToCollect)
		{
			uint32 l_ui32nSamplesTaken=0;
			m_ui32EDK_LastErrorCode = EE_DataUpdateHandle(m_ui32UserID, m_tDataHandle);
			if(m_ui32EDK_LastErrorCode != EDK_OK)
			{
				m_rDriverContext.getLogManager() << LogLevel_Error << "[LOOP] Emotiv Driver: An error occured while updating the DataHandle. EDK Error Code [" << m_ui32EDK_LastErrorCode << "]\n";
				return false;
			}
			m_ui32EDK_LastErrorCode = EE_DataGetNumberOfSample(m_tDataHandle, &l_ui32nSamplesTaken);
			if(m_ui32EDK_LastErrorCode != EDK_OK)
			{
				m_rDriverContext.getLogManager() << LogLevel_Error << "[LOOP] Emotiv Driver: An error occured while getting new samples from device. EDK Error Code [" << m_ui32EDK_LastErrorCode << "]\n";
				return false;
			}
			// warning : if you connect/disconnect then reconnect, the internal buffer may be full of samples, thus maybe l_ui32nSamplesTaken > m_ui32SampleCountPerSentBlock
			m_rDriverContext.getLogManager() << LogLevel_Debug << "EMOTIV EPOC >>> received ["<< l_ui32nSamplesTaken <<"] samples per channel from device.\n";
			
			
			/*for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
			{
				for(uint32 s=0;s<l_ui32nSamplesTaken;s++)
				{
					double* l_pBuffer = new double[l_ui32nSamplesTaken];
					m_ui32EDK_LastErrorCode = EE_DataGet(m_tDataHandle, g_ChannelList[i], l_pBuffer, l_ui32nSamplesTaken);
					if(m_ui32EDK_LastErrorCode != EDK_OK)
					{
						m_rDriverContext.getLogManager() << LogLevel_Error << "[LOOP] Emotiv Driver: An error occured while getting new samples from device. EDK Error Code [" << m_ui32EDK_LastErrorCode << "]\n";
						return false;
					}
					
					m_rDriverContext.getLogManager() << LogLevel_Debug << "EMOTIV EPOC >>> adding sample with value ["<< l_pBuffer[s] <<"]\n";
					m_pSample[i*m_ui32SampleCountPerSentBlock + s] = (float32)l_pBuffer[s];
					delete [] l_pBuffer;
				}
				m_pCallback->setSamples(m_pSample);
			}
			m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
			*/

			double* l_pBuffer = new double[l_ui32nSamplesTaken];
			for(uint32 s=0; s<l_ui32nSamplesTaken; s++)
			{
				for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
				{
					m_ui32EDK_LastErrorCode = EE_DataGet(m_tDataHandle, g_ChannelList[i], l_pBuffer, l_ui32nSamplesTaken);
					if(m_ui32EDK_LastErrorCode != EDK_OK)
					{
						m_rDriverContext.getLogManager() << LogLevel_Error << "[LOOP] Emotiv Driver: An error occured while getting new samples from device. EDK Error Code [" << m_ui32EDK_LastErrorCode << "]\n";
						return false;
					}
					m_pSample[i] = (float32)l_pBuffer[s];
					if(s==0)m_rDriverContext.getLogManager() << LogLevel_Debug << "EMOTIV EPOC >>> sample received has value ["<< l_pBuffer[s] <<"]\n";
					//m_rDriverContext.getLogManager() << LogLevel_Debug << "EMOTIV EPOC >>> sample stored has value ["<< m_pSample[i] <<"]\n";
				}
				m_pCallback->setSamples(m_pSample, 1);
			}
			m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
			delete [] l_pBuffer;
			
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
	CConfigurationEmotivEPOC m_oConfiguration(m_rDriverContext, "../share/openvibe-applications/acquisition-server/interface-Emotiv-EPOC.ui", m_bUseGyroscope, m_sPathToEmotivSDK); 

	if(!m_oConfiguration.configure(m_oHeader)) 
	{
		return false;
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyEmotivAPI
