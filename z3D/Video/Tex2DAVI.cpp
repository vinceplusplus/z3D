
#include "Tex2DAVI.h"
#include "AVIReader.h"
#include "AVIRenderer.h"
#include "../../FreeImage/Header.h"

namespace z3D
{
	namespace Core
	{
		//template<>
		//SPtr<z3D::Video::Tex2D>		AnimKey< SPtr<z3D::Video::Tex2D> >::lerp(const AnimKey< SPtr<z3D::Video::Tex2D> >& other, REAL now) const {return this->key;}
	};
	namespace Video
	{
		//Tex2DAVICF::Tex2DAVICF(const wcs& filepath): ResBase(filepath)
		//{
		//	data = new AnimData< SPtr<Tex2D> >();

		//	loops = -1;
		//}
		//Tex2DAVICF::~Tex2DAVICF()
		//{
		//}
		//SPtr<Tex2DAVICF>				Tex2DAVICF::load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	Gfx* gfx = Gfx::current();

		//	Z_ASSERT( gfx );

		//	ResMgr& mgr = gfx->getResMgrTex2DAVICF();

		//	if(true)
		//	{
		//		Tex2DAVICF* cache = (Tex2DAVICF*)mgr.find(filepath);
		//		if(cache)
		//			return SPtr<Tex2DAVICF>(cache);
		//	}

		//	SPtr<Tex2DAVICF> cache = loadNoCache(filepath, mipmap, format, non_pow2, false);
		//	if(!cache)
		//		return NULL;

		//	mgr.add(cache.get());

		//	return cache;
		//}
		//SPtr<Tex2DAVICF>				Tex2DAVICF::loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2, bool no_cache_avi)
		//{
		//	Gfx* gfx = Gfx::current();

		//	Z_ASSERT( gfx );

		//	AVIRenderer* renderer = NULL;
		//	if(no_cache_avi)
		//		renderer = AVIRenderer::loadNoCache(filepath);
		//	else
		//		renderer = AVIRenderer::load(filepath);

		//	if(!renderer)
		//		return NULL;

		//	if(renderer->reader().frames() == 0)
		//	{
		//		delete renderer;
		//		return NULL;
		//	}

		//	int dx = (int)renderer->dx();
		//	int dy = (int)renderer->dy();
		//	if(!non_pow2)
		//	{
		//		dx = NEXT_POW2(dx);
		//		dy = NEXT_POW2(dy);
		//	}
		//	dx = max(4, min(dx, (int)Tex2DAVI::getMaxRes()));
		//	dy = max(4, min(dy, (int)Tex2DAVI::getMaxRes()));

		//	int levels = 1;
		//	if(mipmap)
		//	{
		//		int x = dx, y = dy;
		//		while(x > 1 || y > 1)
		//		{
		//			++levels;
		//			x >>= 1;
		//			y >>= 1;
		//		}
		//	}

		//	Tex2DAVICF* cache = new Tex2DAVICF(filepath);
		//	fipImage fip;
		//	fip.setSize(FIT_BITMAP, (WORD)dx, (WORD)dy, (WORD)((BITMAPINFO*)renderer->reader().video_format())->bmiHeader.biBitCount);

		//	REAL end_time = 0.0f;
		//	SPtr<Tex2D> last_frame;
		//	for(int i = 0; i < (int)renderer->reader().frames(); ++i)
		//	{
		//		SPtr<Tex2D> img;
		//		if(renderer->reader().get_frame_size(i))
		//		{
		//			renderer->render(i);
		//			img=Tex2D::create(dx, dy, levels, format, false);
		//			for(int j = 0; j < levels; ++j)
		//				img->updateMipLevel(j, renderer->get_fipImage());
		//		}
		//		else
		//		{
		//			if(last_frame)
		//				img = last_frame;
		//		}
		//		if(img)
		//		{
		//			cache->data->addKey(end_time, img);
		//			end_time += 1.0f / renderer->reader().frame_rate();
		//			last_frame = img;
		//		}
		//	}

		//	delete renderer;

		//	if(!cache->data->getKeyCount())
		//	{
		//		delete cache;
		//		return NULL;
		//	}

		//	if(cache->data->getKeyCount())
		//		cache->data->addKey(end_time, cache->data->getKey(cache->data->getKeyCount() - 1).key);

		//	cache->set_filepath(filepath);

		//	return SPtr<Tex2DAVICF>(cache);
		//}

		//Tex2DAVI::Tex2DAVI(const SPtr<Tex2D>& tex2d,AVIRenderer* renderer): _tex2d(tex2d), _renderer(renderer), _cf(NULL)
		//{
		//}
		//Tex2DAVI::~Tex2DAVI()
		//{
		//	if(_renderer)
		//		delete _renderer;
		//}
		//SPtr<Tex2DAVI>			Tex2DAVI::load(const wcs& filepath, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	if(isCacheFramesAVI(filepath))
		//	{
		//		SPtr<Tex2DAVICF> cf = Tex2DAVICF::load(filepath, true, format, non_pow2);
		//		if(!cf)
		//			return NULL;
		//		Tex2DAVI* tex = new Tex2DAVI(cf->data->getKey(0).key, NULL);
		//		tex->_cf_provider = cf;
		//		tex->_cf = cf->data;;
		//		tex->set(0,	cf->data->getKey(cf->data->getKeyCount() - 1).time, cf->loops);
		//		tex->autoAdvance(true);

