
#include "gaussian.h"
#include <math.h>

namespace z3D
{
	namespace Core
	{
		double			gaussian::dist(double sigma, double x)
		{
			return 1.0 / (sqrt(2.0 * (2.0 * tan(1.0))) * exp(sigma * (x * x) / (2.0 * sigma * sigma)));
		}
		double			gaussian::dist(double sigma, double x, double y)
		{
			return 1.0 / (2.0 * (2.0 * tan(1.0)) * sigma * sigma * exp(sigma * (x * x + y * y) / (2.0 * sigma * sigma)));
		}
		double			gaussian::dist_sigma(size_t N)
		{
			return ((N >> 1) + 1.0) / 3.0;
		}
		ptrdiff_t		gaussian::__abs(ptrdiff_t x)
		{
			return abs(x);
		}
	};
};

