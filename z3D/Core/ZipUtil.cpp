
#include "ZipUtil.h"

#include "../../zlib/zip.h"
#include "../../zlib/unzip.h"

#include <io.h>

#include "Misc.h"

namespace z3D
{
	namespace Core
	{
		ZipIOBase::ZipIOBase() {}
		ZipIOBase::~ZipIOBase() {}
		void*					ZipIOBase::__openZip(const char* pathname, bool append)
		{
			zlib_filefunc_def_s s;
			s.opaque=this;
			s.zopen_file=&___open_file_func;
			s.zread_file=&___read_file_func;
			s.zwrite_file=&___write_file_func;
			s.ztell_file=&___tell_file_func;
			s.zseek_file=&___seek_file_func;
			s.zclose_file=&___close_file_func;
			s.zerror_file=&___testerror_file_func;

			return zipOpen2(pathname,append?APPEND_STATUS_ADDINZIP:APPEND_STATUS_CREATE,0,&s);
		}
		void*					ZipIOBase::__openUnzip(const char* pathname)
		{
			zlib_filefunc_def_s s;
			s.opaque=this;
			s.zopen_file=&___open_file_func;
			s.zread_file=&___read_file_func;
			s.zwrite_file=&___write_file_func;
			s.ztell_file=&___tell_file_func;
			s.zseek_file=&___seek_file_func;
			s.zclose_file=&___close_file_func;
			s.zerror_file=&___testerror_file_func;

			return unzOpen2(pathname,&s);
		}
		void*				ZipIOBase::___open_file_func(void* opaque, const char* filename, int mode) {return ((ZipIOBase*)opaque)->__zopen(filename,mode);}
		unsigned long		ZipIOBase::___read_file_func(void* opaque, void* stream, void* buf, unsigned long size) {return ((ZipIOBase*)opaque)->__zread(stream,buf,size);}
		unsigned long		ZipIOBase::___write_file_func(void* opaque, void* stream, const void* buf, unsigned long size) {return ((ZipIOBase*)opaque)->__zwrite(stream,buf,size);}
		long				ZipIOBase::___tell_file_func(void* opaque, void* stream) {return ((ZipIOBase*)opaque)->__ztell(stream);}
		long				ZipIOBase::___seek_file_func(void* opaque, void* stream, unsigned long offset, int origin) {return ((ZipIOBase*)opaque)->__zseek(stream,offset,origin);}
		int					ZipIOBase::___close_file_func(void* opaque, void* stream) {return ((ZipIOBase*)opaque)->__zclose(stream);}
		int					ZipIOBase::___testerror_file_func(void* opaque, void* stream) {return ((ZipIOBase*)opaque)->__ztesterror(stream);}

		ZipBase::ZipBase() {_handle=0;}
		ZipBase::~ZipBase() {finish();}
		void					ZipBase::__init(void* handle)			// ownership is passed in, must be called after __openZip()
		{
			finish();
			_handle=handle;
		}
		void					ZipBase::finish()						// must not be virtual function
		{
			if(_handle)
				zipClose(_handle,0);
			_handle=0;
		}
		bool					ZipBase::__add(const vector<char>& buffer,const mbs& filename_in_zip,bool raw,unsigned long uncompressed_size,unsigned long crc32)
		{
			if(!_handle)
				return false;
			if(zipOpenNewFileInZip2(_handle,filename_in_zip.c_str(),0,0,0,0,0,0,Z_DEFLATED,Z_DEFAULT_COMPRESSION,raw?1:0) != ZIP_OK)
				return false;

			if(buffer.size())
				zipWriteInFileInZip(_handle,&buffer[0],(unsigned int)buffer.size());

			if(raw)
				zipCloseFileInZipRaw(_handle,uncompressed_size,crc32);
			else
				zipCloseFileInZip(_handle);
			return true;
		}
		//	use of password to add file is discouraged, because use in combination of raw and password sometimes causes problem, to encrypt, just overriding the io methods instead
		bool					ZipBase::add(const vector<char>& buffer,const mbs& filename_in_zip)
		{
			return __add(buffer,filename_in_zip,false,0,0);
		}
		bool					ZipBase::add(const wcs& src_filepath, const mbs& filename_in_zip)
		{
			FILE* fp=NULL;
			_wfopen_s(&fp,src_filepath.c_str(),L"rb");
			if(!fp)
				return false;
			vector<char> buffer(_filelength(_fileno(fp)));
			if(buffer.size())
				fread(&buffer[0],buffer.size(),1,fp);
			fclose(fp);

			return add(buffer,filename_in_zip);
		}
		bool					ZipBase::addRAW(const vector<char>& buffer,const mbs& filename_in_zip,unsigned long uncompressed_size,unsigned long crc32)
		{
			return __add(buffer,filename_in_zip,true,uncompressed_size,crc32);
		}

