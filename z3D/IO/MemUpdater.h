
#pragma once

#include "predecl.h"
#include "MemReader.h"

namespace z3D
{
	namespace IO
	{
		// buffer ownership is NEVER passed in, safe for memberwise copy
		class MemUpdater : public MemReader
		{
		public:
			MemUpdater();
			MemUpdater(void* buffer, size_t size);
		public:
			void					write(const void* buffer, size_t size);
		public:
			template<class T>
			void					write(const T& data)			{	Z_STATIC_ASSERT( sizeof(POD_TYPE(data)) == sizeof(uint8_t), WRONG_TYPE );	write(&data, sizeof(data));	}
			void					write(const mbs& data);
			void					write(const wcs& data);
			void					write(const char* data);
			void					write(const wchar_t* data);
		private:
			uint8_t					POD_TYPE(const int8_t&);
			uint8_t					POD_TYPE(const uint8_t&);
			uint8_t					POD_TYPE(const int16_t&);
			uint8_t					POD_TYPE(const uint16_t&);
			uint8_t					POD_TYPE(const int32_t&);
			uint8_t					POD_TYPE(const uint32_t&);
			uint8_t					POD_TYPE(const int64_t&);
			uint8_t					POD_TYPE(const uint64_t&);
			uint8_t					POD_TYPE(const float&);
			uint8_t					POD_TYPE(const double&);
			uint8_t					POD_TYPE(const bool&);
			template<class T>
			uint16_t				POD_TYPE(T);
		};
	};
};

