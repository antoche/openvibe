#ifndef __OpenViBE_Defines_H__
#define __OpenViBE_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Global Class Identifiers                                          //
//___________________________________________________________________//
//                                                                   //

#define OV_ClassId_Object                                    OpenViBE::CIdentifier(0x327B23C6, 0x6B8B4567)
#define OV_ClassId_ObjectVisitor                             OpenViBE::CIdentifier(0x1ED722DC, 0x00BDA996)
#define OV_ClassId_KernelLoader                              OpenViBE::CIdentifier(0x1930F64B, 0x6F689236)
#define OV_ClassId_KernelLoaderFinal                         OpenViBE::CIdentifier(0x624A6E5B, 0x52228CEA)

//___________________________________________________________________//
//                                                                   //
// Kernel Class Identifiers                                          //
//___________________________________________________________________//
//                                                                   //

#define OV_ClassId_Kernel_Kernel                             OpenViBE::CIdentifier(0x255EEE43, 0x2B852543)
#define OV_ClassId_Kernel_KernelContext                      OpenViBE::CIdentifier(0x1AFF3D5A, 0x77F610EE)
#define OV_ClassId_Kernel_KernelDesc                         OpenViBE::CIdentifier(0x5410291E, 0x2FEF6B81)
#define OV_ClassId_Kernel_KernelObject                       OpenViBE::CIdentifier(0x69BAE694, 0x163C8B2D)
#define OV_ClassId_Kernel_KernelObjectFactory                OpenViBE::CIdentifier(0x04D88707, 0x21F2F456)
#define OV_ClassId_Kernel_TypeManager                        OpenViBE::CIdentifier(0x3E800821, 0x5D1DE74E)
#define OV_ClassId_Kernel_Parameter                          OpenViBE::CIdentifier(0xF980A924, 0x696E2BC8)
#define OV_ClassId_Kernel_Configurable                       OpenViBE::CIdentifier(0x52CEA963, 0x210D78D8)
#define OV_ClassId_Kernel_ObjectVisitorContext               OpenViBE::CIdentifier(0x84326BA5, 0xB0E630D8)

#define OV_ClassId_Kernel_Algorithm_AlgorithmContext         OpenViBE::CIdentifier(0x6DAC2B49, 0xF424D47B)
#define OV_ClassId_Kernel_Algorithm_AlgorithmManager         OpenViBE::CIdentifier(0x2596C3E4, 0xB2D91D95)
#define OV_ClassId_Kernel_Algorithm_AlgorithmProto           OpenViBE::CIdentifier(0x06DF986C, 0x39B795CC)
#define OV_ClassId_Kernel_Algorithm_AlgorithmProxy           OpenViBE::CIdentifier(0xE015C4C5, 0x479C3D46)

#define OV_ClassId_Kernel_Log_LogManager                     OpenViBE::CIdentifier(0x22990B50, 0x571010D4)
#define OV_ClassId_Kernel_Log_LogListener                    OpenViBE::CIdentifier(0x766B92CB, 0x33B01B15)

#define OV_ClassId_Kernel_Player_BoxAlgorithmContext         OpenViBE::CIdentifier(0x275F4BB8, 0x49E08D24)
#define OV_ClassId_Kernel_Player_Message                     OpenViBE::CIdentifier(0x64DCCE4E, 0x530CA785)
#define OV_ClassId_Kernel_Player_MessageClock                OpenViBE::CIdentifier(0x58AE3092, 0x0C3371A7)
#define OV_ClassId_Kernel_Player_MessageEvent                OpenViBE::CIdentifier(0x1B9B49DB, 0x6DA217E7)
#define OV_ClassId_Kernel_Player_MessageSignal               OpenViBE::CIdentifier(0x4E1709E5, 0x38AADA6C)
#define OV_ClassId_Kernel_Player_Player                      OpenViBE::CIdentifier(0x790A8CC5, 0x1439C776)
#define OV_ClassId_Kernel_Player_PlayerContext               OpenViBE::CIdentifier(0x1F8D3E14, 0x73A4DB1F)
#define OV_ClassId_Kernel_Player_PlayerManager               OpenViBE::CIdentifier(0x2A49A89B, 0x78843238)

