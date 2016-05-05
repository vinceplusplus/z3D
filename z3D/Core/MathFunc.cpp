
#include "Constants.h"
#include "MathFunc.h"
#include <math.h>
#include <float.h>

namespace z3D
{
	namespace Core
	{
		REAL SIN(REAL x)
		{
			return (REAL)sin((double)x * DEG_TO_RAD);
		}
		REAL COS(REAL x)
		{
			return (REAL)cos((double)x * DEG_TO_RAD);
		}
		REAL TAN(REAL x)
		{
			return (REAL)tan((double)x * DEG_TO_RAD);
		}
		REAL ASIN(REAL x)
		{
			x = min(max(-1.0f, x), 1.0f);
			return (REAL)(asin((double)x) * RAD_TO_DEG);
		}
		REAL ACOS(REAL x)
		{
			x = min(max(-1.0f ,x), 1.0f);
			return (REAL)(acos((double)x) * RAD_TO_DEG);
		}
		REAL ATAN(REAL x)
		{
			return (REAL)(atan((double)x) * RAD_TO_DEG);
		}
		REAL ATAN2(REAL y, REAL x)
		{
			return (REAL)(atan2((double)y, (double)x) * RAD_TO_DEG);
		}
		REAL SQR(REAL x)
		{
			return x * x;
		}
		REAL INVSQRT(REAL x)
		{
			// Chris Lomont's Fast Inverse Square Root
			float xhalf = 0.5f * x;
			int32_t i = *(int32_t*)&x; // get bits for floating value
			i = 0x5f375a86 - (i >> 1); // gives initial guess y0
			x = *(float*)&i; // convert bits back to float
			x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
			x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
			return x;
		}
		REAL SQRT(REAL x)
		{
			return (REAL)sqrt((double)x);
		}
		REAL FABS(REAL x)
		{
			int32_t y;
			return *(float*)&(y = (*(int32_t*)&x) & 0x7fffffff);
		}
		char	SIGNBIT	(REAL x)
		{
			return (*(uint32_t*)&x) >> 31;
		}
		REAL	SETSIGN	(REAL x, char signbit)
		{
			REAL y = x;
			(*(int32_t*)&y) = ((*(uint32_t*)&y) & (~(((int32_t)1) << 31))) | (((int32_t)signbit) << 31);
			return y;
		}
		REAL	COPYSIGN(REAL x, REAL sign)
		{
			Z_STATIC_ASSERT( sizeof(REAL) == sizeof(float), WRONG_ASSUMPTION1 );
			Z_STATIC_ASSERT( sizeof(REAL) == sizeof(uint32_t), WRONG_ASSUMPTION2 );

			*(uint32_t*)&x |= (*(uint32_t*)&sign) & 0x80000000;
			return x;
		}
		void SORT(REAL& x, REAL& y)
		{
			if(x > y)
			{
				*(int32_t*)&x = (*(int32_t*)&x) ^ (*(int32_t*)&y);
				*(int32_t*)&y = (*(int32_t*)&x) ^ (*(int32_t*)&y);
				*(int32_t*)&x = (*(int32_t*)&x) ^ (*(int32_t*)&y);
			}
		}
		bool	EQUAL	(REAL x, REAL y, REAL bias_percentage)
		{
			REAL percentage = x / y;
			if(percentage >= 1 - bias_percentage && percentage <= 1 + bias_percentage)
				return true;
			return false;
		}
		bool	EQUAL2	(REAL x, REAL y, REAL bias)
		{
			if((x >= y - bias && x <= y + bias) || (y >= x - bias && y <= x + bias))
				return true;
			return false;
		}
		bool	FINITE(REAL x)
		{
			return _finite((double)x) ? true : false;
		}
		REAL CEIL(REAL x)
		{
			return (REAL)ceil(x);
		}
		REAL FLOOR(REAL x)
		{
			return (REAL)ceil(x);
		}
		REAL	POW(REAL x,REAL y)
		{
			return (REAL)pow(x,y);
		}
		REAL	EXP(REAL x)
		{
			return (REAL)exp(x);
		}
		REAL	LOG(REAL x)
		{
			return (REAL)log(x);
		}
		REAL	LOG10(REAL x)
		{
			return (REAL)log10(x);
		}
		REAL	FMOD(REAL x,REAL y)
		{
			return fmod(x,y);
		}
		REAL	FRAC(REAL x)
		{
			return FABS(x) - (REAL)((int32_t)FABS(x));
		}
		REAL	CLAMP	(REAL x, REAL min_, REAL max_)
		{
			return min(max(min_, x), max_);
		}
		REAL	SATURATE	(REAL x)
		{
			uint32_t y = (*(uint32_t*)&x) & 0x7fffffff;

			float z = min(*(float*)&y, 1.0f);
			*(uint32_t*)&z |= (*(uint32_t*)&x) & 0x80000000;

			return z;
		}
		REAL	CONVERT_FOV	(REAL x_fov, REAL xy_aspect_ratio)
		{
			return ATAN(TAN(x_fov * 0.5f) / xy_aspect_ratio) * 2.0f;
		}
		REAL			CONVERT_FOCAL_LENGTH_TO_FOV	(REAL focal_length_in_mm)
		{
			return 2.0f * ATAN(18.0f / focal_length_in_mm);
		}
		REAL			CONVERT_FOV_TO_FOCAL_LENGTH	(REAL x_fov)
		{
			return 18.0f / TAN(x_fov * 0.5f);
		}
		int32_t	NINT(REAL x)
		{
			char signbit = SIGNBIT(x);
			int32_t y = (int32_t)(SETSIGN(x, 0) + 0.5f);
			return signbit ? -y : y;
		}
		REAL			LERP_ANGLE(REAL x, REAL y, REAL dt)
		{
			uint16_t x0 = ((uint32_t)(x * (65536.0 / 360.0)) & 0xffffI32);
			uint16_t y0 = ((uint32_t)(y * (65536.0 / 360.0)) & 0xffffI32);
			int32_t d = (int32_t)y0 - (int32_t)x0;
			if(d >= -32767 && d <= 32767)
				return (REAL)(((int16_t)((int32_t)((1 - dt) * x0 + dt * y0) & 0xffffI32)) * (360.0 / 65536.0));
			return (REAL)(((1 - dt) * ((int16_t)x0) + dt * ((int16_t)y0)) * (360.0 / 65536.0));
		}
		uint32_t	NEXT_POW2(uint32_t x)
		{
			// http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float
			--x;
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;
			++x;

			x += (x == 0);

			return x;
			/*
			long y;
			if(x<0x100)
				y=0x100;
			else if(x<0x10000)
				y=0x10000;
			else if(x<1000000)
				y=0x1000000;
			else
				y=0x40000000;
			while(y>x)
				if((y>>=1)<x)
					return y<<1;
			return y;
			*/
		}
		bool		IS_POW2		(uint32_t x)
		{
			return !(x & (x - 1));
		}
		uint32_t	BITCOUNT(uint32_t x)
		{
			// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan

			uint32_t c; // c accumulates the total bits set in x
			for (c = 0; x; c++)
			{
			  x &= x - 1; // clear the least significant bit set
			}
			return c;
		}
		uint32_t	FIRSTBIT(uint8_t x)
		{
			if(x & 0x0f)
			{
				if(x & 0x03)
				{
					if(x & 0x01)
						return 0;
					return 1;
				}
				if(x & 0x04)
					return 2;
				return 3;
			}
			if(x & 0x30)
			{
				if(x & 0x10)
					return 4;
				return 5;
			}
			if(x & 0x40)
				return 6;
			return 7;
		}
		REAL			LINEAR_MAPPING		(REAL x0,REAL x1,REAL y0,REAL y1,REAL x)
		{
			return y0 + ((x - x0) / (x1 - x0)) * (y1 - y0);
		}
		uint64_t			MAKEINT64 ( uint32_t loint, uint32_t hiint )
		{
			return ( ((uint64_t) hiint) << 32 ) | ( loint );
		}
		uint8_t			REVERSE_BITS(uint8_t x)
		{
			x = (x >> 4) | (x << 4);
			x = (((x & 0x33)) << 2) | (((x & 0xcc)) >> 2);
			x = (((x & 0x55)) << 1) | (((x & 0xaa)) >> 1);
			return x;
		}

