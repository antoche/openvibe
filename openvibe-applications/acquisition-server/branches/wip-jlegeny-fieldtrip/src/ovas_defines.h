#ifndef __OpenViBE_AcquisitionServer_Defines_H__
#define __OpenViBE_AcquisitionServer_Defines_H__

#define OVAS_Impedance_NotAvailable -2
#define OVAS_Impedance_Unknown -1
#define OVAS_Impedance_Zero 0

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

// #define OVAS_OS_Linux
// #define OVAS_OS_Windows
// #define OVAS_OS_MacOS
// #define OVAS_OS_

#if defined TARGET_OS_Windows
 #define OVAS_OS_Windows
#elif defined TARGET_OS_Linux
 #define OVAS_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OVAS_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define OVAS_ARCHITECTURE_i386
// #define OVAS_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define OVAS_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define OVAS_COMPILATOR_GCC
// #define OVAS_COMPILATOR_VisualStudio
// #define OVAS_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define OVAS_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define OVAS_COMPILATOR_VisualStudio
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
#if defined OVAS_Shared
 #if defined OVAS_OS_Windows
  #define OVAS_API_Export __declspec(dllexport)
  #define OVAS_API_Import __declspec(dllimport)
 #elif defined OVAS_OS_Linux
  #define OVAS_API_Export __attribute__((visibility("default")))
  #define OVAS_API_Import __attribute__((visibility("default")))
 #else
  #define OVAS_API_Export
  #define OVAS_API_Import
 #endif
#else
 #define OVAS_API_Export
 #define OVAS_API_Import
#endif

#if defined OVAS_Exports
 #define OVAS_API OVAS_API_Export
#else
 #define OVAS_API OVAS_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __OpenViBE_AcquisitionServer_Defines_H__
