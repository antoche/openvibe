#ifndef __Stream_IInputStreamHelper_H__
#define __Stream_IInputStreamHelper_H__

#include "IInputStream.h"

namespace Stream
{
	class Stream_API IInputStreamHelper : public Stream::IInputStream
	{
	public:

		virtual Stream::boolean setEndianness(const Stream::EEndianness& eEndianness)=0;
		virtual Stream::EEndianness getEndianness(void)=0;

		virtual Stream::uint8 receiveUInteger8(void)=0;
		virtual Stream::uint16 receiveUInteger16(void)=0;
		virtual Stream::uint32 receiveUInteger32(void)=0;
		virtual Stream::uint64 receiveUInteger64(void)=0;

		virtual Stream::int8 receiveSInteger8(void)=0;
		virtual Stream::int16 receiveSInteger16(void)=0;
		virtual Stream::int32 receiveSInteger32(void)=0;
		virtual Stream::int64 receiveSInteger64(void)=0;

		virtual Stream::float32 receiveFloat32(void)=0;
		virtual Stream::float64 receiveFloat64(void)=0;
/*
		virtual const char* receiveASCIIString(void)=0;
		virtual const Stream::uint8* receiveUTF8String(void)=0;
		virtual const Stream::uint16* receiveUTF16String(void)=0;
*/
	};

	extern Stream_API Stream::IInputStreamHelper* createInputStreamHelper(Stream::IInputStream& rInputStream);
};

#endif // __Stream_IInputStreamHelper_H__
