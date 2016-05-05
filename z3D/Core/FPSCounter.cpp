

#include "FPSCounter.h"

namespace z3D
{
	namespace Core
	{
		FPSCounter::FPSCounter()
		{
			_count=0;
			_fps=0;
			_timer.update();
			_last=_timer.cur();
		}
		FPSCounter::~FPSCounter() {}
		int			FPSCounter::getFPS()
		{
			++_count;
			_timer.update();
			if(_timer.cur()-_last>=_timer.freq())
			{
				_fps=_count;
				_count=0;
				_last=_timer.cur();
			}
			return _fps;
		}
	};
};


