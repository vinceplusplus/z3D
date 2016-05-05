
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Video
	{
		class rsView
		{
		public:
			class RECT
			{
			public:
				int32_t				x;
				int32_t				y;
				int32_t				dx;
				int32_t				dy;
			};
		public:
			RECT					viewport;
			RECT					cliprect;
			REAL					minZ;
			REAL					maxZ;
		public:
			rsView();
		public:
			void					init(const SPtr<Gfx>& gfx);
		public:
			void					setViewport(int32_t x, int32_t y, int32_t dx, int32_t dy);
			void					setClipRect(int32_t x, int32_t y, int32_t dx, int32_t dy);
		public:
			void					intersectViewport(int32_t x, int32_t y, int32_t dx, int32_t dy);
			void					intersectClipRect(int32_t x, int32_t y, int32_t dx, int32_t dy);
		public:
			void					setDepthRange(REAL minZ, REAL maxZ);
		};
	};
};

