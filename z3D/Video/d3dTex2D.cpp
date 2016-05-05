
#include "predecl.h"

#if defined(Z3D_ENABLE_VIDEO_D3D)

#include "d3dTex2D.h"

namespace z3D
{
	namespace Video
	{
		d3dTex2D::d3dTex2D(const SPtr<d3dGfx>& gfx, IDirect3DTexture9* tex, IDirect3DDevice9* device, const Tex2DCreateParams& create_params)
			: _gfx(gfx), _tex(tex), _device(device), _create_params(create_params)
		{
			_device->AddRef();
		}
		d3dTex2D::~d3dTex2D()
		{
			if(_gfx.valid())
				_gfx.get_unsafe()->removeTexRef(_tex);
			if(_tex)
				_tex->Release();
			if(_device)
				_device->Release();
		}
		void					d3dTex2D::prepare_recreate()
		{
			if(!_tex)
				return;
			_tex->Release();
			_tex = NULL;
		}
		bool					d3dTex2D::recreate()
		{
			if(!_gfx.valid())
				return false;
			_tex = _gfx.get_unsafe()->_createTex2D(_create_params);
			return !!_tex;
		}
		const Tex2DCreateParams&
								d3dTex2D::getCreateParams() const
		{
			return _create_params;
		}
		IDirect3DTexture9*		d3dTex2D::getD3DTexture() const
		{
			return _tex;
		}
		IDirect3DDevice9*		d3dTex2D::getD3DDevice() const
		{
			return _device;
		}
		bool					d3dTex2D::getMipLevelDim(size_t level, size_t& dx, size_t& dy) const
		{
			D3DSURFACE_DESC desc;
			if(FAILED(_tex->GetLevelDesc((UINT)level, &desc)))
				return false;
			dx = (size_t)desc.Width;
			dy = (size_t)desc.Height;

			return true;
		}
		bool					d3dTex2D::lockMipLevel(size_t level, void*& bits, size_t& pitch, const LockParams& lp)
		{
			D3DLOCKED_RECT rt;
			if(FAILED(_tex->LockRect((UINT)level, &rt, NULL, lp.discard ? D3DLOCK_DISCARD : 0)))
				return false;
			bits = rt.pBits;
			pitch = rt.Pitch;

			return true;
		}
		void					d3dTex2D::unlockMipLevel(size_t level)
		{
			_tex->UnlockRect((UINT)level);
		}
		//d3dTex2D::d3dTex2D(const SPtr<d3dGfx>& gfx, IDirect3DTexture9* tex, IDirect3DDevice9* device, size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//	:_gfx(gfx), _tex(tex), _device(device), _dx(dx), _dy(dy), _levels(levels), _format(format), _rendertarget(rendertarget)
		//{
		//	_device->AddRef();
		//}
		//d3dTex2D::~d3dTex2D()
		//{
		//	if(_gfx.valid())
		//		_gfx.get_unsafe()->removeTexRef(_tex);
		//	if(_tex)
		//		_tex->Release();
		//	if(_device)
		//		_device->Release();
		//}
		//IDirect3DTexture9*		d3dTex2D::create_d3dtex9(IDirect3DDevice9* device, size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//{
		//	bool depth = (format == GfxEnum::PIXELFORMAT_D16 || format == GfxEnum::PIXELFORMAT_D32 || format == GfxEnum::PIXELFORMAT_D24S8 || format == GfxEnum::PIXELFORMAT_D24);
		//	D3DFORMAT fmt = d3dGfx::map_D3DFORMAT(format);
		//	if(fmt == D3DFMT_UNKNOWN)
		//		return NULL;
		//	IDirect3DTexture9* tex = NULL;
		//	if(FAILED(device->CreateTexture((UINT)dx, (UINT)dy, (UINT)levels, (rendertarget ? (D3DUSAGE_RENDERTARGET/* | D3DUSAGE_AUTOGENMIPMAP*/) : 0) | (depth ? D3DUSAGE_DEPTHSTENCIL : 0), fmt, (rendertarget | depth) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &tex, NULL)))
		//		if( fmt != D3DFMT_R8G8B8 || FAILED(device->CreateTexture((UINT)dx, (UINT)dy, (UINT)levels, rendertarget ? (D3DUSAGE_RENDERTARGET/* | D3DUSAGE_AUTOGENMIPMAP*/) : 0, D3DFMT_A8R8G8B8, (rendertarget | depth) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &tex, NULL)))			// special case for unable to create r8g8b8 in intel chipset
		//			return NULL;
		//	return tex;
		//}
		//SPtr<d3dTex2D>			d3dTex2D::create(const SPtr<d3dGfx>& gfx, size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//{
		//	Z_ASSERT( gfx->type_info()->exactly(d3dGfx::type_info_static()) );

		//	IDirect3DTexture9* tex = create_d3dtex9(gfx->device(), dx, dy, levels, format, rendertarget);
		//	if(!tex)
		//		return NULL;
		//	SPtr<d3dTex2D> t = new d3dTex2D(gfx, tex, gfx->device(), dx, dy, levels, format, rendertarget);
		//	if(!checkManaged(tex))
		//		Gfx::current<d3dGfx>()->addRecreatable(t.get());
		//	return t;
		//}
		//bool					d3dTex2D::checkManaged(IDirect3DTexture9* tex)
		//{
		//	D3DSURFACE_DESC desc;
		//	size_t c = tex->GetLevelCount();
		//	for(size_t i = 0; i < c; ++i)
		//	{
		//		tex->GetLevelDesc((UINT)i, &desc);
		//		if(desc.Pool != D3DPOOL_MANAGED)
		//			return false;
		//	}
		//	return true;
		//}
		//bool						d3dTex2D::reload()
		//{
		//	if(!filepath().length())
		//		return false;

