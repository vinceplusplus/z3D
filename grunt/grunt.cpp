// grunt.cpp : Defines the entry point for the console application.
//

#include "../antlr/antlr3.h"

#include <string>

using namespace std;

#include "gruntLexer.h"
#include "gruntParser.h"
#include "../z3D/Core/z_on_debug_new.h"

#include "grunteval.h"
#include "../z3D/Core/z_on_debug_new.h"

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "antlr.lib")

wcs		grunt_unescape_string(const wcs& s)
{
	size_t copy_start, copy_len;
	copy_start = copy_len = 0;

	wcs dest;
	const wchar_t* end = &(*s.begin()) + s.length();
	const wchar_t* start = &(*s.begin());
	for(const wchar_t* ch = start; ch != end; ++ch)
	{
		if(*ch != '\\')
			++copy_len;
		else
		{
			if(copy_len)
			{
				dest.append(s, copy_start, copy_len);
				copy_len = 0;
			}
			// safe to look ahead one character without checking length (either valid or terminating null
			switch(*(ch + 1))
			{
			case '\\':
				dest += '\\';
				break;
			case '"':
				dest += '"';
				break;
			case '\'':
				dest += '\'';
				break;
			case '\r':
				dest += '\r';
				break;
			case '\n':
				dest += '\n';
				break;
			case '\t':
				dest += '\t';
				break;
			case 'u':
				if(end - ch >= 5)
				{
					wchar_t* end_hex = (wchar_t*)ch + 6;
					dest += (wchar_t)wcstol(ch + 2, &end_hex, 16);
					ch += 4;
				}
				break;
			case '\0':
				continue;
			}
			++ch;
			copy_start = (ch + 1) - start;
		}
	}
	if(copy_len)
	{
		dest.append(s, copy_start, copy_len);
		copy_len = 0;
	}
	return dest;
}

gruntMemCell* foobar(void*, gruntRuntime* runtime, gruntMemCell* a)
{
	gruntMemCell* r = runtime->addCell(L"");
	r->asInteger() = a->getElement(L"0")->toInteger() + 10;
	return r;
}

wcs grunt_to_code(gruntMemCell* e)
{
	wcs s;
	switch(e->getType())
	{
	case gruntMemCell::TYPE_DICTIONARY:
		{
			s += L"{ ";
			const gruntMemCell::DICT& dict = e->getDictionary();
			bool first = true;
			for(gruntMemCell::DICT::const_iterator iter = dict.begin(); iter != dict.end(); ++iter)
			{
				if(!first)
					s += L", ";
				s += L".";
				s += FORMATW(L"%s", iter->first.c_str());
				s += L" = ";
				s += grunt_to_code(iter->second);

				first = false;
			}
			s += L" }";
		}
		break;
	case gruntMemCell::TYPE_FUNCTION_BINDING:
		s += FORMATW(L"/* FUNCTION_BINDING */ 0 /* FUNCTION_BINDING */");
		break;
	case gruntMemCell::TYPE_STRING:
		s += L"\"";
		s += FORMATW(L"%s", e->toString().c_str());
		s += L"\"";
		break;
	default:
		s += FORMATW(L"%s", e->toString().c_str());
		break;
	}
	return s;
}

gruntMemCell* to_code(void*, gruntRuntime* runtime, gruntMemCell* a)
{
	gruntMemCell* e = a->getElement(L"0");
	if(!e)
		return NULL;
	wcs s = grunt_to_code(e);
	gruntMemCell* r = runtime->addCell(L"");
	r->asString() = s;
	return r;
}

gruntMemCell* print(void*, gruntRuntime* runtime, gruntMemCell* a)
{
	gruntMemCell* e = a->getElement(L"0");
	if(!e)
		return NULL;
	wprintf(L"%s", e->toString().c_str());
	return NULL;
}

template<size_t K>
void a(const temp_int<K>&)
{
}

#include "../z3D/Core/aligned_storage_inl.h"
#include "../z3D/Core/functor_inl.h"


int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_crtDbgFlag | _CRTDBG_LEAK_CHECK_DF);
#endif

	if(true)
	{
		typed_aligned_storage<Vec3, 12, 4, true> kkkk;
		kkkk.create();
		kkkk.get();
		kkkk.get_unsafe();
		//kkkk.destroy();
	}

	a(temp_int<10>::temp_int());

	if(false)
	{
		SPtr<ZipFileSystem> fs = ZipFileSystem::create(L"test.zip", 0, false);
 		SPtr<ZipFileSystem> fs2 = ZipFileSystem::create(L"test/test.zip", fs, false);
		SPtr<FileReader> fr = fs2->openFile(L"test/test.txt", false);
		char s[1024];
		fr->read(s, 10);
		int k = sizeof(k);
		fr.reset();
		fs2.reset();
		fs.reset();
	}

	//wcs s = L"/*aa*/\"張门재협상파장れたのはhihi\";";
	//wcs s = L"\"a\"?\"b\":\"c\";";
	//wcs s = L"if(1)if(2)4;else 5;else 3;";
	//wcs s = L"for(local x = 1; x < 10; ++x)if(x == 5){y = 10;break;}x = x;";
	//wcs s = L"x = 1;{local x = 2;x = x;}x = x;";
	//wcs s = L"switch(4){default: x = 3; break; case 1: x = 1; break; case 2: x = 2; break;}";
	//wcs s = L"do{++x;}while(x < 5);";
	//wcs s = L"foo = function (a, b) {a = a; b = b;}; for(local i = 0; i < 5; ++i) foo(i, i + 1);";
	//wcs s = L"foo = function (n) {if(n == 0) return 0; if(n == 1) return 1; if(n > 1) return __this_func__(n - 1) + __this_func__(n - 2); return 0; }; a = foo(7);#include \"test_include.h\"";
	//wcs s = L"foo = function (n) {if(n == 0) return 0; if(n == 1) return 1; if(n > 1) return __this_func__(n - 1) + __this_func__(n - 2); return 0; }; a = foo(7);";
	wcs s = L"b = (a = {.\"a\" = 1, .\"b\" = \"abc\"})[\"b\"]; print(\"a = \"); print(to_code(a)); print(\"\n\"); print(\"a.b = \"); print(to_code(a.b)); print(\"\n\"); ";
	//wcs s = L"a + a + a + 1;";
	//wcs s = L"if(1) x = 2;";
	//wcs s = L"x = 5;";
	//wcs s = L"s = \"張门재협상파장れたのはhihi\" + \"alksjflk;asdjflk;ad\";";
	//wcs s = L";";
	//wcs s = L"print(a = foobar(10));";

	gruntRuntime r;

	r.bindFunc(&foobar, NULL, L"foobar", false);
	r.bindFunc(&print, NULL, L"print", false);
	r.bindFunc(&to_code, NULL, L"to_code", false);

	r.parse(s);


	return 0;
}

