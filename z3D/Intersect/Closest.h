
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Intersect
	{
		class Closest
		{
		public:
			static void				PointTriangle(const Vec3& p, const Triangle& tri, Vec3& output);
			static void				PointAABB(const Vec3& p, const AABB& aabb, Vec3& output);
		};
	};
};

