#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_BoxAlgorithm_StimulationMultiplexer       OpenViBE::CIdentifier(0x07DB4EFA, 0x472B0938)
#define OVP_ClassId_BoxAlgorithm_StimulationMultiplexerDesc   OpenViBE::CIdentifier(0x79EF4E4D, 0x178F09E6)

#define OVP_ClassId_BoxAlgorithm_SoundPlayer                  OpenViBE::CIdentifier(0x18D06E9F, 0x68D43C23)
#define OVP_ClassId_BoxAlgorithm_SoundPlayerDesc              OpenViBE::CIdentifier(0x246E5EC4, 0x127D21AA)

//___________________________________________________________________//
//                                                                   //
// Plugin Object Descriptor Class Identifiers                        //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_XMLStimulationScenarioPlayerDesc          OpenViBE::CIdentifier(0x00D846C8, 0x264AACC9)
#define OVP_ClassId_KeyboardStimulatorDesc                    OpenViBE::CIdentifier(0x00E51ACD, 0x284CA2CF)
#define OVP_ClassId_BoxAlgorithm_P300IdentifierStimulatorDesc OpenViBE::CIdentifier(0x00F20CA,0x2A4EA9C3)
#define OVP_ClassId_SignChangeDetectorDesc                    OpenViBE::CIdentifier(0x798ACD86, 0xEF1287A4)

//___________________________________________________________________//
//                                                                   //
// Plugin Object Class Identifiers                                   //
//___________________________________________________________________//
//

#define OVP_ClassId_XMLStimulationScenarioPlayer              OpenViBE::CIdentifier(0x00646136, 0x44853D76)
#define OVP_ClassId_KeyboardStimulator                        OpenViBE::CIdentifier(0x00D317B9, 0x6324C3FF)
#define OVP_ClassId_BoxAlgorithm_P300IdentifierStimulator     OpenViBE::CIdentifier(0x00F27FDB,0x8203D1A5)
#define OVP_ClassId_SignChangeDetector                        OpenViBE::CIdentifier(0x4FA78CD, 0xAFE45DE7)

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

// #define OVP_OS_Linux
// #define OVP_OS_Windows
// #define OVP_OS_MacOS
// #define OVP_OS_

#if defined TARGET_OS_Windows
 #define OVP_OS_Windows
#elif defined TARGET_OS_Linux
 #define OVP_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OVP_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define OVP_ARCHITECTURE_i386
// #define OVP_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define OVP_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define OVP_COMPILATOR_GCC
// #define OVP_COMPILATOR_VisualStudio
// #define OVP_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define OVP_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define OVP_COMPILATOR_VisualStudio
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
#if defined OVP_Shared
 #if defined OVP_OS_Windows
  #define OVP_API_Export __declspec(dllexport)
  #define OVP_API_Import __declspec(dllimport)
 #elif defined OVP_OS_Linux
  #define OVP_API_Export __attribute__((visibility("default")))
  #define OVP_API_Import __attribute__((visibility("default")))
 #else
  #define OVP_API_Export
  #define OVP_API_Import
 #endif
#else
 #define OVP_API_Export
 #define OVP_API_Import
#endif

#if defined OVP_Exports
 #define OVP_API OVP_API_Export
#else
 #define OVP_API OVP_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __OpenViBEPlugins_Defines_H__
