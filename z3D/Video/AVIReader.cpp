
#include "AVIReader.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmsystem.h>

#include "aviriff.h"

#undef min
#undef max

namespace z3D
{
	namespace Video
	{
		AVIReader::AVIReader(const wcs& filepath)
			:ResBase(filepath)
		{
			_avi_list = NULL;
			_hdrl_list = NULL;
			_movi_list = NULL;
			_idx1_chunk = NULL;

			_avimainheader = NULL;
			_avistreamheader_vids = NULL;
			_avistreamheader_auds = NULL;

			_avistreamformat_vids = NULL;
			_avistreamformat_auds = NULL;

			_avistreamheaderdata_vids = NULL;
			_avistreamheaderdata_auds = NULL;
		}
		AVIReader::~AVIReader()
		{
		}
		SPtr<AVIReader>	AVIReader::load(const wcs& filepath)
		{
			if(true)
			{
				AVIReader* avireader = (AVIReader*)_res_mgr.find(filepath);
				if(avireader)
					return SPtr<AVIReader>(avireader);
			}

			SPtr<AVIReader> avireader = loadNoCache(filepath);

			_res_mgr.add(avireader.get());

			return avireader;
		}
		SPtr<AVIReader>	AVIReader::loadNoCache(const wcs& filepath)
		{
			// parsing is not safe, doesn't take care of malformed avi file

			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			MemReader reader(sm.get(), sm.size());
			if(reader.size() < 8)
				return NULL;

			if(memcmp(reader.buffer(), "RIFF", 4) != 0)
				return NULL;

			AVIReader* avireader = new AVIReader(filepath);
			//avireader->_reader = reader;
			avireader->_filemem = sm;

			if(!avireader->parse())
			{
				delete avireader;
				return NULL;
			}

			avireader->set_filepath(filepath);

			return SPtr<AVIReader>(avireader);
		}
		AVIReader::CHUNK*			AVIReader::find_list(char* code,char* type_code,const vector<CHUNK*>& array_chunk)
		{
			int i;
			for(i=0;i<(int)array_chunk.size();++i)
			{
				CHUNK* c=array_chunk[i];
				if(memcmp(c->code,code,4))
					continue;
				if(memcmp(c->type_code,type_code,4))
					continue;
				return c;
			}
			return NULL;
		}
		AVIReader::CHUNK*			AVIReader::find_chunk(char* code,const vector<CHUNK*>& array_chunk)
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
		void			AVIReader::find_lists(char* code,char* type_code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2)
		{
			int i;
			for(i=0;i<(int)array_chunk.size();++i)
			{
				CHUNK* c=array_chunk[i];
				if(memcmp(c->code,code,4))
					continue;
				if(memcmp(c->type_code,type_code,4))
					continue;
				array_chunk2.push_back(c);
			}
		}
		void			AVIReader::find_chunks(char* code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2)
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
		bool			AVIReader::parse()
		{
			MemReader reader(_filemem.get(), _filemem.size());
			int i;

			vector<CHUNK*> array_chunk, array_chunk2, array_chunk3;
			enum_chunks(reader.buffer(), (unsigned long)reader.size(), array_chunk);

			_avi_list = find_list("RIFF", "AVI ", array_chunk);
			if(!_avi_list)
				return false;

			array_chunk.clear();
			enum_chunks(_avi_list->list_data, _avi_list->data_size - 4, array_chunk);

			_hdrl_list = find_list("LIST", "hdrl", array_chunk);
			if(!_hdrl_list)
				return false;

			_movi_list = find_list("LIST", "movi", array_chunk);
			if(!_movi_list)
				return false;

			_idx1_chunk = (AVIOLDINDEX*)find_chunk("idx1", array_chunk);
			if(!_idx1_chunk)
				return false;

			array_chunk.clear();
			enum_chunks(_hdrl_list->list_data, _hdrl_list->data_size - 4, array_chunk);

			_avimainheader = (AVIMAINHEADER*)find_chunk("avih", array_chunk);
			if(!_avimainheader)
				return false;

			array_chunk2.clear();
			find_lists("LIST", "strl", array_chunk, array_chunk2);
			for(i = 0; i < (int)array_chunk2.size(); ++i)
			{
				CHUNK* c = array_chunk2[i];
				array_chunk3.clear();
				enum_chunks(c->list_data, c->data_size - 4, array_chunk3);
				c = find_chunk("strh", array_chunk3);
				if(!c)
					return false;
				AVISTREAMHEADER* streamheader = (AVISTREAMHEADER*)c;
				if(!memcmp(&streamheader->fccType, "vids", 4))
				{
					_avistreamheader_vids = streamheader;
					c = find_chunk("strf", array_chunk3);
					if(c)
						_avistreamformat_vids = (BITMAPINFO*)c->data;
					c = find_chunk("strd", array_chunk3);
					if(c)
						_avistreamheaderdata_vids = c;
					break;
				}
			}

			array_chunk2.clear();
			find_lists("LIST", "strl", array_chunk, array_chunk2);
			for(i=0; i < (int)array_chunk2.size(); ++i)
			{
				CHUNK* c = array_chunk2[i];
				array_chunk3.clear();
				enum_chunks(c->list_data, c->data_size - 4, array_chunk3);
				c = find_chunk("strh", array_chunk3);
				if(!c)
					return false;
				AVISTREAMHEADER* streamheader = (AVISTREAMHEADER*)c;
				if(!memcmp(&streamheader->fccType, "auds", 4))
				{
					_avistreamheader_auds = streamheader;
					c = find_chunk("strf", array_chunk3);
					if(c)
						_avistreamformat_auds = (WAVEFORMATEX*)c->data;
					c = find_chunk("strd", array_chunk3);
					if(c)
						_avistreamheaderdata_auds = c;
					break;
				}
			}

			array_chunk.clear();
			array_chunk2.clear();
			array_chunk3.clear();

			enum_chunks(_movi_list->list_data, _movi_list->data_size - 4, array_chunk);

			for(i = 0; i < (int)array_chunk.size(); ++i)
			{
				CHUNK* c = array_chunk[i];
				if(memcmp(c->code + 2, "db", 2) == 0)
				{
					_array_vid_frame.push_back(c);
					continue;
				}
				if(memcmp(c->code + 2, "dc", 2) == 0)
				{
					_array_vid_frame.push_back(c);
					continue;
				}
				// specific to indeo
				if(memcmp(c->code + 2, "iv", 2) == 0)
				{
					_array_vid_frame.push_back(c);
					continue;
				}
				if(memcmp(c->code + 2, "wb", 2) == 0)
				{
					_array_aud_data.push_back(c);
					continue;
				}
			}

			int frame = 0;

			int last_keyframe = 0;

			int c = ((AVIOLDINDEX*)_idx1_chunk)->cb / 16;
			for(i = 0; i < c; ++i)
			{
				DWORD* entry = (DWORD*)(((AVIOLDINDEX*)_idx1_chunk)->aIndex + i);
				DWORD& chunk_id = entry[0];
				DWORD& flags = entry[1];

				if((chunk_id >> 16) != *(unsigned short*)"db" && (chunk_id >> 16) != *(unsigned short*)"dc" && (chunk_id >> 16) != *(unsigned short*)"iv")
					continue;
				if(flags & AVIIF_LIST || flags & AVIIF_NO_TIME)
					continue;
				if(flags & AVIIF_KEYFRAME)
					last_keyframe = frame;

				if(!_array_vid_frame.size())
				{
					_array_frame_offset.push_back(entry[2] + 8);
					_array_frame_size.push_back(entry[3]);
				}

				_array_frame_keyframe.push_back(last_keyframe);
				++frame;
			}
			for(i = (int)_array_frame_keyframe.size(); i < (int)_array_vid_frame.size(); ++i)
				_array_frame_keyframe.push_back(i);

			return true;
		}
		void			AVIReader::enum_chunks(const void* ptr, unsigned long length, vector<CHUNK*>& array_chunk)
		{
			char* cur_ptr = (char*)ptr;
			char* end_ptr = (char*)ptr + length;

			while(cur_ptr < end_ptr)
			{
				if(cur_ptr + 8 > end_ptr)
					return;

				CHUNK* c = (CHUNK*)cur_ptr;

				if(cur_ptr + c->data_size + 8 + (c->data_size & 1) > end_ptr)
					return;

				array_chunk.push_back(c);

				cur_ptr += c->data_size + 8 + (c->data_size & 1);
			}
		}
		void*					AVIReader::main_header() const {return _avimainheader;}
		void*					AVIReader::video_header() const {return _avistreamheader_vids;}
		void*					AVIReader::audio_header() const {return _avistreamheader_auds;}
		void*					AVIReader::video_format() const {return _avistreamformat_vids;}
		void*					AVIReader::audio_format() const {return _avistreamformat_auds;}
		void*					AVIReader::video_header_data() const {if(!_avistreamheaderdata_vids)return NULL; return _avistreamheaderdata_vids->data;}
		unsigned long			AVIReader::video_header_data_size() const {if(!_avistreamheaderdata_vids)return 0; return _avistreamheaderdata_vids->data_size;}
		void*					AVIReader::audio_header_data() const {if(!_avistreamheaderdata_auds)return NULL; return _avistreamheaderdata_auds->data;}
		unsigned long			AVIReader::audio_header_data_size() const {if(!_avistreamheaderdata_auds)return 0; return _avistreamheaderdata_auds->data_size;}
		unsigned long			AVIReader::frames() const {return max((unsigned long)_array_vid_frame.size(),(unsigned long)_array_frame_offset.size());}
		REAL					AVIReader::frame_rate() const {return (REAL)((AVISTREAMHEADER*)video_header())->dwRate / ((AVISTREAMHEADER*)video_header())->dwScale;}
		unsigned long			AVIReader::frame_dx() const {return ((BITMAPINFO*)video_format())->bmiHeader.biWidth;}
		unsigned long			AVIReader::frame_dy() const {return ((BITMAPINFO*)video_format())->bmiHeader.biHeight;}
		void*					AVIReader::get_frame_data(unsigned long index) const {if(index < 0 || index >= frames()) return NULL; return _array_vid_frame.size() ? (void*)_array_vid_frame[index]->data : (void*)(((char*)_movi_list->data) + _array_frame_offset[index]);}
		unsigned long			AVIReader::get_frame_size(unsigned long index) const {if(index < 0 || index >= frames()) return 0; return _array_vid_frame.size() ? _array_vid_frame[index]->data_size : _array_frame_size[index];}
		unsigned long			AVIReader::get_keyframe(unsigned long index) const {if(index < 0 || index >= frames()) return 0; return _array_frame_keyframe[index];}
		bool					AVIReader::is_keyframe(unsigned long index) const {if(index < 0 || index >= frames()) return false; return _array_frame_keyframe[index] == index;}
	};
};
