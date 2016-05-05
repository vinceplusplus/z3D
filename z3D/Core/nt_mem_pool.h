
#pragma once

#include "predecl.h"

#include "noncopyable.h"
#include "aligned_storage.h"

namespace z3D
{
	namespace Core
	{
		// do not change anything about the packing alignment, or the interlocked stuff will fail

		// not-typed mem pool
		template<size_t block_size, size_t block_alignment>
		class nt_mem_pool: noncopyable
		{
		private:
			class NT_TYPE
			{
			public:
				char					s[block_size];
			};
		private:
			class node
			{
			public:
				node*volatile			next;
			};
		private:
			size_t					_max_blocks;
		private:
			struct Z_ALIGN(8)
			{
				node*volatile			_free_start;						// free block node linked list head
				volatile uint32_t		_allocs;							// version no. for alloc to deal with the ABA problem, this member must be immediately after _free_start
			};
		private:
			volatile uint32_t		_blocks_in_use;
		private:
			node*					_start_node;
			NT_TYPE*				_start_data;
			NT_TYPE*				_end_data;
		public:
			class helper;
		private:
			void					__ctor(size_t max_blocks);
			void					__dtor();
		public:
			static size_t			get_required_mem_size(size_t max_blocks);
			static size_t			get_required_mem_alignment();
		public:
			static nt_mem_pool*		create(size_t max_blocks);												// call dispose() if you are finished with the mem_pool
			static nt_mem_pool*		create(size_t max_blocks, void* mem);									// mem must be of size returned by get_required_mem_size(max_blocks) and aligned on the boundary of no. of bytes returned by get_required_mem_alignment(). DON'T call dispose() as you manually provide memory for the mem_pool

			//template<size_t max_blocks, size_t storage_size>
			//static nt_mem_pool*		create(char (&storage)[storage_size]);									// DON'T call dispose() as you manually provide memory for the mem_pool

			template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
			static nt_mem_pool*		create(aligned_storage<storage_size, storage_alignment>& storage);		// DON'T call dispose() as you manually provide memory for the mem_pool
		public:
			void					dispose();
		public:
			bool					can_dispose() const;
		public:
			void*					alloc();																// you need to manually call the constructor after this call if necessary
			void					dealloc(void* ptr);														// you need to manually call the destructor before this call if necessary
		public:
			void					clear_free_blocks();													// not thread-safe
			const void*				get_first_block() const;
		public:
			size_t					get_max_blocks() const;
		public:
			bool					owns(void* ptr) const;
		public:
			size_t					get_blocks_in_use() const;
		public:
			template<class U> friend class Z_ALIGNMENT_QUERY;
		};

		// upon the destruction of nt_mem_pool_keeper, it will call dispose() on the _mp member if its not NULL and dispose_on_destruct is true
		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		class nt_mem_pool_keeper: noncopyable
		{
		private:
			nt_mem_pool<block_size, block_alignment>*
									_mp;
		public:
			nt_mem_pool_keeper(nt_mem_pool<block_size, block_alignment>* mp = 0);

			// helper constructors
			nt_mem_pool_keeper(size_t max_blocks);
			nt_mem_pool_keeper(size_t max_blocks, void* mem);

			//template<size_t max_blocks, size_t storage_size>
			//nt_mem_pool_keeper(const temp_int<max_blocks>&, char (&storage)[storage_size]);

			template<size_t max_blocks, size_t storage_size, size_t storage_alignment>
			nt_mem_pool_keeper(const temp_int<max_blocks>&, aligned_storage<storage_size, storage_alignment>& storage);
		public:
			~nt_mem_pool_keeper();
		private:
			void					___dtor();
		public:
			void					reinit(nt_mem_pool<block_size, block_alignment>* mp);
		public:
			nt_mem_pool<block_size, block_alignment>*
									get() const;
		};
	};
};

