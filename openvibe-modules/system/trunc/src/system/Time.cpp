#include "Time.h"

#include <cmath>

#if defined System_OS_Linux
 #include <unistd.h>
 #include <ctime>
 #include <sys/time.h>
#elif defined System_OS_Windows
 #include <windows.h>
#else
#endif

using namespace System;
#define boolean System::boolean

boolean Time::sleep(const uint32 ui32MilliSeconds)
{
	return zsleep(((((uint64)ui32MilliSeconds)<<22)/1000)<<10);
}

#if defined System_OS_Windows

namespace
{
	static uint64 zgetTime1(void)
	{
		// high definition time estimation
		// counts CPU cycles and uses nominal frequency
		//  - very accurate on short duration
		//  - very unaccurate on long duration

		uint64 l_ui64Result=0;

		static boolean l_bInitialized=false;
		static uint64 l_ui64Frequency;
		static uint64 l_ui64CounterStart;
		uint64 l_ui64Counter;

		if(!l_bInitialized)
		{
			LARGE_INTEGER l_oPerformanceFrequency;
			QueryPerformanceFrequency(&l_oPerformanceFrequency);
			l_ui64Frequency=l_oPerformanceFrequency.QuadPart;

			LARGE_INTEGER l_oPerformanceCounterStart;
			QueryPerformanceCounter(&l_oPerformanceCounterStart);
			l_ui64CounterStart=l_oPerformanceCounterStart.QuadPart;

			l_bInitialized=true;
		}

		LARGE_INTEGER l_oPerformanceCounter;
		QueryPerformanceCounter(&l_oPerformanceCounter);
		l_ui64Counter=l_oPerformanceCounter.QuadPart-l_ui64CounterStart;

		l_ui64Result=((l_ui64Counter/l_ui64Frequency)<<32)+(((l_ui64Counter%l_ui64Frequency)<<32)/l_ui64Frequency);

		return l_ui64Result;
	}

	static uint64 zgetTime2(void)
	{
		// low definition time estimation
		// uses windows multimedia timers
		//  - very unaccurate on short duration (refreshed every 15ms)
		//  - very accurate on long duration

		uint64 l_ui64Result=0;

		static boolean l_bInitialized=false;
		static uint64 l_ui64CounterStart;
		uint64 l_ui64Counter;

		l_ui64Counter=uint64(timeGetTime());
		l_ui64Counter=((l_ui64Counter/1000)<<32)+(((l_ui64Counter%1000)<<32)/1000);

		if(!l_bInitialized)
		{
			l_ui64CounterStart=l_ui64Counter;
			l_bInitialized=true;
		}

		l_ui64Result=l_ui64Counter-l_ui64CounterStart;

		return l_ui64Result;
	}

	static uint64 win32_zgetTime(void)
	{
		// hopefully high definition time estimation
		// tries to mix both low & high definition chronos
		// and rescale the long-time unaccurate chrono in the long-time accurate (but locally unaccurate) chrono range

		uint64 l_ui64Result=0;
		static boolean l_bInitialized=false;
		static uint64 t1_start=0;
		static uint64 t2_start=0;
		static uint64 l_ui64Result1_last=0;
		static uint64 l_ui64Result2_last=0;
		static uint64 l_ui64Result_last=0;

		// intialize start times for both low & high def chronos
		if(!l_bInitialized)
		{
			t1_start=zgetTime1();
			t2_start=zgetTime2();
			l_bInitialized=true;
		}

		// update current time for both low & high def chronos
		uint64 l_ui64Result1=zgetTime1()-t1_start;
		uint64 l_ui64Result2=zgetTime2()-t2_start;

		// when low def chrono has update, recompute scale coefficients
		if(l_ui64Result1!=l_ui64Result1_last)
		{
			l_ui64Result1_last=l_ui64Result1;
			l_ui64Result2_last=l_ui64Result2;
		}

		// when denominator is 0, consider that elapsed time was not sufficient since the begining
		if(l_ui64Result2_last==0)
		{
			return 0;
		}

		// scale high def chrono in low def chrono range
		l_ui64Result=(l_ui64Result2/l_ui64Result2_last)*l_ui64Result1_last;
		l_ui64Result+=((l_ui64Result2%l_ui64Result2_last)*l_ui64Result1_last)/l_ui64Result2_last;

		// ensure no return in past
		if(l_ui64Result<l_ui64Result_last)
		{
			return l_ui64Result_last;
		}
		l_ui64Result_last=l_ui64Result;
		return l_ui64Result;
	}
}

