
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// read/write lock, favors slightly to write
		// not designed for derivation
		class mutex_rw
		{
		private:
			volatile uint32_t	_count;
			volatile uint32_t	_write_count;
		public:
			mutex_rw();
		public:
			void				lock_write();
			void				lock_read();
			void				unlock_write();
			void				unlock_read();
		private:
			mutex_rw(const mutex_rw&);
			mutex_rw&			operator=(const mutex_rw&);
		};
	};
};

