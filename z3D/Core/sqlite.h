
#pragma once

#include "predecl.h"

#include "ResBase.h"

/*******************************

for rebuilding the static library, no special care needs to be taken in source code, just compile, no need to define THREADSAFE preprocessor, because it only affects os2 or unix

using this sqlite3, however, utf-16(ucs-2) encoding is enforced in the storage side

sqlite_db - represents a database object, or a conneciton, has methods to query and get resultset(s)

sqlite_res - the object handling reading from a resultset

beware date format in sqlite is YYYY-MM-DD, zero filled

if you experience poor performance in sqlite with insert, update, delete, try pragma_synchronous(0) if you don't care about undefined result if power-off of sth like that

*******************************/

#pragma warning(push)
#pragma warning(disable:4275)
#pragma warning(disable:4251)

namespace z3D
{
	namespace Core
	{
		class sqlite_res
		{
		public:
			typedef struct
			{
				wstring					text;
				vector<char>			blob;
			} VALUE;
		protected:
			vector< vector<VALUE> >		_res;
		public:
			int							rows() const;
			const vector<VALUE>&		row(int index) const;
		public:
			bool						empty() const;
		public:
			const VALUE&				get(int r,int c) const;
			int							getInt(int r,int c) const;
			unsigned int				getUInt(int r,int c) const;
			long						getLong(int r,int c) const;
			unsigned long				getULong(int r,int c) const;
			__int64						getInt64(int r,int c) const;
			unsigned __int64			getUInt64(int r,int c) const;
			float						getFloat(int r,int c) const;
			double						getDouble(int r,int c) const;
			const wstring&				getWString(int r,int c) const;
			const vector<char>&			getBlob(int r,int c) const;
		public:
			friend class				sqlite_db;
		};

		class sqlite_db: public RefBase
		{
		protected:
			wstring					_filepath;
		protected:
			wstring					_err_msg;							// error message set if any
		protected:
			void*					_handle;							// the sqlite db handle
		protected:
			void*					_stmt;								// currently prepare statment handle
			wstring					_prepare_remains;					// remaining query for next statment preparation
		protected:
			vector<sqlite_res>		_array_res;							// multiple results if in query there is more than one statement which produces result, even empty resultset will be added (condition: no. of columns != 0)
		protected:
			static sqlite_res		_null_res;							// used to avoid returning out of bound result element in _array_res
		public:
			static SPtr<sqlite_db>	open(const wstring& filepath);
		protected:
			sqlite_db();
			virtual ~sqlite_db();
		public:
			void					pragma_synchronous(int mode);					// 0 - off, 1 - normal, 2 - full(default), please read the documentation carefully before calling this method, however, hm... for queries that will write to disk in full mode, the queries will be executed extremely slow... 
		public:
			bool					query(const wstring& s);						// execute one or more query statements, multiple results are returned, even empty resultset will be added (condition: no. of columns != 0), return true if all statements execute successfully
		public:
			bool					queryScalar(const wstring& s,wstring& scalar);
			bool					queryScalar(const wstring& s,int& scalar);
			bool					queryScalar(const wstring& s,unsigned int& scalar);
			bool					queryScalar(const wstring& s,long& scalar);
			bool					queryScalar(const wstring& s,unsigned long& scalar);
			bool					queryScalar(const wstring& s,__int64& scalar);
			bool					queryScalar(const wstring& s,unsigned __int64& scalar);
			bool					queryScalar(const wstring& s,float& scalar);
			bool					queryScalar(const wstring& s,double& scalar);
		public:
			wstring					queryScalar(const wstring& s);
			int						queryScalarInt(const wstring& s);
			unsigned int			queryScalarUInt(const wstring& s);
			long					queryScalarLong(const wstring& s);
			unsigned long			queryScalarULong(const wstring& s);
			__int64					queryScalarInt64(const wstring& s);
			unsigned __int64		queryScalarUInt64(const wstring& s);
			float					queryScalarFloat(const wstring& s);
			double					queryScalarDouble(const wstring& s);
		public:
			bool					prepare(const wstring& query);				// call this method will prepare the first statement in the query, return true means successful and can continue
			bool					prepare();									// call this method will prepare the next statement in the previous query, return true means successful and can continue
			bool					step();										// should be called everytime prepare(const wstring&) or prepare() returns true, of course before call this method, different sorts of bind methods could be called to bind arguments
		public:							// bind argment to the preparing statement, index starting from 1
			void					bind_blob(int index_1_based,const void* arg,int length);
			void					bind_double(int index_1_based,double arg);
			void					bind_int(int index_1_based,int arg);
			void					bind_int64(int index_1_based,__int64 arg);
			void					bind_null(int index_1_based);
			void					bind_text(int index_1_based,const wstring& arg);
		public:
			int						res_count() const;
			const sqlite_res&		get_res() const;							// return the last resultset, the last resultset produced in query
			const sqlite_res&		get_res(int index) const;
		public:
			int						affected_rows() const;
			__int64					last_insert_id() const;
		};
	};
};

#pragma warning(pop)

