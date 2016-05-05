
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace mySQL
	{
		// stands for mySQL Pooled Connection
		class mySQLPConn
		{
		private:
			shared_count			_sc;
		private:
			SPtr<mySQLConn>			_conn;
			intptr_t				_evt;
		private:
			void					___ctor(const SPtr<mySQLConn>& conn, intptr_t evt);
			void					___ctor();
			void					___ctor(const mySQLPConn& other);
		private:
			void					___dtor();
		private:
			mySQLPConn(const SPtr<mySQLConn>& conn, intptr_t evt);
		public:
			mySQLPConn();
			mySQLPConn(const mySQLPConn& other);
		public:
			~mySQLPConn();
		public:
			mySQLPConn&				operator=(const mySQLPConn& other);
		public:
			mySQLConn*				operator->() const;
		public:
			void					release();
		private:
			void					dispose();
		public:
			friend class mySQLConnPool;
		};

		class mySQLConnPool: public RefBase
		{
		private:
			vector< SPtr<mySQLConn> >	_array_conn;
			vector<intptr_t>			_array_evt;
			vector<Counter>				_array_idle_counter;
		private:
			mbs							_host;
			mbs							_user;
			mbs							_passwd;
			mbs							_charset;
			mbs							_db;
			int							_port;
		private:
			REAL						_idle_time;
		protected:
			mySQLConnPool();
			virtual ~mySQLConnPool();
		public:
			static SPtr<mySQLConnPool>		create(const char* host, const char* user, const char* passwd, const char* charset, const char* db, unsigned int port, int count);
		public:
			long							count();
		public:
			mySQLPConn						acquire();
		public:
			void							queryAll(const char* query);
			void							pingAll();
		public:
			void							setKeepAliveIdleTime(REAL idle_time);
			REAL							getKeepAliveIdleTime() const;
		public:
			void							keepAlive();
		};
	};
};


