
#pragma once

#include "predecl.h"

#include "SndPlayer.h"
#include "SndBuf.h"

namespace z3D
{
	namespace Audio
	{
		class ovSndPlayer: public SndPlayer
		{
		protected:
			SMem					_filemem;
			MemReader				_file;
		protected:
			vector< SPtr<SndBuf> >	_array_buffer;
		protected:
			void*					_ov_file;
			void*					_ov_info;
			void*					_ov_comment;
		protected:
			int						_format;
		protected:
			vector<char>			_buffer;
		protected:
			bool					_loop;
		protected:
			ovSndPlayer(unsigned int source_name);
		public:
			~ovSndPlayer();
		public:
			static SPtr<SndPlayer>	load(const wcs& filepath,int buffers=4);			// minimum number of buffers is 2
		public:
			void					update();
		public:
			void					play();
		public:
			void					setLoop(bool loop);
			bool					getLoop() const;
		private:
			int						stream();
		};
	};
};

