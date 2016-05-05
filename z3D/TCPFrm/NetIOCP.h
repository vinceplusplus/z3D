
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		class NetIOCP: public RefBase
		{
		private:
			enum
			{
				OVERLAPPED_SIZE = sizeof(intptr_t) * 2 + sizeof(uint32_t) * 2 + sizeof(intptr_t),
			};
		private:
			class iocp_thread: public W32Thread
			{
			private:
				NetIOCP*		_iocp;
			public:
				iocp_thread();
			public:
				void		setIOCP(NetIOCP* iocp);
			public:
				void		run();
			};
		private:
			SPtr<Listener>				_listener;
		private:
			volatile uint32_t			_started;
			volatile uint32_t			_stopping;
			volatile uint32_t			_suspend_threads;
			volatile uint32_t			_suspended_threads;
			volatile uint32_t			_stop_threads;
		private:
			intptr_t					_iocp;
		private:
			volatile intptr_t			_socket_accept;
		private:
			iocp_thread					_array_iocp_thread[64];
		private:
			uint32_t					_iocp_threads;
		private:
			char						_addresses[16 + 128 + 16 + 128];
			char						_overlapped_listen[OVERLAPPED_SIZE];
		private:
			mem_pool<SPtr<NetConn>>*		_pool_sptr_netconn;
		private:
			lfqueue< SPtr<NetConn>* >		_queue_pending_drop;
		public:
			NetIOCP();
		protected:
			~NetIOCP();
		public:
			bool						create(size_t max_conn);										// call create() first before doing anything
		public:
			bool						setListener(const SPtr<Listener>& listener);					// set the listener before starting the NetIOCP
		public:
			bool						setIOCPThreads(uint32_t threads);								// set no. of iocp threads before starting the NetIOCP, default 1
			uint32_t					getIOCPThreads() const;
		public:
			bool						start();
		public:
			bool						manage(const SPtr<NetConn>& conn);
		public:
			void						stop();
		public:
			bool						isStarted() const;
			bool						isStopping() const;
			bool						isFullyStopped() const;
		public:
			size_t						getMaxConnections() const;
			size_t						getConnectionCount() const;
		private:
			bool						issueAccept();
			bool						issueRecv(NetConn* c);
			void						iocp_worker();
			void						post_death_process(SPtr<NetConn>* conn);
		private:
			SPtr<NetConn>*				__allocNetConnSPtr(const SPtr<NetConn>& conn);
			void						__deallocNetConnSPtr(SPtr<NetConn>* conn);
		private:
			void						__suspendThreads();
			void						__makeAllDead();
			void						__resumeThreads();
		};
	};
};
