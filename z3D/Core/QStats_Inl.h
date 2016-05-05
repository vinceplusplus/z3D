
#include "QStats.h"

namespace z3D
{
	namespace Core
	{
		template<size_t BUCKETS>
		QStats<BUCKETS>::QStats()
		{
			Z_STATIC_ASSERT( BUCKETS, INVALID_NUM_BUCKETS );

			_used = 0;
			_next_pos = 0;
		}
		template<size_t BUCKETS>
		void	QStats<BUCKETS>::reset()
		{
			_used = 0;
			_next_pos = 0;

			Stats::reset();
		}
		template<size_t BUCKETS>
		void	QStats<BUCKETS>::input(double x)
		{
			if(_used >= BUCKETS)
			{
				Stats::deinput(_queue[_next_pos]);
				--_used;
			}
			_queue[_next_pos] = x;
			Stats::input(x);
			_next_pos = (_next_pos + 1) % BUCKETS;
			++_used;
		}
		template<size_t BUCKETS>
		size_t	QStats<BUCKETS>::getNumBuckets() const
		{
			return BUCKETS;
		}
	};
};
