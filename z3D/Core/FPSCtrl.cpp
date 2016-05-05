
#include "FPSCtrl.h"

namespace z3D
{
	namespace Core
	{
		FPSCtrl::FPSCtrl(double fps)
		{
			setFPS(fps);
			start();
		}
		void			FPSCtrl::start()
		{
			_timer.update();
			_next_frame_tick = _timer.cur() + _one_frame_ticks;
		}
		void			FPSCtrl::update()
		{
			_timer.update();
		}
		bool			FPSCtrl::nextFrame()
		{
			if(_timer.cur() >= _next_frame_tick)
			{
				_next_frame_tick += _one_frame_ticks;
				return true;
			}
			return false;
		}
		void			FPSCtrl::setFPS(double fps)
		{
			_one_frame_ticks = (uint64_t)(_timer.freq() / fps);
			_frame_length = (double)_one_frame_ticks / _timer.freq();
		}
		double			FPSCtrl::getFPS() const
		{
			return (double)_timer.freq() / _one_frame_ticks;
		}
		const Timer&	FPSCtrl::getTimer() const
		{
			return _timer;
		}
		double			FPSCtrl::frame_length() const
		{
			return _frame_length;
		}
	};
};


