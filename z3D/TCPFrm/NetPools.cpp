
#include "NetPools.h"

#include "../Core/mem_pool_inl.h"
#include "../Core/lfqueue_inl.h"

namespace z3D
{
	namespace TCPFrm
	{
		void						NetPools::createPools(uint32_t max_netmsg, uint32_t max_send_overlapped, uint32_t max_reusable_bigmsg)
		{
			if(!_pool_netmsg)
				_pool_netmsg = _pool_netmsg->create(max_netmsg);
			if(!_pool_sendol)
				_pool_sendol = _pool_sendol->create(max_send_overlapped);
			_queue_reusable_bigmsg_mem.create_pool(max_reusable_bigmsg);
		}
		void						NetPools::disposePools()
		{
			if(_pool_netmsg)
			{
				_pool_netmsg->dispose();
				_pool_netmsg = NULL;
			}
			if(_pool_sendol)
			{
				_pool_sendol->dispose();
				_pool_sendol = NULL;
			}
			while(!_queue_reusable_bigmsg_mem.empty())
			{
				BigMsg::MEMINFO info;
				if(!_queue_reusable_bigmsg_mem.deq(info))
					break;
				free(info.ptr);
			}
		}
		NetMsg*									NetPools::allocNetMsg()
		{
			// initialize by NetSys::createPools(...);
			Z_ASSERT( _pool_netmsg );

			NetMsg* p = _pool_netmsg->alloc();
			if(!p)
				p = (NetMsg*)mem_alloc::__aligned_alloc(sizeof(NetMsg), Z_GET_ALIGNMENT<NetMsg>::value);

			return p;
		}
		void									NetPools::deallocNetMsg(NetMsg* netmsg)
		{
			if(!_pool_netmsg->owns(netmsg))
			{
				mem_alloc::__aligned_free(netmsg);
				return;
			}
			_pool_netmsg->dealloc(netmsg);
		}
		SENDOL*									NetPools::allocSendOL()
		{
			// initialize by NetSys::createPools(...);
			Z_ASSERT( _pool_sendol );

			SENDOL* p = _pool_sendol->alloc();
			if(!p)
				p = (SENDOL*)mem_alloc::__aligned_alloc(sizeof(SENDOL), Z_GET_ALIGNMENT<SENDOL>::value);

			return p;
		}
		void									NetPools::deallocSendOL(SENDOL* sendol)
		{
			if(!_pool_sendol->owns(sendol))
			{
				mem_alloc::__aligned_free(sendol);
				return;
			}
			_pool_sendol->dealloc(sendol);
		}
	};
};
