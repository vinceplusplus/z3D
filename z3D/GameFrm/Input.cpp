
#include "Input.h"
#include "../Core/aligned_storage_inl.h"

#define _WIN32_WINNT	0x0501

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace GameFrm
	{
		class Input::RAWINPUT_PIMPL
		{
		private:
			HMODULE				_dll;
		private:
			WNDPROC				_proc;
		public:
			HWND				hwnd;
			POINT				delta;
			int32_t				z_delta;
		public:
			RAWINPUT_PIMPL()
			{
				memset(this, 0, sizeof(*this));
			}
			~RAWINPUT_PIMPL()
			{
				if(hwnd)
					DestroyWindow(hwnd);

				if(_dll)
					FreeLibrary(_dll);
			}
		public:
			bool				supported() const
			{
				return !!_dll;
			}
		private:
			template<class T>
			bool				getProc(const char* proc_name, T& proc)
			{
				Z_STATIC_ASSERT( sizeof(void*) == sizeof(T), WRONG_ASSUMPTION );
				void* p = GetProcAddress(_dll, proc_name);
				if(!p)
					return false;
				proc = (T)p;
				return true;
			}
		private:
			static LRESULT CALLBACK
								proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
#pragma warning(suppress:4312)
				RAWINPUT_PIMPL* pimpl = (RAWINPUT_PIMPL*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				if(uMsg == WM_INPUT)
				{
					RAWINPUT buffer[64];

					UINT buffer_size = sizeof(buffer);
					if(pimpl->GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &buffer_size, sizeof(RAWINPUTHEADER)) != (UINT)-1)
					{
						size_t count = 1;

						// the following should be allowed because it will also receive other raw input that it doesn't intend to receive, thus the ones that intend to receive such input may not receive the input!!!
						if(false)
						{
							if(sizeof(buffer) > buffer_size)
							{
								buffer_size = sizeof(buffer) - buffer_size;
								UINT ret = pimpl->GetRawInputBuffer((RAWINPUT*)((char*)buffer + (sizeof(buffer) - buffer_size)), &buffer_size, sizeof(RAWINPUTHEADER));
								if(ret != (UINT)-1)
									count += (size_t)ret;
							}
						}

						pimpl->handleRawInput(buffer, count);
					}
				}

				return CallWindowProc(pimpl->_proc, hwnd, uMsg, wParam, lParam);
			}
		public:
			bool				init()
			{
				if(_dll)
					return true;
				_dll = LoadLibrary(L"user32");
				if(!_dll)
					return false;

#define GETPROC(x)		getProc(#x, x)

				do
				{
					if(!GETPROC(RegisterRawInputDevices))
						break;
					if(!GETPROC(GetRawInputData))
						break;
					if(!GETPROC(GetRawInputBuffer))
						break;
					if(!GETPROC(DefRawInputProc))
						break;

					hwnd = CreateWindowEx(0, L"STATIC", L"", WS_POPUP, 0, 0, 0, 0, NULL, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
					if(!hwnd)
						break;

#pragma warning(suppress:4244)
					SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
#pragma warning(suppress:4244 4312)
					_proc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)proc);

					return true;
				}while(false);

				FreeLibrary(_dll);
				memset(this, 0, sizeof(*this));

				return false;
			}
		public:
			BOOL (WINAPI*		RegisterRawInputDevices)	(
															__in_ecount(uiNumDevices) PCRAWINPUTDEVICE pRawInputDevices,
															__in UINT uiNumDevices,
															__in UINT cbSize);
			UINT (WINAPI*		GetRawInputData)			(
															__in HRAWINPUT hRawInput,
															__in UINT uiCommand,
															__out_bcount_part_opt(*pcbSize, return) LPVOID pData,
															__inout PUINT pcbSize,
															__in UINT cbSizeHeader);
			UINT (WINAPI*		GetRawInputBuffer)			(
															__out_bcount_opt(*pcbSize) PRAWINPUT pData,
															__inout PUINT pcbSize,
															__in UINT cbSizeHeader);
			LRESULT (WINAPI*	DefRawInputProc)			(
															__in_ecount(nInput) PRAWINPUT* paRawInput,
															__in INT nInput,
															__in UINT cbSizeHeader);
		public:
			void			handleRawInput(void* rawinputs, size_t rawinput_count)
			{
				RAWINPUT* p = (RAWINPUT*)rawinputs;
				for(size_t count = rawinput_count; count; --count)
				{
					const RAWINPUT& input = *p;

					if(input.header.dwType == RIM_TYPEMOUSE)
					{
						if(!(input.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
						{
							delta.x += input.data.mouse.lLastX;
							delta.y += input.data.mouse.lLastY;
						}
						if(input.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
						{
							z_delta += (short)input.data.mouse.usButtonData;
						}
					}

					RAWINPUT* p2 = p;
					p = NEXTRAWINPUTBLOCK(p);

					LRESULT ret = DefRawInputProc(&p2, 1, sizeof(RAWINPUTHEADER));
					Z_ASSERT( ret == S_OK );
				}
			}
		};

#include "../Core/z_off_debug_new.h"
		Input::Input(intptr_t hwnd)
			: _ri_pimpl(new (_ri_pimpl.allocate()) RAWINPUT_PIMPL(), aligned_storage_tag::ctor())
		{
			Z_STATIC_ASSERT( sizeof(_ri_pimpl.get().hwnd) == sizeof(HWND), WRONG_ASSUMPTION );

			_ri_pimpl.get().init();

			_registered_rid = false;
			if(_ri_pimpl.get().supported())
				_registered_rid = registerRawInput((intptr_t)_ri_pimpl.get().hwnd);

			_enable_center_mouse = true;
			_enable_clip_mouse = true;

			reinit(hwnd);

			setClipMouse(!!_enable_clip_mouse);
		}
#include "../Core/z_on_debug_new.h"

		Input::~Input()
		{
			if(_registered_rid)
				deregisterRawInput();
		}
		void			Input::reinit(intptr_t hwnd, bool clear_state)
		{
			Z_STATIC_ASSERT( sizeof(POINT) == sizeof(::POINT), WRONG_ASSUMPTION );

			_hwnd = hwnd;
			if(clear_state)
			{
				memset(&_kb_state, 0, sizeof(_kb_state));
				memset(&_kb_state_last, 0, sizeof(_kb_state_last));
				memset(&_button_state, 0, sizeof(_button_state));
				memset(&_button_state_last, 0, sizeof(_button_state_last));
			}
			memset(&_mouse_delta, 0, sizeof(_mouse_delta));
			if(_enable_center_mouse)
			{
				RECT rt;
				GetClientRect((HWND)_hwnd, &rt);
				_mouse_pos_client.x = (rt.right - rt.left) >> 1;
				_mouse_pos_client.y = (rt.bottom - rt.top) >> 1;

				POINT pt = _mouse_pos_client;
				ClientToScreen((HWND)_hwnd, (::POINT*)&pt);
				SetCursorPos(pt.x, pt.y);
			}
			else
			{
				GetCursorPos((::POINT*)&_mouse_pos_last);
				_mouse_pos_client = _mouse_pos_last;
				ScreenToClient((HWND)_hwnd, (::POINT*)&_mouse_pos_client);
			}

			_mouse_z_delta = 0;
			_pending_z_delta = 0;
		}
		void			Input::update()
		{
			Z_STATIC_ASSERT( sizeof(POINT) == sizeof(::POINT), WRONG_ASSUMPTION );

			// copy current to last
			memcpy(_kb_state_last, _kb_state, sizeof(_kb_state));
			memcpy(_button_state_last, _button_state, sizeof(_button_state));

			bool foreground = (GetForegroundWindow() == (HWND)_hwnd);

			// update keyboard
			if(foreground)
			{
				GetKeyboardState((unsigned char*)_kb_state);
				for(size_t i = 0; i < 256; ++i)
					_kb_state[i] = !!(_kb_state[i] & 0x80);
			}
			else
				memset(_kb_state, 0, sizeof(_kb_state));

			// update mouse
			if(foreground)
			{
				if(_enable_center_mouse)
				{
					RECT rt;
					GetClientRect((HWND)_hwnd, &rt);
					::POINT pt;
					GetCursorPos(&pt);
					ScreenToClient((HWND)_hwnd, &pt);
					_mouse_delta.x = pt.x - ((rt.right - rt.left) >> 1);
					_mouse_delta.y = pt.y - ((rt.bottom - rt.top) >> 1);
					_mouse_pos_client.x += _mouse_delta.x;
					_mouse_pos_client.y += _mouse_delta.y;
					if(_enable_clip_mouse)
					{
						_mouse_pos_client.x = min(max((int32_t)rt.left, _mouse_pos_client.x), (int32_t)(rt.right - 1));
						_mouse_pos_client.y = min(max((int32_t)rt.top, _mouse_pos_client.y), (int32_t)(rt.bottom - 1));
					}
					pt.x = ((rt.right - rt.left) >> 1);
					pt.y = ((rt.bottom - rt.top) >> 1);
					ClientToScreen((HWND)_hwnd, &pt);
					SetCursorPos(pt.x, pt.y);
				}
				else
				{
					::POINT pt;
					GetCursorPos(&pt);
					_mouse_delta.x = pt.x - _mouse_pos_last.x;
					_mouse_delta.y = pt.y - _mouse_pos_last.y;
					_mouse_pos_last = *(const POINT*)&pt;
					_mouse_pos_client = _mouse_pos_last;
					ScreenToClient((HWND)_hwnd, (::POINT*)&_mouse_pos_client);
				}

				_mouse_z_delta = _pending_z_delta;
				_pending_z_delta = 0;

				if(GetSystemMetrics(SM_SWAPBUTTON))
				{
					_button_state[0] = _kb_state[VK_RBUTTON];
					_button_state[1] = _kb_state[VK_LBUTTON];
					_button_state[2] = _kb_state[VK_MBUTTON];
				}
				else
				{
					_button_state[0] = _kb_state[VK_LBUTTON];
					_button_state[1] = _kb_state[VK_RBUTTON];
					_button_state[2] = _kb_state[VK_MBUTTON];
				}
			}
			else
			{
				_mouse_delta.x = 0;
				_mouse_delta.y = 0;
				_mouse_z_delta = 0;
				memset(_button_state, 0, sizeof(_button_state));
			}

			if(foreground && _registered_rid)
			{
				_mouse_delta = _ri_pimpl.get().delta;
				_mouse_z_delta = _ri_pimpl.get().z_delta;

				memset(&_ri_pimpl.get().delta, 0, sizeof(_ri_pimpl.get().delta));
				_ri_pimpl.get().z_delta = 0;
			}

			updateMouseClipping();
		}
		bool			Input::isKeyDown(size_t vk_code) const
		{
			Z_ASSERT(vk_code >= 0 && vk_code < 256);
			return !!_kb_state[vk_code];
		}
		bool			Input::isKeyDownLast(size_t vk_code) const
		{
			Z_ASSERT(vk_code >= 0 && vk_code < 256);
			return !!_kb_state_last[vk_code];
		}
		bool			Input::isKeyPressed(size_t vk_code) const
		{
			return !isKeyDownLast(vk_code) && isKeyDown(vk_code);
		}
		bool			Input::isKeyReleased(size_t vk_code) const
		{
			return isKeyDownLast(vk_code) && !isKeyDown(vk_code);
		}
		bool			Input::isButtonDown(size_t button) const
		{
			button = min(max((size_t)0, button), (size_t)2);
			return _button_state[button] == 1;
		}
		bool			Input::isButtonDownLast(size_t button) const
		{
			button = min(max((size_t)0, button), (size_t)2);
			return _button_state_last[button] == 1;
		}
		bool			Input::isButtonPressed(size_t button) const
		{
			return !isButtonDownLast(button) && isButtonDown(button);
		}
		bool			Input::isButtonReleased(size_t button) const
		{
			return isButtonDownLast(button) && !isButtonDown(button);
		}
		int32_t			Input::getMouseDeltaX() const {return _mouse_delta.x;}
		int32_t			Input::getMouseDeltaY() const {return _mouse_delta.y;}
		int32_t			Input::getMouseDeltaZ() const {return _mouse_z_delta;}
		int32_t			Input::getMouseX() const {return _mouse_pos_client.x;}
		int32_t			Input::getMouseY() const {return _mouse_pos_client.y;}
		void			Input::setCenterMouse(bool center)
		{
			Z_STATIC_ASSERT( sizeof(POINT) == sizeof(::POINT), WRONG_ASSUMPTION );

			if((!!_enable_center_mouse) == center)
				return;

			if(_enable_center_mouse)
			{
				::POINT pt = *(::POINT*)&_mouse_pos_client;
				ClientToScreen((HWND)_hwnd, &pt);
				SetCursorPos(pt.x, pt.y);
				_mouse_pos_last = *(POINT*)&pt;
			}
			else
			{
				RECT rt;
				GetClientRect((HWND)_hwnd, &rt);
				::POINT pt;
				pt.x = ((rt.right - rt.left) >> 1);
				pt.y = ((rt.bottom - rt.top) >> 1);
				ClientToScreen((HWND)_hwnd, &pt);
				SetCursorPos(pt.x, pt.y);
			}
			_enable_center_mouse = center;
			if(_enable_center_mouse)
				setClipMouse(true);
		}
		bool			Input::getCenterMouse() const
		{
			return !!_enable_center_mouse;
		}
		void			Input::setClipMouse(bool clip)
		{
			_enable_clip_mouse = clip;
			if(_enable_center_mouse)
				_enable_clip_mouse = true;
		}
		bool			Input::getClipMouse() const
		{
			return !!_enable_clip_mouse;
		}
		void			Input::removeMouseClipping()
		{
			ClipCursor(NULL);
		}
		void			Input::setClientMousePos(int32_t x, int32_t y)
		{
			_mouse_pos_client.x = x;
			_mouse_pos_client.y = y;
		}
		void			Input::centerClientMousePos()
		{
			RECT rt;
			GetClientRect((HWND)_hwnd, &rt);
			POINT pt;
			pt.x = ((rt.right - rt.left) >> 1);
			pt.y = ((rt.bottom - rt.top) >> 1);

			setClientMousePos(pt.x, pt.y);
		}
		void			Input::addPendingZDelta(int32_t delta)
		{
			_pending_z_delta += delta;
		}
		void			Input::updateMouseClipping()
		{
			Z_STATIC_ASSERT( sizeof(POINT) == sizeof(::POINT), WRONG_ASSUMPTION );

			if(_enable_center_mouse)
				_enable_clip_mouse = true;

			bool foreground = (GetForegroundWindow() == (HWND)_hwnd);

			if(!foreground || !_enable_clip_mouse)
				return removeMouseClipping();

			RECT rt;
			GetClientRect((HWND)_hwnd, &rt);
			ClientToScreen((HWND)_hwnd, (::POINT*)&rt.left);
			ClientToScreen((HWND)_hwnd, (::POINT*)&rt.right);
			ClipCursor(&rt);
		}
		bool				Input::registerRawInput(intptr_t hwnd)
		{
			if(!_ri_pimpl.get().supported())
				return false;

			RAWINPUTDEVICE rid;
			memset(&rid, 0, sizeof(rid));
			rid.usUsagePage = 0x01;
			rid.usUsage = 0x02;
			rid.hwndTarget = (HWND)hwnd;

			return !!_ri_pimpl.get().RegisterRawInputDevices(&rid, 1, sizeof(rid));
		}
		bool				Input::deregisterRawInput()
		{
			if(!_ri_pimpl.get().supported())
				return false;

			RAWINPUTDEVICE rid;
			memset(&rid, 0, sizeof(rid));
			rid.usUsagePage = 0x01;
			rid.usUsage = 0x02;
			rid.dwFlags = RIDEV_REMOVE;

			return !!_ri_pimpl.get().RegisterRawInputDevices(&rid, 1, sizeof(rid));
		}
	};
};
