#ifndef __EBML_CWriter_H__
#define __EBML_CWriter_H__

#include "IWriter.h"

namespace EBML
{
	class EBML_API CWriter : public EBML::IWriter
	{
	public:

		CWriter(EBML::IWriterCallback& rWriterCallback);
		virtual ~CWriter(void);

		virtual EBML::boolean openChild(const EBML::CIdentifier& rIdentifier);
		virtual EBML::boolean setChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		virtual EBML::boolean closeChild(void);
		virtual void release(void);

	protected:

		EBML::IWriter* m_pWriterImplementation;

	private:

		CWriter(void);
	};
};

#endif // __EBML_CWriter_H__
