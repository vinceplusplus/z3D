
#pragma once

#include "predecl.h"

#include "noncopyable.h"

namespace z3D
{
	namespace Core
	{
		// support storages with up to 256-byte alignment
		// use aligned_storage or typed_aligned_storage

		class aligned_storage_tag
		{
		public:
			class token;
			class ctor;
		};

		// do not directly use this base class
		template<size_t size, size_t alignment>
		class aligned_storage_base: noncopyable
		{
		public:
			enum
			{
				SIZE			=	size,
				ALIGNMENT		=	alignment,
			};
		public:
			aligned_storage_base(const aligned_storage_tag::token&);
		public:
			template<typename T> void*				allocate();							// if u require non default constructor, use this allocate
			template<typename T> T&					create();							// only for default constructor
			template<typename T> void				destroy();
		public:
			const void*								get_unsafe() const;
			void*									get_unsafe();
		public:
			template<typename T> const T&			get() const;
			template<typename T> T&					get();
		};

		template<size_t size, size_t alignment>
		class aligned_storage: public aligned_storage_base<size, alignment>
		{
		public:
			aligned_storage();
			template<typename U>
			aligned_storage(U unused, const aligned_storage_tag::ctor&);					// see Logger's constructor implementation for similar way to use this constructor
		};

		// partial specialization to restrict and enforce alignment
		template<size_t size> class aligned_storage<size, 1>: public aligned_storage_base<size, 1> { private: Z_ALIGN(1) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 2>: public aligned_storage_base<size, 2> { private: Z_ALIGN(2) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 4>: public aligned_storage_base<size, 4> { private: Z_ALIGN(4) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 8>: public aligned_storage_base<size, 8> { private: Z_ALIGN(8) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 16>: public aligned_storage_base<size, 16> { private: Z_ALIGN(16) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 32>: public aligned_storage_base<size, 32> { private: Z_ALIGN(32) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 64>: public aligned_storage_base<size, 64> { private: Z_ALIGN(64) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 128>: public aligned_storage_base<size, 128> { private: Z_ALIGN(128) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };
		template<size_t size> class aligned_storage<size, 256>: public aligned_storage_base<size, 256> { private: Z_ALIGN(256) char _s[size]; public: aligned_storage(); template<typename U> aligned_storage(U unused, const aligned_storage_tag::ctor&); };

		// just a helper for not having to type the type each time we use it
		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		class typed_aligned_storage: private aligned_storage<size, alignment>
		{
		public:
			typed_aligned_storage();
			template<typename U>
			typed_aligned_storage(U unused, const aligned_storage_tag::ctor&);				// see Logger's constructor implementation for how to use this constructor
		public:
			~typed_aligned_storage();
		public:
			void*									allocate();
			T&										create();
			void									destroy();
		public:
			const void*								get_unsafe() const;
			void*									get_unsafe();
		public:
			const T&								get() const;
			T&										get();
		};
	};
};

