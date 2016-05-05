
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmreg.h>

#include "wavSndPlayer.h"
#include "Oal.h"
#include "WaveLoader.h"

#include "openal/include/al/al.h"

#undef min
#undef max

namespace z3D
{
	namespace Audio
	{
		wavSndPlayer::wavSndPlayer(unsigned int source_name,const SPtr<SndBuf>& buffer):SndPlayer(source_name)
		{
			_buffer = buffer;

			alSourcei(_source_name, AL_BUFFER, _buffer->buffer_name());
		}
		wavSndPlayer::~wavSndPlayer()
		{
			SndPlayer::stop();

			// do it or you have temporarily memory leak until you shutdown OpenAL
			alSourcei(_source_name, AL_BUFFER, 0);

			_buffer.reset();
		}
		SPtr<SndPlayer>	wavSndPlayer::load(const wcs& filepath)
		{
			ResBase* res;
			if((res = _res_mgr.find(filepath)) != NULL)
				return new wavSndPlayer(Oal::current()->genSource(), (SndBuf*)res);

			SPtr<SndBuf> sndbuf = loadWaveOnly(filepath);

			if(!sndbuf)
				return NULL;

			_res_mgr.add(sndbuf.get());
			return new wavSndPlayer(Oal::current()->genSource(), sndbuf);
		}
		SPtr<SndBuf>		wavSndPlayer::loadWaveOnly(const wcs& filepath,bool force_stereo)
		{
			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;
			unsigned short channels;
			unsigned short sample_bits;
			unsigned long samples_per_sec;
			vector<char> data;
			WaveLoader::load(sm.get(), sm.size(), channels, sample_bits, samples_per_sec, data);

			sm.reset();

			if(!data.size())
			{
				syslog << filepath + L", alutLoadMemoryFromFileImage() fails()";
				//syslog<<MBSTOWCS(alutGetErrorString(alutGetError()));
				return NULL;
			}

			ALenum format = AL_FORMAT_MONO8;
			if(channels == 1)
				format = (sample_bits == 16) ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
			else
				format = (sample_bits == 16) ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;

			unsigned int buffer_name = Oal::current()->genBuffer();
			if(!buffer_name)
			{
				syslog << filepath + L", Oal::genbuffer() fails";
				return NULL;
			}
			if(!force_stereo || (format == AL_FORMAT_STEREO8 || format == AL_FORMAT_STEREO16))
				alBufferData(buffer_name, format, &data[0], (ALsizei)data.size(), (ALsizei)samples_per_sec);
			else
			{
				if(format == AL_FORMAT_MONO8)
				{
					int size = (int)data.size();
					vector<char> data2;
					data2.resize(size * 2);
					int i;
					for(i = 0; i < (int)size; ++i)
					{
						data2[(i << 1)] = data[i];
						data2[(i << 1) + 1] = data[i];
					}
					alBufferData(buffer_name, AL_FORMAT_STEREO8, &*data2.begin(), size * 2, (ALsizei)samples_per_sec);
				}
				else if(format == AL_FORMAT_MONO16)
				{
					int size = (int)data.size();
					short* data3 = (short*)&data[0];
					vector<short> data2;
					int size2 = size >> 1;
					data2.resize(size2 * 2);
					int i;
					for(i = 0; i < size2; ++i)
					{
						data2[(i << 1)] = data3[i];
						data2[(i << 1) + 1] = data3[i];
					}
					alBufferData(buffer_name, AL_FORMAT_STEREO16, &*data2.begin(), size2 * 4, (ALsizei)samples_per_sec);
				}
			}
			Oal::current()->log_error(filepath + L" alBufferData ");

			SndBuf* sndbuf = new SndBuf(filepath, buffer_name);
			return SPtr<SndBuf>(sndbuf);
		}
	};
};
