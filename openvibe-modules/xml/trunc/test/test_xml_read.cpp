#include <cstdlib>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "xml/IReader.h"

class CSampleReaderCallback : public XML::IReaderCallback
{
public:

	CSampleReaderCallback(void)
		:m_ui64Indent(0)
	{
	}

	void indent(void)
	{
		for(XML::uint64 i=0; i<m_ui64Indent; i++)
		{
			std::cout << "  ";
		}
	}

	virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
	{
		this->indent();
		std::cout << "opened node " << sName << (ui64AttributeCount?" with attributes : ":" with no attribute");
		for(XML::uint64 i=0; i<ui64AttributeCount; i++)
		{
			std::cout << "[" << sAttributeName[i] << "=" << sAttributeValue[i] << "]";
		}
		std::cout << "\n";
		m_ui64Indent++;
	}

	virtual void processChildData(const char* sData)
	{
		this->indent();
		std::cout << "data for this node is [" << sData << "]\n";
	}

	virtual void closeChild(void)
	{
		m_ui64Indent--;
		this->indent();
		std::cout << "closed node\n";
	}

	XML::uint64 m_ui64Indent;
};

int main(int argc, char** argv)
{
	if(argc<2)
	{
		printf("syntax : %s fileout.xml\n", argv[0]);
		return -1;
	}

	CSampleReaderCallback l_oSampleReaderCallback;
	XML::IReader* l_pReader=XML::createReader(l_oSampleReaderCallback);

	FILE* l_pFile=fopen(argv[1], "rb");
	if(l_pFile)
	{
		char l_pBuffer[1024];
		while(!feof(l_pFile))
		{
			size_t len=fread(l_pBuffer, 1, sizeof(l_pBuffer), l_pFile);
			l_pReader->processData(l_pBuffer, len);
		}
		fclose(l_pFile);
	}

	l_pReader->release();
	l_pReader=NULL;

	return 0;
}
