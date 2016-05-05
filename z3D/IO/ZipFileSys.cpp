
#include "ZipFileSys.h"

#include "../Core/allocators_inl.h"
#include "../Core/Misc.h"

#include "FileReader.h"

#include "../../zlib/unzip.h"
#include "../../zlib/ioapi.h"

#include "../Core/nt_mem_pool_inl.h"

namespace z3D
{
	namespace IO
	{
		voidpf ZCALLBACK			unz_streaming_open_file_func(voidpf opaque, const char* filename, int mode)
		{
			Z_ASSERT( mode == (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING) );
			SPtr<FileSystem>& fs = *(SPtr<FileSystem>*)opaque;
			SPtr<FileReader> fr = fs->openFile(UTF8TOWCS(filename), false);
			if(!fr)
				return NULL;
			SPtr<FileReader>* pfr = new SPtr<FileReader>(fr);
			return pfr;
		}
		voidpf ZCALLBACK			unz_memory_open_file_func(voidpf opaque, const char* filename, int mode)
		{
			SMem& sm = *(SMem*)opaque;
			return new SPtr<FileReader>(new MemFileReader(sm));
		}
		int ZCALLBACK				unz_close_file_func(voidpf opaque, voidpf stream)
		{
			delete (SPtr<FileReader>*)stream;
			return 0;
		}
		uLong ZCALLBACK				unz_read_file_func(voidpf opaque, voidpf stream, void* buf, uLong size)
		{
			(*(SPtr<FileReader>*)stream)->read(buf, (size_t)size);
			return size;
		}
		uLong ZCALLBACK				unz_write_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size)
		{
			Z_ASSERT( false );
			return 0;
		}
		long ZCALLBACK				unz_tell_file_func(voidpf opaque, voidpf stream)
		{
			return (long)(*(SPtr<FileReader>*)stream)->offset();
		}
		long ZCALLBACK				unz_seek_file_func(voidpf opaque, voidpf stream, uLong offset, int origin)
		{
			switch(origin)
			{
			case ZLIB_FILEFUNC_SEEK_SET:
				(*(SPtr<FileReader>*)stream)->seek((size_t)offset);
				break;
			case ZLIB_FILEFUNC_SEEK_CUR:
				(*(SPtr<FileReader>*)stream)->seekRelative((ptrdiff_t)(long)offset);
				break;
			case ZLIB_FILEFUNC_SEEK_END:
				(*(SPtr<FileReader>*)stream)->seek((*(SPtr<FileReader>*)stream)->size() - (size_t)offset);
				break;
			}
			return 0;
		}
		int ZCALLBACK				unz_testerror_file_func(voidpf opaque, voidpf stream)
		{
			return 0;
		}

