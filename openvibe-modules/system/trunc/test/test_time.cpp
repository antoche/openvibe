#include <iostream>

#include <system/Time.h>

#if defined System_OS_Windows
#include <windows.h>
#define boolean System::boolean
#else
#include <netinet/in.h>
#endif

using namespace System;
using namespace std;

int main(int argc, char** argv)
{
	uint64 l_ui64StartTime=Time::zgetTime();
	uint64 l_ui64Time=l_ui64StartTime;
	do
	{
		uint64 l_ui64Duration=l_ui64Time-l_ui64StartTime;
		printf("time(s) 0x%08x\n", (uint32)(l_ui64Duration>>32));

		uint64 l_ui64Time2=l_ui64Time;
		do
		{
			Time::sleep(10);
			l_ui64Time2=Time::zgetTime();
		}
		while(l_ui64Time2-l_ui64Time < 1LL<<32);
		l_ui64Time=l_ui64Time2;
	}
	while(l_ui64Time-l_ui64StartTime < 30LL<<32);

	return 0;
}
