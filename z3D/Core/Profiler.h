
#pragma once

#include "predecl.h"

#include "Timer.h"
#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		class Profiler
		{
		protected:
			class ProfilerRecord
			{
			public:
				wcs				name;
				Timer			timer;
				REAL			acc_time;
				int				count;
				REAL			avg_time;
			public:
				ProfilerRecord() {acc_time=0.0f;count=0;avg_time=0.0f;}
			};
		protected:
			hash_map<wcs,ProfilerRecord>		_map_name_record;
		public:
			Profiler();
			virtual ~Profiler();
		public:
			void					start(const wcs& name);
			void					stop(const wcs& name);
		public:
			REAL					getTotalTime(const wcs& name) const;
			int						getCount(const wcs& name) const;
			REAL					getAverageTime(const wcs& name) const;
		public:
			const vector<wcs>		getNames() const;
		public:
			const wcs				report() const;
			const wcs				report_sort_total_time(bool ascending) const;
			const wcs				report_sort_count(bool ascending) const;
			const wcs				report_sort_average_time(bool ascending) const;
		};
	};
};


