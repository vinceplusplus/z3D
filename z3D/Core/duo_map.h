
#pragma once

#include "predecl.h"

#include <vector>
using namespace std;

namespace z3D
{
	namespace Core
	{
		// direct address table
		class lookup_fast
		{
		protected:
			typedef struct
			{
				unsigned long		key;
				unsigned long		value;
			} ENTRY;
		protected:
			vector<ENTRY>			_array_entry;
		protected:
			unsigned long			_start;
			unsigned long			_end;
			vector<unsigned long>	_table;
		public:
			void					addPair(unsigned long key,unsigned long value);
		public:
			void					build();
		public:
			unsigned long			lookup(unsigned long key) const;
		};

		class duo_map
		{
		protected:
			lookup_fast			_lookup;
			lookup_fast			_reverse;
		public:
			void				addMapping(unsigned long from,unsigned long to);
			void				build();
		public:
			unsigned long		lookup(unsigned long key) const;
			unsigned long		lookup_reverse(unsigned long key) const;
			template< class T >
			T					lookup(unsigned long key) const	{return (T)_lookup.lookup(key);	}
			template<class T>
			T					lookup_reverse(unsigned long key) const	{return (T)_reverse.lookup(key);}
		};
	};
};