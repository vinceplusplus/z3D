
#include "predecl.h"
#include "interlocked.h"

#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedCompareExchange64, _InterlockedIncrement, _InterlockedDecrement, _InterlockedExchangeAdd, _InterlockedExchange)

namespace z3D
{
	namespace Core
	{
		uint32_t		interlocked::compare_exchange(uint32_t volatile * dest, uint32_t exchange, uint32_t comperand)
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) ==  sizeof(long), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( Z_GET_ALIGNMENT<uint32_t>::value ==  Z_GET_ALIGNMENT<long>::value, WRONG_ASSUMPTION );
			Z_ASSERT_ALIGN((void*)dest, sizeof(uint32_t));

			return (uint32_t)_InterlockedCompareExchange((long volatile *)dest, (long)exchange, (long)comperand);
		}
		uint64_t		interlocked::compare_exchange_64(uint64_t volatile * dest, uint64_t exchange, uint64_t comperand)
		{
			Z_STATIC_ASSERT( sizeof(uint64_t) ==  sizeof(__int64), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( Z_GET_ALIGNMENT<uint64_t>::value ==  Z_GET_ALIGNMENT<__int64>::value, WRONG_ASSUMPTION );
			Z_ASSERT_ALIGN((void*)dest, sizeof(uint64_t));

			return (uint64_t)_InterlockedCompareExchange64((__int64 volatile *)dest, (__int64)exchange, (__int64)comperand);
		}
		uint32_t		interlocked::increment(uint32_t volatile * augend)
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) ==  sizeof(long), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( Z_GET_ALIGNMENT<uint32_t>::value ==  Z_GET_ALIGNMENT<long>::value, WRONG_ASSUMPTION );
			Z_ASSERT_ALIGN((void*)augend, sizeof(uint32_t));

			return (uint32_t)_InterlockedIncrement((long volatile *)augend);
		}
		uint32_t		interlocked::decrement(uint32_t volatile * minuend)
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) ==  sizeof(long), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( Z_GET_ALIGNMENT<uint32_t>::value ==  Z_GET_ALIGNMENT<long>::value, WRONG_ASSUMPTION );
			Z_ASSERT_ALIGN((void*)minuend, sizeof(uint32_t));

			return (uint32_t)_InterlockedDecrement((long volatile *)minuend);
		}
		uint32_t		interlocked::exchange_add(uint32_t volatile * augend, uint32_t addend)
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) ==  sizeof(long), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( Z_GET_ALIGNMENT<uint32_t>::value ==  Z_GET_ALIGNMENT<long>::value, WRONG_ASSUMPTION );
			Z_ASSERT_ALIGN((void*)augend, sizeof(uint32_t));

			return (uint32_t)_InterlockedExchangeAdd((long volatile *)augend, (long)addend);
		}
		uint32_t		interlocked::exchange(uint32_t volatile * target, uint32_t value)
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) ==  sizeof(long), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( Z_GET_ALIGNMENT<uint32_t>::value ==  Z_GET_ALIGNMENT<long>::value, WRONG_ASSUMPTION );
			Z_ASSERT_ALIGN((void*)target, sizeof(uint32_t));

			return (uint32_t)_InterlockedExchange((long volatile *)target, (long)value);
		}
	};
};