#define OV_ClassId_Kernel_Plugins_PluginModule               OpenViBE::CIdentifier(0x74EAC344, 0x375C88EC)
#define OV_ClassId_Kernel_Plugins_PluginModuleContext        OpenViBE::CIdentifier(0x624A6E5B, 0x52228CEA)
#define OV_ClassId_Kernel_Plugins_PluginManager              OpenViBE::CIdentifier(0x690ED3AC, 0x2D0702F0)

#define OV_ClassId_Kernel_Scenario_Attributable              OpenViBE::CIdentifier(0x61B64E38, 0x0B36B4E8)
#define OV_ClassId_Kernel_Scenario_Box                       OpenViBE::CIdentifier(0x41209015, 0x1183237A)
#define OV_ClassId_Kernel_Scenario_BoxIO                     OpenViBE::CIdentifier(0x110AAB4F, 0x157CBEF1)
#define OV_ClassId_Kernel_Scenario_BoxProto                  OpenViBE::CIdentifier(0x169ED1C1, 0x639C5957)
#define OV_ClassId_Kernel_Scenario_Link                      OpenViBE::CIdentifier(0x38F6E10D, 0x6BA2721A)
#define OV_ClassId_Kernel_Scenario_ProcessingUnit            OpenViBE::CIdentifier(0x3681EC07, 0x337DE51F)
#define OV_ClassId_Kernel_Scenario_Scenario                  OpenViBE::CIdentifier(0x6EED456C, 0x2FC6F03A)
#define OV_ClassId_Kernel_Scenario_ScenarioImporterContext   OpenViBE::CIdentifier(0x734C0B6A, 0x7EFEBBC8)
#define OV_ClassId_Kernel_Scenario_ScenarioExporterContext   OpenViBE::CIdentifier(0x583A5B71, 0x2C06662D)
#define OV_ClassId_Kernel_Scenario_ScenarioManager           OpenViBE::CIdentifier(0x67F31911, 0x3FA45C02)

#define OV_ClassId_Kernel_Visualisation_VisualisationContext OpenViBE::CIdentifier(0x525AE260, 0x0B2F7E1F)
#define OV_ClassId_Kernel_Visualisation_VisualisationManager OpenViBE::CIdentifier(0x36E5DE48, 0xFD4AF8A8)
#define OV_ClassId_Kernel_Visualisation_VisualisationWidget  OpenViBE::CIdentifier(0xD88B5297, 0xFE0E9041)
#define OV_ClassId_Kernel_Visualisation_VisualisationTree    OpenViBE::CIdentifier(0xB8F4A764, 0x8D1FF3BF)

//___________________________________________________________________//
//                                                                   //
// Plugin Class Identifiers                                          //
//___________________________________________________________________//
//                                                                   //

#define OV_ClassId_Plugins_PluginObject                      OpenViBE::CIdentifier(0x1356510D, 0x6B6532DF)
#define OV_ClassId_Plugins_PluginObjectDesc                  OpenViBE::CIdentifier(0x100E6855, 0x7C1CA212)

#define OV_ClassId_Plugins_Algorithm                         OpenViBE::CIdentifier(0x4C4499C7, 0xBD7F16C9)
#define OV_ClassId_Plugins_AlgorithmDesc                     OpenViBE::CIdentifier(0xBB9BAE23, 0x8C81D56E)

#define OV_ClassId_Plugins_BoxAlgorithm                      OpenViBE::CIdentifier(0x2A910204, 0x57E55735)
#define OV_ClassId_Plugins_BoxAlgorithmDesc                  OpenViBE::CIdentifier(0x2C4ECAD0, 0x19607084)

#define OV_ClassId_Plugins_ScenarioImporter                  OpenViBE::CIdentifier(0x6BF28385, 0x77F8D9A1)
#define OV_ClassId_Plugins_ScenarioImporterDesc              OpenViBE::CIdentifier(0x05146C29, 0x283B5BB8)

#define OV_ClassId_Plugins_ScenarioExporter                  OpenViBE::CIdentifier(0x6C54A6C9, 0x174982CD)
#define OV_ClassId_Plugins_ScenarioExporterDesc              OpenViBE::CIdentifier(0x13B1BD52, 0x06AB7557)

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

#define OV_ClassId_Matrix                                    OpenViBE::CIdentifier(0x65AA951D, 0x73EE781E)
#define OV_ClassId_MatrixBridge                              OpenViBE::CIdentifier(0x4331378D, 0x5A1E0A69)
#define OV_ClassId_MatrixImpl                                OpenViBE::CIdentifier(0x2F59C42E, 0x54166A96)

