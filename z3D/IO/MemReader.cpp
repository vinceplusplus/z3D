
#include "MemReader.h"

namespace z3D
{
	namespace IO
	{
		MemReader::MemReader()
			: _buffer(0), _offset(0), _size(0)
		{
		}
		MemReader::MemReader(const void* buffer, size_t size)
			: _buffer(buffer), _size(size), _offset(0)
		{
		}
		const void*				MemReader::buffer() const
		{
			return _buffer;
		}
		size_t					MemReader::offset() const
		{
			return _offset;
		}
		size_t					MemReader::size() const
		{
			return _size;
		}
		void					MemReader::seek(size_t offset)
		{
			_offset = offset;
			_offset = min(max((size_t)0, _offset), _size);
		}
		void					MemReader::seekRelative(ptrdiff_t offset)
		{
			seek(_offset + offset);
		}
		size_t					MemReader::available() const
		{
			return _size - _offset;
		}
		void					MemReader::read(void* buffer, size_t size)
		{
			if(_offset + size > _size)
			{
				Z_VERIFY( false );
				return;
			}
			memcpy(buffer, ((char*)_buffer) + _offset, size);
			_offset += size;
		}
		void					MemReader::read(mbs& data)
		{
			data.clear();
			uint8_t ch;
			while((ch = read<uint8_t>()) != 0)
				data += (char)ch;
		}
		void					MemReader::read(wcs& data)
		{
			data.clear();
			uint16_t ch;
			while((ch = read<uint16_t>()) != 0)
				data += (wchar_t)ch;
		}
	};
};

