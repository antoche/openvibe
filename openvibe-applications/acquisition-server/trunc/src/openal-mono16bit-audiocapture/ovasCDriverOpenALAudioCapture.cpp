#if defined TARGET_HAS_ThirdPartyOpenAL
#include "ovasCDriverOpenALAudioCapture.h"
#include "ovasCConfigurationOpenALAudioCapture.h"

#include <openvibe-toolkit/ovtk_all.h>
#include <system/Time.h>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CDriverOpenALAudioCapture::CDriverOpenALAudioCapture(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
{
	m_oHeader.setSamplingFrequency(8192);
	m_oHeader.setChannelCount(1);
	Device = NULL;
	Context = NULL;
	CaptureDevice = NULL;
}

CDriverOpenALAudioCapture::~CDriverOpenALAudioCapture(void)
{
}

const char* CDriverOpenALAudioCapture::getName(void)
{
	return "OpenAL audio capture";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverOpenALAudioCapture::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverOpenALAudioCapture::initialize\n";
	
	if(m_rDriverContext.isConnected()) return false;
	if(!m_oHeader.isChannelCountSet()||!m_oHeader.isSamplingFrequencySet()) return false;
	
	m_pSample=new float32[ui32SampleCountPerSentBlock];
	if(!m_pSample)
	{
		delete [] m_pSample;
		m_pSample=NULL;
		return false;
	}
	
	Samples=new ALshort[ui32SampleCountPerSentBlock];
	if(!Samples)
	{
		delete [] Samples;
		Samples=NULL;
		return false;
	}
	
	// Open default audio device
	Device = alcOpenDevice(NULL);
	if (!Device)
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Default audio device opening failed.\n";
		return false;
	}
	
	// Create an audio context
	Context = alcCreateContext(Device, NULL);
	if (!Context)
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Audio context creation failed.\n";
		return false;
	}
	
	// Activate context
	if (!alcMakeContextCurrent(Context))
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Audio context activation failed.\n";
		return false;
	}

	// Verify if audio capture is supported by the computer
	if (alcIsExtensionPresent(Device, "ALC_EXT_CAPTURE") == AL_FALSE)
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Default audio device does not support audio capture.\n";
		return false;
	}

	// Open capture device
	CaptureDevice = alcCaptureOpenDevice(NULL, (ALCsizei)m_oHeader.getSamplingFrequency(), AL_FORMAT_MONO16, (ALCsizei)m_oHeader.getSamplingFrequency());
	if (!CaptureDevice)
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Default capture device opening failed.\n";
		return false;
	}

	// Saves parameters
	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	return true;
}

boolean CDriverOpenALAudioCapture::start(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverOpenALAudioCapture::start\n";
	
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	// ...
	// request hardware to start
	// sending data
	// ...
	alcCaptureStart(CaptureDevice);

	return true;
}

boolean CDriverOpenALAudioCapture::loop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverOpenALAudioCapture::loop\n";
	
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return true;
	
	// Activate context
	if (!alcMakeContextCurrent(Context))
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Audio context activation failed.\n";
	}
	
	ALCint SamplesAvailable;
	
	do
	{
		alcGetIntegerv(CaptureDevice, ALC_CAPTURE_SAMPLES, 1, &SamplesAvailable); 
		if ((uint32)SamplesAvailable >= m_ui32SampleCountPerSentBlock)
		{
			alcCaptureSamples(CaptureDevice, &Samples[0], (ALCsizei)m_ui32SampleCountPerSentBlock);
			for(uint32 i=0; i<m_ui32SampleCountPerSentBlock; i++)
			{
				m_pSample[i] = (float32) Samples[i];
			}
			m_pCallback->setSamples(m_pSample);
			m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
		}
	} 
	while ((uint32)SamplesAvailable >= m_ui32SampleCountPerSentBlock);

	return true;
}

boolean CDriverOpenALAudioCapture::stop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverOpenALAudioCapture::start\n";
	
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return false;

	alcCaptureStop(CaptureDevice);

	return true;
}

boolean CDriverOpenALAudioCapture::uninitialize(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverOpenALAudioCapture::start\n";
	
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;
	
	// Close capture device
	alcCaptureCloseDevice(CaptureDevice);

	// Context desactivation
	alcMakeContextCurrent(NULL);

	// Context destruction
	alcDestroyContext(Context);

	// Close device
	alcCloseDevice(Device);
	
	delete [] m_pSample;
	m_pSample=NULL;
	
	delete [] Samples;
	Samples=NULL;
	
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //
boolean CDriverOpenALAudioCapture::isConfigurable(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverOpenALAudioCapture::isConfigurable\n";
	
	return true;
}

boolean CDriverOpenALAudioCapture::configure(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverOpenALAudioCapture::start\n";
	
	// Change this line if you need to specify some references to your driver attribute that need configuration, e.g. the connection ID.
	CConfigurationOpenALAudioCapture m_oConfiguration(m_rDriverContext,"../share/openvibe-applications/acquisition-server/interface-OpenALAudioCapture.ui");
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}
	return true;
}
#endif //TARGET_HAS_ThirdPartyOpenAL
