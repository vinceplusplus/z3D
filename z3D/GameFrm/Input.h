
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace GameFrm
	{
		class Input: public RefBase
		{
		private:
			typedef struct _POINT
			{
				int32_t			x;
				int32_t			y;

			}				POINT;
		private:
			char			_kb_state[256];
			char			_kb_state_last[256];
			ptrdiff_t		_button_state[3];				// 0 - left button, 1 - right button, 2 - middle button
			ptrdiff_t		_button_state_last[3];
			POINT			_mouse_delta;
			POINT			_mouse_pos_client;
			POINT			_mouse_pos_last;
			int32_t			_mouse_z_delta;
			int32_t			_pending_z_delta;
			intptr_t		_hwnd;
			size_t			_clip_mouse;
			size_t			_enable_clip_mouse;
			size_t			_enable_center_mouse;
		private:
			class RAWINPUT_PIMPL;

			typed_aligned_storage<RAWINPUT_PIMPL, 40, 4, true>
							_ri_pimpl;
			size_t			_registered_rid;
		public:
			Input(intptr_t hwnd);
		protected:
			~Input();
		public:
			void			reinit(intptr_t hwnd, bool clear_state = true);
		public:
			void			update();
		public:
			bool			isKeyDown			(size_t vk_code) const;
			bool			isKeyDownLast		(size_t vk_code) const;
			bool			isKeyPressed		(size_t vk_code) const;
			bool			isKeyReleased		(size_t vk_code) const;

			bool			isButtonDown		(size_t button) const;
			bool			isButtonDownLast	(size_t button) const;
			bool			isButtonPressed		(size_t button) const;
			bool			isButtonReleased	(size_t button) const;

			int32_t			getMouseDeltaX() const;
			int32_t			getMouseDeltaY() const;
			int32_t			getMouseDeltaZ() const;
			int32_t			getMouseX() const;
			int32_t			getMouseY() const;
		public:
			void			addPendingZDelta(int32_t delta);
		public:
			void			setCenterMouse(bool center);
			bool			getCenterMouse() const;
		public:
			void			setClipMouse(bool clip);
			bool			getClipMouse() const;
		public:
			void			removeMouseClipping();
		public:
			void			setClientMousePos(int32_t x, int32_t y);
			void			centerClientMousePos();
		private:
			void			updateMouseClipping();
		private:
			bool			registerRawInput(intptr_t hwnd);
			bool			deregisterRawInput();
		};
	};
};
