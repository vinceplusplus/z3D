
#include "GameView.h"
#include "W32Wnd.h"

namespace z3D
{
	namespace GameFrm
	{
		class GameView::Wnd: public W32Wnd
		{
		private:
			int				_dx;
			int				_dy;
		public:
			Wnd()
			{
				_dx = 1;
				_dy = 1;
			}
		public:
			~Wnd()
			{}
		public:
			void			post_create()
			{
				wm_size(0, 0, 0);
			}
		public:
			bool			wm_size(WPARAM wparam, int width, int height)
			{
				_dx = getWidth();
				_dy = getHeight();
				return true;
			}
		public:
			int				dx() const
			{
				return _dx;
			}
			int				dy() const
			{
				return _dy;
			}
		};

		GameView::GameView()
		{
			_wnd = new Wnd();
		}
		GameView::~GameView()
		{
			delete _wnd;
		}
		int				GameView::dx() const
		{
			return _wnd->dx();
		}
		int				GameView::dy() const
		{
			return _wnd->dy();
		}
		W32Wnd*			GameView::getWnd() const
		{
			return _wnd;
		}
	};
};

