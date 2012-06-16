#include "ovasCHeader.h"

#include <map>
#include <string>

#define _NoValueI_ 0xffffffff

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace std;

//___________________________________________________________________//
//                                                                   //

namespace OpenViBEAcquisitionServer
{
	namespace
	{
		class CHeaderImpl : public OpenViBEAcquisitionServer::IHeader
		{
		public:

			CHeaderImpl(void);
			virtual ~CHeaderImpl(void);
			virtual void reset(void);

			// Experimentation information
			virtual OpenViBE::boolean setExperimentIdentifier(const OpenViBE::uint32 ui32ExperimentIdentifier);
			virtual OpenViBE::boolean setSubjectAge(const OpenViBE::uint32 ui32SubjectAge);
			virtual OpenViBE::boolean setSubjectGender(const OpenViBE::uint32 ui32SubjectGender);

			virtual OpenViBE::uint32 getExperimentIdentifier(void) const;
			virtual OpenViBE::uint32 getSubjectAge(void) const;
			virtual OpenViBE::uint32 getSubjectGender(void) const;

			virtual OpenViBE::boolean isExperimentIdentifierSet(void) const;
			virtual OpenViBE::boolean isSubjectAgeSet(void) const;
			virtual OpenViBE::boolean isSubjectGenderSet(void) const;

			// Chanel information
			virtual OpenViBE::boolean setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual OpenViBE::boolean setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual OpenViBE::boolean setChannelGain(const OpenViBE::uint32 ui32ChannelIndex, const OpenViBE::float32 f32ChannelGain);
			// virtual OpenViBE::boolean setChannelLocation(const OpenViBE::uint32 ui32ChannelIndex, const OpenViBE::float32 ui32ChannelLocationX, const OpenViBE::float32 ui32ChannelLocationY, const OpenViBE::float32 ui32ChannelLocationZ);

			virtual OpenViBE::uint32 getChannelCount(void) const;
			virtual const char* getChannelName(const OpenViBE::uint32 ui32ChannelIndex) const;
			virtual OpenViBE::float32 getChannelGain(const OpenViBE::uint32 ui32ChannelIndex) const;
			// virtual getChannelLocation(const OpenViBE::uint32 ui32ChannelIndex) const;

			virtual OpenViBE::boolean isChannelCountSet(void) const;
			virtual OpenViBE::boolean isChannelNameSet(void) const;
			virtual OpenViBE::boolean isChannelGainSet(void) const;
			// virtual OpenViBE::boolean isChannelLocationSet(void) const;

			// Samples information
			virtual OpenViBE::boolean setSamplingFrequency(const OpenViBE::uint32 ui32SamplingFrequency);

			virtual OpenViBE::uint32 getSamplingFrequency(void) const;

			virtual OpenViBE::boolean isSamplingFrequencySet(void) const;

		protected:

			// Experimentation information
			OpenViBE::uint32 m_ui32ExperimentIdentifier;
			OpenViBE::uint32 m_ui32SubjectAge;
			OpenViBE::uint32 m_ui32SubjectGender;

			// Chanel information
			OpenViBE::uint32 m_ui32ChannelCount;
			std::map<OpenViBE::uint32, std::string> m_vChannelName;
			std::map<OpenViBE::uint32, OpenViBE::float32> m_vChannelGain;

			// Samples information
			OpenViBE::uint32 m_ui32SamplingFrequency;
		};
	};
};

//___________________________________________________________________//
//                                                                   //

CHeaderImpl::CHeaderImpl(void)
	:m_ui32ExperimentIdentifier(_NoValueI_)
	,m_ui32SubjectAge(_NoValueI_)
	,m_ui32SubjectGender(_NoValueI_)
	,m_ui32ChannelCount(_NoValueI_)
	,m_ui32SamplingFrequency(_NoValueI_)
{
}

CHeaderImpl::~CHeaderImpl(void)
{
}

void CHeaderImpl::reset(void)
{
	m_ui32ExperimentIdentifier=_NoValueI_;
	m_ui32SubjectAge=_NoValueI_;
	m_ui32SubjectGender=_NoValueI_;
	m_ui32ChannelCount=_NoValueI_;
	m_vChannelName.clear();
	m_vChannelGain.clear();
	m_ui32SamplingFrequency=_NoValueI_;
}

