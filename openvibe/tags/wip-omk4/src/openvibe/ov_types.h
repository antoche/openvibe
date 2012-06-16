#ifndef __OpenViBE_Types_H__
#define __OpenViBE_Types_H__

namespace OpenViBE
{

//___________________________________________________________________//
//                                                                   //
// Boolean basic type                                                //
//___________________________________________________________________//
//                                                                   //

	typedef bool boolean;					///< Boolean

//___________________________________________________________________//
//                                                                   //
// Integer basic types                                               //
//___________________________________________________________________//
//                                                                   //

	typedef unsigned long long uint64;		///< 64 bits unsigned integer
	typedef signed long long int64;			///< 64 bits signed integer
	typedef unsigned long uint32;			///< 32 bits unsigned integer
	typedef signed long int32;				///< 32 bits signed integer
	typedef unsigned short uint16;			///< 16 bits unsigned integer
	typedef signed short int16;				///< 16 bits signed integer
	typedef unsigned char uint8;			///< 8 bits unsigned integer
	typedef signed char int8;				///< 8 bits signed integer

//___________________________________________________________________//
//                                                                   //
// Floating point basic types                                        //
//___________________________________________________________________//
//                                                                   //

	typedef float float32;					///< 32 bits floating point
	typedef double float64;					///< 64 bits floating point
	typedef long double float80;			///< 80 bits floating point
};

#endif // __OpenViBE_Types_H__
