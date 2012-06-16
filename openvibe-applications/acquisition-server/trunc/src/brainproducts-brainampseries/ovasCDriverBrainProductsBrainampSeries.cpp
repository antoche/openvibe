/*
 * Brain Products Brainamp Series driver for OpenViBE
 * Copyright (C) 2010 INRIA - Author : Yann Renard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "ovasCDriverBrainProductsBrainampSeries.h"

#if defined OVAS_OS_Windows

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Memory.h>
#include <system/Time.h>

#include <list>

#include <windows.h>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
#define boolean OpenViBE::boolean

char* CDriverBrainProductsBrainampSeries::getErrorMessage(uint32 ui32Error)
{
	uint32 l_ui32Code=ui32Error&0xffff;
	uint32 l_ui32AmplifierId=ui32Error>>16;
	switch (l_ui32Code)
	{
		case 0:
			break;
		case 1:
			return
				"Connection between Brainamp and USB 2 Adapter / PCI is broken.\n"
				"Please check connectors, switch and battery power. After the\n"
				"connection is established and if you wish to continue the\n"
				"recording, please press the Start/Resume Recording button.\n"
				"If problem still persists, contact us at techsup@brainproducts.com";
		case 2:
			return
				"The voltage in the amplifier is too low!\n"
				"Check the batteries!";
		case 3:
			return
				"Could not establish communication with the amplifier.\n"
				"Check the connectors and the battery power!";
		case 4:
			return
				"Out of synch, Barker words missing!";
		case 5:
			return
				"Connection between USB 2 Adapter and Computer is broken.\n"
				"Monitoring or recording was interrupted. Please check\n"
				"the USB connectors. If problem still persists, contact\n"
				"us at techsup@brainproducts.com";
		default:
			return "Unknown Amplifier Error\n";
	}
	return "";
}

const ELogLevel LogLevel_TraceAPI=LogLevel_None;

//___________________________________________________________________//
//                                                                   //

CDriverBrainProductsBrainampSeries::CDriverBrainProductsBrainampSeries(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_oHeaderAdapter(m_oHeader, m_peChannelSelected)
	,m_pDevice(NULL)
	,m_pDeviceSetup(NULL)
//	,m_pDeviceCalibrationSettings(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
	,m_ui32USBIndex(1)
	,m_ui32DecimationFactor(10)
	,m_eLowPass(LowPass_250)
	,m_eResolution(Resolution_100nV)
	,m_eDCCoupling(DCCouping_AC)
	,m_eImpedance(Impedance_Low)
{
	// char* l_sChannelName[]={"Fp1", "Fp2", "F3", "F4", "C3", "C4", "P3", "P4", "O1", "O2", "F7", "F8", "T7", "T8", "P7", "P8", "Fz", "Cz", "Pz", "FC1", "FC2", "CP1", "CP2", "FC5", "FC6", "CP5", "CP6", "TP9", "TP10", "Eog", "Ekg1", "Ekg2", };
	char* l_sChannelNameActiCap32 []={"Fp1", "Fp2", "F7", "F3", "Fz", "F4", "F8", "FC5", "FC1", "FC2", "FC6", "T7", "C3", "Cz", "C4", "T8", "TP9", "CP5", "CP1", "CP2", "CP6", "TP10", "P7", "P3", "Pz", "P4", "P8", "PO9", "O1", "Oz", "O2", "PO10" };
	char* l_sChannelNameActiCap64 []={"Fp1", "Fp2", "F7", "F3", "Fz", "F4", "F8", "FC5", "FC1", "FC2", "FC6", "T7", "C3", "Cz", "C4", "T8", "TP9", "CP5", "CP1", "CP2", "CP6", "TP10", "P7", "P3", "Pz", "P4", "P8", "PO9", "O1", "Oz", "O2", "PO10",
	                                  "AF7", "AF3", "AF4", "AF8", "F5", "F1", "F2", "F6", "FT9", "FT7", "FC3", "FC4", "FT8", "FT10", "C5", "C1", "C2", "C6", "TP7", "CP3", "CPz", "CP4", "TP8", "P5", "P1", "P2", "P6", "PO7", "PO3", "POz", "PO4", "PO8", };
	char* l_sChannelNameActiCap128[]={"Fp1", "Fp2", "F7", "F3", "Fz", "F4", "F8", "FC5", "FC1", "FC2", "FC6", "T7", "C3", "Cz", "C4", "T8", "TP9", "CP5", "CP1", "CP2", "CP6", "TP10", "P7", "P3", "Pz", "P4", "P8", "PO9", "O1", "Oz", "O2", "PO10",
	                                  "AF7", "AF3", "AF4", "AF8", "F5", "F1", "F2", "F6", "FT9", "FT7", "FC3", "FC4", "FT8", "FT10", "C5", "C1", "C2", "C6", "TP7", "CP3", "CPz", "CP4", "TP8", "P5", "P1", "P2", "P6", "PO7", "PO3", "POz", "PO4", "PO8",
	                                  "FPz", "F9", "AFF5h", "AFF1h", "AFF2h", "AFF6h", "F10", "FTT9h", "FTT7h", "FCC5h", "FCC3h", "FCC1h", "FCC2h", "FCC4h", "FCC6h", "FTT8h", "FTT10h", "TPP9h", "TPP7h", "CPP5h", "CPP3h", "CPP1h", "CPP2h", "CPP4h", "CPP6h", "TPP8h", "TPP10h", "POO9h", "POO1", "POO2", "POO10h", "Iz",
	                                  "AFp1", "AFp2", "FFT9h", "FFT7h", "FFC5h", "FFC3h", "FFC1h", "FFC2h", "FFC4h", "FFC6h", "FFT8h", "FFT10h", "TTP7h", "CCP5h", "CCP3h", "CCP1h", "CCP2h", "CCP4h", "CCP6h", "TTP8h", "P9", "PPO9h", "PPO5h", "PPO1h", "PPO2h", "PPO6h", "PPO10h", "P10", "I1", "OI1h", "OI2h", "I2", };
	char** l_sChannelName=l_sChannelNameActiCap128;

	uint32 l_ui32AmplifierCount=1;
	this->getDeviceDetails(m_ui32USBIndex, &l_ui32AmplifierCount, NULL);

	m_oHeader.setSamplingFrequency(500);
	m_oHeader.setChannelCount(l_ui32AmplifierCount*32);
	for(uint32 i=0; i<256; i++)
	{
		m_oHeader.setChannelName(i, (i<sizeof(l_sChannelNameActiCap128)/sizeof(void*)?l_sChannelName[i]:""));
		m_peChannelSelected[i]=Channel_Selected;
		m_peLowPassFilterFull[i]=Parameter_Default;
		m_peResolutionFull[i]=Parameter_Default;
		m_peDCCouplingFull[i]=Parameter_Default;
	}
}

void CDriverBrainProductsBrainampSeries::release(void)
{
	delete this;
}

const char* CDriverBrainProductsBrainampSeries::getName(void)
{
	return "Brain Products BrainAmp Series";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverBrainProductsBrainampSeries::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
// #define ui32SampleCountPerSentBlock 20*5

	uint32 i, j;

	if(m_rDriverContext.isConnected()) { return false; }


	// --

	char l_sDeviceName[1024];
	::sprintf(l_sDeviceName, "\\\\.\\BrainAmpUSB%i", m_ui32USBIndex);
	m_rDriverContext.getLogManager() << LogLevel_Info << "Preparing device [" << CString(l_sDeviceName) << "]\n";

	// -- Gets amplifiers type

	uint32 l_ui32AmplifierCount=BrainAmp_MaximumAmplifierCount;
	uint32 l_pAmplifierType[BrainAmp_MaximumAmplifierCount];
	if(!this->getDeviceDetails(m_ui32USBIndex, &l_ui32AmplifierCount, l_pAmplifierType))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not get amplifier(s) type - got error " << uint32(::GetLastError()) << "\n";
		return false;
	}

	for(i=0; i<BrainAmp_MaximumAmplifierCount; i++)
	{
		CString l_sAmplifierType(::getDeviceType(l_pAmplifierType[i]));
		m_rDriverContext.getLogManager() << LogLevel_Info << " - Amplifier " << i+1 << " : " << l_sAmplifierType << "\n";
	}
	m_rDriverContext.getLogManager() << LogLevel_Info << "Found " << l_ui32AmplifierCount << " amplifier(s)\n";

	// -- Opens device

	m_pDevice=::CreateFile(l_sDeviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
	if(m_pDevice==INVALID_HANDLE_VALUE)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not open device [" << CString(l_sDeviceName) << "]\n";
		return false;
	}
	m_rDriverContext.getLogManager() << LogLevel_Info << "Opened device [" << CString(l_sDeviceName) << "]\n";

	// -- Gets driver version from device

	char l_sDriverVersion[1024];
	uint32 l_ui32DriverVersion=0;
	uint32 l_ui32DriverVersionMajor=0;
	uint32 l_ui32DriverVersionMinor=0;
	uint32 l_ui32DriverVersionPatch=0;
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_DRIVERVERSION\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_DRIVERVERSION, NULL, 0, &l_ui32DriverVersion, sizeof(l_ui32DriverVersion), &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not get driver version from device - got error " << uint32(::GetLastError()) << "\n";
		::CloseHandle(m_pDevice);
		return false;
	}
	l_ui32DriverVersionPatch= l_ui32DriverVersion%10000;
	l_ui32DriverVersionMinor=(l_ui32DriverVersion%1000000)/10000;
	l_ui32DriverVersionMajor=(l_ui32DriverVersion        )/1000000;
	::sprintf(l_sDriverVersion, "%i.%02i.%04i", l_ui32DriverVersionMajor, l_ui32DriverVersionMinor, l_ui32DriverVersionPatch);
	m_rDriverContext.getLogManager() << LogLevel_Info << "Found driver version [" << CString(l_sDriverVersion) << "]\n";

	// -- Gets serial number from device

	char l_sDeviceSerialNumber[1024];
	uint32 l_ui32DeviceSerialNumber=0;
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_GET_SERIALNUMBER\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_GET_SERIALNUMBER, NULL, 0, &l_ui32DeviceSerialNumber, sizeof(l_ui32DeviceSerialNumber), &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not get serial number from device - got error " << uint32(::GetLastError()) << "\n";
		::CloseHandle(m_pDevice);
		return false;
	}
	if(m_ui32BytesReturned != 0)
	{
		::sprintf(l_sDeviceSerialNumber, "0x%08x", l_ui32DeviceSerialNumber);
		m_rDriverContext.getLogManager() << LogLevel_Info << "Found serial number [" << CString(l_sDeviceSerialNumber) << "]\n";
	}

	// -- Sets callibration settings

	m_pDeviceCalibrationSettings=new CBrainampCalibrationSettings;
	// m_pDeviceCalibrationSettings->m_ui16WaveForm=WaveForm_Ramp;
	// m_pDeviceCalibrationSettings->m_ui16WaveForm=WaveForm_Triangle;
	// m_pDeviceCalibrationSettings->m_ui16WaveForm=WaveForm_Square;
	m_pDeviceCalibrationSettings->m_ui16WaveForm=WaveForm_SineWave;
	m_pDeviceCalibrationSettings->m_ui32Frequency=5;

	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_CALIBRATION_SETTINGS\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_CALIBRATION_SETTINGS, m_pDeviceCalibrationSettings, sizeof(CBrainampCalibrationSettings), NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not set calibration settings - got error " << uint32(::GetLastError()) << "\n";
		::CloseHandle(m_pDevice);
		return false;
	}

	// -- Configures device according to what has been enterd in the configuration dialog

	boolean l_bLowPassWarning = false;
	float32 l_pf32Resolution[] = {.1f, .5f, 10.f, 152.6f };

	m_pDeviceSetup=new OpenViBEAcquisitionServer::CBrainampSetup;
	System::Memory::set(m_pDeviceSetup, sizeof(CBrainampSetup), 0);
	m_pDeviceSetup->m_ui32ChannelCount=m_oHeaderAdapter.getChannelCount();
	m_pDeviceSetup->m_ui16HoldValue=0x0; // Value without trigger
	m_pDeviceSetup->m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock*m_ui32DecimationFactor;
	m_pDeviceSetup->m_ui8LowImpedance=uint8(m_eImpedance);
	for(j=0, i=0; i<m_oHeader.getChannelCount(); i++)
	{
		if(m_peChannelSelected[i]==Channel_Selected)
		{
			// Should I care about j being greater than 
			m_pDeviceSetup->m_pChannelLookup[j]=i;
			m_pDeviceSetup->m_pLowPassFilter[j]=uint8(m_peLowPassFilterFull[i]==Parameter_Default?m_eLowPass:m_peLowPassFilterFull[i]); // 0 - 1000Hz, 1 - 250Hz
			m_pDeviceSetup->m_pResolution[j]=uint8(m_peResolutionFull[i]==Parameter_Default?m_eResolution:m_peResolutionFull[i]); // 0 - 100 nV, 1 - 500 nV, 2 - 10 µV, 3 - 152.6 µV
			m_pDeviceSetup->m_pDCCoupling[j]=uint8(m_peDCCouplingFull[i]==Parameter_Default?m_eDCCoupling:m_peDCCouplingFull[i]); // 0 - AC, 1 - DC

			m_pf32ResolutionFactor[j]=l_pf32Resolution[m_pDeviceSetup->m_pResolution[j]];

			if(!l_bLowPassWarning && m_ui32DecimationFactor != 1 && (m_eLowPass==LowPass_1000 || m_peLowPassFilterFull[i]==LowPass_1000))
			{
				m_rDriverContext.getLogManager() << LogLevel_Warning << "Using " << uint32(1000) << " Hz low pass filter...\n";
				m_rDriverContext.getLogManager() << LogLevel_Warning << "Using a decimation factor of " << m_ui32DecimationFactor << " (resulting in " << uint32(5000/m_ui32DecimationFactor) << " Hz sampling rate)\n";
				m_rDriverContext.getLogManager() << LogLevel_Warning << "This is probably not safe. Signal quality will suffer.\n";
				l_bLowPassWarning=true;
			}

			j++;
		}
	}

	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_SETUP\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_SETUP, m_pDeviceSetup, sizeof(CBrainampSetup), NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not send setup parameters to device - got error " << uint32(::GetLastError()) << "\n";
		::CloseHandle(m_pDevice);
		return false;
	}

	// -- Allocates sample array and intermediate buffer

	m_pSample=new float32[m_oHeaderAdapter.getChannelCount()*ui32SampleCountPerSentBlock];
	m_pBuffer=new int16[(m_oHeaderAdapter.getChannelCount()+1)*ui32SampleCountPerSentBlock*m_ui32DecimationFactor];
	if(!m_pSample || !m_pBuffer)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not allocate memory for sample array / intermediate buffer\n";
		delete [] m_pSample;
		delete [] m_pBuffer;
		m_pSample=NULL;
		m_pBuffer=NULL;
		::CloseHandle(m_pDevice);
		return false;
	}

	// -- Everything is up and ready to work

	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_ui16Marker=0;

	// -- Optionnaly starts impedance check

	m_rDriverContext.getLogManager() << LogLevel_Trace << (m_rDriverContext.isImpedanceCheckRequested()?"Impedance will be checked":"Impedance won't be checked") << "\n";
	if(m_rDriverContext.isImpedanceCheckRequested())
	{
		if(!this->startImpedanceCheck())
		{
			return false;
		}
	}

	return true;
}

boolean CDriverBrainProductsBrainampSeries::start(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	// -- Optionnaly stops impedance check

	if(m_rDriverContext.isImpedanceCheckRequested())
	{
		if(!this->stopImpedanceCheck())
		{
			return false;
		}
	}

	// -- Configures pull up/down for triggers

	uint16 l_ui16PullUp=0;
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_DIGITALINPUT_PULL_UP\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_DIGITALINPUT_PULL_UP, &l_ui16PullUp, sizeof(l_ui16PullUp), NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not switch pull up\n";
		::CloseHandle(m_pDevice);
		return false;
	}

	// -- Starts acquisition

	uint32 l_ui32AcquisitionType=uint32(AcquisitionType_EEG);
	// uint32 l_ui32AcquisitionType=uint32(AcquisitionType_TestSignal);
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_START\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_START, &l_ui32AcquisitionType, sizeof(uint32), NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not start acquisition\n";
		return false;
	}

	return true;
}

boolean CDriverBrainProductsBrainampSeries::loop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted() && !m_rDriverContext.isImpedanceCheckRequested()) { return true; }

	uint32 i,j;

	// -- Gets error code from device

	uint32 l_ui32Error=0;
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_ERROR_STATE\n";
	if (!::DeviceIoControl(m_pDevice, BRAINAMP_ERROR_STATE, NULL, 0, &l_ui32Error, sizeof(l_ui32Error), &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not retrieve error state\n";
		return false;
	}

	// -- Checks error code

	if(l_ui32Error!=0)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Received error state " << l_ui32Error << " from device\n";
		m_rDriverContext.getLogManager() << LogLevel_Error << "Full error text :\n" << this->getErrorMessage(l_ui32Error) << "\n";
		return false;
	}

	// -- Loops until the buffer is read

	do
	{
		// -- Reads intermediate buffer from device

		if(!::ReadFile(m_pDevice, m_pBuffer, (m_oHeaderAdapter.getChannelCount()+1)*m_ui32SampleCountPerSentBlock*m_ui32DecimationFactor*sizeof(int16), &m_ui32BytesReturned, NULL))
		{
			uint32 l_ui32LastError=::GetLastError();
			m_rDriverContext.getLogManager() << LogLevel_Warning << "Could not read from device - Got error " << uint32(::GetLastError()) << " " << CString(l_ui32LastError==ERROR_MORE_DATA?"(buffer overflow)":"") << "\n";
			return true;
		}

		if(m_ui32BytesReturned==0)
		{
			System::Time::sleep(2);
		}
	}
	while(m_ui32BytesReturned==0);

#if 0
	// -- Gets buffer filling state

	static uint32 l_ui32LastBufferFillingState=0;
	static uint32 l_ui32BufferFillingState=0;
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_BUFFERFILLING_STATE\n";
	if (!::DeviceIoControl(m_pDevice, BRAINAMP_BUFFERFILLING_STATE, NULL, 0, &l_ui32BufferFillingState, sizeof(l_ui32BufferFillingState), &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not retrieve buffer filling state\n";
		return false;
	}
	if(l_ui32BufferFillingState!=l_ui32LastBufferFillingState)
	{
		l_ui32LastBufferFillingState=l_ui32BufferFillingState;
		m_rDriverContext.getLogManager() << LogLevel_Info << "Buffer filling state : " << l_ui32BufferFillingState << "\n";
	}
#endif

	if(!m_rDriverContext.isStarted())
	{
		// -- Converts the intermediate buffer in instant impedances

		float64 l_f64ImpedanceScale = 1000 * (m_eImpedance==Impedance_High ? 2.1e-3  : 2.13219e-4);
		for(i=0; i<m_oHeaderAdapter.getChannelCount(); i++)
		{
			int16 l_i16MinValue=32767;
			int16 l_i16MaxValue=-32767;
			int16* l_pBuffer=m_pBuffer+i;
			for(j=0; j<m_ui32SampleCountPerSentBlock*m_ui32DecimationFactor; j++)
			{
				if(*l_pBuffer > l_i16MaxValue) l_i16MaxValue=*l_pBuffer;
				if(*l_pBuffer < l_i16MinValue) l_i16MinValue=*l_pBuffer;
				l_pBuffer += m_oHeaderAdapter.getChannelCount()+1;
			}
			m_vImpedance[i]=(l_i16MaxValue-l_i16MinValue)*l_f64ImpedanceScale - 1. + .5;
		}

		// -- Backups this impedance in the impedance buffer and limit this buffer size

		m_vImpedanceBuffer.push_back(m_vImpedance);
		if(m_vImpedanceBuffer.size() > m_ui32ImpedanceCheckSignalFrequency)
		{
			m_vImpedanceBuffer.pop_front();
		}

		// -- Averages the impedances over last measures and send them to the acquisition server

		for(i=0; i<m_oHeaderAdapter.getChannelCount(); i++)
		{
			float64 l_f64AverageImpedance=0;
			std::list < std::vector < float64 > >::const_iterator it;
			for(it=m_vImpedanceBuffer.begin(); it!=m_vImpedanceBuffer.end(); it++)
			{
				l_f64AverageImpedance+=(*it)[i];
			}
			l_f64AverageImpedance/=m_vImpedanceBuffer.size();

			m_rDriverContext.updateImpedance(i, l_f64AverageImpedance);
		}
	}
	else
	{
		// -- Converts the intermediate buffer in acquisition server convenient format

		float32* l_pSample=m_pSample;
		int16* l_pBuffer;
		uint32 l_ui16Marker;
		for(i=0; i<m_oHeaderAdapter.getChannelCount(); i++)
		{
			l_pBuffer=m_pBuffer+i;
			for(j=0; j<m_ui32SampleCountPerSentBlock; j++)
			{
				*l_pSample = *l_pBuffer * m_pf32ResolutionFactor[i];

				l_pSample+=1;
				l_pBuffer+=m_ui32DecimationFactor*(m_oHeaderAdapter.getChannelCount()+1);
			}
		}

		CStimulationSet l_oStimulationSet;
		l_pBuffer=m_pBuffer+m_oHeaderAdapter.getChannelCount();
		for(j=0; j<m_ui32SampleCountPerSentBlock*m_ui32DecimationFactor; j++)
		{
			l_ui16Marker = *l_pBuffer;
			l_ui16Marker ^= m_pDeviceSetup->m_ui16HoldValue;
			if(l_ui16Marker != m_ui16Marker)
			{
				m_ui16Marker=l_ui16Marker;
				if(m_ui16Marker != 0)
				{
					l_oStimulationSet.appendStimulation(m_ui16Marker, (uint64(j)<<32)/(m_oHeader.getSamplingFrequency()*m_ui32DecimationFactor), 0);
					m_rDriverContext.getLogManager() << LogLevel_Trace << "Got stim code " << m_ui16Marker << " at sample " << j << "\n";
				}
			}
			l_pBuffer+=m_oHeaderAdapter.getChannelCount()+1;
		}

		// -- Sends data to the acquisition server

		m_pCallback->setSamples(m_pSample, m_ui32SampleCountPerSentBlock);
		m_pCallback->setStimulationSet(l_oStimulationSet);
		m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
	}

	return true;
}

boolean CDriverBrainProductsBrainampSeries::stop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

	// -- Stops acquisition

	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_STOP\n";
	if (!::DeviceIoControl(m_pDevice, BRAINAMP_STOP, NULL, 0, NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not stop acquisition\n";
	}

	// -- Optionnaly starts impedance check

	if(m_rDriverContext.isImpedanceCheckRequested())
	{
		if(!this->startImpedanceCheck())
		{
			return false;
		}
	}

	return true;
}

boolean CDriverBrainProductsBrainampSeries::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	// -- Optionnaly stops impedance check

	if(m_rDriverContext.isImpedanceCheckRequested())
	{
		if(!this->stopImpedanceCheck())
		{
			return false;
		}
	}

	// -- Deletes device setup object

	delete m_pDeviceSetup;
	m_pDeviceSetup=NULL;

	// -- Closes handle to device

	::CloseHandle(m_pDevice);
	m_pDevice=NULL;

	// -- Deletes sample array and intermediate buffer

	delete [] m_pSample;
	delete [] m_pBuffer;
	m_pSample=NULL;
	m_pBuffer=NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverBrainProductsBrainampSeries::startImpedanceCheck(void)
{
	if(!m_rDriverContext.isImpedanceCheckRequested()) return true;

	// Configures impedance frequency

	m_ui32ImpedanceCheckSignalFrequency=5000/(m_ui32SampleCountPerSentBlock*m_ui32DecimationFactor);
	m_rDriverContext.getLogManager() << LogLevel_Trace << "Imepdance check sampling frequency set to " << m_ui32ImpedanceCheckSignalFrequency << " Hz\n";

	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_IMPEDANCE_FREQUENCY\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_IMPEDANCE_FREQUENCY, &m_ui32ImpedanceCheckSignalFrequency, sizeof(uint32), NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not set impedance frequency - got error " << uint32(::GetLastError()) << "\n";
		return false;
	}

	// Starts acquiring impedance signal

	uint32 l_ui32AcquisitionType=uint32(AcquisitionType_Impedance);
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_START\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_START, &l_ui32AcquisitionType, sizeof(uint32), NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not start acquisition\n";
		return false;
	}

	// Configures impedance group range

	uint32 l_ui32ImpedanceGroup=(m_eImpedance==Impedance_High?0:1);
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_IMPEDANCE_GROUPRANGE\n";
	if(!::DeviceIoControl(m_pDevice, BRAINAMP_IMPEDANCE_GROUPRANGE, &l_ui32ImpedanceGroup, sizeof(uint32), NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could set impedance group range\n";
		return false;
	}

	// Prepares impedance buffers

	m_vImpedance.resize(m_oHeaderAdapter.getChannelCount());
	m_vImpedanceBuffer.clear();

	return true;
}

boolean CDriverBrainProductsBrainampSeries::stopImpedanceCheck(void)
{
	if(!m_rDriverContext.isImpedanceCheckRequested()) return true;

	// Stops impedance acquisition

	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_STOP\n";
	if (!::DeviceIoControl(m_pDevice, BRAINAMP_STOP, NULL, 0, NULL, 0, &m_ui32BytesReturned, NULL))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not stop acquisition\n";
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverBrainProductsBrainampSeries::isConfigurable(void)
{
	return true;
}

boolean CDriverBrainProductsBrainampSeries::configure(void)
{
	CConfigurationBrainProductsBrainampSeries l_oConfiguration(
		*this,
		"../share/openvibe-applications/acquisition-server/interface-BrainProducts-BrainampSeries.ui",
		m_ui32USBIndex,
		m_ui32DecimationFactor,
		m_peChannelSelected,
		m_peLowPassFilterFull,
		m_peResolutionFull,
		m_peDCCouplingFull,
		m_eLowPass,
		m_eResolution,
		m_eDCCoupling,
		m_eImpedance);
	l_oConfiguration.configure(m_oHeader);
	m_oHeader.setSamplingFrequency(5000/m_ui32DecimationFactor);
	return true;
}

boolean CDriverBrainProductsBrainampSeries::getDeviceDetails(const uint32 ui32Index, uint32* pAmplifierCount, uint32* pAmplifierType)
{
	uint32 i;

	char l_sDeviceName[1024];
	::sprintf(l_sDeviceName, "\\\\.\\BrainAmpUSB%i", ui32Index);

	// -- Opens device

	void* l_pDevice=::CreateFile(l_sDeviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
	if(l_pDevice==INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// -- Gets amplifiers type

	uint16 l_pAmplifierType[BrainAmp_MaximumAmplifierCount];
	m_rDriverContext.getLogManager() << LogLevel_TraceAPI << "BRAINAMP_AMPLIFIER_TYPE\n";
	if(!::DeviceIoControl(l_pDevice, BRAINAMP_AMPLIFIER_TYPE, NULL, 0, l_pAmplifierType, sizeof(l_pAmplifierType), &m_ui32BytesReturned, NULL))
	{
		::CloseHandle(l_pDevice);
		return false;
	}

	// -- Closes device

	::CloseHandle(l_pDevice);

	// -- Formats result

	uint32 l_ui32AmplifierCount=BrainAmp_MaximumAmplifierCount;
	for(i=0; i<BrainAmp_MaximumAmplifierCount; i++)
	{
		if(pAmplifierType)
		{
			pAmplifierType[i]=l_pAmplifierType[i];
		}
		if(l_ui32AmplifierCount==BrainAmp_MaximumAmplifierCount && l_pAmplifierType[i]==AmplifierType_None)
		{
			l_ui32AmplifierCount=i;
		}
	}
	if(pAmplifierCount)
	{
		*pAmplifierCount=l_ui32AmplifierCount;
	}

	return true;
}

#endif // OVAS_OS_Windows
