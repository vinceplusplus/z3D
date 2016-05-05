
#include "OBB.h"
#include "MathFunc.h"
#include "AABB.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace z3D
{
	namespace Core
	{
		OBB::OBB() {}
		OBB::OBB(const Vec3& center, const Vec3* axis, const Vec3 extent)
		{
			_center = center;
			_axis[0] = axis[0]; _axis[1] = axis[1]; _axis[2] = axis[2];
			_extent = extent;
		}
		OBB::OBB(const AABB& aabb)
		{
			_center = aabb.center();
			_axis[0] = Vec3::xAxis; _axis[1] = Vec3::yAxis; _axis[2] = Vec3::zAxis;
			_extent = aabb.extent();
		}
		const Vec3&		OBB::center() const
		{
			return _center;
		}
		const Vec3&		OBB::extent() const
		{
			return _extent;
		}
		const Vec3&		OBB::axis(ptrdiff_t i) const
		{
			return _axis[i];
		}
		Vec3&		OBB::center()
		{
			return _center;
		}
		Vec3&		OBB::extent()
		{
			return _extent;
		}
		Vec3&		OBB::axis(ptrdiff_t i)
		{
			return _axis[i];
		}
		const Vec3*	OBB::axis() const
		{
			return _axis;
		}
		Vec3*		OBB::axis()
		{
			return _axis;
		}
		void			OBB::rotateBy(const Mat3& rot)
		{
			_axis[0] *= rot;
			_axis[1] *= rot;
			_axis[2] *= rot;
		}
		void			OBB::translateBy(const Vec3& trans)
		{
			_center += trans;
		}
		void			OBB::scaleBy(const Vec3& scale)
		{
			_extent[0] *= scale[0];
			_extent[1] *= scale[1];
			_extent[2] *= scale[2];
		}
		void			OBB::transformBy(const Mat4& m)
		{
			if(true)
			{
				_center *= m;
				Mat3 m2 = ((Mat3)m);
				_axis[0] = _axis[0] * m2;
				_axis[1] = _axis[1] * m2;
				_axis[2] = _axis[2] * m2;
				REAL lens[3];
				_extent[0] *= (lens[0] = _axis[0].length());
				_extent[1] *= (lens[1] = _axis[1].length());
				_extent[2] *= (lens[2] = _axis[2].length());
				_axis[0] /= lens[0];
				_axis[1] /= lens[1];
				_axis[2] /= lens[2];

				return;
			}
			// shit... doesn't deal with zeros in extent
			Z_ASSERT(_extent[0] && _extent[1] && _extent[2]);
			_center *= m;
			Mat3 m2 = ((Mat3)m);
			_axis[0] = _axis[0] * _extent[0] * m2;
			_axis[1] = _axis[1] * _extent[1] * m2;
			_axis[2] = _axis[2] * _extent[2] * m2;
			_extent[0] = _axis[0].length();
			_extent[1] = _axis[1].length();
			_extent[2] = _axis[2].length();
			_axis[0] /= _extent[0];
			_axis[1] /= _extent[1];
			_axis[2] /= _extent[2];
		}
		OBB			OBB::transformedBy(const Mat4& m) const
		{
			OBB obb = (*this);
			obb.transformBy(m);
			return obb;
		}
		Sphere		OBB::fittingSphere() const
		{
			return Sphere(_center, (_axis[0] * _extent[0] + _axis[1] * _extent[1] + _axis[2] * _extent[2]).length());
		}
		AABB			OBB::fittingAABB() const
		{
			Vec3 r=Vec3(
				FABS(_axis[0][0] * _extent[0]) + FABS(_axis[1][0] * _extent[1]) + FABS(_axis[2][0] * _extent[2]),
				FABS(_axis[0][1] * _extent[0]) + FABS(_axis[1][1] * _extent[1]) + FABS(_axis[2][1] * _extent[2]),
				FABS(_axis[0][2] * _extent[0]) + FABS(_axis[1][2] * _extent[1]) + FABS(_axis[2][2] * _extent[2]));
			return AABB(_center - r, _center + r);
		}
		Vec3			OBB::getCorner(size_t index) const
		{
			index = ~index;
			return _center+
				SETSIGN(1.0f, (char)(index & 1)) * _axis[0] * _extent[0] + 
				SETSIGN(1.0f, (char)((index >> 1) & 1)) * _axis[1] * _extent[1] + 
				SETSIGN(1.0f, (char)((index >> 2) & 1)) * _axis[2] * _extent[2]
				;
		}

	};
};