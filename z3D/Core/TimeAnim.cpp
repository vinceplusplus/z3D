
#include "TimeAnim.h"
#include "Timer.h"

#include "MathFunc.h"

namespace z3D
{
	namespace Core
	{
		TimeAnimCfg::TimeAnimCfg() : _FLAGS(0)
		{}
		TimeAnimCfg::TimeAnimCfg(REAL start, REAL end, ptrdiff_t loops, REAL loop_start, REAL loop_end, REAL time)
			:	_start(start), _end(end), _loops(loops), _loop_start(loop_start), _loop_end(loop_end), _time(time),
				_flag_start(1), _flag_end(1), _flag_loops(1), _flag_loop_start(1), _flag_loop_end(1), _flag_time(1),
				_flag_speed(0), _flag_needs_update(0), _flag_enable_auto_advance(0)
		{
			rectify();
		}
		TimeAnimCfg::TimeAnimCfg(REAL start, REAL end, ptrdiff_t loops, REAL time)
			:	_start(start), _end(end), _loops(loops), _loop_start(start), _loop_end(end), _time(time),
				_flag_start(1), _flag_end(1), _flag_loops(1), _flag_loop_start(1), _flag_loop_end(1), _flag_time(1),
				_flag_speed(0), _flag_needs_update(0), _flag_enable_auto_advance(0)
		{
			rectify();
		}
		TimeAnimCfg::TimeAnimCfg(REAL start, REAL end, ptrdiff_t loops)
			:	_start(start), _end(end), _loops(loops), _loop_start(start), _loop_end(end), _time(start),
				_flag_start(1), _flag_end(1), _flag_loops(1), _flag_loop_start(1), _flag_loop_end(1), _flag_time(1),
				_flag_speed(0), _flag_needs_update(0), _flag_enable_auto_advance(0)
		{
			rectify();
		}
		void					TimeAnimCfg::rectify()
		{
			if(!FINITE(_start) || !FINITE(_end) || !FINITE(_loop_start) || !FINITE(_loop_end) || !FINITE(_time))
			{
				_start = -1;
				_end = 0;
				_loops = 0;
				_loop_start = -1;
				_loop_end = 0;
				_time = 0;
			}
		}
		TimeAnimCfg&			TimeAnimCfg::setLoops(ptrdiff_t loops)
		{
			_loops = loops;
			_flag_loops = 1;

			return *this;
		}
		TimeAnimCfg&			TimeAnimCfg::setSpeed(REAL speed)
		{
			_speed = speed;
			_flag_speed = 1;

			return *this;
		}
		TimeAnimCfg&			TimeAnimCfg::setTime(REAL t)
		{
			_time = t;
			_flag_time = 1;

			return *this;
		}
		TimeAnimCfg&			TimeAnimCfg::setNeedsUpdate(bool needs)
		{
			_needs_update = needs;
			_flag_needs_update = 1;

			return *this;
		}
		TimeAnimCfg&			TimeAnimCfg::enableAutoAdvance(bool enable)
		{
			_enable_auto_advance = enable;
			_flag_enable_auto_advance = 1;

			return *this;
		}
		TimeAnimCfg&			TimeAnimCfg::preserveTime()
		{
			_flag_time = 0;

			return *this;
		}
		TimeAnimCfg&			TimeAnimCfg::setStartIndividualMin()
		{
			_flag_start = 2;

			return *this;
		}
		//TimeAnimCfg&			TimeAnimCfg::setStartAggregateMin()
		//{
		//	_flag_start = 3;

		//	return *this;
		//}
		TimeAnimCfg&			TimeAnimCfg::setEndIndividualMax()
		{
			_flag_end = 2;

			return *this;
		}
		//TimeAnimCfg&			TimeAnimCfg::setEndAggregateMax()
		//{
		//	_flag_end = 3;

		//	return *this;
		//}
		TimeAnimCfg&			TimeAnimCfg::setTimeToStart()
		{
			_flag_time = 2;

			return *this;
		}

		TimeAnim::TimeAnim()
		{
			memset(this, 0, sizeof(*this));
			_speed = 1.0f;
			set(TimeAnimCfg(-1, 0, 0));

			_needs_update = true;
		}
		TimeAnim::TimeAnim(const TimeAnimCfg& cfg)
		{
			memset(this, 0, sizeof(*this));
			_speed = 1.0f;
			set(cfg);

			_needs_update = true;
		}
		TimeAnim::TimeAnim(const TimeAnim& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));

