
#pragma once

#include "predecl.h"

#include "../Core/fast_string.h"

namespace z3D
{
	namespace IO
	{
		// buffer ownership is NEVER passed in, safe for memberwise copy
		class MemReader
		{
		protected:
			const void*				_buffer;
			size_t					_offset;
			size_t					_size;
		public:
			MemReader();
			MemReader(const void* buffer, size_t size);
		public:
			const void*				buffer() const;
			size_t					offset() const;
			size_t					size() const;
		public:
			void					seek(size_t offset);
			void					seekRelative(ptrdiff_t offset);
			size_t					available() const;
		public:
			void					read(void* buffer, size_t size);
		public:
			template<class T>
			void					read(T& data)				{	Z_STATIC_ASSERT( sizeof(POD_TYPE(data)) == sizeof(uint8_t), WRONG_TYPE );	read(&data, sizeof(data));	}
			void					read(mbs& data);
			void					read(wcs& data);
		public:
			template<class T>
			T						read()						{	T data;	read(data);	return data;	}
		private:
			uint8_t					POD_TYPE(int8_t&);
			uint8_t					POD_TYPE(uint8_t&);
			uint8_t					POD_TYPE(int16_t&);
			uint8_t					POD_TYPE(uint16_t&);
			uint8_t					POD_TYPE(int32_t&);
			uint8_t					POD_TYPE(uint32_t&);
			uint8_t					POD_TYPE(int64_t&);
			uint8_t					POD_TYPE(uint64_t&);
			uint8_t					POD_TYPE(bool&);
			uint8_t					POD_TYPE(float&);
			uint8_t					POD_TYPE(double&);
			template<class T>
			uint16_t				POD_TYPE(T);
		};
	};
};

