
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// mutex implementation features reentrantable
		// not designed for derivation
		class mutex_fast
		{
		private:
			volatile uint32_t	_seq;
			volatile uint32_t	_cur;
			volatile uint32_t	_tid;
			volatile uint32_t	_ec;
		public:
			mutex_fast();
		public:
			void				lock() volatile;
			void				unlock() volatile;
			bool				try_lock() volatile;
		private:
			mutex_fast(const mutex_fast&);
			mutex_fast&			operator=(const mutex_fast&);
		};
	};
};

