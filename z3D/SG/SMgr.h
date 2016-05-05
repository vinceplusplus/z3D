
#pragma once

#include "predecl.h"

#include "SCameraNode.h"
#include "MeshRenderer.h"
#include "RenderInfo.h"

namespace z3D
{
	namespace SG
	{
		class SMgr: public RefBase, public singleton<SMgr>
		{
		private:
			SPtr<SNode>					_root;
		private:
			SPtr<SCameraNode>			_active_camera;
		private:
			Vec3						_global_ambient;
		private:
			stock<Tex2D>				_stock_screen_float;					// screen -> screen size
			stock<Tex2D>				_stock_screen_depth;
			stock<Tex2D>				_stock_screen_rgba;
			stock<Tex2D>				_stock_2floats;
			stock<Tex2D>				_stock_depth;
			stock<SCameraNode>			_stock_camera;
			stock<Tex2D>				_stock_rgba;
			stock<Renderbuffer>			_stock_screen_rb_depth;
			stock<Renderbuffer>			_stock_rb_depth;
		private:
			uint32_t					_non_screen_size_rt_dx;
			uint32_t					_non_screen_size_rt_dy;
		private:
			size_t						_default_max_recursions;
		private:
			functor2<void, const SRenderInfo*, rsShade&>
										_post_setup_shade_state_adjust_cb;
		private:
			union
			{
				struct
				{
					bool				_enable_shader:1;
					bool				_enable_shadow:1;
				};
				unsigned long	__UNNAMED0;
			};
		private:
			vector<Plane>				_empty_array_clipplane;
		public:
			SMgr();
		protected:
			~SMgr();
		public:
			void						enableShader(bool enable);
			void						enableShadow(bool enable);
			bool						enabledShader() const;
			bool						enabledShadow() const;
		public:
			void						setActiveCamera(const SPtr<SCameraNode>& node);
			const SPtr<SCameraNode>&	getActiveCamera() const;
		public:
			SPtr<SNode>&				root();
		public:
			void						drawAll(DRAWPASS pass = DRAWPASS_DEFAULT, SRenderInfo* ri = NULL);
		public:
			void						draw(const SPtr<SNode>& the_root, DRAWPASS pass = DRAWPASS_DEFAULT, SRenderInfo* ri = NULL);
		//protected:
		//	void						setRenderInfo(SRenderInfo* ri);
		protected:
			void						preRender(const SPtr<SNode>& node, const Frustum& frustum, SRenderInfo* ri);
			void						preRender(const SPtr<SNode>& node, SRenderInfo* ri);								// will skip frustum culling
		protected:
			void						preRenderLighting(const SPtr<SNode>& node, const AABB& bound, SRenderInfo* ri);
			void						preRenderLighting(const SPtr<SNode>& node, SRenderInfo* ri);								// will skip frustum culling
		protected:
			void						render(SRenderInfo* ri);
		protected:
			void						render_normal(SRenderInfo* ri);
			void						render_generate_shadowmap();
			void						render_shadowmapping_fill_shadow();
		protected:
			void						render_dir_shadowmapping_fill_shadow();
			void						render_point_shadowmapping_fill_shadow();
			void						render_spot_shadowmapping_fill_shadow();
		protected:
			void						clear_shadowmap();										// used to fill the whole shadow map with effectively long distance, because normal color clearing can only clear the value up to 1.0
		protected:
			void						render_fill_reflection(const SRenderItem& info);
		private:
		public:
			void						blur5x5(const SPtr<Tex2D>& tex, const SPtr<Tex2D>& tmp);		// will modify rendertarget and other states
		public:
			bool						doUseShader() const;
			bool						doUseShadow() const;
		public:
			void						setNonScreenSizeRTSize(uint32_t dx, uint32_t dy);
			void						getNonScreenSizeRTSize(uint32_t& dx, uint32_t& dy) const;
		public:
			void						setDefaultMaxRecursions(size_t max_recursions);
			size_t						getDefaultMaxRecursions() const;
		public:
			void						setPostSetupShadeStateAdjustCallback(const functor2<void, const SRenderInfo*, rsShade&>& cb);
			const functor2<void, const SRenderInfo*, rsShade&>&
										getPostSetupShadeStateAdjustCallback() const;
		private:						// when screen resolution change, the relevant screen size stocks must be cleared
			SPtr<Tex2D>					__stock_source_screen_float();
			SPtr<Tex2D>					__stock_source_screen_depth();
			SPtr<Tex2D>					__stock_source_screen_rgba();
			SPtr<Tex2D>					__stock_source_2floats();
			SPtr<Tex2D>					__stock_source_depth();
			SPtr<Tex2D>					__stock_source_rgba();
			SPtr<Renderbuffer>			__stock_source_screen_rb_depth();
			SPtr<Renderbuffer>			__stock_source_rb_depth();
			SPtr<SCameraNode>			__stock_source_camera();
			void						__stock_renew_camera(const SPtr<SCameraNode>& camera_node);
		public:
			SPtr<Tex2D>					getStockScreenFloat();
			SPtr<Tex2D>					getStockScreenDepth();
			SPtr<Tex2D>					getStockScreenRGBA();
			SPtr<Tex2D>					getStockNonScreen2Floats();
			SPtr<Tex2D>					getStockNonScreenDepth();
			SPtr<Tex2D>					getStockNonScreenRGBA();
			SPtr<Renderbuffer>			getStockScreenRBDepth();
			SPtr<Renderbuffer>			getStockNonScreenRBDepth();
			SPtr<SCameraNode>			getStockCamera();
		};
	};
};

