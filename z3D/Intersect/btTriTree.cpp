
#include "btTriTree.h"

#include "../Core/allocators_inl.h"

#include "../Core/z_off_debug_new.h"
#include "../../bullet/btBulletCollisionCommon.h"
#include "../Core/z_on_debug_new.h"

namespace z3D
{
	namespace Intersect
	{
		btTriTree::btTriTree()
		{
			_btmesh = NULL;
			_btshape = NULL;
		}
		btTriTree::~btTriTree()
		{
			if(_btshape)
				delete _btshape;
			if(_btmesh)
				delete _btmesh;
		}
		SPtr<btTriTree>		btTriTree::create(const Vec3* verts, size_t vert_count, const INT3* tris, size_t tri_count)
		{
			if(!tri_count)
				return NULL;
			SPtr<btTriTree> tt = new btTriTree();

#include "../Core/z_off_debug_new.h"
			btTriangleMesh* btmesh = new btTriangleMesh();
#include "../Core/z_on_debug_new.h"

			const INT3* tri_end = tris + tri_count;
			for(const INT3* tri_iter = tris; tri_iter != tri_end; ++tri_iter)
			{
				const INT3& t = *tri_iter;
				const Vec3& v0 = verts[t[0]];
				const Vec3& v1 = verts[t[1]];
				const Vec3& v2 = verts[t[2]];
				btVector3 bv0(v0.x, v0.y, v0.z);
				btVector3 bv1(v1.x, v1.y, v1.z);
				btVector3 bv2(v2.x, v2.y, v2.z);
				btmesh->addTriangle(bv0, bv1, bv2);
			}
#include "../Core/z_off_debug_new.h"
			btBvhTriangleMeshShape* btshape = new btBvhTriangleMeshShape(btmesh, false);
#include "../Core/z_on_debug_new.h"

			tt->_btmesh = btmesh;
			tt->_btshape = btshape;

			return tt;
		}
		SPtr<btTriTree>		btTriTree::create(const AABB& aabb)
		{
			vector<Vec3> array_vert;
			vector<INT3> array_tri;

			array_vert.push_back(aabb.getCorner(0));
			array_vert.push_back(aabb.getCorner(1));
			array_vert.push_back(aabb.getCorner(2));
			array_vert.push_back(aabb.getCorner(3));
			array_vert.push_back(aabb.getCorner(4));
			array_vert.push_back(aabb.getCorner(5));
			array_vert.push_back(aabb.getCorner(6));
			array_vert.push_back(aabb.getCorner(7));

			array_tri.push_back(INT3(0, 2, 1));
			array_tri.push_back(INT3(1, 2, 3));
			array_tri.push_back(INT3(4, 6, 0));
			array_tri.push_back(INT3(0, 6, 2));
			array_tri.push_back(INT3(4, 0, 1));
			array_tri.push_back(INT3(4, 1, 5));
			array_tri.push_back(INT3(7, 2, 6));
			array_tri.push_back(INT3(7, 3, 2));
			array_tri.push_back(INT3(4, 7, 6));
			array_tri.push_back(INT3(4, 5, 7));
			array_tri.push_back(INT3(5, 3, 7));
			array_tri.push_back(INT3(5, 1, 3));

			return create(VECTOR_BEGIN_PTR(array_vert), array_vert.size(), VECTOR_BEGIN_PTR(array_tri), array_tri.size());
		}
		SPtr<btTriTree>		btTriTree::create(const OBB& obb)
		{
			vector<Vec3> array_vert;
			vector<INT3> array_tri;

			array_vert.push_back(obb.getCorner(0));
			array_vert.push_back(obb.getCorner(1));
			array_vert.push_back(obb.getCorner(2));
			array_vert.push_back(obb.getCorner(3));
			array_vert.push_back(obb.getCorner(4));
			array_vert.push_back(obb.getCorner(5));
			array_vert.push_back(obb.getCorner(6));
			array_vert.push_back(obb.getCorner(7));

			array_tri.push_back(INT3(0, 2, 1));
			array_tri.push_back(INT3(1, 2, 3));
			array_tri.push_back(INT3(4, 6, 0));
			array_tri.push_back(INT3(0, 6, 2));
			array_tri.push_back(INT3(4, 0, 1));
			array_tri.push_back(INT3(4, 1, 5));
			array_tri.push_back(INT3(7, 2, 6));
			array_tri.push_back(INT3(7, 3, 2));
			array_tri.push_back(INT3(4, 7, 6));
			array_tri.push_back(INT3(4, 5, 7));
			array_tri.push_back(INT3(5, 3, 7));
			array_tri.push_back(INT3(5, 1, 3));

			return create(VECTOR_BEGIN_PTR(array_vert), array_vert.size(), VECTOR_BEGIN_PTR(array_tri), array_tri.size());
		}
		template<class allocator>
		void						btTriTree::intersect(const AABB& aabb, vector<int32_t, allocator>& array_index)
		{
			const Vec3& min0 = aabb.minimum();
			const Vec3& max0 = aabb.maximum();
			btVector3 aabbmin(min0.x, min0.y, min0.z);
			btVector3 aabbmax(max0.x, max0.y, max0.z);
			class mycb: public btTriangleCallback
			{
			private:
				vector<int32_t, allocator>&		_array_index;
			public:
				mycb(vector<int32_t, allocator>& array_index):_array_index(array_index) {}
			public:
				void processTriangle(btVector3* triangle, int partId, int triangleIndex)
				{
					_array_index.push_back(triangleIndex);
				}
			};
			mycb cb(array_index);
			_btshape->processAllTriangles(&cb, aabbmin, aabbmax);
		}
		template<class allocator>
		void						btTriTree::intersect(const Ray& ray, vector<int32_t, allocator>& array_index)
		{
			const Vec3& p = ray.origin();
			const Vec3& p2 = p + ray.direction() * 1000000.0f;
			btVector3 btfrom(p.x, p.y, p.z);
			btVector3 btto(p2.x, p2.y, p2.z);
			class mycb: public btTriangleCallback
			{
			private:
				vector<int32_t, allocator>&		_array_index;
			public:
				mycb(vector<int32_t, allocator>& array_index) : _array_index(array_index) {}
			public:
				void processTriangle(btVector3* triangle, int partId, int triangleIndex)
				{
					_array_index.push_back(triangleIndex);
				}
			};
			mycb cb(array_index);
			//_btshape->processAllTrianglesRay(&cb, btfrom, btto);
			_btshape->performRaycast(&cb, btfrom, btto);
		}

		// explicit instantiation
		template
		void						btTriTree::intersect<allocator<int32_t>>(const AABB&, vector<int32_t, allocator<int32_t>>&);
		template
		void						btTriTree::intersect<frame_temp_allocator<int32_t>>(const AABB&, vector<int32_t, frame_temp_allocator<int32_t>>&);

		template
		void						btTriTree::intersect<allocator<int32_t>>(const Ray&, vector<int32_t, allocator<int32_t>>&);
		template
		void						btTriTree::intersect<frame_temp_allocator<int32_t>>(const Ray&, vector<int32_t, frame_temp_allocator<int32_t>>&);
	};
};