		UnzipBase::UnzipBase() {_handle=0;}
		UnzipBase::~UnzipBase() {finish();}
		void					UnzipBase::__init(void* handle)			// ownership is passed in, must be called after __openUnzip()
		{
			finish();
			_handle=handle;

			buildIndices();
		}
		void					UnzipBase::finish()						// must not be virtual function
		{
			if(_handle)
				unzClose(_handle);
			_handle=0;
		}
		void					UnzipBase::buildIndices()
		{
			if(!_handle)
				return;

			_indices.clear();
			_indices_ci.clear();

			int ret;
			for(ret=unzGoToFirstFile(_handle);ret==UNZ_OK;ret=unzGoToNextFile(_handle))
			{
				char filename[4096];
				if(unzGetCurrentFileInfo(_handle,0,filename,(unsigned long)sizeof(filename),0,0,0,0)!=UNZ_OK)
					continue;
				filename[4095]=0;
				__int64 index=0;
				unzGetFilePos(_handle,(unz_file_pos*)&index);
				_indices[filename]=index;

				_strlwr_s(filename);
				_indices_ci[filename]=index;
			}
		}
		bool					UnzipBase::exists(const mbs& filename,bool case_sens)
		{
			if(case_sens)
				return !!_indices.count(filename);
			return !!_indices_ci.count(filename);
		}
		void					UnzipBase::getAllFilenames(vector<wcs>& array_filename) const
		{
			if(!_handle)
				return;

			int ret;
			for(ret=unzGoToFirstFile(_handle);ret==UNZ_OK;ret=unzGoToNextFile(_handle))
			{
				char filename[4096];
				if(unzGetCurrentFileInfo(_handle,0,filename,(unsigned long)sizeof(filename),0,0,0,0)!=UNZ_OK)
					continue;
				filename[4095]=0;
				array_filename.push_back(MBSTOWCS(filename));
			}
		}
		bool					UnzipBase::__extract(const mbs& filename,vector<char>& buffer,bool case_sens,bool raw,int& method,int& level)
		{
			if(!_handle)
				return false;

			if(!filename.length())
				return false;
			__int64 index;
			if(case_sens)
			{
				hash_map<mbs,__int64>::iterator iter=_indices.find(filename);
				if(iter==_indices.end())
					return false;
				index=iter->second;
			}
			else
			{
				mbs filename2=TOLOWER(filename);
				hash_map<mbs,__int64>::iterator iter=_indices_ci.find(filename2);
				if(iter==_indices_ci.end())
					return false;
				index=iter->second;
			}
			unz_file_info file_info;
			if(unzGoToFilePos(_handle,(unz_file_pos*)&index)!=UNZ_OK)
				return false;
			if(unzGetCurrentFileInfo(_handle,&file_info,0,0,0,0,0,0)!=UNZ_OK)
				return false;
			if(unzOpenCurrentFile2(_handle,&method,&level,raw?1:0)!=UNZ_OK)
				return false;

			buffer.resize(file_info.uncompressed_size);
			int ret;
			int decomp = 0;
			if(buffer.size())
				while(decomp != file_info.uncompressed_size)
				{
					ret = unzReadCurrentFile(_handle,&buffer[decomp], (unsigned int)buffer.size() - decomp);
					if(ret > 0)
						decomp += ret;
					else if(ret <= 0)
						break;
				}

			unzCloseCurrentFile(_handle);

			return (decomp==file_info.uncompressed_size);
		}
		bool					UnzipBase::extract(const mbs& filename,vector<char>& buffer,bool case_sens)
		{
			int method,level;
			return __extract(filename,buffer,case_sens,0,method,level);
		}
		bool					UnzipBase::extract(const mbs& filename,const wcs& dst_filepath,bool case_sens)
		{
			vector<char> buffer;
			if(!extract(filename,buffer,case_sens))
				return false;
			FILE* fp=NULL;
			_wfopen_s(&fp,dst_filepath.c_str(),L"wb");
			if(!fp)
				return false;
			if(buffer.size())
				fwrite(&buffer[0],buffer.size(),1,fp);
			fclose(fp);
			return true;
		}
		bool					UnzipBase::extractRAW(const mbs& filename,vector<char>& buffer,bool case_sens,int& method,int& level)
		{
			return __extract(filename,buffer,case_sens,true,method,level);
		}

