#include "COutputStreamMemory.hpp"

#include <system/Memory.h>

using namespace Stream;

COutputStreamMemory::COutputStreamMemory(uint8* pStream, const uint64 ui64StreamSize)
	:m_pStream(pStream)
	,m_ui64StreamSize(ui64StreamSize)
	,m_ui64StreamIndex(0)
	,m_bIsOpened(false)
{
}

boolean COutputStreamMemory::open(void)
{
	if(m_bIsOpened)
	{
		return false;
	}
	m_bIsOpened=true;
	m_ui64StreamIndex=0;
	return true;
}

boolean COutputStreamMemory::isOpened(void)
{
	return m_bIsOpened;
}

boolean COutputStreamMemory::close(void)
{
	if(!m_bIsOpened)
	{
		return false;
	}
	m_bIsOpened=!this->requestFlush();
	return !m_bIsOpened;
}

uint64 COutputStreamMemory::flush(uint8* pBuffer, uint64 ui64RequestedBufferSize)
{
	uint64 l_ui64StreamLeft=m_ui64StreamSize-m_ui64StreamIndex;
	uint64 l_ui64BytesCount=(ui64RequestedBufferSize<l_ui64StreamLeft?ui64RequestedBufferSize:l_ui64StreamLeft);
	System::Memory::copy(m_pStream+m_ui64StreamIndex, pBuffer, l_ui64BytesCount);
	m_ui64StreamIndex+=l_ui64BytesCount;
	return l_ui64BytesCount;
}

IOutputStream* Stream::createOutputStreamMemory(uint8* pStream, const uint64 ui64StreamSize)
{
	return new COutputStreamMemory(pStream, ui64StreamSize);
}
