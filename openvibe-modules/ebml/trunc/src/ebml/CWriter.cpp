#include "CWriter.h"

using namespace EBML;

CWriter::CWriter(IWriterCallback& rWriterCallback)
	:m_pWriterImplementation(NULL)
{
	m_pWriterImplementation=createWriter(rWriterCallback);
}

CWriter::~CWriter(void)
{
	m_pWriterImplementation->release();
}

boolean CWriter::openChild(const CIdentifier& rIdentifier)
{
	return m_pWriterImplementation->openChild(rIdentifier);
}

boolean CWriter::setChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	return m_pWriterImplementation->setChildData(pBuffer, ui64BufferSize);
}

boolean CWriter::closeChild(void)
{
	return m_pWriterImplementation->closeChild();
}

void CWriter::release(void)
{
}
