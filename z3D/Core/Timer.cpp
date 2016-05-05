
#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmsystem.h>
#include <intrin.h>

#pragma intrinsic(__rdtsc, __cpuid)

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		Timer::Timer()
		{
			_impl = _timer_impl;
			switch(_impl)
			{
			case TIMER_IMPL_QPC:
				_freq = _qpc_freq;
				QueryPerformanceCounter((LARGE_INTEGER*)&_cur);
				_last = _cur;
				break;
			case TIMER_IMPL_TSC:
				_freq = _cpu_speed;
				_cur = _last = __rdtsc();
				break;
			case TIMER_IMPL_TGT:
				_freq = 1000;
				_cur = _last = timeGetTime();
				break;
			}
		}
		void		Timer::update()
		{
			_last = _cur;
			switch(_impl)
			{
			case TIMER_IMPL_QPC:
				QueryPerformanceCounter((LARGE_INTEGER*)&_cur);
				break;
			case TIMER_IMPL_TSC:
				_cur = __rdtsc();
				break;
			case TIMER_IMPL_TGT:
				_cur = timeGetTime();
				_cur |= (_last & 0xffffffff00000000ui64);
				if(_cur < _last)
					_cur += 0x100000000ui64;
				break;
			}
		}
		uint64_t	Timer::freq() const {return _freq;}
		uint64_t	Timer::cur() const {return _cur;}
		uint64_t	Timer::last() const {return _last;}
		uint64_t&	Timer::cur() {return _cur;}
		uint64_t&	Timer::last() {return _last;}
		uint64_t	Timer::elapsedTicks() const
		{
			return _cur - _last;
		}
		REAL		Timer::elapsedTime() const
		{
			return (REAL)elapsedTimeDOUBLE();
		}
		double		Timer::elapsedTimeDOUBLE() const
		{
			return ((double)(_cur - _last)) / _freq;
		}
		Timer::Timer(const Timer& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));
		}
		Timer&		Timer::operator=(const Timer& rhs)
		{
			memcpy(this, &rhs, sizeof(*this));

			return *this;
		}
	};
};

