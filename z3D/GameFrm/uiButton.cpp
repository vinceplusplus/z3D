
#include "uiButton.h"

#include "../Core/functor_weak_inl.h"

#include "uiMgr.h"

namespace z3D
{
	namespace GameFrm
	{
		uiButton::uiButton()
		{
			_pressed = false;
			_hover = false;

			_checkbox = false;
			_checked = false;

			_repeat = false;
			_repeat_first_delay = 0.2f;
			_repeat_delay = 0.1f;

			_font = NULL;
			_icon_text_down_offset = Vec2(1,1);

			_stretch_to_fit = false;
		}
		uiButton::~uiButton()
		{
		}
		void	uiButton::loadImages(const wcs& up_image, const wcs& down_image, const wcs& hover_image, const wcs& disabled_image)
		{
			loadUpImage(up_image);
			loadDownImage(down_image);
			loadHoverImage(hover_image);
			loadDisabledImage(disabled_image);
		}
		void	uiButton::loadUpImage(const wcs& up_image)
		{
			_img_up = Img2D::load(up_image);
		}
		void	uiButton::loadDownImage(const wcs& down_image)
		{
			_img_down = Img2D::load(down_image);
		}
		void	uiButton::loadHoverImage(const wcs& hover_image)
		{
			_img_hover = Img2D::load(hover_image);
		}
		void	uiButton::loadDisabledImage(const wcs& disabled_image)
		{
			_img_disabled = Img2D::load(disabled_image);
		}
		void	uiButton::loadIconImage(const wcs& icon_image)
		{
			_img_icon = Img2D::load(icon_image);
		}
		const SPtr<Img2D>&		uiButton::getUpImage() const
		{
			return _img_up;
		}
		const SPtr<Img2D>&		uiButton::getDownImage() const
		{
			return _img_down;
		}
		const SPtr<Img2D>&		uiButton::getHoverImage() const
		{
			return _img_hover;
		}
		const SPtr<Img2D>&		uiButton::getDisabledImage() const
		{
			return _img_disabled;
		}
		const SPtr<Img2D>&		uiButton::getIconImage() const
		{
			return _img_icon;
		}
		void	uiButton::onDraw()
		{
			if(!isEnabled())
			{
				onDrawDisabled();
			}
			else
			{
				_hover = (uiMgr::current()->uiAtPoint(uiMgr::current()->getPointerPos()) == SPtr<uiBase>(this));
				if((_pressed && _hover) || _checked)
				{
					onDrawDown();
				}
				else if(_hover)
				{
					onDrawHover();
				}
				else
				{
					onDrawUp();
				}
			}
			if(_img_icon || (_font && _text.length()))
			{
				Vec2 pos, ext = getExt();
				if((_pressed && _hover) || _checked)
					pos += _icon_text_down_offset;

				onDrawIconText(pos, ext);
			}
		}
		void	uiButton::onDrawUp()
		{
			if(_cb_on_draw_up.valid())
				return _cb_on_draw_up(this);

			if(_img_up)
			{
				if(!_stretch_to_fit)
					_img_up->draw((getDX() - _img_up->dx()) >> 1, (getDY() - _img_up->dy()) >> 1);
				else
					_img_up->drawStretch(0, 0, _img_up->dx(), _img_up->dy(), 0, 0, getDX(), getDY());
			}
		}
		void	uiButton::onDrawDown()
		{
			if(_cb_on_draw_down.valid())
				return _cb_on_draw_down(this);

			if(_img_down)
			{
				if(!_stretch_to_fit)
					_img_down->draw((getDX() - _img_down->dx()) >> 1, (getDY() - _img_down->dy()) >> 1);
				else
					_img_down->drawStretch(0, 0, _img_down->dx(), _img_down->dy(), 0, 0, getDX(), getDY());
			}
			else if(_img_up)
			{
				if(!_stretch_to_fit)
					_img_up->draw((getDX() - _img_up->dx()) >> 1, (getDY() - _img_up->dy()) >> 1);
				else
					_img_up->drawStretch(0, 0, _img_up->dx(), _img_up->dy(), 0, 0, getDX(), getDY());
			}
		}
		void	uiButton::onDrawHover()
		{
			if(_cb_on_draw_hover.valid())
				return _cb_on_draw_hover(this);

			if(_img_hover)
			{
				if(!_stretch_to_fit)
					_img_hover->draw((getDX() - _img_hover->dx()) >> 1, (getDY() - _img_hover->dy()) >> 1);
				else
					_img_hover->drawStretch(0, 0, _img_hover->dx(), _img_hover->dy(), 0, 0, getDX(), getDY());
			}
			else if(_img_up)
			{
				if(!_stretch_to_fit)
					_img_up->draw((getDX() - _img_up->dx()) >> 1, (getDY() - _img_up->dy()) >> 1);
				else
					_img_up->drawStretch(0, 0, _img_up->dx(), _img_up->dy(), 0, 0, getDX(), getDY());
			}
		}
		void	uiButton::onDrawDisabled()
		{
			if(_cb_on_draw_disabled.valid())
				return _cb_on_draw_disabled(this);

			if(_img_disabled)
			{
				if(!_stretch_to_fit)
					_img_disabled->draw((getDX() - _img_disabled->dx()) >> 1, (getDY() - _img_disabled->dy()) >> 1);
				else
					_img_disabled->drawStretch(0, 0, _img_disabled->dx(), _img_disabled->dy(), 0, 0, getDX(), getDY());
			}
			else if(_img_up)
			{
				if(!_stretch_to_fit)
					_img_up->draw((getDX() - _img_up->dx()) >> 1, (getDY() - _img_up->dy()) >> 1);
				else
					_img_up->drawStretch(0, 0, _img_up->dx(), _img_up->dy(), 0, 0, getDX(), getDY());
			}
		}
		void	uiButton::onDrawIconText(const Vec2& pos, const Vec2& ext)
		{
			if(_cb_on_draw_icon_text.valid())
				return _cb_on_draw_icon_text(this, pos, ext);

			int dx, dy;
			//_font->getTextRect(_text, dx, dy);
			dx = _font->stringWidth(_text);
			dy = (int)_font->getHeight();
			if(_img_icon)
				dx += _img_icon->dx();

			int x = (int)((ext.x - dx) / 2);
			if(_img_icon)
			{
				int y = (int)((ext.y - _img_icon->dy()) / 2);
				_img_icon->draw(x + (int)(pos.x), y + (int)(pos.y));
				x += _img_icon->dx();
			}
			if(_text.length() && _font)
			{
				int y = (int)((ext.y - dy) / 2);
				_font->begin().shadow().end(_text, Vec2((REAL)x + pos.x, (REAL)y + pos.y));
			}
		}
		void	uiButton::onMousePressed(const Vec2& pos, uint32_t button)
		{
			if(button == 0)
			{
				if(_hover)
				{
					_pressed = true;
					uiMgr::current()->setCapture(this);

					if(_repeat)
					{
						if(_cb_on_button_click.valid())
							_cb_on_button_click(this);

						uiMgr::current()->registerUpdate(this);
						_countdown_repeat.setCountDown(_repeat_first_delay);
						_countdown_repeat.start();
					}
				}
			}
		}
		void	uiButton::onMouseReleased(const Vec2& pos, uint32_t button)
		{
			if(button == 0)
			{
				if(_pressed)
				{
					_pressed = false;
					uiMgr::current()->releaseCapture(this);
					if(_hover)
					{
						if(_checkbox)
							_checked = !_checked;

						if(!_repeat)
						{
							if(_cb_on_button_click.valid())
								_cb_on_button_click(this);
						}
					}
				}
			}
		}
		void	uiButton::onMouseMoved(const Vec2& pos)
		{
			_hover = hittest(pos);
		}
		bool	uiButton::isHover() const
		{
			return _visible && _hover;
		}
		void	uiButton::setCheckBox(bool checkbox)
		{
			_checkbox = checkbox;
			_checked = false;
		}
		bool	uiButton::getCheckBox() const
		{
			return _checkbox;
		}
		void	uiButton::setChecked(bool checked)
		{
			_checked = checked;
		}
		bool	uiButton::isChecked() const
		{
			return _checked;
		}
		void	uiButton::setRepeatMode(bool repeat)
		{
			_repeat = repeat;
		}
		bool	uiButton::isRepeatMode() const
		{
			return _repeat;
		}
		void	uiButton::setRepeatFirstDelay(REAL delay)
		{
			_repeat_first_delay = delay;
		}
		REAL	uiButton::getRepeatFirstDelay() const
		{
			return _repeat_first_delay;
		}
		void	uiButton::setRepeatDelay(REAL delay)
		{
			_repeat_delay = delay;
		}
		REAL	uiButton::getRepeatDelay() const
		{
			return _repeat_delay;
		}
		void	uiButton::onUpdate(REAL dt)
		{
			if(uiMgr::current()->getCapture() != SPtr<uiBase>(this))
			{
				uiMgr::current()->unregisterUpdate(this);
				return;
			}
			_countdown_repeat.update();
			if(_countdown_repeat.remaining() < 0.0f)
			{
				_countdown_repeat.setCountDown(_repeat_delay);
				_countdown_repeat.start();

				if(_cb_on_button_click.valid())
					_cb_on_button_click(this);
			}
		}
		void					uiButton::setOnButtonClick(const wfunctor1<void, const SPtr<uiButton>&>& cb)
		{
			_cb_on_button_click = cb;
		}
		void					uiButton::setOnButtonDrawUp(const wfunctor1<void, const SPtr<uiButton>&>& cb)
		{
			_cb_on_draw_up = cb;
		}
		void					uiButton::setOnButtonDrawDown(const wfunctor1<void, const SPtr<uiButton>&>& cb)
		{
			_cb_on_draw_down = cb;
		}
		void					uiButton::setOnButtonDrawHover(const wfunctor1<void, const SPtr<uiButton>&>& cb)
		{
			_cb_on_draw_hover = cb;
		}
		void					uiButton::setOnButtonDrawDisabled(const wfunctor1<void, const SPtr<uiButton>&>& cb)
		{
			_cb_on_draw_disabled = cb;
		}
		void					uiButton::setOnButtonDrawIconText(const wfunctor3<void, const SPtr<uiButton>&, const Vec2&, const Vec2&>& cb)
		{
			_cb_on_draw_icon_text = cb;
		}
		void					uiButton::setFont(const SPtr<TexFont>& font)
		{
			_font = font;
		}
		const SPtr<TexFont>&	uiButton::getFont() const
		{
			return _font;
		}
		void					uiButton::setText(const wcs& text)
		{
			_text = text;
		}
		const wcs&				uiButton::getText() const
		{
			return _text;
		}
		void					uiButton::setIconTextDownOffset(const Vec2& offset)
		{
			_icon_text_down_offset = offset;
		}
		const Vec2&		uiButton::getIconTextDownOffset() const
		{
			return _icon_text_down_offset;
		}
		void				uiButton::setStretchImage(bool stretch)
		{
			_stretch_to_fit = stretch;
		}
		bool				uiButton::getStretchImage() const
		{
			return _stretch_to_fit;
		}
	};
};

#pragma warning(default:4355)

