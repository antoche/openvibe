#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	for(int i=0; i<16; i++)
	{
		unsigned short int l_ui16Value1=(rand()&0xffff);
		unsigned short int l_ui16Value2=(rand()&0xffff);
		unsigned short int l_ui16Value3=(rand()&0xffff);
		unsigned short int l_ui16Value4=(rand()&0xffff);
		printf("#define OV_ClassId_                                        OpenViBE::CIdentifier(0x%04X%04X, 0x%04X%04X)\n", (int)l_ui16Value1, (int)l_ui16Value2, (int)l_ui16Value3, (int)l_ui16Value4);
	}

	return 0;
}
