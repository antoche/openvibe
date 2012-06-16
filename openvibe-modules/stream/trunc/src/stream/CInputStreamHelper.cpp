#include "CInputStreamHelper.hpp"
#include <system/Memory.h>

using namespace Stream;

// ________________________________________________________________________________________________________________
//

CInputStreamHelper::CInputStreamHelper(IInputStream& rInputStream)
	:m_rInputStream(rInputStream)
	,m_eEndianness(Endianness_None)
{
}

CInputStreamHelper::~CInputStreamHelper(void)
{
}

// ________________________________________________________________________________________________________________
//

boolean CInputStreamHelper::setBufferSize(uint64 ui64BufferSize)
{
	return m_rInputStream.setBufferSize(ui64BufferSize);
}

uint64 CInputStreamHelper::getBufferSize(void)
{
	return m_rInputStream.getBufferSize();
}

uint64 CInputStreamHelper::getBufferFill(void)
{
	return m_rInputStream.getBufferFill();
}

// ________________________________________________________________________________________________________________
//

boolean CInputStreamHelper::setEndianness(const EEndianness& eEndianness)
{
	if(m_rInputStream.isOpened())
	{
		return false;
	}

	m_eEndianness=eEndianness;
	return true;
}

EEndianness CInputStreamHelper::getEndianness(void)
{
	return m_eEndianness;
}

// ________________________________________________________________________________________________________________
//

boolean CInputStreamHelper::open(void)
{
	return m_rInputStream.open();
}

boolean CInputStreamHelper::isOpened(void)
{
	return m_rInputStream.isOpened();
}

boolean CInputStreamHelper::isFinished(void)
{
	return m_rInputStream.isFinished();
}

boolean CInputStreamHelper::close(void)
{
	return m_rInputStream.close();
}

// ________________________________________________________________________________________________________________
//

uint64 CInputStreamHelper::receiveBuffer(uint8* pBuffer, const uint64 ui64BufferSize)
{
	return m_rInputStream.receiveBuffer(pBuffer, ui64BufferSize);
}

// ________________________________________________________________________________________________________________
//

uint8 CInputStreamHelper::receiveUInteger8(void)
{
	return receiveUInteger<uint8>();
}

uint16 CInputStreamHelper::receiveUInteger16(void)
{
	return receiveUInteger<uint16>();
}

uint32 CInputStreamHelper::receiveUInteger32(void)
{
	return receiveUInteger<uint32>();
}

uint64 CInputStreamHelper::receiveUInteger64(void)
{
	return receiveUInteger<uint32>();
}

// ________________________________________________________________________________________________________________
//

int8 CInputStreamHelper::receiveSInteger8(void)
{
	return (int8)receiveUInteger<uint8>();
}

int16 CInputStreamHelper::receiveSInteger16(void)
{
	return (int16)receiveUInteger<uint16>();
}

int32 CInputStreamHelper::receiveSInteger32(void)
{
	return (int32)receiveUInteger<uint32>();
}

int64 CInputStreamHelper::receiveSInteger64(void)
{
	return (int64)receiveUInteger<uint64>();
}

// ________________________________________________________________________________________________________________
//

float32 CInputStreamHelper::receiveFloat32(void)
{
	uint32 l_uiResult=receiveUInteger<uint32>();
	float32 l_fResult;
	System::Memory::copy(&l_fResult, &l_uiResult, sizeof(l_fResult));
	return l_fResult;
}

float64 CInputStreamHelper::receiveFloat64(void)
{
	uint64 l_uiResult=receiveUInteger<uint64>();
	float64 l_fResult;
	System::Memory::copy(&l_fResult, &l_uiResult, sizeof(l_fResult));
	return l_fResult;
}

// ________________________________________________________________________________________________________________
//
/*
const char* CInputStreamHelper::receiveASCIIString(void)
{
	return NULL;
}

const uint8* CInputStreamHelper::receiveUTF8String(void)
{
	return NULL;
}

const uint16* CInputStreamHelper::receiveUTF16String(void)
{
	return NULL;
}
*/
// ________________________________________________________________________________________________________________
//

template <typename T>
T CInputStreamHelper::receiveUInteger(void)
{
	T l_tResult=0;
	uint8 l_pBuffer[sizeof(T)];
	if(m_rInputStream.receiveBuffer(l_pBuffer, sizeof(T))==sizeof(T))
	{
		for(unsigned int i=0; i<sizeof(T); i++)
		{
			l_tResult<<=8;
			l_tResult+=l_pBuffer[i];
		}
	}
	return l_tResult;
}

// ________________________________________________________________________________________________________________
//

IInputStreamHelper* Stream::createInputStreamHelper(IInputStream& rInputStream)
{
	return new CInputStreamHelper(rInputStream);
}
