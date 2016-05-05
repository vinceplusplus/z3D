
#include "MeshInst.h"

#if 0

namespace z3D
{
	namespace SG
	{
		MeshInst::MeshInst(const SPtr<Mesh>& mesh)
		{
			_mesh=mesh;

			array_submesh.resize(mesh->data.submesh.size());
			for(int i=0;i<(int)array_submesh.size();++i)
				(SubMesh&)(array_submesh[i]) = mesh->data.submesh[i];

			array_mtl.resize(mesh->data.mtl.size());
			for(int i=0;i<(int)array_mtl.size();++i)
				array_mtl[i]=mesh->data.mtl[i];

			endEdit(true,true,true);
		}

		void					MeshInst::prepare_vbo_vert(int submesh)
		{
			Z_ASSERT(submesh>=0 && submesh<(int)array_submesh.size());
			SubMeshInst& sub=array_submesh[submesh];

			// use original
			if(!array_vert.size())
			{
				if(!sub.vbo_vert || !sub.vbo_vert->isNamed() || sub.vbo_vert->dirty())
					sub.vbo_vert=_mesh->vbo_vert(submesh);
				return;
			}
			// instanced vertex attributes
			if(sub.vbo_vert && sub.vbo_vert->isNamed())
				sub.vbo_vert.reset();						// reset it if still using original

			Z_ASSERT(sub.face_count);

			SPtr<VBO> vbo=sub.vbo_vert;
			if(!vbo || vbo->length() < (int)(sizeof(Vec3)*sub.face_count*3))
				vbo=VBO::create(L"",sizeof(Vec3)*sub.face_count*3,true);
			Z_ASSERT(vbo);
			if(!vbo->dirty())
				return;
			Mesh::update_vbo_vert(vbo,sub.face_count,&array_vert[0],&_mesh->data.face[sub.face_start]);
			vbo->setDirty(false);
			sub.vbo_vert=vbo;
		}
		void					MeshInst::prepare_vbo_norm(int submesh)
		{
			Z_ASSERT(submesh>=0 && submesh<(int)array_submesh.size());
			SubMeshInst& sub=array_submesh[submesh];

			// use original
			if(!array_norm.size())
			{
				if(!sub.vbo_norm || !sub.vbo_norm->isNamed() || sub.vbo_norm->dirty())
					sub.vbo_norm=_mesh->vbo_norm(submesh);
				return;
			}
			// instanced vertex attributes
			if(sub.vbo_norm && sub.vbo_norm->isNamed())
				sub.vbo_norm.reset();						// reset it if still using original

			Z_ASSERT(sub.face_count);

			SPtr<VBO> vbo=sub.vbo_norm;
			if(!vbo || vbo->length() < (int)(sizeof(Vec3)*sub.face_count*3))
				vbo=VBO::create(L"",sizeof(Vec3)*sub.face_count*3,true);
			Z_ASSERT(vbo);
			if(!vbo->dirty())
				return;
			Mesh::update_vbo_norm(vbo,sub.face_count,&array_norm[0],&_mesh->data.face[sub.face_start]);
			vbo->setDirty(false);
			sub.vbo_norm=vbo;
		}
		void					MeshInst::prepare_vbo_diffuse(int submesh)
		{
			Z_ASSERT(submesh>=0 && submesh<(int)array_submesh.size());
			SubMeshInst& sub=array_submesh[submesh];

			// use original
			if(!array_mvert.size())
			{
				if(!sub.vbo_diffuse || !sub.vbo_diffuse->isNamed() || sub.vbo_diffuse->dirty())
					sub.vbo_diffuse=_mesh->vbo_diffuse(submesh);
				return;
			}
			// instanced vertex attributes
			if(sub.vbo_diffuse && sub.vbo_diffuse->isNamed())
				sub.vbo_diffuse.reset();						// reset it if still using original
			if(sub.diffuse_face_start == -1)
				return;

			Z_ASSERT(sub.face_count);

			SPtr<VBO> vbo=sub.vbo_diffuse;
			if(!vbo || vbo->length() < (int)(sizeof(Vec2)*sub.face_count*3))
				vbo=VBO::create(L"",sizeof(Vec2)*sub.face_count*3,true);
			Z_ASSERT(vbo);
			if(!vbo->dirty())
				return;
			Mesh::update_vbo_mvert(vbo,sub.face_count,&array_mvert[0],&_mesh->data.mface[sub.diffuse_face_start]);
			vbo->setDirty(false);
			sub.vbo_diffuse=vbo;
		}
		void						MeshInst::prepare_vbo_normal(int submesh)
		{
			Z_ASSERT(submesh>=0 && submesh<(int)array_submesh.size());
			SubMeshInst& sub=array_submesh[submesh];

			// use original
			if(!array_mvert.size())
			{
				if(!sub.vbo_normal || !sub.vbo_normal->isNamed() || sub.vbo_normal->dirty())
					sub.vbo_normal=_mesh->vbo_normal(submesh);
				return;
			}
			// instanced vertex attributes
			if(sub.vbo_normal && sub.vbo_normal->isNamed())
				sub.vbo_normal.reset();						// reset it if still using original
			if(sub.normal_face_start == -1)
				return;

			Z_ASSERT(sub.face_count);

			SPtr<VBO> vbo=sub.vbo_normal;
			if(!vbo || vbo->length() < (int)(sizeof(Vec2)*sub.face_count*3))
				vbo=VBO::create(L"",sizeof(Vec2)*sub.face_count*3,true);
			Z_ASSERT(vbo);
			if(!vbo->dirty())
				return;
			Mesh::update_vbo_mvert(vbo,sub.face_count,&array_mvert[0],&_mesh->data.mface[sub.normal_face_start]);
			vbo->setDirty(false);
			sub.vbo_normal=vbo;
		}

