
#include "SCameraNode.h"
#include "CameraCtrl.h"

namespace z3D
{
	namespace SG
	{
		SCameraNode::SCameraNode() : _frustum(Mat4::perspectiveFOV(90.0f, 1.0f, -1.0f, -100.0f, 1, -1, 1), 1.0f, -1.0f)
		{
			_mode = 0;

			_znear = 0.1f;
			_zfar = 5000.0f;

			_yfov = 90.0f;
			_aspect_ratio = 1.0f;

			_left = -128;
			_top = 128;
			_right = 128;
			_bottom = -128;

			_target_dist = 128.0f;

			_ortho_size_explicit = 0;

			markDirtyCamera();
		}
		SCameraNode::~SCameraNode() {}
		const Mat4&			SCameraNode::view() {return inv_world_trans();}
		const Mat4&			SCameraNode::proj()
		{
			updateCamera();
			return _proj;
		}
		const Mat4&			SCameraNode::view_proj()
		{
			updateCamera();
			return _viewproj;
		}
		const Frustum&		SCameraNode::frustum()
		{
			updateCamera();
			return _frustum;
		}
		void					SCameraNode::markDirtyCamera()
		{
			_dirty_camera = true;
		}
		void					SCameraNode::updateCamera()
		{
			Gfx* gfx = Gfx::current();
			Z_ASSERT( gfx );
			if(_mode == 0)
			{
				if(!_dirty_camera)
					return;
				_proj = Mat4::perspectiveFOV(_yfov, _aspect_ratio, -_znear, -_zfar, gfx->getZNearMapping(), gfx->getZFarMapping(), gfx->getWMultiplier());
			}
			else
			{
				if(!_dirty_camera)
					return;
				if(!isOrthoSizeExplicit())
				{
					_right = TAN(xfov() * 0.5f) * getTargetDist();
					_top = TAN(yfov() * 0.5f) * getTargetDist();

					_left = -_right;
					_bottom = -_top;
				}

				_proj = Mat4::ortho(_left, _bottom, _right, _top, -_znear, -_zfar, gfx->getZNearMapping(), gfx->getZFarMapping());
			}
			_viewproj = inv_world_trans() * _proj;
			_frustum = Frustum(_viewproj, gfx->getZNearMapping(), gfx->getZFarMapping(), _array_add_clips);
			_dirty_camera = false;
		}
		void				SCameraNode::setPerspective(REAL yfov, REAL znear, REAL zfar)
		{
			_mode = 0;

			_yfov = yfov;

			_znear = znear;
			_zfar = zfar;

			markDirtyCamera();
		}
		void				SCameraNode::setOrtho(REAL left, REAL right, REAL top, REAL bottom, REAL znear, REAL zfar)
		{
			_mode = 1;

			_left = left;
			_right = right;
			_top = top;
			_bottom = bottom;

			_znear = znear;
			_zfar = zfar;

			_ortho_size_explicit = 1;

			markDirtyCamera();
		}
		void				SCameraNode::setOrtho(REAL yfov, REAL znear, REAL zfar)
		{
			_mode = 1;

			_yfov = yfov;

			_znear = znear;
			_zfar = zfar;

			_ortho_size_explicit = 0;

			markDirtyCamera();
		}
		void				SCameraNode::setAspectRatio(REAL aspect_ratio)
		{
			_aspect_ratio = aspect_ratio;

			markDirtyCamera();
		}
		REAL				SCameraNode::yfov() const {return _yfov;}
		REAL				SCameraNode::xfov() const {return ATAN(aspect_ratio() * TAN(_yfov / 2)) * 2;}
		REAL				SCameraNode::aspect_ratio() const {return _aspect_ratio;}
		REAL				SCameraNode::znear() const {return _znear;}
		REAL				SCameraNode::zfar() const {return _zfar;}
		REAL				SCameraNode::left() const {return _left;}
		REAL				SCameraNode::top() const {return _top;}
		REAL				SCameraNode::right() const {return _right;}
		REAL				SCameraNode::bottom() const {return _bottom;}
		bool				SCameraNode::isPerspective() const {return _mode == 0;}
		bool				SCameraNode::isOrtho() const {return _mode == 1;}
		bool				SCameraNode::isOrthoSizeExplicit() const {return _ortho_size_explicit;}
		void				SCameraNode::pan(const Vec3& delta)
		{
			setWorldPos(world_pos() + delta * world_rot().asMat3());
		}
		void				SCameraNode::rotateYaw(REAL delta)
		{
			REAL yaw, pitch;
			(Vec3::zNegAxis * world_rot().asMat3()).resolveOriRH(yaw, pitch);
			setWorldRot(Quat(Mat4::rotation(Vec3::xAxis, pitch) * Mat4::rotation(Vec3::yAxis, yaw + delta)));
		}
		void				SCameraNode::rotatePitch(REAL delta)
		{
			REAL yaw, pitch;
			(Vec3::zNegAxis * world_rot().asMat3()).resolveOriRH(yaw,pitch);
			setWorldRot(Quat(Mat4::rotation(Vec3::xAxis, min(max(pitch + delta, -89.9f), 89.9f)) * Mat4::rotation(Vec3::yAxis, yaw)));
		}
		void				SCameraNode::setYaw(REAL yaw)
		{
			setWorldRot(Quat(Mat4::rotation(Vec3::xAxis, pitch()) * Mat4::rotation(Vec3::yAxis, yaw)));
		}
		void				SCameraNode::setPitch(REAL pitch)
		{
			setWorldRot(Quat(Mat4::rotation(Vec3::xAxis, min(max(pitch, -89.9f), 89.9f)) * Mat4::rotation(Vec3::yAxis, yaw())));
		}
		void				SCameraNode::lookAt(const Vec3& at,bool world_space)
		{
			if(world_space)
			{
				setWorldRot(Quat(((Mat3)Mat4::lookingAt(world_pos(), at)).inverse()));
				_target_dist = (at - world_pos()).length();
			}
			else
			{
				setRot(Quat((Mat3)Mat4::lookingAt(pos(), at).inverse()));
				_target_dist = (at * world_trans() - world_pos()).length();
			}
		}
		void				SCameraNode::lookAt2(const Vec3& at,bool world_space)
		{
			Vec3 cam_pos = world_space ? world_pos() : pos();
			REAL yaw, pitch;
			(at - cam_pos).resolveOriRH(yaw,pitch);
			if(world_space)
			{
				setWorldRot(Quat(Mat3::rotation(Vec3::xAxis, pitch) * Mat3::rotation(Vec3::yAxis, yaw)));
				_target_dist = (at - world_pos()).length();
			}
			else
			{
				setRot(Quat(Mat3::rotation(Vec3::xAxis, pitch) * Mat3::rotation(Vec3::yAxis, yaw)));
				_target_dist = (at * world_trans() - world_pos()).length();
			}
		}
		void				SCameraNode::setTargetDist(REAL dist)
		{
			_target_dist = max(0.0f, dist);

			if(isOrtho() && isOrthoSizeExplicit())
				markDirtyCamera();
		}
		REAL				SCameraNode::getTargetDist() const
		{
			return _target_dist;
		}
		void				SCameraNode::zoom(REAL disp)
		{
			REAL new_target_dist = max(0.0f, _target_dist - disp);
			REAL eff_disp = new_target_dist - _target_dist;
			pan(Vec3(0, 0, eff_disp));
			_target_dist = new_target_dist;
		}
		void				SCameraNode::zoomTo(REAL dist)
		{
			zoom(getTargetDist() - dist);
		}
		void				SCameraNode::arcRotate(REAL delta_yaw, REAL delta_pitch)
		{
			arcRotateTo(yaw() + delta_yaw, pitch() + delta_pitch);
		}
		void				SCameraNode::arcRotateTo(REAL yaw, REAL pitch)
		{
			pitch = CLAMP(pitch, -90.0f, 90.0f);
			Vec3 around = world_pos() + (-*(Vec3*)world_rot().asMat3()[2]) * _target_dist;
			Mat3 rot = Mat3::rotationX(pitch) * Mat3::rotationY(yaw);
			setWorldRot(rot);
			setWorldPos(around + (*(Vec3*)rot[2]) * _target_dist);
		}
		void				SCameraNode::frame(const AABB& box)
		{
			if(!isOrtho())
			{
				REAL radius = (box.maximum() - box.minimum()).length() * 0.5f;

				REAL dist = radius / TAN(yfov() * 0.5f);
				dist = max(dist, radius / TAN(xfov() * 0.5f));

				lookAt(box.center());
				zoomTo(dist);
			}
			else
			{
			}
		}
		REAL				SCameraNode::yaw()
		{
			//REAL yaw,pitch;
			//(Vec3::zNegAxis*world_rot().asMat3()).resolveOriRH(yaw,pitch);
			//return yaw;
			REAL yaw, pitch;
			world_rot().asMat3().resolveOriRH(yaw, pitch);
			return yaw;
		}
		REAL				SCameraNode::pitch()
		{
			//REAL yaw,pitch;
			//(Vec3::zNegAxis*world_rot().asMat3()).resolveOriRH(yaw,pitch);
			//return pitch;
			REAL yaw, pitch;
			world_rot().asMat3().resolveOriRH(yaw, pitch);
			return pitch;
		}
		void				SCameraNode::markDirtyTrans()
		{
			SNode::markDirtyTrans();
			markDirtyCamera();
		}
		void				SCameraNode::copyCameraSettings(const SPtr<SCameraNode>& camera)
		{
			_mode = camera->_mode;

			_znear = camera->_znear;
			_zfar = camera->_zfar;

			_yfov = camera->_yfov;

			_left = camera->_left;
			_top = camera->_top;
			_right = camera->_right;
			_bottom = camera->_bottom;

			_ortho_size_explicit = camera->_ortho_size_explicit;

			_aspect_ratio = camera->_aspect_ratio;

			markDirtyCamera();
		}
		vector<Plane>&		SCameraNode::additionalClipPlanes()
		{
			return _array_add_clips;
		}
		const vector<Plane>&	SCameraNode::additionalClipPlanes() const
		{
			return _array_add_clips;
		}

