
#pragma once

#include "allocators.h"

#include "compile_time_verifiers.h"
#include "nt_mem_pool_inl.h"

namespace z3D
{
	namespace Core
	{
		template<class _Ty>
		void						in_place_allocator<_Ty>::ctor()
		{
			_head = NULL;
		}

		template<class _Ty>
		void						in_place_allocator<_Ty>::dtor()
		{
			node* n = _head;
			while(n)
			{
				node* next = n->next;
				mem_alloc::__aligned_free((void*)n);
				n = next;
			}
		}

		template<class _Ty>
		in_place_allocator<_Ty>::in_place_allocator() _THROW0()
		{
			ctor();
		}

		template<class _Ty>
		in_place_allocator<_Ty>::in_place_allocator(const in_place_allocator<_Ty>&) _THROW0()
		{
			ctor();
		}

		template<class _Ty>
		template<class _Other>
		in_place_allocator<_Ty>::in_place_allocator(const in_place_allocator<_Other>&) _THROW0()
		{
			ctor();
		}

		template<class _Ty>
		in_place_allocator<_Ty>::~in_place_allocator()
		{
			dtor();
		}

		template<class _Ty>
		template<class _Other>
		in_place_allocator<_Ty>&	in_place_allocator<_Ty>::operator=(const in_place_allocator<_Other>&)
		{
			return (*this);
		}

		template<class _Ty>
		typename in_place_allocator<_Ty>::pointer
									in_place_allocator<_Ty>::address(reference _Val) const
		{
			return (&_Val);
		}

		template<class _Ty>
		typename in_place_allocator<_Ty>::const_pointer
									in_place_allocator<_Ty>::address(const_reference _Val) const
		{
			return (&_Val);
		}

		template<class _Ty>
		void						in_place_allocator<_Ty>::deallocate(pointer _Ptr, size_type)
		{
			node* n = (node*)((char*)_Ptr - offsetof(node, data));
			n->next = _head;
			_head = n;
		}

		template<class _Ty>
		typename in_place_allocator<_Ty>::pointer
									in_place_allocator<_Ty>::allocate(size_type _Count)
		{
			Z_ASSERT( _Count == 1 );

			if(!_head)
			{
				_head = (node*)mem_alloc::__aligned_alloc(sizeof(node), Z_GET_ALIGNMENT<node>::value);
				_head->next = NULL;
			}

			node* n = _head;
			_head = n->next;

			return &n->data;
		}

		template<class _Ty>
		typename in_place_allocator<_Ty>::pointer
									in_place_allocator<_Ty>::allocate(size_type _Count, const void _FARQ *)
		{
			return (allocate(_Count));
		}

		template<class _Ty>
		void						in_place_allocator<_Ty>::construct(pointer _Ptr, const _Ty& _Val)
		{
			_Construct(_Ptr, _Val);
		}

		template<class _Ty>
		void						in_place_allocator<_Ty>::destroy(pointer _Ptr)
		{
			_Destroy(_Ptr);
		}

		template<class _Ty>
		typename in_place_allocator<_Ty>::size_type
									in_place_allocator<_Ty>::max_size() const _THROW0()
		{
			size_type _Count = (size_type)(-1) / sizeof (_Ty);
			return (0 < _Count ? _Count : 1);
		}

		template<class U, class V>
		bool operator==(const in_place_allocator<U>&, const in_place_allocator<V>&) { return false; }
		template<class U, class V>
		bool operator!=(const in_place_allocator<U>&, const in_place_allocator<V>&) { return true; }
		template<class T>
		bool operator==(const in_place_allocator<T>&, const in_place_allocator<T>&) { return false; }
		template<class T>
		bool operator!=(const in_place_allocator<T>&, const in_place_allocator<T>&) { return true; }


