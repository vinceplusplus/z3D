
#include "uiSlider.h"

#include "uiMgr.h"

#include "../Core/functor_weak_inl.h"

namespace z3D
{
	namespace GameFrm
	{
		uiSlider::uiSlider(int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t range_dx, int32_t range_dy)
		{
			setPos(x, y);
			setExt(dx, dy);
			setRange(x, y, range_dx, range_dy);
		}
		uiSlider::~uiSlider()
		{}
		void			uiSlider::setRange(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			_range_start = Vec2((REAL)x, (REAL)y);
			_range_extent = Vec2((REAL)dx, (REAL)dy);
		}
		void			uiSlider::onMousePressed(const Vec2& pos, uint32_t button)
		{
			this->uiButton::onMousePressed(pos, button);
			if(button == 0)
			{
				_last_pos = getAbsPos();
				_last_mouse_pos = getAbsPos() + pos;
				uiMgr::current()->setCapture(this);
			}
		}
		void			uiSlider::onMouseReleased(const Vec2& pos, uint32_t button)
		{
			this->uiButton::onMouseReleased(pos, button);
			if(uiMgr::current()->getCapture())
				uiMgr::current()->releaseCapture(this);
		}
		void			uiSlider::onMouseMoved(const Vec2& pos)
		{
			this->uiButton::onMouseMoved(pos);
			if(uiMgr::current()->getCapture().get_unsafe() == this)
			{
				Vec2 new_pos = _last_pos + (getAbsPos() + pos) - _last_mouse_pos - (getParent() ? getParent()->getAbsPos() : 0);
				Vec2 clip_pos = new_pos;
				clip_pos.x = min(max(_range_start.x, clip_pos.x), _range_start.x + _range_extent.x);
				clip_pos.y = min(max(_range_start.y, clip_pos.y), _range_start.y + _range_extent.y);
				setPos(clip_pos);

				if(_cb_on_slider_slide.valid())
					_cb_on_slider_slide(this);
			}
		}
		void			uiSlider::onDrawUp()
		{
			if(uiMgr::current()->getCapture().get_unsafe() == this)
				this->uiButton::onDrawDown();
			else
				this->uiButton::onDrawUp();
		}
		void			uiSlider::setOnSliderSlide(const wfunctor1<void, const SPtr<uiSlider>&>& cb)
		{
			_cb_on_slider_slide = cb;
		}
		void			uiSlider::setOnButtonClick(const wfunctor1<void, const SPtr<uiButton>&>& cb)
		{
			this->uiButton::setOnButtonClick(cb);
		}
		int32_t			uiSlider::getSlideValueX() const
		{
			return (int32_t)(getPos().x - _range_start.x);
		}
		int32_t			uiSlider::getSlideValueY() const
		{
			return (int32_t)(getPos().y - _range_start.y);
		}
		void			uiSlider::setSlideValueX(int32_t x)
		{
			int32_t new_value = (int32_t)(min(max(_range_start.x + (REAL)x, _range_start.x), _range_start.x + _range_extent.x));
			setPos(new_value, (int32_t)(getPos().y));
		}
		void			uiSlider::setSlideValueY(int32_t y)
		{
			int32_t new_value = (int32_t)(min(max(_range_start.y + (REAL)y, _range_start.y), _range_start.y + _range_extent.y));
			setPos((int32_t)(getPos().x), new_value);
		}
		int32_t			uiSlider::getSlideRangeX() const
		{
			return (int32_t)(_range_extent.x);
		}
		int32_t			uiSlider::getSlideRangeY() const
		{
			return (int32_t)(_range_extent.y);
		}
	};
};

