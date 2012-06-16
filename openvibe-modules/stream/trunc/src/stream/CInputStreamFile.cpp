#include "CInputStreamFile.hpp"

using namespace Stream;

CInputStreamFile::CInputStreamFile(const char* sFilename)
	:m_sFilename(sFilename)
{
}

boolean CInputStreamFile::open(void)
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

boolean CInputStreamFile::isOpened(void)
{
	return m_oFile.is_open();
}

boolean CInputStreamFile::close(void)
{
	if(!m_oFile.is_open())
	{
		return false;
	}
	m_oFile.close();
	return true;
}

uint64 CInputStreamFile::bufferize(uint8* pBuffer, uint64 ui64RequestedBufferSize)
{
	if(m_oFile.is_open())
	{
		m_oFile.read(reinterpret_cast<char*>(pBuffer), static_cast<std::streamsize>(ui64RequestedBufferSize));
		uint64 l_ui64Result=m_oFile.gcount();
		return l_ui64Result;
	}
	return 0;
}

IInputStream* Stream::createInputStreamFile(const char* sFilename)
{
	return new CInputStreamFile(sFilename);
}