#define OV_ClassId_StimulationSet                            OpenViBE::CIdentifier(0x691473DE, 0x2EE37ABD)
#define OV_ClassId_StimulationSetBridge                      OpenViBE::CIdentifier(0x39F81801, 0x2491BF81)
#define OV_ClassId_StimulationSetImpl                        OpenViBE::CIdentifier(0x1649669C, 0x25B89AD7)

#define OV_ClassId_MemoryBuffer                              OpenViBE::CIdentifier(0xADF318BB, 0xB728A579)
#define OV_ClassId_MemoryBufferBridge                        OpenViBE::CIdentifier(0xFD84704B, 0xA9E61735)
#define OV_ClassId_MemoryBufferImpl                          OpenViBE::CIdentifier(0x51551D5C, 0x80B34EEE)

//___________________________________________________________________//
//                                                                   //
// Unused Class Identifiers                                          //
//___________________________________________________________________//
//                                                                   //

#define OV_TypeId_Boolean                                    OpenViBE::CIdentifier(0x2CDB2F0B, 0x12F231EA)
#define OV_TypeId_Integer                                    OpenViBE::CIdentifier(0x007DEEF9, 0x2F3E95C6)
#define OV_TypeId_Float                                      OpenViBE::CIdentifier(0x512A166F, 0x5C3EF83F)
#define OV_TypeId_String                                     OpenViBE::CIdentifier(0x79A9EDEB, 0x245D83FC)
#define OV_TypeId_Filename                                   OpenViBE::CIdentifier(0x330306DD, 0x74A95F98)
#define OV_TypeId_Stimulation                                OpenViBE::CIdentifier(0x2C132D6E, 0x44AB0D97)
#define OV_TypeId_LogLevel                                   OpenViBE::CIdentifier(0xA88B3667, 0x0871638C)

#define OV_TypeId_EBMLStream                                 OpenViBE::CIdentifier(0x434F6587, 0x2EFD2B7E)
#define   OV_TypeId_ChannelLocalisation                      OpenViBE::CIdentifier(0x013DF452, 0xA3A8879A)
#define   OV_TypeId_ExperimentationInformation               OpenViBE::CIdentifier(0x403488E7, 0x565D70B6)
#define   OV_TypeId_Stimulations                             OpenViBE::CIdentifier(0x6F752DD0, 0x082A321E)
#define   OV_TypeId_StreamedMatrix                           OpenViBE::CIdentifier(0x544A003E, 0x6DCBA5F6)
#define     OV_TypeId_FeatureVector                          OpenViBE::CIdentifier(0x17341935, 0x152FF448)
#define     OV_TypeId_Signal                                 OpenViBE::CIdentifier(0x5BA36127, 0x195FEAE1)
#define     OV_TypeId_Spectrum                               OpenViBE::CIdentifier(0x1F261C0A, 0x593BF6BD)

//___________________________________________________________________//
//                                                                   //
// Several other defined values                                      //
//___________________________________________________________________//
//                                                                   //

#define OV_Value_EnumeratedStringSeparator                           ';'

//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

// #define OV_OS_Linux
// #define OV_OS_Windows
// #define OV_OS_MacOS
// #define OV_OS_

#if defined TARGET_OS_Windows
 #define OV_OS_Windows
#elif defined TARGET_OS_Linux
 #define OV_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OV_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define OV_ARCHITECTURE_i386
// #define OV_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define OV_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define OV_COMPILATOR_GCC
// #define OV_COMPILATOR_VisualStudio
// #define OV_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define OV_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define OV_COMPILATOR_VisualStudio
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
#if defined OV_Shared
 #if defined OV_OS_Windows
  #define OV_API_Export __declspec(dllexport)
  #define OV_API_Import __declspec(dllimport)
 #elif defined OV_OS_Linux
  #define OV_API_Export __attribute__((visibility("default")))
  #define OV_API_Import __attribute__((visibility("default")))
 #else
  #define OV_API_Export
  #define OV_API_Import
 #endif
#else
 #define OV_API_Export
 #define OV_API_Import
#endif

#if defined OV_Exports
 #define OV_API OV_API_Export
#else
 #define OV_API OV_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __OpenViBE_Defines_H__
