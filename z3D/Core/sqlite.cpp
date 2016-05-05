
#include "sqlite.h"

#include "../../sqlite3/sqlite3.h"

#include "ResBase.h"

namespace z3D
{
	namespace Core
	{
		int							sqlite_res::rows() const {return (int)_res.size();}
		const vector<sqlite_res::VALUE>&		sqlite_res::row(int index) const {return _res[index];}
		bool						sqlite_res::empty() const {return rows()==0;}
		const sqlite_res::VALUE&				sqlite_res::get(int r,int c) const {return _res[r][c];}
		int							sqlite_res::getInt(int r,int c) const {return _wtoi(get(r,c).text.c_str());}
		unsigned int				sqlite_res::getUInt(int r,int c) const {return (unsigned int)_wtoi(get(r,c).text.c_str());}
		long						sqlite_res::getLong(int r,int c) const {return (long)_wtoi(get(r,c).text.c_str());}
		unsigned long				sqlite_res::getULong(int r,int c) const {return (unsigned long)_wtoi(get(r,c).text.c_str());}
		__int64						sqlite_res::getInt64(int r,int c) const {return _wtoi64(get(r,c).text.c_str());}
		unsigned __int64			sqlite_res::getUInt64(int r,int c) const {return (unsigned __int64)_wtoi64(get(r,c).text.c_str());}
		float						sqlite_res::getFloat(int r,int c) const {return (float)_wtof(get(r,c).text.c_str());}
		double						sqlite_res::getDouble(int r,int c) const {return _wtof(get(r,c).text.c_str());}
		const wstring&				sqlite_res::getWString(int r,int c) const {return get(r,c).text;}
		const vector<char>&			sqlite_res::getBlob(int r,int c) const {return get(r,c).blob;}

