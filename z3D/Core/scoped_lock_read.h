
#pragma once

#include "predecl.h"
#include "mutex_rw.h"
#include "mutex_rw2.h"

namespace z3D
{
	namespace Core
	{
		template<class MUTEX_RW>
		class scoped_lock_read
		{
		private:
			MUTEX_RW&		_rw;
		public:
			scoped_lock_read(MUTEX_RW& m):_rw(m) {_rw.lock_read();}
			~scoped_lock_read() {_rw.unlock_read();}
		private:
			scoped_lock_read(const scoped_lock_read&);
			scoped_lock_read&	operator=(const scoped_lock_read&);
		};
	};
};

