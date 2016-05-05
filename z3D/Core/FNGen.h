
#pragma once

#include "predecl.h"
#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		class FNGen
		{
		public:
			static wcs		gen(const wcs& prefix, int digits, const wcs suffix);
		};
	};
};



