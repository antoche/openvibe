#ifndef __OpenViBEPlugins_BoxAlgorithm_PlayerController_H__
#define __OpenViBEPlugins_BoxAlgorithm_PlayerController_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_PlayerController     OpenViBE::CIdentifier(0x5F426DCE, 0x08456E13)
#define OVP_ClassId_BoxAlgorithm_PlayerControllerDesc OpenViBE::CIdentifier(0x2E4142D7, 0x3F957E25)

#define OV_TypeId_PlayerAction                             OpenViBE::CIdentifier(0xCC14D8D6, 0xF27ECB73)
#define OV_TypeId_PlayerAction_Play                        OpenViBE::CIdentifier(0x6BD7E1B3, 0x85E75066)
#define OV_TypeId_PlayerAction_Stop                        OpenViBE::CIdentifier(0x8A72885E, 0xD537A9A4)
#define OV_TypeId_PlayerAction_Pause                       OpenViBE::CIdentifier(0xD2FA4573, 0xE7F1D11D)
#define OV_TypeId_PlayerAction_Forward                     OpenViBE::CIdentifier(0xE43E9888, 0x33A361ED)
/*
#define                         OpenViBE::CIdentifier(0x7B8BC511, 0x5EAF9CB0)
#define                         OpenViBE::CIdentifier(0x6B4AE217, 0x25860BBE)
#define                         OpenViBE::CIdentifier(0x49CA9B94, 0x49A815DF)
#define                         OpenViBE::CIdentifier(0x746A3C26, 0xE15258A8)
#define                         OpenViBE::CIdentifier(0xD4AF14F5, 0xBA955F21)
#define                         OpenViBE::CIdentifier(0x9D538FCD, 0x08C5092B)
#define                         OpenViBE::CIdentifier(0x71808EAC, 0x5991447A)
#define                         OpenViBE::CIdentifier(0xD41F4182, 0x15974FAB)
#define                         OpenViBE::CIdentifier(0x06947447, 0xEC5B71DE)
#define                         OpenViBE::CIdentifier(0x565E11E1, 0x7D9CA029)
#define                         OpenViBE::CIdentifier(0xAD75C745, 0xB60821E0)
*/

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmPlayerController : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_PlayerController);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSet;

			OpenViBE::uint64 m_ui64StimulationIdentifier;
			OpenViBE::uint64 m_ui64ActionIdentifier;
		};

		class CBoxAlgorithmPlayerControllerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Player Controller"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Controls the player execution"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Add some settings to configure the way you want to control the player"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_PlayerController; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmPlayerController; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Action to perform", OV_TypeId_PlayerAction, OV_TypeId_PlayerAction_Pause.toString());

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_PlayerControllerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_PlayerController_H__
