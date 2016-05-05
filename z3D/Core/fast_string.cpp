
#include "fast_string.h"

#include "mem_pool_inl.h"

namespace z3D
{
	namespace Core
	{

#ifdef Z_USE_FAST_STRING

		// explicit instantiation
		template str_allocator_pool::POOL;

		string			CONVERT(const mbs& s) { return string(s.c_str(), s.length()); }
		wstring			CONVERT(const wcs& s) { return wstring(s.c_str(), s.length()); }
		mbs				CONVERT(const string& s) { return mbs(s.c_str(), s.length()); }
		wcs				CONVERT(const wstring& s) { return wcs(s.c_str(), s.length()); }

#else

		mbs				CONVERT(const mbs& s) { return s; }
		wcs				CONVERT(const wcs& s) { return s; }

#endif
	};
};

