#if defined TARGET_HAS_ThirdPartyEmokit

#include "ovasCDriverEmokit.h"
#include "ovasCConfigurationEmokit.h"

#include <system/Time.h>

#include <system/Memory.h>

#include <emokit/emokit.h>

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <vector>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

#define boolean OpenViBE::boolean

CDriverEmokit::CDriverEmokit(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_pSample(NULL)
	,m_device(NULL)
{
	m_bUseGyroscope = false;
	m_ui32UserID = 0;
}

CDriverEmokit::~CDriverEmokit(void)
{
}

const char* CDriverEmokit::getName(void)
{
	return "Emokit";
}

//___________________________________________________________________//
//                                                                   //
boolean CDriverEmokit::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_bUseGyroscope)
	{
		m_oHeader.setChannelCount(16); // 14 + 2 Gyro 
	}
	else
	{
		m_oHeader.setChannelCount(14);
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

	m_oHeader.setSamplingFrequency(128);

	m_rDriverContext.getLogManager() << LogLevel_Trace << "INIT called.\n";
	if(m_rDriverContext.isConnected())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emokit Driver: Driver already initialized.\n";
		return false;
	}

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emokit Driver: Channel count or frequency not set.\n";
		return false;
	}

	//---------------------------------------------------------
	// Builds up a buffer to store acquired samples. This buffer will be sent to the acquisition server later.

	m_pSample=new float32[m_oHeader.getChannelCount()];
	if( !m_pSample )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emokit Driver: Samples allocation failed.\n";
		return false;
	}

	//__________________________________
	// Hardware initialization

	m_device = emokit_create();
	
	if( !m_device )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emokit: Failed to create emokit object\n";
		return false;
	}
	
	uint32_t nDevices = emokit_get_count( m_device, EMOKIT_VID, EMOKIT_PID );
	m_rDriverContext.getLogManager() << LogLevel_Info << "[INIT] Emokit: " << nDevices << " devices connected.\n";
	
	if( m_ui32UserID >= nDevices )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emokit: There is no headset with ID " << m_ui32UserID << "\n";
		emokit_delete( m_device );
		m_device = NULL;
		return false;
	}
	
	if( emokit_open( m_device, EMOKIT_VID, EMOKIT_PID, m_ui32UserID ) != 0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "[INIT] Emokit: Could not connect to headset " << m_ui32UserID << "\n";
		emokit_delete( m_device );
		m_device = NULL;
		return false;
	}
	

	//__________________________________
	// Save parameters

	m_pCallback=&rCallback;
	return true;
}

boolean CDriverEmokit::start(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "START called.\n";
	
	if( !m_device )
	{
		return false;
	}
	
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	if(m_rDriverContext.isStarted())
	{
		return false;
	}

	return true;
}

boolean CDriverEmokit::loop(void)
{
	if(!m_rDriverContext.isConnected())
	{
		return false;
	}

	if(m_rDriverContext.isStarted())
	{
		int read_result = emokit_read_data( m_device );
		if( read_result > 0 )
		{
			emokit_get_next_frame( m_device );
			m_pSample[0] = float32( m_device->current_frame.AF3 );
			m_pSample[1] = float32( m_device->current_frame.F7 );
			m_pSample[2] = float32( m_device->current_frame.F3 );
			m_pSample[3] = float32( m_device->current_frame.FC5 );
			m_pSample[4] = float32( m_device->current_frame.T7 );
			m_pSample[5] = float32( m_device->current_frame.P7 );
			m_pSample[6] = float32( m_device->current_frame.O1 );
			m_pSample[7] = float32( m_device->current_frame.O2 );
			m_pSample[8] = float32( m_device->current_frame.P8 );
			m_pSample[9] = float32( m_device->current_frame.T8 );
			m_pSample[10] = float32( m_device->current_frame.FC6 );
			m_pSample[11] = float32( m_device->current_frame.F4 );
			m_pSample[12] = float32( m_device->current_frame.F8 );
			m_pSample[13] = float32( m_device->current_frame.AF4 );
			if(m_bUseGyroscope)
			{
				m_pSample[14] = float32( m_device->current_frame.gyroX );
				m_pSample[15] = float32( m_device->current_frame.gyroY );
			}
			m_pCallback->setSamples( m_pSample, 1 );
			m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
		}
		else
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "[LOOP] emokit_read_data returned " << read_result << "\n";
		}
	
	}
	return true;
}

boolean CDriverEmokit::stop(void)
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
	return true;
}

boolean CDriverEmokit::uninitialize(void)
{
	if( m_device )
	{
		emokit_close( m_device );
		emokit_delete( m_device );
	}
	m_device = NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverEmokit::isConfigurable(void)
{
	return true;
}

boolean CDriverEmokit::configure(void)
{
	CConfigurationEmokit m_oConfiguration(m_rDriverContext, "../share/openvibe-applications/acquisition-server/interface-Emokit.ui", m_bUseGyroscope, m_ui32UserID); 

	if(!m_oConfiguration.configure(m_oHeader)) 
	{
		return false;
	}

	return true;
}

#endif //TARGET_HAS_ThirdPartyEmokit
