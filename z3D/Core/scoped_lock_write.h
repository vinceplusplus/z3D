
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		template<class MUTEX_RW>
		class scoped_lock_write
		{
		private:
			MUTEX_RW&		_rw;
		public:
			scoped_lock_write(MUTEX_RW& m):_rw(m) {_rw.lock_write();}
			~scoped_lock_write() {_rw.unlock_write();}
		private:
			scoped_lock_write(const scoped_lock_write&);
			scoped_lock_write&	operator=(const scoped_lock_write&);
		};
	};
};

