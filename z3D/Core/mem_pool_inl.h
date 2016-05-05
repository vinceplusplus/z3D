
#pragma once

#include "mem_pool.h"

#include "aligned_storage_inl.h"
#include "nt_mem_pool_inl.h"

namespace z3D
{
	namespace Core
	{
		template<class T>
		mem_pool<T>*			mem_pool<T>::create(size_t max_blocks)
		{
			return (mem_pool<T>*)nt_mem_pool<sizeof(T), Z_GET_ALIGNMENT<T>::value>::create(max_blocks);
		}
		template<class T>
		mem_pool<T>*			mem_pool<T>::create(size_t max_blocks, void* mem)
		{
			return (mem_pool<T>*)nt_mem_pool<sizeof(T), Z_GET_ALIGNMENT<T>::value>::create(max_blocks, mem);
		}

		//template<class T>
		//template<size_t max_blocks, size_t storage_size>
		//mem_pool<T>*			mem_pool<T>::create(char (&storage)[storage_size])
		//{
		//	return (mem_pool<T>*)nt_mem_pool<sizeof(T), Z_GET_ALIGNMENT<T>::value>::create<max_blocks>(storage);
		//}

		template<class T>
		template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
		mem_pool<T>*			mem_pool<T>::create(aligned_storage<storage_size, storage_alignment>& storage)
		{
			return (mem_pool<T>*)nt_mem_pool<sizeof(T), Z_GET_ALIGNMENT<T>::value>::create<max_blocks>(storage);
		}

		template<class T>
		T*						mem_pool<T>::alloc()
		{
			return (T*)nt_mem_pool::alloc();
		}

		template<class T>
		void					mem_pool<T>::dealloc(T* ptr)
		{
			nt_mem_pool::dealloc((void*)ptr);
		}

		template<class T>
		const T*				mem_pool<T>::get_first_block() const
		{
			return (const T*)nt_mem_pool::get_first_block();
		}

		template<class T>
		void					mem_pool<T>::dealloc(void* ptr)
		{
			nt_mem_pool::dealloc(ptr);
		}


		template<class T, bool dispose_on_destruct>
		mem_pool_keeper<T, dispose_on_destruct>::mem_pool_keeper(mem_pool<T>* mp)
			: _mp(mp)
		{}

		template<class T, bool dispose_on_destruct>
		mem_pool_keeper<T, dispose_on_destruct>::mem_pool_keeper(size_t max_blocks)
			: _mp(mem_pool<T>::create(max_blocks))
		{
			Z_STATIC_ASSERT( dispose_on_destruct, USGAE_ERROR );
		}

		template<class T, bool dispose_on_destruct>
		mem_pool_keeper<T, dispose_on_destruct>::mem_pool_keeper(size_t max_blocks, void* mem)
			: _mp(mem_pool<T>::create(max_blocks, mem))
		{
			Z_STATIC_ASSERT( !dispose_on_destruct, USGAE_ERROR );
		}

		//template<class T, bool dispose_on_destruct>
		//template<size_t max_blocks, size_t storage_size>
		//mem_pool_keeper<T, dispose_on_destruct>::mem_pool_keeper(const temp_int<max_blocks>&, char (&storage)[storage_size])
		//	: _mp(mem_pool<T>::create<max_blocks>(storage))
		//{
		//	Z_STATIC_ASSERT( !dispose_on_destruct, USGAE_ERROR );
		//}

		template<class T, bool dispose_on_destruct>
		template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
		mem_pool_keeper<T, dispose_on_destruct>::mem_pool_keeper(const temp_int<max_blocks>&, aligned_storage<storage_size, storage_alignment>& storage)
			: _mp(mem_pool<T>::create<max_blocks>(storage))
		{
			Z_STATIC_ASSERT( !dispose_on_destruct, USGAE_ERROR );
		}

		template<class T, bool dispose_on_destruct>
		mem_pool_keeper<T, dispose_on_destruct>::~mem_pool_keeper()
		{
			___dtor();
		}

		template<class T, bool dispose_on_destruct>
		void					mem_pool_keeper<T, dispose_on_destruct>::___dtor()
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

		template<class T, bool dispose_on_destruct>
		void					mem_pool_keeper<T, dispose_on_destruct>::reinit(mem_pool<T>* mp)
		{
			___dtor();
			_mp = mp;
		}

		template<class T, bool dispose_on_destruct>
		mem_pool<T>*			mem_pool_keeper<T, dispose_on_destruct>::get() const
		{
			return _mp;
		}
	};
};



