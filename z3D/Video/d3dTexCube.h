
#pragma once

#include "predecl.h"
#include "TexCube.h"
#include "d3dGfx.h"

namespace z3D
{
	namespace Video
	{
		class d3dTexCube: public TexCube, public id3dRecreatable
		{
		public:
			DECL_TYPEINFO();
		private:
			WPtr<d3dGfx>			_gfx;
			IDirect3DCubeTexture9*	_tex;
			IDirect3DDevice9*		_device;
		private:					// creation parameters
			TexCubeCreateParams		_create_params;

		protected:
			d3dTexCube(const SPtr<d3dGfx>& gfx, IDirect3DCubeTexture9* tex, IDirect3DDevice9* device, const TexCubeCreateParams& create_params);
			~d3dTexCube();
		protected:
			void					prepare_recreate();
			bool					recreate();
		public:
			const TexCubeCreateParams&
									getCreateParams() const;
		public:
			IDirect3DCubeTexture9*	getD3DCubeTexture() const;
			IDirect3DDevice9*		getD3DDevice() const;
		public:
			bool					getMipLevelDim(size_t level, size_t& length) const;
		public:
			bool					lockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level, void*& bits, size_t& pitch, const LockParams& lp);
			void					unlockMipLevel(size_t level);
		};

		//// texcube creatable for rendertarget only
		//class d3dTexCube: public TexCube, public id3dRecreatable
		//{
		//public:
		//	DECL_TYPEINFO();
		//private:
		//	WPtr<d3dGfx>			_gfx;
		//	IDirect3DCubeTexture9*	_tex;
		//	IDirect3DDevice9*		_device;
		//private:
		//	size_t					_length;
		//	size_t					_levels;
		//	GfxEnum::PIXELFORMAT	_format;
		//	size_t					_rendertarget;
		//protected:
		//	d3dTexCube(const SPtr<d3dGfx>& gfx, IDirect3DCubeTexture9* tex, IDirect3DDevice9* device, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
		//	~d3dTexCube();
		//protected:
		//	static IDirect3DCubeTexture9*	create_d3dtex9(IDirect3DDevice9* device, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
		//public:
		//	static SPtr<d3dTexCube>			create(const SPtr<d3dGfx>& gfx, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
		//private:
		//	static bool						checkManaged(IDirect3DCubeTexture9* tex);
		//public:
		//	SPtr<TexBase>					getRealTexture();
		//public:
		//	GfxEnum::PIXELFORMAT			getPixelFormat() const;
		//public:
		//	size_t							getMipLevels() const;
		//	bool							getMipLevelDim(size_t level, size_t& length) const;
		//public:
		//	bool							lockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level, void*& bits, size_t& pitch, bool& need_mirror, bool& need_flip);
		//	void							unlockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level);
		//public:
		//	IDirect3DCubeTexture9*			d3d_tex() const;
		//protected:
		//	void							prepare_recreate();
		//	bool							recreate();
		//public:
		//	wcs								short_desc() const;
		//public:
		//	IDirect3DDevice9*				device() const;
		//public:
		//	bool							needMirror(GfxEnum::CUBEMAP_FACE face) const;
		//	bool							needFlip(GfxEnum::CUBEMAP_FACE face) const;
		//};
	};
};

