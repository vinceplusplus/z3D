
#include "mutex_rw2.h"
#include "sync_spin.h"

#include "interlocked.h"

namespace z3D
{
	namespace Core
	{
		mutex_rw2::mutex_rw2()
		{
			_count = 0;
			_writing = 0;
		}
		void				mutex_rw2::lock_write()
		{
			while(interlocked::compare_exchange(&_writing, 1, 0))
				sync_spin::spin();
			while(interlocked::compare_exchange(&_count, 2048, 0))
				sync_spin::spin();
		}
		void				mutex_rw2::lock_read()
		{
			while(_writing)
				sync_spin::spin();
			while(interlocked::exchange_add(&_count, 1) > 2048)
			{
				interlocked::exchange_add(&_count, -1);
				sync_spin::spin();
			}
		}
		void				mutex_rw2::unlock_write()
		{
			interlocked::exchange_add(&_count, -2048);
			_writing = 0;
		}
		void				mutex_rw2::unlock_read()
		{
			interlocked::exchange_add(&_count, -1);
		}
	};
};

