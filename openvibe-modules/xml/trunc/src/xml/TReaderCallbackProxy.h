#ifndef __XML_TReaderCallbackProxy_H__
#define __XML_TReaderCallbackProxy_H__

#include "IReader.h"

namespace XML
{

// ________________________________________________________________________________________________________________
//

	template <class COwnerClass>
	class TReaderCallbackProxy1 : public XML::IReaderCallback
	{
	public:
		TReaderCallbackProxy1(
			COwnerClass& rOwnerObject,
			void (COwnerClass::*mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount),
			void (COwnerClass::*mfpProcessChildData)(const char* sData),
			void (COwnerClass::*mfpCloseChild)(void))
			:m_rOwnerObject(rOwnerObject)
			,m_mfpOpenChild(mfpOpenChild)
			,m_mfpProcessChildData(mfpProcessChildData)
			,m_mfpCloseChild(mfpCloseChild)
		{
		}
		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
		{
			if(m_mfpOpenChild)
			{
				m_rOwnerObject.m_mfpOpenChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount);
			}
		}
		virtual void processChildData(const char* sData)
		{
			if(m_mfpProcessChildData)
			{
				m_rOwnerObject.m_mfpProcessChildData(sData);
			}
		}
		virtual void closeChild(void)
		{
			if(m_mfpCloseChild)
			{
				m_rOwnerObject.m_mfpCloseChild();
			}
		}
	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount);
		void (COwnerClass::*m_mfpProcessChildData)(const char* sData);
		void (COwnerClass::*m_mfpCloseChild)(void);
	};

// ________________________________________________________________________________________________________________
//

	template <class COwnerClass, void (COwnerClass::*mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount), void (COwnerClass::*mfpProcessChildData)(const char* sData), void (COwnerClass::*mfpCloseChild)(void)>
	class TReaderCallbackProxy2 : public XML::IReaderCallback
	{
	public:
		TReaderCallbackProxy2(COwnerClass rOwnerObject)
			:m_rOwnerObject(rOwnerObject)
			,m_mfpOpenChild(mfpOpenChild)
			,m_mfpProcessChildData(mfpProcessChildData)
			,m_mfpCloseChild(mfpCloseChild)
		{
		}
		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
		{
			if(mfpOpenChild)
			{
				m_rOwnerObject.mfpOpenChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount);
			}
		}
		virtual void processChildData(const char* sData)
		{
			if(mfpProcessChildData)
			{
				m_rOwnerObject.mfpProcessChildData(sData);
			}
		}
		virtual void closeChild(void)
		{
			if(mfpCloseChild)
			{
				m_rOwnerObject.mfpCloseChild();
			}
		}
	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount);
		void (COwnerClass::*m_mfpProcessChildData)(const char* sData);
		void (COwnerClass::*m_mfpCloseChild)(void);
	};

// ________________________________________________________________________________________________________________
//

};

#endif // __XML_TReaderCallbackProxy_H__
