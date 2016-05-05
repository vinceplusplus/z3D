
#include "MsgWriter.h"

#include "NetMsg.h"

namespace z3D
{
	namespace TCPFrm
	{
		MsgWriter::MsgWriter(NetMsg* netmsg, uint16_t opcode)
		{
			_netmsg = netmsg;
			_netmsg->msg_len = offsetof(NetMsg, data);
			if(opcode)
				_netmsg->opcode = opcode;
		}
		void				MsgWriter::write(const void* buffer, size_t size)
		{
			if(_netmsg->msg_len + size > NetMsg::MAX_LENGTH)
			{
				Z_VERIFY( false );
				return;
			}
			memcpy((char*)_netmsg + _netmsg->msg_len, buffer, size);
			_netmsg->msg_len += (uint16_t)size;
		}
		void				MsgWriter::write(const char* data)
		{
			size_t s = strlen(data);
			write(data, (s + 1) * sizeof(*data));
		}
		void				MsgWriter::write(const wchar_t* data)
		{
			size_t s = wcslen(data);
			write(data, (s + 1) * sizeof(*data));
		}
		void				MsgWriter::write(const mbs& data)
		{
			write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		void				MsgWriter::write(const wcs& data)
		{
			write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		NetMsg*				MsgWriter::getMsg() const
		{
			return _netmsg;
		}
	};
};
