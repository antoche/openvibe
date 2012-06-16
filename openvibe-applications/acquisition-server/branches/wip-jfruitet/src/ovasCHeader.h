#ifndef __OpenViBEAcquisitionServer_CHeader_H__
#define __OpenViBEAcquisitionServer_CHeader_H__

#include "ovasIHeader.h"

namespace OpenViBEAcquisitionServer
{
	class CHeader : public OpenViBEAcquisitionServer::IHeader
	{
	public:

		CHeader(void);
		virtual ~CHeader(void);
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

		OpenViBEAcquisitionServer::IHeader* m_pHeaderImpl;
	};
};

#endif // __OpenViBEAcquisitionServer_CHeader_H__
