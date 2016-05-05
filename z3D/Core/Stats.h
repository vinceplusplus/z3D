
#pragma once

#include "predecl.h"

#include "MathFunc.h"

namespace z3D
{
	namespace Core
	{
		class Stats
		{
		private:
			double		_sum;
			double		_sum_sq;
			size_t		_count;
		public:
			Stats();
		public:
			void		reset();
		public:
			void		input(double x);
			void		deinput(double x);
		public:
			double		sum() const;
			double		square_sum() const;
			size_t		count() const;
			double		mean() const;
			double		variance() const;
			double		standard_deviation() const;
		};
	};
};
