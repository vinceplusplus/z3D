
#pragma once

#include "Core/malloc_alignment.h"

#include "Core/mem_alloc.h"

#include "../z3D/Core/z_off_debug_new.h"

#include <new.h>

#ifdef Z3D_USE_NEDMALLOC
	extern "C" __declspec(dllimport) __declspec(noalias) __declspec(restrict)	void * memalign(size_t alignment, size_t bytes) throw();
	extern "C" __declspec(dllimport) __declspec(noalias) __declspec(restrict)	void * malloc(size_t bytes) throw();
	extern "C" __declspec(dllimport) __declspec(noalias)						void free(void* mem) throw();
	extern "C" __declspec(dllimport) __declspec(noalias) __declspec(restrict)	void * calloc(size_t no, size_t size) throw();
	extern "C" __declspec(dllimport) __declspec(noalias) __declspec(restrict)	void * realloc(void *mem, size_t size) throw();
	extern "C" __declspec(dllimport) __declspec(noalias) __declspec(restrict)	void * _recalloc(void *mem, size_t no, size_t size) throw();
	extern "C" __declspec(dllimport) __declspec(noalias) __declspec(restrict)	void * _expand(void *mem, size_t size) throw();
	extern "C" __declspec(dllimport) __declspec(noalias)						size_t _msize(void* mem) throw();

	namespace z3D
	{
		namespace Core
		{
			void*			mem_alloc::__alloc(size_t size)
			{
				return malloc(size);
			}
			void			mem_alloc::__free(void* mem)
			{
				free(mem);
			}
			void*			mem_alloc::__calloc(size_t num, size_t size)
			{
				return calloc(num, size);
			}
			void*			mem_alloc::__realloc(void* mem, size_t size)
			{
				return realloc(mem, size);
			}
			void*			mem_alloc::__recalloc(void* mem, size_t no, size_t size)
			{
				return _recalloc(mem, no, size);
			}
			void*			mem_alloc::__expand(void* mem, size_t size)
			{
				return _expand(mem, size);
			}
			size_t			mem_alloc::__msize(void* mem)
			{
				return _msize(mem);
			}
			void*			mem_alloc::__aligned_alloc(size_t size, size_t alignment)
			{
				return memalign(alignment, size);
			}
			void			mem_alloc::__aligned_free(void* mem)
			{
				free(mem);
			}
			void*			mem_alloc::__alloc_dbg(size_t size, int block_type, const char* filename, int lineno)
			{
				return __alloc(size);
			}
			void			mem_alloc::__free_dbg(void* mem, int block_type)
			{
				__free(mem);
			}
			void*			mem_alloc::__calloc_dbg(size_t num, size_t size, int block_type, const char* filename, int lineno)
			{
				return __calloc(num, size);
			}
			void*			mem_alloc::__realloc_dbg(void* mem, size_t size, int block_type, const char* filename, int lineno)
			{
				return __realloc(mem, size);
			}
			void*			mem_alloc::__recalloc_dbg(void* mem, size_t no, size_t size, int block_type, const char* filename, int lineno)
			{
				return __recalloc(mem, no, size);
			}
			void*			mem_alloc::__expand_dbg(void* mem, size_t size, int block_type, const char* filename, int lineno)
			{
				return __expand(mem, size);
			}
			size_t			mem_alloc::__msize_dbg(void* mem, int block_type)
			{
				return __msize(mem);
			}
			void*			mem_alloc::__aligned_alloc_dbg(size_t size, size_t alignment, const char* filename, int lineno)
			{
				return __aligned_alloc(size, alignment);
			}
			void			mem_alloc::__aligned_free_dbg(void* mem)
			{
				__aligned_free(mem);
			}
		};
	};

	void*			operator new(size_t s) throw(...)
	{
		return memalign(Z3D_MALLOC_ALIGNMENT, s);
	}
	void*			operator new[](size_t s) throw(...)
	{
		return memalign(Z3D_MALLOC_ALIGNMENT, s);
	}
	void*			operator new(size_t s, const std::nothrow_t&) throw()
	{
		return memalign(Z3D_MALLOC_ALIGNMENT, s);
	}
	void*			operator new[](size_t s, const std::nothrow_t&) throw()
	{
		return memalign(Z3D_MALLOC_ALIGNMENT, s);
	}
	void			operator delete(void* p) throw(...)
	{
		free(p);
	}
	void			operator delete[](void* p) throw(...)
	{
		free(p);
	}
	void			operator delete(void* p, const std::nothrow_t&) throw()
	{
		free(p);
	}
	void			operator delete[](void* p, const std::nothrow_t&) throw()
	{
		free(p);
	}
	void*			operator new(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)
	{
		return memalign(Z3D_MALLOC_ALIGNMENT, _Size);
	}
	void*			operator new[](size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)
	{
		return memalign(Z3D_MALLOC_ALIGNMENT, _Size);
	}

	#pragma message("\t")
	#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
	#pragma message("!!!!!!!!!!!!!!!                                                       WARNING!!!")
	#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
	#pragma message("!! ")
	#pragma message("!! YOU OPT TO USE NEDMALLOC, MAKE SURE nedmalloc.lib IS IN THE ADDITIONAL DEPENDENCY IN THE APPLICATION PROJECT")
	#pragma message("!! INSTEAD OF PLACING #pragma comment(lib, \"nedmalloc.lib\") somewhere in the source code")
	#pragma message("!! OTHERWISE NEDMALLOC WILL NOT BE THE FIRST LIBRARY TO LOOK UP WHEN RESOLVING SYMBOLS FOR malloc, free, calloc, realloc")
	#pragma message("!! ")
	#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
	#pragma message("\t")