//___________________________________________________________________//
//                                                                   //

// Experimentation information
boolean CHeaderImpl::setExperimentIdentifier(const uint32 ui32ExperimentIdentifier)
{
	m_ui32ExperimentIdentifier=ui32ExperimentIdentifier;
	return m_ui32ExperimentIdentifier!=_NoValueI_;
}

boolean CHeaderImpl::setSubjectAge(const uint32 ui32SubjectAge)
{
	m_ui32SubjectAge=ui32SubjectAge;
	return m_ui32SubjectAge!=_NoValueI_;
}

boolean CHeaderImpl::setSubjectGender(const uint32 ui32SubjectGender)
{
	m_ui32SubjectGender=ui32SubjectGender;
	return m_ui32SubjectGender!=_NoValueI_;
}

uint32 CHeaderImpl::getExperimentIdentifier(void) const
{
	return m_ui32ExperimentIdentifier;
}

uint32 CHeaderImpl::getSubjectAge(void) const
{
	return m_ui32SubjectAge;
}

uint32 CHeaderImpl::getSubjectGender(void) const
{
	return m_ui32SubjectGender;
}

boolean CHeaderImpl::isExperimentIdentifierSet(void) const
{
	return m_ui32ExperimentIdentifier!=_NoValueI_;
}

boolean CHeaderImpl::isSubjectAgeSet(void) const
{
	return m_ui32SubjectAge!=_NoValueI_;
}

boolean CHeaderImpl::isSubjectGenderSet(void) const
{
	return m_ui32SubjectGender!=_NoValueI_;
}

//___________________________________________________________________//
//                                                                   //

// Chanel information

boolean CHeaderImpl::setChannelCount(const uint32 ui32ChannelCount)
{
	m_ui32ChannelCount=ui32ChannelCount;
	m_vChannelName.clear();
	m_vChannelGain.clear();
	return m_ui32ChannelCount!=_NoValueI_;
}

boolean CHeaderImpl::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_vChannelName[ui32ChannelIndex]=sChannelName;
	return ui32ChannelIndex<m_ui32ChannelCount;
}

boolean CHeaderImpl::setChannelGain(const uint32 ui32ChannelIndex, const float32 f32ChannelGain)
{
	m_vChannelGain[ui32ChannelIndex]=f32ChannelGain;
	return ui32ChannelIndex<m_ui32ChannelCount;
}

// boolean CHeaderImpl::setChannelLocation(const uint32 ui32ChannelIndex, const float32 ui32ChannelLocationX, const float32 ui32ChannelLocationY, const float32 ui32ChannelLocationZ);

uint32 CHeaderImpl::getChannelCount(void) const
{
	return m_ui32ChannelCount;
}

const char* CHeaderImpl::getChannelName(const uint32 ui32ChannelIndex) const
{
	map<uint32, string>::const_iterator i=m_vChannelName.find(ui32ChannelIndex);
	if(i==m_vChannelName.end())
	{
		return "";
	}
	return i->second.c_str();
}

float32 CHeaderImpl::getChannelGain(const uint32 ui32ChannelIndex) const
{
	map<uint32, float32>::const_iterator i=m_vChannelGain.find(ui32ChannelIndex);
	if(i==m_vChannelGain.end())
	{
		return(ui32ChannelIndex<m_ui32ChannelCount?1.0f:0.0f);
	}
	return i->second;
}

// CHeaderImpl::getChannelLocation(const uint32 ui32ChannelIndex) const;

boolean CHeaderImpl::isChannelCountSet(void) const
{
	return m_ui32ChannelCount!=_NoValueI_ && m_ui32ChannelCount!=0;
}

boolean CHeaderImpl::isChannelNameSet(void) const
{
	return isChannelCountSet();
}

boolean CHeaderImpl::isChannelGainSet(void) const
{
	return isChannelCountSet();
}

// boolean CHeaderImpl::isChannelLocationSet(void) const

//___________________________________________________________________//
//                                                                   //

// Samples information
boolean CHeaderImpl::setSamplingFrequency(const uint32 ui32SamplingFrequency)
{
	m_ui32SamplingFrequency=ui32SamplingFrequency;
	return m_ui32SamplingFrequency!=_NoValueI_;
}