		//		return SPtr<Tex2DAVI>(tex);
		//	}
		//	AVIRenderer* renderer = AVIRenderer::load(filepath);
		//	if(!renderer)
		//		return NULL;
		//	SPtr<Tex2DAVI> tex2davi = load(renderer, format, non_pow2);
		//	if(!tex2davi)
		//		delete renderer;
		//	return tex2davi;
		//}
		//SPtr<Tex2DAVI>			Tex2DAVI::load(AVIRenderer* renderer, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	if(!renderer)
		//		return NULL;
		//	int dx = (int)renderer->dx();
		//	int dy = (int)renderer->dy();
		//	if(!non_pow2)
		//	{
		//		dx = NEXT_POW2(dx);
		//		dy = NEXT_POW2(dy);
		//	}
		//	dx = min(dx, (int)Tex2DAVI::getMaxRes());
		//	dy = min(dy, (int)Tex2DAVI::getMaxRes());

		//	SPtr<Tex2D> tex2d = Tex2D::create(dx, dy, 1, format, false);
		//	if(!tex2d)
		//		return NULL;
		//	SPtr<Tex2DAVI> tex2davi = new Tex2DAVI(tex2d, renderer);
		//	tex2davi->set(0, renderer->length(), -1);
		//	tex2davi->autoAdvance(true);
		//	tex2davi->update();

		//	return tex2davi;
		//}
		//SPtr<Tex2DAVI>			Tex2DAVI::loadNoCache(const wcs& filepath, GfxEnum::PIXELFORMAT format, bool non_pow2)
		//{
		//	if(isCacheFramesAVI(filepath))
		//	{
		//		SPtr<Tex2DAVICF> cf = Tex2DAVICF::loadNoCache(filepath, true, format, non_pow2, true);
		//		if(!cf)
		//			return NULL;
		//		Tex2DAVI* tex = new Tex2DAVI(cf->data->getKey(0).key, NULL);
		//		tex->_cf_provider = cf;
		//		tex->_cf = cf->data;
		//		tex->set(0,	cf->data->getKey(cf->data->getKeyCount() - 1).time, cf->loops);
		//		tex->autoAdvance(true);

		//		return SPtr<Tex2DAVI>(tex);
		//	}
		//	AVIRenderer* renderer = AVIRenderer::loadNoCache(filepath);
		//	if(!renderer)
		//		return NULL;
		//	SPtr<Tex2DAVI> tex2davi = load(renderer, format, non_pow2);
		//	if(!tex2davi)
		//		delete renderer;
		//	return tex2davi;
		//}
		//void					Tex2DAVI::addCacheFramesAVI(const wcs& filepath)
		//{
		//	_map_cache_frames_filepath[filepath] = true;
		//}
		//void					Tex2DAVI::removeCacheFramesAVI(const wcs& filepath)
		//{
		//	_map_cache_frames_filepath.erase(filepath);
		//}
		//bool					Tex2DAVI::isCacheFramesAVI(const wcs& filepath)
		//{
		//	return !!_map_cache_frames_filepath.count(filepath);
		//}
		//vector<wcs>				Tex2DAVI::getCacheFramesAVIs()
		//{
		//	vector<wcs> array;
		//	map<wcs, bool>::iterator iter;
		//	for(iter = _map_cache_frames_filepath.begin(); iter != _map_cache_frames_filepath.end(); ++iter)
		//		array.push_back(iter->first);
		//	return array;
		//}
		//void					Tex2DAVI::setMaxRes(unsigned int max_res)
		//{
		//	MAX_RES = max_res;
		//}
		//unsigned int			Tex2DAVI::getMaxRes()
		//{
		//	return MAX_RES;
		//}
		//void					Tex2DAVI::setForce16Bit(bool force)
		//{
		//	FORCE_16BIT = force;
		//}
		//bool					Tex2DAVI::getForce16Bit()
		//{
		//	return FORCE_16BIT;
		//}
		//GfxEnum::PIXELFORMAT		Tex2DAVI::getPixelFormat() const
		//{
		//	return _tex2d->getPixelFormat();
		//}
		//size_t					Tex2DAVI::getDX() const
		//{
		//	return _tex2d->getDX();
		//}
		//size_t					Tex2DAVI::getDY() const
		//{
		//	return _tex2d->getDY();
		//}
		//size_t					Tex2DAVI::getMipLevels() const
		//{
		//	return _tex2d->getMipLevels();
		//}
		//bool					Tex2DAVI::getMipLevelDim(size_t level, size_t& dx, size_t& dy) const
		//{
		//	return _tex2d->getMipLevelDim(level,dx,dy);
		//}
		//bool					Tex2DAVI::lockMipLevel(size_t level, void*& bits, size_t& pitch)
		//{
		//	return _tex2d->lockMipLevel(level,bits,pitch);
		//}
		//void					Tex2DAVI::unlockMipLevel(size_t level)
		//{
		//	_tex2d->unlockMipLevel(level);
		//}
		//void					Tex2DAVI::updateMipLevel(size_t level,const void* fip)
		//{
		//	_tex2d->updateMipLevel(level,fip);
		//}
		//void					Tex2DAVI::update()
		//{
		//	if(need_update())
		//		AnimInst::update();
		//	if(_renderer)
		//	{
		//		if(_renderer->renderAt(getTime()))
		//			updateMipLevel(0, _renderer->get_fipImage());
		//	}
		//	else
		//	{
		//		_tex2d = _cf.eval_lerp(getTime());
		//	}
		//}
		//SPtr<TexBase>			Tex2DAVI::getRealTexture()
		//{
		//	if(autoAdvanced())
		//		update();

		//	return _tex2d;
		//}
		//AVIRenderer*			Tex2DAVI::renderer()
		//{
		//	return _renderer;
		//}
	};
};