#else

#include "dynamic_memory_allocator_config.h"

#pragma push_macro("__alloc")
#pragma push_macro("__free")
#pragma push_macro("__calloc")
#pragma push_macro("__realloc")
#pragma push_macro("__recalloc")
#pragma push_macro("__expand")
#pragma push_macro("__msize")
#pragma push_macro("__aligned_alloc")
#pragma push_macro("__aligned_free")

#undef __alloc
#undef __free
#undef __calloc
#undef __realloc
#undef __recalloc
#undef __expand
#undef __msize
#undef __aligned_alloc
#undef __aligned_free

	namespace z3D
	{
		namespace Core
		{
			void*			mem_alloc::__alloc(size_t size)
			{
				return malloc(size);
			}
			void			mem_alloc::__free(void* mem)
			{
				free(mem);
			}
			void*			mem_alloc::__calloc(size_t num, size_t size)
			{
				return calloc(num, size);
			}
			void*			mem_alloc::__realloc(void* mem, size_t size)
			{
				return realloc(mem, size);
			}
			void*			mem_alloc::__recalloc(void* mem, size_t no, size_t size)
			{
				return _recalloc(mem, no, size);
			}
			void*			mem_alloc::__expand(void* mem, size_t size)
			{
				return _expand(mem, size);
			}
			size_t			mem_alloc::__msize(void* mem)
			{
				return _msize(mem);
			}
			void*			mem_alloc::__aligned_alloc(size_t size, size_t alignment)
			{
				return _aligned_malloc(size, alignment);
			}
			void			mem_alloc::__aligned_free(void* mem)
			{
				_aligned_free(mem);
			}
			void*			mem_alloc::__alloc_dbg(size_t size, int block_type, const char* filename, int lineno)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				return _malloc_dbg(size, block_type, filename, lineno);
#else
				return malloc(size);
#endif
			}
			void			mem_alloc::__free_dbg(void* mem, int block_type)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				_free_dbg(mem, block_type);
#else
				free(mem);
#endif
			}
			void*			mem_alloc::__calloc_dbg(size_t num, size_t size, int block_type, const char* filename, int lineno)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				return _calloc_dbg(num, size, block_type, filename, lineno);
