#ifndef __Stream_IOutputStream_H__
#define __Stream_IOutputStream_H__

#include "defines.h"

namespace Stream
{
	class Stream_API IOutputStream
	{
	public:

		virtual Stream::boolean setBufferSize(Stream::uint64 ui64BufferSize)=0;

		virtual Stream::uint64 getBufferSize(void)=0;
		virtual Stream::uint64 getBufferFill(void)=0;

		virtual Stream::boolean open(void)=0;
		virtual Stream::boolean isOpened(void)=0;
		virtual Stream::boolean isFinished(void)=0;
		virtual Stream::boolean close(void)=0;

		virtual Stream::uint64 sendBuffer(Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize)=0;

	protected:

		virtual ~IOutputStream(void) { }
	};

	extern Stream_API Stream::IOutputStream* createOutputStreamFile(const char* sFilename);
	extern Stream_API Stream::IOutputStream* createOutputStreamMemory(Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize);
};

#endif // __Stream_IOutputStream_H__
