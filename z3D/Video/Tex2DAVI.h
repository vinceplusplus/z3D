
#pragma once

#include "predecl.h"
#include "Tex2D.h"

namespace z3D
{
	namespace Video
	{
		//// tex2d avi cache frames
		//class Tex2DAVICF: public ResBase
		//{
		//protected:
		//	//static ResMgr					_res_mgr;
		//public:
		//	SPtr< AnimData< SPtr<Tex2D> > >	data;
		//public:
		//	int								loops;
		//protected:
		//	Tex2DAVICF(const wcs& filepath);
		//	~Tex2DAVICF();
		//public:
		//	static SPtr<Tex2DAVICF>			load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//	static SPtr<Tex2DAVICF>			loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2, bool no_cache_avi);
		//};

		//class Tex2DAVI: public Tex2D, public AnimInst
		//{
		//public:
		//	DECL_TYPEINFO();
		//protected:
		//	static unsigned int				MAX_RES;
		//	static bool						FORCE_16BIT;
		//protected:
		//	static map<wcs,bool>			_map_cache_frames_filepath;
		//protected:
		//	SPtr<Tex2D>						_tex2d;
		//	AVIRenderer*					_renderer;
		//protected:
		//	SPtr<Tex2DAVICF>				_cf_provider;
		//	AnimDataEval< SPtr<Tex2D> >		_cf;
		//protected:
		//									Tex2DAVI(const SPtr<Tex2D>& tex2d, AVIRenderer* renderer);
		//	virtual							~Tex2DAVI();
		//public:
		//	static SPtr<Tex2DAVI>			load(const wcs& filepath, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//	static SPtr<Tex2DAVI>			load(AVIRenderer* renderer, GfxEnum::PIXELFORMAT format, bool non_pow2);		// renderer ownership passed in if load successful
		//public:
		//	static SPtr<Tex2DAVI>			loadNoCache(const wcs& filepath, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//public:
		//	static void						addCacheFramesAVI(const wcs& filepath);
		//	static void						removeCacheFramesAVI(const wcs& filepath);
		//	static bool						isCacheFramesAVI(const wcs& filepath);
		//	static vector<wcs>				getCacheFramesAVIs();
		//public:
		//	static void						setMaxRes(unsigned int max_res);
		//	static unsigned int				getMaxRes();
		//	static void						setForce16Bit(bool force);
		//	static bool						getForce16Bit();
		//public:
		//	virtual GfxEnum::PIXELFORMAT	getPixelFormat() const;
		//public:
		//	virtual size_t					getDX() const;
		//	virtual size_t					getDY() const;
		//public:
		//	virtual size_t					getMipLevels() const;
		//	virtual bool					getMipLevelDim(size_t level, size_t& dx, size_t& dy) const;
		//public:
		//	virtual bool					lockMipLevel(size_t level, void*& bits, size_t& pitch);
		//	virtual void					unlockMipLevel(size_t level);
		//public:
		//	virtual void					updateMipLevel(size_t level, const void* fip);
		//public:
		//	virtual void					update();
		//	virtual SPtr<TexBase>			getRealTexture();
		//public:
		//	AVIRenderer*					renderer();
		//};
	};
};

