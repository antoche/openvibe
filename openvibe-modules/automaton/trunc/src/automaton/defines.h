#ifndef __Automaton_Defines_H__
#define __Automaton_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Node type identifiers definitions                                 //
//___________________________________________________________________//
//                                                                   //


#define Automaton_Node_Identifier_Automaton         Automaton::CIdentifier(0x00000000)
#define Automaton_Node_Identifier_No_Node           Automaton::CIdentifier(0x00000001)
#define Automaton_Node_Identifier_Action            Automaton::CIdentifier(0x00000010)
#define Automaton_Node_Identifier_ActionSendEvent   Automaton::CIdentifier(0x00000011)
#define Automaton_Node_Identifier_ActionWaitEvent   Automaton::CIdentifier(0x00000012)
#define Automaton_Node_Identifier_ActionWaitTime    Automaton::CIdentifier(0x00000013)

#define Automaton_Node_Identifier_Loop              Automaton::CIdentifier(0x00000021)
#define Automaton_Node_Identifier_LoopFinite        Automaton::CIdentifier(0x00000022)
#define Automaton_Node_Identifier_LoopInfinite      Automaton::CIdentifier(0x00000023)
#define Automaton_Node_Identifier_LoopWaitEvent     Automaton::CIdentifier(0x00000024)

#define Automaton_Node_Identifier_Switch            Automaton::CIdentifier(0x00000031)
#define Automaton_Node_Identifier_SwitchProbabilist Automaton::CIdentifier(0x00000032)
#define Automaton_Node_Identifier_SwitchEvent       Automaton::CIdentifier(0x00000033)

#define Automaton_Node_Identifier_RandomOrder       Automaton::CIdentifier(0x00000041)


#define Automaton_No_Successor                      Automaton::CIdentifier(0xFFFFFFFF)


//___________________________________________________________________//
//                                                                   //
// Basic type definitions                                            //
//___________________________________________________________________//
//                                                                   //

namespace Automaton
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

// #define Automaton_OS_Linux
// #define Automaton_OS_Windows
// #define Automaton_OS_MacOS
// #define Automaton_OS_

#if defined TARGET_OS_Windows
 #define Automaton_OS_Windows
#elif defined TARGET_OS_Linux
 #define Automaton_OS_Linux
#elif defined TARGET_OS_MacOS
 #define Automaton_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define Automaton_ARCHITECTURE_i386
// #define Automaton_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define Automaton_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define Automaton_COMPILATOR_GCC
// #define Automaton_COMPILATOR_VisualStudio
// #define Automaton_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define Automaton_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define Automaton_COMPILATOR_VisualStudio
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
#if defined Automaton_Shared
 #if defined Automaton_OS_Windows
  #define Automaton_API_Export __declspec(dllexport)
  #define Automaton_API_Import __declspec(dllimport)
 #elif defined Automaton_OS_Linux
  #define Automaton_API_Export __attribute__((visibility("default")))
  #define Automaton_API_Import
 #else
  #define Automaton_API_Export
  #define Automaton_API_Import
 #endif
#else
 #define Automaton_API_Export
 #define Automaton_API_Import
#endif

#if defined Automaton_Exports
 #define Automaton_API Automaton_API_Export
#else
 #define Automaton_API Automaton_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __Automaton_Defines_H__
