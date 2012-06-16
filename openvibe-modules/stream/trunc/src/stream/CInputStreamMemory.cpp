#include "CInputStreamMemory.hpp"

#include <system/Memory.h>
#include <cstdio>

using namespace Stream;

CInputStreamMemory::CInputStreamMemory(const uint8* pStream, uint64 ui64StreamSize)
	:m_pStream(pStream)
	,m_ui64StreamSize(ui64StreamSize)
	,m_ui64StreamIndex(0)
	,m_bIsOpened(false)
{
}

boolean CInputStreamMemory::open(void)
{
	if(m_bIsOpened)
	{
		return false;
	}
	m_bIsOpened=true;
	m_ui64StreamIndex=0;
	return true;
}

boolean CInputStreamMemory::isOpened(void)
{
	return m_bIsOpened;
}

boolean CInputStreamMemory::close(void)
{
	if(!m_bIsOpened)
	{
		return false;
	}
	m_bIsOpened=false;
	return true;
}

uint64 CInputStreamMemory::bufferize(uint8* pBuffer, uint64 ui64RequestedBufferSize)
{
	if(m_bIsOpened)
	{
		uint64 m_ui64StreamFillCount=m_ui64StreamSize-m_ui64StreamIndex;
		if(m_ui64StreamSize!=m_ui64StreamIndex)
		{
			uint64 l_ui64Count=(m_ui64StreamFillCount<ui64RequestedBufferSize?m_ui64StreamFillCount:ui64RequestedBufferSize);
			System::Memory::copy(pBuffer, m_pStream+m_ui64StreamIndex, l_ui64Count);
			m_ui64StreamIndex+=l_ui64Count;
			return l_ui64Count;
		}
	}
	return 0;
}

IInputStream* Stream::createInputStreamMemory(const uint8* pBuffer, const uint64 ui64BufferSize)
{
	return new CInputStreamMemory(pBuffer, ui64BufferSize);
}
