
#pragma once

#include "predecl.h"

#include "uiBase.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiHeader: public uiBase
		{
		private:
			vector<SPtr<uiButton> >		_headerbtns;
			vector<SPtr<uiSlider> >		_sliders;
		private:
			int32_t						_grip_width;
		private:
			wcs							_resize_grip_image;
		private:
			wfunctor2<void, const SPtr<uiHeader>&, size_t>
										_cb_on_headerbtn_resize;
		public:
			uiHeader(int32_t x, int32_t y, int32_t dx, int32_t dy);
		protected:
			~uiHeader();
		public:
			void						setResizeGripImage(const wcs& filepath);
			const wcs&					getResizeGripImage() const;
		public:
			void						addHeaderBtn(const SPtr<uiButton>& btn);
			void						insertHeaderBtn(size_t at, const SPtr<uiButton>& btn);
			void						removeHeaderBtn(size_t at);
		public:
			SPtr<uiButton>				getHeaderBtn(size_t at) const;
		public:
			size_t						getHeaderBtnCount() const;
		public:
			void						doLayout();
		protected:
			void						rearrangeHeaderBtns();
			void						rearrangeResizeGrips();
		public:
			void						setResizeGripWidth(int32_t width);
			int32_t						getResizeGripWidth() const;
		public:
			int32_t						getTotalLength() const;
			ptrdiff_t					getHeaderBtnIndex(const SPtr<uiButton>& btn) const;
		private:
			ptrdiff_t					getSliderIndex(const SPtr<uiSlider>& slider) const;
		private:
			void						onSliderSlide(const SPtr<uiSlider>& slider);
		public:
			void						setOnHeaderButtonResize(const wfunctor2<void, const SPtr<uiHeader>&, size_t>& cb);
		};
	};
};

