#ifndef __OpenViBEKernel_Defines_H__
#define __OpenViBEKernel_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Global Class Identifiers                                          //
//___________________________________________________________________//
//                                                                   //

#define OVK_ClassId_KernelDesc                                OpenViBE::CIdentifier(0x02F2D050, 0x25CD9EE5)

//___________________________________________________________________//
//                                                                   //
// Kernel Class Identifiers                                          //
//___________________________________________________________________//
//                                                                   //

#define OVK_ClassId_Kernel_KernelObjectT                      OpenViBE::CIdentifier(0x672AE465, 0x60CB46D8)
#define OVK_ClassId_Kernel_KernelObjectFactory                OpenViBE::CIdentifier(0x7D380DFA, 0x1B33AE2F)
#define OVK_ClassId_Kernel_KernelContext                      OpenViBE::CIdentifier(0x72D4050C, 0x543DDAD8)
#define OVK_ClassId_Kernel_TypeManager                        OpenViBE::CIdentifier(0x11D27788, 0x0602030A)

#define OVK_ClassId_Kernel_Algorithm_Algorithm                OpenViBE::CIdentifier(0x7C741F2F, 0x24314CFF)
#define OVK_ClassId_Kernel_Algorithm_AlgorithmContext         OpenViBE::CIdentifier(0xBE3E7F65, 0x2752D79E)
#define OVK_ClassId_Kernel_Algorithm_AlgorithmManager         OpenViBE::CIdentifier(0xA75B6AA8, 0xEE0A25A4)
#define OVK_ClassId_Kernel_Algorithm_AlgorithmProto           OpenViBE::CIdentifier(0x3C83B501, 0xD5992078)
#define OVK_ClassId_Kernel_Algorithm_AlgorithmProxy           OpenViBE::CIdentifier(0x25BBE1E2, 0x18275693)

#define OVK_ClassId_Kernel_Configuration_ConfigurationManager OpenViBE::CIdentifier(0xE483786B, 0xA464DD55)

#define OVK_ClassId_Kernel_Log_LogManager                     OpenViBE::CIdentifier(0x1FE5CF01, 0x0B2B3748)
#define OVK_ClassId_Kernel_Log_LogListenerConsole             OpenViBE::CIdentifier(0x1EEB056C, 0x373C17C7)
#define OVK_ClassId_Kernel_Log_LogListenerFile                OpenViBE::CIdentifier(0x6A7FCB0C, 0xC847976D)
#define OVK_ClassId_Kernel_Log_LogListenerDebug               OpenViBE::CIdentifier(0x45C1A72F, 0x647DE336)
#define OVK_ClassId_Kernel_Log_LogListenerNull                OpenViBE::CIdentifier(0x1341E7E6, 0x2232EB75)

#define OVK_ClassId_Kernel_Plugins_PluginManager              OpenViBE::CIdentifier(0x07DB32AD, 0x76FAD392)
#define OVK_ClassId_Kernel_Plugins_PluginModule               OpenViBE::CIdentifier(0x34883FDB, 0x78B6D1E6)
#define OVK_ClassId_Kernel_Plugins_PluginModuleContext        OpenViBE::CIdentifier(0x78071145, 0x308261A2)
#define OVK_ClassId_Kernel_Plugins_PluginObject               OpenViBE::CIdentifier(0x718F6C0B, 0x368B2E6C)
#define OVK_ClassId_Kernel_Plugins_PluginObjectDesc           OpenViBE::CIdentifier(0x65853ECE, 0x6E203E71)

