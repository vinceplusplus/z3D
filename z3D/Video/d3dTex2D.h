
#pragma once

#include "predecl.h"
#include "Tex2D.h"
#include "d3dGfx.h"

namespace z3D
{
	namespace Video
	{
		class d3dTex2D: public Tex2D, public id3dRecreatable
		{
		public:
			DECL_TYPEINFO();
		private:
			WPtr<d3dGfx>			_gfx;
			IDirect3DTexture9*		_tex;
			IDirect3DDevice9*		_device;
		private:					// creation parameters
			Tex2DCreateParams		_create_params;

		protected:
			d3dTex2D(const SPtr<d3dGfx>& gfx, IDirect3DTexture9* tex, IDirect3DDevice9* device, const Tex2DCreateParams& create_params);
			~d3dTex2D();
		protected:
			void					prepare_recreate();
			bool					recreate();
		public:
			const Tex2DCreateParams&
									getCreateParams() const;
		public:
			IDirect3DTexture9*		getD3DTexture() const;
			IDirect3DDevice9*		getD3DDevice() const;
		public:
			bool					getMipLevelDim(size_t level, size_t& dx, size_t& dy) const;
		public:
			bool					lockMipLevel(size_t level, void*& bits, size_t& pitch, const LockParams& lp);
			void					unlockMipLevel(size_t level);
		};

		//// tex2d creatable for rendertarget only
		//class d3dTex2D: public Tex2D, public id3dRecreatable
		//{
		//public:
		//	DECL_TYPEINFO();
		//private:
		//	WPtr<d3dGfx>			_gfx;
		//	IDirect3DTexture9*		_tex;
		//	IDirect3DDevice9*		_device;
		//private:
		//	size_t					_dx;
		//	size_t					_dy;
		//	size_t					_levels;
		//	GfxEnum::PIXELFORMAT	_format;
		//	size_t					_rendertarget;
		//protected:
		//	d3dTex2D(const SPtr<d3dGfx>& gfx, IDirect3DTexture9* tex, IDirect3DDevice9* device, size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
		//	~d3dTex2D();
		//protected:
		//	static IDirect3DTexture9*	create_d3dtex9(IDirect3DDevice9* device, size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget=false);
		//public:
		//	static SPtr<d3dTex2D>	create(const SPtr<d3dGfx>& gfx, size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
		//private:
		//	static bool				checkManaged(IDirect3DTexture9* tex);
		//public:
		//	bool					reload();
		//public:
		//	SPtr<TexBase>			getRealTexture();
		//public:
		//	GfxEnum::PIXELFORMAT	getPixelFormat() const;
		//public:
		//	size_t					getDX() const;
		//	size_t					getDY() const;
		//public:
		//	size_t					getMipLevels() const;
		//	bool					getMipLevelDim(size_t level, size_t& dx, size_t& dy) const;
		//public:
		//	bool					lockMipLevel(size_t level, void*& bits, size_t& pitch);
		//	void					unlockMipLevel(size_t level);
		//public:
		//	IDirect3DTexture9*		d3d_tex() const;
		//protected:
		//	void					prepare_recreate();
		//	bool					recreate();
		//public:
		//	wcs						short_desc() const;
		//public:
		//	IDirect3DDevice9*		device() const;
		//};
	};
};

