
#include "FileReader.h"

namespace z3D
{
	namespace IO
	{
		FileReader::FileReader()
		{}
		FileReader::~FileReader()
		{}
		void								FileReader::read(mbs& data)
		{
			data.clear();
			uint8_t ch;
			while((ch = read<uint8_t>()) != 0)
				data += (char)ch;
		}
		void								FileReader::read(wcs& data)
		{
			data.clear();
			uint16_t ch;
			while((ch = read<uint16_t>()) != 0)
				data += (wchar_t)ch;
		}

		MemFileReader::MemFileReader(const SMem& sm)
			: _sm(sm), _reader(sm.get(), sm.size())
		{
		}
		MemFileReader::~MemFileReader()
		{
		}
		size_t								MemFileReader::size() const
		{
			return _reader.size();
		}
		size_t								MemFileReader::offset() const
		{
			return _reader.offset();
		}
		size_t								MemFileReader::available() const
		{
			return _reader.available();
		}
		void								MemFileReader::seek(size_t offset)
		{
			_reader.seek(offset);
		}
		void								MemFileReader::seekRelative(ptrdiff_t offset)
		{
			_reader.seekRelative(offset);
		}
		void								MemFileReader::read(void* buffer, size_t size)
		{
			_reader.read(buffer, size);
		}

		SubFileReader::SubFileReader(const SPtr<FileReader>& fr, size_t offset, size_t length)
			: _fr(fr), _offset(offset), _length(length)
		{
			_fr->seek(_offset);
		}
		SubFileReader::~SubFileReader()
		{
		}
		size_t								SubFileReader::size() const
		{
			return _length;
		}
		size_t								SubFileReader::offset() const
		{
			return _fr->offset() - _offset;
		}
		size_t								SubFileReader::available() const
		{
			return size() - offset();
		}
		void								SubFileReader::seek(size_t offset)
		{
			_fr->seek(_offset + offset);
		}
		void								SubFileReader::seekRelative(ptrdiff_t offset)
		{
			_fr->seek(_offset + this->offset() + offset);
		}
		void								SubFileReader::read(void* buffer, size_t size)
		{
			_fr->read(buffer, size);
		}
	};
};