		SPtr<sqlite_db>			sqlite_db::open(const wstring& filepath)
		{
			sqlite3* handle=NULL;
			sqlite3_open16(filepath.c_str(),&handle);
			if(!handle)
				return NULL;
			sqlite_db* db=new sqlite_db();
			db->_filepath=filepath;
			db->_handle=handle;

			db->query(L"pragma cache_size = 4096; pragma count_changes = 0; pragma temp_store = memory;");

			return db;
		}
		sqlite_db::sqlite_db()
		{
			_handle=NULL;

			_stmt=NULL;
		}
		sqlite_db::~sqlite_db()
		{
			if(_stmt)
				sqlite3_finalize((sqlite3_stmt*)_stmt);
			if(_handle)
				sqlite3_close((sqlite3*)_handle);
		}
		void					sqlite_db::pragma_synchronous(int mode)
		{
			wchar_t buffer[64];
			_itow_s(mode,buffer,10);
			query(wstring(L"pragma synchronous = ")+buffer+L";");
		}
		bool					sqlite_db::query(const wstring& s)
		{
			if(!prepare(s))
				return false;
			do
			{
				if(!step())
					return false;
			}while(prepare());
			return true;
		}
		bool					sqlite_db::queryScalar(const wstring& s,wstring& scalar)			{if(!query(s) || res_count()==0 || get_res().empty() || !get_res()._res[0].size()) return false; scalar=get_res().getWString(0,0); return true;}
		bool					sqlite_db::queryScalar(const wstring& s,int& scalar)				{wstring r;if(!queryScalar(s,r)) return false; scalar=_wtoi(r.c_str());return true;}
		bool					sqlite_db::queryScalar(const wstring& s,unsigned int& scalar)		{wstring r;if(!queryScalar(s,r)) return false; scalar=_wtoi(r.c_str());return true;}
		bool					sqlite_db::queryScalar(const wstring& s,long& scalar)				{wstring r;if(!queryScalar(s,r)) return false; scalar=_wtoi(r.c_str());return true;}
		bool					sqlite_db::queryScalar(const wstring& s,unsigned long& scalar)		{wstring r;if(!queryScalar(s,r)) return false; scalar=_wtoi(r.c_str());return true;}
		bool					sqlite_db::queryScalar(const wstring& s,__int64& scalar)			{wstring r;if(!queryScalar(s,r)) return false; scalar=_wtoi64(r.c_str());return true;}
		bool					sqlite_db::queryScalar(const wstring& s,unsigned __int64& scalar)	{wstring r;if(!queryScalar(s,r)) return false; scalar=_wtoi64(r.c_str());return true;}
		bool					sqlite_db::queryScalar(const wstring& s,float& scalar)				{wstring r;if(!queryScalar(s,r)) return false; scalar=(float)_wtof(r.c_str());return true;}
		bool					sqlite_db::queryScalar(const wstring& s,double& scalar)			{wstring r;if(!queryScalar(s,r)) return false; scalar=_wtof(r.c_str());return true;}
		wstring				sqlite_db::queryScalar(const wstring& s)							{wstring scalar;queryScalar(s,scalar);return scalar;}
		int					sqlite_db::queryScalarInt(const wstring& s)						{int scalar=0;queryScalar(s,scalar);return scalar;}
		unsigned int			sqlite_db::queryScalarUInt(const wstring& s)						{unsigned int scalar=0;queryScalar(s,scalar);return scalar;}
		long					sqlite_db::queryScalarLong(const wstring& s)						{long scalar=0;queryScalar(s,scalar);return scalar;}
		unsigned long		sqlite_db::queryScalarULong(const wstring& s)						{unsigned long scalar=0;queryScalar(s,scalar);return scalar;}
		__int64				sqlite_db::queryScalarInt64(const wstring& s)						{__int64 scalar=0;queryScalar(s,scalar);return scalar;}
		unsigned __int64		sqlite_db::queryScalarUInt64(const wstring& s)						{unsigned __int64 scalar=0;queryScalar(s,scalar);return scalar;}
		float				sqlite_db::queryScalarFloat(const wstring& s)						{float scalar=0;queryScalar(s,scalar);return scalar;}
		double				sqlite_db::queryScalarDouble(const wstring& s)						{double scalar=0;queryScalar(s,scalar);return scalar;}
		bool					sqlite_db::prepare(const wstring& query)				// call this method will prepare the first statement in the query, return true means successful and can continue
		{
			_err_msg=L"";
			_array_res.clear();
			_prepare_remains=query;

			return prepare();
		}
		bool					sqlite_db::prepare()									// call this method will prepare the next statement in the previous query, return true means successful and can continue
		{
			_err_msg=L"";
			if(_stmt)
				sqlite3_finalize((sqlite3_stmt*)_stmt);
			_stmt=NULL;

			if(!_prepare_remains.length())
			{
				_err_msg=(const wchar_t*)sqlite3_errmsg16((sqlite3*)_handle);
				return false;
			}

			const wchar_t* remains=NULL;
			sqlite3_prepare16_v2((sqlite3*)_handle,_prepare_remains.c_str(),(int)(_prepare_remains.length()<<1),(sqlite3_stmt**)&_stmt,(const void**)&remains);
			if(!_stmt)
				return false;

			_prepare_remains=_prepare_remains.substr(remains-_prepare_remains.c_str());

			return true;
		}
		bool					sqlite_db::step()
		{
			if(!_stmt)
				return false;
			sqlite3_stmt* stmt=(sqlite3_stmt*)_stmt;
			int ret=sqlite3_step(stmt);
			if(ret==SQLITE_DONE)
			{
				if(sqlite3_column_count(stmt))
					_array_res.push_back(sqlite_res());						// check if the statement will produce resultset, add a blank even if no rows to keep consistent
				return true;
			}
			if(ret!=SQLITE_ROW)
			{
				_err_msg=(const wchar_t*)sqlite3_errmsg16((sqlite3*)_handle);
				return false;
			}

			if(ret==SQLITE_ROW)
				_array_res.push_back(sqlite_res());
			sqlite_res& res=*(_array_res.end()-1);
			int row=0;
			while(ret==SQLITE_ROW)
			{
				res._res.push_back(vector<sqlite_res::VALUE>());
				int i;
				int count=sqlite3_column_count(stmt);
				for(i=0;i<count;++i)
				{
					res._res[row].push_back(sqlite_res::VALUE());
					int type=sqlite3_column_type(stmt,i);
					if(type==SQLITE_NULL)
						continue;
					if(type==SQLITE_BLOB)
					{
						if(!sqlite3_column_bytes(stmt,i))
							continue;
						vector<char>& blob=res._res[row][i].blob;
						blob.resize(sqlite3_column_bytes(stmt,i));
						memcpy(&blob[0],sqlite3_column_blob(stmt,i),sqlite3_column_bytes(stmt,i));
						continue;
					}
					res._res[row][i].text=(const wchar_t*)sqlite3_column_text16(stmt,i);
				}
				++row;
				ret=sqlite3_step(stmt);
			}
			return true;
		}
		void					sqlite_db::bind_blob(int index_1_based,const void* arg,int length)
		{
			sqlite3_bind_blob((sqlite3_stmt*)_stmt,index_1_based,arg,length,SQLITE_TRANSIENT);
		}
		void					sqlite_db::bind_double(int index_1_based,double arg)
		{
			sqlite3_bind_double((sqlite3_stmt*)_stmt,index_1_based,arg);
		}
		void					sqlite_db::bind_int(int index_1_based,int arg)
		{
			sqlite3_bind_int((sqlite3_stmt*)_stmt,index_1_based,arg);
		}
		void					sqlite_db::bind_int64(int index_1_based,__int64 arg)
		{
			sqlite3_bind_int64((sqlite3_stmt*)_stmt,index_1_based,arg);
		}
		void					sqlite_db::bind_null(int index_1_based)
		{
			sqlite3_bind_null((sqlite3_stmt*)_stmt,index_1_based);
		}
		void					sqlite_db::bind_text(int index_1_based,const wstring& arg)
		{
			sqlite3_bind_text16((sqlite3_stmt*)_stmt,index_1_based,arg.c_str(),-1,SQLITE_TRANSIENT);
		}
		int						sqlite_db::res_count() const {return (int)_array_res.size();}
		const sqlite_res&		sqlite_db::get_res() const
		{
			if(!res_count())
				return _null_res;
			return _array_res[res_count()-1];
		}
		const sqlite_res&		sqlite_db::get_res(int index) const
		{
			if(index<0 || index>=res_count())
				return _null_res;
			return _array_res[index];
		}
		int						sqlite_db::affected_rows() const {return sqlite3_changes((sqlite3*)_handle);}
		__int64					sqlite_db::last_insert_id() const {return sqlite3_last_insert_rowid((sqlite3*)_handle);}
	};
};

