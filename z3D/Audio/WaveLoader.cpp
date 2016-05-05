
#define WIN32_MEAN_AND_LEAN

#include <windows.h>
#include "WaveLoader.h"

namespace z3D
{
	namespace Audio
	{
		bool					WaveLoader::load(const void* ptr,unsigned long length,unsigned short& channels,unsigned short& sample_bits,unsigned long& samples_per_sec,void*& data,unsigned long& data_size)
		{
			if(length<8)
				return false;
			if(memcmp(ptr,"RIFF",4))
				return false;
			vector<CHUNK*> array_chunk;
			enum_chunks(ptr,length,array_chunk);
			CHUNK* chunk_wave=find_list("RIFF","WAVE",array_chunk);
			if(!chunk_wave)
				return false;
			array_chunk.clear();
			enum_chunks(chunk_wave->list_data,chunk_wave->data_size-4,array_chunk);
			CHUNK* chunk_fmt=find_chunk("fmt ",array_chunk);
			CHUNK* chunk_data=find_chunk("data",array_chunk);
			if(!chunk_fmt || !chunk_data)
				return false;

			if(chunk_fmt->data_size<16)
				return false;
			WAVEFORMATEX* format=(WAVEFORMATEX*)chunk_fmt->data;
			if(format->wFormatTag!=WAVE_FORMAT_PCM)
				return false;
			if(format->nChannels!=1 && format->nChannels!=2)
				return false;
			if(format->wBitsPerSample!=8 && format->wBitsPerSample!=16)
				return false;
			if(format->nBlockAlign != (format->wBitsPerSample>>3)*format->nChannels)
				return false;

			channels = format->nChannels;
			sample_bits = format->wBitsPerSample;
			samples_per_sec = format->nSamplesPerSec;
			data = chunk_data->data;
			data_size = chunk_data->data_size - (chunk_data->data_size % format->nBlockAlign);

			return true;
		}
		bool					WaveLoader::load(const void* ptr,unsigned long length,unsigned short& channels,unsigned short& sample_bits,unsigned long& samples_per_sec,vector<char>& data)
		{
			void* data2;
			unsigned long data_size;
			if(!load(ptr,length,channels,sample_bits,samples_per_sec,data2,data_size))
				return false;
			data.resize(data_size);
			if(data_size)
				memcpy(&data[0],data2,data_size);
			return true;
		}
	};
};

