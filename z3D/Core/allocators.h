
#pragma once

#include "predecl.h"

#include "noncopyable.h"
#include "mutex_fast.h"
#include "nt_mem_pool.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	to use the allocators found here, also include allocators_inl.h in your .cpp file, if you forget, the linker will remind you
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace z3D
{
	namespace Core
	{
		template<class _Ty>
		class in_place_allocator: public _Allocator_base<_Ty>
		{
		public:
			typedef _Allocator_base<_Ty>				_Mybase;
			typedef typename _Mybase::value_type		value_type;
			typedef value_type _FARQ*					pointer;
			typedef value_type _FARQ&					reference;
			typedef const value_type _FARQ*				const_pointer;
			typedef const value_type _FARQ&				const_reference;

			typedef _SIZT								size_type;
			typedef _PDFT								difference_type;

			template<class _Other>
			struct rebind
			{
				typedef in_place_allocator<_Other>			other;
			};
		private:
			typedef struct NODE
			{
				NODE*										next;
				_Ty											data;
			} node;

			node*										_head;
		private:
			void						ctor();
			void						dtor();
		public:
			in_place_allocator() _THROW0();
		public:
			in_place_allocator(const in_place_allocator<_Ty>&) _THROW0();
			template<class _Other>
			in_place_allocator(const in_place_allocator<_Other>&) _THROW0();
		public:
			~in_place_allocator();
		public:
			template<class _Other>
			in_place_allocator&			operator=(const in_place_allocator<_Other>&);
		public:
			pointer						address(reference _Val) const;
			const_pointer				address(const_reference _Val) const;
		public:
			void						deallocate(pointer _Ptr, size_type);
		public:
			pointer						allocate(size_type _Count);
			pointer						allocate(size_type _Count, const void _FARQ *);
		public:
			void						construct(pointer _Ptr, const _Ty& _Val);
			void						destroy(pointer _Ptr);
		public:
			size_type					max_size() const _THROW0();
		};

		template<class U, class V>
		bool operator==(const in_place_allocator<U>&, const in_place_allocator<V>&);
		template<class U, class V>
		bool operator!=(const in_place_allocator<U>&, const in_place_allocator<V>&);
		template<class T>
		bool operator==(const in_place_allocator<T>&, const in_place_allocator<T>&);
		template<class T>
		bool operator!=(const in_place_allocator<T>&, const in_place_allocator<T>&);

		// to use fixed_block_allocator, a nt_mem_pool must be created first, and the right block size and alignment can only be found via compile-time experiment, and use macro Z_DEFINE_FIXED_BLOCK_ALLOCATOR to define an allocator for such block size, alignment and such explicit use
		template<class _Ty, size_t block_size, size_t block_alignment>
		class fixed_block_allocator_base: public _Allocator_base<_Ty>
		{
		public:
			enum
			{
				block_size = typename block_size,
				block_alignment = typename block_alignment,
			};
		public:
			typedef _Allocator_base<_Ty>				_Mybase;
			typedef typename _Mybase::value_type		value_type;
			typedef value_type _FARQ*					pointer;
			typedef value_type _FARQ&					reference;
			typedef const value_type _FARQ*				const_pointer;
			typedef const value_type _FARQ&				const_reference;

			typedef _SIZT								size_type;
			typedef _PDFT								difference_type;

			template<class _Other>
			struct rebind
			{
				typedef fixed_block_allocator_base<_Other, block_size, block_alignment>		other;
			};
		private:
			nt_mem_pool<block_size, block_alignment>*	_mp;
		private:
			void						ctor();
			void						ctor(nt_mem_pool<block_size, block_alignment>* mp);
			void						dtor();
		public:
			fixed_block_allocator_base() _THROW0();
			fixed_block_allocator_base(nt_mem_pool<block_size, block_alignment>* mp) _THROW0();
		public:
			fixed_block_allocator_base(const fixed_block_allocator_base<_Ty, block_size, block_alignment>& rhs) _THROW0();
			template<class _Other>
			fixed_block_allocator_base(const fixed_block_allocator_base<_Other, block_size, block_alignment>& rhs) _THROW0();
		public:
			~fixed_block_allocator_base();
		public:
			template<class _Other>
			fixed_block_allocator_base&	operator=(const fixed_block_allocator_base<_Other, block_size, block_alignment>& rhs);
		public:
			pointer						address(reference _Val) const;
			const_pointer				address(const_reference _Val) const;
		public:
			void						deallocate(pointer _Ptr, size_type);
		public:
			pointer						allocate(size_type _Count);
			pointer						allocate(size_type _Count, const void _FARQ *);
		public:
			void						construct(pointer _Ptr, const _Ty& _Val);
			void						destroy(pointer _Ptr);
		public:
			size_type					max_size() const _THROW0();
		public:
			template<class _Ty2, size_t block_size2, size_t block_alignment2> friend class fixed_block_allocator_base;
		};

		#define Z_DEFINE_FIXED_BLOCK_ALLOCATOR(class_name_, block_size_, block_alignment_) \
		\
		\
		template<class _Ty> \
		class class_name_: public fixed_block_allocator_base<_Ty, block_size_, block_alignment_> \
		{ \
		public: \
			enum \
			{ \
				block_size = block_size_, \
				block_alignment = block_alignment_, \
			}; \
		public: \
			template<class _Other> \
			struct rebind	{typedef class_name_<_Other>		other;}; \
		public: \
			class_name_() _THROW0() \
			{} \
			class_name_(nt_mem_pool<block_size_, block_alignment_>* mp) _THROW0() \
				: fixed_block_allocator_base(mp) \
			{} \
		public: \
			class_name_(const class_name_<_Ty>& rhs) _THROW0() \
				: fixed_block_allocator_base(rhs) \
			{} \
			template<class _Other> \
			class_name_(const class_name_<_Other>& rhs) _THROW0() \
				: fixed_block_allocator_base(rhs) \
			{} \
		public: \
			~class_name_() \
			{} \
		public: \
			template<class _Other> \
			class_name_<_Ty>& operator=(const class_name_<_Other>& rhs) \
			{ \
				fixed_block_allocator_base::operator = (rhs); \
				return *this; \
			} \
		}; \


		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT = 32>
		class fast_temp_mem_allocator: noncopyable
		{
		public:
			class header_t
			{
			public:
				size_t			block_no;
				size_t			req_size;
			};
			class entry
			{
			public:
				size_t			size;
				header_t*		header;
			};
		private:
			char			_mem[AVAILABLE_SIZE];
			entry			_entries[BOOKKEEP_COUNT];
			size_t			_entry_count;
			size_t			_next_addr;
			mutex_fast		_mx;
		public:
			class usage
			{
			public:
				size_t			current;
				size_t			current_allocs;
				size_t			peak;
				size_t			peak_allocs;
				size_t			accumulation;
				size_t			accumulated_failed_req_size;
			};
			usage			_usage;
		public:
			fast_temp_mem_allocator();
			~fast_temp_mem_allocator();
		public:
			void			clear();
		public:
			void*			alloc(size_t s);
			void			dealloc(void* p);
		public:
			size_t			entry_count() const;
		public:
			void			clear_usage();
			void			clear_usage_accumulation();
			const usage&	get_usage() const;
		};

		template<class _Ty>
		class frame_temp_allocator: public _Allocator_base<_Ty>
		{
		public:
			typedef _Allocator_base<_Ty>				_Mybase;
			typedef typename _Mybase::value_type		value_type;
			typedef value_type _FARQ*					pointer;
			typedef value_type _FARQ&					reference;
			typedef const value_type _FARQ*				const_pointer;
			typedef const value_type _FARQ&				const_reference;

			typedef _SIZT								size_type;
			typedef _PDFT								difference_type;

			template<class _Other>
			struct rebind
			{
				typedef frame_temp_allocator<_Other>			other;
			};
		private:
			void						ctor();
			void						dtor();
		public:
			frame_temp_allocator() _THROW0();
		public:
			frame_temp_allocator(const frame_temp_allocator<_Ty>&) _THROW0();
			template<class _Other>
			frame_temp_allocator(const frame_temp_allocator<_Other>&) _THROW0();
		public:
			~frame_temp_allocator();
		public:
			template<class _Other>
			frame_temp_allocator&		operator=(const frame_temp_allocator<_Other>&);
		public:
			pointer						address(reference _Val) const;
			const_pointer				address(const_reference _Val) const;
		public:
			void						deallocate(pointer _Ptr, size_type);
		public:
			pointer						allocate(size_type _Count);
			pointer						allocate(size_type _Count, const void _FARQ *);
		public:
			void						construct(pointer _Ptr, const _Ty& _Val);
			void						destroy(pointer _Ptr);
		public:
			size_type					max_size() const _THROW0();
		};

		extern Z3D_API fast_temp_mem_allocator<8 * 1048576, 8192> g_frame_temp_mem_allocator;

	};
};

