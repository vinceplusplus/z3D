
#pragma once

#include "stock.h"

#include "allocators_inl.h"

namespace z3D
{
	namespace Core
	{
		template<class T>
		stock<T>::stock() {}

		template<class T>
		stock<T>::~stock()
		{
			for(IN_USE_ITEMS::iterator iter = _in_use_items.begin(); iter != _in_use_items.end(); ++iter)
				(*iter)->setUniqueTrigger(functor1<void, void*>(), 0);
		}

		template<class T>
		void					stock<T>::setSourceFunc(const functor0<SPtr<T>>& func)
		{
			_source = func;
		}

		template<class T>
		void					stock<T>::setRenewFunc(const functor1<void, const SPtr<T>&>& func)
		{
			_renew = func;
		}

		template<class T>
		SPtr<T>					stock<T>::alloc()
		{
			if(!_free_items.size())
			{
				Z_ASSERT( _source.valid() );
				SPtr<T> o = _source();
				if(o)
					_free_items.push_front(o);
			}

			if(!_free_items.size())
				return NULL;

			SPtr<T> item = _free_items.front();
			_free_items.pop_front();

			_in_use_items.insert(item);

			item->setUniqueTrigger(functor1<void, void*>(this, &stock::dealloc), (void*)item.get());

			return item;
		}

		template<class T>
		void					stock<T>::clear()
		{
			_free_items.clear();
		}

		template<class T>
		void					stock<T>::dealloc(void* r)
		{
			SPtr<T> item((T*)r);

			item->setUniqueTrigger(functor1<void, void*>(), 0);

			_in_use_items.erase(item);

			if(_renew.valid())
				_renew(item);

			_free_items.push_front(item);
		}
	};
};

