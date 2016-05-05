
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiUpdateLink: public RefBase
		{
		private:
			WPtr<uiBase>				_ui;
		private:
			list<WPtr<uiUpdateLink>>*	_list;
			list<WPtr<uiUpdateLink>>::iterator
										_iter;
		private:
			uiUpdateLink();
		protected:
			~uiUpdateLink();
		public:
			friend class uiMgr;
		};

		class uiMgr: public RefBase, public singleton<uiMgr>
		{
		private:
			class BGCacheCfg
			{
			public:
				int32_t						dx;
				int32_t						dy;
			public:
				BGCacheCfg(int32_t dx_, int32_t dy_);
			public:
				bool						operator<(const BGCacheCfg& rhs);
			};
		private:
			intptr_t					_hwnd;							// platform specific
		private:
			SPtr<uiBase>				_root;
			WPtr<uiBase>				_ui_capture_input;
			WPtr<uiBase>				_ui_draw_overhang;				// used to solve drag & drop
		private:
			Vec2						_pointer_pos;
		private:
			WPtr<uiBase>				_ui_last_hover;
		private:
			size_t						_sync_desktop_mouse;
		protected:
			//PCTimer					_frame_interval;
			//Img2D*					_img_visfade1;
			//Img2D*					_img_visfade2;
			REAL						_visfade_speed;
		private:
			int32_t						_res_dx;
			int32_t						_res_dy;
		private:
			stack<rsView>				_stack_viewstate;
		private:
			temp_stock<Tex2D, BGCacheCfg>
										_stock_bgcache_storage;
		private:
			list<WPtr<uiUpdateLink>>	_update_items;
			vector<WPtr<uiUpdateLink>>	_temp_update_items;					// bullet proof haha
		public:
			uiMgr();
		protected:
			~uiMgr();
		//public:
		//	void			removeUIRef(uiBase* ui);
		public:
			void						setHWND(intptr_t hwnd);
			intptr_t					getHWND() const;
		public:
			const SPtr<uiBase>&			getRoot() const;
			//DIWrap*					getDI();
			SPtr<uiBase>				getLastHoverUI() const;
			SPtr<uiBase>				uiAtPoint(const Vec2& pos) const;
			SPtr<uiBase>				uiAtPoint(const Vec2& pos, const SPtr<uiBase>& ui) const;	// pos in ui coordinates
		public:
			void						update(const SPtr<Input>& input, REAL dt);
		public:
			void						setCapture(const SPtr<uiBase>& ui);
			void						releaseCapture(const SPtr<uiBase>& ui);
			SPtr<uiBase>				getCapture() const;
			void						setDrawOverhang(const SPtr<uiBase>& ui);
			void						releaseDrawOverhang(const SPtr<uiBase>& ui);
			SPtr<uiBase>				getDrawOverhang() const;
			const Vec2&					getPointerPos() const;
		public:
			void						setSyncDesktopMouse(bool sync);
			bool						getSyncDesktopMouse() const;
		public:
			void						setVisibilityFadeSpeed(REAL speed);
			REAL						getVisibilityFadeSpeed() const;

		public:				// imaginary resolution, resolution independent ui
			void						setRes(int32_t dx, int32_t dy);
			int32_t						getResDX() const;
			int32_t						getResDY() const;
		public:
			//REAL						getPixelScaleH() const;
			//REAL						getPixelScaleV() const;
		public:
			//void			set2DRS();
		public:
			void						setTransforms(const Mat4& world);
		public:
			void						drawAll(REAL dt);
		private:
			void						draw(const SPtr<uiBase>& ui, const Vec2& parent_abspos, REAL dt);
		public:
			void						registerUpdate(const SPtr<uiBase>& ui);
			void						unregisterUpdate(const SPtr<uiBase>& ui);
		public:
			void						pushViewState_intersectClipRect(int32_t x, int32_t y, int32_t dx, int32_t dy);
			void						popViewState();
		private:
			SPtr<Tex2D>					__source_bgcache_storage(const BGCacheCfg& cfg);
		public:
			//void						reg_update(uiBase* ui);
			//void						unreg_update(uiBase* ui);
		//public:
		//	void			setScissorLT_push(int x,int y,int dx,int dy);
		//	void			setScissorLT_pop();
		//public:
		//	static bool		should_process_keyboard();
		};
	};
};


