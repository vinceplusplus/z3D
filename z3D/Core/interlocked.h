
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class interlocked
		{
		public:
			static uint32_t		compare_exchange(uint32_t volatile * dest, uint32_t exchange, uint32_t comperand);
			static uint64_t		compare_exchange_64(uint64_t volatile * dest, uint64_t exchange, uint64_t comperand);
			static uint32_t		increment(uint32_t volatile * augend);
			static uint32_t		decrement(uint32_t volatile * minuend);
			static uint32_t		exchange_add(uint32_t volatile * augend, uint32_t addend);
			static uint32_t		exchange(uint32_t volatile * target, uint32_t value);
		};
	};
};

