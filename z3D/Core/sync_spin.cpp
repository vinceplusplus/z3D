
#include "sync_spin.h"
#include "W32Thread.h"

namespace z3D
{
	namespace Core
	{
		void			sync_spin::spin()
		{
			return W32Thread::sleep(_spin = ((_spin + 1) & 3));
		}
		void			sync_spin::spin_yield()
		{
			return W32Thread::sleep(0);
		}

		sync_spin::sync_spin() {}
	};
};

