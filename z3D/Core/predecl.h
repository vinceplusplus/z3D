
#pragma once

#define Z_USE_FAST_STRING

#define WIN32_LEAN_AND_MEAN

#include "../dynamic_memory_allocator_config.h"

#ifdef NDEBUG
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		DON'T DO IT HERE, instead, develop visual studio macros to make them on/off in each projects' release configuration's preprocessor definition
//
//		including 3rd.sln solution's
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//#define _SECURE_SCL 0
	//#define _HAS_ITERATOR_DEBUGGING 0
	//#define _SECURE_SCL_THROWS 0

	////////////////////////////////////////#define _HAS_EXCEPTIONS 0
#endif

#include <stddef.h>
#include "stdint.h"
#include <string>
#include <algorithm>
#include <set>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <hash_map>
#include <stack>
#include <strstream>

#include "Critical/predecl.h"

#ifdef _DEBUG
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#ifdef Z3D_DLL
	#ifdef Z3D_EXPORTS
		#define Z3D_API __declspec(dllexport)
	#else
		#define Z3D_API __declspec(dllimport)
	#endif
#else
	#define Z3D_API 
#endif

using namespace std;
using namespace stdext;

#define __Z_WIDEN(x) L ## x

#define Z_WIDEN(x) __Z_WIDEN(x)

template<bool> struct CompileTimeError {CompileTimeError(...);};
template<> struct CompileTimeError<false> {};

