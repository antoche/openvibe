#include "IReader.h"
#include <cstring>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace EBML;

// ________________________________________________________________________________________________________________
//

#if 0
#define _Debug_ _is_in_debug_mode_(m_ui64TotalBytes)
static bool _is_in_debug_mode_(uint64 ui64Value)
{
	static int i=0;
	// bool result=i++>5500000;
	bool result=ui64Value>29605500;
	if(result)printf("Arround %llu\n", ui64Value);
	return result;

}
#else
#define _Debug_ false
#endif

// ________________________________________________________________________________________________________________
//

inline bool needsTwoBytesToGetCodedSizeLength(unsigned char* pBuffer)
{
	return pBuffer[0]==0;
}

inline unsigned long getCodedSizeLength(unsigned char* pBuffer, unsigned long ulBufferLength)
{
	unsigned long l_ulCodedSizeLength;
	     if(pBuffer[0]>>7)
		l_ulCodedSizeLength=1;
	else if(pBuffer[0]>>6)
		l_ulCodedSizeLength=2;
	else if(pBuffer[0]>>5)
		l_ulCodedSizeLength=3;
	else if(pBuffer[0]>>4)
		l_ulCodedSizeLength=4;
	else if(pBuffer[0]>>3)
		l_ulCodedSizeLength=5;
	else if(pBuffer[0]>>2)
		l_ulCodedSizeLength=6;
	else if(pBuffer[0]>>1)
		l_ulCodedSizeLength=7;
	else if(pBuffer[0])
		l_ulCodedSizeLength=8;
	else
	{
		if(_Debug_ && ulBufferLength<2)
			printf("EBML::getCodedSizeLength called with smaller buffer size %lu - needs at least 2\n", ulBufferLength);

		if(pBuffer[1]>>7)
			l_ulCodedSizeLength=9;
		else
			l_ulCodedSizeLength=10;
	}
	return l_ulCodedSizeLength;
}

inline uint64 getValue(unsigned char* pBuffer, unsigned long ulBufferLength)
{
	uint64 l_uiResult=0;
	unsigned long l_ulCodedSizeLength=getCodedSizeLength(pBuffer, ulBufferLength);
	unsigned long i;
	unsigned long l_ulIthBit=l_ulCodedSizeLength;
	for(i=0; i<l_ulCodedSizeLength; i++)
	{
		l_uiResult=(l_uiResult<<8)+(pBuffer[i]);
		l_uiResult&=~(l_ulIthBit>0 && l_ulIthBit<=8?(1<<(8-l_ulIthBit)):0);
		l_ulIthBit-=8;
	}
	return l_uiResult;
}

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CReaderNode
		{
		public:

			CReaderNode(const CIdentifier& rIdentifier, CReaderNode* pParentNode);

		private:

			CReaderNode(void);

		public:

			CReaderNode* m_pParentNode;
			CIdentifier m_oIdentifier;
			uint64 m_ui64ContentSize;
			uint64 m_ui64ReadContentSize;
			unsigned char* m_pBuffer;
//			boolean m_bBufferShouldBeDeleted;
		};
	};
};

CReaderNode::CReaderNode(const CIdentifier& rIdentifier, CReaderNode* pParentNode)
	:m_pParentNode(pParentNode)
	,m_oIdentifier(rIdentifier)
	,m_ui64ContentSize(0)
	,m_ui64ReadContentSize(0)
	,m_pBuffer(NULL)
{
}

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CReader : public IReader
		{
		public:

			CReader(IReaderCallback& rReaderCallback);
			virtual ~CReader(void);

			virtual boolean processData(const void* pBuffer, const uint64 ui64BufferSize);
			virtual CIdentifier getCurrentNodeIdentifier(void) const;
			virtual uint64 getCurrentNodeSize(void) const;

			virtual void release(void);

		protected:

			enum Status
			{
				FillingIdentifier,
				FillingContentSize,
				FillingContent,
			};

			IReaderCallback& m_rReaderCallback;
			CReaderNode* m_pCurrentNode;
			uint64 m_ui64PendingSize;
			uint64 m_ui64PendingCount;
			unsigned char* m_pPending;
			Status m_eStatus;
			Status m_eLastStatus;
			CIdentifier m_oCurrentIdentifier;
			uint64 m_ui64CurrentContentSize;

			uint64 m_ui64TotalBytes;
		};
	};
};

// ________________________________________________________________________________________________________________
//

CReader::CReader(IReaderCallback& rReaderCallback)
	:m_rReaderCallback(rReaderCallback)
	,m_pCurrentNode(NULL)
	,m_ui64PendingSize(0)
	,m_ui64PendingCount(0)
	,m_pPending(NULL)
	,m_eStatus(FillingIdentifier)
	,m_oCurrentIdentifier(0)
	,m_ui64CurrentContentSize(0)
	,m_ui64TotalBytes(0)
{
}

CReader::~CReader(void)
{
	delete [] m_pPending;
	while (m_pCurrentNode)
	{
		CReaderNode* l_pParentNode=m_pCurrentNode->m_pParentNode;
		delete m_pCurrentNode;
		m_pCurrentNode=l_pParentNode;
	}
}

