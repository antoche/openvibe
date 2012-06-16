#ifndef __XML_IWriter_H__
#define __XML_IWriter_H__

#include "defines.h"

namespace XML
{
	class XML_API IWriterCallback
	{
	public:
		virtual ~IWriterCallback(void) { }
		virtual void write(const char* sString)=0;
	};

	class XML_API IWriterCallBack : public IWriterCallback { };

	class XML_API IWriter
	{
	public:
		virtual XML::boolean openChild(const char* sName)=0;
		virtual XML::boolean setAttribute(const char* sAttributeName, const char* sAttributeValue)=0;
		virtual XML::boolean setChildData(const char* sData)=0;
		virtual XML::boolean closeChild(void)=0;
		virtual void release(void)=0;
	protected:
		virtual ~IWriter(void) { }
	};

	extern XML_API XML::IWriter* createWriter(XML::IWriterCallback& rWriterCallback);
};

#endif // __XML_IWriter_H__
