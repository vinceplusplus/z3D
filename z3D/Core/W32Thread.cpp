
#include "W32Thread.h"

#include "aligned_storage_inl.h"
#include "functor_inl.h"

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

#include <process.h>

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		class W32Thread::pImpl
		{
		public:
			intptr_t	_hThread;
			uint32_t	_dwThreadID;
			int32_t		_bNeedsStop;
		public:
			pImpl()
			{
				memset(this, 0, sizeof(*this));
			}
			~pImpl()
			{
				Z_STATIC_ASSERT( sizeof(_hThread) == sizeof(HANDLE), ASSUMPTION_WRONG );

				if(_hThread && (HANDLE)_hThread != INVALID_HANDLE_VALUE)
					CloseHandle((HANDLE)_hThread);
			}
			static unsigned __stdcall starter(void* arglist)
			{
				((W32Thread*)arglist)->__run();
				return 0;
			}
		};

#include "z_off_debug_new.h"
		W32Thread::W32Thread()
			: _pimpl(new (_pimpl.allocate()) pImpl(), aligned_storage_tag::ctor())
		{}
#include "z_on_debug_new.h"

		W32Thread::~W32Thread()
		{
			stop(0, false);
		}
		void					W32Thread::setProc(const functor0<void>& proc)
		{
			_proc = proc;
		}
		const functor0<void>& 	W32Thread::getProc() const
		{
			return _proc;
		}
		bool W32Thread::start(bool suspend_after_create)
		{
			if(_pimpl.get()._hThread)
				return false;
			Z_STATIC_ASSERT( sizeof(_pimpl.get()._dwThreadID) == sizeof(unsigned int), WRONG_ASSUMPTION );
			_pimpl.get()._hThread = _beginthreadex(NULL, 0, W32Thread::pImpl::starter, this, CREATE_SUSPENDED, (unsigned int*)&_pimpl.get()._dwThreadID);
			if(!_pimpl.get()._hThread)
				return false;
			if(!suspend_after_create)
				ResumeThread((HANDLE)_pimpl.get()._hThread);
			return true;
		}
		void W32Thread::stop(uint32_t timeout, bool waitForever)
		{
			if(!_pimpl.get()._hThread)
				return;
			setNeedsStop(true);
			DWORD ret;
			if(waitForever)
				ret = WaitForSingleObject((HANDLE)_pimpl.get()._hThread, INFINITE);
			else
				ret = WaitForSingleObject((HANDLE)_pimpl.get()._hThread, timeout);
			if(ret != WAIT_OBJECT_0)
				TerminateThread((HANDLE)_pimpl.get()._hThread, 0);
		}
		void W32Thread::suspend()
		{
			SuspendThread((HANDLE)_pimpl.get()._hThread);
		}
		void W32Thread::resume()
		{
			ResumeThread((HANDLE)_pimpl.get()._hThread);
		}
		uint32_t W32Thread::getThreadID() const
		{
			return _pimpl.get()._dwThreadID;
		}
		void				W32Thread::__run()
		{
			if(_proc.valid())
				_proc();
			else
				run();
		}
		void				W32Thread::run() {}
		void W32Thread::sleep(uint32_t interval)
		{
			Sleep(interval);
		}
		uint32_t W32Thread::getCurThreadID()
		{
			return (uint32_t)::GetCurrentThreadId();
		}
		void W32Thread::setNeedsStop(bool b)
		{
			_pimpl.get()._bNeedsStop = b;
		}
		bool W32Thread::needsStop() const {return !!_pimpl.get()._bNeedsStop;}
		bool W32Thread::isStopped() const
		{
			if(!_pimpl.get()._hThread)
				return true;
			if(WaitForSingleObject((HANDLE)_pimpl.get()._hThread, 0) == WAIT_OBJECT_0)
				return true;
			return false;
		}
	};
};
