
#pragma once

#include "aligned_storage.h"

#include "compile_time_verifiers.h"

namespace z3D
{
	namespace Core
	{
		class aligned_storage_tag::token {	private: token() {}	template<size_t size, size_t alignment> friend class z3D::Core::aligned_storage; };
		class aligned_storage_tag::ctor {};

		template<size_t size, size_t alignment>
		aligned_storage_base<size, alignment>::aligned_storage_base(const aligned_storage_tag::token&)			{}

		template<size_t size, size_t alignment>
		template<typename T> void*				aligned_storage_base<size, alignment>::allocate()				{ SIZE_VERIFIER<sizeof(T), SIZE>(); ALIGNMENT_VERIFIER<Z_GET_ALIGNMENT<T>::value, ALIGNMENT>(); return this; }

#include "z_off_debug_new.h"
		template<size_t size, size_t alignment>
		template<typename T> T&					aligned_storage_base<size, alignment>::create()					{ return *(new (allocate<T>()) T()); }
#include "z_on_debug_new.h"

		template<size_t size, size_t alignment>
		template<typename T> void				aligned_storage_base<size, alignment>::destroy()				{ ((T*)this)->T::~T(); }

		template<size_t size, size_t alignment>
		const void*								aligned_storage_base<size, alignment>::get_unsafe() const		{ return this; }

		template<size_t size, size_t alignment>
		void*									aligned_storage_base<size, alignment>::get_unsafe()				{ return this; }

		template<size_t size, size_t alignment>
		template<typename T> const T&			aligned_storage_base<size, alignment>::get() const				{ SIZE_VERIFIER<sizeof(T), SIZE>(); ALIGNMENT_VERIFIER<Z_GET_ALIGNMENT<T>::value, ALIGNMENT>(); return *(const T*)this; }

		template<size_t size, size_t alignment>
		template<typename T> T&					aligned_storage_base<size, alignment>::get()					{ SIZE_VERIFIER<sizeof(T), SIZE>(); ALIGNMENT_VERIFIER<Z_GET_ALIGNMENT<T>::value, ALIGNMENT>(); return *(T*)this; }

		template<size_t size, size_t alignment>
		aligned_storage<size, alignment>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( false, INVALID_ALIGNMENT ); }

		template<size_t size, size_t alignment>
		template<typename U>
		aligned_storage<size, alignment>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( false, INVALID_ALIGNMENT ); }

		template<size_t size>
		aligned_storage<size, 1>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 1>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 2>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 2>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 4>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 4>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 8>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 8>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 16>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 16>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 32>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 32>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 64>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 64>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 128>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 128>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		aligned_storage<size, 256>::aligned_storage()
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }

		template<size_t size>
		template<typename U>
		aligned_storage<size, 256>::aligned_storage(U unused, const aligned_storage_tag::ctor&)
			: aligned_storage_base(aligned_storage_tag::token())
		{ Z_STATIC_ASSERT( sizeof(aligned_storage) == SIZE, SIZE_ALIGNMENT_INCOMPATIBLE ); }


		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		typed_aligned_storage<T, size, alignment, destroy_on_destruct>::typed_aligned_storage()
		{}

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		template<typename U>
		typed_aligned_storage<T, size, alignment, destroy_on_destruct>::typed_aligned_storage(U unused, const aligned_storage_tag::ctor&)
		{}

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		typed_aligned_storage<T, size, alignment, destroy_on_destruct>::~typed_aligned_storage()
		{ if(destroy_on_destruct) destroy(); }

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		void*									typed_aligned_storage<T, size, alignment, destroy_on_destruct>::allocate()				{ return aligned_storage::allocate<T>(); }

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		T&										typed_aligned_storage<T, size, alignment, destroy_on_destruct>::create()				{ return aligned_storage::create<T>(); }

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		void									typed_aligned_storage<T, size, alignment, destroy_on_destruct>::destroy()				{ aligned_storage::destroy<T>(); }

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		const void*								typed_aligned_storage<T, size, alignment, destroy_on_destruct>::get_unsafe() const		{ return this; }

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		void*									typed_aligned_storage<T, size, alignment, destroy_on_destruct>::get_unsafe()			{ return this; }

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		const T&								typed_aligned_storage<T, size, alignment, destroy_on_destruct>::get() const				{ return aligned_storage::get<T>(); }

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		T&										typed_aligned_storage<T, size, alignment, destroy_on_destruct>::get()					{ return aligned_storage::get<T>(); }
	};
};