boolean CReader::processData(const void* pBuffer, const uint64 ui64BufferSize)
{
	m_ui64TotalBytes+=ui64BufferSize;

if(_Debug_)
{
	printf("Received %i byte(s) new buffer :", (int)ui64BufferSize);
	for(int i=0; i<(int)ui64BufferSize /* && i<4*/; i++)
		printf("[%02X]", ((unsigned char*)pBuffer)[i]);
	printf("...\n");
}

	if(!pBuffer || !ui64BufferSize)
	{
		return true;
	}

	unsigned char* l_pBuffer=(unsigned char*)pBuffer;
	uint64 l_ui64BufferSize=ui64BufferSize;
	boolean l_bFinished=false;
	while(!l_bFinished)
	{
		uint64 l_ui64ProcessedPendingBytes=0;
		uint64 l_ui64ProcessedBytes=0;
		m_eLastStatus=m_eStatus;

if(_Debug_)
{
		if(m_ui64PendingCount)
		{
			printf("%i byte(s) pending : ", (int)m_ui64PendingCount);
			for(int i=0; i<(int)m_ui64PendingCount; i++)
				printf("[%02X]", m_pPending[i]);
			printf("\n");
		}
}

		// Processes data
		switch(m_eStatus)
		{
			case FillingIdentifier:
			case FillingContentSize:
				{
					if(needsTwoBytesToGetCodedSizeLength(m_ui64PendingCount?m_pPending:l_pBuffer))
					{
						if(m_ui64PendingCount+l_ui64BufferSize<2)
						{
							l_bFinished=true;
							break;
						}

						if(m_ui64PendingCount==1)
						{
							// assumes (l_ui64BufferSize != 0) because (m_ui64PendingCount + l_ui64BufferSize >= 2) and (m_ui64PendingCount == 1)
							m_pPending[1]=l_pBuffer[0];
							l_pBuffer++;
							m_ui64PendingCount++;
							l_ui64BufferSize--;
						}
					}

					unsigned long l_ulCodedSizeLength=getCodedSizeLength(m_ui64PendingCount?m_pPending:l_pBuffer, m_ui64PendingCount?m_ui64PendingCount:l_ui64BufferSize);
					if(l_ulCodedSizeLength>l_ui64BufferSize+m_ui64PendingCount)
					{
						l_bFinished=true;
					}
					else
					{
						unsigned char* l_pEncodedBuffer=new unsigned char[l_ulCodedSizeLength];
						uint64 l_ui64PendingBytesToCopy=(l_ulCodedSizeLength>m_ui64PendingCount?m_ui64PendingCount:l_ulCodedSizeLength);
						::memcpy(l_pEncodedBuffer, m_pPending, (size_t)(l_ui64PendingBytesToCopy));
						::memcpy(l_pEncodedBuffer+l_ui64PendingBytesToCopy, l_pBuffer, (size_t)(l_ulCodedSizeLength-l_ui64PendingBytesToCopy));
						uint64 l_ui64DecodedValue=getValue(l_pEncodedBuffer, l_ulCodedSizeLength);
						delete [] l_pEncodedBuffer;
						l_ui64ProcessedPendingBytes=l_ui64PendingBytesToCopy;
						l_ui64ProcessedBytes=l_ulCodedSizeLength;

						switch(m_eStatus)
						{
							case FillingIdentifier:
								{
									m_oCurrentIdentifier=l_ui64DecodedValue;
									m_eStatus=FillingContentSize;
if(_Debug_)
{
									printf("Found identifier 0x%llX - Changing status to FillingContentSize...\n", (unsigned long long)m_oCurrentIdentifier);
}
								}
								break;

							case FillingContentSize:
								{
									m_ui64CurrentContentSize=l_ui64DecodedValue;
									if(m_rReaderCallback.isMasterChild(m_oCurrentIdentifier))
									{
										m_eStatus=FillingIdentifier;
if(_Debug_)
{
										printf("Found content size %llu of master node - Changing status to FillingIdentifier...\n", (unsigned long long)m_ui64CurrentContentSize);
}
									}
									else
									{
										m_eStatus=FillingContent;
if(_Debug_)
{
										printf("Found content size %llu of *non* master node - Changing status to FillingContent...\n", (unsigned long long)m_ui64CurrentContentSize);
}
									}
								}
								break;

							case FillingContent:
								// Should never happen - avoids the warning
								break;
						}
					}
				}
				break;

			case FillingContent:
				{
					if(m_pCurrentNode->m_ui64ContentSize==0)
					{
						m_eStatus=FillingIdentifier;
if(_Debug_)
{
						printf("Finished with %llu byte(s) content - Changing status to FillingIdentifier...\n", (unsigned long long)m_pCurrentNode->m_ui64ContentSize);
}
						m_rReaderCallback.processChildData(NULL, 0);
					}
					else
					{
						if(m_pCurrentNode->m_ui64ReadContentSize==0 && m_pCurrentNode->m_ui64ContentSize<=l_ui64BufferSize)
						{
							m_eStatus=FillingIdentifier;

							l_ui64ProcessedBytes=m_pCurrentNode->m_ui64ContentSize;
if(_Debug_)
{
							printf("Optimized processing of %llu byte(s) content - Changing status to FillingIdentifier...\n", (unsigned long long)m_pCurrentNode->m_ui64ContentSize);
}
							m_rReaderCallback.processChildData(l_pBuffer, m_pCurrentNode->m_ui64ContentSize);
						}
						else
						{
							if(m_pCurrentNode->m_ui64ContentSize-m_pCurrentNode->m_ui64ReadContentSize>l_ui64BufferSize)
							{
								::memcpy(m_pCurrentNode->m_pBuffer+m_pCurrentNode->m_ui64ReadContentSize, l_pBuffer, (size_t)(l_ui64BufferSize));
								l_ui64ProcessedBytes=l_ui64BufferSize;
								l_bFinished=true;
							}
							else
							{
								::memcpy(m_pCurrentNode->m_pBuffer+m_pCurrentNode->m_ui64ReadContentSize, l_pBuffer, (size_t)(m_pCurrentNode->m_ui64ContentSize-m_pCurrentNode->m_ui64ReadContentSize));
								l_ui64ProcessedBytes=m_pCurrentNode->m_ui64ContentSize-m_pCurrentNode->m_ui64ReadContentSize;

								m_eStatus=FillingIdentifier;
if(_Debug_)
{
								printf("Finished with %llu byte(s) content - Changing status to FillingIdentifier...\n", (unsigned long long)m_pCurrentNode->m_ui64ContentSize);
}
								m_rReaderCallback.processChildData(m_pCurrentNode->m_pBuffer, m_pCurrentNode->m_ui64ContentSize);
							}
						}
					}
				}
				break;
		}

		// Updates buffer pointer and size
		uint64 l_ui64ProcessedBytesInBuffer=l_ui64ProcessedBytes-l_ui64ProcessedPendingBytes;
		l_pBuffer+=l_ui64ProcessedBytesInBuffer;
		l_ui64BufferSize-=l_ui64ProcessedBytesInBuffer;
		m_ui64PendingCount-=l_ui64ProcessedPendingBytes;

		// Updates read size
		CReaderNode* l_pNode=m_pCurrentNode;
		while (l_pNode)
		{
			l_pNode->m_ui64ReadContentSize+=l_ui64ProcessedBytes;
			l_pNode=l_pNode->m_pParentNode;
		}

		// Creates new node when needed
		if(m_eStatus!=FillingContentSize && m_eLastStatus==FillingContentSize)
		{
			m_pCurrentNode=new CReaderNode(m_oCurrentIdentifier, m_pCurrentNode);
			m_pCurrentNode->m_ui64ContentSize=m_ui64CurrentContentSize;
			m_pCurrentNode->m_pBuffer=new unsigned char[(unsigned int)(m_ui64CurrentContentSize)];
			m_rReaderCallback.openChild(m_pCurrentNode->m_oIdentifier);
		}
		else
		{
			// Closes finished nodes
			while(m_pCurrentNode && m_pCurrentNode->m_ui64ContentSize == m_pCurrentNode->m_ui64ReadContentSize)
			{
				m_rReaderCallback.closeChild();
				CReaderNode* l_pParentNode=m_pCurrentNode->m_pParentNode;
				delete [] m_pCurrentNode->m_pBuffer;
				delete m_pCurrentNode;
				m_pCurrentNode=l_pParentNode;
			}
		}
	}

	// Updates pending data
	if(m_ui64PendingCount+l_ui64BufferSize>m_ui64PendingSize)
	{
		unsigned char* l_pPending=new unsigned char[(unsigned int)(m_ui64PendingCount+l_ui64BufferSize+1)]; // Ugly hack, reserve 1 more byte on pending data so we are sure we can insert this additional pending byte when only one byte is pending and two bytes are needed for decoding identifier and/or buffer size
		::memcpy(l_pPending, m_pPending, (size_t)(m_ui64PendingCount));
		delete [] m_pPending;
		m_pPending=l_pPending;
		m_ui64PendingSize=m_ui64PendingCount+l_ui64BufferSize;
	}
	::memcpy(m_pPending+m_ui64PendingCount, l_pBuffer, (size_t)(l_ui64BufferSize));
	m_ui64PendingCount+=l_ui64BufferSize;

if(_Debug_)
{
	printf("\n");
}
	return true;
}

CIdentifier CReader::getCurrentNodeIdentifier(void) const
{
if(_Debug_)
{
	printf("getCurrentNodeIdentifier : %p\n", m_pCurrentNode);
}
	return m_pCurrentNode?m_pCurrentNode->m_oIdentifier:CIdentifier();
}

uint64 CReader::getCurrentNodeSize(void) const
{
if(_Debug_)
{
	printf("getCurrentNodeSize : %p\n", m_pCurrentNode);
}
	return m_pCurrentNode?m_pCurrentNode->m_ui64ContentSize:0;
}

void CReader::release(void)
{
	delete this;
}

// ________________________________________________________________________________________________________________
//

EBML_API IReader* EBML::createReader(IReaderCallback& rReaderCallback)
{
	return new CReader(rReaderCallback);
}
