
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		class SCameraNode:public SNode
		{
			DECL_TYPEINFO();
		private:
			size_t					_mode;
		private:
			REAL					_znear;					// positive
			REAL					_zfar;					// positive
		private:					// for perspective
			REAL					_yfov;
		private:					// for orthographic
			REAL					_left;
			REAL					_top;
			REAL					_right;
			REAL					_bottom;
		private:
			REAL					_aspect_ratio;
		private:
			REAL					_target_dist;			// world space
		private:
			union
			{
				struct
				{
					bool					_dirty_camera : 1;
					bool					_ortho_size_explicit : 1;
				};
				uint32_t				__UNNAMED01;
			};
		private:
			Mat4					_proj;
			Mat4					_viewproj;
			Frustum					_frustum;
		private:
			vector<Plane>			_array_add_clips;						// clip planes in world space, TODO: make not using dynamic allocation
		private:
			SPtr< AnimData<REAL> >	_animdata_yfov;
			SPtr< AnimData<REAL> >	_animdata_znear;
			SPtr< AnimData<REAL> >	_animdata_zfar;
			SPtr< AnimData<REAL> >	_animdata_target_dist;
		public:
			SCameraNode();
		protected:
			~SCameraNode();
		public:
			const Mat4&				view();
			const Mat4&				proj();
			const Mat4&				view_proj();
			const Frustum&			frustum();
		protected:
			virtual void			markDirtyCamera();
		protected:
			void					updateCamera();
		public:
			void					setPerspective(REAL yfov, REAL znear = 0.1f, REAL zfar = 5000.0f);					// near and far must be positive
			void					setOrtho(REAL left, REAL right, REAL top, REAL bottom, REAL znear, REAL zfar);		// near and far must be positive
			void					setOrtho(REAL yfov, REAL znear, REAL zfar);
		public:
			void					setAspectRatio(REAL aspect_ratio);
		public:
			REAL					yfov() const;
			REAL					xfov() const;
			REAL					aspect_ratio() const;
			REAL					znear() const;
			REAL					zfar() const;
			REAL					left() const;
			REAL					top() const;
			REAL					right() const;
			REAL					bottom() const;
		public:
			bool					isPerspective() const;
			bool					isOrtho() const;
			bool					isOrthoSizeExplicit() const;
		public:
			void					pan(const Vec3& delta);
			void					rotateYaw(REAL delta);
			void					rotatePitch(REAL delta);
			void					setYaw(REAL yaw);
			void					setPitch(REAL pitch);
			void					lookAt(const Vec3& at, bool world_space = true);
			void					lookAt2(const Vec3& at, bool world_space = true);
		public:
			void					setTargetDist(REAL dist);
			REAL					getTargetDist() const;
			void					zoom(REAL disp);
			void					zoomTo(REAL dist);
		public:
			void					arcRotate(REAL delta_yaw, REAL delta_pitch);
			void					arcRotateTo(REAL yaw, REAL pitch);
		public:
			void					frame(const AABB& box);
		public:
			REAL					yaw();
			REAL					pitch();
		protected:
			void					markDirtyTrans();
		public:
			void					copyCameraSettings(const SPtr<SCameraNode>& camera);
		public:
			vector<Plane>&			additionalClipPlanes();						// clip planes in world space
			const vector<Plane>&	additionalClipPlanes() const;
		public:
			const Ray				castRay(const Vec2& p);						// p.x and p.y must lie between -1.0 and 1.0
			//const Ray				castCameraMouseRay(int x,int y);			// x and y are window coordinates, upper-left is (0,0)
		public:
			SPtr< AnimData<REAL> >&	animdata_yfov();
			SPtr< AnimData<REAL> >&	animdata_znear();
			SPtr< AnimData<REAL> >&	animdata_zfar();
			SPtr< AnimData<REAL> >&	animdata_target_dist();
		public:
			void					buildControllers(const wcs& node_name);
		};
	};
};

