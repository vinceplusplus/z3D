
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// read/write lock, plays more fairly than mutex_rw
		// not designed for derivation
		class mutex_rw2
		{
		private:
			volatile uint32_t	_count;
			volatile uint32_t	_writing;
		public:
			mutex_rw2();
		public:
			void				lock_write();
			void				lock_read();
			void				unlock_write();
			void				unlock_read();
		private:
			mutex_rw2(const mutex_rw2&);
			mutex_rw2&			operator=(const mutex_rw2&);
		};
	};
};

