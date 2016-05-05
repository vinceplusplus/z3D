
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Audio
	{
		class Oal: public RefBase, public singleton<Oal>
		{
		protected:
			//ALCdevice*		_device;
			//ALCcontext*		_context;
			void*			_device;
			void*			_context;
		protected:
			static Oal*		_current;
		protected:
			Oal();
			virtual ~Oal();
		public:
			static vector<mbs>		enumDevices();
			static SPtr<Oal>		create(const mbs& name);
		public:
			void			makeCurrent();
			void			makeNotCurrent();
			bool			isCurrent() const;
		public:
			void			setListenerPos(const Vec3& pos);
			void			setListenerVel(const Vec3& vel);
			void			setListenerOri(const Vec3& front,const Vec3& up);
			void			setListenerGain(REAL gain);
		public:
			Vec3			getListenerPos() const;
			Vec3			getListenerVel() const;
			void			getListenerOri(Vec3& front,Vec3& up) const;
			REAL			getListenerGain() const;
		public:
			void			setDM_None();
			void			setDM_Linear();
			void			setDM_Inverse();
			void			setDM_Exponent();
		public:
			unsigned int	genSource();
			unsigned int	genBuffer();
		public:
			static bool		log_error(const wcs& prefix);
		};
	};
};
