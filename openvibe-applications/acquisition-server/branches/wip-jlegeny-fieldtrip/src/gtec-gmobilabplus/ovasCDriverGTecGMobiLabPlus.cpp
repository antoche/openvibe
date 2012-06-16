/**
 * The gMobilab driver was contributed
 * by Lucie Daubigney from Supelec Metz
 */

#include "ovasCDriverGTecGMobiLabPlus.h"
#include "ovasCConfigurationGTecGMobiLabPlus.h"
#include "../ovasCConfigurationBuilder.h"

#if defined TARGET_HAS_ThirdPartyGMobiLabPlusAPI

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#define boolean OpenViBE::boolean

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

static const uint32 g_ui32AcquiredChannelCount=8;

//___________________________________________________________________//
//                                                                   //

//constructor
CDriverGTecGMobiLabPlus::CDriverGTecGMobiLabPlus(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pHeader(NULL)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::CDriverGTecGMobiLabPlus\n";

	m_pHeader = new CHeader();
	m_pHeader->setSamplingFrequency(256);
	m_pHeader->setChannelCount(8);

	m_oBuffer.pBuffer = NULL;
	m_oBuffer.size = 0;
	m_oBuffer.validPoints = 0;
	m_oPortName="/dev/rfcomm0";

	//initialisation of the analog channels of the gTec module : by default no analog exchange are allowed
	m_oAnalogIn.ain1 = false;
	m_oAnalogIn.ain2 = false;
	m_oAnalogIn.ain3 = false;
	m_oAnalogIn.ain4 = false;
	m_oAnalogIn.ain5 = false;
	m_oAnalogIn.ain6 = false;
	m_oAnalogIn.ain7 = false;
	m_oAnalogIn.ain8 = false;
}

CDriverGTecGMobiLabPlus::~CDriverGTecGMobiLabPlus(void)
{
	delete m_pHeader;
}

void CDriverGTecGMobiLabPlus::release(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::release\n";
	delete this;
}

const char* CDriverGTecGMobiLabPlus::getName(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::getName\n";
	return "gTec gMOBIlab+";
}

//___________________________________________________________________//
//                                                                   //

/*
 * configuration
 */

boolean CDriverGTecGMobiLabPlus::isConfigurable(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::isConfigurable\n";
	return true;
}

boolean CDriverGTecGMobiLabPlus::configure(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::configure\n";

	// We use CConfigurationGTecMobilabPlus configuration which is a class that inheritate from the CConfigurationBuilder class
	// The difference between these two classes is the addition of a member of class. This member allows to change the port where is connected the device.
	CConfigurationGTecGMobiLabPlus m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-GTec-GMobiLabPlus.ui");

	// We configure the Header with it...
	if(!m_oConfiguration.configure(*m_pHeader))
	{
		return false;
	}

	//...and the port name
	m_oPortName=m_oConfiguration.getPortName();
	m_rDriverContext.getLogManager() << LogLevel_Debug << "Port name after configuration " << CString(m_oPortName.c_str()) << " \n";
	return true;
}

//___________________________________________________________________//
//                                                                   //

/*
 * initialisation
 */
boolean CDriverGTecGMobiLabPlus::initialize(const uint32 ui32SampleCountPerSentBlock, IDriverCallback& rCallback)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::initialize\n";
	m_rDriverContext.getLogManager() << LogLevel_Debug << "Port name after initialisation " << CString(m_oPortName.c_str()) << "\n";

	if(m_rDriverContext.isConnected())
	{
		return false;
	}

	if(!m_pHeader->isChannelCountSet() || !m_pHeader->isSamplingFrequencySet())
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "either channel count or sampling frequency is not set\n";
		return false;
	}

	uint32 l_ui32ChannelCount = m_pHeader->getChannelCount();

	// analog exchanges allowed on the first "l_ui32CHannelCount" channels:
	for(uint32 i=1; i<=l_ui32ChannelCount; i++)
	{
		CDriverGTecGMobiLabPlus::allowAnalogInputs(i);
	}

	// then buffer of type _BUFFER_ST built to store acquired samples.
	m_oBuffer.pBuffer=new short int[l_ui32ChannelCount];//allocate enough space for the buffer m_oBuffer.pBuffer ; only one set of mesures is acquired (channel 1 to 8) in a row
	m_oBuffer.size=l_ui32ChannelCount*sizeof(short int);
	m_oBuffer.validPoints=0;

	// allocates enough space for m_pSample
	m_pSample=new float32[ui32SampleCountPerSentBlock*l_ui32ChannelCount];

	// if there is a problem while creating the two arrays
	if(!m_oBuffer.pBuffer || !m_pSample)
	{
		delete [] m_oBuffer.pBuffer;
		delete [] m_pSample;
		m_pSample=NULL;
		m_oBuffer.pBuffer=NULL;
		return false;
	}

	// initializes hardware and get
	// available header information
	// from it
	m_oDevice=::GT_OpenDevice(m_oPortName.c_str());
	if(m_oDevice==0)
	{
		delete [] m_oBuffer.pBuffer;
		delete [] m_pSample;
		return false;
	}

	// saves parameters
	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	return true;
}

