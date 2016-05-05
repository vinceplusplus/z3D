
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		template<class MUTEX>
		class scoped_lock
		{
		private:
			MUTEX&			_m;
		public:
			scoped_lock(MUTEX& m):_m(m) {_m.lock();}
			~scoped_lock() {_m.unlock();}
		private:
			scoped_lock(const scoped_lock&);
			scoped_lock&		operator=(const scoped_lock&);
		};
	};
};

