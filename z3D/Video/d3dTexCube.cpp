
#include "d3dGfx.h"
#include "d3dTexCube.h"

namespace z3D
{
	namespace Video
	{
		d3dTexCube::d3dTexCube(const SPtr<d3dGfx>& gfx, IDirect3DCubeTexture9* tex, IDirect3DDevice9* device, const TexCubeCreateParams& create_params)
			: _gfx(gfx), _tex(tex), _device(device), _create_params(create_params)
		{
			_device->AddRef();
		}
		d3dTexCube::~d3dTexCube()
		{
			if(_gfx.valid())
				_gfx.get_unsafe()->removeTexRef(_tex);
			if(_tex)
				_tex->Release();
			if(_device)
				_device->Release();
		}
		void					d3dTexCube::prepare_recreate()
		{
			if(!_tex)
				return;
			_tex->Release();
			_tex = NULL;
		}
		bool					d3dTexCube::recreate()
		{
			if(!_gfx.valid())
				return false;
			_tex = _gfx.get_unsafe()->_createTexCube(_create_params);
			return !!_tex;
		}
		const TexCubeCreateParams&
								d3dTexCube::getCreateParams() const
		{
			return _create_params;
		}
		IDirect3DCubeTexture9*	d3dTexCube::getD3DCubeTexture() const
		{
			return _tex;
		}
		IDirect3DDevice9*		d3dTexCube::getD3DDevice() const
		{
			return _device;
		}
		bool					d3dTexCube::getMipLevelDim(size_t level, size_t& length) const
		{
			D3DSURFACE_DESC desc;
			if(FAILED(_tex->GetLevelDesc((UINT)level, &desc)))
				return false;
			length = desc.Width;

			return true;
		}
		bool					d3dTexCube::lockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level, void*& bits, size_t& pitch, const LockParams& lp)
		{
			D3DLOCKED_RECT rt;
			if(FAILED(_tex->LockRect(d3dGfx::map_D3DCUBEMAP_FACES(face), (UINT)level, &rt, NULL, lp.discard ? D3DLOCK_DISCARD : 0)))					// need optimized for dynamic usage
				return false;
			bits = rt.pBits;
			pitch = rt.Pitch;

			return true;
		}
		void					d3dTexCube::unlockMipLevel(size_t level)
		{
			_tex->UnlockRect((UINT)level);
		}

		//d3dTexCube::d3dTexCube(const SPtr<d3dGfx>& gfx, IDirect3DCubeTexture9* tex, IDirect3DDevice9* device, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//	:_gfx(gfx), _tex(tex), _device(device), _length(length), _levels(levels), _format(format), _rendertarget(rendertarget)
		//{
		//	_device->AddRef();
		//}
		//d3dTexCube::~d3dTexCube()
		//{
		//	if(_gfx.valid())
		//		_gfx.get_unsafe()->removeTexRef(_tex);
		//	if(_tex)
		//		_tex->Release();
		//	if(_device)
		//		_device->Release();
		//}
		//IDirect3DCubeTexture9*			d3dTexCube::create_d3dtex9(IDirect3DDevice9* device, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//{
		//	bool depth = (format == GfxEnum::PIXELFORMAT_D16 || format == GfxEnum::PIXELFORMAT_D32 || format == GfxEnum::PIXELFORMAT_D24S8 || format == GfxEnum::PIXELFORMAT_D24);
		//	D3DFORMAT fmt = d3dGfx::map_D3DFORMAT(format);
		//	if(fmt == D3DFMT_UNKNOWN)
		//		return NULL;
		//	IDirect3DCubeTexture9* tex = NULL;
		//	if(FAILED(device->CreateCubeTexture((UINT)length, (UINT)levels, (rendertarget ? (D3DUSAGE_RENDERTARGET/* | D3DUSAGE_AUTOGENMIPMAP*/) : 0)|(depth ? D3DUSAGE_DEPTHSTENCIL : 0), fmt, (rendertarget | depth) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &tex, NULL)))
		//		if(fmt != D3DFMT_R8G8B8 || FAILED(device->CreateCubeTexture((UINT)length, (UINT)levels, rendertarget ? (D3DUSAGE_RENDERTARGET/* | D3DUSAGE_AUTOGENMIPMAP*/) : 0, D3DFMT_A8R8G8B8, (rendertarget | depth) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &tex, NULL)))			// special case for unable to create r8g8b8 in intel chipset
		//			return NULL;
		//	return tex;
		//}
		//SPtr<d3dTexCube>				d3dTexCube::create(const SPtr<d3dGfx>& gfx, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//{
		//	Z_ASSERT( gfx && gfx->type_info()->exactly(d3dGfx::type_info_static()) );
		//	IDirect3DCubeTexture9* tex = create_d3dtex9(gfx->device(), length, levels, format, rendertarget);
		//	if(!tex)
		//		return NULL;
		//	SPtr<d3dTexCube> t = new d3dTexCube(gfx, tex, gfx->device(), length, levels, format, rendertarget);
		//	if(!checkManaged(tex))
		//		Gfx::current<d3dGfx>()->addRecreatable(t.get());
		//	return t;
		//}
		//bool							d3dTexCube::checkManaged(IDirect3DCubeTexture9* tex)
		//{
		//	D3DSURFACE_DESC desc;
		//	size_t c = (size_t)tex->GetLevelCount();
		//	for(size_t i = 0; i < c; ++i)
		//	{
		//		tex->GetLevelDesc((UINT)i, &desc);
		//		if(desc.Pool != D3DPOOL_MANAGED)
		//			return false;
		//	}
		//	return true;
		//}
		//SPtr<TexBase>						d3dTexCube::getRealTexture()
		//{
		//	return this;
		//}
		//GfxEnum::PIXELFORMAT				d3dTexCube::getPixelFormat() const
		//{
		//	return _format;
		//}
		//size_t							d3dTexCube::getMipLevels() const
		//{
		//	return _levels;
		//}
		//bool							d3dTexCube::getMipLevelDim(size_t level, size_t& length) const
		//{
		//	D3DSURFACE_DESC desc;
		//	if(FAILED(_tex->GetLevelDesc((UINT)level, &desc)))
		//		return false;
		//	length = desc.Width;

