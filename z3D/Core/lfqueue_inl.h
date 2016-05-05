
#pragma once

#include "lfqueue.h"
#include "mem_pool_inl.h"

#include "interlocked.h"

#pragma warning(disable:4311)

namespace z3D
{
	namespace Core
	{
		template<class T>
		lfqueue<T>::lfqueue()
		{
			_pool = NULL;

			_head = _tail = NULL;
			_pops = _pushes = 0;

			_size = 0;
		}
		template<class T>
		lfqueue<T>::~lfqueue()
		{
			Z_ASSERT( _pool->get_blocks_in_use() == 1 );

			_pool->dealloc(_head);

			if(_pool)
				_pool->dispose();
		}
		template<class T>
		bool							lfqueue<T>::create_pool(uint32_t size)					// not thread-safe
		{
			if(_pool)
				return false;
			_pool = mem_pool<node>::create(size + 1);
			if(!_pool)
				return false;
			_head = _tail = _pool->alloc();
			_head->next = NULL;

			return true;
		}
		template<class T>
		bool							lfqueue<T>::enq(const T& value)
		{
			Z_STATIC_ASSERT( sizeof(node*) == sizeof(uint32_t), POINTER_SIZE_ASSUMPTION_WRONG );

			Z_ASSERT( _pool );
			node* n = _pool->alloc();
			if(!n)
				return false;
			n->value = value;
			n->next = NULL;

			uint32_t pushes;
			node* tail;

			while(true)
			{
				pushes = _pushes;
				tail = _tail;

				if(interlocked::compare_exchange((uint32_t*)&_tail->next, (uint32_t)n, NULL) == NULL)
					break;
				else
					interlocked::compare_exchange_64((uint64_t*)&_tail, (uint64_t)_tail->next | (((uint64_t)(pushes + 1)) << 32), (uint64_t)tail | (((uint64_t)pushes) << 32));
				//sync_spin::spin_yield();
			}
			interlocked::compare_exchange_64((uint64_t*)&_tail, (uint64_t)_tail->next | (((uint64_t)(pushes + 1)) << 32), (uint64_t)tail | (((uint64_t)pushes) << 32));

			interlocked::increment(&_size);

			return true;
		}
		template<class T>
		bool							lfqueue<T>::deq(T& value)
		{
			Z_STATIC_ASSERT( sizeof(node*) == sizeof(uint32_t), POINTER_SIZE_ASSUMPTION_WRONG );

			node* head;

			while(true)
			{
				uint32_t pops = _pops;
				uint32_t pushes = _pushes;
				head = _head;
				node* next = head->next;

				if(pops != _pops)
					continue;

				if(head == _tail)
				{
					if(next == NULL)
						return false;
					interlocked::compare_exchange_64((uint64_t*)&_tail, (uint64_t)next | (((uint64_t)(pushes + 1)) << 32), (uint64_t)head | (((uint64_t)pushes) << 32));
				}
				else if(next != NULL)
				{
					value = next->value;
					uint64_t comperand = (uint64_t)head | (((uint64_t)pops) << 32);
					if(interlocked::compare_exchange_64((uint64_t*)&_head, (uint64_t)next | (((uint64_t)(pops + 1)) << 32), comperand) == comperand)
						break;
				}
				//sync_spin::spin_yield();
			}

			interlocked::decrement(&_size);

			_pool->dealloc(head);

			return true;
		}
		template<class T>
		bool							lfqueue<T>::empty() const
		{
			return _head == _tail;
		}
		template<class T>
		const T&						lfqueue<T>::peep() const
		{
			node* next = _head->next;
			if(next)
				return next->value;
			return _head->value;
		}
		template<class T>
		uint32_t						lfqueue<T>::size() const
		{
			return _size;
		}
		template<class T>
		void							lfqueue<T>::clear()
		{
			T t;
			while(!empty())
				deq(t);
		}
	};
};

#pragma warning(default:4311)

