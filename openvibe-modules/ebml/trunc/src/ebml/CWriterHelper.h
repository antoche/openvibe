#ifndef __EBML_CWriterHelper_H__
#define __EBML_CWriterHelper_H__

#include "IWriterHelper.h"

namespace EBML
{
	class EBML_API CWriterHelper : public EBML::IWriterHelper
	{
	public:

		CWriterHelper(void);
		virtual ~CWriterHelper(void);

		virtual EBML::boolean connect(EBML::IWriter* pWriter);
		virtual EBML::boolean disconnect(void);

		virtual EBML::boolean openChild(const EBML::CIdentifier& rIdentifier);
		virtual EBML::boolean closeChild(void);

		virtual EBML::boolean setSIntegerAsChildData(const EBML::int64 iValue);
		virtual EBML::boolean setUIntegerAsChildData(const EBML::uint64 uiValue);
		virtual EBML::boolean setFloat32AsChildData(const EBML::float32 fValue);
		virtual EBML::boolean setFloat64AsChildData(const EBML::float64 fValue);
		virtual EBML::boolean setBinaryAsChildData(const void* pBuffer, const EBML::uint64 ui64BufferLength);
		virtual EBML::boolean setASCIIStringAsChildData(const char* sValue);

		virtual void release(void);

	protected:

		EBML::IWriterHelper* m_pWriterHelperImplementation;
	};
};

#endif // __EBML_CWriterHelper_H__
