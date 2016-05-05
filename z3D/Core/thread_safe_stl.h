
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		template<class T>
		class vector_rw: public vector<T>, public mutex_rw
		{
		public:
			vector_rw():vector<T>(){}
			~vector_rw(){}
		};

		template<class T>
		class list_rw: public list<T>, public mutex_rw
		{
		public:
			list_rw():list<T>(){}
			~list_rw(){}
		};

		template<class S,class T>
		class map_rw: public map<S,T>, public mutex_rw
		{
		public:
			map_rw():map<S,T>(){}
			~map_rw(){}
		};
	};
};

