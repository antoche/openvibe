#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Plugin Object Descriptor Class Identifiers                        //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_GDFFileReaderDesc                                          OpenViBE::CIdentifier(0x2E2543C1, 0x47E3739E)
#define OVP_ClassId_GDFFileWriterDesc                                          OpenViBE::CIdentifier(0x375BF8EC, 0x023ACEEB)
#define OVP_ClassId_GenericStreamReaderDesc                                    OpenViBE::CIdentifier(0x1E8AAB1A, 0x085D72F6)
#define OVP_ClassId_GenericStreamWriterDesc                                    OpenViBE::CIdentifier(0x02817C77, 0x77FE3D6A)
#define OVP_ClassId_BCICompetitionIIIbReaderDesc                               OpenViBE::CIdentifier(0x00CC1B39, 0x206A17AE)
#define OVP_ClassId_BoxAlgorithm_CSVFileWriterDesc 							   OpenViBE::CIdentifier(0x65075FF7, 0x2B555E97)
#define OVP_ClassId_BoxAlgorithm_CSVFileReaderDesc 							   OpenViBE::CIdentifier(0x193F22E9, 0x26A67233)


//-------------------------------------------------------------------//
// Brainamp File Reader
#define OVP_ClassId_Algorithm_BrainampFileReader                               OpenViBE::CIdentifier(0x2533718F, 0x07AB3AF5)
#define OVP_ClassId_Algorithm_BrainampFileReaderDesc                           OpenViBE::CIdentifier(0x2C5B2D6C, 0x242253CF)

#define OVP_ClassId_BoxAlgorithm_BrainampFileReader                            OpenViBE::CIdentifier(0x61894B28, 0x185E57D2)
#define OVP_ClassId_BoxAlgorithm_BrainampFileReaderDesc                        OpenViBE::CIdentifier(0x77C650B0, 0x41386F47)

#define OVP_Algorithm_BrainampFileReader_InputParameterId_Filename             OpenViBE::CIdentifier(0x000C486C, 0x477C80B9)
#define OVP_Algorithm_BrainampFileReader_InputParameterId_EpochDuration        OpenViBE::CIdentifier(0x002AEE72, 0x288D489E)
#define OVP_Algorithm_BrainampFileReader_InputParameterId_SeekTime             OpenViBE::CIdentifier(0x00627156, 0x55790CAE)

#define OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentStartTime    OpenViBE::CIdentifier(0x003CE019, 0x3DADE050)
#define OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentEndTime      OpenViBE::CIdentifier(0x3B87DE16, 0xB8EFB2FB)
#define OVP_Algorithm_BrainampFileReader_OutputParameterId_SamplingRate        OpenViBE::CIdentifier(0x00D3CABB, 0x339326C2)
#define OVP_Algorithm_BrainampFileReader_OutputParameterId_SignalMatrix        OpenViBE::CIdentifier(0x0073A91A, 0x6D1D3D26)
#define OVP_Algorithm_BrainampFileReader_OutputParameterId_Stimulations        OpenViBE::CIdentifier(0x008F7C49, 0x6ED710A9)

#define OVP_Algorithm_BrainampFileReader_InputTriggerId_Open                   OpenViBE::CIdentifier(0x00BCF286, 0x6F5FA2F4)
#define OVP_Algorithm_BrainampFileReader_InputTriggerId_Seek                   OpenViBE::CIdentifier(0x00C3ACAB, 0x4E3DE014)
#define OVP_Algorithm_BrainampFileReader_InputTriggerId_Next                   OpenViBE::CIdentifier(0x003CD062, 0x739F973E)
#define OVP_Algorithm_BrainampFileReader_InputTriggerId_Close                  OpenViBE::CIdentifier(0x003462EA, 0x031FB8FA)

#define OVP_Algorithm_BrainampFileReader_OutputTriggerId_Error                 OpenViBE::CIdentifier(0x00B1E3DF, 0x3ABC6C5A)
#define OVP_Algorithm_BrainampFileReader_OutputTriggerId_DataProduced          OpenViBE::CIdentifier(0x00E7D5F9, 0x1471AFF2)

//-------------------------------------------------------------------//

//-------------------------------------------------------------------//
// OV Matrix File Reader
#define OVP_ClassId_Algorithm_OVMatrixFileReader                               OpenViBE::CIdentifier(0x10661A33, 0x0B0F44A7)
#define OVP_ClassId_Algorithm_OVMatrixFileReaderDesc                           OpenViBE::CIdentifier(0x0E873B5E, 0x0A287FCB)

