#if defined TARGET_OS_Windows

#include "ovasCDriverMitsarEEG202A.h"

#include "ovasCConfigurationMitsarEEG202A.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>
#include <system/Memory.h>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <math.h>

#include <windows.h>
#define boolean OpenViBE::boolean
#define msleep(ms) Sleep(ms) // Sleep windows

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

//___________________________________________________________________//
//                                                                   //

CDriverMitsarEEG202A::CDriverMitsarEEG202A(IDriverContext& rDriverContext)
	:IDriver(rDriverContext),
	m_pCallback(NULL),
	m_ui32RefIndex(0),
	m_bEventAndBioChannelsState(false)
{
	m_oHeader.setChannelCount(CHANNEL_NB);
	m_oHeader.setSamplingFrequency(SAMPLING_RATE);
}

void CDriverMitsarEEG202A::release(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::release\n";
	delete this;
}

const char* CDriverMitsarEEG202A::getName(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::getName\n";
	return "Mitsar EEG 202 - A";
}

//___________________________________________________________________//
//                                                                   //


#define _Mitsar_EEG202A_DLLFileName_ "MitsarDll.dll"

typedef int32 ( __stdcall *MitsarDLL_initialize)();
typedef int32 ( __stdcall *MitsarDLL_start)(int Ref_type);
typedef int32 ( __stdcall *MitsarDLL_stop)();
typedef int32 ( __stdcall *MitsarDLL_uninitialize)();
typedef int32 ( __stdcall *MitsarDLL_loop)(float32* pSample);

static HINSTANCE g_hMitsarDLLInstance = NULL;
static MitsarDLL_initialize g_fpMitsarDLLInitialize = NULL;
static MitsarDLL_start g_fpMitsarDLLStart = NULL;
static MitsarDLL_stop g_fpMitsarDLLStop = NULL;
static MitsarDLL_uninitialize g_fpMitsarDLLUninitialize = NULL;
static MitsarDLL_loop g_fpMitsarDLLLoop = NULL;

//___________________________________________________________________//
//                                                                   //

