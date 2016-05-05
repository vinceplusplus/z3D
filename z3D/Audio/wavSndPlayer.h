
#pragma once

#include "predecl.h"

#include "SndPlayer.h"
#include "SndBuf.h"

namespace z3D
{
	namespace Audio
	{
		class wavSndPlayer: public SndPlayer
		{
		protected:
			static ResMgr			_res_mgr;
		protected:
			SPtr<SndBuf>			_buffer;
		protected:
			wavSndPlayer(unsigned int source_name,const SPtr<SndBuf>& buffer);
		public:
			~wavSndPlayer();
		public:
			static SPtr<SndPlayer>	load(const wcs& filepath);
			static SPtr<SndBuf>		loadWaveOnly(const wcs& filepath,bool force_stereo=false);
		};
	};
};

