// functor.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <tchar.h>

#include <string.h>

#include <string>
#include <vector>

using namespace std;

#include <stdarg.h>

string	REPLACE(const string& source, const string& what, const string& with)
{
	if(!what.length())
		return source;
	string s;
	int pos = 0;
	while(true)
	{
		int pos2 = (int)source.find(what, pos);
		if(pos2 >= pos)
		{
			s += source.substr(pos, pos2 - pos);
			s += with;
			pos = pos2 + (int)what.length();
			continue;
		}
		break;
	}
	s += source.substr(pos);
	return s;
}

string	FORMATA(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char buffer[8192];
	int ret = _vsnprintf_s(buffer, _countof(buffer), format, ap);
	va_end(ap);

	return buffer;
}

int _tmain(int argc, _TCHAR* argv[])
{
	string sources[6];
	sources[0] = 
"\
		template<class RET, class P0, class P1>\n\
		class functor2\n\
		{\n\
		private:\n\
			void*			_i;\n\
			void*			_f;\n\
			size_t			_is_func;\n\
		public:\n\
			functor2();																	// default constructor\n\
			functor2(const functor2& other);											// copy constructor\n\
			// constructor for function\n\
			functor2(RET (*f) ( P0, P1 ));\n\
			// constructor for method\n\
			template<class THE_CLASS>\n\
			functor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ));\n\
		public:\n\
			RET				operator() ( P0 _0, P1 _1 ) const;\n\
		public:\n\
			functor2&		operator=(const functor2& right);\n\
			void			reset();\n\
		public:\n\
			bool			valid() const;\n\
		private:\n\
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use\n\
			template<class RET_2, class P0_2, class P1_2>\n\
			functor2&		operator=(const functor2<RET_2, P0_2, P1_2>&);\n\
		public:\n\
			bool			operator==(const functor2& rhs) const;\n\
			bool			operator!=(const functor2& rhs) const;\n\
			bool			operator<(const functor2& rhs) const;\n\
		};\n\
";
	sources[1] = 
"\
		template<class RET, class P0, class P1>\n\
		functor2<RET, P0, P1>::functor2()\n\
		{\n\
			memset( this, 0, sizeof(*this) );\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		functor2<RET, P0, P1>::functor2(const functor2& other)\n\
		{\n\
			memcpy( this, &other, sizeof(*this) );\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		functor2<RET, P0, P1>::functor2(RET (*f) ( P0, P1 ))\n\
		{\n\
			_i = NULL;\n\
			_f = *(void**)&f;\n\
			_is_func = true;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		template<class THE_CLASS>\n\
		functor2<RET, P0, P1>::functor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ))\n\
		{\n\
			_i = (void*)i;\n\
			_f = *(void**)&m;\n\
			_is_func = false;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		RET			functor2<RET, P0, P1>::operator() ( P0 _0, P1 _1 ) const\n\
		{\n\
			Z_ASSERT( valid() );\n\
			class C {};\n\
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1 )) == sizeof(_f), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case\n\
			return _is_func ? (*(*(RET (**) ( P0, P1 ))&_f)) ( _0, _1 ) : (((C*)_i)->*(*(RET (C::**) ( P0, P1 ))&_f)) ( _0, _1 );\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		functor2<RET, P0, P1>&\n\
						functor2<RET, P0, P1>::operator=(const functor2& right)\n\
		{\n\
			memcpy( this, &right, sizeof(*this) );\n\
			return *this;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		void			functor2<RET, P0, P1>::reset()			{memset( this, 0, sizeof(*this) );}\n\
		template<class RET, class P0, class P1>\n\
		bool			functor2<RET, P0, P1>::valid() const	{return !!_f;}\n\
		template<class RET, class P0, class P1>\n\
		template<class RET_2, class P0_2, class P1_2>\n\
		functor2<RET, P0, P1>&\n\
						functor2<RET, P0, P1>::operator=(const functor2<RET_2, P0_2, P1_2>&)\n\
		{\n\
			Z_STATIC_ASSERT( false, WRONG_FUNCTOR_ASSIGNMENT );\n\
			return *this;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		bool			functor2<RET, P0, P1>::operator==(const functor2& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) == 0); }\n\
		template<class RET, class P0, class P1>\n\
		bool			functor2<RET, P0, P1>::operator!=(const functor2& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) != 0); }\n\
		template<class RET, class P0, class P1>\n\
		bool			functor2<RET, P0, P1>::operator<(const functor2& rhs) const { return (memcmp(this, &rhs, sizeof(*this)) < 0); }\n\
";
	sources[2] = 
"\
		template<class RET, class P0, class P1>\n\
		class wfunctor2\n\
		{\n\
		private:\n\
			WPtr<RefBase>	_i;\n\
			struct _C\n\
			{\n\
				void*			p;\n\
				void*			m;\n\
			}				_c;\n\
		public:\n\
			wfunctor2();\n\
			// copy constructor\n\
			wfunctor2(const wfunctor2& other);\n\
			// constructor for method\n\
			template<class THE_CLASS>\n\
			wfunctor2(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1 ));\n\
			// constructor for method\n\
			template<class THE_CLASS>\n\
			wfunctor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ));\n\
		public:\n\
			RET			operator() ( P0 _0, P1 _1 ) const;\n\
		public:\n\
			wfunctor2&		operator=(const wfunctor2& right);\n\
			void			reset();\n\
		public:\n\
			bool			valid() const;\n\
		private:\n\
			// although making this private, but the compiler cannot recognize it, so we place a static check inside to catch such illegal use\n\
			template<class RET_2, class P0_2, class P1_2>\n\
			wfunctor2&		operator=(const wfunctor2<RET_2, P0_2, P1_2>& right);\n\
		public:\n\
			bool			operator==(const wfunctor2& rhs) const;\n\
			bool			operator!=(const wfunctor2& rhs) const;\n\
			bool			operator<(const wfunctor2& rhs) const;\n\
		};\n\
";
	sources[3] = 
"\
		template<class RET, class P0, class P1>\n\
		wfunctor2<RET, P0, P1>::wfunctor2()\n\
		{\n\
			_c.p = NULL;\n\
			_c.m = NULL;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		wfunctor2<RET, P0, P1>::wfunctor2(const wfunctor2& other)\n\
		{\n\
			_i = other._i;\n\
			_c = other._c;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		template<class THE_CLASS>\n\
		wfunctor2<RET, P0, P1>::wfunctor2(const SPtr<THE_CLASS>& i, RET (THE_CLASS::*m) ( P0, P1 ))\n\
		{\n\
			Z_ASSERT( i );\n\
			_i = i;\n\
			_c.p = (void*)i.get();\n\
			_c.m = *(void**)&m;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		template<class THE_CLASS>\n\
		wfunctor2<RET, P0, P1>::wfunctor2(THE_CLASS* i, RET (THE_CLASS::*m) ( P0, P1 ))\n\
		{\n\
			Z_ASSERT( i );\n\
			_i = SPtr<THE_CLASS>(i);\n\
			_c.p = (void*)i;\n\
			_c.m = *(void**)&m;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		RET			wfunctor2<RET, P0, P1>::operator() ( P0 _0, P1 _1 ) const\n\
		{\n\
			Z_ASSERT( valid() );\n\
			SPtr<RefBase> deref = _i.lock();\n\
			class C {};															// we must have the class defined first then use its member function type, otherwise crash\n\
			Z_STATIC_ASSERT( sizeof(RET (C::*) ( P0, P1 )) == sizeof(_c.m), WRONG_ASSUMPTION );			// just check if we can secure the single-inheritance case\n\
			return (((C*)_c.p)->*(*(RET (C::**) ( P0, P1 ))&_c.m))( _0, _1 );\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		wfunctor2<RET, P0, P1>&\n\
						wfunctor2<RET, P0, P1>::operator=(const wfunctor2& right)\n\
		{\n\
			_i = right._i;\n\
			_c = right._c;\n\
			return *this;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		void			wfunctor2<RET, P0, P1>::reset()			{_i.reset(); _c.p = NULL; _c.m = NULL;}\n\
		template<class RET, class P0, class P1>\n\
		bool			wfunctor2<RET, P0, P1>::valid() const	{return _i.valid() && _c.m;}\n\
		template<class RET, class P0, class P1>\n\
		template<class RET_2, class P0_2, class P1_2>\n\
		wfunctor2<RET, P0, P1>&\n\
						wfunctor2<RET, P0, P1>::operator=(const wfunctor2<RET_2, P0_2, P1_2>& right)\n\
		{\n\
			Z_STATIC_ASSERT( false, WRONG_WFUNCTOR_ASSIGNMENT );\n\
			return *this;\n\
		}\n\
		template<class RET, class P0, class P1>\n\
		bool			wfunctor2<RET, P0, P1>::operator==(const wfunctor2& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) == 0); }\n\
		template<class RET, class P0, class P1>\n\
		bool			wfunctor2<RET, P0, P1>::operator!=(const wfunctor2& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) != 0); }\n\
		template<class RET, class P0, class P1>\n\
		bool			wfunctor2<RET, P0, P1>::operator<(const wfunctor2& rhs) const { return (memcmp(&_c, &rhs._c, sizeof(_c)) < 0); }\n\
";
	sources[4] = 
"\
		template<class RET, class P0, class P1>\n\
		class functor2;\n\
";
	sources[5] = 
"\
		template<class RET, class P0, class P1>\n\
		class wfunctor2;\n\
";
	for(size_t k = 0; k < 6; ++k)
	{
		for(size_t i = 0; i <= 8; ++i)
		{
			string s = sources[k];

			vector<pair<string, string>> replacements;

			replacements.push_back(pair<string, string>("functor2", FORMATA("functor%d", i)));

			if(true)
			{
				string with;
				for(size_t j = 0; j < i; ++j)
					with += FORMATA(", class P%d_2", j);
				replacements.push_back(pair<string, string>(", class P0_2, class P1_2", with));
			}
			if(true)
			{
				string with;
				for(size_t j = 0; j < i; ++j)
					with += FORMATA(", P%d_2", j);
				replacements.push_back(pair<string, string>(", P0_2, P1_2", with));
			}
			if(true)
			{
				string with;
				for(size_t j = 0; j < i; ++j)
					with += FORMATA(", class P%d", j);
				replacements.push_back(pair<string, string>(", class P0, class P1", with));
			}
			if(true)
			{
				string with;
				for(size_t j = 0; j < i; ++j)
					with += FORMATA(", P%d", j);
				replacements.push_back(pair<string, string>(", P0, P1", with));
			}
			if(true)
			{
				string with;
				for(size_t j = 0; j < i; ++j)
					with += (j != 0 ? ", " : "") + FORMATA("P%d _%d", j, j);
				replacements.push_back(pair<string, string>("P0 _0, P1 _1", with));
			}
			if(true)
			{
				string with;
				for(size_t j = 0; j < i; ++j)
					with += (j != 0 ? ", " : "") + FORMATA("P%d", j);
				replacements.push_back(pair<string, string>("P0, P1", with));
			}

			if(true)
			{
				string with;
				for(size_t j = 0; j < i; ++j)
					with += (j != 0 ? ", " : "") + FORMATA("_%d", j);
				replacements.push_back(pair<string, string>("_0, _1", with));
			}

			for(size_t start = 0; start < s.length(); )
			{
				size_t min_pos = -1;
				size_t min_index = -1;
				for(size_t l = 0; l < replacements.size(); ++l)
				{
					size_t p;
					if((p = s.find(replacements[l].first, start)) != string::npos)
					{
						if(p < min_pos)
						{
							min_pos = p;
							min_index = l;
						}
					}
				}
				if(min_index == -1)
					break;

				s.erase(min_pos, replacements[min_index].first.length());
				s.insert(min_pos, replacements[min_index].second);
				start = min_pos + replacements[min_index].second.length();
			}

			puts(s.c_str());
		}
	}

	return 0;
}

