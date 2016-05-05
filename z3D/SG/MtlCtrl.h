	
#pragma once

#include "predecl.h"
#include "Mesh.h"

namespace z3D
{
	namespace SG
	{
		class MtlCtrl: public CtrlBase
		{
		private:
			class MapAnimDataEval
			{
			public:
				AnimDataEval<REAL>				offset_u;
				AnimDataEval<REAL>				offset_v;
				AnimDataEval<REAL>				tiling_u;
				AnimDataEval<REAL>				tiling_v;
				AnimDataEval<REAL>				angle_u;
				AnimDataEval<REAL>				angle_v;
				AnimDataEval<REAL>				angle_w;
			public:
				MapAnimDataEval();
			public:
				void							setAnimData(MtlMap& mtlmap);
			public:
				void							updateMtlMap(MtlMap& mtlmap, REAL t);
			public:
				void							getAnimDataTimeRange(REAL& start, REAL& end) const;
			};
		public:
			DECL_TYPEINFO();
		private:
			WPtr<SMeshNode>					_mesh_node;
		private:
			size_t							_mtl_index;
		private:
			AnimCtrl						_anim_ctrl;
		private:
			AnimDataEval<Vec3>				_eval_ambient;
			AnimDataEval<Vec3>				_eval_diffuse;
			AnimDataEval<Vec3>				_eval_specular;
			AnimDataEval<REAL>				_eval_specular_level;
			AnimDataEval<REAL>				_eval_glossiness;
			AnimDataEval<REAL>				_eval_opacity;
		private:
			MapAnimDataEval					_eval_map_diffuse;
			MapAnimDataEval					_eval_map_normal;
			MapAnimDataEval					_eval_map_selfill;
			MapAnimDataEval					_eval_map_specular;
		public:
			MtlCtrl(const wcs& name, const SPtr<SMeshNode>& node, size_t mtl_index);
		protected:
			~MtlCtrl();
		public:
			bool							hasAnimData() const;
			void							getAnimDataTimeRange(REAL& start, REAL& end) const;
			AnimCtrl*						getAnimCtrl();
		public:
			void							update(REAL dt);
		public:
			ptrdiff_t						getPriority() const;
			bool							isUpdateDeferred() const;
		public:
			void							setAmbientAnimData(const SPtr< AnimData<Vec3> >& animdata);
			void							setDiffuseAnimData(const SPtr< AnimData<Vec3> >& animdata);
			void							setSpecularAnimData(const SPtr< AnimData<Vec3> >& animdata);
			void							setSpecularLevelAnimData(const SPtr< AnimData<REAL> >& animdata);
			void							setGlossinessAnimData(const SPtr< AnimData<REAL> >& animdata);
			void							setOpacityAnimData(const SPtr< AnimData<REAL> >& animdata);
		public:
			const SPtr< AnimData<Vec3> >&	getAmbientAnimData() const;
			const SPtr< AnimData<Vec3> >&	getDiffuseAnimData() const;
			const SPtr< AnimData<Vec3> >&	getSpecularAnimData() const;
			const SPtr< AnimData<REAL> >&	getSpecularLevelAnimData() const;
			const SPtr< AnimData<REAL> >&	getGlossinessAnimData() const;
			const SPtr< AnimData<REAL> >&	getOpacityAnimData() const;
		public:
			MapAnimDataEval&				getDiffuseMapAnimDataEval();
			MapAnimDataEval&				getNormalMapAnimDataEval();
			MapAnimDataEval&				getSelfillMapAnimDataEval();
			MapAnimDataEval&				getSpecularMapAnimDataEval();
		public:
			REAL							getAnimDataStartTime() const;
			REAL							getAnimDataEndTime() const;
		};
	};
};