#define OVK_ClassId_Kernel_Player_BoxAlgorithmContext         OpenViBE::CIdentifier(0x48078FB2, 0x1897AD5A)
#define OVK_ClassId_Kernel_Player_DynamicBoxContext           OpenViBE::CIdentifier(0x18FC58E0, 0x505E97CD)
#define OVK_ClassId_Kernel_Player_MessageT                    OpenViBE::CIdentifier(0x42A56E80, 0x289CBB09)
#define OVK_ClassId_Kernel_Player_MessageClock                OpenViBE::CIdentifier(0x3AA34472, 0x2B1F67CB)
#define OVK_ClassId_Kernel_Player_MessageEvent                OpenViBE::CIdentifier(0x036401EF, 0x31E90C38)
#define OVK_ClassId_Kernel_Player_MessageSignal               OpenViBE::CIdentifier(0x3BB1DDD9, 0x0B289F8D)
#define OVK_ClassId_Kernel_Player_Player                      OpenViBE::CIdentifier(0x7C8777FF, 0x52CE89C2)
#define OVK_ClassId_Kernel_Player_PlayerContext               OpenViBE::CIdentifier(0x76A544A0, 0x01F508ED)
#define OVK_ClassId_Kernel_Player_PlayerManager               OpenViBE::CIdentifier(0x4A46A96D, 0x6D4957C5)
#define OVK_ClassId_Kernel_Player_SimulatedBox                OpenViBE::CIdentifier(0x74D37E85, 0x73952B1D)
#define OVK_ClassId_Kernel_Player_StaticBoxContext            OpenViBE::CIdentifier(0x59B7AE07, 0x4A6FE704)
#define OVK_ClassId_Kernel_Player_Scheduler                   OpenViBE::CIdentifier(0x39A9093F, 0x363DDB65)

#define OVK_ClassId_Kernel_Scenario_AttributableT             OpenViBE::CIdentifier(0x6F4071A4, 0x31183474)
#define OVK_ClassId_Kernel_Scenario_Box                       OpenViBE::CIdentifier(0x715A25DC, 0x5322E94C)
#define OVK_ClassId_Kernel_Scenario_Comment                   OpenViBE::CIdentifier(0xFD134BE1, 0x32BEC860)
#define OVK_ClassId_Kernel_Scenario_BoxListenerContext        OpenViBE::CIdentifier(0x6EBE7BA6, 0xF8C13ACA)
#define OVK_ClassId_Kernel_Scenario_BoxProto                  OpenViBE::CIdentifier(0x290EDACD, 0x6D08F87D)
#define OVK_ClassId_Kernel_Scenario_Link                      OpenViBE::CIdentifier(0x118EF337, 0x095F5A5E)
#define OVK_ClassId_Kernel_Scenario_ProcessingUnit            OpenViBE::CIdentifier(0x3C581F28, 0x5A5E9C6B)
#define OVK_ClassId_Kernel_Scenario_Scenario                  OpenViBE::CIdentifier(0x4215A6BF, 0x6F4E5F96)
#define OVK_ClassId_Kernel_Scenario_ScenarioManager           OpenViBE::CIdentifier(0x5BB96551, 0x133303F9)
#define OVK_ClassId_Kernel_Scenario_ScenarioExporterContext   OpenViBE::CIdentifier(0x78606B83, 0x5EB11EC9)
#define OVK_ClassId_Kernel_Scenario_ScenarioImporterContext   OpenViBE::CIdentifier(0x17DC0F51, 0x795A3A7E)

#define OVK_ClassId_Kernel_ObjectVisitorContext               OpenViBE::CIdentifier(0x7F6010A3, 0x17F4F20A)

#define OVK_ClassId_Kernel_Visualisation_VisualisationContext OpenViBE::CIdentifier(0xC364264A, 0x6CFD8F12)
#define OVK_ClassId_Kernel_Visualisation_VisualisationManager OpenViBE::CIdentifier(0xB067FED2, 0x279602C1)
#define OVK_ClassId_Kernel_Visualisation_VisualisationWidget  OpenViBE::CIdentifier(0xE13332C5, 0x80E01818)
#define OVK_ClassId_Kernel_Visualisation_VisualisationTree    OpenViBE::CIdentifier(0x110D7E2A, 0x10C0E999)

//___________________________________________________________________//
//                                                                   //
// Tools Class Identifiers                                           //
//___________________________________________________________________//
//                                                                   //

#define OVK_ClassId_Tools_ScopeTester                         OpenViBE::CIdentifier(0x5978C732, 0x5903C7A2)
#define OVK_ClassId_Tools_KernelObjectFactoryHelper           OpenViBE::CIdentifier(0x6059EA7C, 0x081FDAB5)
#define OVK_ClassId_Tools_BoxProxyT                           OpenViBE::CIdentifier(0x4DFDFCE4, 0x49EF078E)
#define OVK_ClassId_Tools_BoxIOProxyT                         OpenViBE::CIdentifier(0x25476613, 0x331633DA)

