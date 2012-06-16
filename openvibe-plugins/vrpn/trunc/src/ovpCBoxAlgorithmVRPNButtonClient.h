#ifndef __OpenViBEPlugins_BoxAlgorithm_VRPNButtonClient_H__
#define __OpenViBEPlugins_BoxAlgorithm_VRPNButtonClient_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <list>
#include <vrpn_Button.h>

#define OVP_ClassId_BoxAlgorithm_VRPNButtonClient     OpenViBE::CIdentifier(0x40714327, 0x458877D2)
#define OVP_ClassId_BoxAlgorithm_VRPNButtonClientDesc OpenViBE::CIdentifier(0x16FB6283, 0x45EC313F)

namespace OpenViBEPlugins
{
	namespace VRPN
	{
		class CBoxAlgorithmVRPNButtonClient : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_VRPNButtonClient);

			std::list < std::pair<OpenViBE::uint32, OpenViBE::boolean> > m_vButtonList;
			void setButton(OpenViBE::uint32 ui32ButtonIndex, OpenViBE::boolean bPressed);

		protected:

			OpenViBE::uint64 m_ui64LastChunkEndTime;
			OpenViBE::boolean m_bGotStimulation;
			std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamEncoder;
			std::vector < OpenViBE::IStimulationSet* > m_vStimulationSet;
			std::vector < OpenViBE::uint64 > m_vStimulationIdentifierOn;
			std::vector < OpenViBE::uint64 > m_vStimulationIdentifierOff;
			vrpn_Button_Remote* m_pVRPNButtonRemote;
		};

		class CBoxAlgorithmVRPNButtonClientListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sOutputName[1024];
				char l_sButtonOnName[1024];
				char l_sButtonOffName[1024];
				for(OpenViBE::uint32 i=0; i<rBox.getOutputCount(); i++)
				{
					::sprintf(l_sOutputName, "Output %i", i+1);
					::sprintf(l_sButtonOnName, "Button %i ON", i+1);
					::sprintf(l_sButtonOffName, "Button %i OFF", i+1);
					rBox.setOutputName(i, l_sOutputName);
					rBox.setSettingName(i*2+1, l_sButtonOnName);
					rBox.setSettingName(i*2+2, l_sButtonOffName);
				}
				return true;
			}
/*
			virtual OpenViBE::boolean initialize(void) { return true; }
			virtual OpenViBE::boolean uninitialize(void) { return true; }
			virtual OpenViBE::boolean onInitialized(OpenViBE::Kernel::IBox& rBox) { return true; };
			virtual OpenViBE::boolean onNameChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			virtual OpenViBE::boolean onIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			virtual OpenViBE::boolean onAlgorithmClassIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			virtual OpenViBE::boolean onProcessingUnitChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			virtual OpenViBE::boolean onInputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onInputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onOutputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onOutputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
*/
			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setOutputType(ui32Index, OV_TypeId_Stimulations);
				rBox.addSetting("",  OV_TypeId_Stimulation, "OVTK_GDF_Feedback_Continuous");
				rBox.addSetting("", OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Trial");
				return this->check(rBox);
			}

			virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeSetting(ui32Index*2+2);
				rBox.removeSetting(ui32Index*2+1);
				return this->check(rBox);
			}
/*
			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onOutputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onSettingNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
*/

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmVRPNButtonClientDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Button VRPN Client"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Connects to an external VRPN device and translate a button information into OpenViBE stimulations"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("-"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("VRPN"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-connect"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_VRPNButtonClient; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::VRPN::CBoxAlgorithmVRPNButtonClient; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmVRPNButtonClientListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				// rBoxAlgorithmPrototype.addInput  ("input name", /* input type (OV_TypeId_Signal) */);
				rBoxAlgorithmPrototype.addOutput ("Output", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Peripheral name", OV_TypeId_String, "openvibe-vrpn@localhost");
				rBoxAlgorithmPrototype.addSetting("Button 1 ON",     OV_TypeId_Stimulation, "OVTK_GDF_Feedback_Continuous");
				rBoxAlgorithmPrototype.addSetting("Button 1 OFF",    OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Trial");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddOutput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_VRPNButtonClientDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_VRPNButtonClient_H__
