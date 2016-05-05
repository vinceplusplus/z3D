
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		class BigMsg: noncopyable
		{
		public:
			enum
			{
				GROW_SIZE = 4096,
			};
			class MEMINFO
			{
			public:
				void*			ptr;
				size_t			cap;
			};
		private:
			MEMINFO				_mem_info;
			size_t				_pos;
			size_t				_len;
		public:
			BigMsg();
			~BigMsg();
		private:
			void				__secure_cap(size_t cap);
		public:
			void				clear();										// free memory
		public:
			void				write(const void* buffer, size_t size);
			void				write(bool data);
			void				write(int8_t data);
			void				write(uint8_t data);
			void				write(int16_t data);
			void				write(uint16_t data);
			void				write(int32_t data);
			void				write(uint32_t data);
			void				write(int64_t data);
			void				write(uint64_t data);
			void				write(float data);
			void				write(double data);
			void				write(const char* data);
			void				write(const wchar_t* data);
			void				write(const mbs& data);
			void				write(const wcs& data);
		public:
			void				read(void* buffer, size_t size);
			void				read(bool& data);
			void				read(int8_t& data);
			void				read(uint8_t& data);
			void				read(int16_t& data);
			void				read(uint16_t& data);
			void				read(int32_t& data);
			void				read(uint32_t& data);
			void				read(int64_t& data);
			void				read(uint64_t& data);
			void				read(float& data);
			void				read(double& data);
			void				read(mbs& data);
			void				read(wcs& data);
		public:
			size_t				available() const;
			size_t				length() const;
			size_t				position() const;
			void*				pointer() const;
		public:
			void				seek(size_t pos);
		};
	};
};

