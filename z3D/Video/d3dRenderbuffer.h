
#pragma once

#include "predecl.h"
#include "Renderbuffer.h"
#include "d3dGfx.h"

namespace z3D
{
	namespace Video
	{
		class d3dRenderbuffer: public Renderbuffer, public id3dRecreatable
		{
		private:
			WPtr<d3dGfx>						_gfx;
			IDirect3DSurface9*					_surface;
			IDirect3DDevice9*					_device;
		private:
			size_t								_dx;
			size_t								_dy;
			GfxEnum::PIXELFORMAT				_format;
			size_t								_multisample_type;
			size_t								_rendertarget;
		public:
			DECL_TYPEINFO();
		protected:
			d3dRenderbuffer(const SPtr<d3dGfx>& gfx, IDirect3DSurface9* surface, IDirect3DDevice9* device, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type, bool rendertarget);
			~d3dRenderbuffer();
		public:
			static IDirect3DSurface9*			createRenderTarget(IDirect3DDevice9* device, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
			static IDirect3DSurface9*			createDepthStencil(IDirect3DDevice9* device, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
		public:
			static SPtr<d3dRenderbuffer>		createRenderTarget(const SPtr<d3dGfx>& gfx, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
			static SPtr<d3dRenderbuffer>		createDepthStencil(const SPtr<d3dGfx>& gfx, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
		public:
			bool								isRenderTarget() const;
			bool								isDepthStencil() const;
		public:
			GfxEnum::PIXELFORMAT				getPixelFormat() const;
		public:
			size_t								getMultiSampleType() const;
		public:
			size_t								getDX() const;
			size_t								getDY() const;
		protected:
			void								prepare_recreate();
			bool								recreate();
		public:
			wcs									short_desc() const;
		public:
			IDirect3DSurface9*					d3d_surface() const;
			IDirect3DDevice9*					d3d_device() const;
		};
	};
};