#define OVK_ClassId_Kernel_Configurable                       OpenViBE::CIdentifier(0x1C1D34A4, 0x2EAB9FA1)
#define OVK_ClassId_Kernel_ConfigurableT                      OpenViBE::CIdentifier(0x6500E672, 0x7520EB07)
#define OVK_ClassId_Kernel_ParameterT                         OpenViBE::CIdentifier(0x6DF62060, 0x65A40A62)
#define OVK_ClassId_Kernel_UIntegerParameter                  OpenViBE::CIdentifier(0x665A47DC, 0x650630EE)
#define OVK_ClassId_Kernel_IntegerParameter                   OpenViBE::CIdentifier(0x52DA0F06, 0x45155D4F)
#define OVK_ClassId_Kernel_EnumerationParameter               OpenViBE::CIdentifier(0x1825F2CA, 0x5589B8E2)
#define OVK_ClassId_Kernel_BooleanParameter                   OpenViBE::CIdentifier(0x4E79D376, 0x151D2473)
#define OVK_ClassId_Kernel_FloatParameter                     OpenViBE::CIdentifier(0x37B24229, 0x6A053984)
#define OVK_ClassId_Kernel_StringParameter                    OpenViBE::CIdentifier(0x4AA96293, 0x2B76B89C)
#define OVK_ClassId_Kernel_IdentifierParameter                OpenViBE::CIdentifier(0x1C4D8B4C, 0x67CFF22B)
#define OVK_ClassId_Kernel_MatrixParameter                    OpenViBE::CIdentifier(0xB1C67A8B, 0xED6BD64D)
#define OVK_ClassId_Kernel_StimulationSetParameter            OpenViBE::CIdentifier(0x758D94E1, 0x40F42B99)
#define OVK_ClassId_Kernel_MemoryBufferParameter              OpenViBE::CIdentifier(0x8E21C24F, 0xD77B7E6D)
#define OVK_ClassId_Kernel_ObjectParameter                    OpenViBE::CIdentifier(0x5A9CB52C, 0x7F02D159)
#define OVK_ClassId_Kernel_PointerParameter                   OpenViBE::CIdentifier(0x29BDDFF0, 0x85B669E2)

//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

// #define OVK_OS_Linux
// #define OVK_OS_Windows
// #define OVK_OS_MacOS
// #define OVK_OS_

#if defined TARGET_OS_Windows
 #define OVK_OS_Windows
#elif defined TARGET_OS_Linux
 #define OVK_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OVK_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define OVK_ARCHITECTURE_i386
// #define OVK_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define OVK_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define OVK_COMPILATOR_GCC
// #define OVK_COMPILATOR_VisualStudio
// #define OVK_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define OVK_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define OVK_COMPILATOR_VisualStudio
#else
 #warning "No target compilator defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Build type identification                                         //
//___________________________________________________________________//
//                                                                   //

// #define OVK_BUILDTYPE_Debug
// #define OVK_BUILDTYPE_Release

#if defined TARGET_BUILDTYPE_Debug
 #define OVK_BUILDTYPE_Debug
#elif defined TARGET_BUILDTYPE_Release
 #define OVK_BUILDTYPE_Release
#else
 #warning "No build type defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// API Definition                                                    //
//___________________________________________________________________//
//                                                                   //

// Taken from
// - http://people.redhat.com/drepper/dsohowto.pdf
// - http://www.nedprod.com/programs/gccvisibility.html
#if defined OVK_Shared
 #if defined OVK_OS_Windows
  #define OVK_API_Export __declspec(dllexport)
  #define OVK_API_Import __declspec(dllimport)
 #elif defined OVK_OS_Linux
  #define OVK_API_Export __attribute__((visibility("default")))
  #define OVK_API_Import __attribute__((visibility("default")))
 #else
  #define OVK_API_Export
  #define OVK_API_Import
 #endif
#else
 #define OVK_API_Export
 #define OVK_API_Import
#endif

#if defined OVK_Exports
 #define OVK_API OVK_API_Export
#else
 #define OVK_API OVK_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __OpenViBEKernel_Defines_H__
