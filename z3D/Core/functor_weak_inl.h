
#pragma once

#include "functor_weak.h"

namespace z3D
{
	namespace Core
	{
		template<class RET>
		wfunctor0<RET>::wfunctor0()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET>
		wfunctor0<RET>::wfunctor0(const wfunctor0& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET>
		template<class THE_CLASS>
		wfunctor0<RET>::wfunctor0(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) (  ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET>
		template<class THE_CLASS>
		wfunctor0<RET>::wfunctor0(THE_CLASS* i, RET (THE_CLASS::*m) (  ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET>
		RET			wfunctor0<RET>::operator() (  ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) (  )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) (  ))&_c.m))(  );
		}
		template<class RET>
		wfunctor0<RET>&
						wfunctor0<RET>::operator=(const wfunctor0& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET>
		void			wfunctor0<RET>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET>
		bool			wfunctor0<RET>::valid() const	{return _i.valid() && _c.m;}
		template<class RET>
		template<class RET_2>
		wfunctor0<RET>&
						wfunctor0<RET>::operator=(const wfunctor0<RET_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET>
		bool			wfunctor0<RET>::operator==(const wfunctor0& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET>
		bool			wfunctor0<RET>::operator!=(const wfunctor0& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET>
		bool			wfunctor0<RET>::operator<(const wfunctor0& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0>
		wfunctor1<RET, P0>::wfunctor1()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0>
		wfunctor1<RET, P0>::wfunctor1(const wfunctor1& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0>
		template<class THE_CLASS>
		wfunctor1<RET, P0>::wfunctor1(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0>
		template<class THE_CLASS>
		wfunctor1<RET, P0>::wfunctor1(THE_CLASS* i, RET (THE_CLASS::*m) ( P0 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0>
		RET			wfunctor1<RET, P0>::operator() ( P0 _0 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0 ))&_c.m))( _0 );
		}
		template<class RET, class P0>
		wfunctor1<RET, P0>&
						wfunctor1<RET, P0>::operator=(const wfunctor1& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0>
		void			wfunctor1<RET, P0>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0>
		bool			wfunctor1<RET, P0>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0>
		template<class RET_2, class P0_2>
		wfunctor1<RET, P0>&
						wfunctor1<RET, P0>::operator=(const wfunctor1<RET_2, P0_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0>
		bool			wfunctor1<RET, P0>::operator==(const wfunctor1& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0>
		bool			wfunctor1<RET, P0>::operator!=(const wfunctor1& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0>
		bool			wfunctor1<RET, P0>::operator<(const wfunctor1& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0, class P1>
		wfunctor2<RET, P0, P1>::wfunctor2()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0, class P1>
		wfunctor2<RET, P0, P1>::wfunctor2(const wfunctor2& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0, class P1>
		template<class THE_CLASS>
		wfunctor2<RET, P0, P1>::wfunctor2(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1>
		template<class THE_CLASS>
		wfunctor2<RET, P0, P1>::wfunctor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1>
		RET			wfunctor2<RET, P0, P1>::operator() ( P0 _0, P1 _1 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1 ))&_c.m))( _0, _1 );
		}
		template<class RET, class P0, class P1>
		wfunctor2<RET, P0, P1>&
						wfunctor2<RET, P0, P1>::operator=(const wfunctor2& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0, class P1>
		void			wfunctor2<RET, P0, P1>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0, class P1>
		bool			wfunctor2<RET, P0, P1>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0, class P1>
		template<class RET_2, class P0_2, class P1_2>
		wfunctor2<RET, P0, P1>&
						wfunctor2<RET, P0, P1>::operator=(const wfunctor2<RET_2, P0_2, P1_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1>
		bool			wfunctor2<RET, P0, P1>::operator==(const wfunctor2& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0, class P1>
		bool			wfunctor2<RET, P0, P1>::operator!=(const wfunctor2& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0, class P1>
		bool			wfunctor2<RET, P0, P1>::operator<(const wfunctor2& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0, class P1, class P2>
		wfunctor3<RET, P0, P1, P2>::wfunctor3()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0, class P1, class P2>
		wfunctor3<RET, P0, P1, P2>::wfunctor3(const wfunctor3& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0, class P1, class P2>
		template<class THE_CLASS>
		wfunctor3<RET, P0, P1, P2>::wfunctor3(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2>
		template<class THE_CLASS>
		wfunctor3<RET, P0, P1, P2>::wfunctor3(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2>
		RET			wfunctor3<RET, P0, P1, P2>::operator() ( P0 _0, P1 _1, P2 _2 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1, P2 ))&_c.m))( _0, _1, _2 );
		}
		template<class RET, class P0, class P1, class P2>
		wfunctor3<RET, P0, P1, P2>&
						wfunctor3<RET, P0, P1, P2>::operator=(const wfunctor3& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0, class P1, class P2>
		void			wfunctor3<RET, P0, P1, P2>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0, class P1, class P2>
		bool			wfunctor3<RET, P0, P1, P2>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0, class P1, class P2>
		template<class RET_2, class P0_2, class P1_2, class P2_2>
		wfunctor3<RET, P0, P1, P2>&
						wfunctor3<RET, P0, P1, P2>::operator=(const wfunctor3<RET_2, P0_2, P1_2, P2_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2>
		bool			wfunctor3<RET, P0, P1, P2>::operator==(const wfunctor3& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0, class P1, class P2>
		bool			wfunctor3<RET, P0, P1, P2>::operator!=(const wfunctor3& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0, class P1, class P2>
		bool			wfunctor3<RET, P0, P1, P2>::operator<(const wfunctor3& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3>
		wfunctor4<RET, P0, P1, P2, P3>::wfunctor4()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		wfunctor4<RET, P0, P1, P2, P3>::wfunctor4(const wfunctor4& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		template<class THE_CLASS>
		wfunctor4<RET, P0, P1, P2, P3>::wfunctor4(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		template<class THE_CLASS>
		wfunctor4<RET, P0, P1, P2, P3>::wfunctor4(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		RET			wfunctor4<RET, P0, P1, P2, P3>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1, P2, P3 ))&_c.m))( _0, _1, _2, _3 );
		}
		template<class RET, class P0, class P1, class P2, class P3>
		wfunctor4<RET, P0, P1, P2, P3>&
						wfunctor4<RET, P0, P1, P2, P3>::operator=(const wfunctor4& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		void			wfunctor4<RET, P0, P1, P2, P3>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0, class P1, class P2, class P3>
		bool			wfunctor4<RET, P0, P1, P2, P3>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0, class P1, class P2, class P3>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2>
		wfunctor4<RET, P0, P1, P2, P3>&
						wfunctor4<RET, P0, P1, P2, P3>::operator=(const wfunctor4<RET_2, P0_2, P1_2, P2_2, P3_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		bool			wfunctor4<RET, P0, P1, P2, P3>::operator==(const wfunctor4& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3>
		bool			wfunctor4<RET, P0, P1, P2, P3>::operator!=(const wfunctor4& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3>
		bool			wfunctor4<RET, P0, P1, P2, P3>::operator<(const wfunctor4& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4>
		wfunctor5<RET, P0, P1, P2, P3, P4>::wfunctor5()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		wfunctor5<RET, P0, P1, P2, P3, P4>::wfunctor5(const wfunctor5& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		template<class THE_CLASS>
		wfunctor5<RET, P0, P1, P2, P3, P4>::wfunctor5(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		template<class THE_CLASS>
		wfunctor5<RET, P0, P1, P2, P3, P4>::wfunctor5(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		RET			wfunctor5<RET, P0, P1, P2, P3, P4>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1, P2, P3, P4 ))&_c.m))( _0, _1, _2, _3, _4 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		wfunctor5<RET, P0, P1, P2, P3, P4>&
						wfunctor5<RET, P0, P1, P2, P3, P4>::operator=(const wfunctor5& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		void			wfunctor5<RET, P0, P1, P2, P3, P4>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			wfunctor5<RET, P0, P1, P2, P3, P4>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2>
		wfunctor5<RET, P0, P1, P2, P3, P4>&
						wfunctor5<RET, P0, P1, P2, P3, P4>::operator=(const wfunctor5<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			wfunctor5<RET, P0, P1, P2, P3, P4>::operator==(const wfunctor5& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			wfunctor5<RET, P0, P1, P2, P3, P4>::operator!=(const wfunctor5& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			wfunctor5<RET, P0, P1, P2, P3, P4>::operator<(const wfunctor5& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		wfunctor6<RET, P0, P1, P2, P3, P4, P5>::wfunctor6()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		wfunctor6<RET, P0, P1, P2, P3, P4, P5>::wfunctor6(const wfunctor6& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		template<class THE_CLASS>
		wfunctor6<RET, P0, P1, P2, P3, P4, P5>::wfunctor6(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		template<class THE_CLASS>
		wfunctor6<RET, P0, P1, P2, P3, P4, P5>::wfunctor6(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		RET			wfunctor6<RET, P0, P1, P2, P3, P4, P5>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4, P5 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1, P2, P3, P4, P5 ))&_c.m))( _0, _1, _2, _3, _4, _5 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		wfunctor6<RET, P0, P1, P2, P3, P4, P5>&
						wfunctor6<RET, P0, P1, P2, P3, P4, P5>::operator=(const wfunctor6& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		void			wfunctor6<RET, P0, P1, P2, P3, P4, P5>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			wfunctor6<RET, P0, P1, P2, P3, P4, P5>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2>
		wfunctor6<RET, P0, P1, P2, P3, P4, P5>&
						wfunctor6<RET, P0, P1, P2, P3, P4, P5>::operator=(const wfunctor6<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			wfunctor6<RET, P0, P1, P2, P3, P4, P5>::operator==(const wfunctor6& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			wfunctor6<RET, P0, P1, P2, P3, P4, P5>::operator!=(const wfunctor6& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			wfunctor6<RET, P0, P1, P2, P3, P4, P5>::operator<(const wfunctor6& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::wfunctor7()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::wfunctor7(const wfunctor7& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		template<class THE_CLASS>
		wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::wfunctor7(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		template<class THE_CLASS>
		wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::wfunctor7(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		RET			wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4, P5, P6 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1, P2, P3, P4, P5, P6 ))&_c.m))( _0, _1, _2, _3, _4, _5, _6 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>&
						wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator=(const wfunctor7& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		void			wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2>
		wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>&
						wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator=(const wfunctor7<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator==(const wfunctor7& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator!=(const wfunctor7& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			wfunctor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator<(const wfunctor7& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::wfunctor8()
		{
			_c.p = NULL;
			_c.m = NULL;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::wfunctor8(const wfunctor8& other)
		{
			_i = other._i;
			_c = other._c;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		template<class THE_CLASS>
		wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::wfunctor8(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6, P7 ))
		{
			Z_ASSERT( i );
			_i = i;
			_c.p = (void*)i.get();
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		template<class THE_CLASS>
		wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::wfunctor8(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6, P7 ))
		{
			Z_ASSERT( i );
			_i = SPtr<THE_CLASS>(i);
			_c.p = (void*)i;
			_c.m = *(void**)&m;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		RET			wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6, P7 _7 ) const
		{
			Z_ASSERT( valid() );
			SPtr<RefBase> deref = _i.lock();
			class C {};															// we must have the class defined first then use its member function type, otherwise crash
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4, P5, P6, P7 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1, P2, P3, P4, P5, P6, P7 ))&_c.m))( _0, _1, _2, _3, _4, _5, _6, _7 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>&
						wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator=(const wfunctor8& right)
		{
			_i = right._i;
			_c = right._c;
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		void			wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::valid() const	{return _i.valid() && _c.m;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2, class P7_2>
		wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>&
						wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator=(const wfunctor8<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2, P7_2>& right)
		{
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator==(const wfunctor8& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator!=(const wfunctor8& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			wfunctor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator<(const wfunctor8& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }
	};
};



