
#pragma once

#include "predecl.h"

#pragma warning(push)
#pragma warning(disable:4200)

namespace z3D
{
	namespace Core
	{
		class RiffReader
		{
		public:
			// type for a chunk or a list
			typedef struct
			{
				char			code[4];
				unsigned long	data_size;						// minus by 4 is the size of list_data
				union
				{
					struct
					{
						char			type_code[4];
						char			list_data[0];
					};
					char			data[0];
				};
			} CHUNK;
		public:
			static void					enum_chunks(const void* ptr,unsigned long length,vector<CHUNK*>& array_chunk);
			static RiffReader::CHUNK*	find_list(const char* code,const char* type_code,const vector<CHUNK*>& array_chunk);
			static RiffReader::CHUNK*	find_chunk(const char* code,const vector<CHUNK*>& array_chunk);
			static void					find_lists(const char* code,const char* type_code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2);
			static void					find_chunks(const char* code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2);
		};
	};
};

#pragma warning(pop)

