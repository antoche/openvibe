#ifndef __OpenViBEPlugins_VRPN_CVRPNAnalogServer_H__
#define __OpenViBEPlugins_VRPN_CVRPNAnalogServer_H__

#if defined TARGET_HAS_ThirdPartyVRPN

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <cstdio>
#include <map>

namespace OpenViBEPlugins
{
	namespace VRPN
	{
		class CVRPNAnalogServer : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			CVRPNAnalogServer();
			virtual void release(void) { delete this; }
			virtual OpenViBE::uint64 getClockFrequency(void) { return 64LL<<32; }
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_VRPNAnalogServer)

		protected:

			OpenViBE::CIdentifier m_oServerIdentifier;
			OpenViBE::boolean m_bAnalogSet;

			std::map < OpenViBE::uint32, OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamDecoder;
			std::map < OpenViBE::uint32, OpenViBE::uint32 > m_vAnalogCount;
		};

		class CVRPNAnalogServerListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input %u", i+1);
					rBox.setInputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				return this->check(rBox);
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				return this->check(rBox);
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CVRPNAnalogServerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Analog VRPN Server"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier/Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Creates VRPN analog servers (one per input)."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Creates VRPN analog servers to make data from the plugin's inputs available to VRPN client applications."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("VRPN"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-connect"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_VRPNAnalogServer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::VRPN::CVRPNAnalogServer(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CVRPNAnalogServerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input 1",         OV_TypeId_StreamedMatrix);
				rPrototype.addSetting("Peripheral name", OV_TypeId_String,        "openvibe-vrpn");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_VRPNAnalogServerDesc)
		};
	};
};

#endif // OVP_HAS_Vrpn

#endif // __OpenViBEPlugins_VRPN_CVRPNAnalogServer_H__
