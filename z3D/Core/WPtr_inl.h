
#include "WPtr.h"

namespace z3D
{
	namespace Core
	{
		template<class U>										WPtr<U>::WPtr()
		{
			___ctor();
		}

		template<class U>										WPtr<U>::WPtr(const WPtr<U>& p)
		{
			___ctor(p);
		}

		template<class U>template<class V>						WPtr<U>::WPtr(const SPtr<V>& p)
		{
			___ctor(p);
		}

		template<class U>template<class V>						WPtr<U>::WPtr(const WPtr<V>& p)
		{
			___ctor(p);
		}

		template<class U>						void			WPtr<U>::___ctor()
		{
			_ref = 0;
		}

		template<class U>template<class V>		void			WPtr<U>::___ctor(const SPtr<V>& p)
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			if(p.get_unsafe())
			{
				_ref = p.get_unsafe();
				_sc = _ref->RefBaseImpl::_sc;
			}
			else
			{
				_ref = 0;
				_sc = shared_count();
			}
		}

		template<class U>template<class V>		void			WPtr<U>::___ctor(const WPtr<V>& p)
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			if(p.valid())
			{
				_ref = p.get_unsafe();
				_sc = _ref->RefBaseImpl::_sc;
			}
			else
			{
				_ref = 0;
				_sc = shared_count();
			}
		}

		template<class U>						WPtr<U>&		WPtr<U>::operator=(const WPtr<U>& p)
		{
			return operator=<U>(p);
		}

		template<class U>template<class V>		WPtr<U>&		WPtr<U>::operator=(const WPtr<V>& p)
		{
			___ctor(p);
			return *this;
		}

		template<class U>template<class V>		WPtr<U>&		WPtr<U>::operator=(const SPtr<V>& p)
		{
			___ctor(p);
			return *this;
		}

		template<class U>						void			WPtr<U>::reset()
		{
			*this = WPtr();
		}

		template<class U>						SPtr<U>			WPtr<U>::lock() const
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			if(expired())
				return SPtr<U>();
			return SPtr<U>(_ref);
		}

		template<class U>						uint32_t		WPtr<U>::use_count() const
		{
#ifdef SPTR_THREAD_SAFE
			scoped_lock<mutex> o(RefBaseImpl::_mx_sptr);
#endif
			return _sc.use_count();
		}

		template<class U>						bool			WPtr<U>::expired() const
		{
			return use_count() == 0;
		}

		template<class U>						U*				WPtr<U>::get_unsafe() const
		{
			Z_ASSERT( valid() );
			return _ref;
		}
		template<class U>						bool			WPtr<U>::valid() const
		{
			return !expired() && _ref;
		}
	};
};

