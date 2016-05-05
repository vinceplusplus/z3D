
#include "Tex2D.h"
//#include "Tex2DGIF.h"
//#include "Tex2DAVI.h"

//#include "../../FreeImage/Header.h"

namespace z3D
{
	namespace Video
	{
		Tex2D::Tex2D()
		{
		}
		Tex2D::~Tex2D()
		{
		}
		GfxEnum::TEXTARGET		Tex2D::getTarget() const
		{
			return GfxEnum::TEXTARGET_2D;
		}

		Tex2DCreateParams::Tex2DCreateParams(size_t dx_, size_t dy_, size_t level_count_, GfxEnum::PIXELFORMAT format_, GfxEnum::POOL pool_)
			: dx(dx_), dy(dy_), level_count(level_count_), format(format_), pool(pool_)
		{
			auto_gen_mipmap = false;
			depth_stencil = false;
			dynamic = false;
			render_target = false;
		}
		Tex2DCreateParams::Tex2DCreateParams(const Tex2DCreateParams& rhs)
		{
			*this = rhs;
		}
		Tex2DCreateParams&		Tex2DCreateParams::operator =(const Tex2DCreateParams& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));
			return *this;
		}
		Tex2DCreateParams&		Tex2DCreateParams::setUsageAutoGenMipmap()
		{
			auto_gen_mipmap = true;
			return *this;
		}
		Tex2DCreateParams&		Tex2DCreateParams::setUsageDepthStencil()
		{
			depth_stencil = true;
			return *this;
		}
		Tex2DCreateParams&		Tex2DCreateParams::setUsageDynamic()
		{
			dynamic = true;
			return *this;
		}
		Tex2DCreateParams&		Tex2DCreateParams::setUsageRenderTarget()
		{
			render_target = true;
			return *this;
		}

		//Tex2D::Tex2D():TexBase(L"")
		//{
		//}
		//Tex2D::~Tex2D()
		//{
		//}
		//void					Tex2D::setMaxRes(unsigned int max_res)
		//{
		//	MAX_RES = max_res;
		//}
		//unsigned int			Tex2D::getMaxRes()
		//{
		//	return MAX_RES;
		//}
		//void					Tex2D::setForce16Bit(bool force)
		//{
		//	FORCE_16BIT = force;
		//}
		//bool					Tex2D::getForce16Bit()
		//{
		//	return FORCE_16BIT;
		//}
		//unsigned int			Tex2D::getMaxSize()							// hardware maximum texture 2d size
		//{
		//	//return min(((d3dGfx*)Gfx::current())->caps().MaxTextureWidth,((d3dGfx*)Gfx::current())->caps().MaxTextureHeight);
		//	return Gfx::current()->maxTextureSize();
		//}
		//GfxEnum::TEXTARGET		Tex2D::tex_target() const
		//{
		//	return GfxEnum::TEXTARGET_2D;
		//}
		//void					Tex2D::updateMipLevel(size_t level, const void* fip)
		//{
		//	GfxEnum::PIXELFORMAT pf = getPixelFormat();
		//	size_t dx = 0, dy = 0;
		//	getMipLevelDim(level, dx, dy);
		//	if(!dx || !dy)
		//		return;
		//	void* bits;
		//	size_t pitch;
		//	if(!lockMipLevel(level, bits, pitch))
		//		return;

		//	updateMipLevel(bits, pitch, pf, dx, dy, fip);

		//	unlockMipLevel(level);
		//}
		//bool					Tex2D::reload()
		//{
		//	return false;
		//}
		//SPtr<Tex2D>				Tex2D::create(size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//{
		//	if(getForce16Bit())
		//	{
		//		if(format == GfxEnum::PIXELFORMAT_R8G8B8)
		//			format = GfxEnum::PIXELFORMAT_R5G6B5;
		//		else if(format == GfxEnum::PIXELFORMAT_A8R8G8B8)
		//			format = GfxEnum::PIXELFORMAT_A4R4G4B4;
		//	}
		//	return Gfx::current()->createTex2D(dx, dy, levels, format, rendertarget);
		//}

		//SPtr<Tex2D>				Tex2D::load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	Gfx* gfx = Gfx::current();

		//	if((size_t)filepath.rfind('.') >= 0)
		//	{
		//		wcs ext = filepath.substr(filepath.rfind('.') + 1);
		//		ext = TOLOWER(ext);
		//		if(ext == L"avi")
		//			return Tex2DAVI::load(filepath, format, non_pow2);
		//		else if(ext == L"gif")
		//			return Tex2DGIF::load(filepath, mipmap, format, non_pow2);
		//	}

		//	ResMgr& mgr = gfx->getResMgrTex2D();

		//	Tex2D* tex = mgr.find<Tex2D>(filepath);
		//	if(tex)	return tex;

		//	SPtr<Tex2D> p = loadNoCache(filepath, mipmap, format, non_pow2);
		//	if(!p)
		//		return NULL;
		//	mgr.add(p.get());

		//	return p;
		//}
		//SPtr<Tex2D>				Tex2D::load_fip(void* fip, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	fipImage& img = *(fipImage*)fip;
		//	bool known_opaque = false;
		//	if(format != GfxEnum::PIXELFORMAT_A8R8G8B8 && format != GfxEnum::PIXELFORMAT_A4R4G4B4 && format != GfxEnum::PIXELFORMAT_A8)
		//		known_opaque = true;
		//	else
		//	{
		//		if(img.getBitsPerPixel() == 24)
		//			known_opaque = true;
		//		else if(img.getBitsPerPixel() == 32)
		//			known_opaque = testOpaque(&img);
		//		else if(img.getBitsPerPixel() == 8)
		//		{
		//			// no need to test A8, otherwise no reason to use A8
		//		}
		//	}
		//	size_t dx = (size_t)img.getWidth();
		//	size_t dy = (size_t)img.getHeight();
		//	if(!non_pow2)
		//	{
		//		dx = NEXT_POW2((uint32_t)dx);
		//		dy = NEXT_POW2((uint32_t)dy);
		//	}
		//	dx = min(dx, (size_t)Tex2D::getMaxRes());
		//	dy = min(dy, (size_t)Tex2D::getMaxRes());

		//	size_t levels = 1;
		//	if(mipmap)
		//	{
		//		size_t x = dx, y = dy;
		//		while(x > 1 || y > 1)
		//		{
		//			++levels;
		//			x >>= 1;
		//			y >>= 1;
		//		}
		//	}
		//	SPtr<Tex2D> tex = create(dx, dy, levels, format, false);
		//	if(!tex)
		//		return NULL;
		//	size_t miplevels = tex->getMipLevels();
		//	for(size_t i = 0; i < miplevels; ++i)
		//		tex->updateMipLevel(i, &img);
		//	tex->setKnownOpaque(known_opaque);
		//	return tex;
		//}
		//SPtr<Tex2D>				Tex2D::loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	Gfx* gfx = Gfx::current();

		//	if((size_t)filepath.rfind('.') >= 0)
		//	{
		//		wcs ext = filepath.substr(filepath.rfind('.') + 1);
		//		ext = TOLOWER(ext);
		//		if(ext == L"avi")
		//			return Tex2DAVI::loadNoCache(filepath, format, non_pow2);
		//		else if(ext == L"gif")
		//			return Tex2DGIF::loadNoCache(filepath, mipmap, format, non_pow2);
		//	}

		//	SMem sm;
		//	if(!FileSystem::loadFromAll(filepath, sm))
		//		return NULL;
		//	if(!sm.size())
		//		return NULL;

		//	fipMemoryIO memio((BYTE*)sm.get(), (DWORD)sm.size());
		//	fipImage img;
		//	BOOL ret = img.loadFromMemory(memio);

		//	sm.reset();

		//	if(!ret)
		//		return NULL;
		//	SPtr<Tex2D> p = load_fip(&img, mipmap, format, non_pow2);
		//	if(!p)
		//		return NULL;
		//	p->set_filepath(filepath);

		//	return p;
		//}
		//bool					Tex2D::testOpaque(const void* fip)
		//{
		//	fipImage& img = *(fipImage*)fip;
		//	if(img.getBitsPerPixel() != 32)
		//		return false;
		//	size_t pixels = img.getWidth() * img.getHeight() + 1;
		//	unsigned char* alpha = img.accessPixels() + 3 - 4;
		//	while(--pixels)
		//		if(*(alpha += 4) ^ 0xff)
		//			return false;
		//	return true;
		//}
		//bool					Tex2D::updateMipLevel(void* bits, size_t pitch, GfxEnum::PIXELFORMAT pf,size_t dx, size_t dy, const void* fip)
		//{
		//	if(pf != GfxEnum::PIXELFORMAT_R5G6B5 && pf != GfxEnum::PIXELFORMAT_A4R4G4B4 && pf != GfxEnum::PIXELFORMAT_R8G8B8 && pf != GfxEnum::PIXELFORMAT_A8R8G8B8 && pf != GfxEnum::PIXELFORMAT_A8 && pf != GfxEnum::PIXELFORMAT_L8)
		//		return false;
		//	if(!dx || !dy)
		//		return false;
		//	fipImage img = *(const fipImage*)fip;
		//	if(!img.rescale((WORD)dx, (WORD)dy, FILTER_BILINEAR))
		//		return false;
		//	switch(pf)
		//	{
		//	case GfxEnum::PIXELFORMAT_R5G6B5:
		//		img.convertTo16Bits565();
		//		break;
		//	case GfxEnum::PIXELFORMAT_R8G8B8:
		//		img.convertTo24Bits();
		//		break;
		//	case GfxEnum::PIXELFORMAT_X8R8G8B8:
		//	case GfxEnum::PIXELFORMAT_A8R8G8B8:
		//		img.convertTo32Bits();
		//		break;
		//	case GfxEnum::PIXELFORMAT_A8:
		//	case GfxEnum::PIXELFORMAT_L8:
		//		img.convertTo8Bits();
		//		break;
		//	}
		//	if(pf == GfxEnum::PIXELFORMAT_A4R4G4B4)
		//	{
		//		if(img.convertTo32Bits())
		//		{
		//			size_t width = (size_t)img.getWidth();
		//			size_t height = (size_t)img.getHeight();
		//			for(size_t i = 0; i < height; ++i)
		//			{
		//				unsigned long* src = (unsigned long*)img.getScanLine((WORD)i);
		//				unsigned short* dst = (unsigned short*)((char*)bits + i * pitch);

		//				unsigned long* src_end = src + width;
		//				for(; src != src_end; ++src, ++dst)
		//				{
		//					unsigned char* p = (unsigned char*)src;
		//					*dst = (p[0] >> 4) | (((unsigned short)(p[1] >> 4)) << 4) | (((unsigned short)(p[2] >> 4)) << 8) | (((unsigned short)(p[3] >> 4)) << 12);
		//				}
		//			}
		//		}
		//	}
		//	else
		//	{
		//		size_t height = (size_t)img.getHeight();
		//		size_t linebytes = (size_t)((img.getBitsPerPixel() * img.getWidth() + 7) >> 3);
		//		char* b = (char*)bits;
		//		char* src = (char*)img.accessPixels();
		//		size_t src_sw = (size_t)img.getScanWidth();
		//		for(size_t i = 0; i < height; ++i)
		//		{
		//			memcpy(b, src, linebytes);
		//			b += pitch;
		//			src += src_sw;
		//		}
		//	}
		//	return true;
		//}
	};
};

