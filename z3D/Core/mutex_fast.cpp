
#include "mutex_fast.h"
#include "sync_spin.h"

#include "interlocked.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		mutex_fast::mutex_fast()
		{
			_seq = 0xffffffff;
			_cur = 0;
			_tid = 0;
			_ec = 0;
		}
		void				mutex_fast::lock() volatile
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) == sizeof(DWORD), WRONG_ASSUMPTION );
			uint32_t tid = (uint32_t)GetCurrentThreadId();
			if(tid == _tid)
			{
				++_ec;
				return;
			}
			uint32_t seq = interlocked::increment(&_seq);

			while(_cur != seq)
				Sleep(0);

			_ec = 1;
			_tid = tid;
		}
		void				mutex_fast::unlock() volatile
		{
			if(--_ec)
				return;

			_tid = 0;
			++_cur;
		}
		bool				mutex_fast::try_lock() volatile
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) == sizeof(DWORD), WRONG_ASSUMPTION );
			uint32_t tid = (uint32_t)GetCurrentThreadId();
			if(tid == _tid)
			{
				++_ec;
				return true;
			}

			if(_tid)
				return false;

			uint32_t org_seq = _seq;
			if(_cur != (uint32_t)(org_seq + 1))
				return false;
			if(interlocked::compare_exchange(&_seq, (uint32_t)(org_seq + 1), org_seq) != org_seq)
				return false;

			_ec = 1;
			_tid = tid;

			return true;
		}
	};
};

