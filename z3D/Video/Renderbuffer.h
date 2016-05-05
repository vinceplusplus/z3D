
#pragma once

#include "predecl.h"

#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class Renderbuffer: public RefBase
		{
		public:
			DECL_TYPEINFO();
		protected:
			Renderbuffer();
			~Renderbuffer();
		public:
			static SPtr<Renderbuffer>			createRenderTarget(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
			static SPtr<Renderbuffer>			createDepthStencil(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
		public:
			virtual bool						isRenderTarget() const = 0;
			virtual bool						isDepthStencil() const = 0;
		public:
			virtual GfxEnum::PIXELFORMAT		getPixelFormat() const = 0;
		public:
			virtual size_t						getMultiSampleType() const = 0;
		public:
			virtual size_t						getDX() const = 0;
			virtual size_t						getDY() const = 0;
		};
	};
};

