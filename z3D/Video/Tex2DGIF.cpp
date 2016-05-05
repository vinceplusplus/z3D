
#include "Tex2DGIF.h"
#include "../../FreeImage/Header.h"

namespace z3D
{
	namespace Video
	{
		//Tex2DGIFCF::Tex2DGIFCF(const wcs& filepath): ResBase(filepath)
		//{
		//	data = new AnimData< SPtr<Tex2D> >();

		//	loops = -1;
		//}
		//Tex2DGIFCF::~Tex2DGIFCF()
		//{
		//}
		//SPtr<Tex2DGIFCF>				Tex2DGIFCF::load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	Gfx* gfx = Gfx::current();

		//	Z_ASSERT( gfx );

		//	ResMgr& mgr = gfx->getResMgrTex2DAVICF();

		//	if(true)
		//	{
		//		Tex2DGIFCF* cache = (Tex2DGIFCF*)mgr.find(filepath);
		//		if(cache)
		//			return SPtr<Tex2DGIFCF>(cache);
		//	}

		//	SPtr<Tex2DGIFCF> cache = loadNoCache(filepath, mipmap, format, non_pow2);

		//	mgr.add(cache.get());

		//	return cache;
		//}

		//SPtr<Tex2DGIFCF>				Tex2DGIFCF::loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	Gfx* gfx = Gfx::current();

		//	Z_ASSERT( gfx );

		//	SMem sm;
		//	if(!FileSystem::loadFromAll(filepath, sm))
		//		return NULL;

		//	Tex2DGIFCF* cache = new Tex2DGIFCF(filepath);
		//	int end_time = 0;

		//	do
		//	{
		//		fipMemoryIO memio((BYTE*)sm.get(), (DWORD)sm.size());
		//		fipMultiPage multipage;
		//		if(!multipage.open(memio, GIF_PLAYBACK))
		//			break;

		//		int pagecount = multipage.getPageCount();

		//		for(int i = 0; i < pagecount; ++i)
		//		{
		//			fipImage bmp;
		//			bmp = multipage.lockPage(i);

		//			fipImage fip = bmp;

		//			int delay = 100;
		//			fipTag tag;
		//			if(fip.getMetadata(FIMD_ANIMATION, "FrameTime", tag))
		//				delay = *(int*)tag.getValue();
		//			delay = max(1, delay);
		//			SPtr<Tex2D> img = Tex2D::load_fip(&fip, mipmap, format, non_pow2);
		//			if(img)
		//			{
		//				cache->data->addKey(end_time / 1000.0f, img);
		//				end_time += delay;
		//			}
		//			if(i == 0)
		//			{
		//				fipTag tag;
		//				if(fip.getMetadata(FIMD_ANIMATION, "Loop", tag))
		//				{
		//					int loops = *(int*)tag.getValue();
		//					cache->loops = loops - 1;
		//				}
		//			}
		//			multipage.unlockPage(bmp, false);
		//		}
		//	}while(false);

		//	sm.reset();

		//	if(!cache->data->getKeyCount())
		//	{
		//		delete cache;
		//		return NULL;
		//	}

		//	if(cache->data->getKeyCount())
		//		cache->data->addKey(end_time / 1000.0f, cache->data->getKey(cache->data->getKeyCount() - 1).key);

		//	cache->set_filepath(filepath);

		//	return SPtr<Tex2DGIFCF>(cache);
		//}

		//Tex2DGIF::Tex2DGIF(const SPtr<Tex2D>& tex2d): _tex2d(tex2d), _cf(NULL)
		//{
		//}

		//Tex2DGIF::~Tex2DGIF()
		//{
		//}

		//SPtr<Tex2DGIF>			Tex2DGIF::load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	SPtr<Tex2DGIFCF> cf = Tex2DGIFCF::load(filepath, mipmap, format, non_pow2);
		//	if(!cf)
		//		return NULL;

		//	Tex2DGIF* tex = new Tex2DGIF(cf->data->getKey(0).key);
		//	tex->_cf_provider = cf;
		//	tex->_cf = cf->data;
		//	tex->set(0, cf->data->getKey(cf->data->getKeyCount() - 1).time, cf->loops);
		//	tex->autoAdvance(true);
		//	tex->update();

		//	return SPtr<Tex2DGIF>(tex);
		//}

		//SPtr<Tex2DGIF>			Tex2DGIF::loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	SPtr<Tex2DGIFCF> cf = Tex2DGIFCF::loadNoCache(filepath, mipmap, format, non_pow2);
		//	if(!cf)
		//		return NULL;

		//	Tex2DGIF* tex = new Tex2DGIF(cf->data->getKey(0).key);
		//	tex->_cf_provider = cf;
		//	tex->_cf = cf->data;
		//	tex->set(0, cf->data->getKey(cf->data->getKeyCount() - 1).time, cf->loops);
		//	tex->autoAdvance(true);
		//	tex->update();

		//	return SPtr<Tex2DGIF>(tex);
		//}

		//void					Tex2DGIF::setMaxRes(unsigned int max_res)
		//{
		//	MAX_RES = max_res;
		//}

		//unsigned int			Tex2DGIF::getMaxRes()
		//{
		//	return MAX_RES;
		//}

		//void					Tex2DGIF::setForce16Bit(bool force)
		//{
		//	FORCE_16BIT = force;
		//}

		//bool					Tex2DGIF::getForce16Bit()
		//{
		//	return FORCE_16BIT;
		//}

		//GfxEnum::PIXELFORMAT	Tex2DGIF::getPixelFormat() const
		//{
		//	return _tex2d->getPixelFormat();
		//}

		//size_t					Tex2DGIF::getDX() const
		//{
		//	return _tex2d->getDX();
		//}
		//size_t					Tex2DGIF::getDY() const
		//{
		//	return _tex2d->getDY();
		//}

		//size_t					Tex2DGIF::getMipLevels() const
		//{
		//	return _tex2d->getMipLevels();
		//}

		//bool					Tex2DGIF::getMipLevelDim(size_t level, size_t& dx, size_t& dy) const
		//{
		//	return _tex2d->getMipLevelDim(level, dx, dy);
		//}

		//bool					Tex2DGIF::lockMipLevel(size_t level, void*& bits, size_t& pitch)
		//{
		//	return _tex2d->lockMipLevel(level, bits, pitch);
		//}

		//void					Tex2DGIF::unlockMipLevel(size_t level)
		//{
		//	_tex2d->unlockMipLevel(level);
		//}

		//void					Tex2DGIF::updateMipLevel(size_t level,const void* fip)
		//{
		//	_tex2d->updateMipLevel(level, fip);
		//}

		//void					Tex2DGIF::update()
		//{
		//	if(need_update())
		//		AnimInst::update();
		//	_tex2d = _cf.eval_lerp(getTime());
		//}
		//SPtr<TexBase>			Tex2DGIF::getRealTexture()
		//{
		//	if(autoAdvanced())
		//		update();

		//	return _tex2d;
		//}
	};
};

