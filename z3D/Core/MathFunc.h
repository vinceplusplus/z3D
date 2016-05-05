
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// all angles are in degrees unless specified

		Z3D_API REAL			SIN				(REAL x);
		Z3D_API REAL			COS				(REAL x);
		Z3D_API REAL			TAN				(REAL x);
		Z3D_API REAL			ASIN			(REAL x);
		Z3D_API REAL			ACOS			(REAL x);
		Z3D_API REAL			ATAN			(REAL x);
		Z3D_API REAL			ATAN2			(REAL y, REAL x);
		Z3D_API REAL			SQR				(REAL x);
		Z3D_API REAL			SQRT			(REAL x);
		Z3D_API REAL			INVSQRT			(REAL x);
		Z3D_API REAL			FABS			(REAL x);
		Z3D_API char			SIGNBIT			(REAL x);
		Z3D_API REAL			SETSIGN			(REAL x, char signbit);
		Z3D_API REAL			COPYSIGN		(REAL x, REAL sign);
		Z3D_API void			SORT			(REAL& x, REAL& y);
		Z3D_API bool			EQUAL			(REAL x, REAL y, REAL bias_percentage);
		Z3D_API bool			EQUAL2			(REAL x, REAL y, REAL bias);
		Z3D_API bool			FINITE			(REAL x);
		Z3D_API REAL			CEIL			(REAL x);
		Z3D_API REAL			FLOOR			(REAL x);
		Z3D_API REAL			POW				(REAL x, REAL y);
		Z3D_API REAL			EXP				(REAL x);
		Z3D_API REAL			LOG				(REAL x);
		Z3D_API REAL			LOG10			(REAL x);
		Z3D_API REAL			FMOD			(REAL x, REAL y);
		Z3D_API REAL			FRAC			(REAL x);
		Z3D_API REAL			CLAMP			(REAL x, REAL min_, REAL max_);
		Z3D_API REAL			SATURATE		(REAL x);
		Z3D_API REAL			CONVERT_FOV		(REAL x_fov, REAL xy_aspect_ratio);			// return y fov given x fov and xy aspect ratio
		Z3D_API REAL			CONVERT_FOCAL_LENGTH_TO_FOV	(REAL focal_length_in_mm);		// return x fov given the lens focal length in mm
		Z3D_API REAL			CONVERT_FOV_TO_FOCAL_LENGTH	(REAL x_fov);					// return the lens focal length in mm given the x fov
		Z3D_API int32_t			NINT			(REAL x);									// nearest integer
		Z3D_API REAL			LERP_ANGLE(REAL x, REAL y, REAL dt);						// lerp angle, will take care 360 deg wrap around
		Z3D_API uint32_t		NEXT_POW2		(uint32_t x);								// return an unsigned integer such that return_value == 2 ^ N, N is non-negative number and return_value >= x
		Z3D_API bool			IS_POW2			(uint32_t x);
		Z3D_API uint32_t		BITCOUNT		(uint32_t x);
		Z3D_API uint32_t		FIRSTBIT		(uint8_t x);								// if x is zero, 7 will be returned (incorrect), so before use, check if not zero
		Z3D_API REAL			LINEAR_MAPPING	(REAL x0, REAL x1, REAL y0, REAL y1, REAL x);
		Z3D_API uint64_t		MAKEINT64		(uint32_t loint, uint32_t hiint);
		Z3D_API uint8_t			REVERSE_BITS	(uint8_t x);
		Z3D_API uint16_t		REVERSE_BITS	(uint16_t x);
		Z3D_API uint32_t		REVERSE_BITS	(uint32_t x);
		Z3D_API uint64_t		REVERSE_BITS	(uint64_t x);
		Z3D_API uint32_t		GCD				(uint32_t a, uint32_t b);

		template<class T>
		void					SORT(T& x, T& y)
		{
			if(x > y)
			{
				T t = x;
				x = y;
				y = t;
			}
		}
	};
};

