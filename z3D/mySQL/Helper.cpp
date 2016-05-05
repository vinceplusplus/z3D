
/*
#include "Helper.h"
#include "mysql/include/mysql.h"

namespace z3D
{
	namespace mySQL
	{
		inline string __Z3DMYSQL_API mysql_utf8_to_esc(const char* utf8str)
		{
			char* to=new char[strlen(utf8str)*2+1];
			mysql_escape_string(to,utf8str,(unsigned long)strlen(utf8str));
			string s(to);
			delete[] to;
			return s;
		}
	};
};
*/

