
#pragma once

#include "predecl.h"
#include "Timer.h"

namespace z3D
{
	namespace Core
	{
		class FPSCtrl
		{
		private:
			Timer			_timer;
			uint64_t		_next_frame_tick;
			uint64_t		_one_frame_ticks;
			double			_frame_length;
		public:
			FPSCtrl(double fps = 10.0f);
		public:
			void			start();
			void			update();
		public:
			bool			nextFrame();
		public:
			void			setFPS(double fps);
			double			getFPS() const;
		public:
			const Timer&	getTimer() const;
		public:
			double			frame_length() const;
		};
	};
};


