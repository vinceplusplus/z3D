
#include "Ray.h"
#include "MathFunc.h"

namespace z3D
{
	namespace Core
	{
		Ray::Ray() {}
		Ray::Ray(const Vec3& p,const Vec3& d)
		{
			_origin=p;
			_direction=d;
		}
		const Vec3& Ray::origin() const
		{
			return _origin;
		}
		const Vec3& Ray::direction() const
		{
			return _direction;
		}
		bool Ray::isParallel(const Ray& r,REAL biasAngle) const
		{
			REAL a=ACOS(FABS(_direction.normal()*r.direction().normal()));
			if(a<=biasAngle)
				return true;
			return false;
		}
	};
};