		void						MeshInst::prepare_vbo_selfill(int submesh)
		{
			Z_ASSERT(submesh>=0 && submesh<(int)array_submesh.size());
			SubMeshInst& sub=array_submesh[submesh];

			// use original
			if(!array_mvert.size())
			{
				if(!sub.vbo_selfill || !sub.vbo_selfill->isNamed() || sub.vbo_selfill->dirty())
					sub.vbo_selfill=_mesh->vbo_selfill(submesh);
				return;
			}
			// instanced vertex attributes
			if(sub.vbo_selfill && sub.vbo_selfill->isNamed())
				sub.vbo_selfill.reset();						// reset it if still using original
			if(sub.selfill_face_start == -1)
				return;

			Z_ASSERT(sub.face_count);

			SPtr<VBO> vbo=sub.vbo_selfill;
			if(!vbo || vbo->length() < (int)(sizeof(Vec2)*sub.face_count*3))
				vbo=VBO::create(L"",sizeof(Vec2)*sub.face_count*3,true);
			Z_ASSERT(vbo);
			if(!vbo->dirty())
				return;
			Mesh::update_vbo_mvert(vbo,sub.face_count,&array_mvert[0],&_mesh->data.mface[sub.selfill_face_start]);
			vbo->setDirty(false);
			sub.vbo_selfill=vbo;
		}
		void						MeshInst::loadTextures()
		{
			for(int i=0;i<(int)array_submesh.size();++i)
			{
				SubMeshInst& sub=array_submesh[i];
				Mtl& mtl=array_mtl[i];

				if(!mtl.map_diffuse.tex || mtl.map_diffuse.tex->filepath()!= mtl.map_diffuse.filepath)
					mtl.map_diffuse.tex = Tex2D::load(mtl.map_diffuse.filepath,true,GfxEnum::PIXELFORMAT_A8R8G8B8,false);

				if(!mtl.map_normal.tex || mtl.map_normal.tex->filepath()!= mtl.map_normal.filepath)
					mtl.map_normal.tex = Tex2D::load(mtl.map_normal.filepath,true,GfxEnum::PIXELFORMAT_A8R8G8B8,false);

				if(!mtl.map_selfill.tex || mtl.map_selfill.tex->filepath()!= mtl.map_selfill.filepath)
					mtl.map_selfill.tex = Tex2D::load(mtl.map_selfill.filepath,true,GfxEnum::PIXELFORMAT_A8R8G8B8,false);
			}
		}

		const AABB&				MeshInst::local_bound()
		{
			if(!_dirty_bound)
				return _bound;
			AABB b;
			vector<Vec3>* p;
			if(array_vert.size())
				p=&array_vert;
			else
				p=&_mesh->data.vert;
			const Vec3* end=&*p->begin()+p->size();
			for(const Vec3* iter=&*p->begin();iter!=end;++iter)
				b.fit(*iter);
			_bound=b;
			_dirty_bound=0;
			return _bound;
		}
		void					MeshInst::clearFacePlane()
		{
			_face_plane.clear();
		}
		void					MeshInst::buildFacePlane()
		{
			if(_face_plane.size())
				return;
			//
		}
		const vector<Plane>&	MeshInst::getFacePlane() const
		{
			return _face_plane;
		}
		void					MeshInst::computeFaceVisibility(const Vec3& pov)
		{
			if(pov==_cached_face_visibility_pov && _cached_face_visibility.size())
				return;
			//
		}
		void					MeshInst::clearFaceVisibility()
		{
			_cached_face_visibility.clear();
		}
		const vector<char>&		MeshInst::getFaceVisibility() const
		{
			return _cached_face_visibility;
		}
		void					MeshInst::endEdit(bool edit_vert,bool edit_norm, bool edit_mvert)
		{
			if(edit_vert)
			{
				_dirty_bound=1;

				_face_plane.clear();
				_cached_face_visibility.clear();
			}

			for(int i=0;i<(int)array_submesh.size();++i)
			{
				SubMeshInst& sub=array_submesh[i];
				if(edit_vert)
					if(sub.vbo_vert)
						if(sub.vbo_vert->isNamed())
							sub.vbo_vert.reset();
						else
							sub.vbo_vert->setDirty(true);
				if(edit_norm)
					if(sub.vbo_norm)
						if(sub.vbo_norm->isNamed())
							sub.vbo_norm.reset();
						else
							sub.vbo_norm->setDirty(true);
				if(edit_mvert)
				{
					if(sub.vbo_diffuse)
						if(sub.vbo_diffuse->isNamed())
							sub.vbo_diffuse.reset();
						else
							sub.vbo_diffuse->setDirty(true);
					if(sub.vbo_normal)
						if(sub.vbo_normal->isNamed())
							sub.vbo_normal.reset();
						else
							sub.vbo_normal->setDirty(true);
					if(sub.vbo_selfill)
						if(sub.vbo_selfill->isNamed())
							sub.vbo_selfill.reset();
						else
							sub.vbo_selfill->setDirty(true);
				}
			}
		}
		const SPtr<Mesh>&		MeshInst::mesh() const
		{
			return _mesh;
		}
	};
};

#endif