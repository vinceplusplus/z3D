
#pragma once

#include "predecl.h"

#include "../Core/ResBase.h"

#include "MemReader.h"

namespace z3D
{
	namespace IO
	{
		class FileSystem: public RefBase
		{
		private:
			wcs									_name;
		private:
			static vector<SPtr<FileSystem>>		_array_filesys;
		public:
			FileSystem(const wcs& name);
		protected:
			~FileSystem();
		public:
			virtual bool						hasFile(const wcs& path) = 0;
		public:
			virtual SPtr<FileReader>			openFile(const wcs& path, bool in_memory = false) = 0;
			virtual bool						loadFile(const wcs& path, SMem& sm) = 0;
		public:
			virtual bool						saveFile(const wcs& path, const void* byte_array, size_t byte_count) = 0;
			virtual bool						saveFile(const wcs& path, const SMem& sm) = 0;
		public:
			static int							locateFileSystem(const wcs& name);
			static bool							insertFileSystem(const SPtr<FileSystem>& filesys, int index = 0);
			static SPtr<FileSystem>				getFileSystem(const wcs& name);
			static SPtr<FileSystem>				getFileSystem(int index);
			static int							getFileSystemCount();
			static SPtr<FileSystem>				removeFileSystem(const wcs& name);
			static SPtr<FileSystem>				removeFileSystem(int index);
			static bool							loadFromAll(const wcs& filepath, SMem& sm);
			static mbs							loadUTF8FromAll(const wcs& filepath);
			static wcs							loadUCS2FromAll(const wcs& filepath);
		};
	};
};

