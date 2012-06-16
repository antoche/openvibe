#ifndef __Stream_IInputStream_H__
#define __Stream_IInputStream_H__

#include "defines.h"

namespace Stream
{
	class Stream_API IInputStream
	{
	public:

		virtual Stream::boolean setBufferSize(Stream::uint64 ui64BufferSize)=0;

		virtual Stream::uint64 getBufferSize(void)=0;
		virtual Stream::uint64 getBufferFill(void)=0;

		virtual Stream::boolean open(void)=0;
		virtual Stream::boolean isOpened(void)=0;
		virtual Stream::boolean isFinished(void)=0;
		virtual Stream::boolean close(void)=0;

		virtual Stream::uint64 receiveBuffer(Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize)=0;

	protected:

		virtual ~IInputStream(void) { }
	};

	extern Stream_API Stream::IInputStream* createInputStreamFile(const char* sFilename);
	extern Stream_API Stream::IInputStream* createInputStreamMemory(const Stream::uint8* pBuffer, const Stream::uint64 ui64BufferSize);
};

#endif // __Stream_IInputStream_H__
