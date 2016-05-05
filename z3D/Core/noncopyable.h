
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class noncopyable
		{
		protected:
			noncopyable() {}
			~noncopyable() {}
		protected:
			noncopyable(const noncopyable&) {}
			noncopyable& operator=(const noncopyable&) {return *this;}
		};
	};
};

