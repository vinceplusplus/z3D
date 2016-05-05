
#include "uiScrollbar.h"

#include "../Core/functor_weak_inl.h"

namespace z3D
{
	namespace GameFrm
	{
		uiScrollbar::uiScrollbar(int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t min_value, int32_t max_value)
		{
			_min_value = min_value;
			_max_value = max_value;
			_cur_value = min_value;
			_page_value = 1;
			_arrow_value = 1;

			_hsb_layout = true;

			setPos(x, y);
			setExt(dx, dy);

			_btn_lt = new uiButton();
			_btn_lt->setExt(20, 20);
			_btn_lt->setRepeatMode(true);

			_btn_rb = new uiButton();
			_btn_rb->setExt(20, 20);
			_btn_rb->setRepeatMode(true);

			_slider = new uiSlider(0, 0 , 20 , 20 , 1 , 1);

			_btn_lt->setOnButtonClick(wfunctor1<void, const SPtr<uiButton>&>(this, &uiScrollbar::onButtonClick));
			_btn_rb->setOnButtonClick(wfunctor1<void, const SPtr<uiButton>&>(this, &uiScrollbar::onButtonClick));
			_slider->setOnSliderSlide(wfunctor1<void, const SPtr<uiSlider>&>(this, &uiScrollbar::onSliderSlide));

			addChild(_btn_lt);
			addChild(_btn_rb);
			addChild(_slider);
		}
		uiScrollbar::~uiScrollbar()
		{}
		const SPtr<uiButton>&		uiScrollbar::getButtonLT() const
		{
			return _btn_lt;
		}
		const SPtr<uiButton>&		uiScrollbar::getButtonRB() const
		{
			return _btn_rb;
		}
		const SPtr<uiSlider>&		uiScrollbar::getSlider() const
		{
			return _slider;
		}
		void						uiScrollbar::setOnScrollbarScroll(const wfunctor1<void, const SPtr<uiScrollbar>&>& cb)
		{
			_cb_on_scrollbar_scroll = cb;
		}
		void			uiScrollbar::realizeThumbPos()
		{
			_slider->setSlideValueX(0);
			_slider->setSlideValueY(0);
			if(_hsb_layout)
				_slider->setSlideValueX((_cur_value - _min_value) * _slider->getSlideRangeX() / max(_max_value - _min_value, 1));
			else
				_slider->setSlideValueY((_cur_value - _min_value) * _slider->getSlideRangeY() / max(_max_value - _min_value, 1));
		}
		void			uiScrollbar::doLayout_HScrollbar()
		{
			_btn_lt->setPos(0, (getDY() - _btn_lt->getDY()) >> 1);
			_btn_rb->setPos(getDX() - _btn_rb->getDX(), (getDY() - _btn_rb->getDY()) >> 1);

			_slider->setRange(_btn_lt->getDX(), (getDY() - _slider->getDY()) >> 1, getDX() - _btn_lt->getDX() - _btn_rb->getDX() - _slider->getDX(), 0);
			realizeThumbPos();

			_hsb_layout = true;
		}
		void			uiScrollbar::doLayout_VScrollbar()
		{
			_btn_lt->setPos((getDX() - _btn_lt->getDX()) >> 1, 0);
			_btn_rb->setPos((getDX() - _btn_rb->getDX()) >> 1, getDY() - _btn_rb->getDY());

			_slider->setRange((getDX() - _slider->getDX()) >> 1, _btn_lt->getDY(), 0, getDY() - _btn_lt->getDY() - _btn_rb->getDY() - _slider->getDY());
			realizeThumbPos();

			_hsb_layout = false;
		}
		void			uiScrollbar::doLayout()
		{
			if(_hsb_layout)
				doLayout_HScrollbar();
			else
				doLayout_VScrollbar();
		}
		void			uiScrollbar::setScrollInfo(int32_t min_value, int32_t max_value, int32_t page_value, int32_t arrow_value)
		{
			_min_value = min_value;
			_max_value = max_value;
			_page_value = page_value;
			_arrow_value = arrow_value;

			_max_value = max(_min_value, _max_value);

			_cur_value = min(max(_min_value, _cur_value), _max_value);

			doLayout();
		}
		void			uiScrollbar::setScrollPos(int32_t cur_value, bool do_layout)
		{
			_cur_value = cur_value;

			_cur_value = min(max(_min_value, _cur_value), _max_value);

			if(do_layout)
				doLayout();
		}
		int32_t			uiScrollbar::getMinValue() const
		{
			return _min_value;
		}
		int32_t			uiScrollbar::getMaxValue() const
		{
			return _max_value;
		}
		int32_t			uiScrollbar::getPageValue() const
		{
			return _page_value;
		}
		int32_t			uiScrollbar::getArrowValue() const
		{
			return _arrow_value;
		}
		int32_t			uiScrollbar::getScrollPos() const
		{
			return _cur_value;
		}
		void					uiScrollbar::onButtonClick(const SPtr<uiButton>& btn)
		{
			if(btn == _btn_lt)
			{
				setScrollPos(_cur_value - _arrow_value);

				if(_cb_on_scrollbar_scroll.valid())
					_cb_on_scrollbar_scroll(this);
			}
			else if(btn == _btn_rb)
			{
				setScrollPos(_cur_value + _arrow_value);

				if(_cb_on_scrollbar_scroll.valid())
					_cb_on_scrollbar_scroll(this);
			}
		}
		void					uiScrollbar::onSliderSlide(const SPtr<uiSlider>& slider)
		{
			if(slider == _slider)
			{
				REAL slide_rate = 0.0f;
				if(_hsb_layout)
					slide_rate = (REAL)_slider->getSlideValueX() / max(_slider->getSlideRangeX(), 1);
				else
					slide_rate = (REAL)_slider->getSlideValueY() / max(_slider->getSlideRangeY(), 1);

				setScrollPos(NINT(slide_rate * (_max_value - _min_value) + _min_value), false);

				if(_cb_on_scrollbar_scroll.valid())
					_cb_on_scrollbar_scroll(this);
			}
		}
	};
};

