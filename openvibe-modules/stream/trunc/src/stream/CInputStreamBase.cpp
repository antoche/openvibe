#include "CInputStreamBase.hpp"

#include <system/Memory.h>

using namespace Stream;

#define _Default_BufferSize_ 512

// ________________________________________________________________________________________________________________
//

CInputStreamBase::CInputStreamBase(void)
	:m_pBuffer(NULL)
	,m_ui64BufferSize(_Default_BufferSize_)
	,m_ui64BufferFill(0)
	,m_ui64BufferIndex(0)
{
	m_pBuffer=new uint8[_Default_BufferSize_];
}

CInputStreamBase::~CInputStreamBase(void)
{
	delete [] m_pBuffer;
}

// ________________________________________________________________________________________________________________
//

boolean CInputStreamBase::setBufferSize(uint64 ui64BufferSize)
{
	if(this->isOpened())
	{
		return false;
	}

	if(ui64BufferSize==0)
	{
		return false;
	}

	if(ui64BufferSize==m_ui64BufferSize)
	{
		return true;
	}

	uint8* l_pBuffer=new uint8[static_cast<unsigned int>(ui64BufferSize)];
	if(l_pBuffer==NULL)
	{
		return false;
	}

	delete [] m_pBuffer;
	m_pBuffer=l_pBuffer;
	m_ui64BufferSize=ui64BufferSize;
	m_ui64BufferFill=0;
	m_ui64BufferIndex=0;
	return true;
}

// ________________________________________________________________________________________________________________
//

uint64 CInputStreamBase::getBufferSize(void)
{
	return m_ui64BufferSize;
}

uint64 CInputStreamBase::getBufferFill(void)
{
	return m_ui64BufferFill-m_ui64BufferIndex;
}

boolean CInputStreamBase::isFinished(void)
{
	if(!this->isOpened())
	{
		return true;
	}

	if(m_ui64BufferFill-m_ui64BufferIndex>0)
	{
		return false;
	}

	// m_ui64BufferFill should be m_ui64BufferIndex, so we have to bufferize once again
	m_ui64BufferFill=this->bufferize(m_pBuffer, m_ui64BufferSize);
	m_ui64BufferIndex=0;
	return m_ui64BufferFill==0;
}

// ________________________________________________________________________________________________________________
//

uint64 CInputStreamBase::receiveBuffer(uint8* pBuffer, const uint64 ui64BufferSize)
{
	uint64 l_ui64BufferMissing=ui64BufferSize; // what's missing until out buffer is filled
	boolean l_bFinished=false;
	while(!l_bFinished)
	{
		uint64 l_ui64BufferLeft=m_ui64BufferFill-m_ui64BufferIndex; // what's left in pre-buffered
		uint8* l_pBuffer=pBuffer+ui64BufferSize-l_ui64BufferMissing;

		if(l_ui64BufferLeft!=0)
		{
			uint64 l_ui64ByteCount=(l_ui64BufferLeft<l_ui64BufferMissing?l_ui64BufferLeft:l_ui64BufferMissing);
			System::Memory::copy(l_pBuffer, m_pBuffer+m_ui64BufferIndex, l_ui64ByteCount);
			m_ui64BufferIndex+=l_ui64ByteCount;
			l_ui64BufferMissing-=l_ui64ByteCount;
			l_bFinished|=(l_ui64BufferMissing==0);
		}
		else
		{
			m_ui64BufferFill=this->bufferize(m_pBuffer, m_ui64BufferSize);
			m_ui64BufferIndex=0;
			l_bFinished|=(m_ui64BufferFill==0);
		}
	}
	return ui64BufferSize-l_ui64BufferMissing;
}
