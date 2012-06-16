#ifndef __Stream_COutputStreamFile_HPP__
#define __Stream_COutputStreamFile_HPP__

#include "COutputStreamBase.hpp"

#include <fstream>
#include <string>

namespace Stream
{
	class COutputStreamFile : public Stream::COutputStreamBase
	{
	public:

		COutputStreamFile(const char* sFilename);

		virtual Stream::boolean open(void);
		virtual Stream::boolean isOpened(void);
		virtual Stream::boolean close(void);

		virtual Stream::uint64 flush(Stream::uint8* pBuffer, Stream::uint64 ui64RequestedBufferSize);

	private:

		COutputStreamFile(void);

	protected:

		std::ofstream m_oFile;
		std::string m_sFilename;
	};
};

#endif // __Stream_COutputStreamFile_HPP__
