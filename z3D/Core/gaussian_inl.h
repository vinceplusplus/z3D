
#pragma once

#include "gaussian.h"

namespace z3D
{
	namespace Core
	{
		template<size_t N>
		static void				gaussian::dist_kernel(double sigma, double (&kernel)[N])
		{
			Z_STATIC_ASSERT( N % 2 == 1, WRONG_ASSUMPTION );

			double sum = 0.0;
			for(ptrdiff_t i = 0; i < N; ++i)
				sum += (kernel[i] = dist(sigma, (double)__abs(i - (N >> 1))));
			for(ptrdiff_t i = 0; i < N; ++i)
				kernel[i] /= sum;
		}
		template<size_t N>
		static void				gaussian::dist_kernel(double sigma, double (&kernel)[N][N])
		{
			Z_STATIC_ASSERT( N % 2 == 1, WRONG_ASSUMPTION );

			double sum = 0.0;
			for(ptrdiff_t j = 0; j < N; ++j)
				for(ptrdiff_t i = 0; i < N; ++i)
					sum += (kernel[j][i] = dist(sigma, (double)__abs(i - (N >> 1)), (double)__abs(j - (N >> 1))));
			for(ptrdiff_t j = 0; j < N; ++j)
				for(ptrdiff_t i = 0; i < N; ++i)
					kernel[j][i] /= sum;
		}
	};
};

