
#include "AlignedHeapOp.h"

#include "mem_alloc.h"

namespace z3D
{
	namespace Core
	{
#include "z_off_debug_new.h"

		AlignedHeapOp::AlignedHeapOp()		{}
		AlignedHeapOp::~AlignedHeapOp()		{}
		void*					AlignedHeapOp::operator new				(size_t s)		{		return mem_alloc::__aligned_alloc(s, ALIGNMENT);		}
		void*					AlignedHeapOp::operator new[]			(size_t s)		{		return mem_alloc::__aligned_alloc(s, ALIGNMENT);		}
		void					AlignedHeapOp::operator delete			(void* p)		{		mem_alloc::__aligned_free(p);							}
		void					AlignedHeapOp::operator delete[]		(void* p)		{		mem_alloc::__aligned_free(p);							}
#ifdef _DEBUG
		void*					AlignedHeapOp::operator new				(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)			{		return mem_alloc::__aligned_alloc_dbg(_Size, ALIGNMENT, _Filename, _LineNumber);		}
		void*					AlignedHeapOp::operator new[]			(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)			{		return mem_alloc::__aligned_alloc_dbg(_Size, ALIGNMENT, _Filename, _LineNumber);		}
		void					AlignedHeapOp::operator delete			(void* _Memory, int _BlockType, const char * _Filename, int _LineNumber)		{		mem_alloc::__aligned_free_dbg(_Memory);													}
		void					AlignedHeapOp::operator delete[]		(void* _Memory, int _BlockType, const char * _Filename, int _LineNumber)		{		mem_alloc::__aligned_free_dbg(_Memory);													}
#else
		void*					AlignedHeapOp::operator new				(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)			{		return mem_alloc::__aligned_alloc(_Size, ALIGNMENT);									}
		void*					AlignedHeapOp::operator new[]			(size_t _Size, int _BlockType, const char * _Filename, int _LineNumber)			{		return mem_alloc::__aligned_alloc(_Size, ALIGNMENT);									}
		void					AlignedHeapOp::operator delete			(void* _Memory, int _BlockType, const char * _Filename, int _LineNumber)		{		mem_alloc::__aligned_free(_Memory);														}
		void					AlignedHeapOp::operator delete[]		(void* _Memory, int _BlockType, const char * _Filename, int _LineNumber)		{		mem_alloc::__aligned_free(_Memory);														}
#endif

#include "z_on_debug_new.h"
	};
};

