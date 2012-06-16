#include "ovp_defines.h"

#include "ovpCXMLStimulationScenarioPlayer.h"
#include "ovpCKeyboardStimulator.h"
#include "ovpCSignChangeDetector.h"

#include "box-algorithms/stimulation/ovpCBoxAlgorithmPlayerController.h"

#include "box-algorithms/stimulation/ovpCBoxAlgorithmStimulationMultiplexer.h"
#include "box-algorithms/stimulation/ovpCBoxAlgorithmSoundPlayer.h"

#include "box-algorithms/stimulation/ovpCBoxAlgorithmRunCommand.h"
#if defined TARGET_HAS_ThirdPartyLua
 #include "box-algorithms/stimulation/ovpCBoxAlgorithmLuaStimulator.h"
#endif // TARGET_HAS_ThirdPartyLua
#if defined TARGET_HAS_ThirdPartyOpenAL
 #include "box-algorithms/stimulation/ovpCBoxAlgorithmOpenALSoundPlayer.h"
#endif // TARGET_HAS_ThirdPartyOpenAL

#include "box-algorithms/stimulation/adaptation/ovpCBoxAlgorithmStimulationFilter.h"

#include "ovpCBoxAlgorithmP300SpellerStimulator.h"
#include "ovpCBoxAlgorithmP300IdentifierStimulator.h"

OVP_Declare_Begin();
	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_StimulationFilterAction, "Stimulation Filter Action");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_StimulationFilterAction, "Select", OVP_TypeId_StimulationFilterAction_Select.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_StimulationFilterAction, "Reject", OVP_TypeId_StimulationFilterAction_Reject.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType(OV_TypeId_PlayerAction, "Player Action");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Play", OV_TypeId_PlayerAction_Play.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Stop", OV_TypeId_PlayerAction_Stop.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Pause", OV_TypeId_PlayerAction_Pause.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Forward", OV_TypeId_PlayerAction_Forward.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CXMLStimulationScenarioPlayerDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CKeyboardStimulatorDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CSignChangeDetectorDesc);

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmPlayerControllerDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmStimulationMultiplexerDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmSoundPlayerDesc);

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmRunCommandDesc);
#if defined TARGET_HAS_ThirdPartyLua
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmLuaStimulatorDesc);
#endif // TARGET_HAS_ThirdPartyLua

#if defined TARGET_HAS_ThirdPartyOpenAL
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmOpenALSoundPlayerDesc);
#endif // TARGET_HAS_ThirdPartyOpenAL

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmStimulationFilterDesc);

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmP300SpellerStimulatorDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmP300IdentifierStimulatorDesc);
OVP_Declare_End();
