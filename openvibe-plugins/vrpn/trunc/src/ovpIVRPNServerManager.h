#ifndef __OpenViBEPlugins_VRPN_IVRPNServerManager_H__
#define __OpenViBEPlugins_VRPN_IVRPNServerManager_H__

#if defined TARGET_HAS_ThirdPartyVRPN

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace VRPN
	{
		class IVRPNServerManager
		{
		public:

			static IVRPNServerManager& getInstance(void);

			virtual ~IVRPNServerManager(void) { }

			virtual OpenViBE::boolean initialize(void)=0;
			virtual OpenViBE::boolean uninitialize(void)=0;

			virtual OpenViBE::boolean process(void)=0;
			virtual OpenViBE::boolean reportAnalog(
				const OpenViBE::CIdentifier& rServerIdentifier)=0;
			virtual OpenViBE::boolean reportButton(
				const OpenViBE::CIdentifier& rServerIdentifier)=0;

			virtual OpenViBE::boolean addServer(
				const OpenViBE::CString& sServerName,
				OpenViBE::CIdentifier& rServerIdentifier)=0;
			virtual OpenViBE::boolean isServer(
				const OpenViBE::CIdentifier& rServerIdentifier) const=0;
			virtual OpenViBE::boolean isServer(
				const OpenViBE::CString& sServerName) const=0;
			virtual OpenViBE::boolean getServerIdentifier(
				const OpenViBE::CString& sServerName,
				OpenViBE::CIdentifier& rServerIdentifier) const=0;
			virtual OpenViBE::boolean getServerName(
				const OpenViBE::CIdentifier& rServerIdentifier,
				OpenViBE::CString& sServerName) const=0;
			virtual OpenViBE::boolean removeServer(
				const OpenViBE::CIdentifier& rServerIdentifier)=0;

			virtual OpenViBE::boolean setButtonCount(
				const OpenViBE::CIdentifier& rServerIdentifier,
				const OpenViBE::uint32 ui32ButtonCount)=0;
			virtual OpenViBE::boolean setButtonState(
				const OpenViBE::CIdentifier& rServerIdentifier,
				const OpenViBE::uint32 ui32ButtonIndex,
				const OpenViBE::boolean bButtonStatus)=0;
			virtual OpenViBE::boolean getButtonState(
				const OpenViBE::CIdentifier& rServerIdentifier,
				const OpenViBE::uint32 ui32ButtonIndex) const=0;

			virtual OpenViBE::boolean setAnalogCount(
				const OpenViBE::CIdentifier& rServerIdentifier,
				const OpenViBE::uint32 ui32AnalogCount)=0;
			virtual OpenViBE::boolean setAnalogState(
				const OpenViBE::CIdentifier& rServerIdentifier,
				const OpenViBE::uint32 ui32AnalogIndex,
				const OpenViBE::float64 f64AnalogStatus)=0;
			virtual OpenViBE::float64 getAnalogState(
				const OpenViBE::CIdentifier& rServerIdentifier,
				const OpenViBE::uint32 ui32AnalogIndex) const=0;
		};
	};
};

#endif // OVP_HAS_Vrpn

#endif // __OpenViBEPlugins_VRPN_IVRPNServerManager_H__