			_needs_update = true;
		}
		TimeAnim&				TimeAnim::operator=(const TimeAnim& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));

			_needs_update = true;

			return *this;
		}
		REAL					TimeAnim::getStart() const					{	return _reverse ? _end : _start;								}
		REAL					TimeAnim::getEnd() const					{	return _reverse ? _start : _end;								}
		ptrdiff_t				TimeAnim::getLoops() const					{	return _loops;													}
		REAL					TimeAnim::getLoopStart() const				{	return _reverse ? _end - _loop_start + _start : _loop_start;	}
		REAL					TimeAnim::getLoopEnd() const				{	return _reverse ? _end - _loop_end + _start : _loop_end;		}
		REAL					TimeAnim::getSpeed() const					{	return _speed;													}
		REAL					TimeAnim::getTime() const
		{
			if(!_reverse)
				return _time;
			else
				return _end - _time + _start;
		}
		bool					TimeAnim::needsUpdate() const				{	return _enable_auto_advance || !!_needs_update;					}
		bool					TimeAnim::isAutoAdvanceEnabled() const		{	return !!_enable_auto_advance;									}
		bool					TimeAnim::isStopped() const					{	return _time >= _end && _loops == 0;							}
		void					TimeAnim::setLoops(ptrdiff_t loops)
		{
			_loops = loops;
		}
		void					TimeAnim::setSpeed(REAL speed)
		{
			_speed = speed;
		}
		void					TimeAnim::setTime(REAL time)
		{
			time = min(max(_start, time), _end);
			if(_reverse)
				time = _end - (time - _start);

			if(_time == time)
				return;
			_time = time;
			_needs_update = true;
		}
		void					TimeAnim::enableAutoAdvance(bool enable)
		{
			_enable_auto_advance = enable;
			if(_enable_auto_advance)
			{
				_auto_advance_timer.update();
				_auto_advance_last_tick = _auto_advance_timer.cur();
			}
		}
		void					TimeAnim::set(REAL start, REAL end, ptrdiff_t loops, REAL loop_start, REAL loop_end, REAL time)
		{
			_start = start;
			_end = end;
			_loops = loops;
			_loop_start = loop_start;
			_loop_end = loop_end;
			_time = time;

			SORT(_start, _end);
			SORT(_loop_start, _loop_end);
			_loop_start = max(_start, _loop_start);
			_loop_end = min(_loop_end, _end);
			_time = min(max(_start, _time), _end);

			_reverse = false;

			if(loop_start > loop_end)
			{
				_reverse = true;

				_loop_start = _end - (loop_start - _start);
				_loop_end = _end - (loop_end - _start);

				_time = _end - (_time - _start);
			}
		}
		void					TimeAnim::set(const TimeAnimCfg& cfg)
		{
			if(cfg._flag_start || cfg._flag_end || cfg._flag_loops || cfg._flag_loop_start || cfg._flag_loop_end || cfg._flag_time)
			{
				REAL start, end, loop_start, loop_end, time;
				ptrdiff_t loops;

				switch(cfg._flag_start)
				{
				case 0:
					start = getStart();
					break;
				case 1:
					start = cfg._start;
					break;
				default:
					start = 0;
				}
				switch(cfg._flag_end)
				{
				case 0:
					end = getEnd();
					break;
				case 1:
					end = cfg._end;
					break;
				default:
					end = 10;
				}
				switch(cfg._flag_loops)
				{
				case 0:
					loops = getLoops();
					break;
				case 1:
					loops = cfg._loops;
					break;
				default:				// unreachable, can be omitted
					loops = 0;
				}
				switch(cfg._flag_loop_start)
				{
				case 0:
					loop_start = start;
					break;
				case 1:
					loop_start = cfg._loop_start;
					break;
				default:				// unreachable, can be omitted
					loop_start = start;
				}
				switch(cfg._flag_loop_end)
				{
				case 0:
					loop_end = end;
					break;
				case 1:
					loop_end = cfg._loop_end;
					break;
				default:				// unreachable, can be omitted
					loop_end = end;
				}
				switch(cfg._flag_time)
				{
				case 0:
					time = getTime();
					break;
				case 1:
					time = cfg._time;
					break;
				case 2:
					time = start;
					break;
				default:
					time = start;
				}

				set(start, end, loops, loop_start, loop_end, time);
			}

			if(cfg._flag_speed)
				setSpeed(cfg._speed);

			if(cfg._flag_needs_update)
				setNeedsUpdate(!!cfg._needs_update);

			if(cfg._flag_enable_auto_advance)
				enableAutoAdvance(!!cfg._enable_auto_advance);
		}
		void					TimeAnim::set(const TimeAnim& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));

			_needs_update = true;
		}
		void					TimeAnim::setNeedsUpdate(bool needs)
		{
			_needs_update = needs;
		}
		bool					TimeAnim::advance(REAL dt)
		{
			REAL oldtime = _time;

			_time += dt * _speed;
			if(!_loops)
			{
				if(_time > _end)
					_time = _end;
			}
			else
			{
				if(_time > _loop_end)
				{
					REAL loop_duration = _loop_end - _loop_start;
					_time = _loop_start + FMOD(_time - _loop_end, loop_duration);
					if(!FINITE(_time))										// most of the time, not necessary
					{
						_time = _end;
						_loops = 0;
					}
					if(_loops > 0)
						--_loops;
				}
			}
			if(oldtime != _time)
				_needs_update = true;

			return needsUpdate();
		}
		bool					TimeAnim::autoAdvance()
		{
			if(_enable_auto_advance)
			{
				_auto_advance_timer.update();
				advance((REAL)(((double)(_auto_advance_timer.cur() - _auto_advance_last_tick)) / (double)_auto_advance_timer.freq()));
				_auto_advance_last_tick = _auto_advance_timer.cur();

				_needs_update = true;
			}

			return needsUpdate();
		}
	};
};

