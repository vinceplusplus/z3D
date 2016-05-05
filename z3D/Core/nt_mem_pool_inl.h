
#pragma once

#include "nt_mem_pool.h"

#include "aligned_storage_inl.h"
#include "interlocked.h"
#include "mem_alloc.h"

namespace z3D
{
	namespace Core
	{
		template<size_t block_size, size_t block_alignment>
		class nt_mem_pool<block_size, block_alignment>::helper
		{
		public:
			enum : size_t
			{
				NODE_ALIGNMENT = Z_GET_ALIGNMENT<nt_mem_pool<block_size, block_alignment>::node>::value,
				DATA_ALIGNMENT = block_alignment,
				ALIGNMENT = static_max<static_max<Z_GET_ALIGNMENT<nt_mem_pool<block_size, block_alignment>>::value, NODE_ALIGNMENT>::value, DATA_ALIGNMENT>::value,
			};

			template<size_t max_blocks>
			class compile_time
			{
			public:
				enum : size_t
				{
					START_NODE_OFFSET = (sizeof(nt_mem_pool<block_size, block_alignment>) + nt_mem_pool<block_size, block_alignment>::helper::NODE_ALIGNMENT - 1) & (~(nt_mem_pool<block_size, block_alignment>::helper::NODE_ALIGNMENT - 1)),
					START_DATA_OFFSET = (START_NODE_OFFSET + sizeof(nt_mem_pool<block_size, block_alignment>::node) * max_blocks + nt_mem_pool<block_size, block_alignment>::helper::DATA_ALIGNMENT - 1) & (~(nt_mem_pool<block_size, block_alignment>::helper::DATA_ALIGNMENT - 1)),
					SIZE = (START_DATA_OFFSET + block_size * max_blocks + nt_mem_pool<block_size, block_alignment>::helper::ALIGNMENT - 1) & (~(nt_mem_pool<block_size, block_alignment>::helper::ALIGNMENT - 1)),
				};
			};

			// ensure the above compile-time constants and the following functions produce the exact same result, i have no ways to make one to serve both compile-time and run-time
			static size_t				get_start_node_offset(size_t max_blocks)
			{
				return (sizeof(nt_mem_pool<block_size, block_alignment>) + NODE_ALIGNMENT - 1) & (~(NODE_ALIGNMENT - 1));
			}
			static size_t				get_start_data_offset(size_t max_blocks)
			{
				return (get_start_node_offset(max_blocks) + sizeof(nt_mem_pool<block_size, block_alignment>::node) * max_blocks + DATA_ALIGNMENT - 1) & (~(DATA_ALIGNMENT - 1));
			}
			static size_t				get_size(size_t max_blocks)
			{
				return (get_start_data_offset(max_blocks) + block_size * max_blocks + sizeof(ALIGNMENT) - 1) & (~(ALIGNMENT - 1));
			}
		};

		template<size_t block_size, size_t block_alignment>
		void					nt_mem_pool<block_size, block_alignment>::__ctor(size_t max_blocks)
		{
			_start_node = (node*)((char*)this + helper::get_start_node_offset(max_blocks));
			_start_data = (NT_TYPE*)((char*)this + helper::get_start_data_offset(max_blocks));
			_end_data = _start_data + max_blocks;

			size_t c = max_blocks - 1;
			for(size_t i = 0 ; i < c ; ++i)
				_start_node[i].next = _start_node + (i + 1);
			_start_node[c].next = NULL;

			_max_blocks = max_blocks;

			_free_start = _start_node;

			_allocs = 0;

			_blocks_in_use = 0;
		}

		template<size_t block_size, size_t block_alignment>
		void					nt_mem_pool<block_size, block_alignment>::__dtor()
		{
			Z_ASSERT( can_dispose() );
		}

		template<size_t block_size, size_t block_alignment>
		size_t					nt_mem_pool<block_size, block_alignment>::get_required_mem_size(size_t max_blocks)
		{
			return helper::get_size(max_blocks);
		}

