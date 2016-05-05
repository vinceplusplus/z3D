
#include "MemUpdater.h"

namespace z3D
{
	namespace IO
	{
		MemUpdater::MemUpdater()
			: MemReader()
		{
		}
		MemUpdater::MemUpdater(void* buffer, size_t size)
			: MemReader(buffer, size)
		{
		}
		void		MemUpdater::write(const void* buffer, size_t size)
		{
			if(_offset + size > _size)
			{
				throw "MemUpdater::write(const void*, size_t): out of bound access";
				return;
			}
			memcpy(((char*)_buffer) + _offset, buffer, size);
			_offset += size;
		}
		void		MemUpdater::write(const mbs& data)
		{
			if(data.length())
				write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		void		MemUpdater::write(const wcs& data)
		{
			if(data.length())
				write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		void		MemUpdater::write(const char* data)
		{
			write(data, (strlen(data) + 1) * sizeof(data[0]));
		}
		void		MemUpdater::write(const wchar_t* data)
		{
			write(data, (wcslen(data) + 1) * sizeof(data[0]));
		}
	};
};

