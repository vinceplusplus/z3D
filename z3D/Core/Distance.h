
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class Distance
		{
		public:
			static REAL PointLine(const Vec3& p, const Line& l);
			static REAL PointLineSeg(const Vec3& p, const Line& l);
			static REAL PointAABB(const Vec3& p, const AABB& aabb);
			static REAL PointOBB(const Vec3& p, const OBB& obb);
			static REAL PointTriangle(const Vec3& p, const Triangle& tri);
			static REAL PointTriangle(const Vec3& p, const TriangleAux& tri);
		};
	};
};