		// x and y must lie between -1.0 and 1.0
		const Ray			SCameraNode::castRay(/* x and y must lie between -1.0 and 1.0*/ const Vec2& p)
		{
			Gfx* gfx = Gfx::current();
			Z_ASSERT( gfx );

			// work both for perspective and ortho, not accounting for oblique near clip plane

			Mat4 inv = proj().inverse();
			Vec3 p0 = Vec3(p.x, p.y, gfx->getZNearMapping()) * inv;
			Vec3 p1 = Vec3(p.x, p.y, (gfx->getZNearMapping() + gfx->getZFarMapping()) * 0.5f) * inv;
			Vec3 d = p1 - p0;
			Vec3 o = p0 - d / d.z * this->_znear;
			Vec3 p3 = o + d;
			Vec3 trans_o = o * world_trans();
			return Ray(trans_o, (p3 * world_trans() - trans_o).normal_safe());

			//Vec3 origin=world_pos();
			//Vec3 dst(p.x,p.y,_znear);
			//dst=dst*view_proj().inverse();
			//return Ray(origin,(dst-origin).normal());
		}

		/*
		// x and y are window coordinates, upper-left is (0,0)
		const Ray			SCameraNode::castCameraMouseRay(int x,int y)
		{
			return castRay(Vec2((x-_vp_x-_vp_dx*0.5f)/_vp_dx*2,-(y-_vp_y-_vp_dy*0.5f)/_vp_dy*2));
		}*/
		SPtr< AnimData<REAL> >&	SCameraNode::animdata_yfov()
		{
			return _animdata_yfov;
		}
		SPtr< AnimData<REAL> >&	SCameraNode::animdata_znear()
		{
			return _animdata_znear;
		}
		SPtr< AnimData<REAL> >&	SCameraNode::animdata_zfar()
		{
			return _animdata_zfar;
		}
		SPtr< AnimData<REAL> >&	SCameraNode::animdata_target_dist()
		{
			return _animdata_target_dist;
		}
		void				SCameraNode::buildControllers(const wcs& node_name)
		{
			if(!_animdata_yfov && !_animdata_znear && !_animdata_zfar && !_animdata_target_dist)
				return;

			SPtr<CameraCtrl> c = new CameraCtrl(wcs(L"CameraCtrl: ") + node_name, this);
			c->setAnimDataYFov(_animdata_yfov);
			c->setAnimDataZNear(_animdata_znear);
			c->setAnimDataZFar(_animdata_zfar);
			c->setAnimDataTargetDist(_animdata_target_dist);

			if(true)
			{
				REAL start, end;
				c->getAnimDataTimeRange(start, end);
				c->getAnimCtrl()->set(TimeAnimCfg(start, end, -1));
			}

			getCtrlSet().attachCtrl(c);
		}
	};
};

