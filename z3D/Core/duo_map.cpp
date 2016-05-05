
#include "duo_map.h"

namespace z3D
{
	namespace Core
	{
		void					lookup_fast::addPair(unsigned long key,unsigned long value)
		{
			ENTRY e;
			e.key=key;
			e.value=value;
			_array_entry.push_back(e);
		}
		void					lookup_fast::build()
		{
			unsigned long start=0xffffffff,end=0;
			for(int i=0;i<(int)_array_entry.size();++i)
			{
				start=min(start,_array_entry[i].key);
				end=max(end,_array_entry[i].key);
			}
			if(end-start+1>1024)
				return;
			_start=start;
			_end=end;
			_table.resize(end-start+1);
			for(int i=0;i<(int)_array_entry.size();++i)
			{
				const ENTRY& e=_array_entry[i];
				_table[e.key-_start]=e.value;
			}
		}
		unsigned long			lookup_fast::lookup(unsigned long key) const
		{
			if(key<_start || key>_end)
				return 0x77777777;
			return _table[key-_start];
		}

		void				duo_map::addMapping(unsigned long from,unsigned long to)
		{
			_lookup.addPair(from,to);
			_reverse.addPair(to,from);
		}
		void				duo_map::build()
		{
			_lookup.build();
			_reverse.build();
		}
		unsigned long		duo_map::lookup(unsigned long key) const
		{
			return _lookup.lookup(key);
		}
		unsigned long		duo_map::lookup_reverse(unsigned long key) const
		{
			return _reverse.lookup(key);
		}
	};
};