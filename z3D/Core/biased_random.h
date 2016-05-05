
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class biased_random
		{
		private:
			map<REAL, ptrdiff_t>	_map_entry;
			REAL					_sum;
		public:
			biased_random();
		public:
			void					clear();
			void					add(ptrdiff_t data, REAL weight);
			size_t					count() const;
			REAL					sum() const;
		public:
			ptrdiff_t				random(REAL normalized_index) const;			// range from 0 ~ 1 inclusively
		};
	};
};

