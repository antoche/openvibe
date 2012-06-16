#include "ovpCBoxAlgorithmEBMLStreamSpy.h"

#include <iostream>
#include <fstream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;
using namespace OpenViBEToolkit;
using namespace std;

CBoxAlgorithmEBMLStreamSpy::CBoxAlgorithmEBMLStreamSpy(void)
	:m_pReader(NULL)
	,m_pReaderHelper(NULL)
{
}

void CBoxAlgorithmEBMLStreamSpy::release(void)
{
	delete this;
}

boolean CBoxAlgorithmEBMLStreamSpy::initialize(void)
{
	IBox& l_rStaticBoxContext=getStaticBoxContext();

	m_pReader=EBML::createReader(*this);
	m_pReaderHelper=EBML::createReaderHelper();

	boolean l_bExpand;
	CString l_sFileName   =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	uint64  l_ui64LogLevel=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if(l_rStaticBoxContext.getSettingCount()>2)
	{
		l_bExpand              =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
		m_ui64ExpandValuesCount=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	}
	else
	{
		l_bExpand              =false;
		m_ui64ExpandValuesCount=4;
	}

	m_eLogLevel=static_cast<ELogLevel>(l_ui64LogLevel);

	ifstream l_oFile(l_sFileName);
	while(l_oFile.good() && !l_oFile.eof())
	{
		unsigned int l_ui32Identifier1;
		unsigned int l_ui32Identifier2;
		string l_sIdentifier1;
		string l_sIdentifier2;
		string l_sName;
		string l_sType;

		l_oFile>>l_sName;
		l_oFile>>l_sIdentifier1;
		l_oFile>>l_sIdentifier2;
		l_oFile>>l_sType;

		sscanf(l_sIdentifier1.c_str(), "EBML::CIdentifier(0x%08x", &l_ui32Identifier1);
		sscanf(l_sIdentifier2.c_str(), "0x%08x)", &l_ui32Identifier2);

		if(!l_bExpand)
		{
			if((l_sType == "binary(float80)")
			|| (l_sType == "binary(float64)")
			|| (l_sType == "binary(float32)")
			|| (l_sType == "binary(integer8)")
			|| (l_sType == "binary(integer16)")
			|| (l_sType == "binary(integer32)")
			|| (l_sType == "binary(integer64)")
			|| (l_sType == "binary(uinteger8)")
			|| (l_sType == "binary(uinteger16)")
			|| (l_sType == "binary(uinteger32)")
			|| (l_sType == "binary(uinteger64)"))
			{
				l_sType="binary";
			}
		}

		// cout << "[" << l_sIdentifier1 << "][" << l_sIdentifier2 << "]" << endl;
		// printf("[EBML::CIdentifier(0x%08X,][0x%08X]\n", l_ui32Identifier1, l_ui32Identifier2);
		// cout << EBML::CIdentifier(l_ui32Identifier1, l_ui32Identifier2) << endl;

		m_vName[EBML::CIdentifier(l_ui32Identifier1, l_ui32Identifier2)]=l_sName;
		m_vType[EBML::CIdentifier(l_ui32Identifier1, l_ui32Identifier2)]=l_sType;
	}

	return true;
}

boolean CBoxAlgorithmEBMLStreamSpy::uninitialize(void)
{
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;

	m_pReader->release();
	m_pReader=NULL;

	return true;
}

EBML::boolean CBoxAlgorithmEBMLStreamSpy::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	map<EBML::CIdentifier, string>::iterator n;
	map<EBML::CIdentifier, string>::iterator t;
	n=m_vName.find(rIdentifier);
	t=m_vType.find(rIdentifier);
	if(n!=m_vName.end() && t!=m_vType.end())
	{
		return (t->second=="master");
	}
	return false;
}

