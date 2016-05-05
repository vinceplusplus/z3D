
#pragma once

#include "predecl.h"
#include "DataLoader.h"

#include "VertexDeformCtrlBase.h"

namespace z3D
{
	namespace SG
	{
		class SkinCtrl: public VertexDeformCtrlBase
		{
			DECL_TYPEINFO();
		private:
			WPtr<SMeshNode>			_mesh_node;
		private:
			SPtr<SkinData>			_skin_data;
		private:
			WPtr<SStructNode>		_struct_node;
		private:
			vector< WPtr<SNode> >	_array_used_bone;
		protected:
			vector<Mat3>			_cached_vert_rot_tm;
			vector<Vec3>			_cached_vert_pos_tm;
			vector<Mat3>			_cached_norm_tm;
		public:
			SkinCtrl(const wcs& name, const SPtr<SMeshNode>& mesh_node, const SPtr<SkinData>& skin_data, const SPtr<SStructNode>& struct_node);
		protected:
			~SkinCtrl();
		public:
			bool					deforms(ATTRIB_TYPE attrib_type) const;
		public:
			SPtr<SStructNode>		getStructNode() const;
			SPtr<SMeshNode>			getMeshNode() const;
		public:
			void					update(REAL dt);
		public:
			ptrdiff_t				getPriority() const;
			bool					isUpdateDeferred() const;
		};
	};
};

