
#include "biased_random.h"

namespace z3D
{
	namespace Core
	{
		biased_random::biased_random()
		{
			_sum = 0.0f;
		}
		void					biased_random::clear()
		{
			_map_entry.clear();
			_sum = 0.0f;
		}
		void					biased_random::add(ptrdiff_t data, REAL weight)
		{
			if(weight <= 0.0f)
				return;
			_map_entry[_sum] = data;
			_sum += weight;
		}
		size_t					biased_random::count() const
		{
			return _map_entry.size();
		}
		REAL					biased_random::sum() const
		{
			return _sum;
		}
		ptrdiff_t				biased_random::random(REAL normalized_index) const
		{
			Z_ASSERT( _map_entry.size() );
			REAL v = normalized_index * _sum;

			map<REAL,int>::const_iterator iter = _map_entry.upper_bound(v);
			if(iter == _map_entry.begin())
				return iter->second;
			return (--iter)->second;
		}
	};
};

