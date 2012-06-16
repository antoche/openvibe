#include "xml/IReader.h"
#include "xml/TReaderCallbackProxy.h"
#include "xml/IWriter.h"
#include "xml/TWriterCallbackProxy.h"

#define __check_different_type_size__(t1,t2) \
	static bool __check_different_type_size_value_##t1##t2=(1/(sizeof(t1)-sizeof(t2))==0); \
	__check_different_type_size_value_##t1##t2=false; /* avoids "defined but not used" warning */

namespace XML
{
	namespace
	{
		static void nothing(void)
		{
			__check_different_type_size__(uint64, uint32);
			__check_different_type_size__(uint64, uint16);
			__check_different_type_size__(uint64, uint8);
			__check_different_type_size__(uint32, uint16);
			__check_different_type_size__(uint32, uint8);
			__check_different_type_size__(uint16, uint8);

			__check_different_type_size__(int64, int32);
			__check_different_type_size__(int64, int16);
			__check_different_type_size__(int64, int8);
			__check_different_type_size__(int32, int16);
			__check_different_type_size__(int32, int8);
			__check_different_type_size__(int16, int8);

#ifndef XML_OS_Windows
			__check_different_type_size__(float80, float64);
			__check_different_type_size__(float80, float32);
#endif
			__check_different_type_size__(float64, float32);

			nothing(); /* avoids "defined but not used" warning */
		}
	}
}
