#ifndef __Stream_COutputStreamMemory_HPP__
#define __Stream_COutputStreamMemory_HPP__

#include "COutputStreamBase.hpp"

namespace Stream
{
	class COutputStreamMemory : public Stream::COutputStreamBase
	{
	public:

		COutputStreamMemory(Stream::uint8* pStream, const Stream::uint64 ui64StreamSize);

		virtual Stream::boolean open(void);
		virtual Stream::boolean isOpened(void);
		virtual Stream::boolean close(void);

		virtual Stream::uint64 flush(Stream::uint8* pBuffer, Stream::uint64 ui64RequestedBufferSize);

	private:

		COutputStreamMemory(void);

	protected:

		Stream::uint8* m_pStream;
		Stream::uint64 m_ui64StreamSize;
		Stream::uint64 m_ui64StreamIndex;
		Stream::boolean m_bIsOpened;
	};
};

#endif // __Stream_COutputStreamMemory_HPP__