		template<size_t block_size, size_t block_alignment>
		size_t					nt_mem_pool<block_size, block_alignment>::get_required_mem_alignment()
		{
			return helper::ALIGNMENT;
		}

		template<size_t block_size, size_t block_alignment>
		nt_mem_pool<block_size, block_alignment>*			nt_mem_pool<block_size, block_alignment>::create(size_t max_blocks)
		{
			if(max_blocks <= 0)
				return NULL;

			void* p = mem_alloc::__aligned_alloc(get_required_mem_size(max_blocks), get_required_mem_alignment());
			if(!p)
				return NULL;

			return create(max_blocks, p);
		}

		template<size_t block_size, size_t block_alignment>
		nt_mem_pool<block_size, block_alignment>*			nt_mem_pool<block_size, block_alignment>::create(size_t max_blocks, void* mem)
		{
			Z_STATIC_ASSERT( !(block_alignment & (block_alignment - 1)), WRONG_BLOCK_ALIGNMENT );
			Z_STATIC_ASSERT( !(block_size & (block_alignment - 1)), BLOCK_SIZE_NOT_COMPATIBLE_WITH_BLOCK_ALIGNMENT );

			if(max_blocks <= 0)
				return NULL;

			Z_ASSERT( mem );

			// one more check to ensure users allocate the memory according to get_required_mem_size() & get_required_mem_alignment()
			Z_IS_ALIGNED<helper::ALIGNMENT>(mem);

			nt_mem_pool<block_size, block_alignment>* p = (nt_mem_pool<block_size, block_alignment>*)mem;

			p->__ctor(max_blocks);

			return p;
		}

		//template<size_t block_size, size_t block_alignment>
		//template<size_t max_blocks, size_t storage_size>
		//nt_mem_pool<block_size, block_alignment>*			nt_mem_pool<block_size, block_alignment>::create(char (&storage)[storage_size])
		//{
		//	if(max_blocks <= 0)
		//		return NULL;

		//	INTEGER_VERIFIER<helper::compile_time<max_blocks>::SIZE + helper::ALIGNMENT - 1, storage_size>();

		//	void* p = (void*)(((size_t)storage + helper::ALIGNMENT - 1) & (~(helper::ALIGNMENT - 1)));

		//	return create(max_blocks, p);
		//}

		template<size_t block_size, size_t block_alignment>
		template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
		nt_mem_pool<block_size, block_alignment>*			nt_mem_pool<block_size, block_alignment>::create(aligned_storage<storage_size, storage_alignment>& storage)
		{
			if(max_blocks <= 0)
				return NULL;

			SIZE_VERIFIER<helper::compile_time<max_blocks>::SIZE, storage_size>();
			ALIGNMENT_VERIFIER<helper::ALIGNMENT, storage_alignment>();

			return create(max_blocks, storage.get_unsafe());
		}

		template<size_t block_size, size_t block_alignment>
		void					nt_mem_pool<block_size, block_alignment>::dispose()
		{
			__dtor();
			mem_alloc::__aligned_free((void*)this);
		}

		template<size_t block_size, size_t block_alignment>
		bool					nt_mem_pool<block_size, block_alignment>::can_dispose() const
		{
			return _blocks_in_use == 0;
		}

