#include "CWriterHelper.h"

using namespace EBML;

CWriterHelper::CWriterHelper(void)
	:m_pWriterHelperImplementation(NULL)
{
	m_pWriterHelperImplementation=createWriterHelper();
}

CWriterHelper::~CWriterHelper(void)
{
	m_pWriterHelperImplementation->release();
}

boolean CWriterHelper::connect(IWriter* pWriter)
{
	return m_pWriterHelperImplementation->connect(pWriter);
}

boolean CWriterHelper::disconnect(void)
{
	return m_pWriterHelperImplementation->disconnect();
}

boolean CWriterHelper::openChild(const CIdentifier& rIdentifier)
{
	return m_pWriterHelperImplementation->openChild(rIdentifier);
}

boolean CWriterHelper::closeChild(void)
{
	return m_pWriterHelperImplementation->closeChild();
}

boolean CWriterHelper::setSIntegerAsChildData(const int64 iValue)
{
	return m_pWriterHelperImplementation->setSIntegerAsChildData(iValue);
}

boolean CWriterHelper::setUIntegerAsChildData(const uint64 uiValue)
{
	return m_pWriterHelperImplementation->setUIntegerAsChildData(uiValue);
}

boolean CWriterHelper::setFloat32AsChildData(const float32 fValue)
{
	return m_pWriterHelperImplementation->setFloat32AsChildData(fValue);
}

boolean CWriterHelper::setFloat64AsChildData(const float64 fValue)
{
	return m_pWriterHelperImplementation->setFloat64AsChildData(fValue);
}

boolean CWriterHelper::setBinaryAsChildData(const void* pBuffer, const uint64 ui64BufferLength)
{
	return m_pWriterHelperImplementation->setBinaryAsChildData(pBuffer, ui64BufferLength);
}

boolean CWriterHelper::setASCIIStringAsChildData(const char* sValue)
{
	return m_pWriterHelperImplementation->setASCIIStringAsChildData(sValue);
}

void CWriterHelper::release(void)
{
}
