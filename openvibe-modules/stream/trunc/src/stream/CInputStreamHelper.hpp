#ifndef __Stream_CInputStreamHelper_HPP__
#define __Stream_CInputStreamHelper_HPP__

#include "IInputStreamHelper.h"

namespace Stream
{
	class CInputStreamHelper : public Stream::IInputStreamHelper
	{
	public:

		CInputStreamHelper(Stream::IInputStream& rInputStream);
		virtual ~CInputStreamHelper(void);

		virtual Stream::boolean setBufferSize(Stream::uint64 ui64BufferSize);

		virtual Stream::uint64 getBufferSize(void);
		virtual Stream::uint64 getBufferFill(void);

		virtual Stream::boolean open(void);
		virtual Stream::boolean isOpened(void);
		virtual Stream::boolean isFinished(void);
		virtual Stream::boolean close(void);

		virtual Stream::uint64 receiveBuffer(Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize);

		virtual Stream::boolean setEndianness(const Stream::EEndianness& eEndianness);
		virtual Stream::EEndianness getEndianness(void);

		virtual Stream::uint8 receiveUInteger8(void);
		virtual Stream::uint16 receiveUInteger16(void);
		virtual Stream::uint32 receiveUInteger32(void);
		virtual Stream::uint64 receiveUInteger64(void);

		virtual Stream::int8 receiveSInteger8(void);
		virtual Stream::int16 receiveSInteger16(void);
		virtual Stream::int32 receiveSInteger32(void);
		virtual Stream::int64 receiveSInteger64(void);

		virtual Stream::float32 receiveFloat32(void);
		virtual Stream::float64 receiveFloat64(void);
/*
		virtual const char* receiveASCIIString(void);
		virtual const Stream::uint8* receiveUTF8String(void);
		virtual const Stream::uint16* receiveUTF16String(void);
*/
	private:

		CInputStreamHelper(void);

		template <typename T>
		T receiveUInteger(void);

	protected:

		Stream::IInputStream& m_rInputStream;
		Stream::EEndianness m_eEndianness;
	};
};

#endif // __Stream_CInputStreamHelper_HPP__
