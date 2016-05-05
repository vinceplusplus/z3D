
#pragma once

#define WIN32_MEAN_AND_LEAN

#include "predecl.h"

#include <windows.h>

namespace z3D
{
	namespace TCPFrm
	{
		class SENDOL: noncopyable
		{
		public:
			OVERLAPPED				overlapped;
		public:
			NetConn*				c;
		public:
			NetMsg*					array_netmsg[32];
		private:
			SENDOL();														// disable constructor
			~SENDOL();														// disable destructor
		public:
			static SENDOL*			alloc();								// with overlapped memset 0, first element of array_netmsg NULL
		public:
			void					dealloc();								// will dealloc each element of array_netmsg up to that a NULL is found or the end of the array
		public:
			template<class T> friend class Z_ALIGNMENT_QUERY;
		};
	};
};
