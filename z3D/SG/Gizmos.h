
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace SG
	{
		class TranslateGizmo
		{
		private:
			SPtr<SStructNode>			_gizmo;
			size_t						_operating;
			size_t						_mode;
		private:
			Mat3						_rot;
			Vec3						_org_pos;
			Vec3						_new_pos;
			Vec3						_first_hit_pos;
			Vec3						_last_hit_pos;
			Vec3						_translate_offset;
			Mat4						_vp;
		public:
			TranslateGizmo();
		public:
			void						reset();
		public:
			void						load();
			void						unload();
		private:
			void						reset_materials();
			void						adjust_gizmo_display();
		public:
			void						setViewProj(const Mat4& vp);
		public:
			void						begin(const Vec3& pos, const Mat3& rot);
			void						commit();
		public:
			void						proc_click(const Ray& r);
			bool						proc_move(const Ray& r);
			void						proc_cancel();
		public:
			const SPtr<SStructNode>&	getGizmo() const;
		public:
			const Mat3&					getRot() const;
			const Vec3&					getOrgPos() const;
			const Vec3&					getNewPos() const;
			const Vec3&					getFirstHitPos() const;
			const Vec3&					getLastHitPos() const;
			bool						isOperating() const;
		public:
			void						draw(const SPtr<SMgr>& smgr);							// assuming z-buffer is cleared
		};

		class RotateGizmo
		{
		private:
			SPtr<SStructNode>			_gizmo;
			size_t						_operating;
			size_t						_mode;
		private:
			Vec3						_pos;
			Mat3						_org_rot;
			Mat3						_new_rot;
			Mat4						_view;
			Mat4						_vp;
			Mat3						_cam_rot;
		private:
			Vec3						_screen_hit_pos;
			Vec3						_screen_rot_dir;
		public:
			RotateGizmo();
		public:
			void						reset();
		public:
			void						load();
			void						unload();
		private:
			void						reset_materials();
			void						adjust_gizmo_display();
		public:
			void						setViewProj(const Mat4& view, const Mat4& vp);
		public:
			void						begin(const Vec3& pos, const Mat3& rotation);
			void						commit();
		private:
			bool						hit_test(const Ray& r, size_t& handle, Vec3& hit_pos);
			void						hilite(size_t handle);
		public:
			void						proc_click(const Ray& r);
			bool						proc_move(const Ray& r);
			void						proc_cancel();
		public:
			const SPtr<SStructNode>&	getGizmo() const;
		public:
			const Vec3&					getPos() const;
			const Mat3&					getOrgRot() const;
			const Mat3&					getNewRot() const;
			bool						isOperating() const;
		private:
			void						adjust_trackball_shade_state(const SRenderInfo* ri, rsShade& rs);
		public:
			void						draw(const SPtr<SMgr>& smgr);							// assuming z-buffer is cleared
		};

		class ScaleGizmo
		{
		private:
			SPtr<SStructNode>			_gizmo;
			size_t						_operating;
			size_t						_mode;
		private:
			Vec3						_pos;
			Mat3						_rot;
			Vec3						_org_scale;
			Vec3						_new_scale;
			Vec3						_display_scale;
			Mat4						_vp;
		private:
			Vec3						_hit_pos;
			Vec3						_screen_hit_pos;
		public:
			ScaleGizmo();
		public:
			void						reset();
		public:
			void						load();
			void						unload();
		private:
			void						reset_materials();
			void						adjust_gizmo_display();
		public:
			void						setViewProj(const Mat4& vp);
		public:
			void						begin(const Vec3& pos, const Mat3& rotation, const Vec3& scale);
			void						commit();
		private:
			bool						hit_test(const Ray& r, size_t& handle, Vec3& hit_pos);
			void						hilite(const SPtr<Mesh>& m);
			void						hilite(size_t handle);
		public:
			void						proc_click(const Ray& r);
			bool						proc_move(const Ray& r);
			void						proc_cancel();
		public:
			const SPtr<SStructNode>&	getGizmo() const;
		public:
			const Vec3&					getPos() const;
			const Mat3&					getRot() const;
			const Vec3&					getOrgScale() const;
			const Vec3&					getNewScale() const;
			bool						isOperating() const;
		public:
			void						draw(const SPtr<SMgr>& smgr);							// assuming z-buffer is cleared
		};
	};
};


