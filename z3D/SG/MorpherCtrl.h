
#pragma once

#include "predecl.h"
#include "DataLoader.h"

#include "VertexDeformCtrlBase.h"

namespace z3D
{
	namespace SG
	{
		class MorpherCtrl: public VertexDeformCtrlBase
		{
			DECL_TYPEINFO();
		public:
			class Channel
			{
			public:
				REAL					weight;
				AnimDataEval<REAL>		eval_weight;
			public:
				Channel();
			};
		private:
			WPtr<SMeshNode>			_mesh_node;
		private:
			SPtr<MorpherData>		_morpher_data;
		private:
			AnimCtrl				_anim_ctrl;
		private:
			vector<Channel>			_channels;
		public:
			MorpherCtrl(const wcs& name, const SPtr<SMeshNode>& mesh_node, const SPtr<MorpherData>& morpher_data);
		protected:
			~MorpherCtrl();
		public:
			bool					hasAnimData() const;
			void					getAnimDataTimeRange(REAL& start, REAL& end) const;
			AnimCtrl*				getAnimCtrl();
		public:
			bool					deforms(ATTRIB_TYPE attrib_type) const;
		public:
			void					update(REAL dt);
		public:
			ptrdiff_t				getPriority() const;
			bool					isUpdateDeferred() const;
		public:
			SPtr<SMeshNode>			getMeshNode() const;
		public:
			SPtr<MorpherData>		getMorpherData() const;
			const vector<Channel>&	getChannels() const;
		public:
			void					setChannelWeight(size_t channel_index, REAL weight);
			void					setChannelWeightAnimData(size_t channel_index, const SPtr< AnimData<REAL> >& animdata);
		};
	};
};

