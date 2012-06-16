#include "CReaderHelper.h"

using namespace EBML;

CReaderHelper::CReaderHelper(void)
	:m_pReaderHelperImplementation(NULL)
{
	m_pReaderHelperImplementation=createReaderHelper();
}

CReaderHelper::~CReaderHelper(void)
{
	m_pReaderHelperImplementation->release();
}

uint64 CReaderHelper::getUIntegerFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	return m_pReaderHelperImplementation->getUIntegerFromChildData(pBuffer, ui64BufferSize);
}

int64 CReaderHelper::getSIntegerFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	return m_pReaderHelperImplementation->getSIntegerFromChildData(pBuffer, ui64BufferSize);
}

float64 CReaderHelper::getFloatFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	return m_pReaderHelperImplementation->getFloatFromChildData(pBuffer, ui64BufferSize);
}

const char* CReaderHelper::getASCIIStringFromChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	return m_pReaderHelperImplementation->getASCIIStringFromChildData(pBuffer, ui64BufferSize);
}

void CReaderHelper::release(void)
{
}