#define OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename             OpenViBE::CIdentifier(0x28F87B29, 0x0B09737E)

#define OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix              OpenViBE::CIdentifier(0x2F9521E0, 0x027D789F)

#define OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Open                   OpenViBE::CIdentifier(0x2F996376, 0x2A942485)
#define OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Load                   OpenViBE::CIdentifier(0x22841807, 0x102D681C)
#define OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Close                  OpenViBE::CIdentifier(0x7FDE77DA, 0x384A0B3D)

#define OVP_Algorithm_OVMatrixFileReader_OutputTriggerId_Error                 OpenViBE::CIdentifier(0x6D4F2F4B, 0x05EC6CB9)
#define OVP_Algorithm_OVMatrixFileReader_OutputTriggerId_DataProduced          OpenViBE::CIdentifier(0x76F46051, 0x003B6FE8)

//-------------------------------------------------------------------//

//-------------------------------------------------------------------//
// OV Matrix File Writer
#define OVP_ClassId_Algorithm_OVMatrixFileWriter                               OpenViBE::CIdentifier(0x739158FC, 0x1E8240CC)
#define OVP_ClassId_Algorithm_OVMatrixFileWriterDesc                           OpenViBE::CIdentifier(0x44CF6DD0, 0x329D47F9)

#define OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename             OpenViBE::CIdentifier(0x330D2D0B, 0x175271E6)
#define OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix               OpenViBE::CIdentifier(0x6F6402EE, 0x493044F3)
//-------------------------------------------------------------------//

//-------------------------------------------------------------------//
// ElectrodeLocalisationFileReader
#define OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader               OpenViBE::CIdentifier(0x40704155, 0x19C50E8F)
#define OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReaderDesc           OpenViBE::CIdentifier(0x4796613F, 0x653A48D5)
//-------------------------------------------------------------------//

//___________________________________________________________________//
//                                                                   //
// Plugin Object Class Identifiers                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_GDFFileReader                                              OpenViBE::CIdentifier(0x3EEB1264, 0x4EDFBD9A)
#define OVP_ClassId_GDFFileWriter                                              OpenViBE::CIdentifier(0x1E7B2155, 0x107289CE)
#define OVP_ClassId_GenericStreamReader                                        OpenViBE::CIdentifier(0x0B1D880D, 0x02A17229)
#define OVP_ClassId_GenericStreamWriter                                        OpenViBE::CIdentifier(0x78EA86B0, 0x2933E255)
#define OVP_ClassId_BCICompetitionIIIbReader                                   OpenViBE::CIdentifier(0x0085B814, 0x6E45DA7E)
#define OVP_ClassId_BoxAlgorithm_CSVFileWriter     							   OpenViBE::CIdentifier(0x2C9312F1, 0x2D6613E5)
#define OVP_ClassId_BoxAlgorithm_CSVFileReader     							   OpenViBE::CIdentifier(0x641D0717, 0x02884107)

#define OVP_ClassId_BoxAlgorithm_EDFFileWriter                                 OpenViBE::CIdentifier(0x0D454DCE, 0x470A4C02)
#define OVP_ClassId_BoxAlgorithm_EDFFileWriterDesc                             OpenViBE::CIdentifier(0x0D454DCE, 0x470A4C02)


//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_NodeId_OpenViBEStream_Header              EBML::CIdentifier(0xF59505AB, 0x3684C8D8)
#define OVP_NodeId_OpenViBEStream_Header_Compression  EBML::CIdentifier(0x40358769, 0x166380D1)
#define OVP_NodeId_OpenViBEStream_Header_ChannelType  EBML::CIdentifier(0x732EC1D1, 0xFE904087)
#define OVP_NodeId_OpenViBEStream_Buffer              EBML::CIdentifier(0x2E60AD18, 0x87A29BDF)
#define OVP_NodeId_OpenViBEStream_Buffer_ChannelIndex EBML::CIdentifier(0x30A56D8A, 0xB9C12238)
#define OVP_NodeId_OpenViBEStream_Buffer_StartTime    EBML::CIdentifier(0x093E6A0A, 0xC5A9467B)
#define OVP_NodeId_OpenViBEStream_Buffer_EndTime      EBML::CIdentifier(0x8B5CCCD9, 0xC5024F29)
#define OVP_NodeId_OpenViBEStream_Buffer_Content      EBML::CIdentifier(0x8D4B0BE8, 0x7051265C)

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
