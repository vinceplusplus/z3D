
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class mem_alloc
		{
		public:
			static void*			__alloc(size_t size);
			static void				__free(void* mem);
			static void*			__calloc(size_t num, size_t size);
			static void*			__realloc(void* mem, size_t size);
			static void*			__recalloc(void* mem, size_t num, size_t size);
			static void*			__expand(void* mem, size_t size);
			static size_t			__msize(void* mem);
			static void*			__aligned_alloc(size_t size, size_t alignment);
			static void				__aligned_free(void* mem);

			static void*			__alloc_dbg(size_t size, int block_type, const char* filename, int lineno);
			static void				__free_dbg(void* mem, int block_type);
			static void*			__calloc_dbg(size_t num, size_t size, int block_type, const char* filename, int lineno);
			static void*			__realloc_dbg(void* mem, size_t size, int block_type, const char* filename, int lineno);
			static void*			__recalloc_dbg(void* mem, size_t num, size_t size, int block_type, const char* filename, int lineno);
			static void*			__expand_dbg(void* mem, size_t size, int block_type, const char* filename, int lineno);
			static size_t			__msize_dbg(void* mem, int block_type);
			static void*			__aligned_alloc_dbg(size_t size, size_t alignment, const char* filename, int lineno);
			static void				__aligned_free_dbg(void* mem);
		};
#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#define __alloc(s)					__alloc_dbg(s, 1, __FILE__, __LINE__)
#define __free(p)					__free_dbg(p, 1)
#define __calloc(n, s)				__calloc_dbg(n, s, 1, __FILE__, __LINE__)
#define __realloc(p, s)				__realloc_dbg(p, s, 1, __FILE__, __LINE__)
#define __recalloc(p, n, s)			__recalloc_dbg(p, n, s, 1, __FILE__, __LINE__)
#define __expand(p, s)				__expand_dbg(p, s, 1, __FILE__, __LINE__)
#define __msize(p)					__msize_dbg(p, 1)
#define __aligned_alloc(s, a)		__aligned_alloc_dbg(s, a, __FILE__, __LINE__)
#define __aligned_free(p)			__aligned_free_dbg(p)
#endif
	};
};


