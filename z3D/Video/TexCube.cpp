
#include "TexCube.h"
//#include "Tex2D.h"

//#include "../../FreeImage/Header.h"

namespace z3D
{
	namespace Video
	{
		TexCube::TexCube()
		{
		}
		TexCube::~TexCube()
		{
		}
		GfxEnum::TEXTARGET		TexCube::getTarget() const
		{
			return GfxEnum::TEXTARGET_CUBE;
		}

		TexCubeCreateParams::TexCubeCreateParams(size_t length_, size_t level_count_, GfxEnum::PIXELFORMAT format_, GfxEnum::POOL pool_)
			: length(length_), level_count(level_count_), format(format_), pool(pool_)
		{
			auto_gen_mipmap = false;
			depth_stencil = false;
			dynamic = false;
			render_target = false;
		}
		TexCubeCreateParams::TexCubeCreateParams(const TexCubeCreateParams& rhs)
		{
			*this = rhs;
		}
		TexCubeCreateParams&		TexCubeCreateParams::operator =(const TexCubeCreateParams& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));
			return *this;
		}
		TexCubeCreateParams&		TexCubeCreateParams::setUsageAutoGenMipmap()
		{
			auto_gen_mipmap = true;
			return *this;
		}
		TexCubeCreateParams&		TexCubeCreateParams::setUsageDepthStencil()
		{
			depth_stencil = true;
			return *this;
		}
		TexCubeCreateParams&		TexCubeCreateParams::setUsageDynamic()
		{
			dynamic = true;
			return *this;
		}
		TexCubeCreateParams&		TexCubeCreateParams::setUsageRenderTarget()
		{
			render_target = true;
			return *this;
		}

		//TexCube::TexCube():TexBase(L"")
		//{
		//}
		//TexCube::~TexCube()
		//{
		//}
		//void				TexCube::setMaxRes(unsigned int max_res)
		//{
		//	MAX_RES = max_res;
		//}
		//unsigned int		TexCube::getMaxRes()
		//{
		//	return MAX_RES;
		//}
		//void				TexCube::setForce16Bit(bool force)
		//{
		//	FORCE_16BIT = force;
		//}
		//bool				TexCube::getForce16Bit()
		//{
		//	return FORCE_16BIT;
		//}
		//unsigned int		TexCube::getMaxSize()
		//{
		//	return 0;
		//}
		//GfxEnum::TEXTARGET	TexCube::tex_target() const
		//{
		//	return GfxEnum::TEXTARGET_CUBE;
		//}
		//void				TexCube::updateMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level,const void* fip)
		//{
		//	GfxEnum::PIXELFORMAT pf=getPixelFormat();
		//	size_t length;
		//	getMipLevelDim(level,length);
		//	void* bits;
		//	size_t pitch;
		//	bool need_mirror, need_flip;
		//	if(!lockMipLevel(face, level, bits, pitch, need_mirror, need_flip))
		//		return;
		//	if(need_mirror || need_flip)
		//	{
		//		fipImage img = *(fipImage*)fip;						// clone
		//		if(need_mirror)
		//			img.flipHorizontal();
		//		if(need_flip)
		//			img.flipVertical();
		//		Tex2D::updateMipLevel(bits, pitch, pf, length, length, (const void*)&img);
		//	}
		//	else
		//		Tex2D::updateMipLevel(bits, pitch, pf, length, length, fip);
		//	unlockMipLevel(face, level);
		//}
		//SPtr<TexCube>		TexCube::create(Gfx* gfx, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		//{
		//	return gfx->createTexCube(length, levels, format, rendertarget);
		//}
		//SPtr<TexCube>		TexCube::load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	Gfx* gfx = Gfx::current();
		//	Z_ASSERT( gfx );
		//	ResMgr& mgr = gfx->getResMgrTexCube();
		//	TexCube* tex = mgr.find<TexCube>(filepath);
		//	if(tex)	return tex;

		//	SMem sm;
		//	if(!FileSystem::loadFromAll(filepath, sm))
		//		return NULL;
		//	if(!sm.size())
		//		return NULL;

		//	size_t length, bpp, mips;
		//	vector<void*> pos_x, neg_x, pos_y, neg_y, pos_z, neg_z;
		//	bool dword_aligned;
		//	if(!TexCube::loadUncompressedDDSCube(sm.get(), sm.size(), length, bpp, mips, pos_x, neg_x, pos_y, neg_y, pos_z, neg_z, dword_aligned))
		//		return NULL;

		//	SPtr<TexCube> p = create(gfx, length, mips, format, false);
		//	if(!p)
		//		return NULL;
		//	//GfxEnum::PIXELFORMAT pf = p->getPixelFormat();

		//	fipImage img;
		//	for(size_t i = 0; i < mips; ++i)
		//	{
		//		size_t length = 0;
		//		p->getMipLevelDim(i, length);
		//		if(!length)
		//			continue;

		//		img.setSize(FIT_BITMAP, (WORD)length, (WORD)length, (WORD)bpp);

		//		// positive x
		//		if(dword_aligned)
		//			memcpy(img.accessPixels(), pos_x[i], img.getScanWidth() * img.getHeight());
		//		else
		//			fill(&img, pos_x[i]);
		//		p->updateMipLevel(GfxEnum::CUBEMAP_FACE_POSITIVE_X, i, &img);

		//		// negative x
		//		if(dword_aligned)
		//			memcpy(img.accessPixels(), neg_x[i], img.getScanWidth() * img.getHeight());
		//		else
		//			fill(&img, neg_x[i]);
		//		p->updateMipLevel(GfxEnum::CUBEMAP_FACE_NEGATIVE_X, i, &img);

		//		// positive y
		//		if(dword_aligned)
		//			memcpy(img.accessPixels(), pos_y[i], img.getScanWidth() * img.getHeight());
		//		else
		//			fill(&img, pos_y[i]);
		//		p->updateMipLevel(GfxEnum::CUBEMAP_FACE_POSITIVE_Y, i, &img);

		//		// negative y
		//		if(dword_aligned)
		//			memcpy(img.accessPixels(), neg_y[i],img.getScanWidth() * img.getHeight());
		//		else
		//			fill(&img, neg_y[i]);
		//		p->updateMipLevel(GfxEnum::CUBEMAP_FACE_NEGATIVE_Y, i, &img);

		//		// positive z
		//		if(dword_aligned)
		//			memcpy(img.accessPixels(), pos_z[i], img.getScanWidth() * img.getHeight());
		//		else
		//			fill(&img, pos_z[i]);
		//		p->updateMipLevel(GfxEnum::CUBEMAP_FACE_POSITIVE_Z, i, &img);

		//		// negative z
		//		if(dword_aligned)
		//			memcpy(img.accessPixels(), neg_z[i], img.getScanWidth() * img.getHeight());
		//		else
		//			fill(&img, neg_z[i]);
		//		p->updateMipLevel(GfxEnum::CUBEMAP_FACE_NEGATIVE_Z, i, &img);
		//	}

		//	sm.reset();

		//	p->set_filepath(filepath);
		//	mgr.add(p.get());

		//	return p;
		//}
	};
};

