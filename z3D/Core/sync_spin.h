
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// controlling spin for engine synchronization primitives
		// not designed for derivation
		class sync_spin
		{
		private:
			static volatile uint32_t			 	_spin;
		public:
			static void								spin();
			static void								spin_yield();
		private:
			sync_spin();
		};
	};
};