void CBoxAlgorithmEBMLStreamSpy::openChild(const EBML::CIdentifier& rIdentifier)
{
	map<EBML::CIdentifier, string>::iterator n;
	map<EBML::CIdentifier, string>::iterator t;
	n=m_vName.find(rIdentifier);
	t=m_vType.find(rIdentifier);

	getLogManager() << m_eLogLevel;

	for(size_t i=0; i<=m_vNodes.size(); i++)
	{
		getLogManager() << "  ";
	}

	getLogManager()
		<< "Opened EBML node [id:"
		<< CIdentifier(rIdentifier)
		<< "]-[name:"
		<< CString(n!=m_vName.end()?n->second.c_str():"unknown")
		<< "]";

	if(isMasterChild(rIdentifier))
	{
		getLogManager() << "\n";
	}

	m_vNodes.push(rIdentifier);
}

template <class T>
void CBoxAlgorithmEBMLStreamSpy::processBinaryBlock(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	uint64 l_ui64Count=(ui64BufferSize/sizeof(T));
	const T* l_pBuffer=static_cast<const T*>(pBuffer);
	for(uint64 i=0; i<min(m_ui64ExpandValuesCount, l_ui64Count); i++)
	{
		getLogManager() << (i==0?"":" ") << l_pBuffer[i];
	}
	if(m_ui64ExpandValuesCount<l_ui64Count)
	{
		getLogManager() << " ...";
	}
}

void CBoxAlgorithmEBMLStreamSpy::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	map<EBML::CIdentifier, string>::iterator n;
	map<EBML::CIdentifier, string>::iterator t;
	n=m_vName.find(m_vNodes.top());
	t=m_vType.find(m_vNodes.top());

	if(t!=m_vType.end())
	{
		if(t->second=="uinteger")
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize) << "]";
		else if(t->second=="integer")
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->getSIntegerFromChildData(pBuffer, ui64BufferSize) << "]";
		else if(t->second=="float")
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize) << "]";
		else if(t->second=="string")
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize) << "]";
		else if(t->second=="binary")
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[bytes:" << ui64BufferSize << "]";
		else if(t->second=="binary(float64)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<float64>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(float32)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<float32>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(integer8)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int8>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(integer16)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int16>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(integer32)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int32>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(integer64)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int64>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(uinteger8)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint8>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(uinteger16)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint16>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(uinteger32)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint32>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else if(t->second=="binary(uinteger64)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint64>(pBuffer, ui64BufferSize);
			getLogManager() << "]";
		}
		else
			getLogManager() << "-[type:" << CString("unknown") << "]-[bytes:" << ui64BufferSize << "]";
	}
	getLogManager() << "\n";
}

void CBoxAlgorithmEBMLStreamSpy::closeChild(void)
{
	m_vNodes.pop();
}

boolean CBoxAlgorithmEBMLStreamSpy::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmEBMLStreamSpy::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	IBox& l_rStaticBoxContext=getStaticBoxContext();

	uint64 l_ui64StartTime=0;
	uint64 l_ui64EndTime=0;
	uint64 l_ui64ChunkSize=0;
	const uint8* l_pChunkBuffer=NULL;

	getLogManager() << m_eLogLevel << "\n";

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		if(l_rDynamicBoxContext.getInputChunkCount(i))
		{
			CString l_sInputName;
			l_rStaticBoxContext.getInputName(i, l_sInputName);

			CIdentifier l_sInputType;
			l_rStaticBoxContext.getInputType(i, l_sInputType);

			getLogManager() << m_eLogLevel
				<< "For input " << l_sInputName << " of type " << getTypeManager().getTypeName(l_sInputType) << " :\n";

			for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
			{
				l_rDynamicBoxContext.getInputChunk(i, j, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer);
				l_rDynamicBoxContext.markInputAsDeprecated(i, j);

				getLogManager() << m_eLogLevel
					<< "For chunk [id:" << j << "] at [time:" << CIdentifier(l_ui64StartTime) << "," << CIdentifier(l_ui64EndTime) << "]\n";

				m_pReader->processData(l_pChunkBuffer, l_ui64ChunkSize);
			}
		}
	}

	getLogManager() << m_eLogLevel << "\n";

	return true;
}
