/**
 * \file defines.h
 * \author Yann Renard (INRIA/IRISA)
 * \date 2006-08-07
 * \brief Contains basic type definitions and EBML standard identifiers
 */

#ifndef __EBML_defines_H__
#define __EBML_defines_H__

//___________________________________________________________________//
//                                                                   //
// EBML Standard identifiers definitions                             //
//___________________________________________________________________//
//                                                                   //

/**
 * \name EBML standard identifiers definitions
 *
 * Be sure to look at http://ebml.sourceforge.net/specs/ to
 * have more information on EBML and its standard identifiers.
 */

//@{

/**
 * EBML Header.
 * This is a master node.
 * It contains several information about the ebml stream.
 */
#define EBML_Identifier_Header				EBML::CIdentifier(0x0A45DFA3)
/**
 * EBML Version.
 * This is a child node of type \c integer.
 * It contains the EBML version.
 */
#define EBML_Identifier_EBMLVersion			EBML::CIdentifier(0x0286)
/**
 * EBML Identifier Length.
 * This is a child node of type \c integer.
 * Indicates the parser the maximum number of bytes an identifier
 * can have.
 */
#define EBML_Identifier_EBMLIdLength		EBML::CIdentifier(0x02F7)
/**
 * EBML Size Length.
 * This is a child node of type \c integer.
 * Indicates the paser the maximum number of bytes a node
 * can have.
 */
#define EBML_Identifier_EBMLSizeLength		EBML::CIdentifier(0x02F3)
/**
 * EBML Document Type.
 * This is a child node of type \c string.
 * Indicates the parser the kind of document being parsed.
 */
#define EBML_Identifier_DocType				EBML::CIdentifier(0x0282)
/**
 * EBML Document Type Version.
 * This is a child node of type \c integer.
 * Indicates the parser the specification version of the document
 * being parsed.
 */
#define EBML_Identifier_DocTypeVersion		EBML::CIdentifier(0x0287)
/**
 * EBML Document Type Read Version.
 * This is a child node of type \c integer.
 * Indicates the parser the version of the reader to use in order
 * to understand the document content.
 */
#define EBML_Identifier_DocTypeReadVersion	EBML::CIdentifier(0x0285)
/**
 * EBML Void.
 * This is a child node of type \c binary.
 * Indicates an ununderstandable chunk of data to skip.
 */
#define EBML_Identifier_Void				EBML::CIdentifier(0x6C)
/**
 * EBML CRC32.
 * This is a child node of type \c binary.
 * This is a CRC32 checksum of the content between this CRC32
 * and the last one.
 */
#define EBML_Identifier_CRC32				EBML::CIdentifier(0x02FE)

//@}

//___________________________________________________________________//
//                                                                   //
// Basic type definitions                                            //
//___________________________________________________________________//
//                                                                   //

/**
 * \namespace EBML
 * \author Yann Renard
 * \date 2006-08-07
 * \brief Namespace for the EBML lib
 *
 * This namespace contains the classes, types and functions used
 * and offered by the EBML parsing/formating library.
 *
 * Most important classes to look at are :
 *  - EBML::IReader for EBML parsing
 *  - EBML::IWriter for EBML formating
 */
namespace EBML
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

// #define EBML_OS_Linux
// #define EBML_OS_Windows
// #define EBML_OS_MacOS
// #define EBML_OS_

#if defined TARGET_OS_Windows
 #define EBML_OS_Windows
#elif defined TARGET_OS_Linux
 #define EBML_OS_Linux
#elif defined TARGET_OS_MacOS
 #define EBML_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define EBML_ARCHITECTURE_i386
// #define EBML_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define EBML_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define EBML_COMPILATOR_GCC
// #define EBML_COMPILATOR_VisualStudio
// #define EBML_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define EBML_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define EBML_COMPILATOR_VisualStudio
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
#if defined EBML_Shared
 #if defined EBML_OS_Windows
  #define EBML_API_Export __declspec(dllexport)
  #define EBML_API_Import __declspec(dllimport)
 #elif defined EBML_OS_Linux
  #define EBML_API_Export __attribute__((visibility("default")))
  #define EBML_API_Import
 #else
  #define EBML_API_Export
  #define EBML_API_Import
 #endif
#else
 #define EBML_API_Export
 #define EBML_API_Import
#endif

#if defined EBML_Exports
 #define EBML_API EBML_API_Export
#else
 #define EBML_API EBML_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __EBML_defines_H__
