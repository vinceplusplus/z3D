
#include "d3dRenderbuffer.h"

namespace z3D
{
	namespace Video
	{
		d3dRenderbuffer::d3dRenderbuffer(const SPtr<d3dGfx>& gfx, IDirect3DSurface9* surface, IDirect3DDevice9* device, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type, bool rendertarget)
			: _gfx(gfx), _surface(surface), _device(device), _dx(dx), _dy(dy), _format(format), _multisample_type(multisample_type), _rendertarget(rendertarget)
		{
			_device->AddRef();
		}
		d3dRenderbuffer::~d3dRenderbuffer()
		{
			if(_surface)
				_surface->Release();
			if(_device)
				_device->Release();
		}
		IDirect3DSurface9*						d3dRenderbuffer::createRenderTarget(IDirect3DDevice9* device, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			IDirect3DSurface9* surface = NULL;
			device->CreateRenderTarget((UINT)dx, (UINT)dy, d3dGfx::map_D3DFORMAT(format), (D3DMULTISAMPLE_TYPE)(D3DMULTISAMPLE_NONE + multisample_type), 0, FALSE, &surface, NULL);
			return surface;
		}
		IDirect3DSurface9*						d3dRenderbuffer::createDepthStencil(IDirect3DDevice9* device, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			IDirect3DSurface9* surface = NULL;
			device->CreateDepthStencilSurface((UINT)dx, (UINT)dy, d3dGfx::map_D3DFORMAT(format), (D3DMULTISAMPLE_TYPE)(D3DMULTISAMPLE_NONE + multisample_type), 0, FALSE, &surface, NULL);
			return surface;
		}
		SPtr<d3dRenderbuffer>					d3dRenderbuffer::createRenderTarget(const SPtr<d3dGfx>& gfx, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			Z_ASSERT( gfx->type_info()->exactly(d3dGfx::type_info_static()) );

			IDirect3DSurface9* surface = createRenderTarget(gfx->device(), dx, dy, format, multisample_type);
			if(!surface)
				return NULL;
			SPtr<d3dRenderbuffer> t = new d3dRenderbuffer(gfx, surface, gfx->device(), dx, dy, format, multisample_type, true);
			Gfx::current<d3dGfx>()->addRecreatable(t.get());
			return t;
		}
		SPtr<d3dRenderbuffer>					d3dRenderbuffer::createDepthStencil(const SPtr<d3dGfx>& gfx, size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			Z_ASSERT( gfx->type_info()->exactly(d3dGfx::type_info_static()) );

			IDirect3DSurface9* surface = createDepthStencil(gfx->device(), dx, dy, format, multisample_type);
			if(!surface)
				return NULL;
			SPtr<d3dRenderbuffer> t = new d3dRenderbuffer(gfx, surface, gfx->device(), dx, dy, format, multisample_type, false);
			Gfx::current<d3dGfx>()->addRecreatable(t.get());
			return t;
		}
		bool									d3dRenderbuffer::isRenderTarget() const
		{
			return !!_rendertarget;
		}
		bool									d3dRenderbuffer::isDepthStencil() const
		{
			return !_rendertarget;
		}
		GfxEnum::PIXELFORMAT					d3dRenderbuffer::getPixelFormat() const
		{
			return _format;
		}
		size_t									d3dRenderbuffer::getMultiSampleType() const
		{
			return _multisample_type;
		}
		size_t									d3dRenderbuffer::getDX() const
		{
			return _dx;
		}
		size_t									d3dRenderbuffer::getDY() const
		{
			return _dy;
		}
		void									d3dRenderbuffer::prepare_recreate()
		{
			if(!_surface)
				return;
			_surface->Release();
			_surface = NULL;
		}
		bool									d3dRenderbuffer::recreate()
		{
			_surface = ((!!_rendertarget) ? createRenderTarget(_device, _dx, _dy, _format, _multisample_type) : createDepthStencil(_device, _dx, _dy, _format, _multisample_type));
			return !!_surface;
		}
		wcs										d3dRenderbuffer::short_desc() const
		{
			return wcs(L"Renderbuffer: ") + ((!!_rendertarget) ? L"RenderTarget" : L"DepthStencil");
		}
		IDirect3DSurface9*						d3dRenderbuffer::d3d_surface() const
		{
			return _surface;
		}
		IDirect3DDevice9*						d3dRenderbuffer::d3d_device() const
		{
			return _device;
		}
	};
};

