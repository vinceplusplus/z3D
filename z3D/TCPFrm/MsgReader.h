
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		class MsgReader: noncopyable
		{
		private:
			NetMsg*				_netmsg;
			size_t				_pos;
		public:
			MsgReader(NetMsg* netmsg);
		public:
			bool				read(void* buffer, size_t size);
		public:
			template<class T>
			bool				read(T& data)				{	Z_STATIC_ASSERT( sizeof(SIMPLE_TYPE(data)) == sizeof(uint8_t), WRONG_TYPE );	return read(&data, sizeof(data));	}
			bool				read(mbs& data);
			bool				read(wcs& data);
		public:
			template<class T>
			T					read()						{	T data;	read(data);	return data;	}
		private:
			uint8_t				SIMPLE_TYPE(int8_t&);
			uint8_t				SIMPLE_TYPE(uint8_t&);
			uint8_t				SIMPLE_TYPE(int16_t&);
			uint8_t				SIMPLE_TYPE(uint16_t&);
			uint8_t				SIMPLE_TYPE(int32_t&);
			uint8_t				SIMPLE_TYPE(uint32_t&);
			uint8_t				SIMPLE_TYPE(int64_t&);
			uint8_t				SIMPLE_TYPE(uint64_t&);
			uint8_t				SIMPLE_TYPE(bool&);
			uint8_t				SIMPLE_TYPE(float&);
			uint8_t				SIMPLE_TYPE(double&);
			uint8_t				SIMPLE_TYPE(Vec2&);
			uint8_t				SIMPLE_TYPE(Vec3&);
			uint8_t				SIMPLE_TYPE(Vec4&);
			uint8_t				SIMPLE_TYPE(Mat2&);
			uint8_t				SIMPLE_TYPE(Mat3&);
			uint8_t				SIMPLE_TYPE(Mat4&);
			template<class T>
			uint16_t			SIMPLE_TYPE(T);
		public:
			size_t				available() const;
		};
	};
};