#define Z_STATIC_ASSERT(expr, msg) \
   {\
		class ERROR_##msg {};\
		(sizeof((CompileTimeError<((expr) != 0)>(ERROR_##msg())))); \
   }

Z3D_API void __z_assert_failed(const wchar_t* expr, const wchar_t* file, unsigned int line);
Z3D_API void __z_verify_failed(const wchar_t* expr, const wchar_t* file, unsigned int line);

#if defined(_DEBUG)
#define Z_ASSERT( expr )						(void) ( !!(expr) || (__z_assert_failed(Z_WIDEN(#expr), Z_WIDEN(__FILE__), __LINE__), 0) )
#else
#define Z_ASSERT( expr)
#endif

#if 1
	#define Z_VERIFY( expr )					(void) ( !!(expr) || (__z_verify_failed(Z_WIDEN(#expr), Z_WIDEN(__FILE__), __LINE__), 0) )
#elif
	#if defined(_DEBUG)
		#define Z_VERIFY( expr )					Z_ASSERT( expr )
	#else
		#define Z_VERIFY( expr )					(void) ( !!(expr) || (__z_verify_failed(Z_WIDEN(#expr), Z_WIDEN(__FILE__), __LINE__), 0) )
	#endif
#endif

template<class T, class ALLOCATOR>
const T*		VECTOR_BEGIN_PTR(const vector<T, ALLOCATOR>& x)
{
	Z_ASSERT(x.size());
	return &*x.begin();
}

template<class T, class ALLOCATOR>
T*				VECTOR_BEGIN_PTR(vector<T, ALLOCATOR>& x)
{
	Z_ASSERT(x.size());
	return &*x.begin();
}

template<class T, class ALLOCATOR>
const T*		VECTOR_END_PTR(const vector<T, ALLOCATOR>& x)
{
	Z_ASSERT(x.size());
	return &*x.begin() + x.size();
}

template<class T, class ALLOCATOR>
T*				VECTOR_END_PTR(vector<T, ALLOCATOR>& x)
{
	Z_ASSERT(x.size());
	return &*x.begin() + x.size();
}

#define Z_ALIGN(n)				__declspec(align(n))

template<size_t ALIGNMENT>
bool							Z_IS_ALIGNED(const void* ptr)
{
	Z_STATIC_ASSERT( !(ALIGNMENT & (ALIGNMENT - 1)), ALIGNMENT_MUST_BE_POWER_OF_2 );

	return !(((ptrdiff_t)ptr) & (ALIGNMENT - 1));
}

#define Z_ASSERT_ALIGN(p, n)	Z_ASSERT( Z_IS_ALIGNED<n>(p) )

// http://www.monkeyspeak.com/alignment/

template<class T>	class Z_ALIGNMENT_QUERY	{	T				t;	char			c;	};

template<class T>
class Z_GET_ALIGNMENT
{
public:		enum	{		value = (sizeof(Z_ALIGNMENT_QUERY<T>) > sizeof(T)) ? (sizeof(Z_ALIGNMENT_QUERY<T>) - sizeof(T)) : 0	};
private:	char	t[Z_GET_ALIGNMENT<T>::value * 2 - 1];					// no simple way to report error message, negative script error means alignment detection fails (normally no compiler would optimize in such a way that an added char storage makes no increase in the storage)
};

#define Z_ALIGN_OF(x)			__alignof(x)

// from Modern C++ Design: Generic Programming and Design Patterns Applied

template<class FROM, class TO>
class CONVERTIBLE
{
public:
	typedef struct _BIG {char char2[2];} BIG;
	static FROM		from();
	static char		test(TO);
	static BIG		test(...);
public:
	enum
	{
		value = sizeof(test(from())) == sizeof(char),
		same = false,
	};
};

template<class T>
class CONVERTIBLE<T, T>
{
public:
	enum
	{
		value = true,
		same = true,
	};
};

template<class U, class V>
class SAME_TYPE
{
public:
	enum
	{
		value = false,
	};
};

template<class T>
class SAME_TYPE<T, T>
{
public:
	enum
	{
		value = true,
	};
};

template<size_t U, size_t V>
class static_max
{
public:
	enum: size_t
	{
		value = U > V ? U : V
	};
};

// templated int
// this is a workaround for that we cannot explicitly specify template argument for constructor member functions
// see 14.8.1 [temp.arg.explicit] for more
template<size_t v>
class temp_int{};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// By using the RefBase, SPtr, WPtr systems, you agree to not throw exception inside constructors
//
// If you find that its inevitably to create a SPtr from inside the object's constructor, use SPtrLocker::add(), later inside game loop, just call SPtrLocker::clear()
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define SPTR_THREAD_SAFE

namespace z3D
{
	namespace Core
	{
		typedef float					REAL;
#pragma pack(push)
#pragma pack(1)
		class Z3D_API INT3
		{
		public:
			int32_t		v[3];
		public:
			INT3() {}
			INT3(int32_t v0, int32_t v1, int32_t v2) {v[0] = v0; v[1] = v1; v[2] = v2;}
		public:
			int32_t&	operator[](ptrdiff_t index) { return v[index]; }
			int32_t		operator[](ptrdiff_t index) const { return v[index]; }
		};
#pragma pack(pop)

		// foward declare classes according the order they appear in visual studio

		//////////////////////////////////////////////////////////////////////////////////////
		// Animation
		//////////////////////////////////////////////////////////////////////////////////////

		class Z3D_API AnimCtrl;

		class Z3D_API AnimInst;

		template< class T >
		class AnimKey;

		template< class T >
		class AnimData;

		template< class T >
		class AnimDataEval;

		class Z3D_API CtrlBase;

		class Z3D_API CtrlSet;

		class Z3D_API CtrlUpdater;

		class Z3D_API LOD;

		class Z3D_API TimeAnim;

		class Z3D_API TimeAnimCfg;

		class Z3D_API Timer;

		class Z3D_API TimerService;

		//////////////////////////////////////////////////////////////////////////////////////
		// Math
		//////////////////////////////////////////////////////////////////////////////////////

		class Z3D_API AABB;

		class Z3D_API Distance;

		class Z3D_API Frustum;

		class Z3D_API gaussian;

		class Z3D_API Line;

		class Z3D_API Mat2;

		class Z3D_API Mat3;

		class Z3D_API Mat4;

		class Z3D_API OBB;

		class Z3D_API Plane;

		class Z3D_API Quat;

		class Z3D_API Ray;

		class Z3D_API Sphere;

		class Z3D_API Triangle;

		class Z3D_API TriangleAux;

		class Z3D_API Vec2;

		class Z3D_API Vec3;

		class Z3D_API Vec4;

		//////////////////////////////////////////////////////////////////////////////////////
		// Misc
		//////////////////////////////////////////////////////////////////////////////////////

		class Z3D_API sqlite_res;

		class Z3D_API sqlite_db;

		class Z3D_API W32MemDC;

		//////////////////////////////////////////////////////////////////////////////////////
		// Object System
		//////////////////////////////////////////////////////////////////////////////////////

		class Z3D_API RefBaseImpl;

		class Z3D_API RefBase;

		class Z3D_API ResBase;

		template< class U >
		class SPtr;

		template< class U >
		class WPtr;

		class Z3D_API SPtrLocker;

		class Z3D_API ResMgr;

		class Z3D_API shared_count;

		class Z3D_API singleton_base;

		class Z3D_API TypeInfo;

		//////////////////////////////////////////////////////////////////////////////////////
		// Object System
		//////////////////////////////////////////////////////////////////////////////////////

		class Z3D_API interlocked;

		class Z3D_API mutex;

		class Z3D_API mutex_fast;

		class Z3D_API mutex_rw;

		class Z3D_API mutex_rw2;

		template<class MUTEX>
		class scoped_lock;

		template<class MUTEX_RW>
		class scoped_lock_read;

		template<class MUTEX_RW>
		class scoped_lock_write;

		class Z3D_API sync_spin;

		class Z3D_API W32Thread;

		//////////////////////////////////////////////////////////////////////////////////////
		// Util
		//////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////////////////
		// Util/Lock-Free
		//////////////////////////////////////////////////////////////////////////////////////

		template<class T>
		class lfqueue;

		template<class T>
		class mem_pool;

		template<class T, bool dispose_on_destruct>
		class mem_pool_keeper;

		template<size_t block_size, size_t block_alignment>
		class nt_mem_pool;

		template<size_t block_size, size_t block_alignment, bool dispose_on_destruct>
		class nt_mem_pool_keeper;

		//////////////////////////////////////////////////////////////////////////////////////
		// Util/
		//////////////////////////////////////////////////////////////////////////////////////

		template<size_t size, size_t alignment>
		class aligned_storage_base;

		template<size_t size, size_t alignment>
		class aligned_storage;

		template<typename T, size_t size, size_t alignment, bool destroy_on_destruct>
		class typed_aligned_storage;

		class Z3D_API AlignedHeapOp;

		template<class _Ty>
		class in_place_allocator;

		template<class _Ty>
		class fixed_block_allocator;

		template<size_t AVAILABLE_SIZE, size_t BOOKKEEP_COUNT, size_t ALIGNMENT>
		class fast_temp_mem_allocator;

		template<class _Ty>
		class frame_temp_allocator;

		class Z3D_API AnyValue;

		class Z3D_API biased_random;

		class Z3D_API BitArray;

		class Z3D_API Config;

		class Z3D_API CountDown;

		class Z3D_API Counter;

		class Z3D_API lookup_fast;

		class Z3D_API duo_map;

		class Z3D_API FNGen;

		class Z3D_API FPSCounter;

		class Z3D_API FPSCtrl;

		template<class RET>
		class functor0;
		template<class RET, class P0>
		class functor1;
		template<class RET, class P0, class P1>
		class functor2;
		template<class RET, class P0, class P1, class P2>
		class functor3;
		template<class RET, class P0, class P1, class P2, class P3>
		class functor4;
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		class functor5;
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		class functor6;
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		class functor7;
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		class functor8;

		template<class RET>
		class wfunctor0;
		template<class RET, class P0>
		class wfunctor1;
		template<class RET, class P0, class P1>
		class wfunctor2;
		template<class RET, class P0, class P1, class P2>
		class wfunctor3;
		template<class RET, class P0, class P1, class P2, class P3>
		class wfunctor4;
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		class wfunctor5;
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		class wfunctor6;
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		class wfunctor7;
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		class wfunctor8;

		class Z3D_API LexScan;

		class Z3D_API LoggerSTL;

		class Z3D_API mem_alloc;

		class Z3D_API mt;

		class Z3D_API Profiler;

		template<size_t BUCKETS>
		class QStats;

		class Z3D_API lrng;

		class Z3D_API RiffReader;

		class Z3D_API SMem;

		class Z3D_API stack_any;

		class Z3D_API Stats;

		template<class T>
		class stock;

		template<class T, class CFG>
		class temp_stock;

		class Z3D_API Tokenizer;

		class Z3D_API version;

		class Z3D_API ZipIOBase;
		class Z3D_API ZipBase;
		class Z3D_API UnzipBase;
		class Z3D_API MemZipIO;
		class Z3D_API MemZip;
		class Z3D_API MemUnzip;

		//////////////////////////////////////////////////////////////////////////////////////
	};
};

