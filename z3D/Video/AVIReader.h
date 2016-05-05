
#pragma once

#include "predecl.h"

#pragma warning(push)
#pragma warning(disable:4200)

namespace z3D
{
	namespace Video
	{
		class AVIReader: public ResBase
		{
		protected:
			static ResMgr		_res_mgr;
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
		protected:
			SMem					_filemem;
		protected:
			CHUNK*					_avi_list;
			CHUNK*					_hdrl_list;
			CHUNK*					_movi_list;
			void*					_idx1_chunk;								// AVIOLDINDEX*

			void*					_avimainheader;								// AVIMAINHEADER*
			void*					_avistreamheader_vids;						// AVISTREAMHEADER*
			void*					_avistreamheader_auds;						// AVISTREAMHEADER*

			void*					_avistreamformat_vids;						// BITMAPINFO*
			void*					_avistreamformat_auds;						// WAVEFORMATEX*

			CHUNK*					_avistreamheaderdata_vids;
			CHUNK*					_avistreamheaderdata_auds;

			vector<CHUNK*>			_array_vid_frame;
			vector<CHUNK*>			_array_aud_data;

			vector<unsigned long>	_array_frame_keyframe;
			// used when zero video frames found in movi list
			vector<unsigned long>	_array_frame_offset;
			vector<unsigned long>	_array_frame_size;
		protected:
			AVIReader(const wcs& filepath);
			~AVIReader();
		public:
			static SPtr<AVIReader>	load(const wcs& filepath);
			static SPtr<AVIReader>	loadNoCache(const wcs& filepath);
		protected:
			static CHUNK*			find_list(char* code,char* type_code,const vector<CHUNK*>& array_chunk);
			static CHUNK*			find_chunk(char* code,const vector<CHUNK*>& array_chunk);
			static void				find_lists(char* code,char* type_code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2);
			static void				find_chunks(char* code,const vector<CHUNK*>& array_chunk,vector<CHUNK*>& array_chunk2);
		protected:
			bool					parse();
			void					enum_chunks(const void* ptr,unsigned long length,vector<CHUNK*>& array_chunk);
		public:
			void*					main_header() const;						// AVIMAINHEADER*
			void*					video_header() const;						// AVISTREAMHEADER*
			void*					audio_header() const;						// AVISTREAMHEADER*
		public:
			void*					video_format() const;						// BITMAPINFO*
			void*					audio_format() const;						// WAVEFORMATEX*
		public:
			void*					video_header_data() const;
			unsigned long			video_header_data_size() const;
			void*					audio_header_data() const;
			unsigned long			audio_header_data_size() const;
		public:
			unsigned long			frames() const;
			REAL					frame_rate() const;
			unsigned long			frame_dx() const;
			unsigned long			frame_dy() const;
		public:						// zero-based index
			void*					get_frame_data(unsigned long index) const;
			unsigned long			get_frame_size(unsigned long index) const;
			unsigned long			get_keyframe(unsigned long index) const;
			bool					is_keyframe(unsigned long index) const;
		};
	};
};

#pragma warning(pop)
