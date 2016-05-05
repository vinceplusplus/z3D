
#pragma once

#include "predecl.h"

#include "functor.h"

#include "allocators.h"

namespace z3D
{
	namespace Core
	{
		template<class T>
		class stock
		{
		private:
			typedef typename list<SPtr<T>, in_place_allocator<SPtr<T>>>
											FREE_ITEMS;
			typedef typename set<SPtr<T>, less<SPtr<T>>, in_place_allocator<SPtr<T>>>
											IN_USE_ITEMS;
			FREE_ITEMS						_free_items;
			IN_USE_ITEMS					_in_use_items;
		private:
			functor0<SPtr<T>>				_source;
			functor1<void, const SPtr<T>&>	_renew;
		public:
			stock();
			~stock();
		public:
			void					setSourceFunc(const functor0<SPtr<T>>& func);
			void					setRenewFunc(const functor1<void, const SPtr<T>&>& func);
		public:
			SPtr<T>					alloc();
			void					clear();
		private:
			void					dealloc(void* r);
		};
	};
};

