
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class gaussian
		{
		public:
			static double			dist(double sigma, double x);
			static double			dist(double sigma, double x, double y);
			template<size_t N>
			static void				dist_kernel(double sigma, double (&kernel)[N]);
			template<size_t N>
			static void				dist_kernel(double sigma, double (&kernel)[N][N]);
			static double			dist_sigma(size_t N);
		private:
			static ptrdiff_t		__abs(ptrdiff_t x);
		};
	};
};