boolean CDriverMitsarEEG202A::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::initialize\n";
	
	if (m_rDriverContext.isConnected())
	{
		return false;
	}

	g_hMitsarDLLInstance = ::LoadLibrary(_Mitsar_EEG202A_DLLFileName_);

	if (!g_hMitsarDLLInstance)
	{ 
		std::cout << "INIT ERROR : Load Library" << std::endl;
		return false;
	}

	g_fpMitsarDLLInitialize   = (MitsarDLL_initialize)GetProcAddress(g_hMitsarDLLInstance, "MITSAR_EEG202_initialize");
	g_fpMitsarDLLStart        = (MitsarDLL_start)GetProcAddress(g_hMitsarDLLInstance, "MITSAR_EEG202_start");
	g_fpMitsarDLLStop         = (MitsarDLL_stop)GetProcAddress(g_hMitsarDLLInstance, "MITSAR_EEG202_stop");
	g_fpMitsarDLLUninitialize = (MitsarDLL_uninitialize)GetProcAddress(g_hMitsarDLLInstance, "MITSAR_EEG202_uninitialize");
	g_fpMitsarDLLLoop         = (MitsarDLL_loop)GetProcAddress(g_hMitsarDLLInstance, "MITSAR_EEG202_loop");
	
	if (!g_fpMitsarDLLInitialize || !g_fpMitsarDLLStart || !g_fpMitsarDLLStop || !g_fpMitsarDLLUninitialize || !g_fpMitsarDLLLoop)
	{
		std::cout << "INIT ERROR : DLL functions list" << std::endl;
		std::cout << "g_fpMitsarDLLInitialize : "	<< g_fpMitsarDLLInitialize		<< std::endl;
		std::cout << "g_fpMitsarDLLStart : "		<< g_fpMitsarDLLStart			<< std::endl;
		std::cout << "g_fpMitsarDLLStop : "			<< g_fpMitsarDLLStart			<< std::endl;
		std::cout << "g_fpMitsarDLLUninitialize : " << g_fpMitsarDLLUninitialize	<< std::endl;
		std::cout << "g_fpMitsarDLLLoop : "			<< g_fpMitsarDLLLoop			<< std::endl;
		::FreeLibrary(g_hMitsarDLLInstance);
		g_hMitsarDLLInstance      = NULL;
		g_fpMitsarDLLInitialize   = NULL;
		g_fpMitsarDLLStart        = NULL;
		g_fpMitsarDLLStop         = NULL;
		g_fpMitsarDLLUninitialize = NULL;
		g_fpMitsarDLLLoop         = NULL;
		return false;
	}

	int32 l_i32Error = g_fpMitsarDLLInitialize();
	if (l_i32Error)
	{
		::FreeLibrary(g_hMitsarDLLInstance);
		g_hMitsarDLLInstance      = NULL;
		g_fpMitsarDLLInitialize   = NULL;
		g_fpMitsarDLLStart        = NULL;
		g_fpMitsarDLLStop         = NULL;
		g_fpMitsarDLLUninitialize = NULL;
		g_fpMitsarDLLLoop         = NULL;
		std::cout << "INIT ERROR : Init DLL function" << std::endl;
		return false;
	}
 
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::CDriverMitsarEEG202A\n";
	
	if (this->m_bEventAndBioChannelsState)
	m_oHeader.setChannelCount(CHANNEL_NB);
	else m_oHeader.setChannelCount(CHANNEL_NB-2);

	m_oHeader.setSamplingFrequency(SAMPLING_RATE);

	OpenViBE::uint32 l_ui32ChanIndex = 0;
	m_oHeader.setChannelName(0, "FP1");
	m_oHeader.setChannelName(1, "FPz");
	m_oHeader.setChannelName(2, "FP2");
	m_oHeader.setChannelName(3, "F7");
	m_oHeader.setChannelName(4, "F3");
	m_oHeader.setChannelName(5, "Fz");
	m_oHeader.setChannelName(6, "F4");
	m_oHeader.setChannelName(7, "F8");
	m_oHeader.setChannelName(8, "FT7");
	m_oHeader.setChannelName(9, "FC3");
	m_oHeader.setChannelName(10, "FCz");
	m_oHeader.setChannelName(11, "FC4");
	m_oHeader.setChannelName(12, "FT8");
	m_oHeader.setChannelName(13, "T3");
	m_oHeader.setChannelName(14, "C3");
	m_oHeader.setChannelName(15, "Cz");
	m_oHeader.setChannelName(16, "C4");
	m_oHeader.setChannelName(17, "T4");
	m_oHeader.setChannelName(18, "TP7");
	m_oHeader.setChannelName(19, "CP3");
	m_oHeader.setChannelName(20, "CPz");
	m_oHeader.setChannelName(21, "CP4");
	m_oHeader.setChannelName(22, "TP8");
	m_oHeader.setChannelName(23, "T5");
	m_oHeader.setChannelName(24, "P3");
	m_oHeader.setChannelName(25, "Pz");
	m_oHeader.setChannelName(26, "P4");
	m_oHeader.setChannelName(27, "T6");
	m_oHeader.setChannelName(28, "O1");
	m_oHeader.setChannelName(29, "Oz");
	m_oHeader.setChannelName(30, "O2");

	if (this->m_bEventAndBioChannelsState)
	{
		m_oHeader.setChannelName(31, "CH_Event");    // Event signals (0/+3v)
		m_oHeader.setChannelName(32, "Bio1");        // Biological signals (ECG, EMG, EOG...)
	}
	
	OpenViBE::uint32 l_ui32StimulationChannel	= 31;

	m_vInputSamples.resize(SAMPLES_NB*CHANNEL_NB);

	m_vOutputSamples.resize(SAMPLES_NB*CHANNEL_NB);

	m_ui32LastStimulation		= STIMULATION_0;
	// memorize the stimulation channel position
	m_pfl32StimulationChannel = &m_vOutputSamples[SAMPLES_NB*l_ui32StimulationChannel];
	
	m_vStimulationIdentifier.resize(SAMPLES_NB);
	m_vStimulationDate.resize(SAMPLES_NB);

	m_pCallback = &rCallback;

	return true;
}

boolean CDriverMitsarEEG202A::start(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Info << "CDriverMitsarEEG202A::start\n";


	if (!m_rDriverContext.isConnected())
	{	
		m_rDriverContext.getLogManager() << LogLevel_Error << "CDriverMitsarEEG202A::start - not connected.\n";
		return false; 
	}
	if (m_rDriverContext.isStarted())
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "CDriverMitsarEEG202A::start - already started.\n";
		return false; 
	}

	//) Check reference type : 1->Ref=A1-Left A2-Righ else Ref=Common(A1&A2) 
	int Ref_type = 1;
	if (m_ui32RefIndex==1)
	{
		printf("Ref= A1-Left A2-Right\n");
		Ref_type = 1;
	}
	else
	{	
		printf("Ref=Common(A1&A2) \n");
		Ref_type = 0;
	}

	int32 l_i32Error = g_fpMitsarDLLStart(Ref_type);
	printf("Dll start %s\n", l_i32Error ? "WRONG" : "OK");

	return (l_i32Error ? false : true);

}

