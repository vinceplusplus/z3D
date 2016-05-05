
#include "uiHeader.h"
#include "uiButton.h"
#include "uiSlider.h"

#include "../Core/functor_weak_inl.h"

namespace z3D
{
	namespace GameFrm
	{
		uiHeader::uiHeader(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			setPos(x, y);
			setExt(dx, dy);

			_grip_width = 5;
		}
		uiHeader::~uiHeader()
		{}
		void				uiHeader::setResizeGripImage(const wcs& filepath)
		{
			_resize_grip_image = filepath;

			for(size_t i = 0; i < _sliders.size(); ++i)
				_sliders[i]->loadUpImage(_resize_grip_image);
		}
		const wcs&			uiHeader::getResizeGripImage() const
		{
			return _resize_grip_image;
		}
		void				uiHeader::addHeaderBtn(const SPtr<uiButton>& btn)
		{
			insertHeaderBtn(_headerbtns.size(), btn);
		}
		void				uiHeader::insertHeaderBtn(size_t at, const SPtr<uiButton>& btn)
		{
			Z_ASSERT( at <= _headerbtns.size() );

			btn->setClip(true);
			_headerbtns.insert(_headerbtns.begin() + at, btn);

			addChild(btn);
			doLayout();
		}
		void				uiHeader::removeHeaderBtn(size_t at)
		{
			Z_ASSERT( at < _headerbtns.size() );

			SPtr<uiButton> btn = _headerbtns[at];

			_headerbtns.erase(_headerbtns.begin() + at);
			removeChild(getChildIndex(btn));
			doLayout();
		}
		SPtr<uiButton>		uiHeader::getHeaderBtn(size_t at) const
		{
			Z_ASSERT( at < _headerbtns.size() );

			return _headerbtns[at];
		}
		size_t				uiHeader::getHeaderBtnCount() const
		{
			return _headerbtns.size();
		}
		void				uiHeader::doLayout()
		{
			rearrangeHeaderBtns();
			rearrangeResizeGrips();
		}
		void				uiHeader::rearrangeHeaderBtns()
		{
			int32_t start = 0;
			for(size_t i = 0; i < _headerbtns.size(); ++i)
			{
				const SPtr<uiButton> btn = _headerbtns[i];
				btn->setPos(start, 0);
				btn->setExt(btn->getDX(), getDY());
				start += btn->getDX();
			}
			setExt(start + _grip_width, getDY());
		}
		void				uiHeader::rearrangeResizeGrips()
		{
			while(_sliders.size() > _headerbtns.size())
			{
				_sliders[0]->setOnSliderSlide(wfunctor1<void, const SPtr<uiSlider>&>());
				ptrdiff_t index = getChildIndex(_sliders[0]);

				Z_ASSERT( index >= 0 );

				removeChild(index);
				_sliders.erase(_sliders.begin());
			}
			while(_sliders.size() < _headerbtns.size())
			{
				SPtr<uiSlider> slider = new uiSlider(0, 0, _grip_width << 1, getDY(), 0, 0);
				slider->loadUpImage(_resize_grip_image);

				slider->setOnSliderSlide(wfunctor1<void, const SPtr<uiSlider>&>(this, &uiHeader::onSliderSlide));

				addChild(slider);
				_sliders.push_back(slider);
			}
			for(size_t i = 0; i < _sliders.size(); ++i)
			{
				const SPtr<uiSlider> slider = _sliders[i];
				slider->setExt(_grip_width << 1, getDY());
				slider->setTopMost(true);
				slider->bringToFront();

				const SPtr<uiButton> button = _headerbtns[i];
				slider->setRange(button->getX() - _grip_width, 0, 1024, 0);
				slider->setSlideValueX(button->getDX());
				slider->setSlideValueY(0);
			}
		}
		void				uiHeader::setResizeGripWidth(int32_t width)
		{
			_grip_width = width;

			doLayout();
		}
		int32_t				uiHeader::getResizeGripWidth() const
		{
			return _grip_width;
		}
		int32_t				uiHeader::getTotalLength() const
		{
			int32_t sum = 0;
			for(size_t i = 0; i < _headerbtns.size(); ++i)
				sum += _headerbtns[i]->getDX();

			return sum;
		}
		ptrdiff_t			uiHeader::getHeaderBtnIndex(const SPtr<uiButton>& btn) const
		{
			for(size_t i = 0; i < _headerbtns.size(); ++i)
				if(btn == _headerbtns[i])
					return i;
			return -1;
		}
		ptrdiff_t			uiHeader::getSliderIndex(const SPtr<uiSlider>& slider) const
		{
			for(size_t i = 0; i < _sliders.size(); ++i)
				if(slider == _sliders[i])
					return i;
			return -1;
		}
		void						uiHeader::onSliderSlide(const SPtr<uiSlider>& slider)
		{
			ptrdiff_t index = getSliderIndex(slider);

			Z_ASSERT( index >= 0 );

			_headerbtns[index]->setExt(slider->getSlideValueX(), getDY());

			doLayout();

			if(_cb_on_headerbtn_resize.valid())
				_cb_on_headerbtn_resize(this, index);
		}
		void						uiHeader::setOnHeaderButtonResize(const wfunctor2<void, const SPtr<uiHeader>&, size_t>& cb)
		{
			_cb_on_headerbtn_resize = cb;
		}
	};
};