		template<class _Ty, size_t block_size, size_t block_alignment>
		void						fixed_block_allocator_base<_Ty, block_size, block_alignment>::ctor()
		{
			_mp = NULL;
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		void						fixed_block_allocator_base<_Ty, block_size, block_alignment>::ctor(nt_mem_pool<block_size, block_alignment>* mp)
		{
			_mp = mp;
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		void						fixed_block_allocator_base<_Ty, block_size, block_alignment>::dtor()
		{
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		fixed_block_allocator_base<_Ty, block_size, block_alignment>::fixed_block_allocator_base() _THROW0()
		{
			ctor();
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		fixed_block_allocator_base<_Ty, block_size, block_alignment>::fixed_block_allocator_base(nt_mem_pool<block_size, block_alignment>* mp) _THROW0()
		{
			ctor();
			ctor(mp);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		fixed_block_allocator_base<_Ty, block_size, block_alignment>::fixed_block_allocator_base(const fixed_block_allocator_base<_Ty, block_size, block_alignment>& rhs) _THROW0()
		{
			ctor();
			ctor(rhs._mp);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		template<class _Other>
		fixed_block_allocator_base<_Ty, block_size, block_alignment>::fixed_block_allocator_base(const fixed_block_allocator_base<_Other, block_size, block_alignment>& rhs) _THROW0()
		{
			ctor();
			ctor(rhs._mp);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		fixed_block_allocator_base<_Ty, block_size, block_alignment>::~fixed_block_allocator_base()
		{
			dtor();
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		template<class _Other>
		fixed_block_allocator_base<_Ty, block_size, block_alignment>&
									fixed_block_allocator_base<_Ty, block_size, block_alignment>::operator=(const fixed_block_allocator_base<_Other, block_size, block_alignment>& rhs)
		{
			dtor();
			ctor(rhs._mp);
			return *this;
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		typename fixed_block_allocator_base<_Ty, block_size, block_alignment>::pointer
									fixed_block_allocator_base<_Ty, block_size, block_alignment>::address(reference _Val) const
		{
			return (&_Val);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		typename fixed_block_allocator_base<_Ty, block_size, block_alignment>::const_pointer
									fixed_block_allocator_base<_Ty, block_size, block_alignment>::address(const_reference _Val) const
		{
			return (&_Val);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		void						fixed_block_allocator_base<_Ty, block_size, block_alignment>::deallocate(pointer _Ptr, size_type)
		{
			if(_mp->owns((void*)_Ptr))
				_mp->dealloc(_Ptr);
			else
				mem_alloc::__aligned_free(_Ptr);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		typename fixed_block_allocator_base<_Ty, block_size, block_alignment>::pointer
									fixed_block_allocator_base<_Ty, block_size, block_alignment>::allocate(size_type _Count)
		{
			Z_ASSERT( _Count == 1 );

			// since from my experience, even map or set will have several rebinds, only one allocator at a time will actually allocate, if its not the case then change the following ASSURE_EQUAL to ASSURE_GEQUAL

			ASSURE_EQUAL<block_size, sizeof(_Ty)>();
			ASSURE_EQUAL<block_alignment, Z_GET_ALIGNMENT<_Ty>::value>();

			pointer _Ptr = (pointer)_mp->alloc();
			if(_Ptr)
				return _Ptr;

			return (pointer)mem_alloc::__aligned_alloc(block_size, block_alignment);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		typename fixed_block_allocator_base<_Ty, block_size, block_alignment>::pointer
									fixed_block_allocator_base<_Ty, block_size, block_alignment>::allocate(size_type _Count, const void _FARQ *)
		{
			return (allocate(_Count));
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		void						fixed_block_allocator_base<_Ty, block_size, block_alignment>::construct(pointer _Ptr, const _Ty& _Val)
		{
			_Construct(_Ptr, _Val);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		void						fixed_block_allocator_base<_Ty, block_size, block_alignment>::destroy(pointer _Ptr)
		{
			_Destroy(_Ptr);
		}

		template<class _Ty, size_t block_size, size_t block_alignment>
		typename fixed_block_allocator_base<_Ty, block_size, block_alignment>::size_type
									fixed_block_allocator_base<_Ty, block_size, block_alignment>::max_size() const _THROW0()
		{
			size_type _Count = (size_type)(-1) / sizeof (_Ty);
			return (0 < _Count ? _Count : 1);
		}


		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::fast_temp_mem_allocator()
		{
			Z_STATIC_ASSERT( !(ALIGNMENT & (ALIGNMENT - 1)), ALIGNMENT_MUST_BE_POWER_OF_2 );

			clear();
			clear_usage();
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::~fast_temp_mem_allocator()
		{
			Z_ASSERT( !entry_count() );
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		void			fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::clear()
		{
			_entry_count = 0;
			_next_addr = 0;
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		void*			fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::alloc(size_t s)
		{
			scoped_lock<mutex_fast> sl(_mx);

			if(_usage.current > _usage.peak) _usage.peak = _usage.current;

			if(_entry_count >= BOOKKEEP_COUNT)
			{
				_usage.accumulated_failed_req_size += s;
				return new char[s];
			}

			size_t total;
			if((total = (ALIGNMENT - ((size_t)(_mem + _next_addr) & (ALIGNMENT - 1))) & (ALIGNMENT - 1)) < sizeof(header_t))
				total = (ALIGNMENT - ((size_t)(_mem + _next_addr + sizeof(header_t)) & (ALIGNMENT - 1))) & (ALIGNMENT - 1);

			size_t ret_off = _next_addr + total;
			total += s;

			if(total > AVAILABLE_SIZE - _next_addr)
			{
				_usage.accumulated_failed_req_size += s;
				return new char[s];
			}

			entry& e = _entries[_entry_count];
			e.size = total;
			e.header = (header_t*)(_mem + ret_off - sizeof(header_t));

			e.header->block_no = _entry_count;
			e.header->req_size = s;

			++_entry_count;
			_next_addr += total;

			// record usage
			_usage.current += s;
			++_usage.current_allocs;
			_usage.peak = (_usage.current > _usage.peak) ? _usage.current : _usage.peak;
			_usage.peak_allocs = (_usage.current_allocs > _usage.peak_allocs) ? _usage.current_allocs : _usage.current_allocs;
			_usage.accumulation += s;

			return _mem + ret_off;
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		void			fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::dealloc(void* p)
		{
			scoped_lock<mutex_fast> sl(_mx);

			if((char*)p < _mem || (char*)p >= _mem + AVAILABLE_SIZE)
			{
				delete p;
				return;
			}

			header_t& header = *(header_t*)((char*)p - sizeof(header_t));
			if(header.block_no != _entry_count - 1)
				header.block_no = ~(size_t)0;
			else
			{
				size_t to_free = _entries[header.block_no].size;
				size_t last_freeable = header.block_no;
				entry* end = _entries - 1;
				for(entry* p = _entries + header.block_no - 1; p != end; --p)
				{
					if(p->header->block_no != ~(size_t)0)
						break;
					to_free += p->size;
					--last_freeable;
				}
				_entry_count = last_freeable;
				_next_addr -= to_free;
			}

			// record usage
			_usage.current -= header.req_size;
			--_usage.current_allocs;
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		size_t			fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::entry_count() const
		{
			return _entry_count;
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		void			fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::clear_usage()
		{
			memset(&_usage, 0, sizeof(_usage));
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		void			fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::clear_usage_accumulation()
		{
			_usage.accumulation = 0;
			_usage.accumulated_failed_req_size = 0;
		}

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		const typename fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::usage&
						fast_temp_mem_allocator<AVAILABLE_SIZE, BOOKKEEP_COUNT, ALIGNMENT>::get_usage() const
		{
			return _usage;
		}


		template<class _Ty>
		void						frame_temp_allocator<_Ty>::ctor()
		{
		}

		template<class _Ty>
		void						frame_temp_allocator<_Ty>::dtor()
		{
		}

		template<class _Ty>
		frame_temp_allocator<_Ty>::frame_temp_allocator() _THROW0()
		{
			ctor();
		}

		template<class _Ty>
		frame_temp_allocator<_Ty>::frame_temp_allocator(const frame_temp_allocator<_Ty>&) _THROW0()
		{
			ctor();
		}

		template<class _Ty>
		template<class _Other>
		frame_temp_allocator<_Ty>::frame_temp_allocator(const frame_temp_allocator<_Other>&) _THROW0()
		{
			ctor();
		}

		template<class _Ty>
		frame_temp_allocator<_Ty>::~frame_temp_allocator()
		{
			dtor();
		}

		template<class _Ty>
		template<class _Other>
		frame_temp_allocator<_Ty>&	frame_temp_allocator<_Ty>::operator=(const frame_temp_allocator<_Other>&)
		{
			return (*this);
		}

		template<class _Ty>
		typename frame_temp_allocator<_Ty>::pointer
									frame_temp_allocator<_Ty>::address(reference _Val) const
		{
			return (&_Val);
		}

		template<class _Ty>
		typename frame_temp_allocator<_Ty>::const_pointer
									frame_temp_allocator<_Ty>::address(const_reference _Val) const
		{
			return (&_Val);
		}

		template<class _Ty>
		void						frame_temp_allocator<_Ty>::deallocate(pointer _Ptr, size_type)
		{
			return g_frame_temp_mem_allocator.dealloc(_Ptr);
		}

		template<class _Ty>
		typename frame_temp_allocator<_Ty>::pointer
									frame_temp_allocator<_Ty>::allocate(size_type _Count)
		{
			return (pointer)g_frame_temp_mem_allocator.alloc(sizeof(_Ty) * _Count);
		}

		template<class _Ty>
		typename frame_temp_allocator<_Ty>::pointer
									frame_temp_allocator<_Ty>::allocate(size_type _Count, const void _FARQ *)
		{
			return (allocate(_Count));
		}

		template<class _Ty>
		void						frame_temp_allocator<_Ty>::construct(pointer _Ptr, const _Ty& _Val)
		{
			_Construct(_Ptr, _Val);
		}

		template<class _Ty>
		void						frame_temp_allocator<_Ty>::destroy(pointer _Ptr)
		{
			_Destroy(_Ptr);
		}

		template<class _Ty>
		typename frame_temp_allocator<_Ty>::size_type
									frame_temp_allocator<_Ty>::max_size() const _THROW0()
		{
			size_type _Count = (size_type)(-1) / sizeof (_Ty);
			return (0 < _Count ? _Count : 1);
		}
	};
};

