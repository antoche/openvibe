#include "COutputStreamFile.hpp"

using namespace Stream;

COutputStreamFile::COutputStreamFile(const char* sFilename)
	:m_sFilename(sFilename)
{
}

boolean COutputStreamFile::open(void)
{
	if(m_oFile.is_open())
	{
		return false;
	}
	m_oFile.open(m_sFilename.c_str(), std::ios::binary);
	if(!m_oFile.is_open())
	{
		return false;
	}
	return true;
}

boolean COutputStreamFile::isOpened(void)
{
	return m_oFile.is_open();
}

boolean COutputStreamFile::close(void)
{
	if(!m_oFile.is_open())
	{
		return false;
	}
	if(!this->requestFlush())
	{
		return false;
	}
	m_oFile.close();
	return true;
}

uint64 COutputStreamFile::flush(uint8* pBuffer, uint64 ui64RequestedBufferSize)
{
	if(m_oFile.is_open())
	{
		m_oFile.write(reinterpret_cast<char*>(pBuffer), static_cast<std::streamsize>(ui64RequestedBufferSize));
		return ui64RequestedBufferSize; // $$$ Potential bug : what has effectively been written is not tested
	}
	return 0;
}

IOutputStream* Stream::createOutputStreamFile(const char* sFilename)
{
	return new COutputStreamFile(sFilename);
}
