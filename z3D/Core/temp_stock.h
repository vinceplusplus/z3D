
#pragma once

#include "predecl.h"

#include "functor.h"

#include "allocators.h"

namespace z3D
{
	namespace Core
	{
		template<class T, class CFG>
		class temp_stock
		{
		private:
			typedef typename multimap<CFG, SPtr<T>, less<CFG>, in_place_allocator<pair<const CFG, SPtr<T>>>>
											ITEMS;
			typedef typename map<SPtr<T>, CFG, less<SPtr<T>>, in_place_allocator<pair<const SPtr<T>, CFG>>>
											IN_USE_ITEMS;
		private:
			ITEMS							_dual_items[2];
			ITEMS*							_unused_items;
			ITEMS*							_used_items;
			IN_USE_ITEMS					_in_use_items;
		private:
			functor1<SPtr<T>, const CFG&>	_source;
			functor2<void, const SPtr<T>&, const CFG&>
											_renew;
		public:
			temp_stock();
			~temp_stock();
		public:
			void					setSourceFunc(const functor1<SPtr<T>, const CFG&>& func);
			void					setRenewFunc(const functor2<void, const SPtr<T>&, const CFG&>& func);
		public:
			SPtr<T>					alloc(const CFG& cfg);
			void					clear();
		public:
			void					cycle();								// this call should be called at the beginning of each frame
		private:
			void					dealloc(void* r);
		};
	};
};

