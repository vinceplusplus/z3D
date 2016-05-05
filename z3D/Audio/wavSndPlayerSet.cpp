
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "wavSndPlayerSet.h"
#include "Oal.h"
#include "wavSndPlayer.h"

#include "openal/include/al/al.h"

#undef min
#undef max

namespace z3D
{
	namespace Audio
	{
		wavSndPlayerSet::wavSndPlayerSet(int sources)
		{
			int i;
			for(i=0;i<sources;++i)
			{
				SPtr<SndPlayer> player=SndPlayer::create();
				if(player)
				{
					player->setLoop(false);
					_array_player.push_back(player);
				}
			}
		}
		wavSndPlayerSet::~wavSndPlayerSet()
		{
			int i;
			for(i=0;i<(int)_array_player.size();++i)
			{
				_array_player[i]->stop();
				alSourcei(_array_player[i]->source_name(),AL_BUFFER,0);
				_array_player[i].reset();
			}
			_array_player.clear();

			release();
		}
		int				wavSndPlayerSet::load(const wcs& filepath)
		{
			map<int,SPtr<SndBuf> >::iterator iter;

			// forcing stereo because openal(or directsound3d) has a bug that a source after playing a stereo buffer cannot play a mono buffer(no error generated, just simply not audible)
			SPtr<SndBuf> buf=wavSndPlayer::loadWaveOnly(filepath,true);
			if(buf)
			{
				_map_wave_buffer[buf->buffer_name()]=buf;
				return buf->buffer_name();
			}
			else
				syslog<<FORMATW(L"wavSndPlayerSet::load() cannot load \"%s\"",filepath.c_str());
			return 0;
		}
		void				wavSndPlayerSet::play(int wave_name)
		{
			if(!_array_player.size())
				return;
			if(_map_wave_buffer.find(wave_name)==_map_wave_buffer.end())
			{
				syslog<<FORMATW(L"wavSndPlayerSet::play() can't find %d to play",wave_name);
				return;
			}
			if(!_array_player.size())
			{
				syslog<<FORMATW(L"wavSndPlayerSet::play() find empty source list");
				return;
			}

			SPtr<SndBuf> buf=_map_wave_buffer[wave_name];

			REAL remaining=INFINITY;
			SPtr<SndPlayer> player=NULL;
			int i;
			for(i=0;i<(int)_array_player.size();++i)
			{
				SPtr<SndPlayer> player2=_array_player[i];
				if(player2->isPlaying())
				{
					if(!_map_wave_buffer.count(player2->getBuffer()))
						continue;
					SPtr<SndBuf> buf2=_map_wave_buffer[player2->getBuffer()];
					REAL remaining2=(buf2->duration()-player2->getOffsetSec())/player2->getPitch();
					if(remaining2<remaining)
					{
						player=player2;
						remaining=remaining2;
					}
				}
				else
				{
					player=player2;
					remaining=0;
					break;
				}
			}
			if(!player)
			{
				syslog<<L"wavSndPlayerSet::play(), searching for available source produces no result";
				return;
			}
			player->stop();
			player->setBuffer(buf->buffer_name());
			player->play();
		}
		void				wavSndPlayerSet::release()
		{
			int i;
			for(i=0;i<(int)_array_player.size();++i)
			{
				_array_player[i]->stop();
				alSourcei(_array_player[i]->source_name(),AL_BUFFER,0);
			}
			map<int,SPtr<SndBuf> >::iterator iter;
			for(iter=_map_wave_buffer.begin();iter!=_map_wave_buffer.end();++iter)
				iter->second.reset();
			_map_wave_buffer.clear();
		}
		bool				wavSndPlayerSet::unload(int wave_name)
		{
			if(!_map_wave_buffer.count(wave_name))
				return false;
			SPtr<SndBuf> buf=_map_wave_buffer[wave_name];
			int i;
			for(i=0;i<(int)_array_player.size();++i)
			{
				if(_array_player[i]->getBuffer()==buf->buffer_name())
				{
					_array_player[i]->stop();
					while(!_array_player[i]->isStopped())
						Sleep(1);
				}
			}
			_map_wave_buffer.erase(wave_name);
			buf.reset();

			return true;
		}
		void				wavSndPlayerSet::setGain(REAL gain)
		{
			_gain=gain;
			for(int i=0;i<(int)_array_player.size();++i)
				_array_player[i]->setGain(gain);
		}
		void				wavSndPlayerSet::setPitch(REAL pitch)
		{
			_pitch=pitch;
			for(int i=0;i<(int)_array_player.size();++i)
				_array_player[i]->setPitch(pitch);
		}
		REAL				wavSndPlayerSet::getGain() const
		{
			return _gain;
		}
		REAL				wavSndPlayerSet::getPitch() const
		{
			return _pitch;
		}
	};
};

