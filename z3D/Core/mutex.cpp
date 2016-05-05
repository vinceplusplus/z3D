
#include "mutex.h"

#include "aligned_storage_inl.h"

#include "interlocked.h"

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0403

#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		mutex::mutex()
		{
			InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION*)_cs.allocate<CRITICAL_SECTION>(), 4000);
		}
		mutex::~mutex()
		{
			DeleteCriticalSection((CRITICAL_SECTION*)&_cs.get<CRITICAL_SECTION>());
		}
		void				mutex::lock()
		{
			EnterCriticalSection((CRITICAL_SECTION*)&_cs.get<CRITICAL_SECTION>());
		}
		void				mutex::unlock()
		{
			LeaveCriticalSection((CRITICAL_SECTION*)&_cs.get<CRITICAL_SECTION>());
		}
		bool				mutex::try_lock()
		{
			return !!TryEnterCriticalSection((CRITICAL_SECTION*)&_cs.get<CRITICAL_SECTION>());
		}
		void				mutex::set_spin_count(uint32_t c)
		{
			SetCriticalSectionSpinCount((CRITICAL_SECTION*)&_cs.get<CRITICAL_SECTION>(), (DWORD)c);
		}
	};
};

