
#pragma once

#include "Stats.h"

namespace z3D
{
	namespace Core
	{
		// Queue Stats
		template<size_t BUCKETS>
		class QStats: public Stats
		{
		private:
			double	_queue[BUCKETS];
			size_t	_used;
			size_t	_next_pos;
		public:
			QStats();
		public:
			void	reset();
		public:
			void	input(double x);
		public:
			size_t	getNumBuckets() const;
		};
	};
};

#include "QStats_Inl.h"