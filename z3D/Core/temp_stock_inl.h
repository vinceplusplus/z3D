
#pragma once

#include "temp_stock.h"

#include "allocators_inl.h"

namespace z3D
{
	namespace Core
	{
		template<class T, class CFG>
		temp_stock<T, CFG>::temp_stock()
		{
			_unused_items = _dual_items + 0;
			_used_items = _dual_items + 1;
		}

		template<class T, class CFG>
		temp_stock<T, CFG>::~temp_stock()
		{
			for(IN_USE_ITEMS::iterator iter = _in_use_items.begin(); iter != _in_use_items.end(); ++iter)
				iter->first->setUniqueTrigger(functor1<void, void*>(), 0);
		}

		template<class T, class CFG>
		void					temp_stock<T, CFG>::setSourceFunc(const functor1<SPtr<T>, const CFG&>& func)
		{
			_source = func;
		}

		template<class T, class CFG>
		void					temp_stock<T, CFG>::setRenewFunc(const functor2<void, const SPtr<T>&, const CFG&>& func)
		{
			_renew = func;
		}

		template<class T, class CFG>
		SPtr<T>					temp_stock<T, CFG>::alloc(const CFG& cfg)
		{
			SPtr<T> item;

			do
			{
				if(_used_items->size())
				{
					ITEMS::iterator iter = _used_items->find(cfg);
					if(iter != _used_items->end())
					{
						item = iter->second;
						_used_items->erase(iter);
						break;
					}
				}
				if(_unused_items->size())
				{
					ITEMS::iterator iter = _unused_items->find(cfg);
					if(iter != _unused_items->end())
					{
						item = iter->second;
						_unused_items->erase(iter);
						break;
					}
				}
				Z_ASSERT( _source.valid() );
				item = _source(cfg);
			}while(false);
			if(!item)
				return NULL;

			_in_use_items.insert(pair<const SPtr<T>, CFG>(item, cfg));

			item->setUniqueTrigger(functor1<void, void*>(this, &temp_stock::dealloc), (void*)item.get());

			return item;
		}

		template<class T, class CFG>
		void					temp_stock<T, CFG>::clear()
		{
			_unused_items->clear();
			_used_items->clear();
		}

		template<class T, class CFG>
		void					temp_stock<T, CFG>::cycle()
		{
			_unused_items->clear();
			swap(_unused_items, _used_items);
		}

		template<class T, class CFG>
		void					temp_stock<T, CFG>::dealloc(void* r)
		{
			SPtr<T> item((T*)r);

			item->setUniqueTrigger(functor1<void, void*>(), 0);

			IN_USE_ITEMS::iterator iter = _in_use_items.find(item);

			Z_ASSERT(iter != _in_use_items.end());

			if(_renew.valid())
				_renew(item, iter->second);

			_used_items->insert(pair<const CFG, SPtr<T>>(iter->second, iter->first));
		}
	};
};

