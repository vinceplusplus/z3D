
#define WIN32_MEAN_AND_LEAN

#include <winsock2.h>

#include "Listener.h"

#include "NetSys.h"
#include "NetIOCP.h"

#include "../Core/lfqueue_inl.h"

namespace z3D
{
	namespace TCPFrm
	{
		Listener::Listener()
		{
			Z_STATIC_ASSERT( sizeof(_socket_listen) == sizeof(SOCKET), SOCKET_SIZE_ASSUMPTION_WRONG );

			_socket_listen = INVALID_SOCKET;
			_port = 0;

			_queuing_connections = false;
		}
		Listener::~Listener()
		{
			if(_socket_listen != INVALID_SOCKET)
				closesocket(_socket_listen);

			intptr_t s;
			while(_queued_sockets.deq(s))
				closesocket((SOCKET)s);
		}
		bool						Listener::create(uint32_t port)
		{
			bool succ = false;
			do
			{
				_socket_listen = NetSys::createSocket();
				if(_socket_listen == INVALID_SOCKET)
					break;

				sockaddr_in addr;
				addr.sin_addr.S_un.S_addr=ADDR_ANY;
				addr.sin_family=AF_INET;
				addr.sin_port=htons((unsigned short)port);
				if(bind(_socket_listen,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR)
					break;
				if(listen(_socket_listen, 500) == SOCKET_ERROR)
					break;

				_port = port;

				succ = true;
			}
			while(false);
			if(!succ)
			{
				if(_socket_listen)
				{
					closesocket(_socket_listen);
					_socket_listen = INVALID_SOCKET;
				}

				return false;
			}

			return true;
		}
		void						Listener::enableQueuingConnections()
		{
			_queuing_connections = true;
			_queued_sockets.create_pool(512);
		}
		bool						Listener::isQueuingConnections() const
		{
			return !!_queuing_connections;
		}
		bool						Listener::_onConnected(intptr_t s)
		{
			if(_queuing_connections)
				return _queued_sockets.enq(s);
			else
				return onConnected(s);
		}
		bool						Listener::onConnected(intptr_t s)
		{
			return false;
		}
		bool						Listener::dequeueConnection(intptr_t& s)
		{
			return _queued_sockets.deq(s);
		}
		intptr_t					Listener::getSocket() const
		{
			return _socket_listen;
		}
		uint32_t					Listener::getPort() const
		{
			return _port;
		}
		SPtr<NetIOCP>				Listener::getNetIOCP()
		{
			return _net_iocp.lock();
		}
	};
};

