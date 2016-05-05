
#include "SndPlayer.h"
#include "Oal.h"
#include "wavSndPlayer.h"
#include "ovSndPlayer.h"

#include "openal/include/al/al.h"

namespace z3D
{
	namespace Audio
	{
		SndPlayer::SndPlayer(unsigned int source_name)
		{
			_source_name=source_name;
		}
		SndPlayer::~SndPlayer()
		{
			alDeleteSources(1,&_source_name);
		}
		unsigned int		SndPlayer::source_name() const
		{
			if(!this)
				return 0;
			return _source_name;
		}
		void				SndPlayer::setPosition(const Vec3& pos)
		{
			if(!this)
				return;
			if(pos.valid())
			{
				Vec3 v=pos;
				alSourcefv(_source_name,AL_POSITION,(REAL*)&v);
			}
		}
		void				SndPlayer::setVelocity(const Vec3& vel)
		{
			if(!this)
				return;
			if(vel.valid())
			{
				Vec3 v=vel;
				alSourcefv(_source_name,AL_VELOCITY,(REAL*)&v);
			}
		}
		void				SndPlayer::setRollOff(REAL val)
		{
			if(!this)
				return;
			if(FINITE(val))
				alSourcef(_source_name,AL_ROLLOFF_FACTOR,val);
		}
		void				SndPlayer::setReferenceDistance(REAL val)
		{
			if(!this)
				return;
			if(FINITE(val))
				alSourcef(_source_name,AL_REFERENCE_DISTANCE,val);
		}
		void				SndPlayer::setMaxDistance(REAL val)
		{
			if(!this)
				return;
			if(FINITE(val))
				alSourcef(_source_name,AL_MAX_DISTANCE,val);
		}
		void				SndPlayer::setPitch(REAL val)
		{
			if(!this)
				return;
			if(FINITE(val))
				alSourcef(_source_name,AL_PITCH,val);
		}
		void				SndPlayer::setGain(REAL val)
		{
			if(!this)
				return;
			if(FINITE(val))
				alSourcef(_source_name,AL_GAIN,val);
		}
		void				SndPlayer::setLoop(bool loop)
		{
			if(!this)
				return;
			alSourcei(_source_name,AL_LOOPING,loop?AL_TRUE:AL_FALSE);
		}
		void				SndPlayer::setBuffer(unsigned int buffer_name)
		{
			if(!this)
				return;
			alSourcei(_source_name,AL_BUFFER,buffer_name);
		}
		Vec3				SndPlayer::getPosition() const
		{
			if(!this)
				return Vec3::zero;
			float v[3];
			alGetSourcefv(_source_name,AL_POSITION,v);
			return Vec3(v);
		}
		Vec3				SndPlayer::getVelocity() const
		{
			if(!this)
				return Vec3::zero;
			float v[3];
			alGetSourcefv(_source_name,AL_VELOCITY,v);
			return Vec3(v);
		}
		REAL				SndPlayer::getRollOff() const
		{
			if(!this)
				return 0.0f;
			float v;
			alGetSourcef(_source_name,AL_ROLLOFF_FACTOR,&v);
			return v;
		}
		REAL				SndPlayer::getReferenceDistance() const
		{
			if(!this)
				return 0.0f;
			float v;
			alGetSourcef(_source_name,AL_REFERENCE_DISTANCE,&v);
			return v;
		}
		REAL				SndPlayer::getMaxDistance() const
		{
			if(!this)
				return 0.0f;
			float v;
			alGetSourcef(_source_name,AL_MAX_DISTANCE,&v);
			return v;
		}
		REAL				SndPlayer::getPitch() const
		{
			if(!this)
				return 0.0f;
			float v;
			alGetSourcef(_source_name,AL_PITCH,&v);
			return v;
		}
		REAL				SndPlayer::getGain() const
		{
			if(!this)
				return 0.0f;
			float v;
			alGetSourcef(_source_name,AL_GAIN,&v);
			return v;
		}
		bool				SndPlayer::getLoop() const
		{
			if(!this)
				return false;
			ALint v;
			alGetSourcei(_source_name,AL_LOOPING,&v);
			return v==AL_TRUE?true:false;
		}
		unsigned int		SndPlayer::getBuffer() const
		{
			if(!this)
				return 0;
			ALint v;
			alGetSourcei(_source_name,AL_BUFFER,&v);
			return v;
		}
		REAL				SndPlayer::getOffsetSec() const
		{
			if(!this)
				return 0.0f;
			REAL sec;
			alGetSourcef(_source_name,AL_SEC_OFFSET,&sec);
			return sec;
		}
		void				SndPlayer::play()
		{
			if(!this)
				return;
			alSourcePlay(_source_name);
		}
		void				SndPlayer::stop()
		{
			if(!this)
				return;
			alSourceStop(_source_name);
		}
		void				SndPlayer::pause()
		{
			if(!this)
				return;
			alSourcePause(_source_name);
		}
		bool				SndPlayer::isPlaying() const
		{
			if(!this)
				return false;
			ALint v;
			alGetSourcei(_source_name,AL_SOURCE_STATE,&v);
			return v==AL_PLAYING;
		}
		bool				SndPlayer::isStopped() const
		{
			if(!this)
				return true;
			ALint v;
			alGetSourcei(_source_name,AL_SOURCE_STATE,&v);
			return v==AL_STOPPED;
		}
		bool				SndPlayer::isPaused() const
		{
			if(!this)
				return true;
			ALint v;
			alGetSourcei(_source_name,AL_SOURCE_STATE,&v);
			return v==AL_PAUSED;
		}
		void				SndPlayer::update() {}
		SPtr<SndPlayer>	SndPlayer::load(const wcs& filepath)
		{
			if(filepath.length()<4)
				return NULL;
			if(_wcsicmp(filepath.substr(filepath.length()-4).c_str(),L".wav")==0)
				return wavSndPlayer::load(filepath);
			if(_wcsicmp(filepath.substr(filepath.length()-4).c_str(),L".ogg")==0)
				return ovSndPlayer::load(filepath);
			return NULL;
		}
		SPtr<SndPlayer>	SndPlayer::create()
		{
			unsigned int name=Oal::current()->genSource();
			if(name)
				return SPtr<SndPlayer>(new SndPlayer(name));
			return NULL;
		}
	};
};
