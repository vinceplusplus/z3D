
#include "RiffReader.h"

namespace z3D
{
	namespace Core
	{
		void					RiffReader::enum_chunks(const void* ptr,unsigned long length,vector<CHUNK*>& array_chunk)
		{
			char* cur_ptr=(char*)ptr;
			char* end_ptr=(char*)ptr+length;

			while(cur_ptr<end_ptr)
			{
				if(cur_ptr+8>end_ptr)
					return;

				CHUNK* c=(CHUNK*)cur_ptr;

				if(cur_ptr+c->data_size+8+(c->data_size&1)>end_ptr)
					return;

				array_chunk.push_back(c);

				cur_ptr+=c->data_size+8+(c->data_size&1);
			}
		}
		RiffReader::CHUNK*	RiffReader::find_list(const char* code,const char* type_code,const vector<CHUNK*>& array_chunk)
		{
			int i;
			for(i=0;i<(int)array_chunk.size();++i)
			{
				CHUNK* c=array_chunk[i];
				if(memcmp(c->code,code,4))
					continue;
				if(c->data_size<4)
					continue;
				if(memcmp(c->type_code,type_code,4))
					continue;
				return c;
			}
			return NULL;
		}
		RiffReader::CHUNK*	RiffReader::find_chunk(const char* code,const vector<CHUNK*>& array_chunk)
		{
			int i;
			for(i=0;i<(int)array_chunk.size();++i)
			{
				CHUNK* c=array_chunk[i];
				if(!memcmp(c->code,code,4))
					return c;
			}
			return NULL;
		}
		void					RiffReader::find_lists(const char* code,const char* type_code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2)
		{
			int i;
			for(i=0;i<(int)array_chunk.size();++i)
			{
				CHUNK* c=array_chunk[i];
				if(memcmp(c->code,code,4))
					continue;
				if(c->data_size<4)
					continue;
				if(memcmp(c->type_code,type_code,4))
					continue;
				array_chunk2.push_back(c);
			}
		}
		void					RiffReader::find_chunks(const char* code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2)
		{
			int i;
			for(i=0;i<(int)array_chunk.size();++i)
			{
				CHUNK* c=array_chunk[i];
				if(memcmp(c->code,code,4))
					continue;
				array_chunk2.push_back(c);
			}
		}
	};
};

