
#pragma once

#include "predecl.h"

#include "BigMsg.h"

namespace z3D
{
	namespace TCPFrm
	{
		// *** do we need to implement some kind of get/setUserData() ?? the choices of type are size_t and SPtr<RefBase> *** //

		class NetConn: public RefBase
		{
		private:
			enum
			{
				OVERLAPPED_SIZE = sizeof(intptr_t) * 2 + sizeof(uint32_t) * 2 + sizeof(uintptr_t),
			};
		protected:
			volatile intptr_t			_socket;
		protected:
			volatile uint32_t			_dead;
		private:												// use only functors not wfunctors for performance reason
			functor2<bool, NetConn*, NetMsg*>
										_nmpu;
			functor1<void, NetConn*>	_ondie_cb;
		public:
			struct CRITICAL_SYNC
			{
			private:
				mutex						_mx;
			public:
				friend class NetConn;
				friend class NetIOCP;
			}							critical_sync;
		public:
			struct
			{
			private:
				volatile uint32_t			managed;
				volatile uint32_t			death_processed;
				unsigned char				overlapped_recv[OVERLAPPED_SIZE];
				struct
				{
					uint32_t					len;
					char						buf[4096];
				}							buf_recv;
				volatile uint32_t			recving;
				volatile uint32_t			sending;

				friend class NetIOCP;
				friend class NetConn;
			}							__iocp_internal;
		private:
			char						_sock_addr[64];
			char						_peer_addr[64];
			uint32_t					_sock_port;
			uint32_t					_peer_port;
		private:
			lfqueue<NetMsg*>			_queue_send;
		private:
			uint32_t					_batch_constraint;						// control each WSASend max bytes to send, should be less than path MTU, default 1400 (suitable for ethernet)
		private:
			ptrdiff_t					_defer_msg_proc;
			lfqueue<NetMsg*>			_queue_recv;
		private:
			ptrdiff_t					_enable_preview_recv_msgs;
			functor2<bool, NetConn*, NetMsg*>
										_preview_nmpu;
		private:
			uint32_t					_crypt_flags;
		private:
			uint64_t					_encrypt_key;
			uint64_t					_decrypt_key;
		public:
			struct PING
			{
			private:
				Counter						_counter;
				QStats<20>					_stats;
				size_t						_sent;
				Timer						_reply_delay_timer;
				REAL						_cur;								// for 32-bit (because of sizeof(REAL) == 4) or higher platforms, if its naturally aligned (according to its size), both read and write (just overwrite, NO increment or other stuff) are atomic

				static void					tryPing(NetConn* c);

				friend class NetIOCP;
				friend class NetConn;
			}							ping;
		private:
			mutex_fast					_mx_send_bigmsg;
		private:
			BigMsg						_bigmsg_recv;
			uint32_t					_bigmsg_recv_size;
			uint16_t					_bigmsg_recv_opcode;
		public:
			struct CALLBACK_REDIRECT
			{
				// so we now have per method's friend
				static void					onRecv(NetConn* conn, NetMsg* msg);
				static void					onDie(NetConn* conn);

				friend class NetIOCP;
			}							callback_redirect;
		public:
			NetConn();
			~NetConn();
		public:
			bool				setSocket(intptr_t socket);
			bool				reset();										// can be called to reset everything if it is completely detached from iocp, however, you should ensure that no other objects host a sptr to it and still assume its the previous connection
		private:
			void				_reset();
		public:
			bool				send(NetMsg* msg);								// ownership passed in. the method will be responsible for deallocating the msg even if the send method fails to send the msg. in normal case for winxp, it will not fail
		private:
			void				onRecv(NetMsg* msg);
			void				onDie();
		public:
			void				setNMPU(const functor2<bool, NetConn*, NetMsg*>& nmpu);
			void				setOnDieCallback(const functor1<void, NetConn*>& cb);
		public:
			bool				setDead();
			bool				dead() const;
		public:
			void				flush();										// flush the NetMsgs in the send queue
		public:
			intptr_t			getSocket() const;								// don't use this method, its reserved for iocp, the life time of the return value by this method is not guaranteed, as the connection declared dead, it will be closed as soon as possible
		public:
			const char*			getSockIP() const;
			const char*			getPeerIP() const;
			uint32_t			getSockPort() const;
			uint32_t			getPeerPort() const;
		public:
			uint32_t			getBatchConstraint() const;
			void				setBatchConstraint(uint32_t constraint);
		public:
			void				setDeferRecvMsgProc(bool defer);
			bool				getDeferRecvMsgProc() const;
		public:
			void				enablePreviewRecvMsgs(bool enable);				// useful only if defer recv msg proc ON
			bool				isPreviewRecvMsgsEnabled() const;
			void				setPreviewNMPU(const functor2<bool, NetConn*, NetMsg*>& preview_nmpu);
		public:
			void				procRecvMsgs();									// if setDeferRecvMsgProc(true), process the queued received messages using this method
		public:
			void				enableEncryption();								// must be called by client side
		public:
			REAL				getPing() const;								// the ping maybe not be correct, it can be faked by clients or whatsoever
		public:
			void				sendBigMsg(uint16_t opcode, const BigMsg& msg);
			bool				recvBigMsg(NetMsg* netmsg, BigMsg& msg);
		private:
			bool				_send(NetMsg* msg);
		public:
			friend struct PING;
			friend struct CALLBACK_REDIRECT;
		};
	};
};
