
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "Misc.h"

#include "mutex_fast.h"
#include "scoped_lock.h"

#include <stdarg.h>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		extern mutex_fast			mx_formata;
		extern mutex_fast			mx_formatw;
		extern vector<char>			formata_buffer;
		extern vector<wchar_t>		formatw_buffer;

		mbs		FORMATA(const char* format, ...)
		{
			scoped_lock<mutex_fast> o(mx_formata);

			va_list ap;
			va_start(ap,format);
			int ret = _vsnprintf(&formata_buffer[0], formata_buffer.size(), format,ap);
			va_end(ap);

			if(ret >= 0)
				return &formata_buffer[0];
			va_start(ap, format);
			ret = _vscprintf(format, ap);
			va_end(ap);
			formata_buffer.resize(ret + 1);
			va_start(ap, format);
			_vsnprintf(&formata_buffer[0], formata_buffer.size(), format, ap);
			va_end(ap);
			return &formata_buffer[0];
		}
		wcs		FORMATW(const wchar_t* format, ...)
		{
			scoped_lock<mutex_fast> o(mx_formatw);

			va_list ap;
			va_start(ap, format);
			int ret = _vsnwprintf(&formatw_buffer[0], formatw_buffer.size(), format, ap);
			va_end(ap);

			if(ret >= 0)
				return &formatw_buffer[0];
			va_start(ap, format);
			ret = _vscwprintf(format, ap);
			va_end(ap);
			formatw_buffer.resize(ret + 1);
			va_start(ap, format);
			_vsnwprintf(&formatw_buffer[0], formatw_buffer.size(), format, ap);
			va_end(ap);
			return &formatw_buffer[0];
		}
		wcs		MBSTOWCS(const mbs& s)
		{
			return MBSTOWCS(s, CP_ACP);
		}
		mbs		WCSTOMBS(const wcs& s)
		{
			return WCSTOMBS(s, CP_ACP);
		}
		wcs		MBSTOWCS(const mbs& s, uint32_t codepage)
		{
			int len = MultiByteToWideChar(codepage, 0, s.c_str(), -1, NULL, 0);
			if(len == 0)
				return wcs(L"");
			wchar_t stack_buffer[1024];
			wchar_t* heap_buffer = NULL;
			wchar_t* buffer;
			if(len > _countof(stack_buffer))
				buffer = heap_buffer = new wchar_t[len];
			else
				buffer = stack_buffer;
			MultiByteToWideChar(codepage, 0, s.c_str(), -1, buffer, len);
			wcs str(buffer);
			if(heap_buffer)
				delete[] heap_buffer;
			return str;
		}
		mbs		WCSTOMBS(const wcs& s, uint32_t codepage)
		{
			int len = WideCharToMultiByte(codepage, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
			if(len == 0)
				return mbs("");
			char stack_buffer[1024];
			char* heap_buffer = NULL;
			char* buffer;
			if(len > _countof(stack_buffer))
				buffer = heap_buffer = new char[len];
			else
				buffer = stack_buffer;
			WideCharToMultiByte(codepage, 0, s.c_str(), -1, buffer, len, NULL, NULL);
			mbs str(buffer);
			if(heap_buffer)
				delete[] heap_buffer;
			return str;
		}
		wcs		UTF8TOWCS(const mbs& s)
		{
			return MBSTOWCS(s, CP_UTF8);
		}
		mbs		WCSTOUTF8(const wcs& s)
		{
			return WCSTOMBS(s, CP_UTF8);
		}
		wcs		ESCSTR(const wcs& s)
		{
			wcs dest;
			for(size_t i = 0; i < s.length(); ++i)
			{
				if(s[i] == '\\')
					dest += L"\\\\";
				else if(s[i] == '"')
					dest += L"\\\"";
				else if(s[i] == '\'')
					dest += L"\\\'";
				else
					dest += s[i];
			}
			return dest;
		}
		mbs	REPLACE(const mbs& source,const mbs& what,const mbs& with)
		{
			if(!what.length())
				return source;
			mbs s;
			mbs::size_type pos = 0;
			while(true)
			{
				mbs::size_type pos2 = source.find(what, pos);
				if(pos2 == mbs::npos)
					break;

				s += source.substr(pos, pos2 - pos);
				s += with;
				pos = pos2 + what.length();
			}
			s += source.substr(pos);
			return s;
		}
		wcs		REPLACE(const wcs& source,const wcs& what,const wcs& with)
		{
			if(!what.length())
				return source;
			wcs s;
			wcs::size_type pos = 0;
			while(true)
			{
				wcs::size_type pos2 = source.find(what, pos);
				if(pos2 == wcs::npos)
					break;

				s += source.substr(pos, pos2 - pos);
				s += with;
				pos = pos2 + what.length();
			}
			s += source.substr(pos);
			return s;
		}
		mbs			TOLOWER(const mbs& s)
		{
			mbs s2 = s;
			_strlwr((char*)s2.c_str());
			return s2;
		}
		wcs			TOLOWER(const wcs& s)
		{
			wcs s2 = s;
			_wcslwr((wchar_t*)s2.c_str());
			return s2;
		}
	}
}

