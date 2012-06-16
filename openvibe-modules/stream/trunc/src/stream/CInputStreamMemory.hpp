#ifndef __Stream_CInputStreamMemory_HPP__
#define __Stream_CInputStreamMemory_HPP__

#include "CInputStreamBase.hpp"

namespace Stream
{
	class CInputStreamMemory : public Stream::CInputStreamBase
	{
	public:

		CInputStreamMemory(const Stream::uint8* pStream, Stream::uint64 ui64StreamSize);

		virtual Stream::boolean open(void);
		virtual Stream::boolean isOpened(void);
		virtual Stream::boolean close(void);

		virtual Stream::uint64 bufferize(Stream::uint8* pBuffer, Stream::uint64 ui64RequestedBufferSize);

	private:

		CInputStreamMemory(void);

	protected:

		const Stream::uint8* m_pStream;
		Stream::uint64 m_ui64StreamSize;
		Stream::uint64 m_ui64StreamIndex;
		Stream::boolean m_bIsOpened;
	};
};

#endif // __Stream_CInputStreamMemory_HPP__
