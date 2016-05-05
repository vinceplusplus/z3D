
#include "LexScan.h"

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer scnFlexLexer
#include "FlexLexer.h"

#include "Misc.h"

namespace z3D
{
	namespace Core
	{
		LexScan::LexScan(const mbs& str_utf8):_src_str_utf8(str_utf8),_src_stream(_src_str_utf8.c_str(),(streamsize)_src_str_utf8.length())
		{
			_flex_scanner=(void*)new scnFlexLexer(&_src_stream,NULL);
		}
		LexScan::~LexScan()
		{
			delete (scnFlexLexer*)_flex_scanner;
		}
		int				LexScan::scan()
		{
			return ((scnFlexLexer*)_flex_scanner)->yylex();
		}
		const wcs&		LexScan::getIdentifier() const
		{
			return ((scnFlexLexer*)_flex_scanner)->get_identifier_string();
		}
		const wcs&		LexScan::getValueWString() const
		{
			return ((scnFlexLexer*)_flex_scanner)->get_value_string();
		}
		__int64			LexScan::getValueInt64() const
		{
			return ((scnFlexLexer*)_flex_scanner)->get_value_int64();
		}
		double			LexScan::getValueDouble() const
		{
			return ((scnFlexLexer*)_flex_scanner)->get_value_double();
		}
		wcs				LexScan::getScanLex() const
		{
			return MBSTOWCS(((scnFlexLexer*)_flex_scanner)->YYText());
		}
	};
};

