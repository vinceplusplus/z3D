
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Intersect
	{
		class ocTriTree: public RefBase
		{
		private:
			class PIMPL;

			typed_aligned_storage<PIMPL, 328, 4, true>
										_pimpl;
		private:
			vector<Vec3>				_verts;
			vector<INT3>				_tris;
		protected:
			ocTriTree();
			~ocTriTree();
		public:
			static SPtr<ocTriTree>		create(const Vec3* verts, size_t vert_count, const INT3* tris, size_t tri_count);
			static SPtr<ocTriTree>		create(const z3D::Core::AABB& aabb);
			static SPtr<ocTriTree>		create(const z3D::Core::OBB& obb);
		public:
			void						intersect(const z3D::Core::AABB& aabb, vector<int32_t>& tris);
			void						intersect(const z3D::Core::Ray& ray, vector<int32_t>& tris);
		};
	};
};


