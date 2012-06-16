#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "ebml/IReader.h"
#include "ebml/IReaderHelper.h"
#include "ebml/CReader.h"
#include "ebml/CReaderHelper.h"

using namespace std;

class CReaderCallBack : public EBML::IReaderCallBack
{
public:
	CReaderCallBack(void)
		:m_iDepth(0)
	{
	}

	virtual ~CReaderCallBack(void)
	{
	}

	virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier)
	{
		if(rIdentifier==EBML_Identifier_Header) return true;
		if(rIdentifier==EBML::CIdentifier(0xffff)) return true;

		return false;
	}

	virtual void openChild(const EBML::CIdentifier& rIdentifier)
	{
		m_oCurrentIdentifier=rIdentifier;
		EBML::uint64 l_ui64Identifier=rIdentifier;
		for(int i=0; i<m_iDepth; i++) cout << "   ";
		cout << "Opening child node [0x" << setw(16) << setfill('0') << hex << l_ui64Identifier << dec << "]\n";
		m_iDepth++;
	}

	virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
	{
		for(int i=0; i<m_iDepth; i++) cout << "   ";
		if(m_oCurrentIdentifier==EBML_Identifier_DocType)
			cout << "Got doc type : [" << m_oReaderHelper.getASCIIStringFromChildData(pBuffer, ui64BufferSize) << "]\n";
		else if(m_oCurrentIdentifier==EBML_Identifier_DocTypeVersion)
			cout << "Got doc type version : [0x" << setw(16) << setfill('0') << hex << m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << dec << "]\n";
		else if(m_oCurrentIdentifier==EBML_Identifier_DocTypeReadVersion)
			cout <<"Got doc type read version : [0x" << setw(16) << setfill('0') << hex << m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << dec << "]\n";
		else if(m_oCurrentIdentifier==EBML::CIdentifier(0x1234))
			cout <<"Got uinteger : [0x" << setw(16) << setfill('0') << hex << m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << dec << "]\n";
		else if(m_oCurrentIdentifier==EBML::CIdentifier(0xffffffffffffffffLL))
			cout <<"Got uinteger : [0x" << setw(16) << setfill('0') << hex << m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << dec << "]\n";
		else if(m_oCurrentIdentifier==EBML::CIdentifier(0x4321))
			cout <<"Got float : [" << m_oReaderHelper.getFloatFromChildData(pBuffer, ui64BufferSize) << "]\n";
		else if(m_oCurrentIdentifier==EBML::CIdentifier(0x8765))
			cout <<"Got float : [" << m_oReaderHelper.getFloatFromChildData(pBuffer, ui64BufferSize) << "]\n";
		else
			cout << "Got " << ui64BufferSize << " data bytes\n";
	}

	virtual void closeChild(void)
	{
		m_iDepth--;
		for(int i=0; i<m_iDepth; i++) cout << "   ";
		cout << "Node closed\n";
	}

	int m_iDepth;
	EBML::CReaderHelper m_oReaderHelper;
	EBML::CIdentifier m_oCurrentIdentifier;
};

int main(int argc, char** argv)
{
	srand(time(NULL));

	if(argc<2)
	{
		cout << "syntax : " << argv[0] << " filein.ebml\n";
		return -1;
	}

	// for(unsigned long n=17; n>=1; n--)
	unsigned long n=rand()&0xf;
	{
		CReaderCallBack cb;
		EBML::CReader l_oReader(cb);

		cout << "testing with n=" << n << endl;;
		FILE* f=fopen(argv[1], "rb");
		unsigned char* c=new unsigned char[n];
		int i=0;
		while(!feof(f))
		{
			i=fread(c, 1, n*sizeof(unsigned char), f);
			// cout << " --> reader has read " << dec << i << " bytes\n";
			l_oReader.processData(c, i);
		}
		delete [] c;
		fclose(f);
	}

	return 0;
}
