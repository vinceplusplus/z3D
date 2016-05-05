
#pragma once

#include "predecl.h"

#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		class ZipIOBase
		{
		public:
			ZipIOBase();
			virtual ~ZipIOBase();
		protected:
			void*					__openZip(const char* pathname, bool append);
			void*					__openUnzip(const char* pathname);
		protected:
			virtual void*			__zopen(const char* filename, int mode) = 0;
			virtual unsigned long	__zread(void* stream, void* buf, unsigned long size) = 0;
			virtual unsigned long	__zwrite(void* stream, const void* buf, unsigned long size) = 0;
			virtual long			__ztell(void* stream) = 0;
			virtual long			__zseek(void* stream, unsigned long offset, int origin) = 0;
			virtual int				__zclose(void* stream) = 0;
			virtual int				__ztesterror(void* stream) = 0;
		private:
			static void*			___open_file_func(void* opaque, const char* filename, int mode);
			static unsigned long	___read_file_func(void* opaque, void* stream, void* buf, unsigned long size);
			static unsigned long	___write_file_func(void* opaque, void* stream, const void* buf, unsigned long size);
			static long				___tell_file_func(void* opaque, void* stream);
			static long				___seek_file_func(void* opaque, void* stream, unsigned long offset, int origin);
			static int				___close_file_func(void* opaque, void* stream);
			static int				___testerror_file_func(void* opaque, void* stream);
		};

		class ZipBase
		{
		protected:
			void*					_handle;
		protected:
			ZipBase();
		protected:
			virtual ~ZipBase();
		protected:
			void					__init(void* handle);			// ownership is passed in, must be called after __openZip()
		public:
			void					finish();						// must not be virtual function
		protected:
			bool					__add(const vector<char>& buffer,const mbs& filename_in_zip,bool raw,unsigned long uncompressed_size,unsigned long crc32);
		public:
			//	use of password to add file is discouraged, because use in combination of raw and password sometimes causes problem, to encrypt, just overriding the io methods instead
			bool					add(const vector<char>& buffer,const mbs& filename_in_zip);
			bool					add(const wcs& src_filepath,const mbs& filename_in_zip);
			bool					addRAW(const vector<char>& buffer,const mbs& filename_in_zip,unsigned long uncompressed_size,unsigned long crc32);
		};

		class UnzipBase
		{
		protected:
			void*						_handle;
		protected:
			hash_map<mbs,__int64>		_indices;
			hash_map<mbs,__int64>		_indices_ci;
		protected:
			UnzipBase();
		protected:
			virtual ~UnzipBase();
		protected:
			void					__init(void* handle);			// ownership is passed in, must be called after __openUnzip()
		public:
			void					finish();						// must not be virtual function
		protected:
			void					buildIndices();
		public:
			bool					exists(const mbs& filename,bool case_sens=false);
		public:
			void					getAllFilenames(vector<wcs>& array_filename) const;
		protected:
			bool					__extract(const mbs& filename,vector<char>& buffer,bool case_sens,bool raw,int& method,int& level);
		public:
			bool					extract(const mbs& filename,vector<char>& buffer,bool case_sens=false);
			bool					extract(const mbs& filename,const wcs& dst_filepath,bool case_sens=false);
			bool					extractRAW(const mbs& filename,vector<char>& buffer,bool case_sens,int& method,int& level);
		};

		class MemZipIO: public ZipIOBase
		{
		protected:
			vector<char>		_buffer;
			unsigned long		_pos;
		public:
			MemZipIO();
		protected:
			void*					__zopen(const char* filename, int mode);
			unsigned long			__zread(void* stream, void* buf, unsigned long size);
			unsigned long			__zwrite(void* stream, const void* buf, unsigned long size);
			long					__ztell(void* stream);
			long					__zseek(void* stream, unsigned long offset, int origin);
			int						__zclose(void* stream);
			int						__ztesterror(void* stream);
		public:
			const vector<char>&		buffer() const;
		};

		class MemZip: public MemZipIO, public ZipBase
		{
		public:
			MemZip();
			MemZip(const vector<char>& buffer);
		};

		class MemUnzip: public MemZipIO, public UnzipBase
		{
		public:
			MemUnzip(const vector<char>& buffer);
		protected:
			MemUnzip(int buffer_size);
		public:
			static MemUnzip*		fromFile(const wcs& filepath);
		};
	};
};
