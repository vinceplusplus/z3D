
#include "FPSCore.h"

#include "../Core/allocators_inl.h"

#include "../Intersect/Header.h"

#pragma warning(disable:4311)

using namespace z3D::Intersect;

namespace z3D
{
	namespace Physics
	{
		bool			FPSCore::clipAir(const AABB& world_player_box, const Vec3& disp, const Vec3& velocity, const SPtr<SNode>& map_node, REAL resolve_penetration, REAL touchground_tolerance, Vec3& new_box_center, Vec3& remaining_disp, Vec3& new_velocity, bool& touch_ground, Vec3& collision_normal, SPtr<SNode>& contact_node)
		{
			AABB search_box = AABB((-world_player_box.extent() * 1.015625f + world_player_box.center()), (world_player_box.extent() * 1.015625f + world_player_box.center()));
			search_box.fit(search_box.transformedBy(Mat4::translation(disp)));
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			REAL inv_disp_length = 1.0f / disp.length();

			touch_ground = false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;

			REAL first_time = INFINITY;
			Vec3 first_time_axis;
			SMeshNode* first_time_mn = NULL;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				OBB local_box(world_player_box);
				local_box.transformBy(mn->inv_world_trans());
				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());

				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				if(array_index.size())
				{
					const Vec3& local_box_center = local_box.center();
					const Vec3& local_box_extent = local_box.extent();
					Vec3 local_box_eaxis[3] = {local_box.axis()[0] * local_box_extent[0], local_box.axis()[1] * local_box_extent[1], local_box.axis()[2] * local_box_extent[2]};

					Vec3 local_disp = disp * (Mat3)mn->inv_world_trans();

					Mat3 world_sr = (Mat3)mn->world_trans();
					const Vec3* verts = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();

					for(size_t i = 0; i < array_index.size(); ++i)
					{
						Z_STATIC_ASSERT(sizeof(SMeshNode*) == sizeof(uint32_t), POINTER_SIZE_ASSUMPTION_WRONG);

						int32_t triidx = array_index[i];

						const vnFace& f = faces[triidx];
						const Vec3& v0 = verts[f.v[0]];
						const Vec3& v1 = verts[f.v[1]];
						const Vec3& v2 = verts[f.v[2]];

						REAL t0, t1;
						Vec3 axis;
						if(!CD::OBBTriangle(local_box_center, local_box_eaxis, local_disp, v0, v1, v2, t0, t1, &axis, NULL))
							continue;

						if(t0 > 1.0f)
							continue;

						if(t0 < 0.0f && t1 >= 0.0f)
						{
							// already penetrating...
							int k = sizeof(k);
						}

						if(t1 < 0.0f)
							continue;

						if(t0 < 0.0f && (resolve_penetration == 0.0f || t0 < -resolve_penetration * inv_disp_length))
							continue;

						//if(t0 < 0.0f || t0 > 1.0f)
						//	continue;

						if(t0 >= first_time)
							continue;

						first_time = t0;
						first_time_axis = axis * world_sr;
						first_time_mn = mn;
					}
				}
			}
			if(first_time == INFINITY)
				return false;

			first_time_axis.normalize_safe();
			collision_normal = (first_time_axis * disp > 0.0f)? -first_time_axis: first_time_axis;

			new_box_center = world_player_box.center() + first_time * disp;
			remaining_disp = disp - disp * first_time;
			remaining_disp = remaining_disp.slideAlong(first_time_axis);
			new_velocity = velocity.slideAlong(first_time_axis);
			touch_ground = (collision_normal.angle(Vec3::yAxis) < touchground_tolerance);
			contact_node = first_time_mn;

