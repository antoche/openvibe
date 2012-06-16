#ifndef __OpenViBEPlugins_VRPN_CVRPNButtonServer_H__
#define __OpenViBEPlugins_VRPN_CVRPNButtonServer_H__

#if defined TARGET_HAS_ThirdPartyVRPN

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <map>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace VRPN
	{
		class CVRPNButtonServer
			:public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
			,public OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback::ICallback
		{
		public:

			CVRPNButtonServer(void);
			virtual void release(void) { delete this; }
			virtual OpenViBE::uint64 getClockFrequency(void) { return 64LL<<32; }
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_VRPNButtonServer)

			virtual void setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
			virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

		protected:

			//ebml
			EBML::IReader* m_pReader;
			OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback* m_pStimulationReaderCallBack;

			//Start and end time of the last buffer
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;

			OpenViBE::uint32 m_ui32CurrentInput;

			OpenViBE::CIdentifier m_oServerIdentifier;

			//Pairs of start/stop stimulations id
			std::map<OpenViBE::uint32, std::pair<OpenViBE::uint64, OpenViBE::uint64> > m_vStimulationPair;
		};

		class CVRPNButtonServerListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
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
					rBox.setInputType(i, OV_TypeId_Stimulations);
				}

				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Button %u ON", i+1);
					rBox.setSettingName(i*2+1, l_sName);
					rBox.setSettingType(i*2+1, OV_TypeId_Stimulation);

					sprintf(l_sName, "Button %u OFF", i+1);
					rBox.setSettingName(i*2+2, l_sName);
					rBox.setSettingType(i*2+2, OV_TypeId_Stimulation);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeSetting(ui32Index*2+1);
				rBox.removeSetting(ui32Index*2+1);

				return this->check(rBox);
			};

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.addSetting("", OV_TypeId_Stimulation, "OVTK_GDF_Feedback_Continuous");
				rBox.addSetting("", OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Trial");

				return this->check(rBox);
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CVRPNButtonServerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Button VRPN Server"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Creates VRPN button servers (one per input)."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Creates VRPN button servers to make data from the plugin's inputs available to VRPN client applications."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("VRPN"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-connect"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_VRPNButtonServer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::VRPN::CVRPNButtonServer(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CVRPNButtonServerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input 1",                    OVTK_TypeId_Stimulations);
				rPrototype.addSetting("Peripheral name",            OV_TypeId_String,      "openvibe-vrpn");
				rPrototype.addSetting("Button 1 ON",  OV_TypeId_Stimulation, "OVTK_GDF_Feedback_Continuous");
				rPrototype.addSetting("Button 1 OFF", OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Trial");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_VRPNButtonServerDesc)
		};
	};
};

#endif // OVP_HAS_Vrpn

#endif // __OpenViBEPlugins_VRPN_CVRPNButtonServer_H__
