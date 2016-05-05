
#include "Stats.h"

#include <math.h>

namespace z3D
{
	namespace Core
	{
		Stats::Stats()
		{
			reset();
		}
		void	Stats::reset()
		{
			_sum = 0;
			_sum_sq = 0;
			_count = 0;
		}
		void	Stats::input(double x)
		{
			_sum += x;
			_sum_sq += x * x;
			++_count;
		}
		void	Stats::deinput(double x)
		{
			_sum -= x;
			_sum_sq -= x * x;
			--_count;
		}
		double	Stats::sum() const
		{
			return _sum;
		}
		double	Stats::square_sum() const
		{
			return _sum_sq;
		}
		size_t	Stats::count() const
		{
			return _count;
		}
		double	Stats::mean() const
		{
			if(!_count)
				return 0;
			return _sum / _count;
		}
		double	Stats::variance() const
		{
			double _mean = mean();
			return (_count ? (_sum_sq / _count) : 0) - _mean * _mean;
		}
		double	Stats::standard_deviation() const
		{
			return sqrt(variance());
		}
	};
};

