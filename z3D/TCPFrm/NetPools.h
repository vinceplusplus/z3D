
#pragma once

#include "predecl.h"
#include "NetMsg.h"
#include "SendOL.h"
#include "BigMsg.h"

namespace z3D
{
	namespace TCPFrm
	{
		class NetPools
		{
		private:
			static mem_pool<NetMsg>*						_pool_netmsg;
			static mem_pool<SENDOL>*						_pool_sendol;
			static lfqueue<BigMsg::MEMINFO>					_queue_reusable_bigmsg_mem;
		private:
			static void										createPools(uint32_t max_netmsg, uint32_t max_send_overlapped, uint32_t max_reusable_bigmsg);					// not thread-safe
			static void										disposePools();																									// not thread-safe
		private:
			static NetMsg*									allocNetMsg();
			static void										deallocNetMsg(NetMsg* netmsg);
			static SENDOL*									allocSendOL();
			static void										deallocSendOL(SENDOL* sendol);
		public:
			friend class NetMsg;
			friend class SENDOL;
			friend class NetSys;
			friend class BigMsg;
		};
	};
};
