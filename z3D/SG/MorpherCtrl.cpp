
#ifdef _OPENMP
#include <omp.h>
#endif

#include "MorpherCtrl.h"
#include "SMeshNode.h"
#include "DataLoader.h"
#include "Mesh.h"

namespace z3D
{
	namespace SG
	{
		MorpherCtrl::Channel::Channel()
		{
			weight = 0.0f;
		}

		MorpherCtrl::MorpherCtrl(const wcs& name, const SPtr<SMeshNode>& mesh_node, const SPtr<MorpherData>& morpher_data)
			: VertexDeformCtrlBase(name), _mesh_node(mesh_node), _morpher_data(morpher_data)
		{
			_channels.resize(morpher_data->channel_count);
		}
		MorpherCtrl::~MorpherCtrl()
		{
		}
		bool					MorpherCtrl::hasAnimData() const
		{
			return true;
		}
		template<class T> static REAL	getStartTime(const AnimDataEval<T>& eval, REAL default_time)
		{
			if(!eval.anim_data() || !eval.anim_data()->getKeyCount())
				return default_time;
			return eval.anim_data()->getKey(0).time;
		}
		template<class T> static REAL	getEndTime(const AnimDataEval<T>& eval, REAL default_time)
		{
			if(!eval.anim_data() || !eval.anim_data()->getKeyCount())
				return default_time;
			return eval.anim_data()->getKey(eval.anim_data()->getKeyCount() - 1).time;
		}
		void					MorpherCtrl::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			start = INFINITY;
			end = -INFINITY;
			for(size_t i = 0; i < _channels.size(); ++i)
			{
				start = min(getStartTime(_channels[i].eval_weight, start), start);
				end = max(getEndTime(_channels[i].eval_weight, end), end);
			}
		}
		AnimCtrl*				MorpherCtrl::getAnimCtrl()
		{
			return &_anim_ctrl;
		}
		bool					MorpherCtrl::deforms(ATTRIB_TYPE attrib_type) const
		{
			switch(attrib_type)
			{
			case VertexDeformCtrlBase::ATTRIB_TYPE_POSITION:
			case VertexDeformCtrlBase::ATTRIB_TYPE_NORMAL:
				return true;
			}
			return false;
		}
		void					MorpherCtrl::update(REAL dt)
		{
			if(!_mesh_node.valid())
				return;

			Z_ASSERT( find(_mesh_node.get_unsafe()->getCtrlSet().getArray().begin(), _mesh_node.get_unsafe()->getCtrlSet().getArray().end(), this) != _mesh_node.get_unsafe()->getCtrlSet().getArray().end());

			_anim_ctrl.advance(dt);

			REAL t = _anim_ctrl.current_stage().getTime();

			for(size_t i = 0; i < _channels.size(); ++i)
			{
				Channel& ch = _channels[i];
				if(ch.eval_weight.anim_data())
					ch.weight = ch.eval_weight.sample(t);
			}

			SMeshNode* node = (SMeshNode*)_mesh_node.get_unsafe();

			Mesh& mesh = *node->mesh().get();

			if(isFirstDeformer(_mesh_node.lock(), VertexDeformCtrlBase::ATTRIB_TYPE_POSITION))
			{}
			else
			{}

			Vec3* verts;
			Vec3* norms;
			if(true)
			{
				vector<Vec3>& array_vert = mesh.data.verts.edit();
				vector<Vec3>& array_norm = mesh.data.norms.edit();
				array_vert.resize(_morpher_data->vert_count);
				array_norm.resize(_morpher_data->norm_count);
				verts = VECTOR_BEGIN_PTR(array_vert);
				norms = VECTOR_BEGIN_PTR(array_norm);
			}

			MorpherData* morpher_data = _morpher_data.get();

			memcpy(verts, morpher_data->base_verts, sizeof(Vec3) * morpher_data->vert_count);
			memcpy(norms, morpher_data->base_norms, sizeof(Vec3) * morpher_data->norm_count);

			for(size_t i = 0; i < _channels.size(); ++i)
			{
				Channel& ch = _channels[i];
				REAL weight = ch.weight;

				if(FABS(weight) <= EPSILON)
					continue;

#ifndef _OPENMP
				if(true)
				{
					Vec3* dst = verts;
					const Vec3* src_end = morpher_data->chans_verts[i] + morpher_data->vert_count;
					for(const Vec3* src = morpher_data->chans_verts[i]; src != src_end; ++src, ++dst)
						*dst += *src * weight;
				}
#else
				if(true)
				{
					const Vec3* src = morpher_data->chans_verts[i];
					Vec3* dst = verts;
					ptrdiff_t c = (ptrdiff_t)morpher_data->vert_count;
#pragma omp parallel for
					for(ptrdiff_t j = 0; j < c; ++j)
						dst[j] += src[j] * weight;
				}
#endif

#ifndef _OPENMP
				if(true)
				{
					Vec3* dst = norms;
					const Vec3* src_end = morpher_data->chans_norms[i] + morpher_data->norm_count;
					for(const Vec3* src = morpher_data->chans_norms[i]; src != src_end; ++src, ++dst)
						*dst += *src * weight;
				}
#else
				if(true)
				{
					const Vec3* src = morpher_data->chans_norms[i];
					Vec3* dst = norms;
					ptrdiff_t c = (ptrdiff_t)morpher_data->norm_count;
#pragma omp parallel for
					for(ptrdiff_t j = 0; j < c; ++j)
						dst[j] += src[j] * weight;
				}
#endif
			}

			// about LOD, we can sort processing channels in descending order by the weight, then depending on the LOD, process up to a number of channels

			node->markDirtyBound();
			node->clearSPT();
		}
		ptrdiff_t				MorpherCtrl::getPriority() const
		{
			return 20000;
		}
		bool					MorpherCtrl::isUpdateDeferred() const
		{
			return false;
		}
		SPtr<SMeshNode>			MorpherCtrl::getMeshNode() const
		{
			return _mesh_node.lock();
		}
		SPtr<MorpherData>		MorpherCtrl::getMorpherData() const
		{
			return _morpher_data;
		}
		const vector<MorpherCtrl::Channel>&
								MorpherCtrl::getChannels() const
		{
			return _channels;
		}
		void					MorpherCtrl::setChannelWeight(size_t channel_index, REAL weight)
		{
			Z_ASSERT( channel_index < _channels.size() );

			_channels[channel_index].weight = weight;
		}
		void					MorpherCtrl::setChannelWeightAnimData(size_t channel_index, const SPtr< AnimData<REAL> >& animdata)
		{
			Z_ASSERT( channel_index < _channels.size() );

			_channels[channel_index].eval_weight = animdata;
		}
	};
};

