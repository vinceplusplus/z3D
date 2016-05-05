
#include "Gizmos.h"

#include "SStructNode.h"
#include "SMeshNode.h"
#include "Mesh.h"
#include "SVisitor.h"
#include "SMgr.h"
#include "RenderInfo.h"

#include "../Core/functor_inl.h"

namespace z3D
{
	namespace SG
	{
		TranslateGizmo::TranslateGizmo()
		{
			reset();
		}

		void						TranslateGizmo::reset()
		{
			_operating = false;
			_mode = 0;
			_vp = Mat4::identity;
		}

		void						TranslateGizmo::load()
		{
			_gizmo = SStructNode::load(L"meshes/utils/translate_gizmo/scene.xml");
		}
		void						TranslateGizmo::unload()
		{
			_gizmo.reset();
		}

		void						TranslateGizmo::reset_materials()
		{
			const SStructNode::MAP_NAME_LOADED_NODE& m = _gizmo->getLoadedNodes();
			for(SStructNode::MAP_NAME_LOADED_NODE::const_iterator iter = m.begin(); iter != m.end(); ++iter)
			{
				if(!iter->second->type_info()->exactly(SMeshNode::type_info_static()))
					continue;
				SPtr<Mesh> m = iter->second.cast<SMeshNode>()->mesh();
				for(size_t j = 0; j < m->data.mtls.get().size(); ++j)
					m->data.mtls.edit()[j].ambient = m->getRefMtl()->mtls[j].ambient;
			}
		}
		void						TranslateGizmo::adjust_gizmo_display()
		{
			Mat4 inv = _vp.inverse();
			REAL s = 0;
			s = max(s, ((_new_pos * _vp + Vec3(1.0f / 4, 0, 0)) * inv - _new_pos).length());
			s = max(s, ((_new_pos * _vp + Vec3(-1.0f / 4, 0, 0)) * inv - _new_pos).length());
			s = max(s, ((_new_pos * _vp + Vec3(0, 1.0f / 4, 0)) * inv - _new_pos).length());
			s = max(s, ((_new_pos * _vp + Vec3(0, -1.0f / 4, 0)) * inv - _new_pos).length());

			s = max(0.01f, s);

			_gizmo->setWorldScale(Vec3(s, s, s));
		}

		void						TranslateGizmo::setViewProj(const Mat4& vp)
		{
			_vp = vp;

			adjust_gizmo_display();
		}

		void						TranslateGizmo::begin(const Vec3& pos, const Mat3& rot)
		{
			_org_pos = _new_pos = pos;
			_rot = rot;

			_gizmo->setWorldPos(_org_pos);
			_gizmo->setWorldRot(_rot);

			proc_cancel();
		}
		void						TranslateGizmo::commit()
		{
			_operating = false;

			_org_pos = _new_pos;

			reset_materials();

			adjust_gizmo_display();
		}