		class ZipFileReader: public FileReader
		{
		private:
			unzFile								_zfile;
			unz_file_pos						_ufp;
			SPtr<FileSystem>*					_parent_fs;
			SMem*								_archive_mem;
			size_t								_size;
			size_t								_offset;
		public:
			ZipFileReader(const unzFile& zfile, const unz_file_pos& ufp, SPtr<FileSystem>* pfs, SMem* psm)
				: _zfile(zfile), _ufp(ufp), _parent_fs(pfs), _archive_mem(psm)
			{
				unz_file_info file_info;
				_offset = 0;
				if(unzGetCurrentFileInfo(_zfile, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
				{
					_size = 0;
					return;
				}
				_size = (size_t)file_info.uncompressed_size;
			}
		protected:
			~ZipFileReader()
			{
				unzCloseCurrentFile(_zfile);
				unzClose(_zfile);
				if(_parent_fs)
					delete _parent_fs;
				if(_archive_mem)
					delete _archive_mem;
			}
		public:
			size_t								size() const
			{
				return _size;
			}
			size_t								offset() const
			{
				return _offset;
			}
			size_t								available() const
			{
				return size() - offset();
			}
		public:
			void								seek(size_t offset)						// no random access, so keep forward reading as much as possible
			{
				if(offset == _offset)
					return;

				if(offset > _offset)
				{
					char buffer[1024];
					while(offset != _offset)
					{
						size_t c = min(offset - _offset, _countof(buffer));
						read(buffer, c);
					}
				}
				else
				{
					unzCloseCurrentFile(_zfile);
					unzGoToFilePos(_zfile, &_ufp);
					unzOpenCurrentFile(_zfile);
					_offset = 0;
					seek(offset);
				}
			}
			void								seekRelative(ptrdiff_t offset)
			{
				seek(_offset + offset);
			}
		public:
			void								read(void* buffer, size_t size)
			{
				for(size_t decomp = 0; decomp != size; )
				{
					int ret = unzReadCurrentFile(_zfile, (char*)buffer + decomp, size - decomp);
					if(ret <= 0)
					{
						Z_VERIFY( false );
						return;
					}
					decomp += (size_t)ret;
				}

				_offset += size;
			}
		};

		size_t						ZipFileSystem::__entry_count(void* zfile)
		{
			unz_global_info global_info;
			if(unzGetGlobalInfo((unzFile)zfile, &global_info) != UNZ_OK)
				return 0;
			return (size_t)global_info.number_entry;
		}
		ZipFileSystem::ZipFileSystem(const wcs& archive_path, SPtr<FileSystem>* pfs, SMem* archive_mem, void* zfile)
			:	FileSystem(L"ZipFileSystem: " + archive_path), 
				_archive_path(archive_path), 
				_parent_fs(pfs), 
				_archive_mem(archive_mem), 
				_zfile(zfile), 
				// plus one because zero size map will also allocate one as its root 
				_mp_keeper(nt_mem_pool<fixed_block_allocator<size_t>::block_size, fixed_block_allocator<size_t>::block_alignment>::create(__entry_count(zfile) + 1)), 
				_indices(less<mbs>(), fixed_block_allocator<pair<mbs, uint64_t>>(_mp_keeper.get()))
		{
			for(int ret = unzGoToFirstFile(_zfile); ret == UNZ_OK; ret = unzGoToNextFile(_zfile))
			{
				unz_file_pos ufp;
				if(unzGetFilePos(_zfile, &ufp) != UNZ_OK)
					continue;

				char filename[4096];
				filename[_countof(filename) - 1] = 0;
				unz_file_info file_info;
				if(unzGetCurrentFileInfo(_zfile, &file_info, filename, _countof(filename) - 1, NULL, 0, NULL, 0) != UNZ_OK)
					continue;

				_strlwr_s(filename);

				Z_STATIC_ASSERT( sizeof(ufp) == sizeof(uint64_t), WRONG_ASSUMPTION );
				_indices[filename] = *(uint64_t*)&ufp;
			}
		}
		ZipFileSystem::~ZipFileSystem()
		{
			if(_zfile)
				unzClose(_zfile);
			if(_parent_fs)
				delete _parent_fs;
			if(_archive_mem)
				delete _archive_mem;
		}
		SPtr<ZipFileSystem>			ZipFileSystem::create(const wcs& archive_path, const SPtr<FileSystem>& fs_, bool in_memory)
		{
			SPtr<FileSystem> fs = fs_;
			if(!fs)
				fs = getFileSystem(L"WIN32");
			if(!fs)
				return NULL;
			SPtr<FileSystem>* pfs = NULL;
			SMem* psm = NULL;
			zlib_filefunc_def iodef;
			if(!in_memory)
			{
				pfs = new SPtr<FileSystem>(fs);
				iodef.opaque = pfs;
				iodef.zopen_file = unz_streaming_open_file_func;
			}
			else
			{
				SMem sm;
				if(!fs->loadFile(archive_path, sm))
					return NULL;
				psm = new SMem(sm);
				iodef.opaque = psm;
				iodef.zopen_file = unz_memory_open_file_func;
			}
			iodef.zclose_file = unz_close_file_func;
			iodef.zread_file = unz_read_file_func;
			iodef.zwrite_file = unz_write_file_func;
			iodef.ztell_file = unz_tell_file_func;
			iodef.zseek_file = unz_seek_file_func;
			iodef.zerror_file = unz_testerror_file_func;

			mbs utf8_path = WCSTOUTF8(archive_path);
			unzFile zfile = unzOpen2(utf8_path.c_str(), &iodef);
			if(!zfile)
			{
				if(pfs)
					delete pfs;
				if(psm)
					delete psm;

				return NULL;
			}

			return new ZipFileSystem(archive_path, pfs, psm, (void*)zfile);
		}
		bool						ZipFileSystem::hasFile(const wcs& path)
		{
			return !!_indices.count(WCSTOMBS(path));
		}
		SPtr<FileReader>			ZipFileSystem::openFile(const wcs& path, bool in_memory)
		{
			if(in_memory)
			{
				SMem sm;
				if(!loadFile(path, sm))
					return NULL;
				return new MemFileReader(sm);
			}
			mbs utf8_path = WCSTOUTF8(path);
			INDICES_TYPE::const_iterator iter = _indices.find(utf8_path);
			if(iter == _indices.end())
				return NULL;

			if(true)
			{
				Z_STATIC_ASSERT( sizeof(unz_file_pos) == sizeof(uint64_t), WRONG_ASSUMPTION );
				unzGoToFilePos(_zfile, (unz_file_pos*)&iter->second);

				unz_file_info file_info;
				if(unzGetCurrentFileInfo(_zfile, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
					return false;
				if(file_info.compression_method == 0)
				{
					if(unzOpenCurrentFile(_zfile) != UNZ_OK)
						return false;

					uLong p = unzGetCurrentFileCompressedDataArchiveOffset(_zfile);

					SPtr<FileReader> fr;
					if(_parent_fs)
						fr = (*_parent_fs)->openFile(_archive_path);
					else
						fr = new MemFileReader(*_archive_mem);

					if(!fr)
						return NULL;

					return new SubFileReader(fr, (size_t)p, (size_t)file_info.compressed_size);
				}
			}

			SPtr<FileSystem>* pfs = NULL;
			SMem* psm = NULL;
			zlib_filefunc_def iodef;
			if(_parent_fs)
			{
				iodef.opaque = pfs = new SPtr<FileSystem>(*_parent_fs);
				iodef.zopen_file = unz_streaming_open_file_func;
			}
			else
			{
				iodef.opaque = psm = new SMem(*_archive_mem);
				iodef.zopen_file = unz_memory_open_file_func;
			}
			iodef.zclose_file = unz_close_file_func;
			iodef.zread_file = unz_read_file_func;
			iodef.zwrite_file = unz_write_file_func;
			iodef.ztell_file = unz_tell_file_func;
			iodef.zseek_file = unz_seek_file_func;
			iodef.zerror_file = unz_testerror_file_func;

			mbs utf8_archive_path = WCSTOUTF8(_archive_path);
			unzFile zfile = unzOpen2(utf8_archive_path.c_str(), &iodef);
			if(!zfile)
			{
				if(pfs) delete pfs;
				if(psm) delete psm;

				return NULL;
			}
			Z_STATIC_ASSERT( sizeof(unz_file_pos) == sizeof(uint64_t), WRONG_ASSUMPTION );
			if(unzGoToFilePos(zfile, (unz_file_pos*)&iter->second) != UNZ_OK)
			{
				if(pfs) delete pfs;
				if(psm) delete psm;

				return NULL;
			}
			if(unzOpenCurrentFile(zfile) != UNZ_OK)
			{
				if(pfs) delete pfs;
				if(psm) delete psm;

				return NULL;
			}

			return new ZipFileReader(zfile, *(unz_file_pos*)&iter->second, pfs, psm);
		}
		bool						ZipFileSystem::loadFile(const wcs& path, SMem& sm)
		{
			INDICES_TYPE::const_iterator iter = _indices.find(WCSTOMBS(path));
			if(iter == _indices.end())
				return false;

			Z_STATIC_ASSERT( sizeof(unz_file_pos) == sizeof(uint64_t), WRONG_ASSUMPTION );
			unzGoToFilePos(_zfile, (unz_file_pos*)&iter->second);

			unz_file_info file_info;
			if(unzGetCurrentFileInfo(_zfile, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
				return false;
			if(unzOpenCurrentFile(_zfile) != UNZ_OK)
				return false;

			class dealloc_func
			{
			public:
				static void exec(void* p)
				{
					delete[] (char*)p;
				}
			};
			size_t size = (size_t)file_info.uncompressed_size;
			sm = SMem::create(new char[SMem::getRequiredSize(size)], size, &dealloc_func::exec);

			bool succ = true;
			for(size_t decomp = 0; decomp != size; )
			{
				int ret = unzReadCurrentFile(_zfile, (char*)sm.get() + decomp, sm.size() - decomp);
				if(ret <= 0)
				{
					sm.reset();
					succ = false;
				}
				decomp += (size_t)ret;
			}

			unzCloseCurrentFile(_zfile);

			return succ;
		}
		bool						ZipFileSystem::saveFile(const wcs& path, const void* byte_array, size_t byte_count)
		{
			return false;
		}
		bool						ZipFileSystem::saveFile(const wcs& path, const SMem& sm)
		{
			return false;
		}
	};
};