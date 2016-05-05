
#include "AnimInst.h"
#include "MathFunc.h"
#include "Timer.h"

#include "Constants.h"

#define SAFE_DELETE(x) {if((x)!=NULL){delete (x);(x)=NULL;}}
#define SAFE_DELETE_ARRAY(x) {if((x)!=NULL){delete[] (x);(x)=NULL;}}

namespace z3D
{
	namespace Core
	{
		void AnimInst::___ctor(REAL start, REAL end, int32_t loops, REAL loop_start, REAL loop_end, REAL time)
		{
			setSpeed(1.0f);
			set(start, end, loops, loop_start, loop_end, time);

			_need_update = true;

			_auto_advance = false;
		}

		AnimInst::AnimInst()
		{
			___ctor(0, 0, 0, 0, 0, 0);
		}
		AnimInst::AnimInst(REAL start, REAL end, int32_t loops)
		{
			___ctor(start, end, loops, start, end, start);
		}
		AnimInst::AnimInst(REAL start, REAL end, int32_t loops, REAL time)
		{
			___ctor(start, end, loops, start, end, time);
		}
		AnimInst::AnimInst(REAL start, REAL end, int32_t loops, REAL loop_start, REAL loop_end, REAL time)
		{
			___ctor(start, end, loops, loop_start, loop_end, time);
		}
		AnimInst::~AnimInst()
		{
		}
		REAL			AnimInst::getTime() const
		{
			if(!_reverse)
				return _time;
			return _end - (_time - _start);
		}
		REAL			AnimInst::getStart() const
		{
			if(!_reverse)
				return _start;
			return _end;
		}
		REAL			AnimInst::getEnd() const
		{
			if(!_reverse)
				return _end;
			return _start;
		}
		int32_t			AnimInst::getLoops() const {return _loops;}
		REAL			AnimInst::getLoopStart() const
		{
			if(!_reverse)
				return _loop_start;
			return _end - (_loop_start - _start);
		}
		REAL			AnimInst::getLoopEnd() const
		{
			if(!_reverse)
				return _loop_end;
			return _end - (_loop_end - _start);
		}
		REAL			AnimInst::getSpeed() const {return _speed;}
		bool			AnimInst::need_update() const {return _need_update || _auto_advance;}
		bool			AnimInst::autoAdvanced() const {return !!_auto_advance;}

		void			AnimInst::setLoops(int32_t loops)
		{
			_loops = loops;
		}
		void			AnimInst::setSpeed(REAL speed)
		{
			_speed = speed;
		}
		void			AnimInst::setTime(REAL time)
		{
			time = min(max(_start, time), _end);
			if(_reverse)
				time = _end - (time - _start);

			if(_time == time)
				return;
			_time = time;
			_need_update = true;
		}
		void			AnimInst::autoAdvance(bool auto_advance)
		{
			_auto_advance = auto_advance;
			if(auto_advance)
			{
				_auto_advance_timer.update();
				_auto_advance_last = _auto_advance_timer.cur();
			}
		}
		void			AnimInst::set(REAL start, REAL end, int32_t loops, REAL loop_start, REAL loop_end, REAL time)
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
		void			AnimInst::set(REAL start, REAL end, int32_t loops)
		{
			set(start, end, loops, start, end, start);
		}
		void			AnimInst::set(REAL start, REAL end, int32_t loops, REAL time)
		{
			set(start, end, loops, start, end, time);
		}
		void			AnimInst::set(const AnimInst& other)
		{
			*this = other;
		}
		void			AnimInst::setNeedUpdate(bool need_update)
		{
			_need_update = need_update;
		}
		void			AnimInst::advance(REAL dt)
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
					REAL loop_duration = _loop_end-_loop_start;
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
				_need_update = true;
		}
		void			AnimInst::update()
		{
			if(_auto_advance)
			{
				_auto_advance_timer.update();
				advance((REAL)(((double)(_auto_advance_timer.cur() - _auto_advance_last)) / (double)_auto_advance_timer.freq()));
				_auto_advance_last = _auto_advance_timer.cur();
			}
		}
		bool			AnimInst::stopped() const
		{
			return _time >= _end && _loops == 0;
		}
	};
};
