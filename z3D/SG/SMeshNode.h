
#pragma once

#include "predecl.h"
#include "SNode.h"
//#include "NodeCtrlSet.h"

namespace z3D
{
	namespace SG
	{
		class SMeshNode:public SNode
		{
			DECL_TYPEINFO();
		private:
			SPtr<Mesh>				_mesh;
			//SPtr<MeshInst>			_meshinst;
			//SPtr<TriKDTree>			_spt;
			//vector<TriKDNode*>		_spt_node;
			//vector<int>				_spt_tri;
			//vector< vector<int> >	_spt_submesh_tri;
			//SPtr<TriOCTree>			_spt_oc;

			SPtr<btTriTree>			_spt;
			//SPtr<ocTriTree>			_spt;
		private:
			WPtr<CtrlBase>			_morpher_ctrl;
			WPtr<CtrlBase>			_skin_ctrl;
		private:
			SPtr<MeshRenderer>		_mr;
		protected:
			SMeshNode();
			~SMeshNode();
		public:
			static SPtr<SMeshNode>	load(const wcs& mesh_filepath, const wcs& mtl_filepath);
			static SPtr<SMeshNode>	create(const SPtr<LoadedMesh>& ref_mesh, const SPtr<LoadedMtl>& ref_mtl);
		public:
			void				buildControllers(const wcs& node_name);
		public:
			void				loadTextures();
			//void				buildVBO();
		public:
			bool				supportsSPT() const;
		public:
			void				buildSPT();
			void				clearSPT();
		public:
			//void				buildSPT_OC();
			//void				clearSPT_OC();
		public:
			const AABB&			local_bound();
		public:
			bool				canCull() const;
			bool				renderable() const;
			bool				canCastShadow() const;
			bool				manifold_2();
		public:
			//void				cull(const Frustum& frustum,const SPtr<SCameraNode>& camera);
			//void				constructShadow(const Vec3& light_pos,bool zfail);
			//void				renderShadow(const Vec3& light_world_pos,const ShadowCasterInfo& info);
			void				render(intptr_t parameter, SRenderInfo* ri);
		public:
			void				preRender(SRenderInfo* ri);
		public:
			const SPtr<Mesh>&			mesh();

			const SPtr<btTriTree>&		spt();
			//const SPtr<ocTriTree>&		spt();

		public:
			void					setMeshRenderer(const SPtr<MeshRenderer>& mr);
			const SPtr<MeshRenderer>&
									getMeshRenderer() const;
		public:
			void					setMorpherCtrl(const SPtr<CtrlBase>& ctrl);
			SPtr<CtrlBase>			getMorpherCtrl() const;
		public:
			void					setSkinCtrl(const SPtr<CtrlBase>& ctrl);
			SPtr<CtrlBase>			getSkinCtrl() const;
		public:
			size_t					polyCount();
		public:
			wcs						toString();
		};
	};
};

