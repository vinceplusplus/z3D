
#pragma once

#include "predecl.h"
#include "TexBase.h"

namespace z3D
{
	namespace Video
	{
		class Tex2D: public TexBase
		{
		public:
			DECL_TYPEINFO();
		protected:
											Tex2D();
											~Tex2D();
		public:
			GfxEnum::TEXTARGET				getTarget() const;
		public:
			virtual const Tex2DCreateParams&
											getCreateParams() const = 0;
		public:
			virtual bool					getMipLevelDim(size_t level, size_t& dx, size_t& dy) const = 0;
		public:
			virtual bool					lockMipLevel(size_t level, void*& bits, size_t& pitch, const LockParams& lp) = 0;
			virtual void					unlockMipLevel(size_t level) = 0;
		};

		class Tex2DCreateParams
		{
		public:
			size_t					dx;
			size_t					dy;
			size_t					level_count;
			bool					auto_gen_mipmap;
			bool					depth_stencil;
			bool					dynamic;
			bool					render_target;
			GfxEnum::PIXELFORMAT	format;
			GfxEnum::POOL			pool;
		public:
			Tex2DCreateParams(size_t dx_, size_t dy_, size_t level_count_, GfxEnum::PIXELFORMAT format_, GfxEnum::POOL pool_);
			Tex2DCreateParams(const Tex2DCreateParams& rhs);
			Tex2DCreateParams&		operator =(const Tex2DCreateParams& rhs);
		public:
			Tex2DCreateParams&		setUsageAutoGenMipmap();
			Tex2DCreateParams&		setUsageDepthStencil();
			Tex2DCreateParams&		setUsageDynamic();
			Tex2DCreateParams&		setUsageRenderTarget();
		};


		//class Tex2D: public TexBase
		//{
		//public:
		//	DECL_TYPEINFO();
		//protected:
		//	static unsigned int				MAX_RES;
		//	static bool						FORCE_16BIT;
		//protected:
		//	//static ResMgr					_res_mgr;
		//protected:
		//									Tex2D();
		//	virtual							~Tex2D();
		//public:
		//	static void						setMaxRes(unsigned int max_res);
		//	static unsigned int				getMaxRes();
		//	static void						setForce16Bit(bool force);
		//	static bool						getForce16Bit();
		//public:
		//	static unsigned int				getMaxSize();							// hardware maximum texture 2d size
		//public:
		//	GfxEnum::TEXTARGET				tex_target() const;
		//public:
		//	virtual GfxEnum::PIXELFORMAT	getPixelFormat() const = 0;
		//public:
		//	virtual size_t					getDX() const = 0;
		//	virtual size_t					getDY() const = 0;
		//public:
		//	virtual size_t					getMipLevels() const = 0;
		//	virtual bool					getMipLevelDim(size_t level, size_t& dx, size_t& dy) const = 0;
		//public:
		//	virtual bool					lockMipLevel(size_t level, void*& bits, size_t& pitch) = 0;
		//	virtual void					unlockMipLevel(size_t level) = 0;
		//public:
		//	virtual void					updateMipLevel(size_t level, const void* fip);
		//public:
		//	virtual bool					reload();
		//public:
		//	static SPtr<Tex2D>				create(size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
		//	static SPtr<Tex2D>				load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//	static SPtr<Tex2D>				load_fip(void* fip, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//public:
		//	static SPtr<Tex2D>				loadNoCache(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//public:
		//	static bool						testOpaque(const void* fip);			// assuming 32-bit
		//public:
		//	static bool						updateMipLevel(void* bits, size_t pitch, GfxEnum::PIXELFORMAT pf, size_t dx, size_t dy, const void* fip);
		//};
	};
};

