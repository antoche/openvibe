#include "IWriter.h"

#include <vector>
#include <cstdlib>
#include <cstring>

using namespace EBML;
using namespace std;

// ________________________________________________________________________________________________________________
//

inline unsigned long getCodedSizeLength(const uint64 uiValue)
{
	unsigned long l_ulCodedSizeLength=0;
	     if(uiValue<0x000000000000007fLL)
		l_ulCodedSizeLength=1;
	else if(uiValue<0x0000000000003fffLL)
		l_ulCodedSizeLength=2;
	else if(uiValue<0x00000000001fffffLL)
		l_ulCodedSizeLength=3;
	else if(uiValue<0x000000000fffffffLL)
		l_ulCodedSizeLength=4;
	else if(uiValue<0x00000007ffffffffLL)
		l_ulCodedSizeLength=5;
	else if(uiValue<0x000003ffffffffffLL)
		l_ulCodedSizeLength=6;
	else if(uiValue<0x0001ffffffffffffLL)
		l_ulCodedSizeLength=7;
	else if(uiValue<0x00ffffffffffffffLL)
		l_ulCodedSizeLength=8;
	else if(uiValue<0x7fffffffffffffffLL)
		l_ulCodedSizeLength=9;
	else
		l_ulCodedSizeLength=10;

	return l_ulCodedSizeLength;
}

inline boolean getCodedBuffer(const uint64 uiValue, unsigned char* pBuffer, uint64* pBufferLength)
{
	unsigned long i;
	unsigned long l_ulCodedSizeLength=getCodedSizeLength(uiValue);

	if(l_ulCodedSizeLength>*pBufferLength)
	{
		return false;
	}

	unsigned long l_ulIthBit=l_ulCodedSizeLength;
	for(i=0; i<l_ulCodedSizeLength; i++)
	{
		unsigned long l_ulByteShift=l_ulCodedSizeLength-i-1;
		unsigned long l_ulByte=(l_ulByteShift>=8?0:(unsigned char)((uiValue>>(l_ulByteShift*8))&0xff));
		l_ulByte|=(l_ulIthBit>0 && l_ulIthBit<=8?(1<<(8-l_ulIthBit)):0);
		l_ulIthBit-=8;

		pBuffer[i]=(unsigned char)l_ulByte;
	}

	*pBufferLength=l_ulCodedSizeLength;
	return true;
}

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CWriterNode
		{
		public:

			CWriterNode(const CIdentifier& rIdentifier, CWriterNode* pParentNode);
			virtual ~CWriterNode(void);
			void process(IWriterCallback& rWriterCallback);

		protected:

			uint64 getTotalContentSize(boolean bCountIdentifierAndSize);

		private:

			CWriterNode(void);

		public:

			CIdentifier m_oIdentifier;
			CWriterNode* m_pParentNode;
			uint64 m_ui64BufferLength;
			unsigned char* m_pBuffer;
			boolean m_bBuffered;
			vector<CWriterNode*> m_vChildren;
		};
	};
};

// ________________________________________________________________________________________________________________
//

CWriterNode::CWriterNode(const CIdentifier& rIdentifier, EBML::CWriterNode* pParentNode)
	:m_oIdentifier(rIdentifier)
	,m_pParentNode(pParentNode)
	,m_ui64BufferLength(0)
	,m_pBuffer(NULL)
	,m_bBuffered(false)
{
}

CWriterNode::~CWriterNode(void)
{
	vector<CWriterNode*>::iterator i;
	for(i=m_vChildren.begin(); i!=m_vChildren.end(); i++)
	{
		delete (*i);
	}

	delete [] m_pBuffer;
}

void CWriterNode::process(IWriterCallback& rWriterCallback)
{
	unsigned char l_pIdentifier[10];
	unsigned char l_pContentSize[10];
	uint64 l_ui64ContentSizeLength=sizeof(l_pContentSize);
	uint64 l_ui64IdentifierLength=sizeof(l_pIdentifier);
	uint64 l_ui64ContentSize=getTotalContentSize(false);

	if(!getCodedBuffer(l_ui64ContentSize, l_pContentSize, &l_ui64ContentSizeLength))
	{
		// SHOULD NEVER HAPPEN
	}

	if(!getCodedBuffer(m_oIdentifier, l_pIdentifier, &l_ui64IdentifierLength))
	{
		// SHOULD NEVER HAPPEN
	}

	rWriterCallback.write(l_pIdentifier, l_ui64IdentifierLength);
	rWriterCallback.write(l_pContentSize, l_ui64ContentSizeLength);

	if(m_vChildren.size()==0)
	{
		rWriterCallback.write(m_pBuffer, m_ui64BufferLength);
	}
	else
	{
		vector<CWriterNode*>::iterator i;
		for(i=m_vChildren.begin(); i!=m_vChildren.end(); i++)
		{
			(*i)->process(rWriterCallback);
		}
	}
}