			return true;
		}
		bool			FPSCore::clipGround(const AABB& world_player_box, const SPtr<SNode>& map_node, REAL max_unpen_dist, REAL touchground_tolerance, bool& clipped, REAL& unpen_dist, bool& touch_ground, Vec3& collision_axis, SPtr<SNode>& contact_node)
		{
			AABB search_box = AABB((-world_player_box.extent() * 1.015625f + world_player_box.center()), (world_player_box.extent() * 1.015625f + world_player_box.center()));
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			REAL max_up = -INFINITY;
			Vec3 last_time_axis;
			touch_ground = false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				OBB local_box(world_player_box);
				local_box.transformBy(mn->inv_world_trans());
				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());
				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				if(array_index.size())
				{
					const Vec3& local_box_center = local_box.center();
					const Vec3& local_box_extent = local_box.extent();
					Vec3 local_box_eaxis[3] = {local_box.axis()[0] * local_box_extent[0], local_box.axis()[1] * local_box_extent[1], local_box.axis()[2] * local_box_extent[2]};

					Vec3 local_up = (Vec3::yAxis * ((Mat3)mn->inv_world_trans())).normal();

					Mat3 world_sr = (Mat3)mn->world_trans();

					const Vec3* verts = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();

					for(size_t i = 0; i < array_index.size(); ++i)
					{
						Z_STATIC_ASSERT(sizeof(SMeshNode*) == sizeof(uint32_t), POINTER_SIZE_ASSUMPTION_WRONG);

						int32_t triidx = array_index[i];

						const vnFace& f = faces[triidx];
						const Vec3& v0 = verts[f.v[0]];
						const Vec3& v1 = verts[f.v[1]];
						const Vec3& v2 = verts[f.v[2]];

						if(!Test::OBBTriangle(local_box_center, local_box_eaxis, v0, v1, v2))
							continue;

						REAL d;

						Vec3 axis;
						if(true)
						{
							REAL t0, t1;
							if(!CD::OBBTriangle(local_box_center, local_box_eaxis, local_up, v0, v1, v2, t0, t1, NULL, &axis))
								continue;
							if(t1 <= 0.0f)
								continue;
							d = t1;
						}

						REAL world_up = (local_up * d * world_sr).length();
						if(world_up > max_up)
						{
							if(world_up > max_unpen_dist)
								return false;

							max_up = world_up;
							last_time_axis = axis * world_sr;

							if(ACOS(FABS(local_up * axis.normal_safe())) < touchground_tolerance)
								touch_ground = true;

							contact_node = *node_iter;
						}
					}
				}
			}
			if(max_up == -INFINITY)
			{
				clipped = false;

				unpen_dist = 0.0f;
				touch_ground = false;
				contact_node.reset();

				return true;
			}

			clipped = true;

			unpen_dist = max_up;

			collision_axis = last_time_axis.normal_safe();

			return true;
		}
		bool				FPSCore::clipSphere(const Vec3& so, REAL sr, const Vec3& vel, REAL dt, const SPtr<SNode>& map_node, REAL& toi, Vec3& collinormal, Vec3& collipoint, SPtr<SNode>& contact_node, uint32_t& contact_face)
		{
			toi = INFINITY;

			Vec3 disp = vel * dt;
			Vec3 radius_vec(sr, sr, sr);

			AABB search_box = AABB(-radius_vec * 1.015625f + so, radius_vec * 1.015625f + so);
			search_box.fit(search_box.transformedBy(Mat4::translation(disp)));
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());

				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				if(array_index.size())
				{
					Mat3 world_sr = (Mat3)mn->world_trans();
					Mat3 inv_world_sr = (Mat3)mn->inv_world_trans();

					Vec3 local_center = so * mn->inv_world_trans();
					REAL local_radius = (radius_vec.normal() * sr * inv_world_sr).length();
					Vec3 local_vel = vel * inv_world_sr;

					Vec3 local_disp = disp * inv_world_sr;

					const Vec3* verts = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();

					for(size_t i = 0; i < array_index.size(); ++i)
					{
						int32_t triidx = array_index[i];

						const vnFace& f = faces[triidx];
						const Vec3& v0 = verts[f.v[0]];
						const Vec3& v1 = verts[f.v[1]];
						const Vec3& v2 = verts[f.v[2]];

						if(true)
						{
							REAL t0, t1;
							Vec3 first_time_contact_axis;
							//if(!CD::SphereTriangleSAT<true>(local_center, local_radius, local_vel, v0, v1, v2, t0, t1, &first_time_contact_axis, NULL) || t0 >= toi || t0 > dt || t0 < 0.0f)
							if(!CD::SphereTriangleFull2(local_center, local_radius, local_vel, v0, v1, v2, t0, t1, &first_time_contact_axis, NULL) || t0 >= toi || t0 > dt || t0 < 0.0f)
								continue;

							toi = t0;
							//collinormal = (first_time_contact_axis * SETSIGN(1.0f, !SIGNBIT(local_vel * first_time_contact_axis)) * SETSIGN(1.0f, SIGNBIT(t0))) * world_sr;
							collinormal = (first_time_contact_axis.normal_safe() * SETSIGN(1.0f, !SIGNBIT(local_vel * first_time_contact_axis)) * SETSIGN(1.0f, SIGNBIT(t0))) * world_sr;
							contact_node = mn;
							contact_face = triidx;
						}
						else
						{
							REAL t;
							Vec3 collinormal2, collipoint2;
							if(!CD::SphereTriangleFull(local_center, local_radius, local_vel, v0, v1, v2, t, collinormal2, collipoint2) || t > dt || t >= toi)
								continue;

							toi = t;
							collinormal = collinormal2 * world_sr;
							collipoint = collipoint2 * mn->world_trans();
							contact_node = mn;
							contact_face = triidx;
						}
					}
				}
			}

			if(toi == INFINITY)
				return false;

			collinormal.normalize_safe();
			return true;
		}
		void				FPSCore::collectVertices(const vnFace* faces, const int32_t* face_indices, size_t face_index_count, vector<int32_t, frame_temp_allocator<int32_t>>& vertex_indices)
		{
			//typedef set<int32_t, less<int32_t>, set_stack_allocator<int32_t, 20, 64> > SET_TYPE;
			//set_stack_allocator_mem<20, 64> mem;

			//SET_TYPE used((less<int32_t>()), set_stack_allocator<int32_t, 20, 64>(&mem));

			typedef set<int32_t, less<int32_t>, frame_temp_allocator<int32_t>> SET_TYPE;
			SET_TYPE used;

			const int32_t* end = face_indices + face_index_count;
			for(const int32_t* p = face_indices; p != end; ++p)
			{
				const vnFace& f = faces[*p];
				used.insert(f.v[0]);
				used.insert(f.v[1]);
				used.insert(f.v[2]);
			}
			for(SET_TYPE::const_iterator iter = used.begin(); iter != used.end(); ++iter)
				vertex_indices.push_back(*iter);
		}
		bool				FPSCore::relaxSphere(const Vec3& so, REAL sr, const SPtr<SNode>& map_node, Vec3& shortest_penetration, SPtr<SNode>& contact_node, uint32_t& contact_face)
		{
			REAL shortest_penetration_depth = INFINITY;

			Vec3 radius_vec(sr, sr, sr);

			AABB search_box = AABB(-radius_vec * 1.015625f + so, radius_vec * 1.015625f + so);
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());

				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				if(array_index.size())
				{
					Mat3 world_sr = (Mat3)mn->world_trans();
					Mat3 inv_world_sr = (Mat3)mn->inv_world_trans();

					Vec3 local_center = so * mn->inv_world_trans();
					REAL local_radius = (radius_vec.normal() * sr * inv_world_sr).length();

					const Vec3* verts = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();

					for(size_t i = 0; i < array_index.size(); ++i)
					{
						int32_t triidx = array_index[i];

						const vnFace& f = faces[triidx];
						const Vec3& v0 = verts[f.v[0]];
						const Vec3& v1 = verts[f.v[1]];
						const Vec3& v2 = verts[f.v[2]];

						if(!Test::SphereTriangle(local_center, local_radius, v0, v1, v2))
							continue;

						Vec3 n = ((v1 - v0) ^ (v2 - v0)).normal_safe();
						REAL depth = (local_center - v0) * n;
						Vec3 penetration = SETSIGN(1.0f, !SIGNBIT(depth)) * n * (max(0.0f, local_radius - FABS(depth)));
						penetration *= world_sr;
						if(penetration.normal_safe() == Vec3::zero)
							continue;
						depth = penetration.length();
						if(depth >= shortest_penetration_depth)
							continue;

						shortest_penetration = penetration;
						shortest_penetration_depth = depth;
						contact_node = mn;
						contact_face = triidx;
					}
				}
			}

			if(shortest_penetration_depth == INFINITY)
				return false;

			return true;
		}
		bool				FPSCore::unpenetrate(const Vec3& so, const REAL sr, const Vec3* verts, const int32_t* vertex_indices, size_t vertex_index_count, const Vec3& axis, REAL& unpen_disp)
		{
			Z_ASSERT( vertex_index_count != 0 );

			REAL min0, max0;
			if(true)
			{
				REAL t = so * axis;
				min0 = t - sr;
				max0 = t + sr;
			}
			REAL min1 = INFINITY, max1 = -INFINITY;
			const int32_t* end = vertex_indices + vertex_index_count;
			for(const int32_t* p = vertex_indices; p != end; ++p)
			{
				REAL t;
				t = verts[*p] * axis;
				min1 = min(min1, t); max1 = max(max1, t);
			}
			if(!Test::IntervalInterval(min0, max0, min1, max1))
				return false;

			REAL unpen0 = min1 - max0;
			REAL unpen1 = max1 - min0;

			unpen_disp = FABS(unpen0) < FABS(unpen1) ? unpen0 : unpen1;

			return true;
		}
		bool				FPSCore::unpenetrate(const Vec3& obb_center, const Vec3 obb_eaxis[3], const Vec3* verts, const int32_t* vertex_indices, size_t vertex_index_count, const Vec3& axis, REAL& unpen_disp)
		{
			Z_ASSERT( vertex_index_count != 0 );

			REAL min0, max0;
			if(true)
			{
				REAL c = obb_center * axis;
				REAL e = FABS(obb_eaxis[0] * axis) + FABS(obb_eaxis[1] * axis) + FABS(obb_eaxis[2] * axis);
				min0 = c - e;
				max0 = c + e;
			}
			REAL min1 = INFINITY, max1 = -INFINITY;
			const int32_t* end = vertex_indices + vertex_index_count;
			for(const int32_t* p = vertex_indices; p != end; ++p)
			{
				REAL t;
				t = verts[*p] * axis;
				min1 = min(min1, t); max1 = max(max1, t);
			}
			if(!Test::IntervalInterval(min0, max0, min1, max1))
				return false;

			REAL unpen0 = min1 - max0;
			REAL unpen1 = max1 - min0;

			unpen_disp = FABS(unpen0) < FABS(unpen1) ? unpen0 : unpen1;

			return true;
		}
		bool				FPSCore::unpenetrate(const Vec3& so, const REAL sr, const vnFace* faces, const Vec3* verts, const int32_t* face_indices, size_t face_index_count, const Vec3& axis, REAL& unpen_disp)
		{
			Z_ASSERT( face_index_count != 0 );

			REAL t0 = INFINITY;
			REAL t1 = -INFINITY;

			const int32_t* end = face_indices + face_index_count;
			for(const int32_t* p = face_indices; p != end; ++p)
			{
				const vnFace& f = faces[*p];
				REAL tt0, tt1;
				//if(!CD::SphereTriangleSAT<true>(so, sr, axis, verts[f.v[0]], verts[f.v[1]], verts[f.v[2]], tt0, tt1, NULL, NULL))
				if(!CD::SphereTriangleFull2(so, sr, axis, verts[f.v[0]], verts[f.v[1]], verts[f.v[2]], tt0, tt1, NULL, NULL))
					continue;
				t0 = min(t0, tt0);
				t1 = max(t1, tt1);
			}

			if(t0 > t1)
				return false;

			if(SIGNBIT(t0) == SIGNBIT(t1))
				return false;

			unpen_disp = (FABS(t0) < FABS(t1)) ? t0 : t1;

			return true;
		}
		bool				FPSCore::unpenetrate(const Vec3& obb_center, const Vec3 obb_eaxis[3], const vnFace* faces, const Vec3* verts, const int32_t* face_indices, size_t face_index_count, const Vec3& axis, REAL& unpen_disp)
		{
			Z_ASSERT( face_index_count != 0 );

			REAL t0 = INFINITY;
			REAL t1 = -INFINITY;

			const int32_t* end = face_indices + face_index_count;
			for(const int32_t* p = face_indices; p != end; ++p)
			{
				const vnFace& f = faces[*p];
				REAL tt0, tt1;
				if(!CD::OBBTriangle(obb_center, obb_eaxis, axis, verts[f.v[0]], verts[f.v[1]], verts[f.v[2]], tt0, tt1, NULL, NULL))
					continue;
				t0 = min(t0, tt0);
				t1 = max(t1, tt1);
			}

			if(t0 > t1)
				return false;

			if(SIGNBIT(t0) == SIGNBIT(t1))
				return false;

			unpen_disp = (FABS(t0) < FABS(t1)) ? t0 : t1;

			return true;
		}
		bool				FPSCore::relaxSphere2(const Vec3& so, REAL sr, REAL search_radius, const SPtr<SNode>& map_node, Vec3& shortest_unpen_disp, SPtr<SNode>& contact_node)
		{
			REAL shortest_unpen = INFINITY;

			Vec3 radius_vec(sr, sr, sr);
			Vec3 search_vec(search_radius, search_radius, search_radius);

			AABB search_box = AABB(-search_vec + so, search_vec + so);
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;
			vector<int32_t, frame_temp_allocator<int32_t>> candidates;
			vector<int32_t, frame_temp_allocator<int32_t>> vertex_indices;

			//typedef set<uint64_t, less<uint64_t>, set_stack_allocator<uint64_t, 32, 64> > TESTED_EDGES_SET_TYPE;
			//set_stack_allocator_mem<32, 64> mem;

			//TESTED_EDGES_SET_TYPE tested_edges((less<uint64_t>()), set_stack_allocator<uint64_t, 32, 64>(&mem));

			typedef set<uint64_t, less<uint64_t>, frame_temp_allocator<uint64_t>> TESTED_EDGES_SET_TYPE;
			TESTED_EDGES_SET_TYPE tested_edges;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());

				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				const ConvexGroup& cg = mn->mesh()->getConvexGroup();

				if(!array_index.size())
					continue;

				candidates.clear();

				Mat3 world_sr = (Mat3)mn->world_trans();
				Mat3 inv_world_sr = (Mat3)mn->inv_world_trans();

				Vec3 local_center = so * mn->inv_world_trans();
				REAL local_radius = (radius_vec.normal() * sr * inv_world_sr).length();
				REAL local_search_radius = (search_vec.normal() * search_radius * inv_world_sr).length();

				const Vec3* verts = mn->mesh()->get_verts();
				const vnFace* faces = mn->mesh()->get_faces();

				for(size_t i = 0; i < array_index.size(); ++i)
				{
					int32_t triidx = array_index[i];

					const vnFace& f = faces[triidx];
					const Vec3& v0 = verts[f.v[0]];
					const Vec3& v1 = verts[f.v[1]];
					const Vec3& v2 = verts[f.v[2]];

					if(!Test::SphereTriangle(local_center, local_search_radius, v0, v1, v2))
						continue;

					candidates.push_back(triidx);
				}

				if(!candidates.size())
					continue;

				sort(candidates.begin(), candidates.end(), ConvexGroup::pred(cg));

				REAL local_shortest_unpen = INFINITY;
				Vec3 local_axis;

				for(size_t i = 0; i < candidates.size();)
				{
					//size_t len = ConvexGroup::group_length(cg).get(&candidates[i], candidates.size() - i);

					// the convex group implementation is flawed (2008-12-15), can't be used, so now instead, we test all faces, and i doubt the correct implementation will produce good result, cases are, 1) the colliding face is at the boundary of a convex patch (piece or whatever), then the relaxing will tend to push away the volume from the boundary which may not be a desirable result, 2) if we each face has a convex patch that is dedicated to that face and is built from the face (each addition face to test vertices are behind all faces' planes) then the resulting convex patch may still exhibit the same boundary issue
					size_t len = candidates.size() - i;

					vertex_indices.clear();
					vertex_indices.reserve(len * 3);
					collectVertices(faces, VECTOR_BEGIN_PTR(candidates) + i, len, vertex_indices);

					tested_edges.clear();

					REAL group_shortest_unpen = INFINITY;
					Vec3 group_axis;

					size_t separate = false;

					if(!separate)
					{
						size_t end = i + len;

						for(size_t j = i; j < end; ++j)
						{
							const vnFace& f = faces[candidates[j]];
							REAL unpen;
							Vec3 a;
							Vec3 t;

							// face-face normal
							a = ((verts[f.v[1]] - verts[f.v[0]]) ^ (verts[f.v[2]] - verts[f.v[0]])).normal_safe();
							if(a.lengthsq())
							{
								//if(!unpenetrate(local_center, local_radius, verts, VECTOR_BEGIN_PTR(vertex_indices), vertex_indices.size(), a, unpen))
								if(!unpenetrate(local_center, local_radius, faces, verts, VECTOR_BEGIN_PTR(candidates) + i, len, a, unpen))
								{
									separate = true;
									break;
								}
								if(FABS(unpen) < FABS(group_shortest_unpen))
								{
									group_shortest_unpen = unpen;
									group_axis = a;
								}
							}

							// edge-edge cases
							uint64_t edge;
							int32_t v0, v1;

							for(size_t k = 0; k < 3; ++k)
							{
								v0 = f.v[k]; v1 = f.v[(k + 1) % 3]; SORT(v0, v1);
								edge = v0 | (((uint64_t)v1) << 32);
								if(!tested_edges.count(edge))
								{
									tested_edges.insert(edge);

									t = verts[v1] - verts[v0];
									a = ((t ^ (local_center - verts[v0])) ^ t).normal_safe();
									if(a.lengthsq())
									{
										//if(!unpenetrate(local_center, local_radius, verts, VECTOR_BEGIN_PTR(vertex_indices), vertex_indices.size(), a, unpen))
										if(!unpenetrate(local_center, local_radius, faces, verts, VECTOR_BEGIN_PTR(candidates) + i, len, a, unpen))
										{
											separate = true;
											break;
										}
										if(FABS(unpen) < FABS(group_shortest_unpen))
										{
											group_shortest_unpen = unpen;
											group_axis = a;
										}
									}
								}
							}
						}
					}

					// face-vertex cases
					if(!separate)
					{
						const int32_t* end = VECTOR_BEGIN_PTR(vertex_indices) + vertex_indices.size();
						for(const int32_t* p = VECTOR_BEGIN_PTR(vertex_indices); p != end; ++p)
						{
							REAL unpen;
							Vec3 a = (local_center - verts[*p]).normal_safe();
							if(a.lengthsq())
							{
								//if(!unpenetrate(local_center, local_radius, verts, VECTOR_BEGIN_PTR(vertex_indices), vertex_indices.size(), a, unpen))
								if(!unpenetrate(local_center, local_radius, faces, verts, VECTOR_BEGIN_PTR(candidates) + i, len, a, unpen))
								{
									separate = true;
									break;
								}
								if(FABS(unpen) < FABS(group_shortest_unpen))
								{
									group_shortest_unpen = unpen;
									group_axis = a;
								}
							}
						}
					}

					if(!separate && FABS(group_shortest_unpen) < FABS(local_shortest_unpen))
					{
						local_shortest_unpen = group_shortest_unpen;
						local_axis = group_axis;
					}

					i += len;
				}

				if(local_shortest_unpen == INFINITY)
					continue;

				Vec3 unpen_disp = local_shortest_unpen * local_axis * world_sr;
				REAL unpen = unpen_disp.length();
				if(unpen < shortest_unpen)
				{
					shortest_unpen = unpen;
					shortest_unpen_disp = unpen_disp;
					contact_node = mn;
				}
			}

			if(shortest_unpen == INFINITY)
				return false;

			return true;
		}
		bool				FPSCore::relaxOBB(const OBB& box, const AABB search_box, const SPtr<SNode>& map_node, Vec3& shortest_unpen_disp, SPtr<SNode>& contact_node)
		{
			REAL shortest_unpen = INFINITY;

			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;
			vector<int32_t, frame_temp_allocator<int32_t>> candidates;
			vector<int32_t, frame_temp_allocator<int32_t>> vertex_indices;

			//typedef set<uint64_t, less<uint64_t>, set_stack_allocator<uint64_t, 32, 64> > TESTED_EDGES_SET_TYPE;
			//set_stack_allocator_mem<32, 64> mem;

			//TESTED_EDGES_SET_TYPE tested_edges((less<uint64_t>()), set_stack_allocator<uint64_t, 32, 64>(&mem));

			typedef set<uint64_t, less<uint64_t>, frame_temp_allocator<uint64_t>> TESTED_EDGES_SET_TYPE;
			TESTED_EDGES_SET_TYPE tested_edges;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());

				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				const ConvexGroup& cg = mn->mesh()->getConvexGroup();

				if(!array_index.size())
					continue;

				candidates.clear();

				Mat3 world_sr = (Mat3)mn->world_trans();
				Mat3 inv_world_sr = (Mat3)mn->inv_world_trans();

				const Vec3* verts = mn->mesh()->get_verts();
				const vnFace* faces = mn->mesh()->get_faces();

				Vec3 local_search_box_center = local_search_box.center();
				Vec3 local_search_box_eaxis[3] = {Vec3::xAxis * local_search_box.extent()[0], Vec3::yAxis * local_search_box.extent()[1], Vec3::zAxis * local_search_box.extent()[2]};

				for(size_t i = 0; i < array_index.size(); ++i)
				{
					int32_t triidx = array_index[i];

					const vnFace& f = faces[triidx];
					const Vec3& v0 = verts[f.v[0]];
					const Vec3& v1 = verts[f.v[1]];
					const Vec3& v2 = verts[f.v[2]];

					if(!Test::OBBTriangle(local_search_box_center, local_search_box_eaxis, v0, v1, v2))
						continue;

					candidates.push_back(triidx);
				}

				if(!candidates.size())
					continue;

				sort(candidates.begin(), candidates.end(), ConvexGroup::pred(cg));

				OBB local_box = box.transformedBy(mn->inv_world_trans());

				const Vec3& local_box_center = local_box.center();
				const Vec3& local_box_extent = local_box.extent();
				const Vec3 local_box_eaxis[3] = {local_box.axis(0) * local_box_extent[0], local_box.axis(1) * local_box_extent[1], local_box.axis(2) * local_box_extent[2]};
				const Vec3 local_box_axis[3] = {local_box_eaxis[0].normal_safe(), local_box_eaxis[1].normal_safe(), local_box_eaxis[2].normal_safe()};

				REAL local_shortest_unpen = INFINITY;
				Vec3 local_axis;

				for(size_t i = 0; i < candidates.size();)
				{
					//size_t len = ConvexGroup::group_length(cg).get(&candidates[i], candidates.size() - i);

					// the convex group implementation is flawed (2008-12-15), can't be used, so now instead, we test all faces, and i doubt the correct implementation will produce good result, cases are, 1) the colliding face is at the boundary of a convex patch (piece or whatever), then the relaxing will tend to push away the volume from the boundary which may not be a desirable result, 2) if we each face has a convex patch that is dedicated to that face and is built from the face (each addition face to test vertices are behind all faces' planes) then the resulting convex patch may still exhibit the same boundary issue
					size_t len = candidates.size() - i;

					vertex_indices.clear();
					vertex_indices.reserve(len * 3);
					collectVertices(faces, VECTOR_BEGIN_PTR(candidates) + i, len, vertex_indices);

					tested_edges.clear();

					REAL group_shortest_unpen = INFINITY;
					Vec3 group_axis;

					size_t separate = false;

					// face-face normal
					if(!separate)
					{
						REAL unpen;
						Vec3 a;

						for(size_t j = 0; j < 3; ++j)
						{
							a = local_box_axis[j];
							if(a.lengthsq())
							{
								if(!unpenetrate(local_box_center, local_box_eaxis, faces, verts, VECTOR_BEGIN_PTR(candidates) + i, len, a, unpen))
								{
									separate = true;
									break;
								}
								if(FABS(unpen) < FABS(group_shortest_unpen))
								{
									group_shortest_unpen = unpen;
									group_axis = a;
								}
							}
						}
					}

					if(!separate)
					{
						size_t end = i + len;

						for(size_t j = i; j < end; ++j)
						{
							const vnFace& f = faces[candidates[j]];
							REAL unpen;
							Vec3 a;

							// face-face normal
							a = ((verts[f.v[1]] - verts[f.v[0]]) ^ (verts[f.v[2]] - verts[f.v[0]])).normal_safe();
							if(a.lengthsq())
							{
								if(!unpenetrate(local_box_center, local_box_eaxis, faces, verts, VECTOR_BEGIN_PTR(candidates) + i, len, a, unpen))
								{
									separate = true;
									break;
								}
								if(FABS(unpen) < FABS(group_shortest_unpen))
								{
									group_shortest_unpen = unpen;
									group_axis = a;
								}
							}

							// edge-edge cases
							uint64_t edge;
							int32_t v0, v1;

							for(size_t k = 0; k < 3; ++k)
							{
								v0 = f.v[k]; v1 = f.v[(k + 1) % 3]; SORT(v0, v1);
								edge = v0 | (((uint64_t)v1) << 32);
								if(!tested_edges.count(edge))
								{
									tested_edges.insert(edge);
									for(size_t l = 0; l < 3; ++l)
									{
										a = ((verts[v1] - verts[v0]) ^ local_box_eaxis[l]).normal_safe();
										if(a.lengthsq())
										{
											if(!unpenetrate(local_box_center, local_box_eaxis, faces, verts, VECTOR_BEGIN_PTR(candidates) + i, len, a, unpen))
											{
												separate = true;
												break;
											}
											if(FABS(unpen) < FABS(group_shortest_unpen))
											{
												group_shortest_unpen = unpen;
												group_axis = a;
											}
										}
									}
								}
							}
						}
					}

					if(!separate && FABS(group_shortest_unpen) < FABS(local_shortest_unpen))
					{
						local_shortest_unpen = group_shortest_unpen;
						local_axis = group_axis;
					}

					i += len;
				}

				if(local_shortest_unpen == INFINITY)
					continue;

				Vec3 unpen_disp = local_shortest_unpen * local_axis * world_sr;
				REAL unpen = unpen_disp.length();
				if(unpen < shortest_unpen)
				{
					shortest_unpen = unpen;
					shortest_unpen_disp = unpen_disp;
					contact_node = mn;
				}
			}

			if(shortest_unpen == INFINITY)
				return false;

			return true;
		}
		bool				FPSCore::relaxAABB(const AABB& box, const AABB search_box, const SPtr<SNode>& map_node, Vec3& shortest_unpen_disp, SPtr<SNode>& contact_node)
		{
			return relaxOBB(box, search_box, map_node, shortest_unpen_disp, contact_node);
		}
		bool				FPSCore::anyOverlap(const Vec3& so, REAL sr, const SPtr<SNode>& map_node)
		{
			Vec3 radius_vec(sr, sr, sr);

			AABB search_box = AABB(-radius_vec * 1.015625f + so, radius_vec * 1.015625f + so);
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());

				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				if(array_index.size())
				{
					Mat3 inv_world_sr = (Mat3)mn->inv_world_trans();

					Vec3 local_center = so * mn->inv_world_trans();
					REAL local_radius = (radius_vec.normal() * sr * inv_world_sr).length();

					const Vec3* verts = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();

					for(size_t i = 0; i < array_index.size(); ++i)
					{
						int32_t triidx = array_index[i];

						const vnFace& f = faces[triidx];
						const Vec3& v0 = verts[f.v[0]];
						const Vec3& v1 = verts[f.v[1]];
						const Vec3& v2 = verts[f.v[2]];

						if(Test::SphereTriangle(local_center, local_radius, v0, v1, v2))
							return true;
					}
				}
			}

			return false;
		}
		bool				FPSCore::anyOverlap(const OBB& box, const SPtr<SNode>& map_node)
		{
			AABB search_box = box.fittingAABB();
			search_box = AABB(-search_box.extent() * 1.015625f + search_box.center(), search_box.extent() * 1.015625f + search_box.center());
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return false;

			vector<int32_t, frame_temp_allocator<int32_t>> array_index;

			const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
			for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
			{
				SMeshNode* mn = (SMeshNode*)node_iter->get();

				OBB local_box(box);
				local_box.transformBy(mn->inv_world_trans());
				AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());
				mn->buildSPT();

				array_index.clear();
				mn->spt()->intersect(local_search_box, array_index);

				if(array_index.size())
				{
					const Vec3& local_box_center = local_box.center();
					const Vec3& local_box_extent = local_box.extent();
					Vec3 local_box_eaxis[3] = {local_box.axis()[0] * local_box_extent[0], local_box.axis()[1] * local_box_extent[1], local_box.axis()[2] * local_box_extent[2]};

					Vec3 local_up = (Vec3::yAxis * ((Mat3)mn->inv_world_trans())).normal();

					const Vec3* verts = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();

					for(size_t i = 0; i < array_index.size(); ++i)
					{
						Z_STATIC_ASSERT(sizeof(SMeshNode*) == sizeof(uint32_t), POINTER_SIZE_ASSUMPTION_WRONG);

						int32_t triidx = array_index[i];

						const vnFace& f = faces[triidx];
						const Vec3& v0 = verts[f.v[0]];
						const Vec3& v1 = verts[f.v[1]];
						const Vec3& v2 = verts[f.v[2]];

						if(!Test::OBBTriangle(local_box_center, local_box_eaxis, v0, v1, v2))
							return true;
					}
				}
			}
			return false;
		}
		bool				FPSCore::anyOverlap(const AABB& box, const SPtr<SNode>& map_node)
		{
			return anyOverlap(OBB(box), map_node);
		}
		bool				FPSCore::selectTriangle(STriangleSelector& selector, const Ray& ray, const SPtr<SNode>& map_node)
		{
			selector.reset(ray);
			selector.traverse(map_node);
			if(selector.time == INFINITY)
				return false;
			return true;
		}
		bool				FPSCore::selectTriangle(const Ray& ray, const SPtr<SNode>& map_node, REAL& time, SPtr<SMeshNode>& mn, int32_t& triidx)
		{
			STriangleSelector s(ray);
			if(!selectTriangle(s, ray, map_node))
				return false;
			time = s.time;
			mn = s.mn;
			triidx = s.tri_idx;

			return true;
		}
		void				FPSCore::OBBTriangle(const OBB& box,const TriangleAux& tri,int cur_iter,int max_iter,Vec3& sum,int& hit)
		{
			if(!Test::OBBTriangle(box,tri))
				return;
			if(cur_iter+1==max_iter)
			{
				sum+=box.center();
				++hit;
				return;
			}
			int next_iter=cur_iter+1;
			switch(cur_iter%3)
			{
			case 0:
				{
					Vec3 e=box.extent();
					e.x*=0.5f;
					OBBTriangle(OBB(box.center()+box.axis(0)*box.extent()[0]*0.5f,box.axis(),e),tri,next_iter,max_iter,sum,hit);
					OBBTriangle(OBB(box.center()-box.axis(0)*box.extent()[0]*0.5f,box.axis(),e),tri,next_iter,max_iter,sum,hit);
				}
				break;
			case 1:
				{
					Vec3 e=box.extent();
					e.y*=0.5f;
					OBBTriangle(OBB(box.center()+box.axis(1)*box.extent()[1]*0.5f,box.axis(),e),tri,next_iter,max_iter,sum,hit);
					OBBTriangle(OBB(box.center()-box.axis(1)*box.extent()[1]*0.5f,box.axis(),e),tri,next_iter,max_iter,sum,hit);
				}
				break;
			case 2:
				{
					Vec3 e=box.extent();
					e.z*=0.5f;
					OBBTriangle(OBB(box.center()+box.axis(2)*box.extent()[2]*0.5f,box.axis(),e),tri,next_iter,max_iter,sum,hit);
					OBBTriangle(OBB(box.center()-box.axis(2)*box.extent()[2]*0.5f,box.axis(),e),tri,next_iter,max_iter,sum,hit);
				}
				break;
			}
		}
		bool				FPSCore::unpenetrate(const OBB& box,const TriangleAux& tri,const Vec3& unpen_dir,REAL& d)
		{
			REAL t0,t1;
			if(!CD::OBBTriangle(box,unpen_dir,tri,t0,t1))
				return false;
			if(t1<0.0f)
				return false;
			d=t1;
			return true;
		}
		bool			FPSCore::tryUnpenetrate(const OBB& local_box,const TriangleAux& tri,const Vec3& local_unpen_dir,const Mat3& world_sr,REAL& min_unpen,Vec3& unpen_disp)
		{
			REAL d;
			if(!unpenetrate(local_box,tri,local_unpen_dir,d))
				return false;
			Vec3 disp=d*local_unpen_dir*world_sr;
			REAL dist=disp.length();
			if(FINITE(dist))
			{
				if(dist<min_unpen)
				{
					if(dist>0.0001f)
					{
						unpen_disp=disp;
						min_unpen=dist;
					}
					else
					{
						unpen_disp=(local_unpen_dir*world_sr).normal()*0.0001f;
						//min_unpen=0.0001f;
						min_unpen=dist;
					}
					return true;
				}
			}
			return false;
		}
	};
};

#pragma warning(default:4311)