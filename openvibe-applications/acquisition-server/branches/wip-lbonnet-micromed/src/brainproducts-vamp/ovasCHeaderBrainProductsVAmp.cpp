#include "ovasCHeaderBrainProductsVAmp.h"
#include "ovasCConfigurationBrainProductsVAmp.h"
#include "../ovasCHeader.h"

#include <system/Memory.h>

#if defined TARGET_HAS_ThirdPartyUSBFirstAmpAPI

#include <windows.h>
#include <FirstAmp.h>

#include <map>
#include <string>

#define _NoValueI_ 0xffffffff
#define boolean OpenViBE::boolean

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

namespace
{
	uint32 g_vEEGChannelCount[] = { 16, 8, 4 };
	uint32 g_vAuxiliaryChannelCount[] = { 2, 2, 0 };
	uint32 g_vTriggerChannelCount[] = { 1, 1, 1 };
};

//___________________________________________________________________//
//                                                                   //

CHeaderBrainProductsVAmp::CHeaderBrainProductsVAmp(boolean bAcquireAuxiliaryAsEEG, boolean bAcquireTriggerAsEEG)
{
	System::Memory::set(g_vAuxiliaryChannelCount, sizeof(g_vAuxiliaryChannelCount)*(bAcquireAuxiliaryAsEEG?0:1), 0);
	System::Memory::set(g_vTriggerChannelCount, sizeof(g_vTriggerChannelCount)*(bAcquireTriggerAsEEG?0:1), 0);

	m_pBasicHeader = new CHeader();

	// additional information
	m_i32DeviceId = -1;
	m_ui32AcquisitionMode = AcquisitionMode_VAmp16;

	// Pair information
	m_ui32PairCount = 0;
}

CHeaderBrainProductsVAmp::~CHeaderBrainProductsVAmp(void)
{
	delete m_pBasicHeader;
}

void CHeaderBrainProductsVAmp::reset(void)
{
	m_pBasicHeader->reset();
	m_i32DeviceId = FA_ID_INVALID;

	// Pair information
	m_ui32PairCount = 0;
}

//___________________________________________________________________//
//                                                                   //

uint32 CHeaderBrainProductsVAmp::getEEGChannelCount(uint32 ui32AcquisitionMode)
{
	return g_vEEGChannelCount[ui32AcquisitionMode];
}

uint32 CHeaderBrainProductsVAmp::getAuxiliaryChannelCount(uint32 ui32AcquisitionMode)
{
	return g_vAuxiliaryChannelCount[ui32AcquisitionMode];
}

uint32 CHeaderBrainProductsVAmp::getTriggerChannelCount(uint32 ui32AcquisitionMode)
{
	return g_vTriggerChannelCount[ui32AcquisitionMode];
}

// Pair information

boolean CHeaderBrainProductsVAmp::setPairCount(const uint32 ui32PairCount)
{
	m_ui32PairCount=ui32PairCount;
	m_vPairName.clear();
	m_vPairGain.clear();
	return m_ui32PairCount!=_NoValueI_;
}

boolean CHeaderBrainProductsVAmp::setPairName(const uint32 ui32PairIndex, const char* sPairName)
{
	m_vPairName[ui32PairIndex]=sPairName;
	return ui32PairIndex<m_ui32PairCount;
}

boolean CHeaderBrainProductsVAmp::setPairGain(const uint32 ui32PairIndex, const float32 f32PairGain)
{
	m_vPairGain[ui32PairIndex]=f32PairGain;
	return ui32PairIndex<m_ui32PairCount;
}

boolean CHeaderBrainProductsVAmp::setFastModeSettings(t_faDataModeSettings tFastModeSettings)
{
	m_tFastModeSettings=tFastModeSettings;
	return isFastModeSettingsSet();
}

uint32 CHeaderBrainProductsVAmp::getPairCount(void) const
{
	return m_ui32PairCount;
}

const char* CHeaderBrainProductsVAmp::getPairName(const uint32 ui32PairIndex) const
{
	map<uint32, string>::const_iterator i=m_vPairName.find(ui32PairIndex);
	if(i==m_vPairName.end())
	{
		return "";
	}
	return i->second.c_str();
}

float32 CHeaderBrainProductsVAmp::getPairGain(const uint32 ui32PairIndex) const
{
	map<uint32, float32>::const_iterator i=m_vPairGain.find(ui32PairIndex);
	if(i==m_vPairGain.end())
	{
		return(ui32PairIndex<m_ui32PairCount?1.0f:0.0f);
	}
	return i->second;
}

t_faDataModeSettings CHeaderBrainProductsVAmp::getFastModeSettings(void) const
{
	return m_tFastModeSettings;
}

boolean CHeaderBrainProductsVAmp::isPairCountSet(void) const
{
	return m_ui32PairCount!=_NoValueI_ && m_ui32PairCount!=0;
}

boolean CHeaderBrainProductsVAmp::isPairNameSet(void) const
{
	return isPairCountSet();
}

boolean CHeaderBrainProductsVAmp::isPairGainSet(void) const
{
	return isPairCountSet();
}

//___________________________________________________________________//
//                                                                   //

// Experimentation information
boolean CHeaderBrainProductsVAmp::setExperimentIdentifier(const uint32 ui32ExperimentIdentifier)
{
	return m_pBasicHeader->setExperimentIdentifier(ui32ExperimentIdentifier);
}

