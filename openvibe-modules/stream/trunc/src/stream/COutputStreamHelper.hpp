#ifndef __Stream_COutputStreamHelper_HPP__
#define __Stream_COutputStreamHelper_HPP__

#include "IOutputStreamHelper.h"

namespace Stream
{
	class COutputStreamHelper : public Stream::IOutputStreamHelper
	{
	public:

		COutputStreamHelper(Stream::IOutputStream& rOutputStream);
		virtual ~COutputStreamHelper(void);

		virtual Stream::boolean setBufferSize(Stream::uint64 ui64BufferSize);

		virtual Stream::uint64 getBufferSize(void);
		virtual Stream::uint64 getBufferFill(void);

		virtual Stream::boolean open(void);
		virtual Stream::boolean isOpened(void);
		virtual Stream::boolean isFinished(void);
		virtual Stream::boolean close(void);

		virtual Stream::uint64 sendBuffer(Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize);

		virtual Stream::boolean setEndianness(const Stream::EEndianness& eEndianness);
		virtual Stream::EEndianness getEndianness(void);

		virtual Stream::boolean sendUInteger8(Stream::uint8 uiValue);
		virtual Stream::boolean sendUInteger16(Stream::uint16 uiValue);
		virtual Stream::boolean sendUInteger32(Stream::uint32 uiValue);
		virtual Stream::boolean sendUInteger64(Stream::uint64 uiValue);

		virtual Stream::boolean sendSInteger8(Stream::int8 iValue);
		virtual Stream::boolean sendSInteger16(Stream::int16 iValue);
		virtual Stream::boolean sendSInteger32(Stream::int32 iValue);
		virtual Stream::boolean sendSInteger64(Stream::int64 iValue);

		virtual Stream::boolean sendFloat32(Stream::float32 fValue);
		virtual Stream::boolean sendFloat64(Stream::float64 fValue);

		virtual Stream::boolean sendASCIIString(const char* sString);
		virtual Stream::boolean sendUTF8String(const Stream::uint8* sString);
		virtual Stream::boolean sendUTF16String(const Stream::uint16* sString);

	private:

		COutputStreamHelper(void);

		template <typename T>
		Stream::boolean sendUInteger(const T& rValue);

	protected:

		Stream::IOutputStream& m_rOutputStream;
		Stream::EEndianness m_eEndianness;
	};
};

#endif // __Stream_COutputStreamHelper_HPP__
