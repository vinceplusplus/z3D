
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		template<class T>
		class UniqueMap: public map<T,bool>
		{
		public:
			static UniqueMap<T> fromVector(const vector<T>& src);
		public:
			void toVector(vector<T>& array) const;
			vector<T> toVector() const;
			map<T,int> toMapWithIndex() const;
		};

		template<class T>
		UniqueMap<T> UniqueMap<T>::fromVector(const vector<T>& src)
		{
			UniqueMap<T> map;
			int i;
			for(i=0;i<(int)src.size();++i)
				map[src[i]]=true;
			return map;
		}

		template<class T>
		void UniqueMap<T>::toVector(vector<T>& array) const
		{
			map<T,bool>::const_iterator iter;
			for(iter=begin();iter!=end();++iter)
				array.push_back(iter->first);
		}

		template<class T>
		vector<T> UniqueMap<T>::toVector() const
		{
			vector<T> array;
			toVector(array);
			return array;
		}

		template<class T>
		map<T,int> UniqueMap<T>::toMapWithIndex() const
		{
			map<T,int> dest;
			map<T,bool>::const_iterator src_iter;
			int c=-1;
			for(src_iter=begin();src_iter!=end();++src_iter)
				dest[src_iter->first]=++c;
			return dest;
		}
	};
};

