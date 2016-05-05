
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class Timer
		{
		public:
			enum
			{
				TIMER_IMPL_QPC = 0,
				TIMER_IMPL_TSC,
				TIMER_IMPL_TGT,
			};
		private:
			static char			_cpu_id[16];
			static uint64_t		_cpu_speed;
			static uint8_t		_cpus;
			static uint8_t		_affinities;
			static uint8_t		_intel_speedstep;
			static uint8_t		_timer_impl;
			static uint64_t		_qpc_freq;
		private:
			uint64_t	_freq;
			uint64_t	_cur;
			uint64_t	_last;
			size_t		_impl;
		public:
			Timer();
		public:
			void		update();
		public:
			uint64_t	freq() const;
			uint64_t	cur() const;
			uint64_t	last() const;
		public:
			uint64_t	elapsedTicks() const;
			REAL		elapsedTime() const;
			double		elapsedTimeDOUBLE() const;
		public:
			uint64_t&	cur();
			uint64_t&	last();
		public:
			Timer(const Timer& rhs);
			Timer&		operator=(const Timer& rhs);
		public:
			friend class TimerService;
		};
	};
};
