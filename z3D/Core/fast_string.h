
#pragma once

#include "predecl.h"

#ifdef Z_USE_FAST_STRING

#include "mem_pool.h"

namespace z3D
{
	namespace Core
	{
		class str_allocator_pool
		{
		public:
			class ITEM
			{
			public:
				char										s[256];
			};
			typedef mem_pool<ITEM>						POOL;
			static Z3D_API POOL*						pool;
		};

		template<class _Ty>
		class str_allocator: public _Allocator_base<_Ty>
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
				typedef str_allocator<_Other>			other;
			};
		public:
			str_allocator() _THROW0() {}
		public:
			str_allocator(const str_allocator<_Ty>&) _THROW0() {}
			template<class _Other>
			str_allocator(const str_allocator<_Other>&) _THROW0() {}
		public:
			template<class _Other>
			str_allocator<_Ty>& operator=(const str_allocator<_Other>&)
			{
				return (*this);
			}

		public:
			pointer address(reference _Val) const
			{
				return (&_Val);
			}
			const_pointer address(const_reference _Val) const
			{
				return (&_Val);
			}
		public:
			void deallocate(pointer _Ptr, size_type)
			{
				if(str_allocator_pool::pool->owns((str_allocator_pool::POOL::TYPE*)_Ptr))
					str_allocator_pool::pool->dealloc((str_allocator_pool::POOL::TYPE*)_Ptr);
				else
					delete (void*)_Ptr;
			}
		public:
			pointer allocate(size_type _Count)
			{
				void* p = NULL;
				if(sizeof(_Ty) * _Count <= sizeof(str_allocator_pool::POOL::TYPE))
					p = (void*)str_allocator_pool::pool->alloc();
				if(!p)
					p = new char[sizeof(_Ty) * _Count];
				return (pointer)p;
			}
			pointer allocate(size_type _Count, const void _FARQ *)
			{
				return (allocate(_Count));
			}
		public:
			void construct(pointer _Ptr, const _Ty& _Val)
			{
				_Construct(_Ptr, _Val);
			}
			void destroy(pointer _Ptr)
			{
				_Destroy(_Ptr);
			}
		public:
			_SIZT max_size() const _THROW0()
			{
				_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
				return (0 < _Count ? _Count : 1);
			}
		};

		template<class U, class V>
		bool operator==(const str_allocator<U>&, const str_allocator<V>&) { return true; }
		template<class U, class V>
		bool operator!=(const str_allocator<U>&, const str_allocator<V>&) { return false; }
		template<class T>
		bool operator==(const str_allocator<T>&, const str_allocator<T>&) { return true; }
		template<class T>
		bool operator!=(const str_allocator<T>&, const str_allocator<T>&) { return false; }

		typedef std::basic_string<char, std::char_traits<char>, str_allocator<char>>				mbs;
		typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, str_allocator<wchar_t>>		wcs;

		Z3D_API string			CONVERT(const mbs& s);
		Z3D_API wstring			CONVERT(const wcs& s);
		Z3D_API mbs				CONVERT(const string& s);
		Z3D_API wcs				CONVERT(const wstring& s);
	};
};

#else

namespace z3D
{
	namespace Core
	{
		typedef string																				mbs;
		typedef wstring																				wcs;

		mbs				CONVERT(const mbs& s);
		wcs				CONVERT(const wcs& s);
	};
};

#endif