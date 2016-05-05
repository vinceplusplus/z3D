
#pragma once

#include "predecl.h"
#include "Plane.h"
#include "Matrix4.h"

namespace z3D
{
	namespace Core
	{
		class Frustum
		{
		public:
			typedef enum
			{
				FC_OUTSIDE = 0,
				FC_INSIDE,
				FC_INTERSECT
			} FRUSTUMCULL;
		private:
			Mat4			_m;						// original matrix
			Plane			_p[12];					// with normals pointing inside, 0 - left, 1 - right, 2 - bottom, 3 - up, 4 - near, 5 - far, hereafter, additional, support up to 12 planes
			size_t			_planes;
		public:
			Frustum(const Mat4& m, REAL znear_mapping, REAL zfar_mapping, const vector<Plane>& additional_planes = vector<Plane>());
		public:
			FRUSTUMCULL		contains(const AABB& aabb) const;
			FRUSTUMCULL		contains(const OBB& obb) const;
			FRUSTUMCULL		contains(const Vec3& p) const;
			FRUSTUMCULL		contains(const Sphere& s) const;
			FRUSTUMCULL		contains(const vector<Vec4>& array_point) const;
		public:
			const Mat4&		getMatrix() const;
			const Plane&	getPlane(size_t i) const;
			size_t			getPlanes() const;
		};
	};
};


