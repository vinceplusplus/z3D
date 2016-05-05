
#include <winsock2.h>
#include "ConnMaker.h"
#include "NetSys.h"

namespace z3D
{
	namespace TCPFrm
	{
		ConnMaker::ConnMaker()
		{
			_s = INVALID_SOCKET;
			_finished = 1;
			_connected = 0;
		}
		ConnMaker::~ConnMaker()
		{
			__result(false);
		}
		void				ConnMaker::connect(const char* host, uint32_t port)
		{
			__result(false);
			_finished = 0;
			_connected = 0;

			_s = NetSys::createSocket();
			if(_s == INVALID_SOCKET)
				__result(false);

			sockaddr_in addr;
			mbs ip=NetSys::resolve_1addr(host);
			if(!ip.size())
			{
				__result(false);
				return;
			}
			addr.sin_addr.S_un.S_addr=NetSys::inet_addr(ip);
			addr.sin_family=AF_INET;
			addr.sin_port=htons((unsigned short)port);

			__blocking(false);

			if(::connect(_s,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR)
			{
				if(WSAGetLastError()!=WSAEWOULDBLOCK)
				{
					__result(false);
					return;
				}
			}
			else
			{
				__result(true);
				return;
			}
		}
		void				ConnMaker::cancel()
		{
			__result(false);
		}
		bool				ConnMaker::finished()
		{
			if(_finished)
				return true;

			if(__writable())
			{
				__result(true);
				return true;
			}
			if(__unexpected())
			{
				__result(false);
				return true;
			}
			return false;
		}
		bool				ConnMaker::connected() const
		{
			return !!_connected;
		}
		bool				ConnMaker::takeSocket(intptr_t& s)
		{
			if(_s != INVALID_SOCKET)
			{
				s = _s;
				_s = INVALID_SOCKET;
				__result(false);
				return true;
			}
			return false;
		}
		void				ConnMaker::__blocking(bool blocking_mode)
		{
			if(_s == INVALID_SOCKET)
				return;
			unsigned long dummy = blocking_mode?0:1;
			ioctlsocket(_s, FIONBIO, &dummy);
		}
		void				ConnMaker::__result(bool connected)
		{
			if(_s != INVALID_SOCKET)
				__blocking(true);
			if(!connected && _s != INVALID_SOCKET)
			{
				shutdown(_s, SD_BOTH);
				closesocket(_s);
				_s = INVALID_SOCKET;
			}
			_finished = 1;
			_connected = connected?1:0;
		}
		bool				ConnMaker::__writable()
		{
			if(_s == INVALID_SOCKET)
				return false;
			fd_set set;
			FD_ZERO(&set);
			FD_SET(_s, &set);
			timeval t={0,0};

			::select(0, NULL, &set, NULL, &t);
			if(FD_ISSET(_s, &set))
				return true;
			return false;
		}
		bool				ConnMaker::__unexpected()
		{
			if(_s == INVALID_SOCKET)
				return true;
			fd_set set;
			FD_ZERO(&set);
			FD_SET(_s, &set);
			timeval t={0,0};

			::select(0, NULL, NULL, &set, &t);
			if(FD_ISSET(_s, &set))
				return true;
			return false;
		}
	};
};