		template<size_t block_size, size_t block_alignment>
		void*					nt_mem_pool<block_size, block_alignment>::alloc()
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) == 4, POINTER_SIZE_ASSUMPTION_WRONG );
			Z_STATIC_ASSERT( sizeof(uint32_t) == sizeof(void*), POINTER_SIZE_ASSUMPTION_WRONG );

			node* s;
			do
			{
				s = _free_start;
				if(!s)
					return NULL;
				long allocs = _allocs;
				uint64_t comperand = ((uint64_t)s) | (((uint64_t)allocs) << 32);
				if(interlocked::compare_exchange_64((uint64_t*)&_free_start, ((uint64_t)s->next) | (((uint64_t)(allocs + 1)) << 32), comperand) == comperand)
					break;
				//sync_spin::spin_yield();
			}while(true);
			interlocked::increment(&_blocks_in_use);

			return (void*)(_start_data + (s - _start_node));
		}

		template<size_t block_size, size_t block_alignment>
		void					nt_mem_pool<block_size, block_alignment>::dealloc(void* ptr)
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) == 4, POINTER_SIZE_ASSUMPTION_WRONG );
			Z_STATIC_ASSERT( sizeof(long) == sizeof(void*), POINTER_SIZE_ASSUMPTION_WRONG );

			Z_ASSERT( ptr >= _start_data && ptr < _end_data );

			node* s = _start_node + ((NT_TYPE*)ptr - _start_data);
			node* next;
			do
			{
				next = (node*)_free_start;
				s->next = next;
#pragma warning(suppress:4311)
				if(interlocked::compare_exchange((uint32_t*)&_free_start, (uint32_t)s, (uint32_t)next) == (uint32_t)next)
					break;
				//sync_spin::spin_yield();
			}while(true);
			interlocked::decrement(&_blocks_in_use);
		}
		template<size_t block_size, size_t block_alignment>
		void					nt_mem_pool<block_size, block_alignment>::clear_free_blocks()
		{
			for(node* n = _free_start; n; n = n->next)
				memset(_start_data + (n - _start_node), 0, block_size);
		}
		template<size_t block_size, size_t block_alignment>
		const void*				nt_mem_pool<block_size, block_alignment>::get_first_block() const
		{
			return _start_data;
		}
		template<size_t block_size, size_t block_alignment>
		size_t					nt_mem_pool<block_size, block_alignment>::get_max_blocks() const
		{
			return _max_blocks;
		}
		template<size_t block_size, size_t block_alignment>
		bool					nt_mem_pool<block_size, block_alignment>::owns(void* ptr) const
		{
			return (ptr >= _start_data && ptr < _end_data);
		}
		template<size_t block_size, size_t block_alignment>
		size_t					nt_mem_pool<block_size, block_alignment>::get_blocks_in_use() const
		{
			return _blocks_in_use;
		}


		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::nt_mem_pool_keeper(nt_mem_pool<block_size, block_alignment>* mp)
			: _mp(mp)
		{}

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::nt_mem_pool_keeper(size_t max_blocks)
			: _mp(nt_mem_pool<block_size, block_alignment>::create(max_blocks))
		{
			Z_STATIC_ASSERT( dispose_on_destruct, USGAE_ERROR );
		}

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::nt_mem_pool_keeper(size_t max_blocks, void* mem)
			: _mp(nt_mem_pool<block_size, block_alignment>::create(max_blocks, mem))
		{
			Z_STATIC_ASSERT( !dispose_on_destruct, USGAE_ERROR );
		}

		//template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		//template<size_t max_blocks, size_t storage_size>
		//nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::nt_mem_pool_keeper(const temp_int<max_blocks>&, char (&storage)[storage_size])
		//	: _mp(nt_mem_pool<block_size, block_alignment>::create<max_blocks>(storage))
		//{
		//	Z_STATIC_ASSERT( !dispose_on_destruct, USGAE_ERROR );
		//}

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
		nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::nt_mem_pool_keeper(const temp_int<max_blocks>&, aligned_storage<storage_size, storage_alignment>& storage)
			: _mp(nt_mem_pool<block_size, block_alignment>::create<max_blocks>(storage))
		{
			Z_STATIC_ASSERT( !dispose_on_destruct, USGAE_ERROR );
		}

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::~nt_mem_pool_keeper()
		{
			___dtor();
		}

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		void					nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::___dtor()
		{
			if(dispose_on_destruct)
			{
				if(_mp)
					_mp->dispose();
			}
			else
			{
				if(_mp)
				{
					Z_ASSERT( _mp->can_dispose() );
				}
			}
		}

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		void					nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::reinit(nt_mem_pool<block_size, block_alignment>* mp)
		{
			___dtor();
			_mp = mp;
		}

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		nt_mem_pool<block_size, block_alignment>*			nt_mem_pool_keeper<block_size, block_alignment, dispose_on_destruct>::get() const
		{
			return _mp;
		}
	};
};



