#ifndef __OpenViBE_Types_H__
#define __OpenViBE_Types_H__

namespace OpenViBE
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
	
//___________________________________________________________________//
//                                                                   //
// Time basic type                                                   //
//___________________________________________________________________//
//                                                                   //

	typedef struct _time64
	{
		_time64(uint64 ui64TimeValue) : m_ui64TimeValue(ui64TimeValue){}
		uint64 m_ui64TimeValue;

	}time64;                                ///< Time on uint64

};

#endif // __OpenViBE_Types_H__
