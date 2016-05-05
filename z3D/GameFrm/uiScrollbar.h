
#pragma once

#include "predecl.h"

#include "uiBase.h"
#include "uiButton.h"
#include "uiSlider.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiScrollbar: public uiBase
		{
		private:
			SPtr<uiSlider>			_slider;
			SPtr<uiButton>			_btn_lt;
			SPtr<uiButton>			_btn_rb;
		private:
			int32_t					_min_value;
			int32_t					_max_value;
			int32_t					_page_value;
			int32_t					_arrow_value;
		private:
			int32_t					_cur_value;
		private:
			size_t					_hsb_layout;
		private:
			wfunctor1<void, const SPtr<uiScrollbar>&>
									_cb_on_scrollbar_scroll;
		public:
			uiScrollbar(int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t min_value, int32_t max_value);
		protected:
			~uiScrollbar();
		public:
			const SPtr<uiButton>&	getButtonLT() const;
			const SPtr<uiButton>&	getButtonRB() const;
			const SPtr<uiSlider>&	getSlider() const;
		public:
			void					setOnScrollbarScroll(const wfunctor1<void, const SPtr<uiScrollbar>&>& cb);
		public:
			void					realizeThumbPos();
		public:
			virtual void			doLayout_HScrollbar();
			virtual void			doLayout_VScrollbar();
			virtual void			doLayout();
		public:
			void					setScrollInfo(int32_t min_value, int32_t max_value, int32_t page_value, int32_t arrow_value);
			void					setScrollPos(int32_t cur_value, bool do_layout = true);
		public:
			int32_t					getMinValue() const;
			int32_t					getMaxValue() const;
			int32_t					getPageValue() const;
			int32_t					getArrowValue() const;
		public:
			int32_t					getScrollPos() const;
		private:
			void					onButtonClick(const SPtr<uiButton>& btn);
			void					onSliderSlide(const SPtr<uiSlider>& slider);
		};
	};
};

