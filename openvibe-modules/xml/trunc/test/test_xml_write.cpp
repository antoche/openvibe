#include <cstdio>

#include "xml/IWriter.h"

using namespace XML;

class CWriterCallBack : public XML::IWriterCallBack
{
public:
	CWriterCallBack(char* filename)
	{
		f=fopen(filename, "wt");
	}

	virtual ~CWriterCallBack(void)
	{
		if(f) fclose(f);
	}

	virtual void write(const char* sString)
	{
		if(f) fprintf(f, sString);
	}

	FILE* f;
};

int main(int argc, char**argv)
{
	if(argc<2)
	{
		printf("syntax : %s fileout.xml\n", argv[0]);
		return -1;
	}

	CWriterCallBack cb(argv[1]);
	XML::IWriter* l_pWriter=XML::createWriter(cb);

	l_pWriter->openChild("MasterNode");

		l_pWriter->openChild("SubMasterNode");

			l_pWriter->openChild("ChildNode");
			l_pWriter->setChildData("some data for this node");
			l_pWriter->closeChild();

			l_pWriter->openChild("UnusedNode");
			l_pWriter->setChildData("this is useless");
			l_pWriter->closeChild();

		l_pWriter->closeChild();

		l_pWriter->openChild("AnotherChildNode");
		l_pWriter->setChildData("some data");
		l_pWriter->closeChild();

	l_pWriter->closeChild();

	return 0;
}
