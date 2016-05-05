
#include "AABB.h"
#include "MathFunc.h"

#include <algorithm>
using namespace std;

namespace z3D
{
	namespace Core
	{
		AABB::AABB(){ _init = 0; }
		AABB::AABB(const Vec3& min_p, const Vec3& max_p)
		{
			__min = min_p;
			__max = max_p;
			SORT(__min[0], __max[0]);
			SORT(__min[1], __max[1]);
			SORT(__min[2], __max[2]);
			_center = (__min + __max) * ((REAL)0.5);
			_extent = (__max - __min) * ((REAL)0.5);
			_init = 1;
		}

		const Vec3& AABB::minimum() const
		{
			return __min;
		}

		const Vec3& AABB::maximum() const
		{
			return __max;
		}

		const Vec3& AABB::center() const
		{
			return _center;
		}

		const Vec3& AABB::extent() const
		{
			return _extent;
		}

		void AABB::fit(const Vec3& p)
		{
			if(!_init)
			{
				__min = __max = p;
				_init = 1;
			}
			else
			{
				for(size_t i = 0; i < 3; i++)
				{
					__min[i] = min(p[i], __min[i]);
					__max[i] = max(p[i], __max[i]);
				}
			}
			_center = (__min + __max) * ((REAL)0.5);
			_extent = (__max - __min) * ((REAL)0.5);
		}
		void AABB::fit(const AABB& aabb)
		{
			if(aabb.isEmpty())
				return;
			fit(aabb.minimum());
			fit(aabb.maximum());
		}
		bool AABB::isEmpty() const
		{
			if(!_init)
				return true;
			return __min == __max;
		}
		void				AABB::transformBy(const Mat4& m)
		{
			(*this) = transformedBy(m);
		}
		AABB AABB::transformedBy(const Mat4& m) const
		{
			if(isEmpty())
				return AABB();
			Vec3 x = _center + Vec3::xAxis * _extent[0];
			Vec3 y = _center + Vec3::yAxis * _extent[1];
			Vec3 z = _center + Vec3::zAxis * _extent[2];
			Vec3 o = _center;
			x *= m;
			y *= m;
			z *= m;
			o *= m;
			Vec3 new_extent;
			new_extent[0] = FABS(x[0] - o[0]) + FABS(y[0] - o[0]) + FABS(z[0] - o[0]);
			new_extent[1] = FABS(x[1] - o[1]) + FABS(y[1] - o[1]) + FABS(z[1] - o[1]);
			new_extent[2] = FABS(x[2] - o[2]) + FABS(y[2] - o[2]) + FABS(z[2] - o[2]);
			return AABB(o - new_extent, o + new_extent);
		}
		Vec3	AABB::getCorner(size_t index) const
		{
			index = ~index;
			return _center + Vec3(
				SETSIGN(_extent[0], (char)(index & 1)),
				SETSIGN(_extent[1], (char)((index >> 1) & 1)),
				SETSIGN(_extent[2], (char)((index >> 2) & 1))
				);
		}
	};
};