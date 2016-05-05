
#pragma once

#include "predecl.h"
#include "Vector3.h"

namespace z3D
{
	namespace Core
	{
		class Ray
		{
		private:
			Vec3 _origin,_direction;
		public:
			Ray();
			Ray(const Vec3& p,const Vec3& d);
		public:
			const Vec3&		origin() const;
			const Vec3&		direction() const;
		public:
			bool		isParallel(const Ray& r,REAL biasAngle) const;
		};
	};
};

