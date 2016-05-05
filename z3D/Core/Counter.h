
#pragma once

#include "predecl.h"
#include "Timer.h"

namespace z3D
{
	namespace Core
	{
		class Counter
		{
		private:
			uint64_t	_last;
			Timer		_timer;
		public:
			Counter();
		public:
			void		start();
			void		update();
		public:
			uint64_t	last() const;
			Timer		timer() const;
		public:
			uint64_t	last();
			Timer		timer();
		public:
			uint64_t	elapsedTicks() const;
			REAL		elapsedTime() const;
			double		elapsedTimeDOUBLE() const;
		public:
			void		setElapsedTime(REAL elapsed_time);
			void		setElapsedTime(double elapsed_time);
			void		setElapsedTicks(uint64_t elapsed_ticks);
		public:
			Counter(const Counter& rhs);
			Counter&	operator=(const Counter& rhs);
		};
	};
};
