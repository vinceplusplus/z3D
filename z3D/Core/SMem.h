
#pragma once

#include "predecl.h"
#include "functor.h"

namespace z3D
{
	namespace Core
	{
		// smart memory, to aid implementing cross-heap safe freeing
		// not thread-safe for assignment
		class SMem
		{
		private:
			class BLOCK
			{
			public:
				volatile uint32_t		ref_count;
				uint8_t					data[1];
			};
		private:
			BLOCK*					_block;
			size_t					_size;
			functor1<void, void*>	_dealloc_func;
		private:
			void					___ctor();
			void					___ctor(const SMem& rhs);
			void					___dtor();
		public:
			SMem();
			SMem(const SMem& rhs);
		public:
			~SMem();
		public:
			SMem&					operator=(const SMem& rhs);
		public:
			static size_t			getRequiredSize(size_t data_size);
			static SMem				create(void* required_mem, size_t data_size, const functor1<void, void*>& dealloc_func);					// *** the required_mem must be as large as SMem::getRequiredSize(data_size), otherwise, you'll end up corrupting the heap ***
		public:
			void*					get() const;
			size_t					size() const;
		public:
			void					reset();
		};
	};
};


