
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Audio
	{
		class wavSndPlayerSet: public RefBase
		{
		protected:
			vector< SPtr<SndPlayer> >	_array_player;
			map<int,SPtr<SndBuf> >		_map_wave_buffer;
		protected:
			REAL						_gain;
			REAL						_pitch;
		public:
			wavSndPlayerSet(int sources);
			~wavSndPlayerSet();
		public:
			int					load(const wcs& filepath);
			bool				unload(int wave_name);
		public:
			void				play(int wave_name);
		public:
			void				release();
		public:
			void				setGain(REAL gain);
			void				setPitch(REAL pitch);
		public:
			REAL				getGain() const;
			REAL				getPitch() const;
		};
	};
};

