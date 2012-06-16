#ifndef __Stream_Defines_H__
#define __Stream_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Basic type definitions                                            //
//___________________________________________________________________//
//                                                                   //

namespace Stream
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

	enum EEndianness
	{
		Endianness_None,
		Endianness_LittleEndian,
		Endianness_BigEndian,
	};

};

//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

// #define Stream_OS_Linux
// #define Stream_OS_Windows
// #define Stream_OS_MacOS
// #define Stream_OS_

#if defined TARGET_OS_Windows
 #define Stream_OS_Windows
#elif defined TARGET_OS_Linux
 #define Stream_OS_Linux
#elif defined TARGET_OS_MacOS
 #define Stream_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define Stream_ARCHITECTURE_i386
// #define Stream_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define Stream_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define Stream_COMPILATOR_GCC
// #define Stream_COMPILATOR_VisualStudio
// #define Stream_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define Stream_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define Stream_COMPILATOR_VisualStudio
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
#if defined Stream_Shared
 #if defined Stream_OS_Windows
  #define Stream_API_Export __declspec(dllexport)
  #define Stream_API_Import __declspec(dllimport)
 #elif defined Stream_OS_Linux
  #define Stream_API_Export __attribute__((visibility("default")))
  #define Stream_API_Import
 #else
  #define Stream_API_Export
  #define Stream_API_Import
 #endif
#else
 #define Stream_API_Export
 #define Stream_API_Import
#endif

#if defined Stream_Exports
 #define Stream_API Stream_API_Export
#else
 #define Stream_API Stream_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __Stream_Defines_H__
