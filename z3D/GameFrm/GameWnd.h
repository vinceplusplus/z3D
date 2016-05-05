
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace GameFrm
	{
		class W32Wnd;

		class GameWnd: public RefBase
		{
		private:
			class Wnd;
			Wnd*					_wnd;
		private:
			SPtr<Input>				_input;
		public:
			GameWnd();
		protected:
			~GameWnd();
		public:
			void					setInput(const SPtr<Input>& input);
		public:
			W32Wnd*					getWnd() const;
		};
	};
};

