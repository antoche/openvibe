#include "COutputStreamHelper.hpp"
#include <system/Memory.h>

using namespace Stream;

// ________________________________________________________________________________________________________________
//

COutputStreamHelper::COutputStreamHelper(IOutputStream& rOutputStream)
	:m_rOutputStream(rOutputStream)
	,m_eEndianness(Endianness_None)
{
}

COutputStreamHelper::~COutputStreamHelper(void)
{
}

// ________________________________________________________________________________________________________________
//

boolean COutputStreamHelper::setBufferSize(uint64 ui64BufferSize)
{
	return m_rOutputStream.setBufferSize(ui64BufferSize);
}

uint64 COutputStreamHelper::getBufferSize(void)
{
	return m_rOutputStream.getBufferSize();
}

uint64 COutputStreamHelper::getBufferFill(void)
{
	return m_rOutputStream.getBufferFill();
}

// ________________________________________________________________________________________________________________
//

boolean COutputStreamHelper::setEndianness(const EEndianness& eEndianness)
{
	if(m_rOutputStream.isOpened())
	{
		return false;
	}

	m_eEndianness=eEndianness;
	return true;
}

EEndianness COutputStreamHelper::getEndianness(void)
{
	return m_eEndianness;
}

// ________________________________________________________________________________________________________________
//

boolean COutputStreamHelper::open(void)
{
	return m_rOutputStream.open();
}

boolean COutputStreamHelper::isOpened(void)
{
	return m_rOutputStream.isOpened();
}

boolean COutputStreamHelper::isFinished(void)
{
	return m_rOutputStream.isFinished();
}

boolean COutputStreamHelper::close(void)
{
	return m_rOutputStream.close();
}

// ________________________________________________________________________________________________________________
//

uint64 COutputStreamHelper::sendBuffer(uint8* pBuffer, const uint64 ui64BufferSize)
{
	return m_rOutputStream.sendBuffer(pBuffer, ui64BufferSize);
}

// ________________________________________________________________________________________________________________
//

boolean COutputStreamHelper::sendUInteger8(uint8 uiValue)
{
	return sendUInteger<uint8>(uiValue);
}

boolean COutputStreamHelper::sendUInteger16(uint16 uiValue)
{
	return sendUInteger<uint16>(uiValue);
}

boolean COutputStreamHelper::sendUInteger32(uint32 uiValue)
{
	return sendUInteger<uint32>(uiValue);
}

boolean COutputStreamHelper::sendUInteger64(uint64 uiValue)
{
	return sendUInteger<uint64>(uiValue);
}

// ________________________________________________________________________________________________________________
//

boolean COutputStreamHelper::sendSInteger8(int8 iValue)
{
	return (int8)sendUInteger<uint8>(iValue);
}

boolean COutputStreamHelper::sendSInteger16(int16 iValue)
{
	return (int16)sendUInteger<uint16>(iValue);
}

boolean COutputStreamHelper::sendSInteger32(int32 iValue)
{
	return (int32)sendUInteger<uint32>(iValue);
}

boolean COutputStreamHelper::sendSInteger64(int64 iValue)
{
	return (int64)sendUInteger<uint64>(iValue);
}

// ________________________________________________________________________________________________________________
//


boolean COutputStreamHelper::sendFloat32(float32 fValue)
{
	uint32 l_uiValue;
	System::Memory::copy(&l_uiValue, &fValue, sizeof(l_uiValue));
	return sendUInteger<uint32>(l_uiValue);
}

boolean COutputStreamHelper::sendFloat64(float64 fValue)
{
	uint64 l_uiValue;
	System::Memory::copy(&l_uiValue, &fValue, sizeof(l_uiValue));
	return sendUInteger<uint64>(l_uiValue);
}

// ________________________________________________________________________________________________________________
//

boolean COutputStreamHelper::sendASCIIString(const char* sString)
{
	const char* l_sString=sString;
	while(*l_sString)
	{
		if(!sendUInteger<uint8>(*l_sString))
		{
			return false;
		}
		l_sString++;
	}
	return sendUInteger<uint8>(0);
}

boolean COutputStreamHelper::sendUTF8String(const uint8* sString)
{
	const uint8* l_sString=sString;
	while(*l_sString)
	{
		if(!sendUInteger<uint8>(*l_sString))
		{
			return false;
		}
		l_sString++;
	}
	return sendUInteger<uint8>(0);
}

boolean COutputStreamHelper::sendUTF16String(const uint16* sString)
{
	const uint16* l_sString=sString;
	while(*l_sString)
	{
		if(!sendUInteger<uint16>(*l_sString))
		{
			return false;
		}
		l_sString++;
	}
	return sendUInteger<uint16>(0);
}

// ________________________________________________________________________________________________________________
//

template <typename T>
boolean COutputStreamHelper::sendUInteger(const T& rValue)
{
	uint8 l_pBuffer[sizeof(T)];
	for(unsigned int i=0; i<sizeof(T); i++)
	{
		l_pBuffer[i]=(uint8)(((rValue>>(8*(sizeof(T)-i-1)))&0xff));
	}
	return (m_rOutputStream.sendBuffer(l_pBuffer, sizeof(T))==sizeof(T)?true:false);
}

// ________________________________________________________________________________________________________________
//

IOutputStreamHelper* Stream::createOutputStreamHelper(IOutputStream& rOutputStream)
{
	return new COutputStreamHelper(rOutputStream);
}
