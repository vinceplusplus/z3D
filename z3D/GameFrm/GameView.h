
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace GameFrm
	{
		class W32Wnd;

		class GameView: public RefBase
		{
		private:
			class Wnd;
			Wnd*			_wnd;
		private:
			int				_dx;
			int				_dy;
		public:
			GameView();
		protected:
			~GameView();
		public:
			int				dx() const;
			int				dy() const;
		public:
			W32Wnd*			getWnd() const;
		};
	};
};

