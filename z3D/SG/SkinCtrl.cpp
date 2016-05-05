
#ifdef _OPENMP
#include <omp.h>
#endif

#include "SkinCtrl.h"
#include "SStructNode.h"
#include "SMeshNode.h"
#include "MeshInst.h"

#define WIN32_MEAN_AND_LEAN

#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace SG
	{
		SkinCtrl::SkinCtrl(const wcs& name, const SPtr<SMeshNode>& mesh_node, const SPtr<SkinData>& skin_data, const SPtr<SStructNode>& struct_node)
			: VertexDeformCtrlBase(name), _mesh_node(mesh_node), _skin_data(skin_data), _struct_node(struct_node)
		{
			const wchar_t* bn = skin_data->array_bone_name;
			for(size_t i = 0; i < skin_data->bone_count; ++i)
			{
				_array_used_bone.push_back(struct_node->getLoadedNode(bn));
				bn += wcslen(bn) + 1;
			}
		}
		SkinCtrl::~SkinCtrl()
		{
		}
		bool					SkinCtrl::deforms(ATTRIB_TYPE attrib_type) const
		{
			switch(attrib_type)
			{
			case VertexDeformCtrlBase::ATTRIB_TYPE_POSITION:
			case VertexDeformCtrlBase::ATTRIB_TYPE_NORMAL:
				return true;
			}
			return false;
		}
		SPtr<SStructNode>		SkinCtrl::getStructNode() const
		{
			return _struct_node.lock();
		}
		SPtr<SMeshNode>			SkinCtrl::getMeshNode() const
		{
			return _mesh_node.lock();
		}
		void					SkinCtrl::update(REAL dt)
		{
			if(!_mesh_node.valid() || !_struct_node.valid())
				return;

			Z_ASSERT( find(_mesh_node.get_unsafe()->getCtrlSet().getArray().begin(), _mesh_node.get_unsafe()->getCtrlSet().getArray().end(), this) != _mesh_node.get_unsafe()->getCtrlSet().getArray().end());

			SMeshNode* node = (SMeshNode*)_mesh_node.get_unsafe();
			const SkinData::VERT* skinverts = _skin_data->verts;

			size_t vert_count = _skin_data->vert_count;
			size_t norm_count = _skin_data->norm_count;

			const Mat4 m2 = _struct_node.get_unsafe()->inv_world_trans() * _skin_data->inv_init_world_tm;

			Mesh& mesh = *node->mesh().get();

			Vec3* verts = NULL;
			Vec3* norms = NULL;

			if(true)
			{
				vector<Vec3>& v = mesh.data.verts.edit();
				vector<Vec3>& n = mesh.data.norms.edit();
				v.resize(vert_count);
				n.resize(norm_count);
				verts = VECTOR_BEGIN_PTR(v);
				norms = VECTOR_BEGIN_PTR(n);
			}

			if(_cached_vert_rot_tm.size() != _array_used_bone.size())
				_cached_vert_rot_tm.resize(_array_used_bone.size());
			if(_cached_vert_pos_tm.size() != _array_used_bone.size())
				_cached_vert_pos_tm.resize(_array_used_bone.size());
			if(_cached_norm_tm.size() != _array_used_bone.size())
				_cached_norm_tm.resize(_array_used_bone.size());

#ifndef _OPENMP
			for(size_t i = 0; i < _array_used_bone.size(); ++i)
			{
				if(!_array_used_bone[i].valid())
					continue;

				// this one can allow us to offset mesh node
				//Mat4 m = _skin_data->array_dismount_tm[i] * _array_used_bone[i].get_unsafe()->world_trans() * m2;

				// this one conforms to the option of back transform vertices option found in max's skin modifier and back transform vertices is on by default
				Mat4 m = _skin_data->array_dismount_tm[i] * _array_used_bone[i].get_unsafe()->world_trans() * node->inv_world_trans();

				// because i haven't come up a solution to detect which method we should use (i am using igame and it doesn't have a function to check this), i just picked one up (the default)

				_cached_vert_rot_tm[i] = (Mat3)m;
				_cached_vert_pos_tm[i] = Vec3(m[3]);
				_cached_norm_tm[i] = ((Mat3)m).inverse().transpose();
			}
#else
			for(size_t i = 0; i < _array_used_bone.size(); ++i)
			{
				if(!_array_used_bone[i].valid())
					continue;
				_array_used_bone[i].get_unsafe()->world_trans();
			}

			node->inv_world_trans();

			ptrdiff_t c = (ptrdiff_t)_array_used_bone.size();
#pragma omp parallel for
			for(ptrdiff_t i = 0; i < c; ++i)
			{
				if(!_array_used_bone[i].valid())
					continue;

				// this one can allow us to offset mesh node
				//Mat4 m = _skin_data->array_dismount_tm[i] * _array_used_bone[i].get_unsafe()->world_trans() * m2;

				// this one conforms to the option of back transform vertices option found in max's skin modifier and back transform vertices is on by default
				Mat4 m = _skin_data->array_dismount_tm[i] * _array_used_bone[i].get_unsafe()->world_trans() * node->inv_world_trans();

				// because i haven't come up a solution to detect which method we should use (i am using igame and it doesn't have a function to check this), i just picked one up (the default)

				_cached_vert_rot_tm[i] = (Mat3)m;
				_cached_vert_pos_tm[i] = Vec3(m[3]);
				_cached_norm_tm[i] = ((Mat3)m).inverse().transpose();
			}
#endif
			const Mat3* cached_vert_rot_tm = &_cached_vert_rot_tm[0];
			const Vec3* cached_vert_pos_tm = &_cached_vert_pos_tm[0];
			const Mat3* cached_norm_tm = &_cached_norm_tm[0];

			if(true)
			{
				const Vec3* src_vert = NULL;

				if(isFirstDeformer(_mesh_node.lock(), VertexDeformCtrlBase::ATTRIB_TYPE_POSITION))
				{
					Z_ASSERT( mesh.getRef()->data.verts );
					src_vert = mesh.getRef()->data.verts;

					Z_ASSERT( mesh.getRef()->data.vert_count == vert_count );
				}
				else
					src_vert = verts;

#ifndef _OPENMP
				if(true)
				{
					const Vec3* src_ptr = src_vert;
					Vec3* dst_ptr = verts;
					const int32_t* vert_offset = _skin_data->verts_offsets;
					for(size_t i = 0; i < vert_count; ++i)
					{
						const Vec3& src = *src_ptr;
						Vec3& dst = *dst_ptr;
						const SkinData::VERT& skinvert = *(const SkinData::VERT*)((const char*)skinverts + *vert_offset);
						if(skinvert.bone_weight_count)
						{
							Vec3 v;
							const SkinData::VERT::BONE_WEIGHT* bw_end = skinvert.bone_weights + skinvert.bone_weight_count;
							for(const SkinData::VERT::BONE_WEIGHT* bw = skinvert.bone_weights; bw != bw_end; ++bw)
								v += (src * cached_vert_rot_tm[bw->b] + cached_vert_pos_tm[bw->b]) * bw->w;
							dst = v;
						}
						else
							dst = src;

						++src_ptr;
						++dst_ptr;
						++vert_offset;
					}
				}
#else
				if(true)
				{
					ptrdiff_t c = (ptrdiff_t)vert_count;
#pragma omp parallel for
					for(ptrdiff_t i = 0; i < c; ++i)
					{
						const Vec3& src = *(src_vert + i);
						Vec3& dst = *(verts + i);
						const SkinData::VERT& skinvert = *(const SkinData::VERT*)((const char*)skinverts + _skin_data->verts_offsets[i]);
						if(skinvert.bone_weight_count)
						{
							Vec3 v;
							const SkinData::VERT::BONE_WEIGHT* bw_end = skinvert.bone_weights + skinvert.bone_weight_count;
							for(const SkinData::VERT::BONE_WEIGHT* bw = skinvert.bone_weights; bw != bw_end; ++bw)
								v += (src * cached_vert_rot_tm[bw->b] + cached_vert_pos_tm[bw->b]) * bw->w;
							dst = v;
						}
						else
							dst = src;
					}
				}
#endif
			}

			if(true)
			{
				const Vec3* src_norm = NULL;

				if(isFirstDeformer(_mesh_node.lock(), VertexDeformCtrlBase::ATTRIB_TYPE_NORMAL))
				{
					Z_ASSERT( mesh.getRef()->data.norms );
					src_norm = mesh.getRef()->data.norms;

					Z_ASSERT( mesh.getRef()->data.norm_count == norm_count );
				}
				else
					src_norm = norms;

#ifndef _OPENMP
				if(true)
				{
					const Vec3* src_ptr = src_norm;
					Vec3* dst_ptr = norms;
					const int32_t* norm_offset = _skin_data->norms_offsets;
					for(size_t i = 0; i < norm_count; ++i)
					{
						const Vec3& src = *src_ptr;
						Vec3& dst = *dst_ptr;
						const SkinData::VERT& skinvert = *(const SkinData::VERT*)((const char*)skinverts + *norm_offset);
						if(skinvert.bone_weight_count)
						{
							Vec3 v;
							const SkinData::VERT::BONE_WEIGHT* bw_end = skinvert.bone_weights + skinvert.bone_weight_count;
							for(const SkinData::VERT::BONE_WEIGHT* bw = skinvert.bone_weights; bw != bw_end; ++bw)
								v += src * cached_norm_tm[bw->b] * bw->w;
							dst = v;
						}
						else
							dst = src;

						++src_ptr;
						++dst_ptr;
						++norm_offset;
					}
				}
#else
				if(true)
				{
					ptrdiff_t c = (ptrdiff_t)norm_count;
#pragma omp parallel for
					for(ptrdiff_t i = 0; i < c; ++i)
					{
						const Vec3& src = *(src_norm + i);
						Vec3& dst = *(norms + i);
						const SkinData::VERT& skinnorm = *(const SkinData::VERT*)((const char*)skinverts + _skin_data->norms_offsets[i]);
						if(skinnorm.bone_weight_count)
						{
							Vec3 v;
							const SkinData::VERT::BONE_WEIGHT* bw_end = skinnorm.bone_weights + skinnorm.bone_weight_count;
							for(const SkinData::VERT::BONE_WEIGHT* bw = skinnorm.bone_weights; bw != bw_end; ++bw)
								v += (src * cached_norm_tm[bw->b]) * bw->w;
							dst = v;
						}
						else
							dst = src;
					}
				}
#endif
			}
			// about lod, as we already have sorted the bone weight in each skinned vertex / normal, so we can process up a number of bones each vertex, but a final division is needed for each vertex

			node->markDirtyBound();
			node->clearSPT();
		}
		ptrdiff_t				SkinCtrl::getPriority() const
		{
			return 10000;
		}
		bool					SkinCtrl::isUpdateDeferred() const
		{
			return true;
		}
	};
};
