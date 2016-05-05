
#pragma once

#include "predecl.h"
#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class Gfx: public RefBase, public singleton<Gfx>
		{
		public:
			DECL_TYPEINFO();
		protected:
			virtual						~Gfx();
#if defined(Z3D_ENABLE_VIDEO_D3D)
		public:
			static SPtr<Gfx>			createD3D(intptr_t hwnd, bool vsync);
			//static void					enumAdaptorsD3D();
#endif
		public:
			virtual size_t				getWindowDX() const = 0;
			virtual size_t				getWindowDY() const = 0;
			virtual size_t				getFramebufferDX() const = 0;
			virtual size_t				getFramebufferDY() const = 0;
		public:
			virtual void				clear(uint32_t x, uint32_t y, uint32_t dx, uint32_t dy, bool clear_color, bool clear_depth, bool clear_stencil, const Vec4& color, REAL depth, uint32_t stencil) = 0;
			virtual void				clear(bool clear_color, bool clear_depth, bool clear_stencil, const Vec4& color, REAL depth, uint32_t stencil) = 0;
		public:
			virtual void				beginScene() = 0;
			virtual void				endScene() = 0;
		public:
			virtual void				flush() = 0;
		public:
			virtual void				present() = 0;
		public:
			virtual void				present(intptr_t hwnd, uint32_t src_x, uint32_t src_y, uint32_t src_dx, uint32_t src_dy, uint32_t dst_x, uint32_t dst_y, uint32_t dst_dx, uint32_t dst_dy) = 0;
		public:
			virtual bool				copyToTexture(const SPtr<Tex2D>& tex, uint32_t src_x, uint32_t src_y, uint32_t src_dx, uint32_t src_dy, uint32_t dst_x, uint32_t dst_y, uint32_t dst_dx, uint32_t dst_dy) = 0;
		public:
			virtual void				notifyDisplaySettingsChanged() = 0;
		public:
			virtual void				enumMultiSampleTypeSupport(GfxEnum::PIXELFORMAT format, bool support[16]) = 0;
		public:
			virtual SPtr<VBO>			createVBO(size_t length, bool dynamic) = 0;
			virtual SPtr<IBO>			createIBO(size_t length, bool _32bit, bool dynamic) = 0;
			virtual SPtr<Tex2D>			createTex2D(size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget) = 0;
			virtual SPtr<Tex2D>			createTex2D(const Tex2DCreateParams& cp) = 0;
			virtual SPtr<TexCube>		createTexCube(size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget) = 0;
			virtual SPtr<Shader>		createShaderFromSource(GfxEnum::SHADERTYPE type, const mbs& source) = 0;
			virtual SPtr<Shader>		createShaderFromFile(GfxEnum::SHADERTYPE type, const wcs& filepath) = 0;
			virtual SPtr<Renderbuffer>	createRenderTargetRenderbuffer(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type) = 0;
			virtual SPtr<Renderbuffer>	createDepthStencilRenderbuffer(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type) = 0;
			virtual ShaderGen*			getShaderGen() = 0;
		public:
			virtual void				getTexFontShaders(bool use_decal, SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
			virtual void				getImg2DShaders(SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
			virtual void				getPassThruShaders(SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
			virtual void				getVSMScreenDepth(bool alpha, SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
			virtual void				getVSMGenShadowMap(bool alpha, SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
			virtual void				getVSMMarkShadow(size_t light_count, SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
			virtual void				getGaussianBlur5(SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
			virtual void				getGaussianBlur3(SPtr<Shader>& vs, SPtr<Shader>& ps) = 0;
		public:
			virtual ResMgr&				getResMgrTex2D() = 0;
			virtual ResMgr&				getResMgrTexCube() = 0;
			virtual ResMgr&				getResMgrTex2DAVICF() = 0;
			virtual ResMgr&				getResMgrTex2DGIFCF() = 0;
		public:							// usage: 
										// unbindDefaultDepthStencil() first
										// unbindAllRenderTargets() then
										// bindRenderTarget(), bindRenderTarget() ...
										// bindDepthStencil() or bindDefaultDepthStencil() ...
			virtual bool				bindRenderTarget(size_t index, const SPtr<Tex2D>& tex2d) = 0;
			virtual bool				bindRenderTarget(size_t index, const SPtr<TexCube>& texcube, GfxEnum::CUBEMAP_FACE face) = 0;
			virtual bool				bindRenderTarget(size_t index, const SPtr<Renderbuffer>& rendertarget) = 0;
			virtual bool				bindDefaultRenderTarget() = 0;														// index must be 0
			virtual bool				unbindRenderTarget(size_t index) = 0;												// just a shortcut to binding NULL
			virtual void				unbindAllRenderTargets() = 0;
		public:
			virtual bool				bindDepthStencil(const SPtr<Tex2D>& tex2d) = 0;
			virtual bool				bindDepthStencil(const SPtr<Renderbuffer>& depthstencil) = 0;
			virtual bool				bindDefaultDepthStencil() = 0;
			virtual bool				unbindDepthStencil() = 0;													// just a short to binding NULL
		public:
			virtual bool				boundRenderTarget() const = 0;					// OBSOLETE, subject to remove
			virtual bool				boundDepthStencil() const = 0;					// OBSOLETE, subject to remove
		public:
			virtual void				push_RT_n_DS() = 0;								// push the current rendertarget and depthstencil, don't use (push and pop) across different present
			virtual void				pop_RT_n_DS() = 0;								// pop back the old rendertarget and depthstencil, don't use (push and pop) across different present
		public:
			virtual bool				supportsNonPower2Tex() const = 0;
			virtual bool				supportsShaderModel2() const = 0;
			virtual bool				supportsShaderModel3() const = 0;
			virtual unsigned int		maxTextures() const = 0;
			virtual unsigned int		maxTextureStages() const = 0;
			virtual unsigned int		maxLights() const = 0;
			virtual unsigned int		maxUserClipPlanes() const = 0;
			virtual unsigned int		maxTextureSize() const = 0;
		public:		// projection stuff
			virtual REAL				getZNearMapping() const = 0;						// the z mapping value of near plane, for ogl, its 1.0f, for d3d, its 0.0f
			virtual REAL				getZFarMapping() const = 0;							// the z mapping value of far plane, for ogl, its -1.0f, for d3d, its 1.0f
			virtual REAL				getWMultiplier() const = 0;							// for perspective, its used to multiply the z value of the vertex(after view), for ogl, its 1.0f, for d3d, its -1.0f, for details, refer to Mat4::perspective()
		public:
			virtual size_t				getAccumPolycount() const = 0;
			virtual void				resetAccumPolycount() = 0;
		//public:						// 1)
		//								// render target stuff first
		//	//virtual void				bindRenderTarget(const SPtr<Tex2D>& tex2d) = 0;
		//	//virtual void				bindRenderTarget(const SPtr<TexCube>& texcube,CUBEMAP_FACE face) = 0;
		//	//virtual void				bindDepthStencil(const SPtr<Tex2D>& tex2d) = 0;

		public:							// 2)
										// view stuff next
			virtual void				setViewState(const rsView& rs) = 0;
			virtual const rsView&		getViewState() const = 0;
			virtual bool				isEmptyRenderRect() const = 0;

		public:							// 3)
										// view and projection transform stuff
			virtual void				setViewProjTransform(const Mat4& view, const Mat4& proj) = 0;								// world transform will not be set
			virtual void				setWorldViewProjTransform(const Mat4& world, const Mat4& view, const Mat4& proj) = 0;		// world transform will be set, useful for 2d stuff
			virtual void				setViewProjTransform2D(const Mat4& world = Mat4::identity) = 0;
			virtual Mat4				adjustProjectionTransform(const Mat4& proj) const = 0;						// adjust projection transform before sending it to shader
			virtual const Mat4&			getViewTransform() const = 0;
			virtual const Mat4&			getUnadjustedProjectionTransform() const = 0;
			virtual const Mat4&			getAdjustedProjectionTransform() const = 0;
			virtual const Mat4&			getUnadjustedViewProjectionTransform() const = 0;
			virtual const Mat4&			getAdjustedViewProjectionTransform() const = 0;
			virtual const Mat4&			getInverseViewTransform() const = 0;

		public:							// 4)
										// shader or not?
			virtual void				useFixedPipeline() = 0;
			virtual void				useProgrammablePipeline(const SPtr<Shader>& vs, const SPtr<Shader>& ps) = 0;

		public:							// 5)
										// world transform stuff (no need for programmable pipeline), here we can once again change the world transform, primarily only used by 2d stuff
			virtual void				setWorldTransform(const Mat4& world) = 0;
			virtual const Mat4&			getWorldTransform() const = 0;

			virtual const Mat4&			getUnadjustedWorldViewProjectionTransform() const = 0;
			virtual const Mat4&			getAdjustedWorldViewProjectionTransform() const = 0;

										// shade render state stuff
			virtual void				setShadeState(const rsShade& rs) = 0;

										// texture stuff
			virtual void				bindTexture							(size_t stage, const SPtr<TexBase>& tex) = 0;

										// vbo & vertex attribute stuff
			virtual void				bindVBO								(const SPtr<VBO>& vbo, uint8_t stride, size_t offset) = 0;
			virtual void				bindVertexPosition					(uint8_t offset, uint8_t comps) = 0;						// with respect to the last VBO set using bindVBO
			virtual void				bindVertexNormal					(uint8_t offset) = 0;										// with respect to the last VBO set using bindVBO
			virtual void				bindVertexColor						(uint8_t offset) = 0;										// with respect to the last VBO set using bindVBO
			virtual void				bindVertexTexCoord					(uint8_t offset, uint8_t index, uint8_t comps) = 0;			// with respect to the last VBO set using bindVBO

										// shader constant stuff
			virtual void				setShaderUniforms					(const ShaderUniforms& s) = 0;
			virtual void				setShaderUniforms					(const ShaderUniforms2& s) = 0;

		//public:						// 6)
		//								// okay, draw now
			virtual void				drawPrimitives						(GfxEnum::PRIMTYPE pt, size_t start_vert, size_t prim_count) = 0;
			virtual void				drawPrimitives						(GfxEnum::PRIMTYPE pt, const Jotter& jotter) = 0;
			virtual void				drawIndexedPrimitives				(GfxEnum::PRIMTYPE pt, const SPtr<IBO>& ibo, size_t start_index, size_t prim_count) = 0;
			virtual void				drawIndexedPrimitives				(GfxEnum::PRIMTYPE pt, const uint32_t* indices, size_t index_count) = 0;
		};

		class LockParams
		{
		public:
			bool					discard;
		public:
			LockParams();
			LockParams(const LockParams& rhs);
			LockParams&				operator =(const LockParams& rhs);
		public:
			void					setDiscard();
		};

		class GfxHelper
		{
		public:
			static void					copy2D(const void* src, void* dst, size_t copy_length_per_line, size_t height, size_t src_pitch, size_t dst_pitch);
		};
	};
};