		void						TranslateGizmo::proc_click(const Ray& r)
		{
			proc_cancel();

			_org_pos = _new_pos;

			reset_materials();

			STriangleSelector sel(r);
			sel.traverse(_gizmo);
			if(sel.time != INFINITY)
			{
				if(sel.mn.cast<SNode>() == _gizmo->getLoadedNode(L"X"))
				{
					_operating = true;
					_mode = 0;
				}
				else if(sel.mn.cast<SNode>() == _gizmo->getLoadedNode(L"Y"))
				{
					_operating = true;
					_mode = 1;
				}
				else if(sel.mn.cast<SNode>() == _gizmo->getLoadedNode(L"Z"))
				{
					_operating = true;
					_mode = 2;
				}
				else if(sel.mn.cast<SNode>() == _gizmo->getLoadedNode(L"YZ"))
				{
					_operating = true;
					_mode = 3;
				}
				else if(sel.mn.cast<SNode>() == _gizmo->getLoadedNode(L"ZX"))
				{
					_operating = true;
					_mode = 4;
				}
				else if(sel.mn.cast<SNode>() == _gizmo->getLoadedNode(L"XY"))
				{
					_operating = true;
					_mode = 5;
				}

				_first_hit_pos = r.origin() + sel.time * r.direction();

				_translate_offset = _org_pos - _first_hit_pos;

				SPtr<Mesh> m = sel.mn->mesh();
				vector<Mtl>& mtls = m->data.mtls.edit();
				for(size_t i = 0; i < mtls.size(); ++i)
					mtls[i].ambient = Vec3(1, 1, 1);
			}
		}
		bool						TranslateGizmo::proc_move(const Ray& r)
		{
			if(!_operating)
			{
				proc_cancel();

				reset_materials();

				STriangleSelector sel(r);
				sel.traverse(_gizmo);
				if(sel.time != INFINITY)
				{
					SPtr<Mesh> m = sel.mn->mesh();
					vector<Mtl>& mtls = m->data.mtls.edit();
					for(size_t i = 0; i < mtls.size(); ++i)
						mtls[i].ambient = Vec3(1, 1, 1);
				}

				return false;
			}

			REAL t, t2;
			t = t2 = INFINITY;
			switch(_mode)
			{
			case 0:
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[1])), t);
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[2])), t2);
					SORT(t, t2);
				break;
			case 1:
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[2])), t);
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[0])), t2);
					SORT(t, t2);
				break;
			case 2:
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[0])), t);
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[1])), t2);
					SORT(t, t2);
				break;
			case 3:
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[0])), t);
				break;
			case 4:
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[1])), t);
				break;
			case 5:
					Intr::RayPlane(r, Plane(_first_hit_pos, Vec3(_rot[2])), t);
				break;
			}
			if(t != INFINITY)
			{
				Vec3 p = _last_hit_pos = r.origin() + t * r.direction();
				Vec3 new_pos = _first_hit_pos;

				switch(_mode)
				{
				case 0:
					new_pos = new_pos - (new_pos * Vec3(_rot[0])) * Vec3(_rot[0]) + (p * Vec3(_rot[0])) * Vec3(_rot[0]);
					break;
				case 1:
					new_pos = new_pos - (new_pos * Vec3(_rot[1])) * Vec3(_rot[1]) + (p * Vec3(_rot[1])) * Vec3(_rot[1]);
					break;
				case 2:
					new_pos = new_pos - (new_pos * Vec3(_rot[2])) * Vec3(_rot[2]) + (p * Vec3(_rot[2])) * Vec3(_rot[2]);
					break;
				case 3:
				case 4:
				case 5:
					new_pos = p;
					break;
				}

				_new_pos = new_pos + _translate_offset;

				_gizmo->setWorldPos(_new_pos);

				return true;
			}
			return false;
		}
		void						TranslateGizmo::proc_cancel()
		{
			_operating = false;

			_new_pos = _org_pos;
			_gizmo->setWorldPos(_org_pos);

			reset_materials();

			adjust_gizmo_display();
		}

		const SPtr<SStructNode>&	TranslateGizmo::getGizmo() const
		{
			return _gizmo;
		}

		const Mat3&					TranslateGizmo::getRot() const
		{
			return _rot;
		}
		const Vec3&					TranslateGizmo::getOrgPos() const
		{
			return _org_pos;
		}
		const Vec3&					TranslateGizmo::getNewPos() const
		{
			return _new_pos;
		}
		const Vec3&					TranslateGizmo::getFirstHitPos() const
		{
			return _first_hit_pos;
		}
		const Vec3&					TranslateGizmo::getLastHitPos() const
		{
			return _last_hit_pos;
		}
		bool						TranslateGizmo::isOperating() const
		{
			return !!_operating;
		}
		void						TranslateGizmo::draw(const SPtr<SMgr>& smgr)
		{
			SRenderInfo* ri = SRenderInfo::alloc();

			ri->setCamera(smgr->getActiveCamera());

			smgr->draw(getGizmo(), DRAWPASS_DEFAULT, ri);

			ri->dealloc();
		}



		RotateGizmo::RotateGizmo()
		{
			reset();
		}

		void						RotateGizmo::reset()
		{
			_operating = false;
			_mode = 0;
			_view = Mat4::identity;
			_vp = Mat4::identity;
			_cam_rot = Mat4::identity;
		}

		void						RotateGizmo::load()
		{
			_gizmo = SStructNode::load(L"meshes/utils/rotate_gizmo/scene.xml");
		}
		void						RotateGizmo::unload()
		{
			_gizmo.reset();
		}

		void						RotateGizmo::reset_materials()
		{
			const SStructNode::MAP_NAME_LOADED_NODE& m = _gizmo->getLoadedNodes();
			for(SStructNode::MAP_NAME_LOADED_NODE::const_iterator iter = m.begin(); iter != m.end(); ++iter)
			{
				if(!iter->second->type_info()->exactly(SMeshNode::type_info_static()))
					continue;
				SPtr<Mesh> m = iter->second.cast<SMeshNode>()->mesh();
				for(size_t j = 0; j < m->data.mtls.get().size(); ++j)
					m->data.mtls.edit()[j].ambient = m->getRefMtl()->mtls[j].ambient;
			}
		}
		void						RotateGizmo::adjust_gizmo_display()
		{
			Mat4 inv = _vp.inverse();
			REAL s = 0;
			s = max(s, ((_pos * _vp + Vec3(1.0f / 8, 0, 0)) * inv - _pos).length());
			s = max(s, ((_pos * _vp + Vec3(-1.0f / 8, 0, 0)) * inv - _pos).length());
			s = max(s, ((_pos * _vp + Vec3(0, 1.0f / 8, 0)) * inv - _pos).length());
			s = max(s, ((_pos * _vp + Vec3(0, -1.0f / 8, 0)) * inv - _pos).length());

			s = max(0.01f, s);

			_gizmo->setWorldScale(Vec3(s, s, s));

			_gizmo->getLoadedNode(L"Outer")->setWorldRot(_cam_rot);
		}

		void						RotateGizmo::setViewProj(const Mat4& view, const Mat4& vp)
		{
			_view = view;
			_vp = vp;

			_cam_rot = ((Mat3)_view).inverse();

			adjust_gizmo_display();
		}

		void						RotateGizmo::begin(const Vec3& pos, const Mat3& rotation)
		{
			_org_rot = _new_rot = rotation;
			_pos = pos;

			_gizmo->setWorldPos(_pos);
			_gizmo->setWorldRot(_new_rot);

			proc_cancel();
		}
		void						RotateGizmo::commit()
		{
			_operating = false;

			_org_rot = _new_rot;

			reset_materials();

			adjust_gizmo_display();
		}
		bool						RotateGizmo::hit_test(const Ray& r, size_t& handle, Vec3& hit_pos)
		{
			STriangleSelector sel(r);
			sel.traverse(_gizmo);
			if(sel.time == INFINITY)
				return false;

			hit_pos = r.origin() + sel.time * r.direction();

			if(sel.mn == _gizmo->getLoadedNode(L"X").cast<SMeshNode>())
				handle = 0;
			else if(sel.mn == _gizmo->getLoadedNode(L"Y").cast<SMeshNode>())
				handle = 1;
			else if(sel.mn == _gizmo->getLoadedNode(L"Z").cast<SMeshNode>())
				handle = 2;
			else if(sel.mn == _gizmo->getLoadedNode(L"Trackball").cast<SMeshNode>())
				handle = 3;
			else if(sel.mn == _gizmo->getLoadedNode(L"Outer").cast<SMeshNode>())
				handle = 4;
			else
				Z_ASSERT( false );

			return true;
		}
		void						RotateGizmo::hilite(size_t handle)
		{
			SPtr<Mesh> m;
			switch(handle)
			{
			case 0:
				m = _gizmo->getLoadedNode(L"X").cast<SMeshNode>()->mesh();
				break;
			case 1:
				m = _gizmo->getLoadedNode(L"Y").cast<SMeshNode>()->mesh();
				break;
			case 2:
				m = _gizmo->getLoadedNode(L"Z").cast<SMeshNode>()->mesh();
				break;
			case 3:
				m = _gizmo->getLoadedNode(L"Trackball").cast<SMeshNode>()->mesh();
				break;
			case 4:
				m = _gizmo->getLoadedNode(L"Outer").cast<SMeshNode>()->mesh();
				break;
			}
			vector<Mtl>& mtls = m->data.mtls.edit();
			for(size_t i = 0; i < mtls.size(); ++i)
				mtls[i].ambient = Vec3(1, 1, 1);
		}

		void						RotateGizmo::proc_click(const Ray& r)
		{
			proc_cancel();

			if(true)
			{
				size_t handle;
				Vec3 hit_pos;
				if(!hit_test(r, handle, hit_pos))
					return;

				hilite(handle);

				_operating = true;
				_mode = handle;

				_screen_hit_pos = hit_pos * _vp;
				_screen_hit_pos.z = 0;

				if(_mode < 3)
				{
					Vec3 tangent = (Vec3(_org_rot[_mode]) ^ (hit_pos - _pos)).normal_safe();
					_screen_rot_dir = (hit_pos + tangent) * _vp - _screen_hit_pos;
					_screen_rot_dir.z = 0;
					_screen_rot_dir.normalize_safe();
				}
				else if(_mode == 4)
				{
					Vec3 tangent = (Vec3(_org_rot[_mode]) ^ (hit_pos - _pos)).normal_safe();
					_screen_rot_dir = (hit_pos + tangent) * _vp - _screen_hit_pos;
					_screen_rot_dir.z = 0;
					_screen_rot_dir.normalize_safe();
				}
			}
		}
		bool						RotateGizmo::proc_move(const Ray& r)
		{
			if(!_operating)
			{
				proc_cancel();

				reset_materials();

				size_t handle;
				Vec3 hit_pos;
				if(hit_test(r, handle, hit_pos))
					hilite(handle);

				return false;
			}

			Vec3 new_screen_pos = (r.origin() + r.direction()) * _vp;
			new_screen_pos.z = 0;

			REAL len = (new_screen_pos - _screen_hit_pos) * _screen_rot_dir;

			if(_mode < 3)
				_new_rot = _org_rot * Mat3::rotation(Vec3(_org_rot[_mode]), len * 360.0f);
			else if(_mode == 3)
			{
				Vec3 p0 = _screen_hit_pos;
				p0.z = 0.5f;
				Vec3 p1 = new_screen_pos;
				p1.z = 0.5f;
				Mat4 inv_vp = _vp.inverse();
				p0 *= inv_vp;
				p1 *= inv_vp;
				Vec3 dir = (p1 - p0).normal_safe();
				if(dir.length() != 0.0f)
				{
					len = (new_screen_pos - _screen_hit_pos).length();
					_new_rot = _org_rot * Mat3::rotation((Vec3(_cam_rot[2]) ^ dir).normal_safe(), len * 360.0f);
				}
				else
					_new_rot = _org_rot;
			}
			else if(_mode == 4)
				_new_rot = _org_rot * Mat3::rotation(Vec3(_cam_rot[2]), len * 360.0f);

			_gizmo->setWorldRot(_new_rot);
			adjust_gizmo_display();

			return true;
		}
		void						RotateGizmo::proc_cancel()
		{
			_operating = false;

			_new_rot = _org_rot;
			_gizmo->setWorldRot(_org_rot);

			reset_materials();

			adjust_gizmo_display();
		}

		const SPtr<SStructNode>&	RotateGizmo::getGizmo() const
		{
			return _gizmo;
		}

		const Vec3&					RotateGizmo::getPos() const
		{
			return _pos;
		}
		const Mat3&					RotateGizmo::getOrgRot() const
		{
			return _org_rot;
		}
		const Mat3&					RotateGizmo::getNewRot() const
		{
			return _new_rot;
		}
		bool						RotateGizmo::isOperating() const
		{
			return !!_operating;
		}

		void						RotateGizmo::adjust_trackball_shade_state(const SRenderInfo* ri, rsShade& rs)
		{
			rs.enableDepthTest(GfxEnum::CMPFUNC_ALWAYS);
			rs.enableWriteDepth(true);
		}
		void						RotateGizmo::draw(const SPtr<SMgr>& smgr)
		{
			SRenderInfo* ri = SRenderInfo::alloc();

			ri->setCamera(smgr->getActiveCamera());

			ri->setPostSetupShadeStateAdjustCallback(functor2<void, const SRenderInfo*, rsShade&>(this, &RotateGizmo::adjust_trackball_shade_state));

			smgr->draw(getGizmo()->getLoadedNode(L"Trackball"), DRAWPASS_DEFAULT, ri);
			ri->setPostSetupShadeStateAdjustCallback(functor2<void, const SRenderInfo*, rsShade&>());
			smgr->draw(getGizmo()->getLoadedNode(L"Handles"), DRAWPASS_DEFAULT, ri);

			ri->dealloc();
		}


		ScaleGizmo::ScaleGizmo()
		{
			reset();
		}
		void						ScaleGizmo::reset()
		{
			_operating = false;
			_mode = 0;
			_display_scale = Vec3(1, 1, 1);

			_vp = Mat4::identity;
		}
		void						ScaleGizmo::load()
		{
			_gizmo = SStructNode::load(L"meshes/utils/scale_gizmo/scene.xml");
		}
		void						ScaleGizmo::unload()
		{
			_gizmo.reset();
		}
		void						ScaleGizmo::reset_materials()
		{
			const SStructNode::MAP_NAME_LOADED_NODE& m = _gizmo->getLoadedNodes();
			for(SStructNode::MAP_NAME_LOADED_NODE::const_iterator iter = m.begin(); iter != m.end(); ++iter)
			{
				if(!iter->second->type_info()->exactly(SMeshNode::type_info_static()))
					continue;
				SPtr<Mesh> m = iter->second.cast<SMeshNode>()->mesh();
				for(size_t j = 0; j < m->data.mtls.get().size(); ++j)
					m->data.mtls.edit()[j].ambient = m->getRefMtl()->mtls[j].ambient;
			}
		}
		void						ScaleGizmo::adjust_gizmo_display()
		{
			Mat4 inv = _vp.inverse();
			REAL s = 0;
			s = max(s, ((_pos * _vp + Vec3(1.0f / 4, 0, 0)) * inv - _pos).length());
			s = max(s, ((_pos * _vp + Vec3(-1.0f / 4, 0, 0)) * inv - _pos).length());
			s = max(s, ((_pos * _vp + Vec3(0, 1.0f / 4, 0)) * inv - _pos).length());
			s = max(s, ((_pos * _vp + Vec3(0, -1.0f / 4, 0)) * inv - _pos).length());

			s = max(0.01f, s);

			_gizmo->setWorldScale(Vec3(max(0.01f, s * _display_scale[0]), max(0.01f, s * _display_scale[1]), max(0.01f, s * _display_scale[2])));
		}
		void						ScaleGizmo::setViewProj(const Mat4& vp)
		{
			_vp = vp;

			adjust_gizmo_display();
		}
		void						ScaleGizmo::begin(const Vec3& pos, const Mat3& rotation, const Vec3& scale)
		{
			_rot = rotation;
			_pos = pos;

			_org_scale = _new_scale = scale;
			_display_scale = Vec3(1, 1, 1);

			_gizmo->setWorldPos(_pos);
			_gizmo->setWorldRot(_rot);

			proc_cancel();
		}
		void						ScaleGizmo::commit()
		{
			_operating = false;

			_org_scale = _new_scale;
			_display_scale = Vec3(1, 1, 1);

			reset_materials();

			adjust_gizmo_display();
		}
		bool						ScaleGizmo::hit_test(const Ray& r, size_t& handle, Vec3& hit_pos)
		{
			STriangleSelector sel(r);
			sel.traverse(_gizmo);
			if(sel.time == INFINITY)
				return false;

			hit_pos = r.origin() + sel.time * r.direction();

			if(sel.mn == _gizmo->getLoadedNode(L"X").cast<SMeshNode>())
				handle = 0;
			else if(sel.mn == _gizmo->getLoadedNode(L"Y").cast<SMeshNode>())
				handle = 1;
			else if(sel.mn == _gizmo->getLoadedNode(L"Z").cast<SMeshNode>())
				handle = 2;
			else if(sel.mn == _gizmo->getLoadedNode(L"XY").cast<SMeshNode>())
				handle = 3;
			else if(sel.mn == _gizmo->getLoadedNode(L"YZ").cast<SMeshNode>())
				handle = 4;
			else if(sel.mn == _gizmo->getLoadedNode(L"ZX").cast<SMeshNode>())
				handle = 5;
			else if(sel.mn == _gizmo->getLoadedNode(L"XYZ").cast<SMeshNode>())
				handle = 6;
			else
				Z_ASSERT( false );

			return true;
		}
		void						ScaleGizmo::hilite(const SPtr<Mesh>& m)
		{
			vector<Mtl>& mtls = m->data.mtls.edit();
			for(size_t i = 0; i < mtls.size(); ++i)
				mtls[i].ambient = Vec3(1, 1, 1);
		}
		void						ScaleGizmo::hilite(size_t handle)
		{
			SPtr<Mesh> m;
			switch(handle)
			{
			case 0:
				hilite(_gizmo->getLoadedNode(L"X").cast<SMeshNode>()->mesh());
				break;
			case 1:
				hilite(_gizmo->getLoadedNode(L"Y").cast<SMeshNode>()->mesh());
				break;
			case 2:
				hilite(_gizmo->getLoadedNode(L"Z").cast<SMeshNode>()->mesh());
				break;
			case 3:
				hilite(_gizmo->getLoadedNode(L"XY").cast<SMeshNode>()->mesh());
				hilite(_gizmo->getLoadedNode(L"X").cast<SMeshNode>()->mesh());
				hilite(_gizmo->getLoadedNode(L"Y").cast<SMeshNode>()->mesh());
				break;
			case 4:
				hilite(_gizmo->getLoadedNode(L"YZ").cast<SMeshNode>()->mesh());
				hilite(_gizmo->getLoadedNode(L"Y").cast<SMeshNode>()->mesh());
				hilite(_gizmo->getLoadedNode(L"Z").cast<SMeshNode>()->mesh());
				break;
			case 5:
				hilite(_gizmo->getLoadedNode(L"ZX").cast<SMeshNode>()->mesh());
				hilite(_gizmo->getLoadedNode(L"Z").cast<SMeshNode>()->mesh());
				hilite(_gizmo->getLoadedNode(L"X").cast<SMeshNode>()->mesh());
				break;
			case 6:
				hilite(_gizmo->getLoadedNode(L"XYZ").cast<SMeshNode>()->mesh());
				break;
			}
		}
		void						ScaleGizmo::proc_click(const Ray& r)
		{
			proc_cancel();

			if(true)
			{
				size_t handle;
				Vec3 hit_pos;
				if(!hit_test(r, handle, hit_pos))
					return;

				hilite(handle);

				_operating = true;
				_mode = handle;

				_screen_hit_pos = hit_pos * _vp;
				_screen_hit_pos.z = 0;

				_hit_pos = hit_pos;
			}
		}
		bool						ScaleGizmo::proc_move(const Ray& r)
		{
			if(!_operating)
			{
				proc_cancel();

				reset_materials();

				size_t handle;
				Vec3 hit_pos;
				if(hit_test(r, handle, hit_pos))
					hilite(handle);

				return false;
			}

			REAL t, t2;
			t = t2 = INFINITY;
			REAL dt = INFINITY;
			switch(_mode)
			{
			case 0:
				Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[1])), t);
				Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[2])), t2);
				SORT(t, t2);
				if(t != INFINITY)
					dt = (r.origin() + t * r.direction() - _hit_pos) * _rot[0];
				break;
			case 1:
				Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[2])), t);
				Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[0])), t2);
				SORT(t, t2);
				if(t != INFINITY)
					dt = (r.origin() + t * r.direction() - _hit_pos) * _rot[1];
				break;
			case 2:
				Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[0])), t);
				Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[1])), t2);
				SORT(t, t2);
				if(t != INFINITY)
					dt = (r.origin() + t * r.direction() - _hit_pos) * _rot[2];
				break;
			case 3:
				if(Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[2])), t))
					dt = (r.origin() + t * r.direction() - _hit_pos) * (Vec3(_rot[0]) + Vec3(_rot[1])).normal_safe();
				break;
			case 4:
				if(Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[0])), t))
					dt = (r.origin() + t * r.direction() - _hit_pos) * (Vec3(_rot[1]) + Vec3(_rot[2])).normal_safe();
				break;
			case 5:
				if(Intr::RayPlane(r, Plane(_hit_pos, Vec3(_rot[1])), t))
					dt = (r.origin() + t * r.direction() - _hit_pos) * (Vec3(_rot[2]) + Vec3(_rot[0])).normal_safe();
				break;
			case 6:
				if(Intr::RayPlane(r, Plane(_hit_pos, (Vec3(_rot[0]) + Vec3(_rot[1]) + Vec3(_rot[2]).normal_safe())), t))
				{
					Mat4 inv_vp = _vp.inverse();
					dt = (r.origin() + t * r.direction() - _hit_pos) * (Vec3(0, 1, 0) * inv_vp - Vec3(0, -1, 0) * inv_vp).normal_safe();
				}
				break;
			}

			if(dt == INFINITY)
				return false;

			if(true)
			{
				// ugly!!! copy from adjust_gizmo_display
				Mat4 inv = _vp.inverse();
				REAL s = 0;
				s = max(s, ((_pos * _vp + Vec3(1.0f / 4, 0, 0)) * inv - _pos).length());
				s = max(s, ((_pos * _vp + Vec3(-1.0f / 4, 0, 0)) * inv - _pos).length());
				s = max(s, ((_pos * _vp + Vec3(0, 1.0f / 4, 0)) * inv - _pos).length());
				s = max(s, ((_pos * _vp + Vec3(0, -1.0f / 4, 0)) * inv - _pos).length());

				s = max(0.01f, s);

				dt /= s;

				dt *= 32.0f;
			}

			REAL s = 0;
			if(dt < 0.0f)
				s = POW(0.99f, -dt);
			else
				s = POW(1.01f, dt);

			switch(_mode)
			{
			case 0:
				_new_scale = _org_scale;
				_new_scale.x *= s;
				_new_scale.x = max(0.01f, _new_scale.x);_new_scale.y = max(0.01f, _new_scale.y);_new_scale.z = max(0.01f, _new_scale.z);
				_display_scale = Vec3(s, 1, 1);
				break;
			case 1:
				_new_scale = _org_scale;
				_new_scale.y *= s;
				_display_scale = Vec3(1, s, 1);
				_new_scale.x = max(0.01f, _new_scale.x);_new_scale.y = max(0.01f, _new_scale.y);_new_scale.z = max(0.01f, _new_scale.z);
				break;
			case 2:
				_new_scale = _org_scale;
				_new_scale.z *= s;
				_display_scale = Vec3(1, 1, s);
				_new_scale.x = max(0.01f, _new_scale.x);_new_scale.y = max(0.01f, _new_scale.y);_new_scale.z = max(0.01f, _new_scale.z);
				break;
			case 3:
				_new_scale = _org_scale;
				_new_scale.x *= s;
				_new_scale.y *= s;
				_display_scale = Vec3(s, s, 1);
				_new_scale.x = max(0.01f, _new_scale.x);_new_scale.y = max(0.01f, _new_scale.y);_new_scale.z = max(0.01f, _new_scale.z);
				break;
			case 4:
				_new_scale = _org_scale;
				_new_scale.y *= s;
				_new_scale.z *= s;
				_display_scale = Vec3(1, s, s);
				_new_scale.x = max(0.01f, _new_scale.x);_new_scale.y = max(0.01f, _new_scale.y);_new_scale.z = max(0.01f, _new_scale.z);
				break;
			case 5:
				_new_scale = _org_scale;
				_new_scale.z *= s;
				_new_scale.x *= s;
				_display_scale = Vec3(s, 1, s);
				_new_scale.x = max(0.01f, _new_scale.x);_new_scale.y = max(0.01f, _new_scale.y);_new_scale.z = max(0.01f, _new_scale.z);
				break;
			case 6:
				_new_scale = _org_scale * s;
				_display_scale = Vec3(s, s, s);
				_new_scale.x = max(0.01f, _new_scale.x);_new_scale.y = max(0.01f, _new_scale.y);_new_scale.z = max(0.01f, _new_scale.z);
				break;
			}

			adjust_gizmo_display();

			return true;
		}
		void						ScaleGizmo::proc_cancel()
		{
			_operating = false;

			_new_scale = _org_scale;
			_display_scale = Vec3(1, 1, 1);

			reset_materials();

			adjust_gizmo_display();
		}
		const SPtr<SStructNode>&	ScaleGizmo::getGizmo() const
		{
			return _gizmo;
		}
		const Vec3&					ScaleGizmo::getPos() const
		{
			return _pos;
		}
		const Mat3&					ScaleGizmo::getRot() const
		{
			return _rot;
		}
		const Vec3&					ScaleGizmo::getOrgScale() const
		{
			return _org_scale;
		}
		const Vec3&					ScaleGizmo::getNewScale() const
		{
			return _new_scale;
		}
		bool						ScaleGizmo::isOperating() const
		{
			return !_operating;
		}
		void						ScaleGizmo::draw(const SPtr<SMgr>& smgr)
		{
			SRenderInfo* ri = SRenderInfo::alloc();

			ri->setCamera(smgr->getActiveCamera());

			smgr->draw(getGizmo(), DRAWPASS_DEFAULT, ri);

			ri->dealloc();
		}
	};
};


