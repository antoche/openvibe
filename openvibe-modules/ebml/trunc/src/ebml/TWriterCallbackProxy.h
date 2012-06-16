#ifndef __EBML_TWriterCallbackProxy__
#define __EBML_TWriterCallbackProxy__

#include "IWriter.h"

namespace EBML
{

// ________________________________________________________________________________________________________________
//

	template <class COwnerClass>
	class TWriterCallbackProxy1 : public EBML::IWriterCallback
	{
	public:
		TWriterCallbackProxy1(
			COwnerClass& rOwnerObject,
			void (COwnerClass::*mfpWrite)(const void* pBuffer, const EBML::uint64 ui64BufferSize))
			:m_rOwnerObject(rOwnerObject)
			,m_mfpWrite(mfpWrite)
		{
		}
		virtual void write(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			if(m_mfpWrite)
			{
				(m_rOwnerObject.*m_mfpWrite)(pBuffer, ui64BufferSize);
			}
		}
	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const void* pBuffer, const EBML::uint64 ui64BufferSize);
	};

// ________________________________________________________________________________________________________________
//

	template <class COwnerClass, void (COwnerClass::*mfpWrite)(const void* pBuffer, const EBML::uint64 ui64BufferSize)>
	class TWriterCallbackProxy2 : public EBML::IWriterCallback
	{
	public:
		TWriterCallbackProxy2(
			COwnerClass& rOwnerObject)
			:m_rOwnerObject(rOwnerObject)
			,m_mfpWrite(mfpWrite)
		{
		}
		virtual void write(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			if(m_mfpWrite)
			{
				(m_rOwnerObject.*m_mfpWrite)(pBuffer, ui64BufferSize);
			}
		}
	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const void* pBuffer, const EBML::uint64 ui64BufferSize);
	};

// ________________________________________________________________________________________________________________
//

};

#endif // __EBML_TWriterCallbackProxy__
