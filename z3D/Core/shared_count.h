
#pragma once

#include "predecl.h"
#include "mem_pool.h"

namespace z3D
{
	namespace Core
	{
		// not thread-safe for assignment, but counter allocation, increment, decrement, try increment are thread-safe
		class shared_count
		{
		public:
			enum
			{
				POOL_MAX_BLOCKS = 16384,
			};
		private:
			uint32_t volatile*volatile		_ref_count;						// both address & value can be modified by any threads, the next uint32_t element is the shared_count
			//uint32_t volatile*volatile		_shared_count;					// ditto
		private:
			static mem_pool<uint64_t>*		_pool;
		private:
			void			___ctor();
			void			___ctor(const shared_count& sc);
		private:
			void			___dtor();
		public:
			shared_count();
			shared_count(const shared_count& sc);
		public:
			~shared_count();
		public:
			shared_count&	operator=(const shared_count& sc);
		public:
			long			operator++();							// increment the _shared_count(use count) by one
			long			operator--();							// decrement the _shared_count(use_count) by one
			bool			safe_try_inc();							// safe to try to increment the _shared_count(use count) only if _shared_count(use count) > 0
		public:
			long			use_count() const;
			bool			unique() const;
			long			ref_count() const;						// return the reference count to this shared_count object
		public:
			friend
			bool			operator==(const shared_count& a, const shared_count& b)			{Z_STATIC_ASSERT(sizeof(a) == 4, ASSUMPTION_WRONG); return *(uint32_t*)&a == *(uint32_t*)&b;}
			friend
			bool			operator!=(const shared_count& a, const shared_count& b)			{Z_STATIC_ASSERT(sizeof(a) == 4, ASSUMPTION_WRONG); return *(uint32_t*)&a != *(uint32_t*)&b;}
			friend
			bool			operator<(const shared_count& a, const shared_count& b)			{Z_STATIC_ASSERT(sizeof(a) == 4, ASSUMPTION_WRONG); return *(uint32_t*)&a < *(uint32_t*)&b;}
		public:
			friend class CoreDeinit;
		};
	};
};

