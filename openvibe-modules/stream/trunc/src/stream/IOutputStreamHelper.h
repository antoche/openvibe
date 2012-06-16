#ifndef __Stream_IOutputStreamHelper_H__
#define __Stream_IOutputStreamHelper_H__

#include "IOutputStream.h"

namespace Stream
{
	class Stream_API IOutputStreamHelper : public Stream::IOutputStream
	{
	public:

		virtual Stream::boolean setEndianness(const Stream::EEndianness& eEndianness)=0;
		virtual Stream::EEndianness getEndianness(void)=0;

		virtual Stream::boolean sendUInteger8(Stream::uint8 uiValue)=0;
		virtual Stream::boolean sendUInteger16(Stream::uint16 uiValue)=0;
		virtual Stream::boolean sendUInteger32(Stream::uint32 uiValue)=0;
		virtual Stream::boolean sendUInteger64(Stream::uint64 uiValue)=0;

		virtual Stream::boolean sendSInteger8(Stream::int8 iValue)=0;
		virtual Stream::boolean sendSInteger16(Stream::int16 iValue)=0;
		virtual Stream::boolean sendSInteger32(Stream::int32 iValue)=0;
		virtual Stream::boolean sendSInteger64(Stream::int64 iValue)=0;

		virtual Stream::boolean sendFloat32(Stream::float32 fValue)=0;
		virtual Stream::boolean sendFloat64(Stream::float64 fValue)=0;

		virtual Stream::boolean sendASCIIString(const char* sString)=0;
		virtual Stream::boolean sendUTF8String(const Stream::uint8* sString)=0;
		virtual Stream::boolean sendUTF16String(const Stream::uint16* sString)=0;
	};

	extern Stream_API Stream::IOutputStreamHelper* createOutputStreamHelper(Stream::IOutputStream& rOutputStream);
};

#endif // __Stream_IOutputStreamHelper_H__
