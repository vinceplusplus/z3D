
#pragma once

#include "predecl.h"

#include "z_off_debug_new.h"

#include "malloc_alignment.h"

namespace z3D
{
	namespace Core
	{
		// for 4-byte or 8-byte alignment, malloc and new already take care of it, so no need to use AlignedHeapOp
		// if you have an object whose class contains data structures need alignment of 16(e.g. SSE) allocated on heap, you will have to make the class inherit AlignedHeapOp
		// for 64-byte alignment, no simple solution, just change the enum ALIGNMENT to 64, of course more memory will be wasted

		class AlignedHeapOp
		{
		public:
			enum
			{
				ALIGNMENT		=		Z3D_MALLOC_ALIGNMENT,
			};
		protected:
			AlignedHeapOp();
			~AlignedHeapOp();
		public:
			void*					operator new			(size_t s);
			void*					operator new[]			(size_t s);
			void					operator delete			(void* p);
			void					operator delete[]		(void* p);
		public:
			void*					operator new			(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber);
			void*					operator new[]			(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber);
			void					operator delete			(void* _Memory, int _BlockType, const char * _Filename, int _LineNumber);
			void					operator delete[]		(void* _Memory, int _BlockType, const char * _Filename, int _LineNumber);
		};
	};
};

#include "z_on_debug_new.h"

