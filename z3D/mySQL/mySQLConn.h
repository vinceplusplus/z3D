
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace mySQL
	{
		class mySQLConn: public RefBase
		{
		private:
			mbs					_host;
			mbs					_user;
			mbs					_passwd;
			mbs					_charset;
			mbs					_db;
			int					_port;
		private:
			void*				_handle;			// mysql handle
			unsigned int		_codepage;			// win32 codepage constant indicating current connection character set
			mbs					_charset_name;
		private:
			mutex_fast			_mx_last_query;
			mbs					_last_query;
		public:
			mySQLConn();
		protected:
			virtual ~mySQLConn();
		public:
			bool		connect(const char* host, const char* user, const char* passwd, const char* charset, const char* db, unsigned int port);
		private:
			bool		reconnect();
		public:
			bool		connected() const;
		public:
			bool		query(const char* query, unsigned long length = -1);
			bool		query(const mbs& query);
			bool		query(const wcs& query);
			bool		queryScalar(const mbs& query, char& scalar);
			bool		queryScalar(const mbs& query, unsigned char& scalar);
			bool		queryScalar(const mbs& query, short& scalar);
			bool		queryScalar(const mbs& query, unsigned short& scalar);
			bool		queryScalar(const mbs& query, long& scalar);
			bool		queryScalar(const mbs& query, unsigned long& scalar);
			bool		queryScalar(const mbs& query, int& scalar);
			bool		queryScalar(const mbs& query, unsigned int& scalar);
			bool		queryScalar(const mbs& query, __int64& scalar);
			bool		queryScalar(const mbs& query, unsigned __int64& scalar);
			bool		queryScalar(const mbs& query, float& scalar);
			bool		queryScalar(const mbs& query, double& scalar);
			bool		queryScalar(const mbs& query, mbs& scalar);
			bool		queryScalar(const mbs& query, wcs& scalar);
		public:
			int			queryScalarInt(const mbs& query);
			wcs			queryScalarWString(const mbs& query);
			mbs			queryScalarString(const mbs& query);
		public:
			int			findAvailableID(const mbs& table_name, const mbs& field_name);
		public:
			mySQLRes	store();
		public:
			unsigned __int64	affectedRows() const;
			unsigned __int64	getInsertID() const;
		public:
			bool		ping();
		public:
			mbs			escStr(const char* str) const;
			mbs			escStr(const mbs& str) const;
			wcs			escStr(const wcs& str) const;
		private:
			void		updateCharSet();
		private:
			void		setLastQuery(const mbs& last_query);
		public:
			const mbs	getLastQuery();
		};
	};
};