boolean CDriverMitsarEEG202A::loop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverMitsarEEG202A::loop\n";

    if (!m_rDriverContext.isConnected()) { return false; }
    if (!m_rDriverContext.isStarted()) { return true; }

	if (g_fpMitsarDLLLoop(&m_vInputSamples[0]))
	{
		return false;
	}

	// INPUT  s1 c1 c2 c3 ....cN
	//		  s2 c1 c2 c3 ....cN
	//		  s3 c1 c2 c3 ....cN
	//		  sM c1 c2 c3 ....cN
	// OUTPUT c1 s1 s2 s3 ....sM
	//        c2 s1 s2 s3 ....sM
	//        c3 s1 s2 s3 ....sM
	//        cN s1 s2 s3 ....sM

	// transpose data from sample lines to channel lines
	for (uint32 j=0; j < CHANNEL_NB; j++) // channel
	{
		for (uint32 i=0; i < SAMPLES_NB; i++) // sample
		{			
			m_vOutputSamples[j*SAMPLES_NB + i] = m_vInputSamples[CHANNEL_NB*i + j];//CHANNEL_NB
		}
	}
	
	if (m_bEventAndBioChannelsState)
	{
			// look for stimulations
			OpenViBE::uint32   l_ui32NbStimulations      = 0;  
			OpenViBE::float32* l_pfl32StimulationChannel = m_pfl32StimulationChannel;

			for (OpenViBE::uint32 iSample=0; iSample < SAMPLES_NB; iSample++, l_pfl32StimulationChannel++)
			{	
				OpenViBE::uint32 l_ui32Stimulation = STIMULATION_0;

		        //std::cout << *l_pfl32StimulationChannel << " " << l_ui32Stimulation << std::endl;

				// Stim192 < 0.03 <= Stim128 < 0.1 <= Stim64 < 0.16 <= Stim0
				OpenViBE::float32 l_f32Stimulation = *l_pfl32StimulationChannel;

				if ( l_f32Stimulation < 0.03)
				{
					l_ui32Stimulation          = STIMULATION_192;
					*l_pfl32StimulationChannel = 3.0f;
				}
				else if ( l_f32Stimulation < 0.1)
				{
					l_ui32Stimulation          = STIMULATION_128;
					*l_pfl32StimulationChannel = 2.0f;
				}
				else if ( l_f32Stimulation < 0.16)
				{
					l_ui32Stimulation          = STIMULATION_64;
					*l_pfl32StimulationChannel = 1.0f;
				}
				else
				{	
					*l_pfl32StimulationChannel = 0.0f;
				}


				if ( (l_ui32Stimulation != STIMULATION_0) && (l_ui32Stimulation != m_ui32LastStimulation) )
				{
					m_vStimulationIdentifier[l_ui32NbStimulations] = l_ui32Stimulation;
					m_vStimulationDate[l_ui32NbStimulations]       = iSample;
					l_ui32NbStimulations++;
				}

				m_ui32LastStimulation = l_ui32Stimulation;

		       //std::cout << *l_pfl32StimulationChannel << " " << l_ui32Stimulation << std::endl;
			}

			// prepare stimulations
			CStimulationSet   l_oStimulationSet;
			l_oStimulationSet.setStimulationCount(l_ui32NbStimulations);

			OpenViBE::uint64 identifier;

			for ( OpenViBE::uint32 iStimulation=0; iStimulation < l_ui32NbStimulations; iStimulation++ )
			{
				switch (m_vStimulationIdentifier[iStimulation])
				{
					case STIMULATION_64  : identifier = OVTK_StimulationId_Label_01; break;
					case STIMULATION_128 : identifier = OVTK_StimulationId_Label_02; break;
					case STIMULATION_192 : identifier = OVTK_StimulationId_Label_03; break;
				}
		
				l_oStimulationSet.setStimulationIdentifier(iStimulation, identifier);
				l_oStimulationSet.setStimulationDate(iStimulation, ( uint64(m_vStimulationDate[iStimulation]) << 32) / m_oHeader.getSamplingFrequency());
				l_oStimulationSet.setStimulationDuration(iStimulation, 1);

				//std::cout << "Trigger " << std::hex << m_vStimulationIdentifier[iStimulation] << " ";
			}

			//m_pCallback->setStimulationSet(l_oStimulationSet);
	}

	// Drift correction from GUI
	m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

	// SEND SAMPLES & STIMULATIONS
	m_pCallback->setSamples(&m_vOutputSamples[0],(uint32) SAMPLES_NB);
    

	msleep(1); // free CPU ressources
	return true;
}


boolean CDriverMitsarEEG202A::stop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::stop\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

	int32 l_i32Error = g_fpMitsarDLLStop();
	return (l_i32Error ? false : true);
}

boolean CDriverMitsarEEG202A::uninitialize(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::uninitialize\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	::FreeLibrary(g_hMitsarDLLInstance);
	m_pCallback               = NULL;
	g_hMitsarDLLInstance      = NULL;
	g_fpMitsarDLLInitialize   = NULL;
	g_fpMitsarDLLStart        = NULL;
	g_fpMitsarDLLStop         = NULL;
	g_fpMitsarDLLUninitialize = NULL;
	g_fpMitsarDLLLoop         = NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMitsarEEG202A::isConfigurable(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::isConfigurable\n";
	
	return true;
}

boolean CDriverMitsarEEG202A::configure(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverMitsarEEG202A::configure\n";
	
	CConfigurationMitsarEEG202A m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-Mitsar-EEG202.ui", m_ui32RefIndex, m_bEventAndBioChannelsState);
	
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}

	return true;
}

#endif