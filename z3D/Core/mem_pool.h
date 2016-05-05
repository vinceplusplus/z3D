
#pragma once

#include "predecl.h"

#include "noncopyable.h"
#include "aligned_storage.h"
#include "nt_mem_pool.h"

namespace z3D
{
	namespace Core
	{
		// do not change anything about the packing alignment, or the interlocked stuff will fail

		template<class T>
		class mem_pool: public nt_mem_pool<sizeof(T), Z_GET_ALIGNMENT<T>::value>
		{
		public:
			typedef typename T		TYPE;
		public:
			static mem_pool*		create(size_t max_blocks);												// call dispose() if you are finished with the mem_pool
			static mem_pool*		create(size_t max_blocks, void* mem);									// mem must be of size returned by get_required_mem_size(max_blocks) and aligned on the boundary of no. of bytes returned by get_required_mem_alignment(). DON'T call dispose() as you manually provide memory for the mem_pool

			//template<size_t max_blocks, size_t storage_size>
			//static mem_pool*		create(char (&storage)[storage_size]);									// DON'T call dispose() as you manually provide memory for the mem_pool

			template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
			static mem_pool*		create(aligned_storage<storage_size, storage_alignment>& storage);		// DON'T call dispose() as you manually provide memory for the mem_pool
		public:
			T*						alloc();																// you need to manually call the constructor after this call if necessary
			void					dealloc(T* ptr);														// you need to manually call the destructor before this call if necessary
			const T*				get_first_block() const;
		private:
			void					dealloc(void* ptr);
		public:
			template<class U> friend class Z_ALIGNMENT_QUERY;
		};

		// upon the destruction of mem_pool_keeper, it will call dispose() on the _mp member if its not NULL and dispose_on_destruct is true
		template<class T, bool dispose_on_destruct>
		class mem_pool_keeper: noncopyable
		{
		private:
			mem_pool<T>*			_mp;
		public:
			mem_pool_keeper(mem_pool<T>* mp = 0);

			// helper constructors
			mem_pool_keeper(size_t max_blocks);
			mem_pool_keeper(size_t max_blocks, void* mem);

			//template<size_t max_blocks, size_t storage_size>
			//mem_pool_keeper(const temp_int<max_blocks>&, char (&storage)[storage_size]);

			template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
			mem_pool_keeper(const temp_int<max_blocks>&, aligned_storage<storage_size, storage_alignment>& storage);
		public:
			~mem_pool_keeper();
		private:
			void					___dtor();
		public:
			void					reinit(mem_pool<T>* mp);
		public:
			mem_pool<T>*			get() const;
		};
	};
};

