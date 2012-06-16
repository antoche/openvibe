#ifndef __Socket_Defines_H__
#define __Socket_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Basic type definitions                                            //
//___________________________________________________________________//
//                                                                   //

namespace Socket
{

//___________________________________________________________________//
//                                                                   //
// Boolean basic type                                                //
//___________________________________________________________________//
//                                                                   //

	typedef bool boolean;                   ///< Boolean

//___________________________________________________________________//
//                                                                   //
// Integer basic types                                               //
//___________________________________________________________________//
//                                                                   //

	typedef unsigned long long uint64;      ///< 64 bits unsigned integer
	typedef signed long long int64;         ///< 64 bits signed integer
	typedef unsigned int uint32;            ///< 32 bits unsigned integer
	typedef signed int int32;               ///< 32 bits signed integer
	typedef unsigned short uint16;          ///< 16 bits unsigned integer
	typedef signed short int16;             ///< 16 bits signed integer
	typedef unsigned char uint8;            ///< 8 bits unsigned integer
	typedef signed char int8;               ///< 8 bits signed integer

//___________________________________________________________________//
//                                                                   //
// Adressing and indexing types                                      //
//___________________________________________________________________//
//                                                                   //

	typedef void* pointer;                  ///< untyped pointer
	typedef unsigned long int pointer_size; ///< addressing range

//___________________________________________________________________//
//                                                                   //
// Floating point basic types                                        //
//___________________________________________________________________//
//                                                                   //

	typedef float float32;                  ///< 32 bits floating point
	typedef double float64;                 ///< 64 bits floating point
	typedef long double float80;            ///< 80 bits floating point

};

//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

// #define Socket_OS_Linux
// #define Socket_OS_Windows
// #define Socket_OS_MacOS
// #define Socket_OS_

#if defined TARGET_OS_Windows
 #define Socket_OS_Windows
#elif defined TARGET_OS_Linux
 #define Socket_OS_Linux
#elif defined TARGET_OS_MacOS
 #define Socket_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define Socket_ARCHITECTURE_i386
// #define Socket_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define Socket_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define Socket_COMPILATOR_GCC
// #define Socket_COMPILATOR_VisualStudio
// #define Socket_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define Socket_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define Socket_COMPILATOR_VisualStudio
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
#if defined Socket_Shared
 #if defined Socket_OS_Windows
  #define Socket_API_Export __declspec(dllexport)
  #define Socket_API_Import __declspec(dllimport)
 #elif defined Socket_OS_Linux
  #define Socket_API_Export __attribute__((visibility("default")))
  #define Socket_API_Import
 #else
  #define Socket_API_Export
  #define Socket_API_Import
 #endif
#else
 #define Socket_API_Export
 #define Socket_API_Import
#endif

#if defined Socket_Exports
 #define Socket_API Socket_API_Export
#else
 #define Socket_API Socket_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __Socket_Defines_H__