		MemZipIO::MemZipIO():_pos(0) {}
		void*					MemZipIO::__zopen(const char* filename, int mode)
		{
			_pos=0;
			if((mode & ZLIB_FILEFUNC_MODE_EXISTING) && (mode & ZLIB_FILEFUNC_MODE_WRITE))
				_pos=(unsigned long)_buffer.size();
			return (void*)1;
		}
		unsigned long			MemZipIO::__zread(void* stream, void* buf, unsigned long size)
		{
			unsigned long s=min(size,(unsigned long)(_buffer.size()-_pos));
			memcpy(buf,&_buffer[_pos],s);
			_pos+=s;
			return size;
		}
		unsigned long			MemZipIO::__zwrite(void* stream, const void* buf, unsigned long size)
		{
			_buffer.resize(max(_pos+size,(unsigned long)_buffer.size()));
			memcpy(&_buffer[_pos],buf,size);
			_pos+=size;
			return size;
		}
		long					MemZipIO::__ztell(void* stream)
		{
			return _pos;
		}
		long					MemZipIO::__zseek(void* stream, unsigned long offset, int origin)
		{
			switch(origin)
			{
			case ZLIB_FILEFUNC_SEEK_SET:
				_pos=min(max(offset,0UL),(unsigned long)_buffer.size());
				break;
			case ZLIB_FILEFUNC_SEEK_CUR:
				_pos=min(max(_pos+offset,0UL),(unsigned long)_buffer.size());
				break;
			case ZLIB_FILEFUNC_SEEK_END:
				_pos=min(max((unsigned long)_buffer.size()-offset,0UL),(unsigned long)_buffer.size());
				break;
			}
			return 0;
		}
		int						MemZipIO::__zclose(void* stream)
		{
			return 0;
		}
		int						MemZipIO::__ztesterror(void* stream)
		{
			return 0;
		}
		const vector<char>&		MemZipIO::buffer() const
		{
			return _buffer;
		}

		MemZip::MemZip()
		{
			__init(__openZip("1",0));
		}
		MemZip::MemZip(const vector<char>& buffer)
		{
			_buffer=buffer;
			__init(__openZip("1",1));
		}

		MemUnzip::MemUnzip(const vector<char>& buffer)
		{
			_buffer=buffer;
			__init(__openUnzip("1"));
		}
		MemUnzip::MemUnzip(int buffer_size)
		{
			_buffer.resize(buffer_size);
		}
		MemUnzip*		MemUnzip::fromFile(const wcs& filepath)
		{
			FILE* fp=NULL;
			_wfopen_s(&fp,filepath.c_str(),L"rb");
			if(!fp)
				return NULL;
			MemUnzip* unzip=new MemUnzip(_filelength(_fileno(fp)));
			int ret=0;
			if(unzip->_buffer.size())
				ret=(int)fread(&unzip->_buffer[0],unzip->_buffer.size(),1,fp);
			fclose(fp);

			if(ret!=1)
			{
				delete unzip;
				return NULL;
			}

			unzip->__init(unzip->__openUnzip("1"));

			return unzip;
		}
	};
};
