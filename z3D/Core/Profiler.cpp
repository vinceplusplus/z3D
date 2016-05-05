
#include "Profiler.h"
#include "Misc.h"
#include "MathFunc.h"

namespace z3D
{
	namespace Core
	{
		Profiler::Profiler()
		{
		}
		Profiler::~Profiler()
		{
			/*
			wcs s=report();
			if(s.length())
				MessageBoxW(NULL,s.c_str(),L"",0);
				*/
		}
		void						Profiler::start(const wcs& name)
		{
			ProfilerRecord& r=_map_name_record[name];
			if(!r.name.length())
				r.name=name;
			r.timer.update();
		}
		void						Profiler::stop(const wcs& name)
		{
			ProfilerRecord& r=_map_name_record[name];
			r.timer.update();
			r.acc_time+=r.timer.elapsedTime();
			++r.count;
			r.avg_time=r.acc_time/(REAL)r.count;
		}
		REAL						Profiler::getTotalTime(const wcs& name) const
		{
			hash_map<wcs,ProfilerRecord>::const_iterator iter=_map_name_record.find(name);
			//auto iter=_map_name_record.find(name);								// test for C++98
			if(iter==_map_name_record.end())
				return 0.0f;
			const Profiler::ProfilerRecord& r=iter->second;
			return r.acc_time;
		}
		int						Profiler::getCount(const wcs& name) const
		{
			hash_map<wcs,ProfilerRecord>::const_iterator iter=_map_name_record.find(name);
			if(iter==_map_name_record.end())
				return 0;
			const Profiler::ProfilerRecord& r=iter->second;
			return r.count;
		}
		REAL						Profiler::getAverageTime(const wcs& name) const
		{
			hash_map<wcs,ProfilerRecord>::const_iterator iter=_map_name_record.find(name);
			if(iter==_map_name_record.end())
				return 0.0f;
			const Profiler::ProfilerRecord& r=iter->second;
			return r.avg_time;
		}
		const vector<wcs>	Profiler::getNames() const
		{
			vector<wcs> array_name;
			hash_map<wcs,ProfilerRecord>::const_iterator iter;
			for(iter=_map_name_record.begin();iter!=_map_name_record.end();++iter)
				array_name.push_back(iter->first);
			return array_name;
		}
		const wcs			Profiler::report() const
		{
			wcs s;
			s+=FORMATW(L"%-25s%17s%11s%17s\r\n",L"Name",L"Total Time",L"Count",L"Average Time");

			hash_map<wcs,ProfilerRecord>::const_iterator iter;
			for(iter=_map_name_record.begin();iter!=_map_name_record.end();++iter)
			{
				const ProfilerRecord& r=iter->second;

				s+=FORMATW(L"%-25s%17.6f%11d%17.6f\r\n",r.name.c_str(),r.acc_time,r.count,r.avg_time);
			}
			return s;
		}
		const wcs				Profiler::report_sort_total_time(bool ascending) const
		{
			return L"";
		}
		const wcs				Profiler::report_sort_count(bool ascending) const
		{
			return L"";
		}
		const wcs				Profiler::report_sort_average_time(bool ascending) const
		{
			return L"";
		}
	};
};
