#ifndef __Stream_CInputStreamFile_HPP__
#define __Stream_CInputStreamFile_HPP__

#include "CInputStreamBase.hpp"

#include <fstream>
#include <string>

namespace Stream
{
	class CInputStreamFile : public Stream::CInputStreamBase
	{
	public:

		CInputStreamFile(const char* sFilename);

		virtual Stream::boolean open(void);
		virtual Stream::boolean isOpened(void);
		virtual Stream::boolean close(void);

		virtual Stream::uint64 bufferize(Stream::uint8* pBuffer, Stream::uint64 ui64RequestedBufferSize);

	private:

		CInputStreamFile(void);

	protected:

		std::ifstream m_oFile;
		std::string m_sFilename;
	};
};

#endif // __Stream_CInputStreamFile_HPP__
