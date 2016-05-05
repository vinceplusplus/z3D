
#pragma once

#include "predecl.h"
#include "Vector3.h"

namespace z3D
{
	namespace Core
	{
		class Sphere
		{
		private:
			Vec3		_origin;
			REAL		_radius;
		public:
			Sphere(const Vec3& o,REAL r);
		public:
			const Vec3&		origin() const;
			const REAL		radius() const;
		public:
			void		fit(const Vec3& p);
			void		fit(const Sphere& s);
		};
	};
};