		//	if(!type_info()->exactly(d3dTex2D::type_info_static()))
		//		return false;

		//	SPtr<Tex2D> tex = Tex2D::loadNoCache(filepath(), _levels > 1, _format, (((_dx - 1) & _dx) != 0) || (((_dy - 1) & _dy) != 0));
		//	if(!tex)
		//		return false;

		//	if(!tex->type_info()->exactly(d3dTex2D::type_info_static()))
		//		return false;

		//	SPtr<d3dTex2D> d3dtex = tex.cast<d3dTex2D>();
		//	if(d3dtex->_gfx != _gfx)
		//		return false;

		//	if(_tex)
		//		_tex->Release();
		//	_tex = d3dtex->_tex;
		//	_tex->AddRef();

		//	_dx = d3dtex->_dx;
		//	_dy = d3dtex->_dy;
		//	_levels = d3dtex->_levels;
		//	_format = d3dtex->_format;
		//	_rendertarget = d3dtex->_rendertarget;

		//	return false;
		//}
		//SPtr<TexBase>				d3dTex2D::getRealTexture()
		//{
		//	return this;
		//}
		//GfxEnum::PIXELFORMAT		d3dTex2D::getPixelFormat() const
		//{
		//	return _format;
		//}
		//size_t						d3dTex2D::getDX() const
		//{
		//	return _dx;
		//}
		//size_t						d3dTex2D::getDY() const
		//{
		//	return _dy;
		//}
		//size_t						d3dTex2D::getMipLevels() const
		//{
		//	return _levels;
		//}
		//bool					d3dTex2D::getMipLevelDim(size_t level, size_t& dx, size_t& dy) const
		//{
		//	D3DSURFACE_DESC desc;
		//	if(FAILED(_tex->GetLevelDesc((UINT)level, &desc)))
		//		return false;
		//	dx = (size_t)desc.Width;
		//	dy = (size_t)desc.Height;

		//	return true;
		//}
		//bool					d3dTex2D::lockMipLevel(size_t level, void*& bits, size_t& pitch)
		//{
		//	D3DLOCKED_RECT rt;
		//	//if(FAILED(_tex->LockRect(level, &rt, NULL, D3DLOCK_DISCARD)))				// valid only for dynamic
		//	if(FAILED(_tex->LockRect((UINT)level, &rt, NULL, 0)))
		//		return false;
		//	bits = rt.pBits;
		//	pitch = rt.Pitch;

		//	return true;
		//}
		//void					d3dTex2D::unlockMipLevel(size_t level)
		//{
		//	_tex->UnlockRect((UINT)level);
		//}
		//IDirect3DTexture9*			d3dTex2D::d3d_tex() const
		//{
		//	return _tex;
		//}
		//void						d3dTex2D::prepare_recreate()
		//{
		//	if(!_tex)
		//		return;
		//	_tex->Release();
		//	_tex = NULL;
		//}
		//bool						d3dTex2D::recreate()
		//{
		//	_tex = create_d3dtex9(_device, _dx, _dy, _levels, _format, !!_rendertarget);
		//	return !!_tex;
		//}
		//wcs							d3dTex2D::short_desc() const
		//{
		//	wcs desc;
		//	desc = FORMATW(L"Tex2D: %d x %d, ", _dx, _dy);
		//	switch(_format)
		//	{
		//	case GfxEnum::PIXELFORMAT_R8G8B8:
		//		desc += L"PIXELFORMAT_R8G8B8";
		//		break;
		//	case GfxEnum::PIXELFORMAT_A8R8G8B8:
		//		desc += L"PIXELFORMAT_A8R8G8B8";
		//		break;
		//	case GfxEnum::PIXELFORMAT_A4R4G4B4:
		//		desc += L"PIXELFORMAT_A4R4G4B4";
		//		break;
		//	case GfxEnum::PIXELFORMAT_R5G6B5:
		//		desc += L"PIXELFORMAT_R5G6B5";
		//		break;
		//	case GfxEnum::PIXELFORMAT_A8:
		//		desc += L"PIXELFORMAT_A8";
		//		break;
		//	case GfxEnum::PIXELFORMAT_L8:
		//		desc += L"PIXELFORMAT_L8";
		//		break;
		//	case GfxEnum::PIXELFORMAT_D16:
		//		desc += L"PIXELFORMAT_D16";
		//		break;
		//	case GfxEnum::PIXELFORMAT_D32:
		//		desc += L"PIXELFORMAT_D32";
		//		break;
		//	case GfxEnum::PIXELFORMAT_D24S8:
		//		desc += L"PIXELFORMAT_D24S8";
		//		break;
		//	case GfxEnum::PIXELFORMAT_D24:
		//		desc += L"PIXELFORMAT_D24";
		//		break;
		//	case GfxEnum::PIXELFORMAT_R32F:
		//		desc += L"PIXELFORMAT_R32F";
		//		break;
		//	}
		//	if(_rendertarget)
		//		desc += L", rendertarget";
		//	return desc;
		//}
		//IDirect3DDevice9*		d3dTex2D::device() const
		//{
		//	return _device;
		//}
	};
};

#endif