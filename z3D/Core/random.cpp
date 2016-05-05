
#include "random.h"

#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedCompareExchange64, _InterlockedIncrement, _InterlockedDecrement, _InterlockedExchangeAdd, _InterlockedExchange, __rdtsc)

namespace z3D
{
	namespace Core
	{
		lrng::lrng()
		{
			_a = ((0x1000000000000I64) + ((uint32_t)__rdtsc())) * 16 + 1;
			_b = ((0x1000000000000I64) + ((uint32_t)__rdtsc())) * 8 + 1;
			_x0 = (uint32_t)__rdtsc();
		}
		lrng::lrng(uint64_t seed)
		{
			_a = ((0x1000000000000I64) + ((uint32_t)__rdtsc())) * 16 + 1;
			_b = ((0x1000000000000I64) + ((uint32_t)__rdtsc())) * 8 + 1;
			_x0 = seed;
		}
		uint32_t				lrng::next()
		{
			return (uint32_t)((_x0 = _a * _x0 + _b) >> 32);
		}
		double					lrng::nextDouble()
		{
			return next() / ((double)0x100000000I64);
		}
		uint32_t				lrng::nextRange(uint32_t minimum, uint32_t maximum)
		{
			return (uint32_t)(minimum + (next() % ((uint64_t)maximum - minimum + 1)));
		}
		double					lrng::nextDoubleRange(double minimum, double maximum)
		{
			return min(max(minimum, minimum + ( next() / ( (double)(0xffffffffI64) ) )*( maximum - minimum )), maximum);
		}
		uint32_t				lrng::randSpecial(uint64_t& x0)
		{
			uint64_t a = (x0<<4) + 1;
			uint64_t b = (x0<<3) + 1;
			return (uint32_t)((x0 = a * x0 + b) >> 32);
		}
		uint32_t				lrng::randSpecial2(uint64_t& x0)
		{
			uint64_t a = (x0<<5) + 1;
			uint64_t b = (x0<<4) + 1;
			return (uint32_t)((x0 = a * x0 + b) >> 32);
		}
	};
};

