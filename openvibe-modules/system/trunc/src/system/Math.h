#ifndef __System_Math_H__
#define __System_Math_H__

#include "defines.h"

namespace System
{
	class System_API Math
	{
	public:

		static System::boolean initializeRandomMachine(const System::uint64 ui64RandomSeed);

		static System::uint8 randomUInteger8(void);
		static System::uint16 randomUInteger16(void);
		static System::uint32 randomUInteger32(void);
		static System::uint64 randomUInteger64(void);

		static System::int8 randomSInterger8(void);
		static System::int16 randomSInterger16(void);
		static System::int32 randomSInterger32(void);
		static System::int64 randomSInterger64(void);

		static System::float32 randomFloat32(void);
		static System::float64 randomFloat64(void);

	private:

		Math(void);
	};
};

#endif // __System_Math_H__
