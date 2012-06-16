#include "IReaderHelper.h"

#include <string>
#include <cstring>
#include <cstdlib>

using namespace EBML;
using namespace std;

namespace EBML
{
	namespace
	{
		class CReaderHelper : public IReaderHelper
		{
		public:
			CReaderHelper(void);

			virtual uint64 getUIntegerFromChildData(const void* pBuffer, const uint64 ui64BufferSize);
			virtual int64 getSIntegerFromChildData(const void* pBuffer, const uint64 ui64BufferSize);
			virtual float64 getFloatFromChildData(const void* pBuffer, const uint64 ui64BufferSize);
			virtual const char* getASCIIStringFromChildData(const void* pBuffer, const uint64 ui64BufferSize);

			virtual void release(void);

			std::string m_sASCIIString;
		};
	};
};

CReaderHelper::CReaderHelper(void)
{
}

uint64 CReaderHelper::getUIntegerFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	uint64 l_ui64Result=0;
	uint64 i;
	for(i=0; i<ui64BufferSize; i++)
	{
		l_ui64Result<<=8;
		l_ui64Result|=((unsigned char*)pBuffer)[i];
	}
	return l_ui64Result;
}

int64 CReaderHelper::getSIntegerFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	int64 l_i64Result=0;
	uint64 i;

	if(ui64BufferSize!=0 && ((unsigned char*)pBuffer)[0]&0x80)
	{
		l_i64Result=-1;
	}

	for(i=0; i<ui64BufferSize; i++)
	{
		l_i64Result<<=8;
		l_i64Result|=((unsigned char*)pBuffer)[i];
	}
	return l_i64Result;
}

float64 CReaderHelper::getFloatFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	float32 l_f32Result;
	float64 l_f64Result;
	int32 l_ui32Result;
	int64 l_ui64Result;

	switch(ui64BufferSize)
	{
		case 0:
			l_f64Result=0;
			break;

		case 4:
			l_ui32Result=(uint32)getUIntegerFromChildData(pBuffer, ui64BufferSize);
			::memcpy(&l_f32Result, &l_ui32Result, sizeof(l_f32Result));
			l_f64Result=l_f32Result;
			break;

		case 8:
			l_ui64Result=(uint64)getUIntegerFromChildData(pBuffer, ui64BufferSize);
			::memcpy(&l_f64Result, &l_ui64Result, sizeof(l_f64Result));
			break;

		case 10:
			l_f64Result=0;
			break;

		default:
			l_f64Result=0;
			break;
	}

	return l_f64Result;
}

const char* CReaderHelper::getASCIIStringFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	if(ui64BufferSize)
	{
		m_sASCIIString.assign((char*)pBuffer, (size_t)(ui64BufferSize));
	}
	else
	{
		m_sASCIIString="";
	}
	return m_sASCIIString.c_str();
}

void CReaderHelper::release(void)
{
	delete this;
}

EBML_API IReaderHelper* EBML::createReaderHelper(void)
{
	return new CReaderHelper();
}
