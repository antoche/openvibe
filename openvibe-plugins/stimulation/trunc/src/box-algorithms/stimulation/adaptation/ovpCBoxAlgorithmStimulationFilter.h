#ifndef __OpenViBEPlugins_BoxAlgorithm_StimulationFilter_H__
#define __OpenViBEPlugins_BoxAlgorithm_StimulationFilter_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <vector>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_StimulationFilter     OpenViBE::CIdentifier(0x02F96101, 0x5E647CB8)
#define OVP_ClassId_BoxAlgorithm_StimulationFilterDesc OpenViBE::CIdentifier(0x4D2A23FC, 0x28191E18)
#define OVP_TypeId_StimulationFilterAction             OpenViBE::CIdentifier(0x09E59E57, 0x8D4A553A)
#define OVP_TypeId_StimulationFilterAction_Select      OpenViBE::CIdentifier(0xBDBBA98D, 0xC0477399)
#define OVP_TypeId_StimulationFilterAction_Reject      OpenViBE::CIdentifier(0xB7C594D2, 0x32474226)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStimulationFilter : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationFilter);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer;

			typedef struct
			{
				OpenViBE::uint64 ui64Action;
				OpenViBE::uint64 ui64StartStimulationId;
				OpenViBE::uint64 ui64EndStimulationId;
			} SRule;

			OpenViBE::uint64 m_ui64DefaultAction;
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;
			std::vector < OpenViBEPlugins::Stimulation::CBoxAlgorithmStimulationFilter::SRule > m_vRules;
		};

		class CBoxAlgorithmStimulationFilterListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{

		public:
			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeSetting(ui32Index);
				//we had a whole rule (3 settings)
				rBox.addSetting("Action to perform",       OVP_TypeId_StimulationFilterAction, "Select");
				rBox.addSetting("Stimulation range begin", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBox.addSetting("Stimulation range end",   OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0F");

				return true;
			}

			virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				//we must remove the 2 other settings corresponding to the rule
				OpenViBE::uint32 l_ui32SettingGroupIndex = (ui32Index-3) / 3;
				rBox.removeSetting(l_ui32SettingGroupIndex*3+3);
				rBox.removeSetting(l_ui32SettingGroupIndex*3+3);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmStimulationFilterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stimulation Filter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Filters incoming stimulations selecting or rejecting specific ranges of stimulations"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation/Adaptation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StimulationFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmStimulationFilter; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Stimulations",            OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Modified Stimulations",   OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Default action",          OVP_TypeId_StimulationFilterAction, "Reject");
				rBoxAlgorithmPrototype.addSetting("Time range begin",        OV_TypeId_Float, "0");
				rBoxAlgorithmPrototype.addSetting("Time range end",          OV_TypeId_Float, "0");
				rBoxAlgorithmPrototype.addSetting("Action to perform",       OVP_TypeId_StimulationFilterAction, "Select");
				rBoxAlgorithmPrototype.addSetting("Stimulation range begin", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Stimulation range end",   OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0F");

				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddSetting);
				// rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmStimulationFilterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationFilterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_StimulationFilter_H__
