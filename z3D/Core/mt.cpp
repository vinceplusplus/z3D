
#include "mt.h"

namespace z3D
{
	namespace Core
	{
		mt::mt() : _mti(N + 1)					/* mti == N + 1 means mt[N] is not initialized */
		{
		}
		void			mt::init_genrand(uint32_t s)
		{
			_mt[0] = s & 0xffffffffUL;
			for (_mti = 1; _mti < N; ++_mti)
			{
				_mt[_mti] = 
				(1812433253UL * (_mt[_mti - 1] ^ (_mt[_mti - 1] >> 30)) + _mti); 
				/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
				/* In the previous versions, MSBs of the seed affect   */
				/* only MSBs of the array mt[].                        */
				/* 2002/01/09 modified by Makoto Matsumoto             */
				_mt[_mti] &= 0xffffffffUL;
				/* for >32 bit machines */
			}
		}
		void			mt::init_by_array(uint32_t init_key[], size_t key_length)
		{
			int32_t i, j, k;
			init_genrand(19650218UL);
			i=1; j=0;
			k = (N > key_length ? N : key_length);
			for (; k; --k) {
				_mt[i] = (_mt[i] ^ ((_mt[i-1] ^ (_mt[i-1] >> 30)) * 1664525UL))
				  + init_key[j] + j; /* non linear */
				_mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
				i++; j++;
				if (i >= N) { _mt[0] = _mt[N - 1]; i = 1; }
				if (j >= (int32_t)key_length) j = 0;
			}
			for (k = N - 1; k; --k) {
				_mt[i] = (_mt[i] ^ ((_mt[i-1] ^ (_mt[i - 1] >> 30)) * 1566083941UL))
				  - i; /* non linear */
				_mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
				i++;
				if (i >= N) { _mt[0] = _mt[N - 1]; i = 1; }
			}

			_mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
		}
		uint32_t		mt::genrand_int32(void)
		{
			uint32_t y;
			const uint32_t mag01[2] = {0x0UL, MATRIX_A};
			/* mag01[x] = x * MATRIX_A  for x=0,1 */

			if (_mti >= N) { /* generate N words at one time */
				int32_t kk;

				if (_mti == N + 1)   /* if init_genrand() has not been called, */
					init_genrand(5489UL); /* a default initial seed is used */

				for (kk = 0; kk < N - M; ++kk) {
					y = (_mt[kk] & UPPER_MASK) | (_mt[kk + 1] & LOWER_MASK);
					_mt[kk] = _mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
				}
				for (; kk < N - 1; ++kk) {
					y = (_mt[kk] & UPPER_MASK) | (_mt[kk + 1] & LOWER_MASK);
					_mt[kk] = _mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
				}
				y = (_mt[N - 1] & UPPER_MASK) | (_mt[0] & LOWER_MASK);
				_mt[N - 1] = _mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

				_mti = 0;
			}
		  
			y = _mt[_mti++];

			/* Tempering */
			y ^= (y >> 11);
			y ^= (y << 7) & 0x9d2c5680UL;
			y ^= (y << 15) & 0xefc60000UL;
			y ^= (y >> 18);

			return y;
		}
		int32_t			mt::genrand_int31(void)
		{
			return (int32_t)(genrand_int32() >> 1);
		}
		double			mt::genrand_real1(void)
		{
			return genrand_int32() * (1.0 / 4294967295.0); 
			/* divided by 2^32-1 */ 
		}
		double			mt::genrand_real2(void)
		{
			return genrand_int32() * (1.0 / 4294967296.0); 
			/* divided by 2^32 */
		}
		double			mt::genrand_real3(void)
		{
			return (((double)genrand_int32()) + 0.5) * (1.0 / 4294967296.0); 
			/* divided by 2^32 */
		}
		double			mt::genrand_res53(void) 
		{ 
			uint32_t a = genrand_int32() >> 5, b = genrand_int32() >> 6; 
			return(a * 67108864.0 + b) * (1.0 / 9007199254740992.0); 
		} 
	};
};

