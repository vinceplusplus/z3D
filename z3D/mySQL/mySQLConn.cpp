
#include "mySQLConn.h"
#include "mySQLRes.h"

#include <winsock2.h>

#include "mysql/include/mysql.h"
#include "mysql/include/errmsg.h"

namespace z3D
{
	namespace mySQL
	{
		mySQLConn::mySQLConn()
		{
			_handle=(MYSQL*)mysql_init(NULL);
			_codepage=CP_ACP;
		}
		mySQLConn::~mySQLConn()
		{
			if(_handle)
			{
				mysql_close((MYSQL*)_handle);
				_handle=NULL;
			}
		}
		bool mySQLConn::connect(const char* host,const char* user,const char* passwd,const char* charset,const char* db,unsigned int port)
		{
			_host=host;
			_user=user;
			_passwd=passwd;
			_charset=charset;
			_db=db;
			_port=port;

			return reconnect();
		}
		bool mySQLConn::reconnect()
		{
			mysql_options((MYSQL*)_handle,MYSQL_SET_CHARSET_NAME,_charset.c_str());
			if(!mysql_real_connect((MYSQL*)_handle,_host.c_str(),_user.c_str(),_passwd.c_str(),_db.c_str(),_port,NULL,0))
			{
				mysql_close((MYSQL*)_handle);
				_handle=NULL;
				return false;
			}
			updateCharSet();
			return true;
		}
		bool mySQLConn::connected() const
		{
			return _handle!=NULL;
		}
		bool mySQLConn::query(const char* query,unsigned long length)
		{
			if(length==-1)
				length=(unsigned long)strlen(query);

			setLastQuery(query);

			int err=0;
			do
			{
				if(mysql_real_query((MYSQL*)_handle,query,length)==0)
					return true;
				err = mysql_errno((MYSQL*)_handle);
				if(err == CR_CONNECTION_ERROR || err == CR_CONN_HOST_ERROR || err == CR_COMMANDS_OUT_OF_SYNC || err == CR_SERVER_GONE_ERROR || err == CR_SERVER_LOST || err ==CR_UNKNOWN_ERROR)
				{
					while(!ping())
						Sleep(10);
					continue;
				}
				if(err)
				{
					mbs s;
					s.resize(length);
					memcpy(&s[0],query,length);
					syslog<<wcs(L"\"") + MBSTOWCS(s) + L"\" errno: " + FORMATW(L"%d errstring: ", err) + MBSTOWCS(mysql_error((MYSQL*)_handle));
				}
			}while(false);
			return err==0;
		}
		bool mySQLConn::query(const mbs& query)
		{
			return this->query(query.c_str(),(unsigned long)query.length());
		}
		bool mySQLConn::query(const wcs& query)
		{
			return this->query(WCSTOMBS(query,_codepage));
		}
		bool mySQLConn::queryScalar(const mbs& query,char& scalar)
		{
			__int64 value;
			if(!queryScalar(query,value))
				return false;
			scalar=(char)value;
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query,unsigned char& scalar)
		{
			return queryScalar(query,(char&)scalar);
		}
		bool mySQLConn::queryScalar(const mbs& query,short& scalar)
		{
			__int64 value;
			if(!queryScalar(query,value))
				return false;
			scalar=(short)value;
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query,unsigned short& scalar)
		{
			return queryScalar(query,(short&)scalar);
		}
		bool mySQLConn::queryScalar(const mbs& query,long& scalar)
		{
			__int64 value;
			if(!queryScalar(query,value))
				return false;
			scalar=(long)value;
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query,unsigned long& scalar)
		{
			return queryScalar(query,(long&)scalar);
		}
		bool mySQLConn::queryScalar(const mbs& query,int& scalar)
		{
			__int64 value;
			if(!queryScalar(query,value))
				return false;
			scalar=(int)value;
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query,unsigned int& scalar)
		{
			return queryScalar(query,(int&)scalar);
		}
		bool mySQLConn::queryScalar(const mbs& query,__int64& scalar)
		{
			bool ret=this->query(query);
			if(!ret)
				return false;
			mySQLRes res=store();
			if(res.empty())
				return false;
			if(!res.rows())
				return false;
			scalar=_atoi64(res.nextRow()[0]);
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query,unsigned __int64& scalar)
		{
			return queryScalar(query,(__int64&)scalar);
		}
		bool mySQLConn::queryScalar(const mbs& query,float& scalar)
		{
			double value;
			if(!queryScalar(query,value))
				return false;
			scalar=(float)value;
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query,double& scalar)
		{
			bool ret=this->query(query);
			if(!ret)
				return false;
			mySQLRes res=store();
			if(res.empty())
				return false;
			if(!res.rows())
				return false;
			scalar=atof(res.nextRow()[0]);
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query, mbs& scalar)
		{
			bool ret=this->query(query);
			if(!ret)
				return false;
			mySQLRes res=store();
			if(res.empty())
				return false;
			if(!res.rows())
				return false;
			scalar=res.nextRow()[0];
			return true;
		}
		bool mySQLConn::queryScalar(const mbs& query, wcs& scalar)
		{
			mbs s;
			if(!queryScalar(query,s))
				return false;
			scalar = MBSTOWCS(s, _codepage);
			return true;
		}
		int		mySQLConn::queryScalarInt(const mbs& query)
		{
			int scalar;
			if(queryScalar(query, scalar))
				return scalar;
			return 0;
		}
		wcs		mySQLConn::queryScalarWString(const mbs& query)
		{
			wcs scalar;
			if(queryScalar(query,scalar))
				return scalar;
			return L"";
		}
		mbs		mySQLConn::queryScalarString(const mbs& query)
		{
			mbs scalar;
			if(queryScalar(query,scalar))
				return scalar;
			return "";
		}
		int		mySQLConn::findAvailableID(const mbs& table_name, const mbs& field_name)
		{
			if(queryScalarInt("select count(*) from `"+table_name+"` where `"+field_name+"`=1")==0)
				return 1;
			return queryScalarInt("select a.`"+field_name+"`+1 from `"+table_name+"` a where (select count(*) from `"+table_name+"` b where b.`"+field_name+"`=a.`"+field_name+"`+1) = 0 limit 0,1");
		}
		mySQLRes mySQLConn::store()
		{
			MYSQL_RES* res=mysql_store_result((MYSQL*)_handle);
			if(!res)
				return mySQLRes(NULL,_codepage);
			return mySQLRes(res,_codepage);
		}
		unsigned __int64 mySQLConn::affectedRows() const
		{
			return mysql_affected_rows((MYSQL*)_handle);
		}
		unsigned __int64 mySQLConn::getInsertID() const
		{
			return mysql_insert_id((MYSQL*)_handle);
		}
		bool mySQLConn::ping()
		{
			DWORD last_tick=GetTickCount();
			int ret;
			while((ret=mysql_ping((MYSQL*)_handle))!=0)
			{
				switch(ret)
				{
				case CR_CONNECTION_ERROR:
					syslog<<L"mySQLConn::ping(), mysql_ping() returns CR_CONNECTION_ERROR";
					break;
				case CR_CONN_HOST_ERROR:
					syslog<<L"mySQLConn::ping(), mysql_ping() returns CR_CONN_HOST_ERROR";
					break;
				case CR_COMMANDS_OUT_OF_SYNC:
					syslog<<L"mySQLConn::ping(), mysql_ping() returns CR_COMMANDS_OUT_OF_SYNC";
					break;
				case CR_SERVER_GONE_ERROR:
					syslog<<L"mySQLConn::ping(), mysql_ping() returns CR_SERVER_GONE_ERROR";
					break;
				case CR_SERVER_LOST:
					syslog<<L"mySQLConn::ping(), mysql_ping() returns CR_SERVER_LOST";
					break;
				case CR_UNKNOWN_ERROR:
					syslog<<L"mySQLConn::ping(), mysql_ping() returns CR_UNKNOWN_ERROR";
					break;
				default:
					syslog<<FORMATW(L"mySQLConn::ping(), mysql_ping() returns %d, ", ret) + L"mysql_error() returns " + MBSTOWCS(mysql_error((MYSQL*)_handle));
					break;
				}
				DWORD cur_tick=GetTickCount();
				if(cur_tick>last_tick+5000)
				{
					syslog<<L"mySQLConn::ping(), pinged over 5 seconds, sleep 5 seconds first, then reconnect...";
					Sleep(5000);
					mysql_close((MYSQL*)_handle);
					_handle=NULL;
					do
					{
						_handle=(MYSQL*)mysql_init(NULL);
						_codepage=CP_ACP;

						if(reconnect())
							break;
						Sleep(5000);
						syslog<<L"mySQLConn::ping(), reconnect fails, sleep 5 seconds first, then reconnect...";
					}while(true);
					last_tick=GetTickCount();
					continue;
				}
				Sleep(1000);
			}
			return true;
		}
		mbs		mySQLConn::escStr(const char* str) const
		{
			// need to optimize, don't allocate every time
			int len=(int)strlen(str);
			char* to=new char[len*2+1];
			mysql_real_escape_string((MYSQL*)_handle, to, str, len);
			mbs s(to);
			delete[] to;
			return s;
		}
		mbs		mySQLConn::escStr(const mbs& str) const
		{
			return escStr(str.c_str());
		}
		wcs		mySQLConn::escStr(const wcs& str) const
		{
			wcs s;
			s.reserve(str.length()*2);
			int i;
			for(i=0;i<(int)str.length();++i)
			{
				switch(str[i])
				{
				case '\"':
				case '\\':
					s+='\\';
				}
				s+=str[i];
			}
			return s;
		}
		void mySQLConn::updateCharSet()
		{
			mbs cs;
			query("show variables like 'character_set_connection';");
			mySQLRes res = store();
			cs = res.getString(0,1);
			if(cs=="big5")
				_codepage=10002;
			else if(cs=="dec8")
				_codepage=CP_ACP;
			else if(cs=="cp850")
				_codepage=CP_ACP;
			else if(cs=="hp8")
				_codepage=CP_ACP;
			else if(cs=="koi8r")
				_codepage=20866;
			else if(cs=="latin1")
				_codepage=28591;
			else if(cs=="swe7")
				_codepage=20107;
			else if(cs=="ascii")
				_codepage=20127;
			else if(cs=="ujis")
				_codepage=51932;
			else if(cs=="cp1251")
				_codepage=1251;
			else if(cs=="hebrew")
				_codepage=28598;
			else if(cs=="tis620")
				_codepage=CP_ACP;
			else if(cs=="euckr")
				_codepage=51949;
			else if(cs=="koi8u")
				_codepage=21866;
			else if(cs=="gb2312")
				_codepage=20936;
			else if(cs=="greek")
				_codepage=28597;
			else if(cs=="cp1250")
				_codepage=1250;
			else if(cs=="gbk")
				_codepage=CP_ACP;
			else if(cs=="latin5")
				_codepage=28599;
			else if(cs=="armscii8")
				_codepage=CP_ACP;
			else if(cs=="utf8")
				_codepage=CP_UTF8;
			else if(cs=="ucs2")
				_codepage=1200;
			else if(cs=="cp866")
				_codepage=CP_ACP;
			else if(cs=="keybcs2")
				_codepage=CP_ACP;
			else if(cs=="macce")
				_codepage=CP_ACP;
			else if(cs=="macroman")
				_codepage=CP_ACP;
			else if(cs=="cp852")
				_codepage=CP_ACP;
			else if(cs=="latin7")
				_codepage=CP_ACP;
			else if(cs=="cp1256")
				_codepage=1256;
			else if(cs=="cp1257")
				_codepage=1257;
			else if(cs=="binary")
				_codepage=CP_ACP;
			else if(cs=="geostd8")
				_codepage=CP_ACP;
			else
				_codepage=CP_ACP;
			_charset_name=cs;
		}
		void				mySQLConn::setLastQuery(const mbs& last_query)
		{
			scoped_lock<mutex_fast> o(_mx_last_query);
			_last_query = last_query;
		}
		const mbs			mySQLConn::getLastQuery()
		{
			scoped_lock<mutex_fast> o(_mx_last_query);

			mbs s;
			s = _last_query;

			return s;
		}
	};
};