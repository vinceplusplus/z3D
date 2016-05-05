
#include "SndBuf.h"

#include "openal/include/al/al.h"

namespace z3D
{
	namespace Audio
	{
		SndBuf::SndBuf(const wcs& filepath,unsigned int buffer_name):ResBase(filepath)
		{
			_buffer_name=buffer_name;
		}
		SndBuf::~SndBuf()
		{
			alDeleteBuffers(1,&_buffer_name);
		}
		unsigned int		SndBuf::buffer_name() const
		{
			return _buffer_name;
		}
		REAL				SndBuf::duration() const
		{
			int freq;
			int bits;
			int channels;
			int size;
			alGetBufferi(_buffer_name,AL_FREQUENCY,&freq);
			alGetBufferi(_buffer_name,AL_BITS,&bits);
			alGetBufferi(_buffer_name,AL_CHANNELS,&channels);
			alGetBufferi(_buffer_name,AL_SIZE,&size);

			return ((REAL)size)/(freq*(bits>>3)*channels);
		}
	};
};
