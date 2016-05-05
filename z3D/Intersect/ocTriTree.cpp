
#define ICE_NO_DLL
#define BAN_OPCODE_AUTOLINK

#pragma warning(disable:4312)
#include "../../Opcode/Opcode.h"
#pragma warning(default:4312)

#undef PI
#undef EXP
#undef random

#undef SQR

#include "ocTriTree.h"

#include "../Core/aligned_storage_inl.h"

namespace z3D
{
	namespace Intersect
	{
		class ocTriTree::PIMPL
		{
		public:
			Opcode::Model				model;
			Opcode::MeshInterface		mesh_interface;
			Opcode::AABBCollider		aabb_collider;
			Opcode::RayCollider			ray_collider;
			Opcode::CollisionFaces		ray_collider_dest;
		};

#include "../Core/z_off_debug_new.h"
		ocTriTree::ocTriTree()
		{
			new (_pimpl.allocate()) PIMPL();

			_pimpl.get().aabb_collider.SetFirstContact(false);
			_pimpl.get().aabb_collider.SetTemporalCoherence(false);
			_pimpl.get().aabb_collider.SetPrimitiveTests(true);

			_pimpl.get().ray_collider.SetFirstContact(false);
			_pimpl.get().ray_collider.SetTemporalCoherence(false);
			_pimpl.get().ray_collider.SetPrimitiveTests(true);
			_pimpl.get().ray_collider.SetDestination(&_pimpl.get().ray_collider_dest);
		}
#include "../Core/z_on_debug_new.h"

		ocTriTree::~ocTriTree()
		{}
		SPtr<ocTriTree>		ocTriTree::create(const Vec3* verts, size_t vert_count, const INT3* tris, size_t tri_count)
		{
			Z_ASSERT( vert_count && tri_count );

			ocTriTree* tree = new ocTriTree();
			tree->_verts.resize(vert_count);
			tree->_tris.resize(tri_count);
			memcpy(&*tree->_verts.begin(), verts, vert_count * sizeof(verts[0]));
			memcpy(&*tree->_tris.begin(), tris, tri_count * sizeof(tris[0]));

			tree->_pimpl.get().mesh_interface.SetNbVertices((udword)vert_count);
			tree->_pimpl.get().mesh_interface.SetNbTriangles((udword)tri_count);
			tree->_pimpl.get().mesh_interface.SetPointers((const IceMaths::IndexedTriangle*)&*tree->_tris.begin(), (const IceMaths::Point*)&*tree->_verts.begin());

			Opcode::OPCODECREATE opcc;

			opcc.mIMesh = &tree->_pimpl.get().mesh_interface;

			opcc.mNoLeaf = true;
			opcc.mQuantized = false;

			opcc.mKeepOriginal = false;
			opcc.mCanRemap = false;

			bool status = tree->_pimpl.get().model.Build(opcc);
			if(!status)
			{
				delete tree;
				return NULL;
			}

			return tree;
		}
		SPtr<ocTriTree>		ocTriTree::create(const z3D::Core::AABB& aabb)
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
		SPtr<ocTriTree>		ocTriTree::create(const z3D::Core::OBB& obb)
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
		void						ocTriTree::intersect(const z3D::Core::AABB& aabb, vector<int32_t>& tris)
		{
			Opcode::AABBCache cache;

			cache.Model = &_pimpl.get().model;
			IceMaths::AABB box;
			box.SetMinMax(Point(&aabb.minimum().x), Point(&aabb.maximum().x));

			bool okay = _pimpl.get().aabb_collider.Collide(cache, box, _pimpl.get().model);

			if(!okay)
				return;

			Z_STATIC_ASSERT( sizeof(udword) == sizeof(uint32_t), WRONG_ASSUMPTION );

			size_t count = _pimpl.get().aabb_collider.GetNbTouchedPrimitives();
			uint32_t* tris2 = (uint32_t*)_pimpl.get().aabb_collider.GetTouchedPrimitives();

			if(count)
				tris.insert(tris.end(), tris2, tris2 + count);
		}
		void						ocTriTree::intersect(const z3D::Core::Ray& ray, vector<int32_t>& tris)
		{
			IceMaths::Ray ray2(Point(&ray.origin().x), Point(&ray.direction().x));

			_pimpl.get().ray_collider_dest.Reset();
			bool okay = _pimpl.get().ray_collider.Collide(ray2, _pimpl.get().model);
			if(!okay)
				return;

			size_t count = (size_t)_pimpl.get().ray_collider_dest.GetNbFaces();
			const Opcode::CollisionFace* face_iter = _pimpl.get().ray_collider_dest.GetFaces();
			for(size_t i = 0; i < count; ++i, ++face_iter)
				tris.push_back(face_iter->mFaceID);
		}
	};
};


