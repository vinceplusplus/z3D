
#include "Win32FileSys.h"
#include "FileReader.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max


namespace z3D
{
	namespace IO
	{
		class Win32FileReader: public FileReader
		{
		private:
			HANDLE								_handle;
			SPtr<Win32FileSystem>				_fs;
		public:
			Win32FileReader(HANDLE handle, const SPtr<Win32FileSystem>& fs)
				: _handle(handle), _fs(fs)
			{
			}
		protected:
			~Win32FileReader()
			{
				CloseHandle(_handle);
			}
		public:
			size_t								size() const
			{
				return (size_t)GetFileSize(_handle, NULL);
			}
			size_t								offset() const
			{
				return (size_t)SetFilePointer(_handle, 0, NULL, FILE_CURRENT);
			}
			size_t								available() const
			{
				return size() - offset();
			}
		public:
			void								seek(size_t offset)
			{
				SetFilePointer(_handle, (LONG)offset, NULL, FILE_BEGIN);
			}
			void								seekRelative(ptrdiff_t offset)
			{
				SetFilePointer(_handle, (LONG)offset, NULL, FILE_CURRENT);
			}
		public:
			void								read(void* buffer, size_t size)
			{
				DWORD bytecount = 0;
				BOOL ret = ReadFile(_handle, buffer, (DWORD)size, &bytecount, NULL);
				if(!ret)
				{
					Z_VERIFY( false );
				}
				if((size_t)bytecount != size)
				{
					Z_VERIFY( false );
				}
			}
		};

		Win32FileSystem::Win32FileSystem()
			: FileSystem(L"WIN32")
		{}

		Win32FileSystem::~Win32FileSystem() {}

		bool Win32FileSystem::hasFile(const wcs& path)
		{
			WIN32_FIND_DATA data;
			HANDLE handle;
			handle = FindFirstFile(path.c_str(), &data);
			if(handle == INVALID_HANDLE_VALUE)
				return false;
			FindClose(handle);
			return true;
		}
		SPtr<FileReader>			Win32FileSystem::openFile(const wcs& path, bool in_memory)
		{
			if(in_memory)
			{
				SMem sm;
				if(!loadFile(path, sm))
					return NULL;
				return new MemFileReader(sm);
			}
			HANDLE handle = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if(handle == INVALID_HANDLE_VALUE)
				return NULL;
			return new Win32FileReader(handle, this);
		}
		bool						Win32FileSystem::loadFile(const wcs& path, SMem& sm)
		{
			HANDLE handle = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if(handle == INVALID_HANDLE_VALUE)
				return false;
			size_t byte_count = (size_t)GetFileSize(handle, NULL);

			class dealloc_func
			{
			public:
				static void exec(void* p)
				{
					delete[] (char*)p;
				}
			};
			SMem fm = SMem::create(new char[SMem::getRequiredSize(byte_count)], byte_count, &dealloc_func::exec);

			DWORD readcount = 0;
			BOOL ret = ReadFile(handle, fm.get(), (DWORD)byte_count, &readcount, NULL);
			CloseHandle(handle);
			if(byte_count != (size_t)readcount)
				return false;

			sm = fm;
			return !!ret;
		}
		bool						Win32FileSystem::saveFile(const wcs& path, const void* byte_array, size_t byte_count)
		{
			HANDLE handle;
			handle = CreateFile(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
			if(handle == INVALID_HANDLE_VALUE)
				return false;
			DWORD byteswritten = 0;
			BOOL ret = WriteFile(handle, byte_array, (DWORD)byte_count, &byteswritten, NULL);
			CloseHandle(handle);
			return !!ret;
		}
		bool						Win32FileSystem::saveFile(const wcs& path, const SMem& sm)
		{
			return saveFile(path, sm.get(), sm.size());
		}
	};
};
