
#pragma once

#include "predecl.h"
#include "FileSystem.h"

namespace z3D
{
	namespace IO
	{
		class Win32FileSystem: public FileSystem
		{
		public:
			static SPtr<Win32FileSystem>		_instance;
		public:
			Win32FileSystem();
		protected:
			~Win32FileSystem();
		public:
			bool						hasFile(const wcs& path);
		public:
			SPtr<FileReader>			openFile(const wcs& path, bool in_memory);
			bool						loadFile(const wcs& path, SMem& sm);
		public:
			bool						saveFile(const wcs& path, const void* byte_array, size_t byte_count);
			bool						saveFile(const wcs& path, const SMem& sm);
		};
	};
};

