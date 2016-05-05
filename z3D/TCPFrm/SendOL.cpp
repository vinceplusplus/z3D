
#include "SendOL.h"
#include "NetPools.h"

namespace z3D
{
	namespace TCPFrm
	{
		SENDOL*					SENDOL::alloc()
		{
			SENDOL* s = NetPools::allocSendOL();

			Z_STATIC_ASSERT( sizeof(s->array_netmsg) > 0, ARRAY_NETMSG_SIZE_ASSUMPTION_WRONG );

			memset(&s->overlapped, 0, sizeof(s->overlapped));
			s->array_netmsg[0] = NULL;

			return s;
		}
		void					SENDOL::dealloc()
		{
			NetMsg** end = array_netmsg + _countof(array_netmsg);
			for(NetMsg** p = array_netmsg; p != end; ++p)
			{
				NetMsg* msg = *p;
				if(!msg)
					break;
				msg->dealloc();
			}

			NetPools::deallocSendOL(this);
		}
	};
};
