
#include "MsgReader.h"

#include "NetMsg.h"

namespace z3D
{
	namespace TCPFrm
	{
		MsgReader::MsgReader(NetMsg* netmsg)
		{
			_netmsg = netmsg;
			_pos = offsetof(NetMsg, data);
		}
		bool				MsgReader::read(void* buffer, size_t size)
		{
			if(_pos + size > _netmsg->msg_len)
			{
				Z_ASSERT( false );
				return false;
			}
			memcpy(buffer, (char*)_netmsg + _pos, size);
			_pos += size;

			return true;
		}
		bool				MsgReader::read(mbs& data)
		{
			char* p = (char*)((char*)_netmsg + _pos);
			ptrdiff_t t = (ptrdiff_t)_netmsg->msg_len - _pos;
			ptrdiff_t c = t;
			while(c > 0)
			{
				if(!*p)
					break;

				c -= sizeof(*p);
				++p;
			}
			if(c <= 0)
				return false;

			data.resize( (t - c) / sizeof(*p) );
			read((void*)data.c_str(), (data.length() + 1) * sizeof(*p));

			return true;
		}
		bool				MsgReader::read(wcs& data)
		{
			wchar_t* p = (wchar_t*)((char*)_netmsg + _pos);
			ptrdiff_t t = (ptrdiff_t)_netmsg->msg_len - _pos;
			ptrdiff_t c = t;
			while(c > 0)
			{
				if(!*p)
					break;

				c -= sizeof(*p);
				++p;
			}
			if(c <= 0)
				return false;

			data.resize( (t - c) / sizeof(*p) );
			read((void*)data.c_str(), (data.length() + 1) * sizeof(*p));

			return true;
		}
		size_t				MsgReader::available() const
		{
			return (size_t)_netmsg->msg_len - _pos;
		}
	};
};
