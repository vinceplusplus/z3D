
#pragma once

#include "predecl.h"

#include "aligned_storage.h"

namespace z3D
{
	namespace Core
	{
		// mutex implementation features reentrantable
		// not designed for derivation
		class mutex
		{
		private:
			aligned_storage<24, 4>
								_cs;
		public:
			mutex();
			~mutex();
		public:
			void				lock();
			void				unlock();
			bool				try_lock();
		public:
			void				set_spin_count(uint32_t c);
		private:
			mutex(const mutex&);
			mutex&				operator=(const mutex&);
		};
	};
};

