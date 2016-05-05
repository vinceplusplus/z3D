
#include "SPtr.h"

namespace z3D
{
	namespace Core
	{
		template<class U>										SPtr<U>::SPtr(U* ref)
		{
			___ctor(ref);
		}

		template<class U>										SPtr<U>::SPtr(const SPtr<U>& p)
		{
			___ctor(p);
		}

		template<class U>template<class V>						SPtr<U>::SPtr(const SPtr<V>& p)
		{
			___ctor(p);
		}

		template<class U>						void			SPtr<U>::___ctor(U* ref)
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			_ref = ref;
			if(_ref)
			{
				_sc = _ref->RefBaseImpl::_sc;
				if(_sc.ref_count() != 2)							// check if this is not the first time to make a SPtr from the RefBaseImpl ref, 2 == the first time. how can i tell this? because we cannot construct a WPtr from a raw pointer(no such constructor available), but SPtr, so the first SPtr can eliminate the call to incrementing the shared count
					++_sc;
			}
		}

		template<class U>template<class V>		void			SPtr<U>::___ctor(const SPtr<V>& p)
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			_ref = p.get_unsafe();
			if(_ref)
			{
				_sc = _ref->RefBaseImpl::_sc;
				++_sc;
			}
		}

		template<class U>										SPtr<U>::~SPtr()
		{
			___dtor(false);
		}

		template<class U>						void			SPtr<U>::___dtor(bool renew_sc)
		{
			if(!_ref)
				return;

			bool delete_ref = false;
			{
#ifdef SPTR_THREAD_SAFE
				scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
				switch(--_sc)
				{
				case 0:
					delete_ref = true;
					break;
				case 1:
					if(_ref && _ref->_unique_trigger.func.valid())
						_ref->_unique_trigger.func(_ref->_unique_trigger.ref);
					break;
				}
			}
			if(delete_ref && _ref)
				_ref->RefBaseImpl::dispose();
			_ref = 0;
			if(renew_sc)
				_sc = shared_count();
		}

		template<class U>						SPtr<U>&		SPtr<U>::operator=(const SPtr<U>& p)
		{
			return operator=<U>(p);
		}

		template<class U>template<class V>		SPtr<U>&		SPtr<U>::operator=(const SPtr<V>& p)
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			if(_ref == p.get_unsafe())
				return *this;
			this->___dtor(true);
			this->___ctor(p);
			return *this;
		}

		template<class U>						void			SPtr<U>::reset()
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			this->___dtor(true);
		}

		template<class U>						U&				SPtr<U>::operator*() const
		{
			Z_ASSERT( _ref );
			return *_ref;
		}

		template<class U>						U*				SPtr<U>::operator->() const
		{
			Z_ASSERT( _ref );
			return _ref;
		}

		template<class U>						U*				SPtr<U>::get() const
		{
			Z_ASSERT( _ref );
			return _ref;
		}

		template<class U>						U*				SPtr<U>::get_unsafe() const
		{
			return _ref;
		}

		template<class U>						bool			SPtr<U>::unique() const
		{
			return use_count() == 1;
		}

		template<class U>						uint32_t		SPtr<U>::use_count() const
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			return _sc.use_count();
		}

		template<class U>template<class V>		SPtr<V>			SPtr<U>::cast() const
		{
			return SPtr<V>((V*)_ref);
		}

		template<class U>template<class V>		SPtr<V>			SPtr<U>::cast_dynamic() const
		{
			return SPtr<V>(dynamic_cast<V*>(_ref));
		}

		template<class U>										SPtr<U>::operator bool() const
		{
			return _ref != 0;
		}
	};
};

