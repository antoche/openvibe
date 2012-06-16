#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "ebml/defines.h"
#include "ebml/IWriter.h"
#include "ebml/IWriterHelper.h"
#include "ebml/CWriterHelper.h"

class CWriterCallBack : public EBML::IWriterCallBack
{
public:
	CWriterCallBack(char* filename)
	{
		f=fopen(filename, "wb");
	}

	virtual ~CWriterCallBack(void)
	{
		if(f) fclose(f);
	}

	virtual void write(const void* pBuffer, const EBML::uint64 ui64BufferSize)
	{
		if(f) fwrite(pBuffer, ui64BufferSize, 1, f);
	}

	FILE* f;
};

int main(int argc, char** argv)
{
	if(argc<2)
	{
		printf("syntax : %s fileout.ebml\n", argv[0]);
		return -1;
	}

	CWriterCallBack cb(argv[1]);
	EBML::IWriter* l_pWriter=EBML::createWriter(cb);
	EBML::CWriterHelper l_oWriterHelper;

	l_oWriterHelper.connect(l_pWriter);

	l_oWriterHelper.openChild(EBML_Identifier_Header);

		l_oWriterHelper.openChild(EBML_Identifier_DocType);
		l_oWriterHelper.setASCIIStringAsChildData("matroska");
		l_oWriterHelper.closeChild();

		l_oWriterHelper.openChild(0xffff);

			l_oWriterHelper.openChild(EBML_Identifier_DocTypeVersion);
			l_oWriterHelper.setUIntegerAsChildData(1);
			l_oWriterHelper.closeChild();

			l_oWriterHelper.openChild(EBML_Identifier_DocTypeReadVersion);
			l_oWriterHelper.setSIntegerAsChildData(655356);
			l_oWriterHelper.closeChild();

		l_oWriterHelper.closeChild();

		l_oWriterHelper.openChild(0x1234);
		l_oWriterHelper.setUIntegerAsChildData(0);
		l_oWriterHelper.closeChild();

		l_oWriterHelper.openChild(0xffffffffffffffffLL);
		l_oWriterHelper.setUIntegerAsChildData(0xff000000ff000000LL);
		l_oWriterHelper.closeChild();

		l_oWriterHelper.openChild(0x4321);
		l_oWriterHelper.setFloat64AsChildData(M_PI);
		l_oWriterHelper.closeChild();

		l_oWriterHelper.openChild(0x8765);
		l_oWriterHelper.setFloat32AsChildData(M_PI);
		l_oWriterHelper.closeChild();

	l_oWriterHelper.closeChild();

	return 0;
}
