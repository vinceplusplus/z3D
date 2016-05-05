
#include "Gfx.h"

#if defined(Z3D_ENABLE_VIDEO_D3D)
#include "d3dGfx.h"
#endif

namespace z3D
{
	namespace Video
	{
		Gfx::~Gfx() {}
#if defined(Z3D_ENABLE_VIDEO_D3D)
		SPtr<Gfx>			Gfx::createD3D(intptr_t hwnd, bool vsync)
		{
			return d3dGfx::create((HWND)hwnd, vsync);
		}
#endif

		LockParams::LockParams()
			: discard(false)
		{}
		LockParams::LockParams(const LockParams& rhs)
		{
			*this = rhs;
		}
		LockParams&				LockParams::operator =(const LockParams& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));
			return *this;
		}
		void					LockParams::setDiscard()
		{
			discard = true;
		}

		void					GfxHelper::copy2D(const void* src, void* dst, size_t copy_length_per_line, size_t height, size_t src_pitch, size_t dst_pitch)
		{
			const char* s = (const char*)src;
			char* d = (char*)dst;
			for(size_t i = 0; i < height; ++i)
			{
				memcpy(d, s, copy_length_per_line);
				s += src_pitch;
				d += dst_pitch;
			}
		}
	};
};

