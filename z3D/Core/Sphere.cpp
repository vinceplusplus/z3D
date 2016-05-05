
#include "Sphere.h"

namespace z3D
{
	namespace Core
	{
		Sphere::Sphere(const Vec3& o,REAL r)
		{
			_origin=o;
			_radius=r;
		}
		const Vec3& Sphere::origin() const
		{
			return _origin;
		}
		const REAL Sphere::radius() const
		{
			return _radius;
		}
		void Sphere::fit(const Vec3& p)
		{
			REAL len=(p-_origin).length();
			_radius=len>_radius?len:_radius;
		}
		void Sphere::fit(const Sphere& s)
		{
			REAL len=(s.origin()-_origin).length()+s._radius;
			_radius=len>_radius?len:_radius;
		}
	};
};