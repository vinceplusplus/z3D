
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		Z3D_API Vec3		rgb2hsl		(const Vec3& rgb);
		Z3D_API Vec3		hsl2rgb		(const Vec3& hsl);
		Z3D_API Vec3		rgb2hsv		(const Vec3& rgb);
		Z3D_API Vec3		hsv2rgb		(const Vec3& hsv);
		Z3D_API Vec3		hsl2hsv		(const Vec3& hsl);
		Z3D_API Vec3		hsv2hsl		(const Vec3& hsv);
	};
};

