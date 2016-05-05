
#pragma once

#include "predecl.h"

#include "aligned_storage.h"
#include "functor.h"

namespace z3D
{
	namespace Core
	{
		class W32Thread
		{
		private:
			class pImpl;

			typed_aligned_storage<pImpl, 12, 4, true>
								_pimpl;
			functor0<void>		_proc;
		public: 
			W32Thread();
			virtual ~W32Thread();
		public:
			void					setProc(const functor0<void>& proc);
			const functor0<void>&	getProc() const;
		public:
			bool				start(bool suspend_after_create = false);
			void				stop(uint32_t timeout, bool waitForever);
		public:
			void				suspend();
			void				resume();
		public:
			uint32_t			getThreadID() const;
		private:
			void				__run();
		protected:
			virtual void		run();
		public:
			static void			sleep(uint32_t interval);
			static uint32_t		getCurThreadID();
		public:
			void				setNeedsStop(bool b);
			bool				needsStop() const;
		public:
			bool				isStopped() const;
		};
	};
};

