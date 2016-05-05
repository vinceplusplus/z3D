
#pragma once

#include "rsView.h"
#include "Gfx.h"

namespace z3D
{
	namespace Video
	{
		rsView::rsView()
		{
			viewport.x = viewport.y = 0;
			viewport.dx = viewport.dy = 1;

			cliprect.x = cliprect.y = 0;
			cliprect.dx = cliprect.dy = 1;

			minZ = 0.0f;
			maxZ = 1.0f;
		}
		void					rsView::init(const SPtr<Gfx>& gfx)
		{
			viewport.x = viewport.y = 0;
			viewport.dx = (int32_t)gfx->getFramebufferDX();
			viewport.dy = (int32_t)gfx->getFramebufferDY();

			// test viewport
			//viewport.x += 50;
			//viewport.y += 50;
			//viewport.dx -= 100;
			//viewport.dy -= 100;

			minZ = 0.0f;
			maxZ = 1.0f;

			cliprect = viewport;

			// test viewport
			//cliprect.x = cliprect.y = 0;
		}
		void					rsView::setViewport(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			viewport.x = x;
			viewport.y = y;
			viewport.dx = dx;
			viewport.dy = dy;
		}
		void					rsView::setClipRect(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			cliprect.x = x;
			cliprect.y = y;
			cliprect.dx = dx;
			cliprect.dy = dy;
		}
		void					rsView::intersectViewport(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			intersect_area(viewport.x, viewport.y, viewport.dx, viewport.dy, x, y, dx, dy);
		}
		void					rsView::intersectClipRect(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			intersect_area(cliprect.x, cliprect.y, cliprect.dx, cliprect.dy, x, y, dx, dy);
		}
		void					rsView::setDepthRange(REAL minZ, REAL maxZ)
		{
			this->minZ = minZ;
			this->maxZ = maxZ;
		}
	};
};

