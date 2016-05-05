
#pragma once

#include "predecl.h"
#include "TexBase.h"

namespace z3D
{
	namespace Video
	{
		class TexCube: public TexBase
		{
		public:
			DECL_TYPEINFO();
		protected:
											TexCube();
											~TexCube();
		public:
			GfxEnum::TEXTARGET				getTarget() const;
		public:
			virtual const TexCubeCreateParams&
											getCreateParams() const = 0;
		public:
			virtual bool					getMipLevelDim(size_t level, size_t& length) const = 0;
		public:
			virtual bool					lockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level, void*& bits, size_t& pitch, const LockParams& lp) = 0;
			virtual void					unlockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level) = 0;
		};

		class TexCubeCreateParams
		{
		public:
			size_t					length;
			size_t					level_count;
			bool					auto_gen_mipmap;
			bool					depth_stencil;
			bool					dynamic;
			bool					render_target;
			GfxEnum::PIXELFORMAT	format;
			GfxEnum::POOL			pool;
		public:
			TexCubeCreateParams(size_t length_, size_t level_count_, GfxEnum::PIXELFORMAT format_, GfxEnum::POOL pool_);
			TexCubeCreateParams(const TexCubeCreateParams& rhs);
			TexCubeCreateParams&	operator =(const TexCubeCreateParams& rhs);
		public:
			TexCubeCreateParams&	setUsageAutoGenMipmap();
			TexCubeCreateParams&	setUsageDepthStencil();
			TexCubeCreateParams&	setUsageDynamic();
			TexCubeCreateParams&	setUsageRenderTarget();
		};

		//class TexCube: public TexBase
		//{
		//public:
		//	DECL_TYPEINFO();
		//protected:
		//	static unsigned int				MAX_RES;
		//	static bool						FORCE_16BIT;
		//protected:
		//									TexCube();
		//	virtual							~TexCube();
		//public:
		//	static void						setMaxRes(unsigned int max_res);
		//	static unsigned int				getMaxRes();
		//	static void						setForce16Bit(bool force);
		//	static bool						getForce16Bit();
		//public:
		//	static unsigned int				getMaxSize();							// hardware maximum texture cube size
		//public:
		//	GfxEnum::TEXTARGET				tex_target() const;
		//public:
		//	virtual GfxEnum::PIXELFORMAT	getPixelFormat() const = 0;
		//public:
		//	virtual size_t					getMipLevels() const = 0;
		//	virtual bool					getMipLevelDim(size_t level, size_t& length) const = 0;
		//public:
		//	virtual bool					lockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level, void*& bits, size_t& pitch, bool& need_mirror, bool& need_flip) = 0;
		//	virtual void					unlockMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level) = 0;
		//public:
		//	virtual void					updateMipLevel(GfxEnum::CUBEMAP_FACE face, size_t level,const void* fip);
		//public:
		//	static SPtr<TexCube>			create(Gfx* gfx, size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
		//	static SPtr<TexCube>			load(const wcs& filepath, bool mipmap, GfxEnum::PIXELFORMAT format, bool non_pow2);
		//public:
		//	static bool						loadUncompressedDDSCube(const void* src, size_t src_len, size_t& length, size_t& bpp, size_t& mips, vector<void*>& rh_pos_x, vector<void*>& rh_neg_x, vector<void*>& rh_pos_y, vector<void*>& rh_neg_y, vector<void*>& rh_pos_z, vector<void*>& rh_neg_z, bool& dword_aligned);				// the dds needs to be conformed to left hand rule, the function will convert it to right hand rule
		//	static void						fill(void* fipimage, const void* unaligned);
		//};
	};
};

