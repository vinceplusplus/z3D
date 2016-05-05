// GaussianDistribution.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <math.h>

double gau_dist(double sigma, double x)
{
	return 1.0 / (sqrt(2.0 * (2.0 * tan(1.0))) * exp(sigma * (x * x) / (2.0 * sigma * sigma)));
}

double gau_dist(double sigma, double x, double y)
{
	return 1.0 / (2.0 * (2.0 * tan(1.0)) * sigma * sigma * exp(sigma * (x * x + y * y) / (2.0 * sigma * sigma)));
}

template<int N>
void gau_dist_kernel(double sigma, double (&kernel)[N])
{
	if(true)
	{
		class test_odd
		{
			char t[(!!(N & 1)) * 2 - 1];
		};
	}

	double sum = 0.0;
	for(int i = 0; i < N; ++i)
		sum += (kernel[i] = gau_dist(sigma, (double)abs(i - (N >> 1))));
	for(int i = 0; i < N; ++i)
		kernel[i] /= sum;
}

template<int N>
void gau_dist_kernel(double sigma, double (&kernel)[N][N])
{
	if(true)
	{
		class test_odd
		{
			char t[(!!(N & 1)) * 2 - 1];
		};
	}

	double sum = 0.0;
	for(int j = 0; j < N; ++j)
		for(int i = 0; i < N; ++i)
			sum += (kernel[j][i] = gau_dist(sigma, (double)abs(i - (N >> 1)), (double)abs(j - (N >> 1))));
	for(int j = 0; j < N; ++j)
		for(int i = 0; i < N; ++i)
			kernel[j][i] /= sum;
}

template<int N>
double gau_dist_sigma(double (&kernel)[N])
{
	return ((N >> 1) + 1.0) / 3.0;
}

template<int N>
double gau_dist_sigma(double (&kernel)[N][N])
{
	return ((N >> 1) + 1.0) / 3.0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(true)
	{
		const int N = 5;

		double kernel[N];
		gau_dist_kernel(gau_dist_sigma(kernel), kernel);

		for(int i = 0; i < N; ++i)
			printf("%f ", kernel[i]);
		printf("\n");
	}

	printf("\n");

	if(true)
	{
		const int N = 9;

		double kernel[N][N];
		gau_dist_kernel(gau_dist_sigma(kernel), kernel);

		for(int j = 0; j < N; ++j)
		{
			for(int i = 0; i < N; ++i)
				printf("%f ", kernel[j][i]);
			printf("\n");
		}
	}

	return 0;
}

