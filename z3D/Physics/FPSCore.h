
#pragma once

#include "predecl.h"

#include "../SG/Header.h"
using namespace z3D::SG;

namespace z3D
{
	namespace Physics
	{
		class FPSCore
		{
		public:
			// !!!!! NOTE !!!!! the output "new_box_center" is the new center of the box, if your box origin is not the center, you need to offset output somehow to fit your application
			static bool				clipAir(const AABB& world_player_box, const Vec3& disp, const Vec3& velocity, const SPtr<SNode>& map_node, REAL resolve_penetration, REAL touchground_tolerance, Vec3& new_box_center, Vec3& remaining_disp, Vec3& new_velocity, bool& touch_ground, Vec3& collision_normal, SPtr<SNode>& contact_node);
			static bool				clipGround(const AABB& world_player_box, const SPtr<SNode>& map_node, REAL max_unpen_dist, REAL touchground_tolerance, bool& clipped, REAL& unpen_dist, bool& touch_ground, Vec3& collision_axis, SPtr<SNode>& contact_node);
		public:
			static bool				clipSphere(const Vec3& so, REAL sr, const Vec3& vel, REAL dt, const SPtr<SNode>& map_node, REAL& toi, Vec3& collinormal, Vec3& collipoint, SPtr<SNode>& contact_node, uint32_t& contact_face);
			static bool				relaxSphere(const Vec3& so, REAL sr, const SPtr<SNode>& map_node, Vec3& shortest_penetration, SPtr<SNode>& contact_node, uint32_t& contact_face);								// not recommended, use relaxSphere2() instead
		public:
			static void				collectVertices(const vnFace* faces, const int32_t* face_indices, size_t face_index_count, vector<int32_t, frame_temp_allocator<int32_t>>& vertex_indices);
		public:
			static bool				unpenetrate(const Vec3& so, const REAL sr, const Vec3* verts, const int32_t* vertex_indices, size_t vertex_index_count, const Vec3& axis, REAL& unpen_disp);					// for sphere vs vertex cloud (treating as a convex hull), assuming axis is a unit-length vector
			static bool				unpenetrate(const Vec3& obb_center, const Vec3 obb_eaxis[3], const Vec3* verts, const int32_t* vertex_indices, size_t vertex_index_count, const Vec3& axis, REAL& unpen_disp);	// for obb vs vertex cloud (treating as a convex hull), assuming axis is a unit-length vector
		public:
			static bool				unpenetrate(const Vec3& so, const REAL sr, const vnFace* faces, const Vec3* verts, const int32_t* face_indices, size_t face_index_count, const Vec3&axis, REAL& unpen_disp);		// for sphere vs faces, assuming axis is a unit-length vector, a very accurate and time-consuming operation
			static bool				unpenetrate(const Vec3& obb_center, const Vec3 obb_eaxis[3], const vnFace* faces, const Vec3* verts, const int32_t* face_indices, size_t face_index_count, const Vec3&axis, REAL& unpen_disp);		// for obb vs faces, assuming axis is a unit-length vector, a very accurate and time-consuming operation
		public:						// a more accurate version, generally more perferrable
			static bool				relaxSphere2(const Vec3& so, REAL sr, REAL search_radius, const SPtr<SNode>& map_node, Vec3& shortest_unpen_disp, SPtr<SNode>& contact_node);
		public:
			static bool				relaxOBB(const OBB& box, const AABB search_box, const SPtr<SNode>& map_node, Vec3& shortest_unpen_disp, SPtr<SNode>& contact_node);
			static bool				relaxAABB(const AABB& box, const AABB search_box, const SPtr<SNode>& map_node, Vec3& shortest_unpen_disp, SPtr<SNode>& contact_node);
		public:
			static bool				anyOverlap(const Vec3& so, REAL sr, const SPtr<SNode>& map_node);																												// for sphere
			static bool				anyOverlap(const OBB& box, const SPtr<SNode>& map_node);																														// for OBB
			static bool				anyOverlap(const AABB& box, const SPtr<SNode>& map_node);																														// for AABB
		public:
			static bool				selectTriangle(STriangleSelector& selector, const Ray& ray, const SPtr<SNode>& map_node);
			static bool				selectTriangle(const Ray& ray, const SPtr<SNode>& map_node, REAL& time, SPtr<SMeshNode>& mn, int32_t& triidx);

		public:						// obsolete !!!
			static void				OBBTriangle(const OBB& box,const TriangleAux& tri,int cur_iter,int max_iter,Vec3& sum,int& hit);
		public:
			static bool				unpenetrate(const OBB& box,const TriangleAux& tri,const Vec3& unpen_dir,REAL& d);
			static bool				tryUnpenetrate(const OBB& local_box,const TriangleAux& tri,const Vec3& local_unpen_dir,const Mat3& world_sr,REAL& min_unpen,Vec3& unpen_disp);
		};
	};
};

