
#pragma once

#include "predecl.h"

#include "../Core/allocators.h"

#include "FileSystem.h"

namespace z3D
{
	namespace IO
	{
		class ZipFileSystem: public FileSystem
		{
		private:
			Z_DEFINE_FIXED_BLOCK_ALLOCATOR(fixed_block_allocator, 64, 8);
			typedef map<mbs, uint64_t, less<mbs>, ZipFileSystem::fixed_block_allocator<pair<mbs, uint64_t>>>
										INDICES_TYPE;
		private:
			wcs							_archive_path;
			SPtr<FileSystem>*			_parent_fs;						// if NOT in_memory, this will be non-NULL
			SMem*						_archive_mem;					// if in_memory, this will be non-NULL
		private:
			void*						_zfile;
		private:
			nt_mem_pool_keeper<fixed_block_allocator<size_t>::block_size, fixed_block_allocator<size_t>::block_alignment, true>
										_mp_keeper;
			INDICES_TYPE				_indices;
		private:
			static size_t				__entry_count(void* zfile);
		protected:
			ZipFileSystem(const wcs& archive_path, SPtr<FileSystem>* pfs, SMem* archive_mem, void* zfile);
			~ZipFileSystem();
		public:
			static SPtr<ZipFileSystem>	create(const wcs& archive_path, const SPtr<FileSystem>& fs = 0, bool in_memory = false);
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

