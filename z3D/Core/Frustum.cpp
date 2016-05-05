
#include "Frustum.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "AABB.h"
#include "MathFunc.h"
#include "OBB.h"
#include "Sphere.h"

namespace z3D
{
	namespace Core
	{
		Frustum::Frustum(const Mat4& m, REAL znear_mapping, REAL zfar_mapping, const vector<Plane>& additional_planes)
		{
			_m = m;

			const REAL& N = znear_mapping;
			const REAL& F = zfar_mapping;

			_p[0] = Plane( m.a11 + m.a14,  m.a21 + m.a24,  m.a31 + m.a34,  m.a41 + m.a44);
			_p[1] = Plane(-m.a11 + m.a14, -m.a21 + m.a24, -m.a31 + m.a34, -m.a41 + m.a44);
			_p[2] = Plane( m.a12 + m.a14,  m.a22 + m.a24,  m.a32 + m.a34,  m.a42 + m.a44);
			_p[3] = Plane(-m.a12 + m.a14, -m.a22 + m.a24, -m.a32 + m.a34, -m.a42 + m.a44);
			if(znear_mapping < zfar_mapping)
			{
				_p[4] = Plane( m.a13 - N * m.a14,  m.a23 - N * m.a24,  m.a33 - N * m.a34,  m.a43 - N * m.a44);
				_p[5] = Plane(-m.a13 + F * m.a14, -m.a23 + F * m.a24, -m.a33 + F * m.a34, -m.a43 + F * m.a44);
			}
			else
			{
				_p[4] = Plane( N * m.a14 - m.a13, N * m.a24 - m.a23, N * m.a34 - m.a33, N * m.a44 - m.a43);
				_p[5] = Plane(m.a13 - F * m.a14, m.a23 - F * m.a24, m.a33 - F * m.a34, m.a43 - F * m.a44);
			}

			_planes = 6;
			if(additional_planes.size())
			{
				size_t MAX_PLANES = _countof(_p);
				for(size_t j = 0; j < additional_planes.size() && _planes < MAX_PLANES; ++j)
				{
					_p[_planes] = additional_planes[j];
					++_planes;
				}
			}
		}
		Frustum::FRUSTUMCULL	Frustum::contains(const AABB& aabb) const
		{
			Frustum::FRUSTUMCULL ret = FC_INSIDE;
			const Plane* end = _p + _planes;
			REAL m, n;
			for(const Plane* p2 = _p; p2 != end; ++p2)
			{
				m = p2->sub(aabb.center());
				n = FABS(p2->normal()[0] * aabb.extent()[0]) + FABS(p2->normal()[1] * aabb.extent()[1]) + FABS(p2->normal()[2] * aabb.extent()[2]);
				if(m < -n)
					return FC_OUTSIDE;
				if(m < n)
					ret = FC_INTERSECT;
			}
			return ret;
		}
		Frustum::FRUSTUMCULL	Frustum::contains(const OBB& obb) const
		{
			const Vec3& obb_center = obb.center();
			const Vec3& obb_axis0 = obb.axis(0);
			const Vec3& obb_axis1 = obb.axis(1);
			const Vec3& obb_axis2 = obb.axis(2);
			const Vec3& obb_extent = obb.extent();

			Frustum::FRUSTUMCULL ret = FC_INSIDE;
			const Plane* end = _p + _planes;
			REAL m, n;
			for(const Plane* p2 = _p; p2 != end; ++p2)
			{
				const Vec3& normal = p2->normal();
				m = p2->sub(obb_center);
				n = FABS(obb_axis0 * normal * obb_extent[0]) + FABS(obb_axis1 * normal * obb_extent[1]) + FABS(obb_axis2 * normal * obb_extent[2]);
				if(m < -n)
					return FC_OUTSIDE;
				if(m < n)
					ret = FC_INTERSECT;
			}
			return ret;
		}
		Frustum::FRUSTUMCULL	Frustum::contains(const Vec3& p) const
		{
			Frustum::FRUSTUMCULL ret = FC_INSIDE;
			const Plane* end = _p + _planes;
			REAL m;
			for(const Plane* p2 = _p; p2 != end; ++p2)
			{
				m = p2->sub(p);
				if(m < 0)
					return FC_OUTSIDE;
			}
			return ret;
		}
		Frustum::FRUSTUMCULL	Frustum::contains(const Sphere& s) const
		{
			const Vec3& sphere_origin = s.origin();
			const REAL sphere_radius = s.radius();

			Frustum::FRUSTUMCULL ret = FC_INSIDE;
			const Plane* end = _p + _planes;
			REAL m;
			for(const Plane* p2 = _p; p2 != end; ++p2)
			{
				m = p2->sub(sphere_origin);
				if(m < -sphere_radius)
					return FC_OUTSIDE;
				if(m < sphere_radius)
					ret = FC_INTERSECT;
			}
			return ret;
		}
		Frustum::FRUSTUMCULL	Frustum::contains(const vector<Vec4>& array_point) const
		{
			Frustum::FRUSTUMCULL ret = FC_INSIDE;
			const Plane* end = _p + _planes;
			int j;
			for(const Plane* p3 = _p; p3 != end; ++p3)
			{
				const Plane& p2 = *p3;
				int c = 0;
				for(j = 0; j < (int)array_point.size(); ++j)
				{
					Vec4 p = array_point[j];
					if(p.x * p2[0] + p.y * p2[1] + p.z * p2[2] + p.w * p2[3] >= 0)
						++c;
				}
				if(c == 0)
					return FC_OUTSIDE;
				if(c != (int)array_point.size())
					ret = FC_INTERSECT;
			}
			return ret;
		}
		const Mat4&	Frustum::getMatrix() const
		{
			return _m;
		}
		const Plane&	Frustum::getPlane(size_t i) const
		{
			return _p[i];
		}
		size_t			Frustum::getPlanes() const
		{
			return _planes;
		}
	};
};