
#pragma once

#include "predecl.h"

/************************************************

SndPlayer Implementation Notes:

OpenAL uses some kind of reference counting on buffers used by sources
However, alGenBuffers() will not create buffers with reference count of 1
the reference count is really used to count how many sources using the buffer

So, if you look at the source, you find that alDeleteBuffers check
if the buffers to be deleted have reference count of 0!!
If not, it will not delete the buffers

Finally, you cannot delete the buffers first then sources,
instead, you can either delete the all sources using the buffers and then delete the buffers
or set all the source using the buffers to use a NULL buffer then delete the buffers and then the sources lastly

************************************************/

namespace z3D
{
	namespace Audio
	{
		class SndPlayer: public RefBase
		{
		protected:
			unsigned int		_source_name;
		protected:
			SndPlayer(unsigned int source_name);
			virtual ~SndPlayer();
		public:
			unsigned int			source_name() const;
		public:
			virtual void			setPosition(const Vec3& pos);
			virtual void			setVelocity(const Vec3& vel);
			virtual void			setRollOff(REAL val);
			virtual void			setReferenceDistance(REAL val);
			virtual void			setMaxDistance(REAL val);
			virtual void			setPitch(REAL val);
			virtual void			setGain(REAL val);
			virtual void			setLoop(bool loop);
			virtual void			setBuffer(unsigned int buffer_name);
		public:
			virtual Vec3			getPosition() const;
			virtual Vec3			getVelocity() const;
			virtual REAL			getRollOff() const;
			virtual REAL			getReferenceDistance() const;
			virtual REAL			getMaxDistance() const;
			virtual REAL			getPitch() const;
			virtual REAL			getGain() const;
			virtual bool			getLoop() const;
			virtual unsigned int	getBuffer() const;
			virtual REAL			getOffsetSec() const;
		public:
			virtual void			play();
			virtual void			stop();
			virtual void			pause();
		public:
			virtual bool			isPlaying() const;
			virtual bool			isStopped() const;
			virtual bool			isPaused() const;
		public:
			virtual void			update();
		public:
			static SPtr<SndPlayer>	load(const wcs& filepath);
			static SPtr<SndPlayer>	create();
		};
	};
};
