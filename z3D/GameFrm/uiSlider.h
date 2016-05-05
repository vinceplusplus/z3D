
#pragma once

#include "predecl.h"

#include "uiBase.h"
#include "uiButton.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiSlider: public uiButton
		{
		public:
			Vec2			_range_start;
			Vec2			_range_extent;
		private:
			Vec2			_last_pos;
			Vec2			_last_mouse_pos;
		private:
			wfunctor1<void, const SPtr<uiSlider>&>
							_cb_on_slider_slide;
		public:
			uiSlider(int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t range_dx, int32_t range_dy);
		protected:
			~uiSlider();
		public:
			void			setRange(int32_t x, int32_t y, int32_t dx, int32_t dy);
		protected:
			void			onMousePressed(const Vec2& pos, uint32_t button);
			void			onMouseReleased(const Vec2& pos, uint32_t button);
			void			onMouseMoved(const Vec2& pos);
		protected:
			void			onDrawUp();
		public:
			void			setOnSliderSlide(const wfunctor1<void, const SPtr<uiSlider>&>& cb);
		private:
			void			setOnButtonClick(const wfunctor1<void, const SPtr<uiButton>&>& cb);
		public:
			int32_t			getSlideValueX() const;
			int32_t			getSlideValueY() const;
		public:
			void			setSlideValueX(int32_t x);
			void			setSlideValueY(int32_t y);
		public:
			int32_t			getSlideRangeX() const;
			int32_t			getSlideRangeY() const;
		};
	};
};

