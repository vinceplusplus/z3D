
#pragma once

#include "predecl.h"

#include "uiBase.h"

namespace z3D
{
	namespace GameFrm
	{
		//class __Z3DGAMEFRM_API uiButtonRenderHandler
		//{
		//public:
		//	virtual void	draw_up(uiBase* ui);
		//	virtual void	draw_down(uiBase* ui);
		//	virtual void	draw_hover(uiBase* ui);
		//	virtual void	draw_disabled(uiBase* ui);
		//	virtual void	draw_icon_text(uiBase* ui,const Vec2& pos,const Vec2& ext);
		//};

		class uiButton: public uiBase//, public uiButtonRenderHandler
		{
		private:
			SPtr<Img2D>		_img_up;
			SPtr<Img2D>		_img_down;
			SPtr<Img2D>		_img_hover;
			SPtr<Img2D>		_img_disabled;
		private:
			bool			_pressed;
			bool			_hover;
			bool			_checkbox;
			bool			_checked;
		private:
			bool			_repeat;
			REAL			_repeat_first_delay;
			REAL			_repeat_delay;
			CountDown		_countdown_repeat;
		private:
			SPtr<Img2D>		_img_icon;
			SPtr<TexFont>	_font;
			wcs				_text;
			Vec2			_icon_text_down_offset;
		private:
			bool			_stretch_to_fit;
		private:
			wfunctor1<void, const SPtr<uiButton>&>
							_cb_on_button_click;
		private:
			wfunctor1<void, const SPtr<uiButton>&>									_cb_on_draw_up;
			wfunctor1<void, const SPtr<uiButton>&>									_cb_on_draw_down;
			wfunctor1<void, const SPtr<uiButton>&>									_cb_on_draw_hover;
			wfunctor1<void, const SPtr<uiButton>&>									_cb_on_draw_disabled;
			wfunctor3<void, const SPtr<uiButton>&, const Vec2&, const Vec2&>		_cb_on_draw_icon_text;
		public:
			uiButton();
		protected:
			~uiButton();
		public:
			void					loadImages(const wcs& up_image, const wcs& down_image, const wcs& hover_image, const wcs& disabled_image);
			void					loadUpImage(const wcs& up_image);
			void					loadDownImage(const wcs& down_image);
			void					loadHoverImage(const wcs& hover_image);
			void					loadDisabledImage(const wcs& disabled_image);
			void					loadIconImage(const wcs& icon_image);
		public:
			const SPtr<Img2D>&		getUpImage() const;
			const SPtr<Img2D>&		getDownImage() const;
			const SPtr<Img2D>&		getHoverImage() const;
			const SPtr<Img2D>&		getDisabledImage() const;
			const SPtr<Img2D>&		getIconImage() const;
		protected:
			void					onDraw();
		protected:
			virtual void			onDrawUp();
			virtual void			onDrawDown();
			virtual void			onDrawHover();
			virtual void			onDrawDisabled();
			virtual void			onDrawIconText(const Vec2& pos, const Vec2& ext);
		protected:
			void					onMousePressed(const Vec2& pos, uint32_t button);
			void					onMouseReleased(const Vec2& pos, uint32_t button);
			void					onMouseMoved(const Vec2& pos);
		public:
			void					onUpdate(REAL dt);
		public:
			void					setOnButtonClick(const wfunctor1<void, const SPtr<uiButton>&>& cb);
		public:
			void					setOnButtonDrawUp(const wfunctor1<void, const SPtr<uiButton>&>& cb);
			void					setOnButtonDrawDown(const wfunctor1<void, const SPtr<uiButton>&>& cb);
			void					setOnButtonDrawHover(const wfunctor1<void, const SPtr<uiButton>&>& cb);
			void					setOnButtonDrawDisabled(const wfunctor1<void, const SPtr<uiButton>&>& cb);
			void					setOnButtonDrawIconText(const wfunctor3<void, const SPtr<uiButton>&, const Vec2&, const Vec2&>& cb);
		public:
			bool					isHover() const;
		public:
			void					setCheckBox(bool checkbox);
			bool					getCheckBox() const;
		public:
			void					setChecked(bool checked);
			bool					isChecked() const;
		public:
			void					setRepeatMode(bool repeat);
			bool					isRepeatMode() const;
		public:
			void					setRepeatFirstDelay(REAL delay);
			REAL					getRepeatFirstDelay() const;
		public:
			void					setRepeatDelay(REAL delay);
			REAL					getRepeatDelay() const;
		public:
			void					setFont(const SPtr<TexFont>& font);
			const SPtr<TexFont>&	getFont() const;
			void					setText(const wcs& text);
			const wcs&				getText() const;
			void					setIconTextDownOffset(const Vec2& offset);
			const Vec2&				getIconTextDownOffset() const;
		public:
			void					setStretchImage(bool stretch);
			bool					getStretchImage() const;
		//public:
		//	void					setButtonRenderHandler(uiButtonRenderHandler* handler);
		//	uiButtonRenderHandler*	getButtonRenderHandler() const;
		};
	};
};

