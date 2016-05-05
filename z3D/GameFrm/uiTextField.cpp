
#include "uiTextField.h"

#include "../Core/functor_weak_inl.h"

#include "uiMgr.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace GameFrm
	{
		class uiTextField::EDIT_PROC
		{
		public:
			static LRESULT CALLBACK		new_edit_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
				if(msg == WM_CHAR)
				{
					if(wParam == '\r')
					{
																						#pragma warning(suppress:4312)
						uiTextField* ui = (uiTextField*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
						ui->onTextInputComplete();
						return 0;
					}
				}
				return CallWindowProc((WNDPROC)(((uiTextField*)GetWindowLongPtr(hwnd, GWLP_USERDATA))->_original_edit_proc), hwnd, msg, wParam, lParam);
			}
		};

		uiTextField::uiTextField(intptr_t parent, const SPtr<TexFont>& font, bool password_mode)
		{
			Z_STATIC_ASSERT( sizeof(HWND) == sizeof(_hwnd), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( sizeof(WNDPROC) == sizeof(_original_edit_proc), WRONG_ASSUMPTION2 );

			_hwnd = (intptr_t)CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | (password_mode ? ES_PASSWORD : 0), 0, 3000, 1, 1, (HWND)parent, (HMENU)NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
																						#pragma warning(suppress:4244)
			SetWindowLongPtr((HWND)_hwnd, GWLP_USERDATA, (LONG_PTR)this);
																						#pragma warning(suppress:4244)
			_original_edit_proc = (intptr_t)SetWindowLongPtr((HWND)_hwnd, GWLP_WNDPROC, (LONG_PTR)&uiTextField::EDIT_PROC::new_edit_proc);
			_hwnd_parent = parent;
			_font = font;
			if(!_font)
			{
				_font = TexFont::load("", 16);
			}
			_display_index = 0;

			setClip(true);
		}
		uiTextField::~uiTextField()
		{
			if(_hwnd)
				DestroyWindow((HWND)_hwnd);
		}
		const SPtr<TexFont>&	uiTextField::getFont() const{return _font;}
		void			uiTextField::onDraw()
		{
			if(!_visible)
				return;

			update_display_index();

			wcs s = getText();
			if(GetWindowLongPtr((HWND)_hwnd, GWL_STYLE) & ES_PASSWORD)
			{
				size_t len = s.length();
				s.resize(0);
				s.resize(len,'*');
			}
			s = s.substr(_display_index);
			s = s.substr(0, displayable(s));
			_font->begin().ext(getExt()).color(1, 1, 1, 1).left().middle().shadow().end(s, 0, 0);
			if(isFocused())
			{
				ptrdiff_t cursor;

				SendMessage((HWND)_hwnd, EM_SETSEL, -1, 0);
				SendMessage((HWND)_hwnd, EM_GETSEL, (WPARAM)&cursor, NULL);

				cursor -= (ptrdiff_t)_display_index;
				cursor = max(0, cursor);
				s.resize(min(cursor, (ptrdiff_t)s.length()));

				size_t text_dx = _font->stringWidth(s);
				size_t text_dy = _font->getHeight();

				if(((GetTickCount() / 333) & 1) == 0)
					_font->begin().ext(getExt() - Vec2((REAL)text_dx, 0)).color(1, 1, 1, 1).left().middle().shadow().end(L"_", Vec2((REAL)text_dx, 0));
			}
		}
		void			uiTextField::setFocus()
		{
			SetFocus((HWND)_hwnd);
		}
		wcs				uiTextField::getText() const
		{
			wcs s;
			s.resize(GetWindowTextLength((HWND)_hwnd) + 1);
			GetWindowText((HWND)_hwnd, &s[0], (int)s.length());
			s.resize(s.length() - 1);
			return s;
		}
		void			uiTextField::setText(const wcs& text)
		{
			SetWindowText((HWND)_hwnd, text.c_str());
		}
		void			uiTextField::setTextLimit(size_t limit)
		{
			SendMessage((HWND)_hwnd, EM_LIMITTEXT, (WPARAM)limit, 0);
		}
		size_t			uiTextField::getTextLimit() const
		{
			return (size_t)SendMessage((HWND)_hwnd, EM_GETLIMITTEXT, 0, 0);
		}
		bool			uiTextField::isFocused() const
		{
			return GetFocus() == (HWND)_hwnd;
		}
		void					uiTextField::setOnTextInputComplete(const wfunctor1<void, const SPtr<uiTextField>&>& cb)
		{
			_cb_on_text_input_complete = cb;
		}
		void			uiTextField::onMousePressed(const Vec2& pos, uint32_t button)
		{
			setFocus();
		}
		void					uiTextField::onTextInputComplete()
		{
			if(_cb_on_text_input_complete.valid())
				_cb_on_text_input_complete(this);
		}
		void			uiTextField::update_display_index()
		{
			wcs s = getText();
			ptrdiff_t cursor = 0;
			SendMessage((HWND)_hwnd, EM_SETSEL, -1, 0);
			SendMessage((HWND)_hwnd, EM_GETSEL, (WPARAM)&cursor, NULL);

			cursor = min(cursor, (ptrdiff_t)s.length());

			size_t cursor_dx = _font->stringWidth(L"_");

			size_t text_dx;
			ptrdiff_t start = cursor;
			while(start >= 0)
			{
				text_dx = _font->stringWidth(s.substr(start, cursor - start));
				if(cursor_dx + text_dx > (size_t)getDX())
				{
					start = min(start + 1, (ptrdiff_t)s.length());
					break;
				}
				--start;
			}
			_display_index = (int32_t)max(_display_index, start);
			_display_index = (int32_t)min(_display_index, cursor);
		}
		size_t		uiTextField::displayable(const wcs& text)
		{
			size_t sum = 0;
			size_t i;
			size_t dx = (size_t)getDX();
			for(i = 0; i < text.length(); ++i)
			{
				sum += _font->getCharAdvance(text[i]);
				if(sum > dx)
					return i + 1;
			}
			return i;
		}
	};
};

