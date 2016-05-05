
#include "mySQLRes.h"
#include <winsock2.h>
#include "mysql/include/mysql.h"

namespace z3D
{
	namespace mySQL
	{
		inline static char* SAFE_STR(char* s) {if(s) return s;return "";}

		void				mySQLRes::___ctor()
		{
			_res=NULL;
			_codepage=CP_ACP;
		}
		void				mySQLRes::___ctor(void* res,unsigned int codepage)
		{
			_res=res;
			_codepage=codepage;
		}
		void				mySQLRes::___ctor(const mySQLRes& other)
		{
			_sc=other._sc;
			_res=other._res;
			_codepage=other._codepage;
			++_sc;
		}
		void				mySQLRes::___dtor()
		{
			if(!(--_sc))
			{
				mysql_free_result((MYSQL_RES*)_res);
				_res=NULL;
				_codepage=CP_ACP;
				_sc=shared_count();
			}
		}
		mySQLRes::mySQLRes()
		{
			___ctor();
		}
		mySQLRes::mySQLRes(void* res,unsigned int codepage)
		{
			___ctor(res,codepage);
		}
		mySQLRes::mySQLRes(const mySQLRes& other)
		{
			___ctor(other);
		}
		mySQLRes::~mySQLRes()
		{
			___dtor();
		}
		mySQLRes& mySQLRes::operator=(const mySQLRes& other)
		{
			___dtor();
			___ctor(other);
			return *this;
		}
		bool mySQLRes::empty() const
		{
			return _res==NULL;
		}
		unsigned __int64 mySQLRes::rows() const
		{
			return _res?mysql_num_rows((MYSQL_RES*)_res):0;
		}
		char** mySQLRes::row(unsigned __int64 row)
		{
			mysql_data_seek((MYSQL_RES*)_res,row);
			return mysql_fetch_row((MYSQL_RES*)_res);
		}
		char** mySQLRes::nextRow()
		{
			return mysql_fetch_row((MYSQL_RES*)_res);
		}
		char		mySQLRes::getChar(int r,int c)
		{
			return (char)atoi(SAFE_STR(row(r)[c]));
		}
		short	mySQLRes::getShort(int r,int c)
		{
			return (short)atoi(SAFE_STR(row(r)[c]));
		}
		int		mySQLRes::getInt(int r,int c)
		{
			return (int)atoi(SAFE_STR(row(r)[c]));
		}
		__int64	mySQLRes::getInt64(int r,int c)
		{
			return (__int64)_atoi64(SAFE_STR(row(r)[c]));
		}
		unsigned char		mySQLRes::getUChar(int r,int c)
		{
			return (unsigned char)atoi(SAFE_STR(row(r)[c]));
		}
		unsigned short	mySQLRes::getUShort(int r,int c)
		{
			return (unsigned short)atoi(SAFE_STR(row(r)[c]));
		}
		unsigned int		mySQLRes::getUInt(int r,int c)
		{
			return (unsigned int)atoi(SAFE_STR(row(r)[c]));
		}
		unsigned __int64	mySQLRes::getUInt64(int r,int c)
		{
			return (unsigned __int64)_atoi64(SAFE_STR(row(r)[c]));
		}
		float	mySQLRes::getFloat(int r,int c)
		{
			return (float)atof(SAFE_STR(row(r)[c]));
		}
		double	mySQLRes::getDouble(int r,int c)
		{
			return (double)atof(SAFE_STR(row(r)[c]));
		}
		mbs		mySQLRes::getString(int r,int c)
		{
			return SAFE_STR(row(r)[c]);
		}
		wcs		mySQLRes::getWString(int r,int c)
		{
			return MBSTOWCS(SAFE_STR(row(r)[c]),_codepage);
		}
	};
};