
#pragma once

#include "functor.h"

namespace z3D
{
	namespace Core
	{
		template<class RET>
		functor0<RET>::functor0()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET>
		functor0<RET>::functor0(const functor0& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET>
		functor0<RET>::functor0(RET (*f) (  ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET>
		template<class THE_CLASS>
		functor0<RET>::functor0(THE_CLASS* i, RET (THE_CLASS::*m) (  ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET>
		RET			functor0<RET>::operator() (  ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) (  )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) (  ))&_f)) (  ) : (((C*)_i)->*(*(RET (C::**) (  ))&_f)) (  );
		}
		template<class RET>
		functor0<RET>&
						functor0<RET>::operator=(const functor0& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET>
		void			functor0<RET>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET>
		bool			functor0<RET>::valid() const	{return !!_f;}
		template<class RET>
		template<class RET_2>
		functor0<RET>&
						functor0<RET>::operator=(const functor0<RET_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET>
		bool			functor0<RET>::operator==(const functor0& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET>
		bool			functor0<RET>::operator!=(const functor0& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET>
		bool			functor0<RET>::operator<(const functor0& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0>
		functor1<RET, P0>::functor1()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0>
		functor1<RET, P0>::functor1(const functor1& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0>
		functor1<RET, P0>::functor1(RET (*f) ( P0 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0>
		template<class THE_CLASS>
		functor1<RET, P0>::functor1(THE_CLASS* i, RET (THE_CLASS::*m) ( P0 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0>
		RET			functor1<RET, P0>::operator() ( P0 _0 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0 ))&_f)) ( _0 ) : (((C*)_i)->*(*(RET (C::**) ( P0 ))&_f)) ( _0 );
		}
		template<class RET, class P0>
		functor1<RET, P0>&
						functor1<RET, P0>::operator=(const functor1& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0>
		void			functor1<RET, P0>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0>
		bool			functor1<RET, P0>::valid() const	{return !!_f;}
		template<class RET, class P0>
		template<class RET_2, class P0_2>
		functor1<RET, P0>&
						functor1<RET, P0>::operator=(const functor1<RET_2, P0_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0>
		bool			functor1<RET, P0>::operator==(const functor1& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0>
		bool			functor1<RET, P0>::operator!=(const functor1& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0>
		bool			functor1<RET, P0>::operator<(const functor1& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0, class P1>
		functor2<RET, P0, P1>::functor2()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0, class P1>
		functor2<RET, P0, P1>::functor2(const functor2& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0, class P1>
		functor2<RET, P0, P1>::functor2(RET (*f) ( P0, P1 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0, class P1>
		template<class THE_CLASS>
		functor2<RET, P0, P1>::functor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0, class P1>
		RET			functor2<RET, P0, P1>::operator() ( P0 _0, P1 _1 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0, P1 ))&_f)) ( _0, _1 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1 ))&_f)) ( _0, _1 );
		}
		template<class RET, class P0, class P1>
		functor2<RET, P0, P1>&
						functor2<RET, P0, P1>::operator=(const functor2& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0, class P1>
		void			functor2<RET, P0, P1>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0, class P1>
		bool			functor2<RET, P0, P1>::valid() const	{return !!_f;}
		template<class RET, class P0, class P1>
		template<class RET_2, class P0_2, class P1_2>
		functor2<RET, P0, P1>&
						functor2<RET, P0, P1>::operator=(const functor2<RET_2, P0_2, P1_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1>
		bool			functor2<RET, P0, P1>::operator==(const functor2& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0, class P1>
		bool			functor2<RET, P0, P1>::operator!=(const functor2& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0, class P1>
		bool			functor2<RET, P0, P1>::operator<(const functor2& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0, class P1, class P2>
		functor3<RET, P0, P1, P2>::functor3()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2>
		functor3<RET, P0, P1, P2>::functor3(const functor3& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2>
		functor3<RET, P0, P1, P2>::functor3(RET (*f) ( P0, P1, P2 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0, class P1, class P2>
		template<class THE_CLASS>
		functor3<RET, P0, P1, P2>::functor3(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0, class P1, class P2>
		RET			functor3<RET, P0, P1, P2>::operator() ( P0 _0, P1 _1, P2 _2 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0, P1, P2 ))&_f)) ( _0, _1, _2 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1, P2 ))&_f)) ( _0, _1, _2 );
		}
		template<class RET, class P0, class P1, class P2>
		functor3<RET, P0, P1, P2>&
						functor3<RET, P0, P1, P2>::operator=(const functor3& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0, class P1, class P2>
		void			functor3<RET, P0, P1, P2>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0, class P1, class P2>
		bool			functor3<RET, P0, P1, P2>::valid() const	{return !!_f;}
		template<class RET, class P0, class P1, class P2>
		template<class RET_2, class P0_2, class P1_2, class P2_2>
		functor3<RET, P0, P1, P2>&
						functor3<RET, P0, P1, P2>::operator=(const functor3<RET_2, P0_2, P1_2, P2_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2>
		bool			functor3<RET, P0, P1, P2>::operator==(const functor3& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0, class P1, class P2>
		bool			functor3<RET, P0, P1, P2>::operator!=(const functor3& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0, class P1, class P2>
		bool			functor3<RET, P0, P1, P2>::operator<(const functor3& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3>
		functor4<RET, P0, P1, P2, P3>::functor4()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3>
		functor4<RET, P0, P1, P2, P3>::functor4(const functor4& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3>
		functor4<RET, P0, P1, P2, P3>::functor4(RET (*f) ( P0, P1, P2, P3 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		template<class THE_CLASS>
		functor4<RET, P0, P1, P2, P3>::functor4(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		RET			functor4<RET, P0, P1, P2, P3>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0, P1, P2, P3 ))&_f)) ( _0, _1, _2, _3 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1, P2, P3 ))&_f)) ( _0, _1, _2, _3 );
		}
		template<class RET, class P0, class P1, class P2, class P3>
		functor4<RET, P0, P1, P2, P3>&
						functor4<RET, P0, P1, P2, P3>::operator=(const functor4& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		void			functor4<RET, P0, P1, P2, P3>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0, class P1, class P2, class P3>
		bool			functor4<RET, P0, P1, P2, P3>::valid() const	{return !!_f;}
		template<class RET, class P0, class P1, class P2, class P3>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2>
		functor4<RET, P0, P1, P2, P3>&
						functor4<RET, P0, P1, P2, P3>::operator=(const functor4<RET_2, P0_2, P1_2, P2_2, P3_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3>
		bool			functor4<RET, P0, P1, P2, P3>::operator==(const functor4& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3>
		bool			functor4<RET, P0, P1, P2, P3>::operator!=(const functor4& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3>
		bool			functor4<RET, P0, P1, P2, P3>::operator<(const functor4& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4>
		functor5<RET, P0, P1, P2, P3, P4>::functor5()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		functor5<RET, P0, P1, P2, P3, P4>::functor5(const functor5& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		functor5<RET, P0, P1, P2, P3, P4>::functor5(RET (*f) ( P0, P1, P2, P3, P4 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		template<class THE_CLASS>
		functor5<RET, P0, P1, P2, P3, P4>::functor5(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		RET			functor5<RET, P0, P1, P2, P3, P4>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0, P1, P2, P3, P4 ))&_f)) ( _0, _1, _2, _3, _4 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1, P2, P3, P4 ))&_f)) ( _0, _1, _2, _3, _4 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		functor5<RET, P0, P1, P2, P3, P4>&
						functor5<RET, P0, P1, P2, P3, P4>::operator=(const functor5& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		void			functor5<RET, P0, P1, P2, P3, P4>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			functor5<RET, P0, P1, P2, P3, P4>::valid() const	{return !!_f;}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2>
		functor5<RET, P0, P1, P2, P3, P4>&
						functor5<RET, P0, P1, P2, P3, P4>::operator=(const functor5<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			functor5<RET, P0, P1, P2, P3, P4>::operator==(const functor5& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			functor5<RET, P0, P1, P2, P3, P4>::operator!=(const functor5& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4>
		bool			functor5<RET, P0, P1, P2, P3, P4>::operator<(const functor5& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		functor6<RET, P0, P1, P2, P3, P4, P5>::functor6()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		functor6<RET, P0, P1, P2, P3, P4, P5>::functor6(const functor6& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		functor6<RET, P0, P1, P2, P3, P4, P5>::functor6(RET (*f) ( P0, P1, P2, P3, P4, P5 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		template<class THE_CLASS>
		functor6<RET, P0, P1, P2, P3, P4, P5>::functor6(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		RET			functor6<RET, P0, P1, P2, P3, P4, P5>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4, P5 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0, P1, P2, P3, P4, P5 ))&_f)) ( _0, _1, _2, _3, _4, _5 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1, P2, P3, P4, P5 ))&_f)) ( _0, _1, _2, _3, _4, _5 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		functor6<RET, P0, P1, P2, P3, P4, P5>&
						functor6<RET, P0, P1, P2, P3, P4, P5>::operator=(const functor6& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		void			functor6<RET, P0, P1, P2, P3, P4, P5>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			functor6<RET, P0, P1, P2, P3, P4, P5>::valid() const	{return !!_f;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2>
		functor6<RET, P0, P1, P2, P3, P4, P5>&
						functor6<RET, P0, P1, P2, P3, P4, P5>::operator=(const functor6<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			functor6<RET, P0, P1, P2, P3, P4, P5>::operator==(const functor6& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			functor6<RET, P0, P1, P2, P3, P4, P5>::operator!=(const functor6& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		bool			functor6<RET, P0, P1, P2, P3, P4, P5>::operator<(const functor6& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		functor7<RET, P0, P1, P2, P3, P4, P5, P6>::functor7()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		functor7<RET, P0, P1, P2, P3, P4, P5, P6>::functor7(const functor7& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		functor7<RET, P0, P1, P2, P3, P4, P5, P6>::functor7(RET (*f) ( P0, P1, P2, P3, P4, P5, P6 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		template<class THE_CLASS>
		functor7<RET, P0, P1, P2, P3, P4, P5, P6>::functor7(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		RET			functor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4, P5, P6 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0, P1, P2, P3, P4, P5, P6 ))&_f)) ( _0, _1, _2, _3, _4, _5, _6 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1, P2, P3, P4, P5, P6 ))&_f)) ( _0, _1, _2, _3, _4, _5, _6 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		functor7<RET, P0, P1, P2, P3, P4, P5, P6>&
						functor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator=(const functor7& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		void			functor7<RET, P0, P1, P2, P3, P4, P5, P6>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			functor7<RET, P0, P1, P2, P3, P4, P5, P6>::valid() const	{return !!_f;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2>
		functor7<RET, P0, P1, P2, P3, P4, P5, P6>&
						functor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator=(const functor7<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			functor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator==(const functor7& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			functor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator!=(const functor7& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		bool			functor7<RET, P0, P1, P2, P3, P4, P5, P6>::operator<(const functor7& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::functor8()
		{
			memset( this, 0, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::functor8(const functor8& other)
		{
			memcpy( this, &other, sizeof(*this) );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::functor8(RET (*f) ( P0, P1, P2, P3, P4, P5, P6, P7 ))
		{
			_i = NULL;
			_f = *(void**)&f;
			_is_func = true;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		template<class THE_CLASS>
		functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::functor8(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6, P7 ))
		{
			_i = (void*)i;
			_f = *(void**)&m;
			_is_func = false;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		RET			functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6, P7 _7 ) const
		{
			Z_ASSERT( valid() );
			class C {};
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1, P2, P3, P4, P5, P6, P7 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case
			return _is_func ? (*(*(RET (**) ( P0, P1, P2, P3, P4, P5, P6, P7 ))&_f)) ( _0, _1, _2, _3, _4, _5, _6, _7 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1, P2, P3, P4, P5, P6, P7 ))&_f)) ( _0, _1, _2, _3, _4, _5, _6, _7 );
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>&
						functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator=(const functor8& right)
		{
			memcpy( this, &right, sizeof(*this) );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		void			functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::reset()			{memset( this, 0, sizeof(*this) );}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::valid() const	{return !!_f;}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2, class P7_2>
		functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>&
						functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator=(const functor8<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2, P7_2>&)
		{
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );
			return *this;
		}
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator==(const functor8& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator!=(const functor8& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }
		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		bool			functor8<RET, P0, P1, P2, P3, P4, P5, P6, P7>::operator<(const functor8& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }
	};
};



