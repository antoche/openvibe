#include "Math.h"
#include <cstdlib>
#include <cstring>

using namespace System;

boolean Math::initializeRandomMachine(const uint64 ui64RandomSeed)
{
	srand((unsigned int)ui64RandomSeed);
	return true;
}

uint8 Math::randomUInteger8(void)
{
	return static_cast<uint8>(randomUInteger64());
}

uint16 Math::randomUInteger16(void)
{
	return static_cast<uint16>(randomUInteger64());
}

uint32 Math::randomUInteger32(void)
{
	return static_cast<uint32>(randomUInteger64());
}

uint64 Math::randomUInteger64(void)
{
	uint64 r1=rand();
	uint64 r2=rand();
	uint64 r3=rand();
	uint64 r4=rand();
	return (r1<<24)^(r2<<16)^(r3<<8)^(r4);
}

int8 Math::randomSInterger8(void)
{
	return static_cast<int8>(randomUInteger64());
}

int16 Math::randomSInterger16(void)
{
	return static_cast<int16>(randomUInteger64());
}

int32 Math::randomSInterger32(void)
{
	return static_cast<int32>(randomUInteger64());
}

int64 Math::randomSInterger64(void)
{
	return static_cast<int64>(randomUInteger64());
}

float32 Math::randomFloat32(void)
{
	uint32 r=randomUInteger32();
	float32 fr;
	::memcpy(&fr, &r, sizeof(fr));
	return fr;
}

float64 Math::randomFloat64(void)
{
	uint64 r=randomUInteger64();
	float64 fr;
	::memcpy(&fr, &r, sizeof(fr));
	return fr;
}
