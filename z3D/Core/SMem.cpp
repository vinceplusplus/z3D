
#include "SMem.h"

#include "interlocked.h"

namespace z3D
{
	namespace Core
	{
		void					SMem::___ctor()
		{
			_block = 0;
			_size = 0;
		}
		void					SMem::___ctor(const SMem& rhs)
		{
			if(!rhs._block)
			{
				___ctor();
				return;
			}
			_block = rhs._block;
			_size = rhs._size;
			_dealloc_func = rhs._dealloc_func;

			interlocked::increment(&_block->ref_count);
		}
		void					SMem::___dtor()
		{
			if(!_block)
				return;
			if(interlocked::decrement(&_block->ref_count) != 0)
				return;
			if(_dealloc_func.valid())
				_dealloc_func(_block);
		}
		SMem::SMem()
		{
			___ctor();
		}
		SMem::SMem(const SMem& rhs)
		{
			___ctor(rhs);
		}
		SMem::~SMem()
		{
			___dtor();
		}
		SMem&					SMem::operator=(const SMem& rhs)
		{
			if(this == &rhs)
				return *this;
			___dtor();
			___ctor(rhs);
			return *this;
		}
		size_t					SMem::getRequiredSize(size_t data_size)
		{
			return sizeof(BLOCK) - 1 + data_size;
		}
		SMem					SMem::create(void* required_mem, size_t data_size, const functor1<void, void*>& dealloc_func)
		{
			SMem sm;
			sm._block = (BLOCK*)required_mem;
			sm._size = data_size;
			sm._dealloc_func = dealloc_func;
			interlocked::exchange(&sm._block->ref_count, 1);

			return sm;
		}
		void*					SMem::get() const
		{
			return _block->data;
		}
		size_t					SMem::size() const
		{
			return _size;
		}
		void					SMem::reset()
		{
			___dtor();
			___ctor();
		}
	};
};


