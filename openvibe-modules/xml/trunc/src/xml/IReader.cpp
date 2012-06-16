#include "IReader.h"

#include <expat.h>
#include <string>

using namespace XML;
using namespace std;

namespace XML
{
	class CReader : public IReader
	{
	public:
		CReader(IReaderCallback& rReaderCallback);

		virtual boolean processData(const void* pBuffer, const uint64 ui64BufferSize);
		virtual void release(void);

		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount);
		virtual void processChildData(const char* sData);
		virtual void closeChild(void);

	protected:

		IReaderCallback& m_rReaderCallback;
		::XML_Parser m_pXMLParser;
		std::string m_sData;
	};

	static void XMLCALL expat_xml_start(void* pData, const char* pElement, const char** ppAttribute);
	static void XMLCALL expat_xml_end(void* pData, const char* pElement);
	static void XMLCALL expat_xml_data(void* pData, const char* pDataValue, int iDataLength);
};

CReader::CReader(IReaderCallback& rReaderCallback)
	:m_rReaderCallback(rReaderCallback)
	,m_pXMLParser(NULL)
{
	m_pXMLParser=XML_ParserCreate(NULL);
	XML_SetElementHandler(m_pXMLParser, expat_xml_start, expat_xml_end);
	XML_SetCharacterDataHandler(m_pXMLParser, expat_xml_data);
	XML_SetUserData(m_pXMLParser, this);
}

boolean CReader::processData(const void* pBuffer, const uint64 ui64BufferSize)
{
	// $$$ TODO take 64bits size into consideration
	XML_Status l_eStatus=XML_Parse(
		m_pXMLParser,
		static_cast<const char*>(pBuffer),
		static_cast<const int>(ui64BufferSize),
		false);
	return (l_eStatus==XML_STATUS_OK);
}

void CReader::release(void)
{
	delete this;
}

void CReader::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount)
{
	m_rReaderCallback.openChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount);
	m_sData="";
}

void CReader::processChildData(const char* sData)
{
	m_sData+=sData;
}

void CReader::closeChild(void)
{
	if(m_sData.size()!=0)
	{
		m_rReaderCallback.processChildData(m_sData.c_str());
	}
	m_sData="";
	m_rReaderCallback.closeChild();
}

XML_API IReader* XML::createReader(IReaderCallback& rReaderCallback)
{
	return new CReader(rReaderCallback);
}

static void XMLCALL XML::expat_xml_start(void* pData, const char* pElement, const char** ppAttribute)
{
	uint64 i, l_ui64AttributeCount=0;
	while(ppAttribute[l_ui64AttributeCount++]);
	l_ui64AttributeCount>>=1;

	// $$$ TODO take 64bits size into consideration
	const char** l_pAttributeName=new const char*[static_cast<size_t>(l_ui64AttributeCount)];
	const char** l_pAttributeValue=new const char*[static_cast<size_t>(l_ui64AttributeCount)];

	for(i=0; i<l_ui64AttributeCount; i++)
	{
		l_pAttributeName[i]=ppAttribute[(i<<1)];
		l_pAttributeValue[i]=ppAttribute[(i<<1)+1];
	}

	static_cast<CReader*>(pData)->openChild(pElement, l_pAttributeName, l_pAttributeValue, l_ui64AttributeCount);

	delete [] l_pAttributeName;
	delete [] l_pAttributeValue;
}

static void XMLCALL XML::expat_xml_end(void* pData, const char* pElement)
{
	static_cast<CReader*>(pData)->closeChild();
}

static void XMLCALL XML::expat_xml_data(void* pData, const char* pDataValue, int iDataLength)
{
	string sData(pDataValue, iDataLength);
	static_cast<CReader*>(pData)->processChildData(sData.c_str());
}
