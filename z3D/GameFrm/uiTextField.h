
#pragma once

#include "predecl.h"

#include "uiBase.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiTextField: public uiBase
		{
		private:
			intptr_t				_hwnd_parent;
			intptr_t				_hwnd;
			SPtr<TexFont>			_font;
			intptr_t				_original_edit_proc;
		private:
			int32_t					_display_index;
		private:
			wfunctor1<void, const SPtr<uiTextField>&>
									_cb_on_text_input_complete;
		public:
			uiTextField(intptr_t parent, const SPtr<TexFont>& font, bool password_mode = false);		// font ownership is passed in
		protected:
			~uiTextField();
		protected:
			class EDIT_PROC;
		public:
			const SPtr<TexFont>&	getFont() const;
		public:
			void					onDraw();
		public:
			void					setFocus();
			wcs						getText() const;
			void					setText(const wcs& text);
			void					setTextLimit(size_t limit);
			size_t					getTextLimit() const;
			bool					isFocused() const;
		public:
			void					setOnTextInputComplete(const wfunctor1<void, const SPtr<uiTextField>&>& cb);
		protected:
			void					onMousePressed(const Vec2& pos, uint32_t button);
			void					onTextInputComplete();
		private:
			void					update_display_index();
			size_t					displayable(const wcs& text);
		public:
			friend class EDIT_PROC;
		};
	};
};

