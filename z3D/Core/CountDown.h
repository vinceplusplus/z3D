
#pragma once

#include "predecl.h"
#include "Counter.h"

namespace z3D
{
	namespace Core
	{
		class CountDown
		{
		private:
			Counter					_counter;
			REAL					_countdown;
		public:
			CountDown();
			virtual ~CountDown();
		public:
			void					setCountDown(REAL countdown);
			REAL					getCountDown() const;
		public:
			void					start();
			void					update();
		public:
			REAL					remaining() const;
		public:
			void					setRemaining(REAL remaining_time);
		};
	}
}

