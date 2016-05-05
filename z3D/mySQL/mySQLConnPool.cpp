
#include "mySQLConnPool.h"
#include "mySQLConn.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace mySQL
	{
		void					mySQLPConn::___ctor(const SPtr<mySQLConn>& conn,intptr_t evt)
		{
			_conn=conn;
			_evt=evt;
		}
		void					mySQLPConn::___ctor()
		{
			_evt=NULL;
		}
		void					mySQLPConn::___ctor(const mySQLPConn& other)
		{
			_conn=other._conn;
			_evt=other._evt;
			_sc=other._sc;
			++_sc;
		}
		void					mySQLPConn::___dtor()
		{
			if(!(--_sc))
				dispose();
			_sc=shared_count();
		}
		mySQLPConn::mySQLPConn(const SPtr<mySQLConn>& conn,intptr_t evt)
		{
			___ctor(conn,evt);
		}
		mySQLPConn::mySQLPConn()
		{
			___ctor();
		}
		mySQLPConn::mySQLPConn(const mySQLPConn& other)
		{
			___ctor(other);
		}
		mySQLPConn::~mySQLPConn()
		{
			___dtor();
		}
		mySQLPConn&			mySQLPConn::operator=(const mySQLPConn& other)
		{
			___dtor();
			___ctor(other);
			return *this;
		}
		mySQLConn*			mySQLPConn::operator->() const
		{
			return _conn.get();
		}
		void					mySQLPConn::release()
		{
			___dtor();
		}
		void					mySQLPConn::dispose()
		{
			if(_evt)
			{
				_conn.reset();
				SetEvent((HANDLE)_evt);
				_evt=NULL;
			}
		}

		mySQLConnPool::mySQLConnPool()
		{
			_idle_time = 60;
		}
		mySQLConnPool::~mySQLConnPool()
		{
			int i;
			for(i=0;i<(int)_array_evt.size();++i)
				CloseHandle((HANDLE)_array_evt[i]);
		}
		SPtr<mySQLConnPool>			mySQLConnPool::create(const char* host,const char* user,const char* passwd,const char* charset,const char* db,unsigned int port,int count)
		{
			count=max(1,count);
			vector<SPtr<mySQLConn> > array;
			int i;
			for(i=0;i<count;++i)
			{
				SPtr<mySQLConn> conn(new mySQLConn());
				if(!conn->connect(host,user,passwd,charset,db,port))
					return NULL;
				array.push_back(conn);
			}
			SPtr<mySQLConnPool> pool(new mySQLConnPool());
			pool->_array_conn=array;
			for(i=0;i<count;++i)
			{
				pool->_array_evt.push_back((intptr_t)CreateEvent(NULL,FALSE,TRUE,NULL));
				pool->_array_idle_counter.push_back(Counter());
			}
			return pool;
		}
		long						mySQLConnPool::count()
		{
			return (long)_array_conn.size();
		}
		mySQLPConn					mySQLConnPool::acquire()
		{
			int index=0;

			while(true)
			{
				DWORD ret=WaitForMultipleObjects(count(),(const HANDLE*)&_array_evt[0],FALSE,5000);
				if(ret>=WAIT_OBJECT_0&&ret<WAIT_OBJECT_0+count())
				{
					index=(int)(ret-WAIT_OBJECT_0);
					break;
				}
			}
			_array_idle_counter[index].start();
			return mySQLPConn(_array_conn[index],_array_evt[index]);
		}
		void						mySQLConnPool::queryAll(const char* query)
		{
			int i;
			for(i=0;i<count();++i)
				if(WaitForSingleObject((HANDLE)_array_evt[i],INFINITE)==WAIT_OBJECT_0)
				{
					_array_conn[i]->query(query);
					_array_idle_counter[i].start();
					SetEvent((HANDLE)_array_evt[i]);
				}
		}
		void						mySQLConnPool::pingAll()
		{
			int i;
			for(i=0;i<count();++i)
				if(WaitForSingleObject((HANDLE)_array_evt[i],INFINITE)==WAIT_OBJECT_0)
				{
					_array_conn[i]->ping();
					_array_idle_counter[i].start();
					SetEvent((HANDLE)_array_evt[i]);
				}
		}
		void						mySQLConnPool::setKeepAliveIdleTime(REAL idle_time)
		{
			_idle_time = idle_time;
		}
		REAL						mySQLConnPool::getKeepAliveIdleTime() const
		{
			return _idle_time;
		}
		void						mySQLConnPool::keepAlive()
		{
			int i;
			for(i=0;i<count();++i)
				if(WaitForSingleObject((HANDLE)_array_evt[i],INFINITE)==WAIT_OBJECT_0)
				{
					Counter& c=_array_idle_counter[i];
					c.update();
					if(c.elapsedTime()>=_idle_time)
					{
						_array_conn[i]->ping();
						c.start();
					}
					SetEvent((HANDLE)_array_evt[i]);
				}
		}
	};
};