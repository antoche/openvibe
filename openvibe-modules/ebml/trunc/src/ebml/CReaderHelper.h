#ifndef __EBML_CReaderHelper_H__
#define __EBML_CReaderHelper_H__

#include "IReaderHelper.h"

namespace EBML
{
	class EBML_API CReaderHelper : public EBML::IReaderHelper
	{
	public:

		CReaderHelper(void);
		virtual ~CReaderHelper(void);

		virtual EBML::uint64 getUIntegerFromChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		virtual EBML::int64 getSIntegerFromChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		virtual EBML::float64 getFloatFromChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		virtual const char* getASCIIStringFromChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		virtual void release(void);

	protected:

		EBML::IReaderHelper* m_pReaderHelperImplementation;
	};
};

#endif // __EBML_CReaderHelper_H__
