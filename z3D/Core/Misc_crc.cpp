
#include "Misc.h"

#include "../../crc/crc.h"

namespace z3D
{
	namespace Core
	{
		uint32_t			gen_crc(const void* array_byte, size_t bytes)
		{
			return crcFast((const unsigned char*)array_byte, bytes);
		}
	};
};

