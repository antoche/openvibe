#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Plugin Object Descriptor Class Identifiers                        //
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_LDAClassifierDesc                       OpenViBE::CIdentifier(0x1AE009FE, 0xF4FB82FB)

/*
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xBE9EBA5C, 0xA8415D37)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xADF3B87A, 0x875B6EB0)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x88633534, 0x5ECB0F4E)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xC18FFCC7, 0xD7C3ECD1)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xE4861C73, 0x25FC9333)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x45DF353F, 0x7AFF72B0)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x16DFF805, 0x0BC431BF)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x020300BF, 0xB4BAC0A1)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xBB1C5CFB, 0x1DD8690F)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x1575A534, 0xD7C09DD9)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xDA68368B, 0xAD279BF7)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x335384EA, 0x88C917D9)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xA15EAEC5, 0xAB0CE73D)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xE405260B, 0x59EEFAE4)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x1E1065B2, 0x2CA32013)
*/

//___________________________________________________________________//
//                                                                   //
// Plugin Object Class Identifiers                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_LDAClassifier                           OpenViBE::CIdentifier(0x49F18236, 0x75AE12FD)

/*
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x7FEFDCA9, 0x816ED903)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x781F51CA, 0xE6E3B0B8)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xE5103C63, 0x08D825E0)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x043A1BC4, 0x925D3CD6)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x5CF66A73, 0xF5BBF0BF)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xD5C5EF91, 0xE1B1C4F4)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xEBAEB213, 0xDD4735A0)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x344A52F5, 0x489DB439)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x2F9ECA0B, 0x8D3CA7BD)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x1BD67420, 0x587600E6)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xB083614E, 0x26C6B4BD)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x7D13B924, 0xF194DA09)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x2397E74F, 0xCAC8F95C)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0xD6E1AC79, 0xBE50C28F)
#define OVP_ClassId_                                        OpenViBE::CIdentifier(0x5E8D9B97, 0xF82F92D7)
*/

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
