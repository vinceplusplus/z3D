
#include "shared_count.h"

#include "interlocked.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

#include "mem_pool_inl.h"

#include "mem_alloc.h"

namespace z3D
{
	namespace Core
	{
		void			shared_count::___ctor()
		{
			uint64_t* p = _pool->alloc();
			if(!p)
				p = (uint64_t*)mem_alloc::__aligned_alloc(sizeof(uint64_t), Z_GET_ALIGNMENT<uint32_t>::value);

			*(_ref_count = (uint32_t*)p) = 1;
			*(_ref_count + 1) = 1;
		}
		void			shared_count::___ctor(const shared_count& sc)
		{
			_ref_count = sc._ref_count;
			*(_ref_count + 1) = *(sc._ref_count + 1);

			interlocked::increment(_ref_count);
		}
		void			shared_count::___dtor()
		{
			if(!interlocked::decrement(_ref_count))
			{
				if(!_pool->owns((uint64_t*)_ref_count))
					mem_alloc::__aligned_free((uint64_t*)_ref_count);
				else
					_pool->dealloc((uint64_t*)_ref_count);
			}
		}
		shared_count::shared_count()
		{
			___ctor();
		}
		shared_count::shared_count(const shared_count& sc)
		{
			___ctor(sc);
		}
		shared_count::~shared_count()
		{
			___dtor();
		}
		shared_count&	shared_count::operator=(const shared_count& sc)
		{
			if(this == &sc)
				return *this;
			___dtor();
			___ctor(sc);
			return *this;
		}
		long				shared_count::operator++()
		{
			return interlocked::increment(_ref_count + 1);
		}
		long				shared_count::operator--()
		{
			return interlocked::decrement(_ref_count + 1);
		}
		bool				shared_count::safe_try_inc()
		{
			uint32_t cur;
			if((cur = use_count()) > 0)
				if(interlocked::compare_exchange(_ref_count + 1, cur + 1, cur) == cur)
					return true;
			return false;
		}
		long				shared_count::use_count() const
		{
			return *(_ref_count + 1);
		}
		bool				shared_count::unique() const
		{
			return use_count() == 1;
		}
		long				shared_count::ref_count() const
		{
			return *_ref_count;
		}
	};
};