boolean CDriverGTecGMobiLabPlus::uninitialize(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::uninitialize\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	// uninitializes hardware here
	if (!::GT_CloseDevice(m_oDevice))
	{
		return false;
	}

	// frees memory
	delete [] m_pSample;
	delete [] m_oBuffer.pBuffer;

	m_pSample=NULL;
	m_oBuffer.pBuffer=NULL;
	m_pCallback=NULL;

	// uninitialisation of the analog channels : set valus to default ones
	m_oAnalogIn.ain1 = false;
	m_oAnalogIn.ain2 = false;
	m_oAnalogIn.ain3 = false;
	m_oAnalogIn.ain4 = false;
	m_oAnalogIn.ain5 = false;
	m_oAnalogIn.ain6 = false;
	m_oAnalogIn.ain7 = false;
	m_oAnalogIn.ain8 = false;

	return true;
}

const IHeader* CDriverGTecGMobiLabPlus::getHeader(void)
{
	return m_pHeader;
}

//___________________________________________________________________//
//                                                                   //

/*
 * acquisition
 */

boolean CDriverGTecGMobiLabPlus::start(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::start\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	// we use none of the digital inputs/outputs
	_DIO l_oDigitalInOut;
	l_oDigitalInOut.dio1_enable = false;
	l_oDigitalInOut.dio2_enable = false;
	l_oDigitalInOut.dio3_enable = false;
	l_oDigitalInOut.dio4_enable = false;
	l_oDigitalInOut.dio5_enable = false;
	l_oDigitalInOut.dio6_enable = false;
	l_oDigitalInOut.dio7_enable = false;
	l_oDigitalInOut.dio8_enable = false;

	// channel initialisation
	if(!::GT_InitChannels(m_oDevice, m_oAnalogIn, l_oDigitalInOut))
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "GT_InitChannels failed\n";
		return false;
	}

	// requests hardware to start sending data
	if(!::GT_StartAcquisition(m_oDevice))
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "GT_StartAcquisition failed\n";
		return false;
	}
	return true;
}

boolean CDriverGTecGMobiLabPlus::loop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::loop\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return true; }

	uint32 i, j;
	uint32 l_ui32ChannelCount=m_pHeader->getChannelCount();

	// only "l-ui32ChannelCount" measures corresponding to one measure per channel are acquired in a row with the function GT_GetData()
	// these measures are stored in m_oBuffer.pBuffer[]
	// the acquisition is reapeted m_ui32SampleCountPerSendBlock times to fill in the array "m_pSample"
	for(i=0 ; i<m_ui32SampleCountPerSentBlock ; i++)
	{
		if (!::GT_GetData(m_oDevice, &m_oBuffer))// receive samples from hardware (one per channel)
		{
			m_rDriverContext.getLogManager() << LogLevel_Trace << "GT_GetData failed\n";
			return false;
		}

		// here the "l_ui32ChannelCount" measures just acquired are stored in m_pSample not to be deleted by the next acquisition
		m_rDriverContext.getLogManager() << LogLevel_Debug << "Here are the " << l_ui32ChannelCount << " measures of the " << i << " th sample\n" << LogLevel_Debug;
		for(j=0; j<l_ui32ChannelCount; j++)
		{
			m_rDriverContext.getLogManager() << (m_oBuffer.pBuffer[j]*0.5)/32768. << " ";
			//operation made to modify the short int in a number between 0 and 500mV (in Volt)
			m_pSample[m_ui32SampleCountPerSentBlock*j+i] = (m_oBuffer.pBuffer[j]*0.5)/32768.;
		}
		m_rDriverContext.getLogManager() << "\n";
	}

	// the buffer is full : it is send to the acquisition server
	m_pCallback->setSamples(m_pSample);
	m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

	return true;
}

boolean CDriverGTecGMobiLabPlus::stop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGTecGMobiLabPlus::stop\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

	// requests the hardware to stop sending data
	if(!::GT_StopAcquisition(m_oDevice))
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "GT_StopAcquisition failed\n";
		return false;
	}

	return true;
}

// this function allows exchanges of data on the "ui32ChannelIndex" channel
// function used to initialize the analog inputs according to the number "channelCount"
void CDriverGTecGMobiLabPlus::allowAnalogInputs(uint32 ui32ChannelIndex)
{
	switch(ui32ChannelIndex)
	{
		case 8: m_oAnalogIn.ain8 = true; break;
		case 7: m_oAnalogIn.ain7 = true; break;
		case 6: m_oAnalogIn.ain6 = true; break;
		case 5: m_oAnalogIn.ain5 = true; break;
		case 4: m_oAnalogIn.ain4 = true; break;
		case 3: m_oAnalogIn.ain3 = true; break;
		case 2: m_oAnalogIn.ain2 = true; break;
		case 1: m_oAnalogIn.ain1 = true; break;
		default:
			m_rDriverContext.getLogManager() << LogLevel_Trace << "Unexpected value " << ui32ChannelIndex << " in CDriverGTecGMobiLabPlus::allowAnalogInputs\n";
			break;
	}
}

#endif // TARGET_HAS_ThirdPartyGMobiLabPlusAPI
