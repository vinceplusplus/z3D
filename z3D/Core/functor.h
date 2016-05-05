
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// see gen_functor.cpp how to generate the following classes

		template<class RET>
		class functor0
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor0();																	// default constructor
			functor0(const functor0& other);											// copy constructor
			// constructor for function
			functor0(RET (*f) (  ));
			// constructor for method
			template<class THE_CLASS>
			functor0(THE_CLASS* i, RET (THE_CLASS::*m) (  ));
		public:
			RET				operator() (  ) const;
		public:
			functor0&		operator=(const functor0& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2>
			functor0&		operator=(const functor0<RET_2>&);
		public:
			bool			operator==(const functor0& rhs) const;
			bool			operator!=(const functor0& rhs) const;
			bool			operator<(const functor0& rhs) const;
		};

		template<class RET, class P0>
		class functor1
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor1();																	// default constructor
			functor1(const functor1& other);											// copy constructor
			// constructor for function
			functor1(RET (*f) ( P0 ));
			// constructor for method
			template<class THE_CLASS>
			functor1(THE_CLASS* i, RET (THE_CLASS::*m) ( P0 ));
		public:
			RET				operator() ( P0 _0 ) const;
		public:
			functor1&		operator=(const functor1& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2>
			functor1&		operator=(const functor1<RET_2, P0_2>&);
		public:
			bool			operator==(const functor1& rhs) const;
			bool			operator!=(const functor1& rhs) const;
			bool			operator<(const functor1& rhs) const;
		};

		template<class RET, class P0, class P1>
		class functor2
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor2();																	// default constructor
			functor2(const functor2& other);											// copy constructor
			// constructor for function
			functor2(RET (*f) ( P0, P1 ));
			// constructor for method
			template<class THE_CLASS>
			functor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ));
		public:
			RET				operator() ( P0 _0, P1 _1 ) const;
		public:
			functor2&		operator=(const functor2& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2>
			functor2&		operator=(const functor2<RET_2, P0_2, P1_2>&);
		public:
			bool			operator==(const functor2& rhs) const;
			bool			operator!=(const functor2& rhs) const;
			bool			operator<(const functor2& rhs) const;
		};

		template<class RET, class P0, class P1, class P2>
		class functor3
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor3();																	// default constructor
			functor3(const functor3& other);											// copy constructor
			// constructor for function
			functor3(RET (*f) ( P0, P1, P2 ));
			// constructor for method
			template<class THE_CLASS>
			functor3(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2 ));
		public:
			RET				operator() ( P0 _0, P1 _1, P2 _2 ) const;
		public:
			functor3&		operator=(const functor3& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2>
			functor3&		operator=(const functor3<RET_2, P0_2, P1_2, P2_2>&);
		public:
			bool			operator==(const functor3& rhs) const;
			bool			operator!=(const functor3& rhs) const;
			bool			operator<(const functor3& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3>
		class functor4
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor4();																	// default constructor
			functor4(const functor4& other);											// copy constructor
			// constructor for function
			functor4(RET (*f) ( P0, P1, P2, P3 ));
			// constructor for method
			template<class THE_CLASS>
			functor4(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3 ));
		public:
			RET				operator() ( P0 _0, P1 _1, P2 _2, P3 _3 ) const;
		public:
			functor4&		operator=(const functor4& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2>
			functor4&		operator=(const functor4<RET_2, P0_2, P1_2, P2_2, P3_2>&);
		public:
			bool			operator==(const functor4& rhs) const;
			bool			operator!=(const functor4& rhs) const;
			bool			operator<(const functor4& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4>
		class functor5
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor5();																	// default constructor
			functor5(const functor5& other);											// copy constructor
			// constructor for function
			functor5(RET (*f) ( P0, P1, P2, P3, P4 ));
			// constructor for method
			template<class THE_CLASS>
			functor5(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4 ));
		public:
			RET				operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4 ) const;
		public:
			functor5&		operator=(const functor5& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2>
			functor5&		operator=(const functor5<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2>&);
		public:
			bool			operator==(const functor5& rhs) const;
			bool			operator!=(const functor5& rhs) const;
			bool			operator<(const functor5& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5>
		class functor6
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor6();																	// default constructor
			functor6(const functor6& other);											// copy constructor
			// constructor for function
			functor6(RET (*f) ( P0, P1, P2, P3, P4, P5 ));
			// constructor for method
			template<class THE_CLASS>
			functor6(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5 ));
		public:
			RET				operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5 ) const;
		public:
			functor6&		operator=(const functor6& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2>
			functor6&		operator=(const functor6<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2>&);
		public:
			bool			operator==(const functor6& rhs) const;
			bool			operator!=(const functor6& rhs) const;
			bool			operator<(const functor6& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
		class functor7
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor7();																	// default constructor
			functor7(const functor7& other);											// copy constructor
			// constructor for function
			functor7(RET (*f) ( P0, P1, P2, P3, P4, P5, P6 ));
			// constructor for method
			template<class THE_CLASS>
			functor7(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6 ));
		public:
			RET				operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6 ) const;
		public:
			functor7&		operator=(const functor7& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2>
			functor7&		operator=(const functor7<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2>&);
		public:
			bool			operator==(const functor7& rhs) const;
			bool			operator!=(const functor7& rhs) const;
			bool			operator<(const functor7& rhs) const;
		};

		template<class RET, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		class functor8
		{
		private:
			void*			_i;
			void*			_f;
			size_t			_is_func;
		public:
			functor8();																	// default constructor
			functor8(const functor8& other);											// copy constructor
			// constructor for function
			functor8(RET (*f) ( P0, P1, P2, P3, P4, P5, P6, P7 ));
			// constructor for method
			template<class THE_CLASS>
			functor8(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1, P2, P3, P4, P5, P6, P7 ));
		public:
			RET				operator() ( P0 _0, P1 _1, P2 _2, P3 _3, P4 _4, P5 _5, P6 _6, P7 _7 ) const;
		public:
			functor8&		operator=(const functor8& right);
			void			reset();
		public:
			bool			valid() const;
		private:
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use
			template<class RET_2, class P0_2, class P1_2, class P2_2, class P3_2, class P4_2, class P5_2, class P6_2, class P7_2>
			functor8&		operator=(const functor8<RET_2, P0_2, P1_2, P2_2, P3_2, P4_2, P5_2, P6_2, P7_2>&);
		public:
			bool			operator==(const functor8& rhs) const;
			bool			operator!=(const functor8& rhs) const;
			bool			operator<(const functor8& rhs) const;
		};
	};
};



