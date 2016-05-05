
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// see gen_functor.cpp how to generate the following classes

		template<class RET>
		class wfunctor0
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor0();
			// copy constructor
			wfunctor0(const wfunctor0& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor0(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) (  ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor0(THE_CLASS* i, RET (THE_CLASS::*m) (  ));
		public:
			RET			operator() (  ) const;
		public:
			wfunctor0&		operator=(const wfunctor0& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2>
			wfunctor0&		operator=(const wfunctor0<RET_2>& right);
		public:
			bool			operator==(const wfunctor0& rhs) const;
			bool			operator!=(const wfunctor0& rhs) const;
			bool			operator<(const wfunctor0& rhs) const;
		};

		template<class RET, class P0>
		class wfunctor1
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor1();
			// copy constructor
			wfunctor1(const wfunctor1& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor1(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor1(THE_CLASS* i, RET (THE_CLASS::*m) ( P0 ));
		public:
			RET			operator() ( P0 _0 ) const;
		public:
			wfunctor1&		operator=(const wfunctor1& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2>
			wfunctor1&		operator=(const wfunctor1<RET_2, P0_2>& right);
		public:
			bool			operator==(const wfunctor1& rhs) const;
			bool			operator!=(const wfunctor1& rhs) const;
			bool			operator<(const wfunctor1& rhs) const;
		};

		template<class RET, class P0, class P1>
		class wfunctor2
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor2();
			// copy constructor
			wfunctor2(const wfunctor2& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor2(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ));
		public:
			RET			operator() ( P0 _0, P1 _1 ) const;
		public:
			wfunctor2&		operator=(const wfunctor2& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2>
			wfunctor2&		operator=(const wfunctor2<RET_2, P0_2, P1_2>& right);
		public:
			bool			operator==(const wfunctor2& rhs) const;
			bool			operator!=(const wfunctor2& rhs) const;
			bool			operator<(const wfunctor2& rhs) const;
		};

		template<class RET, class P0, class P1, class P2>
		class wfunctor3
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor3();
			// copy constructor
			wfunctor3(const wfunctor3& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor3(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor3(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2 ));
		public:
			RET			operator() ( P0 _0, P1 _1, P2 _2 ) const;
		public:
			wfunctor3&		operator=(const wfunctor3& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2>
			wfunctor3&		operator=(const wfunctor3<RET_2, P0_2, P1_2, P2_2>& right);
		public:
			bool			operator==(const wfunctor3& rhs) const;
			bool			operator!=(const wfunctor3& rhs) const;
			bool			operator<(const wfunctor3& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3>
		class wfunctor4
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor4();
			// copy constructor
			wfunctor4(const wfunctor4& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor4(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor4(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3 ));
		public:
			RET			operator() ( P0 _0, P1 _1, P2 _2, P3 _3 ) const;
		public:
			wfunctor4&		operator=(const wfunctor4& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2>
			wfunctor4&		operator=(const wfunctor4<RET_2, P0_2, P1_2, P2_2, P3_2>& right);
		public:
			bool			operator==(const wfunctor4& rhs) const;
			bool			operator!=(const wfunctor4& rhs) const;
			bool			operator<(const wfunctor4& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4>
		class wfunctor5
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor5();
			// copy constructor
			wfunctor5(const wfunctor5& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor5(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor5(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4 ));
		public:
			RET			operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4 ) const;
		public:
			wfunctor5&		operator=(const wfunctor5& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2>
			wfunctor5&		operator=(const wfunctor5<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2>& right);
		public:
			bool			operator==(const wfunctor5& rhs) const;
			bool			operator!=(const wfunctor5& rhs) const;
			bool			operator<(const wfunctor5& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		class wfunctor6
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor6();
			// copy constructor
			wfunctor6(const wfunctor6& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor6(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor6(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5 ));
		public:
			RET			operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5 ) const;
		public:
			wfunctor6&		operator=(const wfunctor6& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2>
			wfunctor6&		operator=(const wfunctor6<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2>& right);
		public:
			bool			operator==(const wfunctor6& rhs) const;
			bool			operator!=(const wfunctor6& rhs) const;
			bool			operator<(const wfunctor6& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		class wfunctor7
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor7();
			// copy constructor
			wfunctor7(const wfunctor7& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor7(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor7(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6 ));
		public:
			RET			operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6 ) const;
		public:
			wfunctor7&		operator=(const wfunctor7& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2>
			wfunctor7&		operator=(const wfunctor7<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2>& right);
		public:
			bool			operator==(const wfunctor7& rhs) const;
			bool			operator!=(const wfunctor7& rhs) const;
			bool			operator<(const wfunctor7& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		class wfunctor8
		{
		private:
			WPtr<RefBase>	_i;
			struct _C
			{
				void*			p;
				void*			m;
			}				_c;
		public:
			wfunctor8();
			// copy constructor
			wfunctor8(const wfunctor8& other);
			// constructor for method
			template<class THE_CLASS>
			wfunctor8(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6, P7 ));
			// constructor for method
			template<class THE_CLASS>
			wfunctor8(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6, P7 ));
		public:
			RET			operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6, P7 _7 ) const;
		public:
			wfunctor8&		operator=(const wfunctor8& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2, class P7_2>
			wfunctor8&		operator=(const wfunctor8<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2, P7_2>& right);
		public:
			bool			operator==(const wfunctor8& rhs) const;
			bool			operator!=(const wfunctor8& rhs) const;
			bool			operator<(const wfunctor8& rhs) const;
		};
	};
};



