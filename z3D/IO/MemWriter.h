
#pragma once

#include "predecl.h"
#include "MemUpdater.h"

namespace z3D
{
	namespace IO
	{
		class MemWriter : public noncopyable
		{
		private:
			enum
			{
				BLOCKSIZE = 4096,
			};
		private:
			vector<MemUpdater*>		_block;
			size_t					_offset;
			size_t					_size;
			size_t					_length;
			MemUpdater*				_active;
			char					_base_buffer[BLOCKSIZE];
		public:
			MemWriter();
			~MemWriter();
		private:
			void					__allocBlock();
		public:
			size_t					offset() const;
			size_t					length() const;
		public:
			void					clear();
		public:
			void					seek(size_t offset);
			void					seekRelative(ptrdiff_t offset);
		public:
			void					read(void* buffer, size_t size);
		public:
			template<class T>
			void					read(T& data)					{	Z_STATIC_ASSERT( sizeof(POD_READ_TYPE(data)) == sizeof(uint8_t), WRONG_TYPE );	read(&data, sizeof(data));	}
			void					read(mbs& data);
			void					read(wcs& data);
		public:
			template<class T>
			T						read()							{	T data;	read(data);	return data;	}
		public:
			void					write(const void* buffer, size_t size);
			void					write(const SMem& sm);
		public:
			template<class T>
			void					write(const T& data)			{	Z_STATIC_ASSERT( sizeof(POD_WRITE_TYPE(data)) == sizeof(uint8_t), WRONG_TYPE );	write(&data, sizeof(data));	}
			void					write(const mbs& data);
			void					write(const wcs& data);
			void					write(const char* data);
			void					write(const wchar_t* data);
		public:
			SMem					retrieveAll();
		private:
			uint8_t					POD_READ_TYPE(int8_t&);
			uint8_t					POD_READ_TYPE(uint8_t&);
			uint8_t					POD_READ_TYPE(int16_t&);
			uint8_t					POD_READ_TYPE(uint16_t&);
			uint8_t					POD_READ_TYPE(int32_t&);
			uint8_t					POD_READ_TYPE(uint32_t&);
			uint8_t					POD_READ_TYPE(int64_t&);
			uint8_t					POD_READ_TYPE(uint64_t&);
			uint8_t					POD_READ_TYPE(bool&);
			uint8_t					POD_READ_TYPE(float&);
			uint8_t					POD_READ_TYPE(double&);
			uint8_t					POD_WRITE_TYPE(const int8_t&);
			uint8_t					POD_WRITE_TYPE(const uint8_t&);
			uint8_t					POD_WRITE_TYPE(const int16_t&);
			uint8_t					POD_WRITE_TYPE(const uint16_t&);
			uint8_t					POD_WRITE_TYPE(const int32_t&);
			uint8_t					POD_WRITE_TYPE(const uint32_t&);
			uint8_t					POD_WRITE_TYPE(const int64_t&);
			uint8_t					POD_WRITE_TYPE(const uint64_t&);
			uint8_t					POD_WRITE_TYPE(const bool&);
			uint8_t					POD_WRITE_TYPE(const float&);
			uint8_t					POD_WRITE_TYPE(const double&);
			template<class T>
			uint16_t				POD_READ_TYPE(T);
			template<class T>
			uint16_t				POD_WRITE_TYPE(T);
		};
	};
};

