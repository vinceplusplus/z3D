
#include "Renderbuffer.h"
#include "Gfx.h"

namespace z3D
{
	namespace Video
	{
		Renderbuffer::Renderbuffer() {}
		Renderbuffer::~Renderbuffer() {}

		SPtr<Renderbuffer>				Renderbuffer::createRenderTarget(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			return Gfx::current()->createRenderTargetRenderbuffer(dx, dy, format, multisample_type);
		}
		SPtr<Renderbuffer>				Renderbuffer::createDepthStencil(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			return Gfx::current()->createDepthStencilRenderbuffer(dx, dy, format, multisample_type);
		}
	};
};

