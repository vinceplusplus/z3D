
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class TimerService
		{
		public:
			TimerService();
			~TimerService();
		private:
			static void			getCPUTicks(uint64_t& cputicks, uint64_t& ticks);
			static uint64_t		getCPUSpeed();
			static uint64_t		getCurCPUTick();
		};
	};
};

