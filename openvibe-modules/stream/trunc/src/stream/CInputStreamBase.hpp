#ifndef __Stream_CInputStreamBaseBase_HPP__
#define __Stream_CInputStreamBaseBase_HPP__

#include "IInputStream.h"

namespace Stream
{
	class CInputStreamBase : public Stream::IInputStream
	{
	public:

		CInputStreamBase(void);
		virtual ~CInputStreamBase(void);

		virtual Stream::boolean setBufferSize(Stream::uint64 ui64BufferSize);

		virtual Stream::uint64 getBufferSize(void);
		virtual Stream::uint64 getBufferFill(void);

		virtual Stream::boolean isFinished(void);

		virtual Stream::uint64 receiveBuffer(Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize);

	protected:

		virtual Stream::uint64 bufferize(Stream::uint8* pBuffer, Stream::uint64 ui64RequestedBufferSize)=0;

	private:

		template <typename T>
		T receiveUInteger(void);

	protected:

		Stream::EEndianness m_eEndianness;
		Stream::uint8* m_pBuffer;
		Stream::uint64 m_ui64BufferSize;
		Stream::uint64 m_ui64BufferFill;
		Stream::uint64 m_ui64BufferIndex;
	};
};

#endif // __Stream_CInputStreamBase_HPP__
