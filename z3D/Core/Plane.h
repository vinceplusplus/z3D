
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class Plane
		{
		public:
			union
			{
				REAL v[4];
				struct{REAL v0, v1, v2, v3;};
				struct{REAL a, b, c, d;};
			};
		public:
			Plane();
			Plane(REAL a,REAL b,REAL c,REAL d);
			Plane(const REAL* const v);
			Plane(const Vec3& p,const Vec3& n);
			Plane(const Vec3& n,REAL d);
			Plane(const Vec3& p0,const Vec3& p1,const Vec3& p2,bool normalize=true);
		public:
			const REAL		operator[](int i) const;
			Plane			operator-() const;
		public:
			const Vec3		normal() const;
			const REAL		distance() const;
			const Vec3		origin() const;
		public:
			const REAL		sub(const Vec3& v) const;	// substitute
			const REAL		sub(const Vec4& v) const;	// substitute
		public:
			bool			isParallel(const Plane& p,REAL biasAngle) const;
		public:
			Plane			normalization() const;
			void			normalize();
		public:
			void			transformBy(const Mat4& m,bool inverse_transposed=false);
			Plane			transformedBy(const Mat4& m,bool inverse_transposed=false) const;
		};
	};
};

