#include <iostream>

#include <system/Memory.h>

#if defined System_OS_Windows
#include <windows.h>
#define boolean System::boolean
#else
#include <netinet/in.h>
#endif

using namespace System;
using namespace std;

#define __test__(t,v) \
	{ \
		uint8 buf[1024]; \
		boolean b1, b2; \
		t v1; \
		t v2; \
		v1=v; \
		b1=Memory::hostToLittleEndian(v1, buf); \
		b2=Memory::littleEndianToHost(buf, &v2); \
		cout << "little endian for "#t" : " << v1 << " and " << v2 << " [" << b1 << ";" << b2 << "]" << endl; \
		b1=Memory::hostToBigEndian(v1, buf); \
		b2=Memory::bigEndianToHost(buf, &v2); \
		cout << "big endian for    "#t" : " << v1 << " and " << v2 << " [" << b1 << ";" << b2 << "]" << endl; \
	}

int main(int argc, char** argv)
{
	__test__(uint16, 1);
	__test__(uint32, -1);
	__test__(uint64, 1);

	__test__(float32, -1);
	__test__(float64, 1);

	return 0;
}
