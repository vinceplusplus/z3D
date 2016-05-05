
#pragma once

#include "predecl.h"
#include "Vector3.h"

namespace z3D
{
	namespace Core
	{
		class AABB
		{
		public:
			static const AABB	empty_bound;
		private:
			uint32_t			_init;
			Vec3				__min, __max;
			Vec3				_center, _extent;
		public:
			AABB();
			AABB(const Vec3& min_p, const Vec3& max_p);
		public:
			const Vec3&			minimum() const;
			const Vec3&			maximum() const;
			const Vec3&			center() const;
			const Vec3&			extent() const;
		public:
			void				fit(const Vec3& p);
			void				fit(const AABB& aabb);
		public:
			bool				isEmpty() const;
		public:
			void				transformBy(const Mat4& m);
			AABB				transformedBy(const Mat4& m) const;
		public:
			Vec3				getCorner(size_t index) const;
		};
	};
};