#else
				return calloc(num, size);
#endif
			}
			void*			mem_alloc::__realloc_dbg(void* mem, size_t size, int block_type, const char* filename, int lineno)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				return _realloc_dbg(mem, size, block_type, filename, lineno);
#else
				return realloc(mem, size);
#endif
			}
			void*			mem_alloc::__recalloc_dbg(void* mem, size_t no, size_t size, int block_type, const char* filename, int lineno)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				return _recalloc_dbg(mem, no, size, block_type, filename, lineno);
#else
				return _recalloc(mem, no, size);
#endif
			}
			void*			mem_alloc::__expand_dbg(void* mem, size_t size, int block_type, const char* filename, int lineno)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				return _expand_dbg(mem, size, block_type, filename, lineno);
#else
				return _expand(mem, size);
#endif
			}
			size_t			mem_alloc::__msize_dbg(void* mem, int block_type)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				return _msize_dbg(mem, block_type);
#else
				return _msize(mem);
#endif
			}
			void*			mem_alloc::__aligned_alloc_dbg(size_t size, size_t alignment, const char* filename, int lineno)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				return _aligned_malloc_dbg(size, alignment, filename, lineno);
#else
				return _aligned_malloc(size, alignment);
#endif
			}
			void			mem_alloc::__aligned_free_dbg(void* mem)
			{
#if defined(_CRTDBG_MAP_ALLOC)
				_aligned_free_dbg(mem);
#else
				_aligned_free(mem);
#endif
			}
		};
	};

#pragma pop_macro("__alloc")
#pragma pop_macro("__free")
#pragma pop_macro("__calloc")
#pragma pop_macro("__realloc")
#pragma pop_macro("__recalloc")
#pragma pop_macro("__expand")
#pragma pop_macro("__msize")
#pragma pop_macro("__aligned_alloc")
#pragma pop_macro("__aligned_free")

	void*			operator new(size_t s) throw(...)
	{
		return _aligned_malloc(s, Z3D_MALLOC_ALIGNMENT);
	}
	void*			operator new[](size_t s) throw(...)
	{
		return _aligned_malloc(s, Z3D_MALLOC_ALIGNMENT);
	}
	void*			operator new(size_t s, const std::nothrow_t&) throw()
	{
		return _aligned_malloc(s, Z3D_MALLOC_ALIGNMENT);
	}
	void*			operator new[](size_t s, const std::nothrow_t&) throw()
	{
		return _aligned_malloc(s, Z3D_MALLOC_ALIGNMENT);
	}
	void			operator delete(void* p) throw(...)
	{
		_aligned_free(p);
	}
	void			operator delete[](void* p) throw(...)
	{
		_aligned_free(p);
	}
	void			operator delete(void* p, const std::nothrow_t&) throw()
	{
		_aligned_free(p);
	}
	void			operator delete[](void* p, const std::nothrow_t&) throw()
	{
		_aligned_free(p);
	}
	void*			operator new(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)
	{
	#ifdef _DEBUG
		return _aligned_malloc_dbg(_Size, Z3D_MALLOC_ALIGNMENT, _Filename, _LineNumber);
	#else
		return _aligned_malloc(_Size, Z3D_MALLOC_ALIGNMENT);
	#endif
	}
	void*			operator new[](size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)
	{
	#ifdef _DEBUG
		return _aligned_malloc_dbg(_Size, Z3D_MALLOC_ALIGNMENT, _Filename, _LineNumber);
	#else
		return _aligned_malloc(_Size, Z3D_MALLOC_ALIGNMENT);
	#endif
	}

	// just make sure that we don't link with nedmalloc.lib accidentally
	#pragma comment(linker, "/NODEFAULTLIB:nedmalloc.lib")
#endif

#include "../z3D/Core/z_on_debug_new.h"