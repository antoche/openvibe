#ifndef __OpenViBEDesigner_defines_H__
#define __OpenViBEDesigner_defines_H__

//Attributes of visualisation window :
#define OVD_AttributeId_VisualisationWindow_Width           OpenViBE::CIdentifier(0x7B814CCA, 0x271DF6DD)
#define OVD_AttributeId_VisualisationWindow_Height          OpenViBE::CIdentifier(0x4C90D4AD, 0x7A2554EC)

//Attributes of visualisation paned :
#define OVD_AttributeId_VisualisationWidget_DividerPosition         OpenViBE::CIdentifier(0x54E45F5B, 0x76C036E2)
#define OVD_AttributeId_VisualisationWidget_MaxDividerPosition      OpenViBE::CIdentifier(0x237E56D2, 0x10CD68AE)

namespace OpenViBEDesigner
{

	typedef enum
	{
		CommandLineFlag_None             =0x00000000,
		CommandLineFlag_Open             =0x00000001,
		CommandLineFlag_Play             =0x00000002,
		CommandLineFlag_PlayFast         =0x00000004,
		CommandLineFlag_NoGui            =0x00000008,
		CommandLineFlag_NoCheckColorDepth=0x00000010,
		CommandLineFlag_NoManageSession  =0x00000020,
		CommandLineFlag_Define           =0x00000040,
	} ECommandLineFlag;

	enum
	{
		ContextMenu_SelectionCopy,
		ContextMenu_SelectionCut,
		ContextMenu_SelectionPaste,
		ContextMenu_SelectionDelete,

		ContextMenu_BoxRename,
		ContextMenu_BoxDelete,
		ContextMenu_BoxAddInput,
		ContextMenu_BoxEditInput,
		ContextMenu_BoxRemoveInput,
		ContextMenu_BoxAddOutput,
		ContextMenu_BoxEditOutput,
		ContextMenu_BoxRemoveOutput,
		ContextMenu_BoxAddSetting,
		ContextMenu_BoxRemoveSetting,
		ContextMenu_BoxEditSetting,
		ContextMenu_BoxConfigure,
		ContextMenu_BoxAbout,

		ContextMenu_ScenarioAbout,
	};

	enum
	{
		Resource_StringName,
		Resource_StringShortDescription,
		Resource_StringIdentifier,
		Resource_StringStockIcon,
		Resource_StringColor,
		Resource_BooleanIsPlugin,
		Resource_BooleanIsUnstable,
	};

	enum
	{
		Color_BackgroundPlayerStarted,
		Color_BoxBackground,
		Color_BoxBackgroundSelected,
		Color_BoxBackgroundMissing,
		Color_BoxBackgroundDeprecated,
		Color_BoxBackgroundUnstable,
		Color_BoxBackgroundNeedsUpdate,
		Color_BoxBorder,
		Color_BoxBorderSelected,
		Color_BoxInputBackground,
		Color_BoxInputBorder,
		Color_BoxOutputBackground,
		Color_BoxOutputBorder,
		Color_BoxSettingBackground,
		Color_BoxSettingBorder,
		Color_CommentBackground,
		Color_CommentBackgroundSelected,
		Color_CommentBorder,
		Color_CommentBorderSelected,
		Color_Link,
		Color_LinkSelected,
		Color_LinkDownCast,
		Color_LinkUpCast,
		Color_LinkInvalid,
		Color_SelectionArea,
		Color_SelectionAreaBorder,
	};

	enum
	{
		Connector_None,
		Connector_Input,
		Connector_Output,
		Connector_Setting,
		Connector_Link,
	};

	enum
	{
		Mode_None,
		Mode_Selection,
		Mode_SelectionAdd,
		Mode_MoveScenario,
		Mode_MoveSelection,
		Mode_Connect,
		Mode_EditSettings,
	};
};

//___________________________________________________________________//
//                                                                   //
// Gloabal defines                                                   //
//___________________________________________________________________//
//                                                                   //

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
 #include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

// #define OVD_OS_Linux
// #define OVD_OS_Windows
// #define OVD_OS_MacOS
// #define OVD_OS_

#if defined TARGET_OS_Windows
 #define OVD_OS_Windows
#elif defined TARGET_OS_Linux
 #define OVD_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OVD_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define OVD_ARCHITECTURE_i386
// #define OVD_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define OVD_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define OVD_COMPILATOR_GCC
// #define OVD_COMPILATOR_VisualStudio
// #define OVD_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define OVD_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define OVD_COMPILATOR_VisualStudio
#else
 #warning "No target compilator defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// API Definition                                                    //
//___________________________________________________________________//
//                                                                   //

// Taken from
// - http://people.redhat.com/drepper/dsohowto.pdf
// - http://www.nedprod.com/programs/gccvisibility.html
#if defined OVD_Shared
 #if defined OVD_OS_Windows
  #define OVD_API_Export __declspec(dllexport)
  #define OVD_API_Import __declspec(dllimport)
 #elif defined OVD_OS_Linux
  #define OVD_API_Export __attribute__((visibility("default")))
  #define OVD_API_Import __attribute__((visibility("default")))
 #else
  #define OVD_API_Export
  #define OVD_API_Import
 #endif
#else
 #define OVD_API_Export
 #define OVD_API_Import
#endif

#if defined OVD_Exports
 #define OVD_API OVD_API_Export
#else
 #define OVD_API OVD_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __OpenViBEDesigner_defines_H__
