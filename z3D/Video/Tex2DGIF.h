
#pragma once

#include "predecl.h"
#include "Tex2D.h"

namespace z3D
{
	namespace Video
	{
		//// tex2d gif cache frames
		//class Tex2DGIFCF: public ResBase
		//{
		//protected:
		//	//static ResMgr					_res_mgr;
		//public:
		//	SPtr<AnimData< SPtr<Tex2D> > >	data;
		//public:
		//	int								loops;
		//protected:
		//	Tex2DGIFCF(const wcs& filepath);
		//	~Tex2DGIFCF();
		//public:
		//	static SPtr<Tex2DGIFCF>			load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//	static SPtr<Tex2DGIFCF>			loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//};

		//class Tex2DGIF: public Tex2D, public AnimInst
		//{
		//public:
		//	DECL_TYPEINFO();
		//protected:
		//	static unsigned int			MAX_RES;
		//	static bool					FORCE_16BIT;
		//protected:
		//	SPtr<Tex2D>					_tex2d;
		//protected:
		//	SPtr<Tex2DGIFCF>			_cf_provider;
		//	AnimDataEval< SPtr<Tex2D> >	_cf;
		//protected:
		//									Tex2DGIF(const SPtr<Tex2D>& tex2d);		// first frame
		//	virtual							~Tex2DGIF();
		//public:
		//	static SPtr<Tex2DGIF>			load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//	static SPtr<Tex2DGIF>			loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
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
		//};
	};
};

