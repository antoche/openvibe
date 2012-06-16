#include "ovasCHeaderBrainProductsVAmp.h"
#include "../ovasCHeader.h"

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

//___________________________________________________________________//
//                                                                   //

CHeaderBrainProductsVAmp::CHeaderBrainProductsVAmp(void)
{
	m_pBasicHeader = new CHeader();

	// additional information
	m_i32DeviceId = -1;
	m_tDataMode = dmNormal;

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
	m_tDataMode = dmNormal;

	// Pair information
	m_ui32PairCount = 0;
}

//___________________________________________________________________//
//                                                                   //

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
boolean CHeaderBrainProductsVAmp::setDataMode(t_faDataMode tDataMode)
{
	m_tDataMode=tDataMode;
	return (m_tDataMode != dmNormal && m_tDataMode != dm20kHz4Channels);
}

boolean CHeaderBrainProductsVAmp::setDeviceId(int32 i32DeviceId)
{
	m_i32DeviceId=i32DeviceId;
	return m_i32DeviceId!=_NoValueI_;
}

boolean CHeaderBrainProductsVAmp::setFastModeSettings(t_faDataModeSettings tFastModeSettings)
{
	m_tFastModeSettings=tFastModeSettings;

	return isFastModeSettingsSet();

}

t_faDataMode CHeaderBrainProductsVAmp::getDataMode(void) const
{
	return m_tDataMode;
}

int32 CHeaderBrainProductsVAmp::getDeviceId(void) const
{
	return m_i32DeviceId;
}

t_faDataModeSettings CHeaderBrainProductsVAmp::getFastModeSettings(void) const
{
	return m_tFastModeSettings;
}

boolean CHeaderBrainProductsVAmp::isDataModeSet(void) const
{
	return (m_tDataMode != dmNormal && m_tDataMode != dm20kHz4Channels);
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
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->setChannelCount(ui32ChannelCount);
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->setPairCount(ui32ChannelCount);
	}
}

boolean CHeaderBrainProductsVAmp::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->setChannelName(ui32ChannelIndex, sChannelName);
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->setPairName(ui32ChannelIndex, sChannelName);
	}

}

boolean CHeaderBrainProductsVAmp::setChannelGain(const uint32 ui32ChannelIndex, const float32 f32ChannelGain)
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->setChannelGain(ui32ChannelIndex,f32ChannelGain);
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->setPairGain(ui32ChannelIndex,f32ChannelGain);
	}
}

uint32 CHeaderBrainProductsVAmp::getChannelCount(void) const
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->getChannelCount();
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->getPairCount();
	}
}

const char* CHeaderBrainProductsVAmp::getChannelName(const uint32 ui32ChannelIndex) const
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->getChannelName(ui32ChannelIndex);
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->getPairName(ui32ChannelIndex);
	}
}

float32 CHeaderBrainProductsVAmp::getChannelGain(const uint32 ui32ChannelIndex) const
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->getChannelGain(ui32ChannelIndex);
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->getPairGain(ui32ChannelIndex);
	}
}

boolean CHeaderBrainProductsVAmp::isChannelCountSet(void) const
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->isChannelCountSet();
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->isPairCountSet();
	}
}

boolean CHeaderBrainProductsVAmp::isChannelNameSet(void) const
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->isChannelNameSet();
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->isPairNameSet();
	}
}

boolean CHeaderBrainProductsVAmp::isChannelGainSet(void) const
{
	if(m_tDataMode == dmNormal)
	{
		return m_pBasicHeader->isChannelGainSet();
	}
	else
	{
		// in fast mode the channel count is the pair count (to display in the designer as a "channel")
		return this->isPairGainSet();
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
