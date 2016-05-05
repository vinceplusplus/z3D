
#include "GameWnd.h"
#include "Input.h"

#include "W32Wnd.h"

namespace z3D
{
	namespace GameFrm
	{
		class GameWnd::Wnd: public W32Wnd
		{
		private:
			SPtr<Input>		_input;
		public:
			Wnd()
			{}
		public:
			~Wnd()
			{}
		public:
			void			setInput(const SPtr<Input>& input)
			{
				_input = input;
			}
		public:
			bool			wm_close()
			{
				PostQuitMessage(0);
				return true;
			}
		public:
			bool			wm_unknown(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& ret)
			{
				switch(message)
				{
				case WM_ENTERMENULOOP:
				case WM_ENTERSIZEMOVE:
					_input->removeMouseClipping();
					break;
				}
				return false;
			}
		};

		GameWnd::GameWnd()
		{
			_wnd = new Wnd();
		}
		GameWnd::~GameWnd()
		{
			delete _wnd;
		}
		void					GameWnd::setInput(const SPtr<Input>& input)
		{
			_wnd->setInput(input);
		}
		W32Wnd*					GameWnd::getWnd() const
		{
			return _wnd;
		}
	};
};

