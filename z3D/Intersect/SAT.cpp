
#include "SAT.h"

namespace z3D
{
	namespace Intersect
	{
		Vec3&				SAT::stablize_axis(Vec3& axis)
		{
			REAL lensq = axis.lengthsq();
			if(lensq && (lensq < 0.001 || lensq > 1000.0f))
				axis.normalize_safe();								// accounts for zero length case
			return axis;
		}
		const Vec3			SAT::stablize_axis(const Vec3& axis)
		{
			REAL lensq = axis.lengthsq();
			if(lensq && (lensq < 0.001 || lensq > 1000.0f))
				return axis.normal_safe();							// accounts for zero length case
			return axis;
		}
		bool				SAT::test_intervals_sep(REAL min0, REAL max0, REAL min1, REAL max1)
		{
			return (min0 > max1 || min1 > max0);
		}
		bool				SAT::test_1axis_obb_tri_sep(const Vec3& axis, const Vec3& obb_center, const Vec3 array_obb_eaxis[3], const Vec3& v0, const Vec3& v1, const Vec3& v2)
		{
			REAL min0, max0, min1, max1, temp;

			REAL obb_radius = FABS(array_obb_eaxis[0] * axis) + FABS(array_obb_eaxis[1] * axis) + FABS(array_obb_eaxis[2] * axis);
			temp = obb_center * axis;
			min0 = temp - obb_radius;
			max0 = temp + obb_radius;
			min1 = max1 = v0 * axis;
			min1 = min(min1, temp = v1 * axis);
			max1 = max(max1, temp);
			min1 = min(min1, temp = v2 * axis);
			max1 = max(max1, temp);

			return test_intervals_sep(min0, max0, min1, max1);
		}
		bool				SAT::cd_1axis_moving_obb_tri(const Vec3& axis, const Vec3& obb_center, const Vec3 array_obb_eaxis[3], const Vec3& obb_velocity, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis)
		{
			REAL min0, max0, min1, max1, temp;

			REAL obb_radius = FABS(array_obb_eaxis[0] * axis) + FABS(array_obb_eaxis[1] * axis) + FABS(array_obb_eaxis[2] * axis);
			temp = obb_center * axis;
			min0 = temp - obb_radius;
			max0 = temp + obb_radius;
			min1 = max1 = v0 * axis;
			min1 = min(min1, temp = v1 * axis);
			max1 = max(max1, temp);
			min1 = min(min1, temp = v2 * axis);
			max1 = max(max1, temp);

			REAL delta = obb_velocity * axis;
			if(FABS(delta) < EPSILON)
				return !test_intervals_sep(min0, max0, min1, max1);

			REAL inv_delta = 1.0f / delta;
			REAL mini = (min1 - max0) * inv_delta;
			REAL maxi = (max1 - min0) * inv_delta;
			SORT(mini, maxi);

			if(first_time_contact_axis && mini > t0)
				*first_time_contact_axis = axis;
			if(last_time_contact_axis && maxi < t1)
				*last_time_contact_axis = axis;
			t0 = max(t0, mini);
			t1 = min(t1, maxi);

			return true;
		}
		bool				SAT::cd_1axis_moving_sphere_tri(const Vec3& axis, const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis)
		{
			REAL min0, max0, min1, max1, temp;

			temp = so * axis;
			min0 = temp - sr;
			max0 = temp + sr;

			min1 = max1 = v0 * axis;
			min1 = min(min1, temp = v1 * axis);
			max1 = max(max1, temp);
			min1 = min(min1, temp = v2 * axis);
			max1 = max(max1, temp);

			REAL delta = w * axis;
			if(FABS(delta) < EPSILON)
				return !test_intervals_sep(min0, max0, min1, max1);

			REAL inv_delta = 1.0f / delta;
			REAL mini = (min1 - max0) * inv_delta;
			REAL maxi = (max1 - min0) * inv_delta;
			SORT(mini, maxi);

			if(first_time_contact_axis && mini > t0)
				*first_time_contact_axis = axis;
			if(last_time_contact_axis && maxi < t1)
				*last_time_contact_axis = axis;
			t0 = max(t0, mini);
			t1 = min(t1, maxi);

			return true;
		}
	};
};
