
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		// not thread-safe
		class ConnMaker: noncopyable
		{
		private:
			intptr_t			_s;
			uint32_t			_finished;
			uint32_t			_connected;
		public:
			ConnMaker();
			~ConnMaker();
		public:
			void				connect(const char* host, uint32_t port);				// step 1
			bool				finished();												// step 2, keep querying the status
			bool				connected() const;										// step 3, once finished, check if the connection is made successfully
		public:
			bool				takeSocket(intptr_t& s);								// step 4, take the socket ownership, new connection making starts with step 1 again
		public:
			void				cancel();												// cancel can be called anytime so that after this call, the status is finished and the connection isn't made successfully
		private:
			void				__blocking(bool blocking_mode);
			void				__result(bool connected);
			bool				__writable();
			bool				__unexpected();
		};
	};
};
