
#include "BigMsg.h"
#include "NetPools.h"
#include "../Core/lfqueue_inl.h"

namespace z3D
{
	namespace TCPFrm
	{
		BigMsg::BigMsg()
		{
			memset(this, 0, sizeof(*this));
		}
		BigMsg::~BigMsg()
		{
			clear();
		}
		void				BigMsg::__secure_cap(size_t cap)
		{
			if(!cap)
				return;
			if(_mem_info.cap >= cap)
				return;

			if(!_mem_info.ptr)
				if(!NetPools::_queue_reusable_bigmsg_mem.deq(_mem_info))
				{
					_mem_info.ptr = new char[GROW_SIZE];
					Z_ASSERT(_mem_info.ptr != 0);
					_mem_info.cap = GROW_SIZE;
				}
			if(cap > _mem_info.cap)
			{
				size_t newsize = (cap + GROW_SIZE - 1) / GROW_SIZE * GROW_SIZE;
				void* newptr = realloc(_mem_info.ptr, newsize);
				if(newptr)
				{
					_mem_info.ptr = newptr;
					_mem_info.cap = newsize;
				}
				else
				{
					newptr = new char[newsize];
					Z_ASSERT(newptr != 0);
					memcpy(newptr, _mem_info.ptr, _mem_info.cap);
					delete[] (char*)_mem_info.ptr;
					_mem_info.ptr = newptr;
					_mem_info.cap = newsize;
				}
			}
		}
		void				BigMsg::clear()
		{
			if(!NetPools::_queue_reusable_bigmsg_mem.enq(_mem_info))
				delete[] (char*)_mem_info.ptr;
			memset(this, 0, sizeof(*this));
		}
		void				BigMsg::write(const void* buffer, size_t size)
		{
			__secure_cap(_pos + size);
			memcpy((unsigned char*)_mem_info.ptr + _pos, buffer, size);

			_pos += size;
			_len = max(_pos, _len);
		}
		void				BigMsg::write(bool data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(int8_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(uint8_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(int16_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(uint16_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(int32_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(uint32_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(int64_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(uint64_t data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(float data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(double data)
		{
			write(&data, sizeof(data));
		}
		void				BigMsg::write(const char* data)
		{
			size_t s = strlen(data);
			write(data, (s+1)*sizeof(*data));
		}
		void				BigMsg::write(const wchar_t* data)
		{
			size_t s = wcslen(data);
			write(data, (s+1)*sizeof(*data));
		}
		void				BigMsg::write(const mbs& data)
		{
			write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		void				BigMsg::write(const wcs& data)
		{
			write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		void				BigMsg::read(void* buffer, size_t size)
		{
			if(_pos + size > _len)
				return;
			memcpy(buffer, (char*)_mem_info.ptr + _pos, size);
			_pos += size;
		}
		void				BigMsg::read(bool& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(int8_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(uint8_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(int16_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(uint16_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(int32_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(uint32_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(int64_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(uint64_t& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(float& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(double& data)
		{
			read(&data, sizeof(data));
		}
		void				BigMsg::read(mbs& data)
		{
			char* p = (char*)((char*)_mem_info.ptr + _pos);
			ptrdiff_t t = (ptrdiff_t)_len - _pos;
			ptrdiff_t c = t;
			while(c > 0)
			{
				if(!*p)
					break;

				c -= sizeof(*p);
				++p;
			}
			if(c <= 0)
				return;

			data.resize( (t-c)/sizeof(*p) );
			read((void*)data.c_str(), (data.length()+1) * sizeof(*p));
		}
		void				BigMsg::read(wcs& data)
		{
			wchar_t* p = (wchar_t*)((char*)_mem_info.ptr + _pos);
			ptrdiff_t t = (ptrdiff_t)_len - _pos;
			ptrdiff_t c = t;
			while(c > 0)
			{
				if(!*p)
					break;

				c -= sizeof(*p);
				++p;
			}
			if(c <= 0)
				return;

			data.resize( (t-c)/sizeof(*p) );
			read((void*)data.c_str(), (data.length()+1) * sizeof(*p));
		}
		size_t				BigMsg::available() const
		{
			return _len - (uint32_t)_pos;
		}
		size_t				BigMsg::length() const
		{
			return _len;
		}
		size_t				BigMsg::position() const
		{
			return _pos;
		}
		void*				BigMsg::pointer() const
		{
			return _mem_info.ptr;
		}
		void				BigMsg::seek(size_t pos)
		{
			_pos = min(pos, _len);
		}
	};
};

