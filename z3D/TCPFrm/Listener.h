
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		class Listener: public RefBase
		{
		private:
			intptr_t					_socket_listen;
			uint32_t					_port;
		private:
			WPtr<NetIOCP>				_net_iocp;
		private:
			size_t						_queuing_connections;				// boolean
			lfqueue<intptr_t>			_queued_sockets;					// start to change naming convention
		public:
			Listener();
		protected:
			~Listener();
		public:
			bool						create(uint32_t port);
		public:
			void						enableQueuingConnections();
			bool						isQueuingConnections() const;
		private:
			bool						_onConnected(intptr_t s);
		protected:
			virtual bool				onConnected(intptr_t s);
		public:
			bool						dequeueConnection(intptr_t& s);
		public:
			intptr_t					getSocket() const;
			uint32_t					getPort() const;
		public:
			SPtr<NetIOCP>				getNetIOCP();
		public:
			friend class				NetIOCP;
		};
	};
};
