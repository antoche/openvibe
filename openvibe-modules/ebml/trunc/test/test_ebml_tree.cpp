#include <cstdio>
#include <cstdlib>

#include <map>
#include <string>

#include "ebml/IReader.h"
#include "ebml/IReaderHelper.h"

using namespace std;
using namespace EBML;

class CReaderCallBack : public IReaderCallBack
{
public:
	CReaderCallBack(int dicc, char** dicv)
		:m_iDepth(0)
		,m_pReaderHelper(NULL)
	{
		m_oMasterNode[EBML_Identifier_Header]=std::pair<string, string>("EBML_Identifier_Header", "");

		m_oNotMasterNode[EBML_Identifier_EBMLVersion]=std::pair<string, string>("EBML_Identifier_EBMLVersion", "uinteger");
		m_oNotMasterNode[EBML_Identifier_EBMLIdLength]=std::pair<string, string>("EBML_Identifier_EBMLIdLength", "uinteger");
		m_oNotMasterNode[EBML_Identifier_EBMLSizeLength]=std::pair<string, string>("EBML_Identifier_EBMLSizeLength", "uinteger");
		m_oNotMasterNode[EBML_Identifier_DocType]=std::pair<string, string>("EBML_Identifier_DocType", "string");
		m_oNotMasterNode[EBML_Identifier_DocTypeVersion]=std::pair<string, string>("EBML_Identifier_DocTypeVersion", "uinteger");
		m_oNotMasterNode[EBML_Identifier_DocTypeReadVersion]=std::pair<string, string>("EBML_Identifier_DocTypeReadVersion", "uinteger");
		m_oNotMasterNode[EBML_Identifier_Void]=std::pair<string, string>("EBML_Identifier_Void", "uinteger");
		m_oNotMasterNode[EBML_Identifier_CRC32]=std::pair<string, string>("EBML_Identifier_CRC32", "uinteger");

		// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

		int i,j;
		char l_sDesc[1024];
		char l_sMaster[1024];
		char l_sType[1024];
		unsigned int l_uiIdentifier;

		for(i=0; i<dicc; i++)
		{
			FILE* f=fopen(dicv[i], "rt");
			if(f)
			{
				while(!feof(f))
				{
					strcpy(l_sType, "n/a");
					strcpy(l_sMaster, "n/a");
					strcpy(l_sDesc, "n/a");
					if(fscanf(f, "%s%i%s%s\n", l_sDesc, &l_uiIdentifier, l_sMaster, l_sType)==4)
					{
						for(j=0; j<32; j++)
						{
							if(l_uiIdentifier&(1<<(31-j)))
							{
								l_uiIdentifier&=~(1<<(31-j));
								break;
							}
						}

						if(!strcmp(l_sMaster, "master"))
							m_oMasterNode[l_uiIdentifier]=std::pair<string, string>(l_sDesc, l_sType);
						else
							m_oNotMasterNode[l_uiIdentifier]=std::pair<string, string>(l_sDesc, l_sType);
					}
				}
				fclose(f);
			}
		}

		// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

		m_pReaderHelper=createReaderHelper();
	}

	virtual ~CReaderCallBack(void)
	{
		m_pReaderHelper->release();

		printf("\n");
	}

	virtual bool isMasterChild(const CIdentifier& rIdentifier)
	{
		if(m_oMasterNode.find(rIdentifier)!=m_oMasterNode.end()) return true;
		return false;
	}

	virtual void openChild(const CIdentifier& rIdentifier)
	{
		map<CIdentifier, pair<string, string> >::iterator n;
		uint64 l_ui64Identifier=rIdentifier;

		if((n=m_oMasterNode.find(rIdentifier))!=m_oMasterNode.end())
		{
			m_sDescription=n->second.first;
			m_sType=n->second.second;
		}
		else if((n=m_oNotMasterNode.find(rIdentifier))!=m_oNotMasterNode.end())
		{
			m_sDescription=n->second.first;
			m_sType=n->second.second;
		}
		else
		{
			m_sDescription="unknown";
			m_sType="unknown";
		}

		m_oCurrentIdentifier=rIdentifier;
		printf("\n");
		for(int i=0; i<m_iDepth; i++) printf("   ");
		printf("Opening child node [id:0x%08X]-[name:%s]", (int)l_ui64Identifier, m_sDescription.c_str());
		m_iDepth++;
	}

	virtual void processChildData(const void* pBuffer, const uint64 ui64BufferSize)
	{
		if(m_sType=="float")
			printf("-[type:%s]-[value:%f]", m_sType.c_str(), (float) m_pReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize));
		else if(m_sType=="integer")
			printf("-[type:%s]-[value:%i]", m_sType.c_str(), (int) m_pReaderHelper->getSIntegerFromChildData(pBuffer, ui64BufferSize));
		else if(m_sType=="uinteger")
			printf("-[type:%s]-[value:%i]", m_sType.c_str(), (int) m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize));
		else if(m_sType=="string")
			printf("-[type:%s]-[value:%s]", m_sType.c_str(), m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize));
		else if(m_sType=="binary")
			printf("-[type:%s] %i byte(s)", m_sType.c_str(), (int)ui64BufferSize);
		else
			printf("-[type:%s] %i byte(s)", m_sType.c_str(), (int)ui64BufferSize);
	}

	virtual void closeChild(void)
	{
		m_iDepth--;
	}

	int m_iDepth;
	IReaderHelper* m_pReaderHelper;
	CIdentifier m_oCurrentIdentifier;
	string m_sDescription;
	string m_sType;
	map<CIdentifier, pair<string, string> > m_oMasterNode;
	map<CIdentifier, pair<string, string> > m_oNotMasterNode;
};

int main(int argc, char** argv)
{
	if(argc<2)
	{
		printf("syntax : %s filein.ebml [dictionnaries...]\n", argv[0]);
		return -1;
	}

	CReaderCallBack cb(argc>2?argc-2:0, argc>2?argv+2:NULL);
	IReader* l_pReader=createReader(cb);

	FILE* file=fopen(argv[1], "rb");
	unsigned char buffer[102400];
	while(!feof(file))
	{
		int count=fread(buffer, 1, sizeof(buffer), file);
		l_pReader->processData(buffer, count);
	}
	fclose(file);

	return 0;
}
