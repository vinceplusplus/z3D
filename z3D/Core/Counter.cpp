
#include "Counter.h"

namespace z3D
{
	namespace Core
	{
		Counter::Counter()
		{
			_last = _timer.cur();
		}
		void		Counter::start()
		{
			_timer.update();
			_last = _timer.cur();
		}
		void		Counter::update()
		{
			_timer.update();
		}
		uint64_t	Counter::last() const{return _last;}
		Timer		Counter::timer() const{return _timer;}
		uint64_t	Counter::last(){return _last;}
		Timer		Counter::timer(){return _timer;}
		uint64_t	Counter::elapsedTicks() const
		{
			return _timer.cur()-_last;
		}
		REAL		Counter::elapsedTime() const
		{
			return (REAL)elapsedTimeDOUBLE();
		}
		double		Counter::elapsedTimeDOUBLE() const
		{
			return ((double)(_timer.cur() - _last)) / _timer.freq();
		}
		void		Counter::setElapsedTime(REAL elapsed_time)
		{
			setElapsedTime((double)elapsed_time);
		}
		void		Counter::setElapsedTime(double elapsed_time)
		{
			setElapsedTicks((uint64_t)(elapsed_time * _timer.freq()));
		}
		void		Counter::setElapsedTicks(uint64_t elapsed_ticks)
		{
			_timer.update();
			_last = _timer.cur() - elapsed_ticks;
		}
		Counter::Counter(const Counter& rhs)
		{
			_last = rhs._last;
			_timer = rhs._timer;
		}
		Counter&	Counter::operator=(const Counter& rhs)
		{
			_last = rhs._last;
			_timer = rhs._timer;

			return *this;
		}
	};
};
