
#pragma once

#include "predecl.h"
#include "Timer.h"

namespace z3D
{
	namespace Core
	{
		class FPSCounter
		{
		private:
			Timer			_timer;
			uint64_t		_last;
			int				_count;
			int				_fps;
		public:
			FPSCounter();
			~FPSCounter();
		public:
			int				getFPS();
		};
	};
};
