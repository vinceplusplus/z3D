
#include "MemWriter.h"

#include "../Core/SMem.h"

namespace z3D
{
	namespace IO
	{
		MemWriter::MemWriter()
		{
			_offset = _size = _length = 0;
			__allocBlock();
			seek(0);
		}
		MemWriter::~MemWriter()
		{
			for(size_t i = 0; i < _block.size(); i++)
			{
				const void* buffer = _block[i]->buffer();
				if(buffer != _base_buffer)
					delete[] (char*)buffer;
				delete _block[i];
			}
			_block.clear();
		}
		void					MemWriter::__allocBlock()
		{
			if(!_block.size())
				_block.push_back(new MemUpdater(_base_buffer, BLOCKSIZE));
			else
				_block.push_back(new MemUpdater(new char[BLOCKSIZE], BLOCKSIZE));
			_size += BLOCKSIZE;
		}
		size_t					MemWriter::offset() const
		{
			return _offset;
		}
		size_t					MemWriter::length() const
		{
			return _length;
		}
		void					MemWriter::clear()
		{
			_length = 0;
			seek(0);
		}
		void					MemWriter::seek(size_t offset)
		{
			_offset = offset;
			_offset = min(max((size_t)0, _offset), _length);
			_active = _block[min(_block.size() - 1, _offset / BLOCKSIZE)];
			_active->seek(_offset % BLOCKSIZE);
		}
		void					MemWriter::seekRelative(ptrdiff_t offset)
		{
			seek((size_t)(_offset + offset));
		}
		void					MemWriter::read(void* buffer, size_t size)
		{
			if(_offset + size > _length)
			{
				memset(buffer, 0, size);
				return;
			}
			size_t needread = min(_active->available(), size);
			size_t remaining = max((ptrdiff_t)(size-_active->available()), (ptrdiff_t)0);
			_active->read(buffer, needread);
			seekRelative(needread);
			if(remaining > 0)
				read((char*)buffer + needread, remaining);
		}
		void					MemWriter::read(mbs& data)
		{
			data.clear();
			char ch;
			while((ch = read<uint8_t>()) != 0)
				data += ch;
		}
		void					MemWriter::read(wcs& data)
		{
			data.clear();
			wchar_t ch;
			while((ch = read<uint16_t>()) != 0)
				data += ch;
		}
		void					MemWriter::write(const void* buffer, size_t size)
		{
			if(_offset + size > _size)
				__allocBlock();
			seekRelative(0);
			size_t needwrite = min(_active->available(), size);
			ptrdiff_t remaining = max((ptrdiff_t)(size - _active->available()), (ptrdiff_t)0);
			_active->write(buffer, needwrite);
			_length += (size_t)max((ptrdiff_t)(needwrite - (_length - _offset)), (ptrdiff_t)0);
			seekRelative(needwrite);
			if(remaining > 0)
				write((char*)buffer + needwrite, remaining);
		}
		void					MemWriter::write(const SMem& sm)
		{
			write(sm.get(), sm.size());
		}
		void					MemWriter::write(const mbs& data)
		{
			write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		void					MemWriter::write(const wcs& data)
		{
			write(data.c_str(), (data.length() + 1) * sizeof(data[0]));
		}
		void					MemWriter::write(const char* data)
		{
			write(data, (strlen(data) + 1) * sizeof(data[0]));
		}
		void					MemWriter::write(const wchar_t* data)
		{
			write(data, (wcslen(data) + 1) * sizeof(data[0]));
		}
		SMem					MemWriter::retrieveAll()
		{
			class dealloc_func
			{
			public:
				static void exec(void* p)
				{
					delete[] (char*)p;
				}
			};
			SMem sm = SMem::create(new char[SMem::getRequiredSize(length())], length(), &dealloc_func::exec);
			size_t old_offset = offset();
			seek(0);
			read(sm.get(), sm.size());
			seek(old_offset);

			return sm;
		}
	};
};