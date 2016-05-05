
#pragma once

#include "predecl.h"

class btTriangleMesh;
class btBvhTriangleMeshShape;

namespace z3D
{
	namespace Intersect
	{
		class btTriTree: public RefBase
		{
		protected:
			btTriangleMesh*				_btmesh;
			btBvhTriangleMeshShape*		_btshape;
		protected:
			btTriTree();
			~btTriTree();
		public:
			static SPtr<btTriTree>		create(const Vec3* verts, size_t vert_count, const INT3* tris, size_t tri_count);
			static SPtr<btTriTree>		create(const AABB& aabb);
			static SPtr<btTriTree>		create(const OBB& obb);
		public:
			template<class allocator>
			void						intersect(const AABB& aabb, vector<int32_t, allocator>& array_index);
			template<class allocator>
			void						intersect(const Ray& ray, vector<int32_t, allocator>& array_index);
		};
	};
};


