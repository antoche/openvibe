#ifndef __System_Memory_H__
#define __System_Memory_H__

#include "defines.h"

namespace System
{
	class System_API Memory
	{
	public:

		static System::boolean copy(void* pTargetBuffer, const void* pSourceBuffer, const System::uint64 ui64BufferSize);
		static System::boolean set(void* pTargetBuffer, const System::uint64 ui64BufferSize, const System::uint8 ui8Value);
		static System::boolean compare(const void* pSourceBuffer1, const void* pSourceBuffer2, const System::uint64 ui64BufferSize);

		static System::boolean hostToLittleEndian(const System::uint16 ui16Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::uint32 ui32Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::uint64 ui64Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::int16 i16Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::int32 i32Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::int64 i64Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::float32 f32Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::float64 f64Value, System::uint8* pBuffer);
		static System::boolean hostToLittleEndian(const System::float80 f80Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::uint16 ui16Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::uint32 ui32Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::uint64 ui64Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::int16 i16Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::int32 i32Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::int64 i64Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::float32 f32Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::float64 f64Value, System::uint8* pBuffer);
		static System::boolean hostToBigEndian(const System::float80 f80Value, System::uint8* pBuffer);

		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::uint16* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::uint32* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::uint64* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::int16* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::int32* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::int64* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::float32* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::float64* pValue);
		static System::boolean littleEndianToHost(const System::uint8* pBuffer, System::float80* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::uint16* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::uint32* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::uint64* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::int16* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::int32* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::int64* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::float32* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::float64* pValue);
		static System::boolean bigEndianToHost(const System::uint8* pBuffer, System::float80* pValue);

	private:

		Memory(void);
	};
};

#endif // __System_Memory_H__
