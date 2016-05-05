
#include "SMeshNode.h"
#include "Mesh.h"
#include "MeshInst.h"
#include "SMgr.h"
#include "MtlCtrl.h"
#include "MeshRenderer.h"
#include "SkinCtrl.h"

namespace z3D
{
	namespace SG
	{
		SMeshNode::SMeshNode()
		{
		}
		SMeshNode::~SMeshNode()
		{
		}
		SPtr<SMeshNode>		SMeshNode::load(const wcs& mesh_filepath, const wcs& mtl_filepath)
		{
			SPtr<Mesh> mesh = Mesh::load(mesh_filepath, mtl_filepath);
			if(!mesh)
				return NULL;

			SMeshNode* node = new SMeshNode();
			node->_mesh = mesh;

			return node;
		}
		SPtr<SMeshNode>		SMeshNode::create(const SPtr<LoadedMesh>& ref_mesh, const SPtr<LoadedMtl>& ref_mtl)
		{
			SPtr<Mesh> mesh = Mesh::create(ref_mesh, ref_mtl);

			if(!mesh)
				return NULL;

			SMeshNode* node = new SMeshNode();
			node->_mesh = mesh;

			return node;
		}
		void					SMeshNode::buildControllers(const wcs& node_name)
		{
			vector<Mtl>& array_mtl = _mesh->data.mtls.edit();
			for(size_t i = 0; i < array_mtl.size(); ++i)
			{
				Mtl& mtl = array_mtl[i];
				if(mtl.animdata.ambient || mtl.animdata.diffuse || mtl.animdata.specular || mtl.animdata.specular_level || mtl.animdata.glossiness || mtl.animdata.opacity ||
					mtl.map_diffuse.animdata.any() || mtl.map_normal.animdata.any() || mtl.map_selfill.animdata.any() || mtl.map_specular.animdata.any())
				{
					MtlCtrl* ctrl=new MtlCtrl(wcs(L"MtlCtrl: ") + node_name + FORMATW(L" %d",i), this, i);
					ctrl->setAmbientAnimData(mtl.animdata.ambient);
					ctrl->setDiffuseAnimData(mtl.animdata.diffuse);
					ctrl->setSpecularAnimData(mtl.animdata.specular);
					ctrl->setSpecularLevelAnimData(mtl.animdata.specular_level);
					ctrl->setGlossinessAnimData(mtl.animdata.glossiness);
					ctrl->setOpacityAnimData(mtl.animdata.opacity);

					ctrl->getDiffuseMapAnimDataEval().setAnimData(mtl.map_diffuse);
					ctrl->getNormalMapAnimDataEval().setAnimData(mtl.map_normal);
					ctrl->getSelfillMapAnimDataEval().setAnimData(mtl.map_selfill);
					ctrl->getSpecularMapAnimDataEval().setAnimData(mtl.map_specular);

					if(true)
					{
						REAL start, end;
						ctrl->getAnimDataTimeRange(start, end);
						ctrl->getAnimCtrl()->set(TimeAnimCfg(start, end, -1));
					}

					getCtrlSet().attachCtrl(ctrl);
				}
			}
		}
		void					SMeshNode::loadTextures()
		{
			if(_mesh)
				_mesh->loadTextures();
		}
		/*
		void					SMeshNode::buildVBO()
		{
			if(_meshinst)
				_meshinst->buildVBO_VND();
		}
		*/
		bool					SMeshNode::supportsSPT() const
		{
			return true;
		}
		void					SMeshNode::buildSPT()
		{
			if(_spt)
				return;

			/*
			vector<int> face_vert_idx;
			face_vert_idx.reserve(_meshinst->array_face.size()*3);
			int i;
			for(i=0;i<(int)_meshinst->array_face.size();++i)
			{
				face_vert_idx.push_back(_meshinst->array_face[i].v[0]);
				face_vert_idx.push_back(_meshinst->array_face[i].v[1]);
				face_vert_idx.push_back(_meshinst->array_face[i].v[2]);
			}

			_spt=TriKDTree::buildTree(&_meshinst->array_vert[0],&face_vert_idx[0],(int)_meshinst->array_face.size(),(int)POW((REAL)_meshinst->array_face.size(),2.0f/3.0f),(int)POW((REAL)_meshinst->array_face.size(),1.0f/3.0f),20);
			*/
			//if(!_mesh->data.face.get().size() || !_mesh->data.vert.get().size())
			//	return;
			const Vec3* verts = _mesh->get_verts();
			const vnFace* faces = _mesh->get_faces();
			vector<INT3> index;
			const vnFace* face_end = faces + _mesh->get_face_count();
			for(const vnFace* face_iter = faces; face_iter != face_end; ++face_iter)
				index.push_back(face_iter->v);

			_spt = btTriTree::create(verts, _mesh->get_vert_count(), VECTOR_BEGIN_PTR(index), index.size());
			//_spt = ocTriTree::create(verts, _mesh->get_vert_count(), VECTOR_BEGIN_PTR(index), index.size());
		}
		void					SMeshNode::clearSPT()
		{
			_spt.reset();
		}
		/*
		void					SMeshNode::buildSPT_OC()
		{
			//SAFE_RELEASE(_spt_oc);
			_spt_oc.reset();

			vector<int> face_vert_idx;
			face_vert_idx.reserve(_meshinst->array_face.size()*3);
			int i;
			for(i=0;i<(int)_meshinst->array_face.size();++i)
			{
				face_vert_idx.push_back(_meshinst->array_face[i].v[0]);
				face_vert_idx.push_back(_meshinst->array_face[i].v[1]);
				face_vert_idx.push_back(_meshinst->array_face[i].v[2]);
			}

			_spt_oc=TriOCTree::buildTree(&_meshinst->array_vert[0],(int)_meshinst->array_vert.size(),&face_vert_idx[0],(int)_meshinst->array_face.size());
		}
		void					SMeshNode::clearSPT_OC()
		{
			//SAFE_RELEASE(_spt_oc);
			_spt_oc.reset();
		}
		*/
		const AABB&			SMeshNode::local_bound()
		{
			return _mesh->local_bound();
		}
		bool					SMeshNode::canCull() const
		{
			return false;
			//return _spt?true:false;
		}
		bool					SMeshNode::renderable() const
		{
			return true;
		}
		bool					SMeshNode::canCastShadow() const
		{
			/*
			int i;
			for(i=0;i<(int)_meshinst->array_submesh.size();++i)
				if(_meshinst->array_mtlinst[_meshinst->array_submesh[i].mtl_index].reflect)
					return false;
				else if(_meshinst->array_mtlinst[_meshinst->array_submesh[i].mtl_index].alpha_blend)
					return false;
			return true;
			*/
			return true;
		}
		bool					SMeshNode::manifold_2()
		{
			return _mesh->manifold_2();
		}
		/*
		void					SMeshNode::cull(const Frustum& frustum,const SPtr<SCameraNode>& camera)
		{
			if(!_spt)
				return;
			_spt_node.clear();
			_spt_tri.clear();
			_spt->cull(Frustum((camera->world_trans()*inv_world_trans()).inverse()*camera->proj()),_spt_node);
			_spt->getTriangle(_spt_node,_spt_tri);
			_meshinst->sortSubMeshTri(_spt_tri,_spt_submesh_tri);
		}
		*/
		/*
		void					SMeshNode::constructShadow(const Vec3& light_pos,bool zfail)
		{
		}
		void					SMeshNode::renderShadow(const Vec3& light_world_pos,const ShadowCasterInfo& info)
		{
			Vec3 light_pos=light_world_pos*inv_world_trans();
			if(info.zfail)
				_meshinst->drawShadowZFail(light_pos,info.light_cap,info.dark_cap);
			else
				_meshinst->drawShadowZPass(light_pos,info.back_cap);
		}
		*/
		void					SMeshNode::render(intptr_t parameter, SRenderInfo* ri)
		{
			if(true)
			{
				const SPtr<MeshRenderer>& mr = (_mr ? _mr : DefMeshRenderer::instance());
				SRenderParamQuery param(ri);
				switch(ri->getDrawPass())
				{
				case DRAWPASS_DEPTH_ONLY:
					{
						mr->draw_depth_only(_mesh, parameter, param);
					}
					break;
				case DRAWPASS_DEPTH_ONLY_SOLID_RECV_SHADOW_VSM:
					{
						mr->draw_depth_only_vsm(_mesh, parameter, param);
					}
					break;
				case DRAWPASS_GEN_SHADOW_MAP_VSM:
					{
						mr->draw_gen_shadow_map_vsm(_mesh, parameter, param);
					}
					break;
				case DRAWPASS_SHADE:
				case DRAWPASS_SHADE_EQUAL_DEPTH:
					{
						mr->draw(_mesh, parameter, param);
					}
					break;
				}
			}
		}
		void					SMeshNode::preRender(SRenderInfo* ri)
		{
			DRAWPASS pass = ri->getDrawPass();

			//if(dc == DRAWCONTROL_GENERATE_SHADOWMAP && !castShadow())
			//	return;

			const vector<Mtl>& array_mtl = _mesh->data.mtls.get();
			const SubMesh* submeshes = _mesh->get_submeshes();
			size_t submesh_count = _mesh->get_submesh_count();
			if(false/*dc == DRAWCONTROL_GENERATE_SHADOWMAP*/)
			{
				for(size_t i = 0; i < submesh_count; ++i)
				{
					const Mtl& mtl = array_mtl[submeshes[i].mtl_index % array_mtl.size()];
					if(mtl.type != Mtl::TYPE_SOLID)
						continue;
					if(mtl.opacity * getFinalColor().w < 1.0f)
						continue;
					ri->regSolidItem(this, i);
				}
			}
			else
			{
				for(size_t i = 0; i < submesh_count; ++i)
				{
					const Mtl& mtl = array_mtl[submeshes[i].mtl_index % array_mtl.size()];
					if(mtl.opacity <= 0.0f || getFinalColor().w <= 0.0f)
						continue;
					if(mtl.type == Mtl::TYPE_SOLID && mtl.opacity * getFinalColor().w >= 1.0f && !getFinalForceBlend())
						ri->regSolidItem(this, i);
					else
					{
						ri->regTransparentItem(this, i, mtl.layer_index);
					}
				}
			}
		}
		const SPtr<Mesh>&			SMeshNode::mesh()
		{
			return _mesh;
		}