boolean CHeaderBrainProductsVAmp::setSubjectAge(const uint32 ui32SubjectAge)
{
	return m_pBasicHeader->setSubjectAge(ui32SubjectAge);
}

boolean CHeaderBrainProductsVAmp::setSubjectGender(const uint32 ui32SubjectGender)
{
	return m_pBasicHeader->setSubjectGender(ui32SubjectGender);
}

uint32 CHeaderBrainProductsVAmp::getExperimentIdentifier(void) const
{
	return m_pBasicHeader->getExperimentIdentifier();
}

uint32 CHeaderBrainProductsVAmp::getSubjectAge(void) const
{
	return m_pBasicHeader->getSubjectAge();
}

uint32 CHeaderBrainProductsVAmp::getSubjectGender(void) const
{
	return m_pBasicHeader->getSubjectGender();
}

boolean CHeaderBrainProductsVAmp::isExperimentIdentifierSet(void) const
{
	return m_pBasicHeader->isExperimentIdentifierSet();
}

boolean CHeaderBrainProductsVAmp::isSubjectAgeSet(void) const
{
	return m_pBasicHeader->isSubjectAgeSet();
}

boolean CHeaderBrainProductsVAmp::isSubjectGenderSet(void) const
{
	return m_pBasicHeader->isSubjectGenderSet();
}

//___________________________________________________________________//
//                                                                   //

boolean CHeaderBrainProductsVAmp::setDeviceId(int32 i32DeviceId)
{
	m_i32DeviceId=i32DeviceId;
	return m_i32DeviceId!=_NoValueI_;
}

int32 CHeaderBrainProductsVAmp::getDeviceId(void) const
{
	return m_i32DeviceId;
}

boolean CHeaderBrainProductsVAmp::isDeviceIdSet(void) const
{
	return m_i32DeviceId != _NoValueI_;
}

boolean CHeaderBrainProductsVAmp::isFastModeSettingsSet(void) const
{
	return (m_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[0] == _NoValueI_
	|| m_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[0] == _NoValueI_
	|| m_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[1] == _NoValueI_
	|| m_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[1] == _NoValueI_
	|| m_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[2] == _NoValueI_
	|| m_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[2] == _NoValueI_
	|| m_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[3] == _NoValueI_
	|| m_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[3] == _NoValueI_);
}

//___________________________________________________________________//
//                                                                   //

// Channel information

boolean CHeaderBrainProductsVAmp::setChannelCount(const uint32 ui32ChannelCount)
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->setPairCount(ui32ChannelCount);
	}
	else
	{
		return m_pBasicHeader->setChannelCount(ui32ChannelCount);
	}
}

boolean CHeaderBrainProductsVAmp::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->setPairName(ui32ChannelIndex, sChannelName);
	}
	else
	{
		return m_pBasicHeader->setChannelName(ui32ChannelIndex, sChannelName);
	}

}

boolean CHeaderBrainProductsVAmp::setChannelGain(const uint32 ui32ChannelIndex, const float32 f32ChannelGain)
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->setPairGain(ui32ChannelIndex,f32ChannelGain);
	}
	else
	{
		return m_pBasicHeader->setChannelGain(ui32ChannelIndex,f32ChannelGain);
	}
}

uint32 CHeaderBrainProductsVAmp::getChannelCount(void) const
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->getPairCount();
	}
	else
	{
		return m_pBasicHeader->getChannelCount();
	}
}

const char* CHeaderBrainProductsVAmp::getChannelName(const uint32 ui32ChannelIndex) const
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->getPairName(ui32ChannelIndex);
	}
	else
	{
		return m_pBasicHeader->getChannelName(ui32ChannelIndex);
	}
}

float32 CHeaderBrainProductsVAmp::getChannelGain(const uint32 ui32ChannelIndex) const
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->getPairGain(ui32ChannelIndex);
	}
	else
	{
		return m_pBasicHeader->getChannelGain(ui32ChannelIndex);
	}
}

boolean CHeaderBrainProductsVAmp::isChannelCountSet(void) const
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->isPairCountSet();
	}
	else
	{
		return m_pBasicHeader->isChannelCountSet();
	}
}

boolean CHeaderBrainProductsVAmp::isChannelNameSet(void) const
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->isPairNameSet();
	}
	else
	{
		return m_pBasicHeader->isChannelNameSet();
	}
}

boolean CHeaderBrainProductsVAmp::isChannelGainSet(void) const
{
	if(m_ui32AcquisitionMode == AcquisitionMode_VAmp4Fast)
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->isPairGainSet();
	}
	else
	{
		return m_pBasicHeader->isChannelGainSet();
	}
}

//___________________________________________________________________//
//                                                                   //

// Samples information
boolean CHeaderBrainProductsVAmp::setSamplingFrequency(const uint32 ui32SamplingFrequency)
{
	return m_pBasicHeader->setSamplingFrequency(ui32SamplingFrequency);
}

uint32 CHeaderBrainProductsVAmp::getSamplingFrequency(void) const
{
	return m_pBasicHeader->getSamplingFrequency();
}

boolean CHeaderBrainProductsVAmp::isSamplingFrequencySet(void) const
{
	return m_pBasicHeader->isSamplingFrequencySet();
}

#endif // TARGET_HAS_ThirdPartyUSBFirstAmpAPI
