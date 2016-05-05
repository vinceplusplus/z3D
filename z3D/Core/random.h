
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class lrng
		{
		private:
			uint64_t				_a;
			uint64_t				_b;
			uint64_t				_x0;
		public:
			lrng();
			lrng(uint64_t seed);
		public:
			uint32_t				next();
			double					nextDouble();													// return 0 <= x < 1
			uint32_t				nextRange(uint32_t minimum, uint32_t maximum);
			double					nextDoubleRange(double minimum, double maximum);
		public:
			static uint32_t			randSpecial(uint64_t& x0);										// 4 periods, with length 2^30 long each
			static uint32_t			randSpecial2(uint64_t& x0);										// same as the above
		};
	};
};