uint64 CWriterNode::getTotalContentSize(boolean bCountIdentifierAndSize)
{
	uint64 l_ui64ContentSize=0;
	if(m_vChildren.size()==0)
	{
		l_ui64ContentSize=m_ui64BufferLength;
	}
	else
	{
		vector<CWriterNode*>::iterator i;
		for(i=m_vChildren.begin(); i!=m_vChildren.end(); i++)
		{
			l_ui64ContentSize+=(*i)->getTotalContentSize(true);
		}
	}

	uint64 l_ui64Result=l_ui64ContentSize;
	if(bCountIdentifierAndSize)
	{
		l_ui64Result+=getCodedSizeLength(m_oIdentifier);
		l_ui64Result+=getCodedSizeLength(l_ui64ContentSize);
	}

	return l_ui64Result;
}

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CWriter : public IWriter
		{
		public:

			CWriter(IWriterCallback& rWriterCallback);

			virtual boolean openChild(const CIdentifier& rIdentifier);
			virtual boolean setChildData(const void* pBuffer, const uint64 ui64BufferSize);
			virtual boolean closeChild(void);

			virtual void release(void);

		protected:

			CWriterNode* m_pCurrentNode;
			IWriterCallback& m_rWriterCallback;

		private:

			CWriter(void);
		};
	};
};

// ________________________________________________________________________________________________________________
//

CWriter::CWriter(IWriterCallback& rWriterCallback)
	:m_pCurrentNode(NULL)
	,m_rWriterCallback(rWriterCallback)
{
}

boolean CWriter::openChild(const CIdentifier& rIdentifier)
{
	if(m_pCurrentNode)
	{
		if(m_pCurrentNode->m_bBuffered)
		{
			return false;
		}
	}

	CWriterNode* pResult=new CWriterNode(rIdentifier, m_pCurrentNode);
	if(m_pCurrentNode)
	{
		m_pCurrentNode->m_vChildren.push_back(pResult);
	}
	m_pCurrentNode=pResult;
	return true;
}

boolean CWriter::setChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	if(!m_pCurrentNode)
	{
		return false;
	}

	if(m_pCurrentNode->m_vChildren.size()!=0)
	{
		return false;
	}

	unsigned char* l_pBufferCopy=NULL;
	if(ui64BufferSize)
	{
		if(!pBuffer)
		{
			return false;
		}
		l_pBufferCopy=new unsigned char[(unsigned int)(ui64BufferSize)];
		if(!l_pBufferCopy)
		{
			return false;
		}
		memcpy(l_pBufferCopy, pBuffer, (size_t)(ui64BufferSize));
	}

	m_pCurrentNode->m_ui64BufferLength=ui64BufferSize;
	m_pCurrentNode->m_pBuffer=l_pBufferCopy;
	m_pCurrentNode->m_bBuffered=true;
	return true;
}

boolean CWriter::closeChild(void)
{
	if(!m_pCurrentNode)
	{
		return false;
	}

	if((!m_pCurrentNode->m_bBuffered) && (!m_pCurrentNode->m_vChildren.size()))
	{
		m_pCurrentNode->m_ui64BufferLength=0;
		m_pCurrentNode->m_pBuffer=NULL;
		m_pCurrentNode->m_bBuffered=true;
	}

	CWriterNode* l_pParentNode=m_pCurrentNode->m_pParentNode;
	if(!l_pParentNode)
	{
		m_pCurrentNode->process(m_rWriterCallback);
		delete m_pCurrentNode;
	}

	m_pCurrentNode=l_pParentNode;
	return true;
}

void CWriter::release(void)
{
	while(m_pCurrentNode)
	{
		closeChild();
	}
	delete this;
}

// ________________________________________________________________________________________________________________
//

EBML_API IWriter* EBML::createWriter(IWriterCallback& rWriterCallback)
{
	return new CWriter(rWriterCallback);
}
