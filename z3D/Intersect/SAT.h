
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Intersect
	{
		class SAT
		{
		public:
			static Vec3&			stablize_axis(Vec3& axis);
			static const Vec3		stablize_axis(const Vec3& axis);
			static bool				test_intervals_sep(REAL min0, REAL max0, REAL min1, REAL max1);
			static bool				test_1axis_obb_tri_sep(const Vec3& axis, const Vec3& obb_center, const Vec3 array_obb_eaxis[3], const Vec3& v0, const Vec3& v1, const Vec3& v2);
			static bool				cd_1axis_moving_obb_tri(const Vec3& axis, const Vec3& obb_center, const Vec3 array_obb_eaxis[3], const Vec3& obb_velocity, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis);
			static bool				cd_1axis_moving_sphere_tri(const Vec3& axis, const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis);				// assuming axis is a unit vector
		};
	};
};