uint32 CHeaderImpl::getSamplingFrequency(void) const
{
	return m_ui32SamplingFrequency;
}

boolean CHeaderImpl::isSamplingFrequencySet(void) const
{
	return m_ui32SamplingFrequency!=_NoValueI_;
}

//___________________________________________________________________//
//                                                                   //

CHeader::CHeader(void)
	:m_pHeaderImpl(NULL)
{
	m_pHeaderImpl=new CHeaderImpl();
}

CHeader::~CHeader(void)
{
	delete m_pHeaderImpl;
}

void CHeader::reset(void)
{
	m_pHeaderImpl->reset();
}

//___________________________________________________________________//
//                                                                   //

// Experimentation information
boolean CHeader::setExperimentIdentifier(const uint32 ui32ExperimentIdentifier)
{
	return m_pHeaderImpl->setExperimentIdentifier(ui32ExperimentIdentifier);
}

boolean CHeader::setSubjectAge(const uint32 ui32SubjectAge)
{
	return m_pHeaderImpl->setSubjectAge(ui32SubjectAge);
}

boolean CHeader::setSubjectGender(const uint32 ui32SubjectGender)
{
	return m_pHeaderImpl->setSubjectGender(ui32SubjectGender);
}

uint32 CHeader::getExperimentIdentifier(void) const
{
	return m_pHeaderImpl->getExperimentIdentifier();
}

uint32 CHeader::getSubjectAge(void) const
{
	return m_pHeaderImpl->getSubjectAge();
}

uint32 CHeader::getSubjectGender(void) const
{
	return m_pHeaderImpl->getSubjectGender();
}

boolean CHeader::isExperimentIdentifierSet(void) const
{
	return m_pHeaderImpl->isExperimentIdentifierSet();
}

boolean CHeader::isSubjectAgeSet(void) const
{
	return m_pHeaderImpl->isSubjectAgeSet();
}

boolean CHeader::isSubjectGenderSet(void) const
{
	return m_pHeaderImpl->isSubjectGenderSet();
}

//___________________________________________________________________//
//                                                                   //

// Chanel information

boolean CHeader::setChannelCount(const uint32 ui32ChannelCount)
{
	return m_pHeaderImpl->setChannelCount(ui32ChannelCount);
}

boolean CHeader::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	return m_pHeaderImpl->setChannelName(ui32ChannelIndex, sChannelName);
}

boolean CHeader::setChannelGain(const uint32 ui32ChannelIndex, const float32 f32ChannelGain)
{
	return m_pHeaderImpl->setChannelGain(ui32ChannelIndex, f32ChannelGain);
}

// boolean CHeader::setChannelLocation(const uint32 ui32ChannelIndex, const float32 ui32ChannelLocationX, const float32 ui32ChannelLocationY, const float32 ui32ChannelLocationZ);

uint32 CHeader::getChannelCount(void) const
{
	return m_pHeaderImpl->getChannelCount();
}

const char* CHeader::getChannelName(const uint32 ui32ChannelIndex) const
{
	return m_pHeaderImpl->getChannelName(ui32ChannelIndex);
}

float32 CHeader::getChannelGain(const uint32 ui32ChannelIndex) const
{
	return m_pHeaderImpl->getChannelGain(ui32ChannelIndex);
}

// CHeader::getChannelLocation(const uint32 ui32ChannelIndex) const;

boolean CHeader::isChannelCountSet(void) const
{
	return m_pHeaderImpl->isChannelCountSet();
}

boolean CHeader::isChannelNameSet(void) const
{
	return m_pHeaderImpl->isChannelNameSet();
}

boolean CHeader::isChannelGainSet(void) const
{
	return m_pHeaderImpl->isChannelGainSet();
}

// boolean CHeader::isChannelLocationSet(void) const

//___________________________________________________________________//
//                                                                   //

// Samples information
boolean CHeader::setSamplingFrequency(const uint32 ui32SamplingFrequency)
{
	return m_pHeaderImpl->setSamplingFrequency(ui32SamplingFrequency);
}

uint32 CHeader::getSamplingFrequency(void) const
{
	return m_pHeaderImpl->getSamplingFrequency();
}

boolean CHeader::isSamplingFrequencySet(void) const
{
	return m_pHeaderImpl->isSamplingFrequencySet();
}
