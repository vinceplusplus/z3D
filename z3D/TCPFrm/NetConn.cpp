
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>

#undef min
#undef max

#include "NetConn.h"
#include "../Core/functor_inl.h"

#include "NetPools.h"

#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedCompareExchange64, _InterlockedIncrement, _InterlockedDecrement, _InterlockedExchangeAdd, _InterlockedExchange, __rdtsc)

#include "../Core/lfqueue_inl.h"

#include "MsgWriter.h"
#include "MsgReader.h"

#include "NetSys.h"

namespace z3D
{
	namespace TCPFrm
	{
		void						NetConn::PING::tryPing(NetConn* c)
		{
			if(!c->ping._sent)
			{
				if(c->ping._stats.count() < 10 || ((c->ping._counter.update(), true) && c->ping._counter.elapsedTime() >= 0.1f))
				{
					c->_send(NetMsg::alloc(0xfffe));
					c->ping._sent = true;
				}
			}
		}
		void						NetConn::CALLBACK_REDIRECT::onRecv(NetConn* conn, NetMsg* msg)
		{
			conn->onRecv(msg);
		}
		void						NetConn::CALLBACK_REDIRECT::onDie(NetConn* conn)
		{
			conn->onDie();
		}

		NetConn::NetConn()
		{
			Z_STATIC_ASSERT( sizeof(OVERLAPPED) == OVERLAPPED_SIZE, OVERLAPPED_SIZE_ASSUMPTION_WRONG );

			_socket = INVALID_SOCKET;

			_queue_send.create_pool(1024);
			_queue_recv.create_pool(1024);

			_batch_constraint = 1400;

			_defer_msg_proc = 0;

			_enable_preview_recv_msgs = 0;

			_reset();
		}
		NetConn::~NetConn()
		{
			reset();
		}
		bool				NetConn::setSocket(intptr_t socket)
		{
			scoped_lock<mutex> o(critical_sync._mx);

			if(!_dead || __iocp_internal.managed || _socket != INVALID_SOCKET)
				return false;
			if(socket == INVALID_SOCKET)
				return false;

			_socket = socket;

			_dead = 0;

			if(true)
			{
				sockaddr_in addr;
				int size;
				BYTE* no;

				size = sizeof(addr);
				getsockname(_socket, (sockaddr*)&addr, &size);
				no = (BYTE*)&addr.sin_addr.S_un.S_addr;
				sprintf_s(_sock_addr, "%u.%u.%u.%u", no[0], no[1], no[2], no[3]);
				_sock_port = ntohs(addr.sin_port);

				size = sizeof(addr);
				getpeername(_socket, (sockaddr*)&addr, &size);
				no = (BYTE*)&addr.sin_addr.S_un.S_addr;
				sprintf_s(_peer_addr, "%u.%u.%u.%u", no[0], no[1], no[2], no[3]);
				_peer_port = ntohs(addr.sin_port);
			}

			return true;
		}
		bool				NetConn::reset()
		{
			scoped_lock<mutex> o(critical_sync._mx);

			if(__iocp_internal.managed)
				return false;

			if(true)
			{
				NetMsg* msg;
				while(_queue_send.deq(msg))
					msg->dealloc();
				while(_queue_recv.deq(msg))
					msg->dealloc();
			}
			_queue_send.clear();
			_queue_recv.clear();

			if(_socket != INVALID_SOCKET)
			{
				closesocket(_socket);
				_socket = INVALID_SOCKET;
			}

			_reset();

			return true;
		}
		void				NetConn::_reset()
		{
			_dead = 1;

			__iocp_internal.managed = 0;
			__iocp_internal.death_processed = 0;
			__iocp_internal.buf_recv.len = 0;
			__iocp_internal.recving = 0;
			__iocp_internal.sending = 0;

			*_sock_addr = 0;
			*_peer_addr = 0;
			_sock_port = 0;
			_peer_port = 0;

			_crypt_flags = 0;

			_encrypt_key = 0;
			_decrypt_key = 0;

			ping._sent = 0;
			ping._cur = 0.0f;

			_bigmsg_recv_opcode = 0xffff;

			ping._stats.reset();
		}
		bool				NetConn::send(NetMsg* msg)
		{
			// 0xffff is reserved for encyprtion purposes
			Z_ASSERT( msg->opcode != 0xffff && msg->opcode != 0xfffe && msg->opcode != 0xfffd);

			return _send(msg);
		}
		void				NetConn::onRecv(NetMsg* msg)
		{
			if(_crypt_flags & 4)
			{
				NetSys::sym_decrypt((unsigned char*)&msg->opcode, sizeof(msg->opcode), _decrypt_key);
				NetSys::sym_decrypt((unsigned char*)msg->data, msg->msg_len - offsetof(NetMsg, data), _decrypt_key);
			}
			else if(msg->opcode == 0xffff)
			{
				if( !(_crypt_flags & 1) )
				{
					// server receive this msg

					char buffer[8];
					uint32_t olen = sizeof(buffer);
					if(!NetSys::rsa_decrypt(msg->data, msg->msg_len - offsetof(NetMsg, data), buffer, &olen))
						return;

					if(olen != sizeof(buffer))
						return;

					_decrypt_key = *(uint32_t*)buffer;
					_encrypt_key = *(uint32_t*)(buffer + 4);

					// make sure the following line is before send(NetMsg::alloc(0xffff)), otherwise, if there is a situation that this thread is executing "_crypt_flags |= 4" and another thread is flushing this NetConn and executing "_crypt_flags |= 2" then the value of _crypt_flags will be corrupted
					_crypt_flags |= 4;

					_send(NetMsg::alloc(0xffff));
				}
				else
				{
					// client receive this msg

					_crypt_flags |= 4;
				}

				return;
			}
			switch(msg->opcode)
			{
			case 0xfffe:
				{
					ping._reply_delay_timer.update();
					_send(NetMsg::alloc(0xfffd));
					// we are not going to flush here, flush() will stuff the delay time between this receipt and the actual flush
					return;
				}
				break;
			case 0xfffd:
				{
					ping._counter.update();
					// even if we don't do the subtraction, the client or whatsoever can still fake the ping somehow (e.g. delay more so to make it higher ping), so the ping is not reliable
					ping._stats.input(max(0.0f, ping._counter.elapsedTime() - ((msg->msg_len >= offsetof(NetMsg, data) + sizeof(REAL)) ? max(0.0f, *(REAL*)msg->data) : 0.0f)));
					ping._cur = (REAL)ping._stats.mean();
					ping._counter.start();
					ping._sent = false;
					return;
				}
				break;
			}
			if(_defer_msg_proc)
			{
				if(_enable_preview_recv_msgs)
					_preview_nmpu(this, msg);

				NetMsg* clone = msg->clone();
				if(!_queue_recv.enq(clone))
				{
					clone->dealloc();
					setDead();
				}
			}
			else if(_nmpu.valid())
				_nmpu(this, msg);
		}
		void				NetConn::onDie()
		{
			if(_ondie_cb.valid())
				_ondie_cb(this);
		}
		void				NetConn::setNMPU(const functor2<bool, NetConn*, NetMsg*>& nmpu)
		{
			_nmpu = nmpu;
		}
		void				NetConn::setOnDieCallback(const functor1<void, NetConn*>& cb)
		{
			_ondie_cb = cb;
		}
		bool				NetConn::setDead()
		{
			scoped_lock<mutex> o(critical_sync._mx);

			if(interlocked::exchange(&_dead, 1))
				return false;

			shutdown(_socket, SD_BOTH);

			if(_socket != INVALID_SOCKET)
			{
				closesocket(_socket);
				_socket = INVALID_SOCKET;
			}

			onDie();

			return true;
		}
		bool				NetConn::dead() const
		{
			return !!_dead;
		}
		void				NetConn::flush()
		{
			Z_STATIC_ASSERT( sizeof(OVERLAPPED) == sizeof(WSAOVERLAPPED), WSAOVERLAPPED_SIZE_ASSUMPTION_WRONG );

			scoped_lock<mutex> o(critical_sync._mx);
			if(_dead || !__iocp_internal.managed)
				return;

			if(_queue_send.empty())
				return;

			uint32_t est_msgs = _queue_send.size();

			uint32_t sent_msgs = 0;

			while(sent_msgs < est_msgs)				// avoid looping infinitely
			{
				WSABUF wsabuf[32];

				SENDOL* s = SENDOL::alloc();
				if(!s)
					return;

				s->c = this;
				size_t c = min(_countof(s->array_netmsg), _countof(wsabuf));
				size_t sum = 0;						// no. of msg this batch

				uint32_t bytes = 0;					// constraint each batch maximum bytes to send to path MTU

				NetMsg** msg_ptr = s->array_netmsg;
				WSABUF* wsabuf_ptr = wsabuf;

				for(size_t i = 0; i < c; ++i)
				{
					if(sum)							// make sure not the first msg
						if(_queue_send.peep()->msg_len + bytes > _batch_constraint)
							break;
					NetMsg* msg;
					if(!_queue_send.deq(msg))
						break;

					switch(msg->opcode)
					{
					case 0xfffe:
						ping._counter.start();
						break;
					case 0xfffd:
						msg->msg_len = offsetof(NetMsg, data) + sizeof(REAL);
						ping._reply_delay_timer.update();
						*(REAL*)msg->data = ping._reply_delay_timer.elapsedTime();
						break;
					}

					*msg_ptr = msg;
					wsabuf_ptr->buf = (char*)msg;
					wsabuf_ptr->len = msg->msg_len;

					if(_crypt_flags & 2)
					{
						// 2 separates to optimize data encrypt so that each 32-bit word in data lies in 32-bit boundary and one fetch is okay
						NetSys::sym_encrypt((unsigned char*)&msg->opcode, sizeof(msg->opcode), _encrypt_key);
						NetSys::sym_encrypt((unsigned char*)msg->data, msg->msg_len - offsetof(NetMsg, data), _encrypt_key);
					}
					else if(msg->opcode == 0xffff)
						_crypt_flags |= 2;

					++msg_ptr;
					++wsabuf_ptr;
					++sum;
					bytes += msg->msg_len;

					++sent_msgs;
					if(sent_msgs >= est_msgs)
						break;
				}
				if(sum == 0)
				{
					s->dealloc();
					return;
				}
				if(sum<_countof(s->array_netmsg))
					*msg_ptr = NULL;

				DWORD bytes_sent = 0;
				int ret = WSASend(_socket, wsabuf, (DWORD)sum, &bytes_sent, 0, (WSAOVERLAPPED*)s, NULL);
				int wsaerr;
				if(ret == SOCKET_ERROR && (wsaerr = WSAGetLastError()) != ERROR_IO_PENDING)
				{
					s->dealloc();

					setDead();
					return;
				}
				interlocked::increment(&__iocp_internal.sending);
			}
		}
		intptr_t			NetConn::getSocket() const
		{
			return _socket;
		}
		const char*			NetConn::getSockIP() const
		{
			return _sock_addr;
		}
		const char*			NetConn::getPeerIP() const
		{
			return _peer_addr;
		}
		uint32_t			NetConn::getSockPort() const
		{
			return _sock_port;
		}
		uint32_t			NetConn::getPeerPort() const
		{
			return _peer_port;
		}
		uint32_t			NetConn::getBatchConstraint() const
		{
			return _batch_constraint;
		}
		void				NetConn::setBatchConstraint(uint32_t constraint)
		{
			_batch_constraint = constraint;
		}
		void				NetConn::setDeferRecvMsgProc(bool defer)
		{
			_defer_msg_proc = defer;
		}
		bool				NetConn::getDeferRecvMsgProc() const
		{
			return !!_defer_msg_proc;
		}
		void				NetConn::enablePreviewRecvMsgs(bool enable)
		{
			_enable_preview_recv_msgs = enable;
		}
		bool				NetConn::isPreviewRecvMsgsEnabled() const
		{
			return !!_enable_preview_recv_msgs;
		}
		void				NetConn::setPreviewNMPU(const functor2<bool, NetConn*, NetMsg*>& preview_nmpu)
		{
			_preview_nmpu = preview_nmpu;
		}
		void				NetConn::procRecvMsgs()
		{
			if(!_nmpu.valid())
				return;

			uint32_t est_msgs = _queue_recv.size();
			NetMsg* msg;

			for(uint32_t i = est_msgs; i; --i)
			{
				if(!_queue_recv.deq(msg))
					break;
				_nmpu(this, msg);
				msg->dealloc();
			}
		}
		void				NetConn::enableEncryption()
		{
			Z_STATIC_ASSERT( sizeof(uint32_t) == sizeof(DWORD), DWORD_SIZE_ASSUMPTION_WRONG );
			if(interlocked::compare_exchange(&_crypt_flags, 1, 0))
				return;

			uint64_t key = __rdtsc();
			_encrypt_key = (uint32_t)(key & 0xffffffff);
			_decrypt_key = lrng::randSpecial2(key);

			MsgWriter writer(NetMsg::alloc(), 0xffff);

			char input[8];
			*(uint32_t*)input = (uint32_t)_encrypt_key;
			*(uint32_t*)(input + 4) = (uint32_t)_decrypt_key;

			char output[2048];

			uint32_t olen = NetSys::rsa_get_modulus_size();

			Z_ASSERT( sizeof(output) >= olen );
			Z_ASSERT( NetMsg::MAX_LENGTH - offsetof(NetMsg, data) >= olen );

			NetSys::rsa_encrypt(input, 8, output, olen);
			writer.write(output, olen);

			_send(writer.getMsg());
		}
		REAL				NetConn::getPing() const
		{
			return ping._cur;
		}
		void				NetConn::sendBigMsg(uint16_t opcode, const BigMsg& msg)
		{
			if(!msg.length())
				return;

			scoped_lock<mutex_fast> o(_mx_send_bigmsg);

			unsigned char* ptr = (unsigned char*)msg.pointer();
			size_t size = msg.length();
			ptrdiff_t first = 1;
			while(size)
			{
				NetMsg* m = NetMsg::alloc(opcode);
				MsgWriter writer(m);
				if(first)
					writer.write((uint32_t)size);
				first = 0;
				size_t l = min(size, (size_t)NetMsg::MAX_LENGTH - m->msg_len);
				writer.write(ptr, l);

				send(m);

				ptr += l;
				size -= l;
			}
		}
		bool				NetConn::recvBigMsg(NetMsg* netmsg, BigMsg& msg)
		{
			ptrdiff_t first = 0;
			if(_bigmsg_recv_opcode != netmsg->opcode)
				first = 1;

			MsgReader reader(netmsg);
			if(first)
			{
				_bigmsg_recv_opcode = netmsg->opcode;
				reader.read(_bigmsg_recv_size);

				_bigmsg_recv.clear();
			}
			_bigmsg_recv.write(((unsigned char*)netmsg) + (netmsg->msg_len - reader.available()), reader.available());
			if(_bigmsg_recv.length() == _bigmsg_recv_size)
			{
				msg.clear();
				// no way, just hack, its clearer
				Z_ASSERT( _bigmsg_recv.pointer() == *(void**)&_bigmsg_recv );
				memcpy(&msg, &_bigmsg_recv, sizeof(BigMsg));
				memset(&_bigmsg_recv, 0, sizeof(BigMsg));
				msg.seek(0);
				_bigmsg_recv_opcode = 0xffff;

				return true;
			}

			return false;
		}
		bool				NetConn::_send(NetMsg* msg)
		{
			_queue_send.enq(msg);

			return true;
		}
	};
};
