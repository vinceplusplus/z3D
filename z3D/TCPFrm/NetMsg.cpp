
#include "NetMsg.h"
#include "NetPools.h"

namespace z3D
{
	namespace TCPFrm
	{
		NetMsg*					NetMsg::alloc()
		{
			Z_STATIC_ASSERT( sizeof(NetMsg) == NetMsg::MAX_LENGTH, NOT_OF_ASSUMED_SIZE );

			return NetPools::allocNetMsg();
		}
		NetMsg*					NetMsg::alloc(unsigned short opcode)
		{
			NetMsg* netmsg = alloc();
			netmsg->opcode = opcode;
			netmsg->msg_len = offsetof(NetMsg, data);

			return netmsg;
		}
		void					NetMsg::dealloc()
		{
			NetPools::deallocNetMsg(this);
		}
		NetMsg*					NetMsg::clone() const
		{
			NetMsg* netmsg = alloc();
			memcpy(netmsg, this, msg_len);
			return netmsg;
		}
	};
};