		uint16_t		REVERSE_BITS(uint16_t x)
		{
			x = (x >> 8) | (x << 8);
			x = (((x & 0x0f0f)) << 4) | (((x & 0xf0f0)) >> 4);
			x = (((x & 0x3333)) << 2) | (((x & 0xcccc)) >> 2);
			x = (((x & 0x5555)) << 1) | (((x & 0xaaaa)) >> 1);
			return x;
		}

		uint32_t		REVERSE_BITS(uint32_t x)
		{
			x = (x >> 16) | (x << 16);
			x = (((x & 0x00ff00ff)) << 8) | (((x & 0xff00ff00)) >> 8);
			x = (((x & 0x0f0f0f0f)) << 4) | (((x & 0xf0f0f0f0)) >> 4);
			x = (((x & 0x33333333)) << 2) | (((x & 0xcccccccc)) >> 2);
			x = (((x & 0x55555555)) << 1) | (((x & 0xaaaaaaaa)) >> 1);
			return x;
		}

		uint64_t		REVERSE_BITS(uint64_t x)
		{
			x = (x >> 32) | (x << 32);
			x = (((x & 0x0000ffff0000ffff)) << 16) | (((x & 0xffff0000ffff0000)) >> 16);
			x = (((x & 0x00ff00ff00ff00ff)) << 8) | (((x & 0xff00ff00ff00ff00)) >> 8);
			x = (((x & 0x0f0f0f0f0f0f0f0f)) << 4) | (((x & 0xf0f0f0f0f0f0f0f0)) >> 4);
			x = (((x & 0x3333333333333333)) << 2) | (((x & 0xcccccccccccccccc)) >> 2);
			x = (((x & 0x5555555555555555)) << 1) | (((x & 0xaaaaaaaaaaaaaaaa)) >> 1);
			return x;
		}

		uint32_t		GCD(uint32_t a, uint32_t b)
		{
			if(b) return GCD(b, a % b);
			return a;
		}
	};
};
