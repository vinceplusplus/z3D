
#include "CountDown.h"

namespace z3D
{
	namespace Core
	{
		CountDown::CountDown()
		{
			_countdown = 10;
		}
		CountDown::~CountDown()
		{
		}
		void					CountDown::setCountDown(REAL countdown)
		{
			_countdown = countdown;
		}
		REAL					CountDown::getCountDown() const
		{
			return _countdown;
		}
		void					CountDown::start()
		{
			_counter.start();
		}
		void					CountDown::update()
		{
			_counter.update();
		}
		REAL					CountDown::remaining() const
		{
			return _countdown - _counter.elapsedTime();
		}
		void					CountDown::setRemaining(REAL remaining_time)
		{
			_counter.setElapsedTime(_countdown - remaining_time);
		}
	}
}

