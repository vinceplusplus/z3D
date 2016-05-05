
#include "mutex_rw.h"
#include "sync_spin.h"

#include "interlocked.h"

namespace z3D
{
	namespace Core
	{
		mutex_rw::mutex_rw()
		{
			_count = 0;
			_write_count = 0;
		}
		void				mutex_rw::lock_write()
		{
			interlocked::increment(&_write_count);
			while(_count & 0x7ff)
				sync_spin::spin();
			while(interlocked::compare_exchange(&_count, 2048, 0))
				sync_spin::spin();
		}
		void				mutex_rw::lock_read()
		{
			while(_write_count)
				sync_spin::spin();
			while(interlocked::exchange_add(&_count, 1) > 2048)
			{
				interlocked::exchange_add(&_count, -1);
				sync_spin::spin();
			}
		}
		void				mutex_rw::unlock_write()
		{
			interlocked::exchange_add(&_count, -2048);
			interlocked::decrement(&_write_count);
		}
		void				mutex_rw::unlock_read()
		{
			interlocked::exchange_add(&_count, -1);
		}
	};
};

