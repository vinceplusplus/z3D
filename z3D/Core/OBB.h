
#pragma once

#include "predecl.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Sphere.h"

namespace z3D
{
	namespace Core
	{
		class OBB
		{
		private:
			Vec3		_center;
			Vec3		_axis[3];
			Vec3		_extent;
		public:
			OBB();
			OBB(const Vec3& center,const Vec3* axis,const Vec3 extent);
			OBB(const AABB& aabb);
		public:
			const Vec3&		center() const;
			const Vec3&		extent() const;
			const Vec3&		axis(ptrdiff_t i) const;
			Vec3&		center();
			Vec3&		extent();
			Vec3&		axis(ptrdiff_t i);
			const Vec3*	axis() const;
			Vec3*		axis();
		public:
			void		rotateBy(const Mat3& rot);
			void		translateBy(const Vec3& trans);
			void		scaleBy(const Vec3& scale);
		public:
			void		transformBy(const Mat4& m);
			OBB			transformedBy(const Mat4& m) const;
		public:
			Sphere		fittingSphere() const;
			AABB		fittingAABB() const;
		public:
			Vec3		getCorner(size_t index) const;
		};
	};
};