		const SPtr<btTriTree>&			SMeshNode::spt()
		//const SPtr<ocTriTree>&			SMeshNode::spt()
		{
			return _spt;
		}

		void					SMeshNode::setMeshRenderer(const SPtr<MeshRenderer>& mr)
		{
			_mr = mr;
		}
		const SPtr<MeshRenderer>&
								SMeshNode::getMeshRenderer() const
		{
			return _mr;
		}
		void					SMeshNode::setMorpherCtrl(const SPtr<CtrlBase>& ctrl)
		{
			if(_morpher_ctrl.valid())
				getCtrlSet().detachCtrl(_morpher_ctrl.lock());
			if(ctrl)
				getCtrlSet().attachCtrl(ctrl);
			_morpher_ctrl = ctrl;
		}
		SPtr<CtrlBase>			SMeshNode::getMorpherCtrl() const
		{
			return _morpher_ctrl.lock();
		}
		void					SMeshNode::setSkinCtrl(const SPtr<CtrlBase>& ctrl)
		{
			if(_skin_ctrl.valid())
				getCtrlSet().detachCtrl(_skin_ctrl.lock());
			if(ctrl)
				getCtrlSet().attachCtrl(ctrl);
			_skin_ctrl = ctrl;
		}
		SPtr<CtrlBase>			SMeshNode::getSkinCtrl() const
		{
			return _skin_ctrl.lock();
		}
		size_t					SMeshNode::polyCount()
		{
			return _mesh->get_face_count();
		}
		wcs						SMeshNode::toString()
		{
			return SNode::toString() + L", \"" + (mesh()->getRef() ? mesh()->getRef()->filepath() : L"") + L"\"";
		}
	};
};
