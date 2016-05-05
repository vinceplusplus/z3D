
#include "FileSystem.h"

#include "../Core/LoggerSTL.h"
#include "../Core/SMem.h"
#include "../Core/Misc.h"

namespace z3D
{
	namespace IO
	{
		FileSystem::FileSystem(const wcs& name)
		{
			_name = name;
		}
		FileSystem::~FileSystem()
		{
		}
		int FileSystem::locateFileSystem(const wcs& name)
		{
			for(int i = 0; i < (int)_array_filesys.size(); ++i)
				if(_array_filesys[i]->_name == name)
					return i;
			return -1;
		}
		bool FileSystem::insertFileSystem(const SPtr<FileSystem>& filesys, int index)
		{
			if(!filesys)
				return false;

			if(index == -1)
				_array_filesys.push_back(filesys);
			else
				_array_filesys.insert(_array_filesys.begin() + (min(max(0, index), (int)_array_filesys.size())), filesys);

			return true;
		}
		SPtr<FileSystem> FileSystem::getFileSystem(const wcs& name)
		{
			int index = locateFileSystem(name);
			if(index == -1)
				return NULL;
			return getFileSystem(index);
		}
		SPtr<FileSystem> FileSystem::getFileSystem(int index)
		{
			Z_ASSERT( _array_filesys.size() );
			Z_ASSERT( index >= 0 && index < (int)_array_filesys.size() );

			return _array_filesys[index];
		}
		int FileSystem::getFileSystemCount()
		{
			return (int)_array_filesys.size();
		}
		SPtr<FileSystem> FileSystem::removeFileSystem(const wcs& name)
		{
			int index = locateFileSystem(name);
			if(index == -1)
				return NULL;
			return removeFileSystem(index);
		}
		SPtr<FileSystem> FileSystem::removeFileSystem(int index)
		{
			SPtr<FileSystem> filesys = getFileSystem(index);

			_array_filesys.erase(_array_filesys.begin() + index);
			return filesys;
		}
		bool							FileSystem::loadFromAll(const wcs& filepath, SMem& sm)
		{
			if(!filepath.length())
				return false;
			wcs path2 = filepath;
			size_t double_slash_pos = -1;
			size_t back_slash_pos = -1;
			while((back_slash_pos = (size_t)path2.find('\\')) != (size_t)-1)
				path2[back_slash_pos] = '/';
			while((double_slash_pos = (size_t)path2.find(L"//")) != (size_t)-1)
				path2.erase(double_slash_pos, 1);

			for(size_t i = 0; i < _array_filesys.size(); ++i)
				if(_array_filesys[i]->loadFile(path2, sm))
					return true;

			syslog << wcs(L"cannot load file: ") + path2;
			return false;
		}
		mbs FileSystem::loadUTF8FromAll(const wcs& filepath)
		{
			SMem sm;
			if(!loadFromAll(filepath, sm))
				return "";
			return getTextUTF8(sm.get(), sm.size());
		}
		wcs							FileSystem::loadUCS2FromAll(const wcs& filepath)
		{
			SMem sm;
			if(!loadFromAll(filepath, sm))
				return L"";
			return getTextUCS2(sm.get(), sm.size());
		}
	};
};

