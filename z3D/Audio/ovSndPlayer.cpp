
#include "openal/include/al/al.h"

#include "ovSndPlayer.h"
#include "Oal.h"

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"

static size_t read_ov  (void *ptr, size_t size, size_t nmemb, void *datasource)
{
	MemReader* reader = (MemReader*)datasource;
	if((ptrdiff_t)size * (ptrdiff_t)nmemb <= 0)
		return 0;
	ptrdiff_t items = (reader->size() - reader->offset()) / size;
	items = min(items, (ptrdiff_t)nmemb);
	reader->read(ptr, (size_t)(items * size));
	return items;
}
static int    seek_ov  (void *datasource, ogg_int64_t offset, int whence)
{
	MemReader* reader = (MemReader*)datasource;
	if(whence == SEEK_SET)
		reader->seek((size_t)offset);
	else if(whence == SEEK_CUR)
		reader->seek((size_t)(reader->offset() + offset));
	else
		reader->seek((size_t)(reader->size() + offset));
	return 0;
}
static int    close_ov (void *datasource)
{
	return 0;
}
static long   tell_ov  (void *datasource)
{
	return (long)((MemReader*)datasource)->offset();
}

namespace z3D
{
	namespace Audio
	{
		ovSndPlayer::ovSndPlayer(unsigned int source_name):SndPlayer(source_name)
		{
			_ov_file = NULL;
			_ov_info = NULL;
			_ov_comment = NULL;

			_format = AL_FORMAT_MONO8;

			_loop = false;
		}
		ovSndPlayer::~ovSndPlayer()
		{
			SndPlayer::stop();

			// do it or you have temporarily memory leak until you shutdown OpenAL
			alSourcei(_source_name, AL_BUFFER, 0);

			for(int i = 0; i < (int)_array_buffer.size(); ++i)
				_array_buffer[i].reset();
			_array_buffer.clear();

			ov_clear((OggVorbis_File*)_ov_file);
			delete (OggVorbis_File*)_ov_file;
			//delete _file;
		}
		SPtr<SndPlayer>	ovSndPlayer::load(const wcs& filepath,int buffers)
		{
			buffers = max(buffers, 2);

			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			ov_callbacks callbacks;
			callbacks.read_func = read_ov;
			callbacks.seek_func = seek_ov;
			callbacks.close_func = close_ov;
			callbacks.tell_func = tell_ov;

			OggVorbis_File* ov_file = new OggVorbis_File;

			MemReader reader(sm.get(), sm.size());

			if(ov_open_callbacks(&reader, ov_file, NULL, 0, callbacks) < 0)
			{
				delete ov_file;

				return NULL;
			}

			vorbis_info* info = ov_info(ov_file, -1);
			vorbis_comment* comment = ov_comment(ov_file, -1);

			ALenum format;

			if(info->channels == 1)
				format = AL_FORMAT_MONO16;
			else if(info->channels == 2)
				format = AL_FORMAT_STEREO16;
			else
			{
				ov_clear(ov_file);
				delete ov_file;

				return NULL;
			}

			unsigned int source_name = Oal::current()->genSource();

			ovSndPlayer* player = new ovSndPlayer(source_name);
			player->_filemem = sm;
			player->_file = MemReader(sm.get(), sm.size());
			ov_file->datasource = &player->_file;
			player->_ov_file = (void*)ov_file;
			player->_ov_info = (void*)info;
			player->_ov_comment = (void*)comment;
			player->_format = format;
			player->_array_buffer.resize(buffers);
			for(int i = 0; i < buffers; ++i)
				player->_array_buffer[i] = new SndBuf(L"", Oal::current()->genBuffer());
			player->_buffer.resize(info->rate);
			//player->_buffer.resize(info->rate * 2 * 2 / 2);

			return SPtr<SndPlayer>(player);
		}
		void				ovSndPlayer::update()
		{
			if(!this)
				return;
			ALint buffers = 0;
			alGetSourcei(_source_name, AL_BUFFERS_QUEUED, &buffers);
			if(buffers == 0)
			{
				int size, buffer_name;

				for(int i = 0; i < (int)_array_buffer.size(); ++i)
				{
					size = stream();
					if(size)
					{
						buffer_name = _array_buffer[i]->buffer_name();
						alBufferData(buffer_name, _format, &*_buffer.begin(), size, ((vorbis_info*)_ov_info)->rate);
						alSourceQueueBuffers(_source_name, 1, (const unsigned int*)&buffer_name);
					}
				}
			}
			else
			{
				int buffers = 0;
				int size, buffer_name;

				alGetSourcei(_source_name, AL_BUFFERS_PROCESSED, &buffers);
				if(buffers > 0)
				{
					alSourceUnqueueBuffers(_source_name, 1, (unsigned int*)&buffer_name);

					size = stream();
					if(size)
					{
						alBufferData(buffer_name, _format, &*_buffer.begin(), size, ((vorbis_info*)_ov_info)->rate);
						alSourceQueueBuffers(_source_name, 1, (const unsigned int*)&buffer_name);
					}
				}
			}
			if(isPaused() || isStopped())
				SndPlayer::play();
		}
		void				ovSndPlayer::play()
		{
			if(!this)
				return;
			if(isPaused())
				SndPlayer::play();
			else
			{
				SndPlayer::stop();

				for(int i = 0; i < (int)_array_buffer.size(); ++i)
				{
					int buffer;
					alGetError();
					alSourceUnqueueBuffers(_source_name, 1, (unsigned int*)&buffer);
					alGetError();
				}

				ov_raw_seek((OggVorbis_File*)_ov_file, 0);

				update();
				SndPlayer::play();
			}
		}
		void				ovSndPlayer::setLoop(bool loop)
		{
			if(!this)
				return;
			_loop = loop;
		}
		bool				ovSndPlayer::getLoop() const
		{
			if(!this)
				return false;
			return _loop;
		}
		int				ovSndPlayer::stream()
		{
			if(!this)
				return 0;
			int pos = 0;
			long bytes = 0;
			do
			{
				int current;
				bytes = ov_read((OggVorbis_File*)_ov_file, &*_buffer.begin() + pos, (int)_buffer.size() - pos, 0, 2, 1, &current);
				if(bytes > 0)
					pos += bytes;
				else if(bytes == 0)
				{
					if(_loop)
						ov_raw_seek((OggVorbis_File*)_ov_file, 0);
					else
						break;
				}
				else
					break;
			//}while(bytes>0);
			}while(true);
			return pos;
		}
	};
};
