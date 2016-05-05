
#define WIN32_MEAN_AND_LEAN

#include <winsock2.h>
#include <mswsock.h>

#include "NetIOCP.h"
#include "NetConn.h"
#include "NetMsg.h"
#include "SendOL.h"

#include "Listener.h"
#include "NetSys.h"

#include "../Core/lfqueue_inl.h"

namespace z3D
{
	namespace TCPFrm
	{
		NetIOCP::iocp_thread::iocp_thread()
		{
			_iocp = NULL;
		}
		void		NetIOCP::iocp_thread::setIOCP(NetIOCP* iocp)
		{
			_iocp = iocp;
		}
		void		NetIOCP::iocp_thread::run()
		{
			_iocp->iocp_worker();
		}

		NetIOCP::NetIOCP()
		{
			Z_STATIC_ASSERT( sizeof(_iocp) == sizeof(HANDLE), HANDLE_SIZE_ASSUMPTION_WRONG );
			Z_STATIC_ASSERT( sizeof(SPtr<Listener>*) == sizeof(ULONG_PTR), ULONG_PTR_SIZE_ASSUMPTION_WRONG );
			Z_STATIC_ASSERT( sizeof(SPtr<NetConn>*) == sizeof(ULONG_PTR), ULONG_PTR_SIZE_ASSUMPTION_WRONG2 );
			Z_STATIC_ASSERT( sizeof(uint32_t) == sizeof(DWORD), DWORD_SIZE_ASSUMPTION_WRONG );
			Z_STATIC_ASSERT( sizeof(intptr_t) == sizeof(LONG), LONG_SIZE_ASSUMPTION_WRONG );
			Z_STATIC_ASSERT( sizeof(SOCKADDR_STORAGE) == 128, SOCKADDR_STORAGE_SIZE_ASSUMPTION_WRONG );
			//Z_STATIC_ASSERT( sizeof(OVERLAPPED) == OVERLAPPED_SIZE, OVERLAPPED_SIZE_ASSUMPTION_WRONG2 );

			_iocp = NULL;

			_socket_accept = INVALID_SOCKET;

			for(size_t i = 0; i < _countof(_array_iocp_thread); ++i)
				_array_iocp_thread[i].setIOCP(this);

			_iocp_threads = min(1, _countof(_array_iocp_thread));

			_started = 0;
			_stopping = 0;

			_suspend_threads = 0;
			_suspended_threads = 0;
			_stop_threads = 0;

			_pool_sptr_netconn = NULL;

			_queue_pending_drop.create_pool(65536);
		}
		NetIOCP::~NetIOCP()
		{
			Z_ASSERT( !isStarted() || ( isStopping() && isFullyStopped() ) );

			if(_socket_accept != INVALID_SOCKET)
				closesocket(_socket_accept);

			if(_iocp)
				CloseHandle( (HANDLE)_iocp );

			if(_pool_sptr_netconn)
				_pool_sptr_netconn->dispose();

			//printf("net iocp disposed\n");
		}
		bool						NetIOCP::create(size_t max_conn)
		{
			bool succ = false;
			do
			{
				_pool_sptr_netconn = _pool_sptr_netconn->create(max_conn);
				if(!_pool_sptr_netconn)
					break;
				_iocp = (intptr_t)CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);
				if(!_iocp)
					break;

				succ = true;
			}while(false);
			if(!succ)
			{
				if(_pool_sptr_netconn)
				{
					_pool_sptr_netconn->dispose();
					_pool_sptr_netconn = NULL;
				}
				if(_iocp)
				{
					CloseHandle((HANDLE)_iocp);
					_iocp = NULL;
				}
				return false;
			}
			return true;
		}
		bool						NetIOCP::setListener(const SPtr<Listener>& listener)
		{
			if(_started)
				return false;

			if(_listener)
				return false;

			_listener = listener;

			Z_ASSERT(checkHeldBySPtr());

			_listener->_net_iocp = SPtr<NetIOCP>(this);

			if(!CreateIoCompletionPort( (HANDLE)listener->getSocket(), (HANDLE)_iocp, (ULONG_PTR)_listener.get(), 0 ))
				return false;

			return false;
		}
		bool						NetIOCP::setIOCPThreads(uint32_t threads)
		{
			if(_started)
				return false;

			_iocp_threads = min( max(1, threads), _countof(_array_iocp_thread) );

			return true;
		}
		uint32_t					NetIOCP::getIOCPThreads() const
		{
			return _iocp_threads;
		}
		bool						NetIOCP::start()
		{
			if(interlocked::compare_exchange(&_started, 1, 0))
				return false;

			for(uint32_t i = 0; i < _iocp_threads; ++i)
				_array_iocp_thread[i].start();

			return true;
		}
		void						NetIOCP::stop()
		{
			if(!_started)
				return;

			__suspendThreads();

			if(interlocked::exchange(&_stopping, 1))
				return;

			__makeAllDead();
			__resumeThreads();
		}
		bool						NetIOCP::isStarted() const
		{
			return !!_started;
		}
		bool						NetIOCP::isStopping() const
		{
			return !!_stopping;
		}
		bool						NetIOCP::isFullyStopped() const
		{
			if(!isStarted())
				return true;
			for(uint32_t i = 0; i < _iocp_threads; ++i)
				if(!_array_iocp_thread[i].isStopped())
					return false;
			return true;
		}
		size_t						NetIOCP::getMaxConnections() const
		{
			return _pool_sptr_netconn->get_max_blocks();
		}
		size_t						NetIOCP::getConnectionCount() const
		{
			return _pool_sptr_netconn->get_blocks_in_use();
		}
		bool						NetIOCP::manage(const SPtr<NetConn>& conn)
		{
			if(!conn)
				return false;
			if(conn->__iocp_internal.managed)
				return false;
			SPtr<NetConn>* p = __allocNetConnSPtr(conn);
			if(!p)
				return false;
			do
			{
				if(!CreateIoCompletionPort( (HANDLE)conn->getSocket(), (HANDLE)_iocp, (ULONG_PTR)p, 0 ))
					break;
				if(interlocked::compare_exchange(&p->get()->__iocp_internal.managed, 1, 0))
					break;
				p->get()->__iocp_internal.recving = 1;
				if(!issueRecv(p->get()))
					break;
				return true;
			}while(false);
			p->get()->__iocp_internal.recving = 0;
			__deallocNetConnSPtr(p);
			return false;
		}
		bool						NetIOCP::issueAccept()
		{
			if(_stopping)
				return false;

			if(_socket_accept != INVALID_SOCKET)
				return false;

			if(!_listener)
				return false;

			intptr_t s = NetSys::createSocket();

			if(s == INVALID_SOCKET)
				return false;

			Z_STATIC_ASSERT( sizeof(_socket_accept) == sizeof(uint32_t), WRONG_ASSUMPTION );
			if(interlocked::compare_exchange((uint32_t*)&_socket_accept, s, INVALID_SOCKET) != INVALID_SOCKET)
			{
				closesocket(s);
				return false;
			}

			memset(_overlapped_listen, 0, sizeof(_overlapped_listen));
			BOOL ret = AcceptEx(_listener->getSocket(), _socket_accept, _addresses, 0, sizeof(SOCKADDR_STORAGE) + 16, sizeof(SOCKADDR_STORAGE) + 16, NULL, (OVERLAPPED*)_overlapped_listen);
			if(ret == 0 && WSAGetLastError() != ERROR_IO_PENDING)
			{
				closesocket(_socket_accept);
				_socket_accept = INVALID_SOCKET;
				return false;
			}
			return true;
		}
		bool						NetIOCP::issueRecv(NetConn* c)
		{
			c->ping.tryPing(c);

			scoped_lock<mutex> o(c->critical_sync._mx);

			if(c->getSocket() == INVALID_SOCKET)
				return false;

			uint32_t len = c->__iocp_internal.buf_recv.len;
			WSABUF buf;
			buf.buf = c->__iocp_internal.buf_recv.buf + len;
			buf.len = sizeof(c->__iocp_internal.buf_recv.buf) - len;
			void* overlapped = (void*)c->__iocp_internal.overlapped_recv;
			memset(overlapped, 0, sizeof(c->__iocp_internal.overlapped_recv));
			DWORD bytes;
			DWORD flags = 0;
			int ret = WSARecv(c->getSocket(), &buf, 1, &bytes, &flags, (OVERLAPPED*)overlapped, NULL);
			if(ret == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
				return false;

			return true;
		}
		void						NetIOCP::iocp_worker()
		{
			// only the first thread will process
			bool process_cleanup = (W32Thread::getCurThreadID() == _array_iocp_thread[0].getThreadID());

			while(true)
			{
				if(_suspend_threads)
				{
					interlocked::increment(&_suspended_threads);
					while(_suspend_threads)
						Sleep(100);
					interlocked::decrement(&_suspended_threads);
				}
				uint32_t bytes;
				Z_STATIC_ASSERT( sizeof(bytes) == sizeof(DWORD), DWORD_SIZE_ASSUMPTION_WRONG );

				ULONG_PTR key = 0;
				OVERLAPPED* overlapped = NULL;
				BOOL ret = GetQueuedCompletionStatus((HANDLE)_iocp, (DWORD*)&bytes, &key, &overlapped, 100);
				if(key && overlapped)
				{
					if(_listener && key == (ULONG_PTR)_listener.get())
					{
						if(ret)
						{
							intptr_t s = _socket_accept;
							Z_STATIC_ASSERT( sizeof(_socket_accept) == sizeof(uint32_t), WRONG_ASSUMPTION );
							interlocked::exchange((uint32_t*)&_socket_accept, INVALID_SOCKET);

							if(s != INVALID_SOCKET)
							{
								setsockopt(s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&_listener->_socket_listen, sizeof(_listener->_socket_listen));

								if(_stopping || !_listener->_onConnected(s))
									closesocket(s);
							}

							issueAccept();
						}
						else
						{
							// indicate listening socket error...
							intptr_t s = _socket_accept;
							Z_STATIC_ASSERT( sizeof(_socket_accept) == sizeof(uint32_t), WRONG_ASSUMPTION );
							interlocked::exchange((uint32_t*)&_socket_accept, INVALID_SOCKET);

							if(s != INVALID_SOCKET)
								closesocket(s);

							issueAccept();
						}
					}
					else
					{
						SPtr<NetConn>* conn = (SPtr<NetConn>*)key;
						NetConn* c = conn->get();

						if(ret && bytes)
						{
							// some transfer done ...
							if(overlapped == (OVERLAPPED*)c->__iocp_internal.overlapped_recv)
							{
								// event of receiving
								int error = 0;

								c->__iocp_internal.buf_recv.len += bytes;

								uint32_t l = c->__iocp_internal.buf_recv.len;
								char* p = c->__iocp_internal.buf_recv.buf;					// msg head
								while(l >= 2)
								{
									uint32_t len = (uint32_t)(*(unsigned short*)p);
									if(len < offsetof(NetMsg, data) || len > (uint32_t)NetMsg::MAX_LENGTH)
									{
										// invalid msg length, fatal error, stop receiving...
										error = 1;

										c->__iocp_internal.recving = 0;

										c->setDead();
										post_death_process(conn);
										break;
									}
									else
									{
										if(len > l)
											break;
										// handling
										c->callback_redirect.onRecv(c, (NetMsg*)p);

										p += len;
										l -= len;
									}
								}
								if(!error && p != c->__iocp_internal.buf_recv.buf)
								{
									if(l)
										memmove(c->__iocp_internal.buf_recv.buf, p, l);
									c->__iocp_internal.buf_recv.len = l;
								}
								// as long as no error, this c is still valid because there is one instance of SPtr<NetConn> pointing to that c inside c, thus continue receiving
								if(!error)
								{
									if(!issueRecv(c))
									{
										c->__iocp_internal.recving = 0;

										c->setDead();
										post_death_process(conn);
									}
								}
							}
							else
							{
								//// done sent...
								((SENDOL*)overlapped)->dealloc();
								interlocked::decrement(&c->__iocp_internal.sending);
							}
						}
						else
						{
							// either socket closed or socket error
							if(overlapped == (OVERLAPPED*)c->__iocp_internal.overlapped_recv)
							{
								// event of recv
								c->__iocp_internal.recving = 0;

								c->setDead();
								post_death_process(conn);
							}
							else
							{
								//// event of send
								((SENDOL*)overlapped)->dealloc();
								interlocked::decrement(&c->__iocp_internal.sending);

								c->setDead();
								//post_death_process(conn);							// not necessary because the recving will detect this
							}
						}
					}
				}
				else
				{
					// timeout ...
					issueAccept();
				}

				if(process_cleanup)
				{
					while(!_queue_pending_drop.empty())
					{
						SPtr<NetConn>* conn = _queue_pending_drop.peep();
						NetConn* c = conn->get();
						if(c->__iocp_internal.recving || c->__iocp_internal.sending)
							break;

						SPtr<NetConn>* conn2;
						if(!_queue_pending_drop.deq(conn2))
							break;

						if(conn2 != conn)
						{
							_queue_pending_drop.enq(conn2);
							break;
						}

						interlocked::exchange(&c->__iocp_internal.managed, 0);

						__deallocNetConnSPtr(conn);
					}
				}
				if(_stopping)
				{
					if(!_pool_sptr_netconn->get_blocks_in_use())
						break;
				}
			}
		}
		void						NetIOCP::post_death_process(SPtr<NetConn>* conn)
		{
			// usage check, it must be after the connection being declared dead
			Z_ASSERT( conn->get()->dead() );
			if(interlocked::compare_exchange(&conn->get()->__iocp_internal.death_processed, 1, 0))
				return;
			_queue_pending_drop.enq(conn);
		}
		SPtr<NetConn>*				NetIOCP::__allocNetConnSPtr(const SPtr<NetConn>& conn)
		{
			SPtr<NetConn>* p = _pool_sptr_netconn->alloc();
			if(!p)
				return NULL;
			p->SPtr<NetConn>::SPtr(conn);
			return p;
		}
		void						NetIOCP::__deallocNetConnSPtr(SPtr<NetConn>* conn)
		{
			conn->SPtr<NetConn>::~SPtr();
			_pool_sptr_netconn->dealloc(conn);
		}
		void						NetIOCP::__suspendThreads()
		{
			_suspend_threads = 1;
			while(_suspended_threads != _iocp_threads && !isFullyStopped())
				Sleep(50);
		}
		void						NetIOCP::__makeAllDead()
		{
			_pool_sptr_netconn->clear_free_blocks();

			size_t c = 0;

			const SPtr<NetConn>* end_block = _pool_sptr_netconn->get_first_block() + _pool_sptr_netconn->get_max_blocks();
			for(const SPtr<NetConn>* b = _pool_sptr_netconn->get_first_block(); b != end_block; ++b)
			{
				// check if not zeroed out, if not skip
				char* ptr = (char*)b;
				bool empty = true;
				for(size_t i = 0; i < sizeof(SPtr<NetConn>); ++i)
					if(ptr[i])
					{
						empty = false;
						break;
					}

				if(empty)
					continue;

				b->get()->setDead();

				if(++c >= _pool_sptr_netconn->get_blocks_in_use())
					break;
			}
		}
		void						NetIOCP::__resumeThreads()
		{
			_suspend_threads = 0;
			while(_suspended_threads != 0 && !isFullyStopped())
				Sleep(50);
		}
	};
};