#endif

boolean Time::zsleep(const uint64 ui64Seconds)
{
#if defined System_OS_Linux
	usleep((ui64Seconds*1000000)>>32);
#elif defined System_OS_Windows
	Sleep((uint32)(((ui64Seconds>>10)*1000)>>22));
#else
#endif
	return true;
}

uint32 Time::getTime(void)
{
	return (uint32)(((zgetTime()>>22)*1000)>>10);
}

uint64 Time::zgetTime(void)
{
	uint64 l_ui64Result=0;

#if defined System_OS_Linux

	static boolean l_bInitialized=false;
	static struct timeval l_oTimeValueStart;
	struct timeval l_oTimeValue;
	uint64 l_i64TimeMicroSecond=0;

	if(!l_bInitialized)
	{
		gettimeofday(&l_oTimeValueStart, NULL);

		l_bInitialized=true;
	}

	gettimeofday(&l_oTimeValue, NULL);
	int64 l_i64SecDiff=(int64)(l_oTimeValue.tv_sec-l_oTimeValueStart.tv_sec);
	int64 l_i64USecDiff=(int64)(l_oTimeValue.tv_usec-l_oTimeValueStart.tv_usec);

	l_i64TimeMicroSecond+=l_i64SecDiff*1000000;
	l_i64TimeMicroSecond+=l_i64USecDiff;

	l_ui64Result=((l_i64TimeMicroSecond/1000000)<<32)+(((l_i64TimeMicroSecond%1000000)<<32)/1000000);

#elif defined System_OS_Windows

#if 0
	static boolean l_bInitialized=false;
	static uint64 l_ui64Frequency;
	static uint64 l_ui64CounterStart;
	uint64 l_ui64Counter;

	if(!l_bInitialized)
	{
		LARGE_INTEGER l_oPerformanceFrequency;
		QueryPerformanceFrequency(&l_oPerformanceFrequency);
		l_ui64Frequency=l_oPerformanceFrequency.QuadPart;

		LARGE_INTEGER l_oPerformanceCounterStart;
		QueryPerformanceCounter(&l_oPerformanceCounterStart);
		l_ui64CounterStart=l_oPerformanceCounterStart.QuadPart;

		l_bInitialized=true;
	}

	LARGE_INTEGER l_oPerformanceCounter;
	QueryPerformanceCounter(&l_oPerformanceCounter);
	l_ui64Counter=l_oPerformanceCounter.QuadPart-l_ui64CounterStart;

	l_ui64Result=((l_ui64Counter/l_ui64Frequency)<<32)+(((l_ui64Counter%l_ui64Frequency)<<32)/l_ui64Frequency);

#elif 0

	static boolean l_bInitialized=false;
	static uint64 l_ui64CounterStart;
	uint64 l_ui64Counter;

	l_ui64Counter=uint64(timeGetTime());
	l_ui64Counter=((l_ui64Counter/1000)<<32)+(((l_ui64Counter%1000)<<32)/1000);

	if(!l_bInitialized)
	{
		l_ui64CounterStart=l_ui64Counter;
		l_bInitialized=true;
	}

	l_ui64Result=l_ui64Counter-l_ui64CounterStart;

#else

	l_ui64Result=win32_zgetTime();

#endif

#else
#endif

	return l_ui64Result;
}
