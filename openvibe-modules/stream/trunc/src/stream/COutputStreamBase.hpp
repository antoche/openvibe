#ifndef __Stream_COutputStreamBaseBase_HPP__
#define __Stream_COutputStreamBaseBase_HPP__

#include "IOutputStream.h"

namespace Stream
{
	class COutputStreamBase : public Stream::IOutputStream
	{
	public:

		COutputStreamBase(void);
		virtual ~COutputStreamBase(void);

		virtual Stream::boolean setBufferSize(Stream::uint64 ui64BufferSize);

		virtual Stream::uint64 getBufferSize(void);
		virtual Stream::uint64 getBufferFill(void);

		virtual Stream::boolean isFinished(void);

		virtual Stream::uint64 sendBuffer(Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize);

	protected:

		virtual Stream::boolean requestFlush(void);

		virtual Stream::uint64 flush(Stream::uint8* pBuffer, Stream::uint64 ui64RequestedBufferSize)=0;

	private:

		template <typename T>
		T receiveUInteger(void);

	protected:

		Stream::EEndianness m_eEndianness;
		Stream::uint8* m_pBuffer;
		Stream::uint64 m_ui64BufferSize;
		Stream::uint64 m_ui64BufferFill;
	};
};

#endif // __Stream_COutputStreamBase_HPP__