		//	return true;
		//}
		//bool							d3dTexCube::lockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level, void*& bits, size_t& pitch, bool& need_mirror, bool& need_flip)
		//{
		//	D3DLOCKED_RECT rt;
		//	if(FAILED(_tex->LockRect(d3dGfx::map_D3DCUBEMAP_FACES(face), (UINT)level, &rt, NULL, 0)))					// need optimized for dynamic usage
		//		return false;
		//	bits = rt.pBits;
		//	pitch = rt.Pitch;
		//	need_mirror = needMirror(face);
		//	need_flip = needFlip(face);

		//	return true;
		//}
		//void							d3dTexCube::unlockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level)
		//{
		//	_tex->UnlockRect(d3dGfx::map_D3DCUBEMAP_FACES(face), (UINT)level);
		//}
		//IDirect3DCubeTexture9*			d3dTexCube::d3d_tex() const
		//{
		//	return _tex;
		//}
		//void							d3dTexCube::prepare_recreate()
		//{
		//	if(!_tex)
		//		return;
		//	_tex->Release();
		//	_tex=NULL;
		//}
		//bool							d3dTexCube::recreate()
		//{
		//	_tex = create_d3dtex9(_device, _length, _levels, _format, !!_rendertarget);
		//	return !!_tex;
		//}
		//wcs								d3dTexCube::short_desc() const
		//{
		//	wcs desc;
		//	desc = FORMATW(L"TexCube: %d, ", _length);
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
		//IDirect3DDevice9*				d3dTexCube::device() const
		//{
		//	return _device;
		//}
		//bool							d3dTexCube::needMirror(GfxEnum::CUBEMAP_FACE face) const
		//{
		//	switch(face)
		//	{
		//	case GfxEnum::CUBEMAP_FACE_POSITIVE_X:		return true;
		//	case GfxEnum::CUBEMAP_FACE_NEGATIVE_X:		return true;
		//	case GfxEnum::CUBEMAP_FACE_POSITIVE_Y:		return false;
		//	case GfxEnum::CUBEMAP_FACE_NEGATIVE_Y:		return false;
		//	case GfxEnum::CUBEMAP_FACE_POSITIVE_Z:		return true;
		//	case GfxEnum::CUBEMAP_FACE_NEGATIVE_Z:		return true;
		//	}
		//	Z_ASSERT( false );
		//	return false;
		//}
		//bool							d3dTexCube::needFlip(GfxEnum::CUBEMAP_FACE face) const
		//{
		//	switch(face)
		//	{
		//	case GfxEnum::CUBEMAP_FACE_POSITIVE_X:		return false;
		//	case GfxEnum::CUBEMAP_FACE_NEGATIVE_X:		return false;
		//	case GfxEnum::CUBEMAP_FACE_POSITIVE_Y:		return true;
		//	case GfxEnum::CUBEMAP_FACE_NEGATIVE_Y:		return true;
		//	case GfxEnum::CUBEMAP_FACE_POSITIVE_Z:		return false;
		//	case GfxEnum::CUBEMAP_FACE_NEGATIVE_Z:		return false;
		//	}
		//	Z_ASSERT( false );
		//	return false;
		//}
	};
};

