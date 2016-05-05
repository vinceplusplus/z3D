
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		// TODO: we should have something to store about the nth stack walk in debug build when allocating NetMsg, then later when shutting down the app, we can detect memory leak and spot the culprit from the previous stored stack walk
		class NetMsg: noncopyable
		{
		public:
			enum
			{
				MAX_LENGTH = 4096,
			};
		public:
			uint16_t				msg_len;
			uint16_t				opcode;											// opcode of 0xffff is reserved for encryption mechanism
			char					data[MAX_LENGTH - sizeof(uint16_t) - sizeof(uint16_t)];				// must make the whole NetMsg of size MAX_LENGTH
		private:
			NetMsg();																// disable constructor
			~NetMsg();																// disable destructor
		public:
			static NetMsg*			alloc();										// return uninitialized netmsg
			static NetMsg*			alloc(unsigned short opcode);					// return opcode & msg_len initialized netmsg
		public:
			void					dealloc();
		public:
			NetMsg*					clone() const;
		public:
			template<class T> friend class Z_ALIGNMENT_QUERY;
		};
	};
};
