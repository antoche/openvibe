#ifndef __OpenViBEDesigner_defines_H__
#define __OpenViBEDesigner_defines_H__

#define OVD_AttributeId_XBoxCenterPosition                  OpenViBE::CIdentifier(0x207C9054, 0x3C841B63)
#define OVD_AttributeId_YBoxCenterPosition                  OpenViBE::CIdentifier(0x1FA7A38F, 0x54EDBE0B)
#define OVD_AttributeId_XLinkSourcePosition                 OpenViBE::CIdentifier(0x358AE8B5, 0x0F8BACD1)
#define OVD_AttributeId_YLinkSourcePosition                 OpenViBE::CIdentifier(0x1B32C44C, 0x1905E0E9)
#define OVD_AttributeId_XLinkTargetPosition                 OpenViBE::CIdentifier(0x6267B5C5, 0x676E3E42)
#define OVD_AttributeId_YLinkTargetPosition                 OpenViBE::CIdentifier(0x3F0A3B27, 0x570913D2)

#define OVD_AttributeId_SettingOverrideFilename             OpenViBE::CIdentifier(0x8D21FF41, 0xDF6AFE7E)

//Attributes of visualisation window :
#define OVD_AttributeId_VisualisationWindow_Width           OpenViBE::CIdentifier(0x7B814CCA, 0x271DF6DD)
#define OVD_AttributeId_VisualisationWindow_Height          OpenViBE::CIdentifier(0x4C90D4AD, 0x7A2554EC)

//Attributes of visualisation paned :
//boolean : isHorizontal
//int : divider position
//int : max divider position
#define OVD_AttributeId_VisualisationWidget_IsPanedHorizontal       OpenViBE::CIdentifier(0x2F4B198E, 0x35B15432)
#define OVD_AttributeId_VisualisationWidget_DividerPosition         OpenViBE::CIdentifier(0x54E45F5B, 0x76C036E2)
#define OVD_AttributeId_VisualisationWidget_MaxDividerPosition      OpenViBE::CIdentifier(0x237E56D2, 0x10CD68AE)

namespace OpenViBEDesigner
{
	enum
	{
		Resource_StringName,
		Resource_StringShortDescription,
		Resource_StringIdentifier,
		Resource_StringStockIcon,
		Resource_BooleanIsPlugin,
	};

	enum
	{
		Color_BackgroundPlayerStarted,
		Color_BoxBackground,
		Color_BoxBackgroundSelected,
		Color_BoxBackgroundMissing,
		Color_BoxBackgroundObsolete,
		Color_BoxBorder,
		Color_BoxBorderSelected,
		Color_BoxInputBackground,
		Color_BoxInputBorder,
		Color_BoxOutputBackground,
		Color_BoxOutputBorder,
		Color_BoxSettingBackground,
		Color_BoxSettingBorder,
		Color_Link,
		Color_LinkSelected,
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
