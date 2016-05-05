
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		class MsgWriter: noncopyable
		{
		private:
			NetMsg*				_netmsg;
		public:
			MsgWriter(NetMsg* netmsg, uint16_t opcode = 0);						// opcode == 0 means not changing the opcode inside
		public:
			void				write(const void* buffer, size_t size);
		public:
			template<class T>
			void				write(const T& data)			{	Z_STATIC_ASSERT( sizeof(SIMPLE_TYPE(data)) == sizeof(uint8_t), WRONG_TYPE );	write(&data, sizeof(data));	}
			void				write(const mbs& data);
			void				write(const wcs& data);
			void				write(const char* data);
			void				write(const wchar_t* data);
		private:
			uint8_t				SIMPLE_TYPE(const int8_t&);
			uint8_t				SIMPLE_TYPE(const uint8_t&);
			uint8_t				SIMPLE_TYPE(const int16_t&);
			uint8_t				SIMPLE_TYPE(const uint16_t&);
			uint8_t				SIMPLE_TYPE(const int32_t&);
			uint8_t				SIMPLE_TYPE(const uint32_t&);
			uint8_t				SIMPLE_TYPE(const int64_t&);
			uint8_t				SIMPLE_TYPE(const uint64_t&);
			uint8_t				SIMPLE_TYPE(const float&);
			uint8_t				SIMPLE_TYPE(const double&);
			uint8_t				SIMPLE_TYPE(const bool&);
			uint8_t				SIMPLE_TYPE(const Vec2&);
			uint8_t				SIMPLE_TYPE(const Vec3&);
			uint8_t				SIMPLE_TYPE(const Vec4&);
			uint8_t				SIMPLE_TYPE(const Mat2&);
			uint8_t				SIMPLE_TYPE(const Mat3&);
			uint8_t				SIMPLE_TYPE(const Mat4&);
			template<class T>
			uint16_t			SIMPLE_TYPE(T);
		public:
			NetMsg*				getMsg() const;
		};
	};
};
