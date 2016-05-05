
#pragma once

#include "predecl.h"

#include "../Core/ResBase.h"
#include "../Core/SMem.h"

#include "MemReader.h"

namespace z3D
{
	namespace IO
	{
		class FileReader: public RefBase
		{
		protected:
			FileReader();
			~FileReader();
		public:
			virtual size_t						size() const = 0;
			virtual size_t						offset() const = 0;
			virtual size_t						available() const = 0;
		public:
			virtual void						seek(size_t offset) = 0;
			virtual void						seekRelative(ptrdiff_t offset) = 0;
		public:
			virtual void						read(void* buffer, size_t size) = 0;
		public:
			template<class T>
			void								read(T& data)				{	Z_STATIC_ASSERT( sizeof(POD_TYPE(data)) == sizeof(uint8_t), WRONG_TYPE );	read(&data, sizeof(data));	}
			void								read(mbs& data);
			void								read(wcs& data);
		public:
			template<class T>
			T									read()						{	T data;	read(data);	return data;	}
		private:
			uint8_t								POD_TYPE(int8_t&);
			uint8_t								POD_TYPE(uint8_t&);
			uint8_t								POD_TYPE(int16_t&);
			uint8_t								POD_TYPE(uint16_t&);
			uint8_t								POD_TYPE(int32_t&);
			uint8_t								POD_TYPE(uint32_t&);
			uint8_t								POD_TYPE(int64_t&);
			uint8_t								POD_TYPE(uint64_t&);
			uint8_t								POD_TYPE(bool&);
			uint8_t								POD_TYPE(float&);
			uint8_t								POD_TYPE(double&);
			template<class T>
			uint16_t							POD_TYPE(T);
		};

		class MemFileReader: public FileReader
		{
		private:
			SMem								_sm;
			MemReader							_reader;
		public:
			MemFileReader(const SMem& sm);
		protected:
			~MemFileReader();
		public:
			size_t								size() const;
			size_t								offset() const;
			size_t								available() const;
		public:
			void								seek(size_t offset);
			void								seekRelative(ptrdiff_t offset);
		public:
			void								read(void* buffer, size_t size);
		};

		// maps a segment of a file to read as a single file
		class SubFileReader: public FileReader
		{
		private:
			SPtr<FileReader>					_fr;
			size_t								_offset;					// the start of the segment in _fr
			size_t								_length;					// the length of the segment in _fr
		public:
			SubFileReader(const SPtr<FileReader>& fr, size_t offset, size_t length);
		protected:
			~SubFileReader();
		public:
			size_t								size() const;
			size_t								offset() const;
			size_t								available() const;
		public:
			void								seek(size_t offset);
			void								seekRelative(ptrdiff_t offset);
		public:
			void								read(void* buffer, size_t size);
		};
	};
};

