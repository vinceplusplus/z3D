
#include "Oal.h"

#include "openal/include/al/al.h"
#include "openal/include/al/alc.h"

namespace z3D
{
	namespace Audio
	{
		Oal::Oal()
		{
			_device=NULL;
			_context=NULL;
		}
		Oal::~Oal()
		{
			if(_context)
				alcDestroyContext((ALCcontext*)_context);
			if(_device)
				alcCloseDevice((ALCdevice*)_device);
		}
		vector<mbs>		Oal::enumDevices()
		{
			vector<mbs> devices;

			if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)
			{
				char* device_list=(char *)alcGetString(NULL, ALC_DEVICE_SPECIFIER);

				if(device_list)
					while(device_list[0])
					{
						devices.push_back(device_list);
						device_list+=strlen(device_list)+1;
					}
			}

			return devices;
		}
		SPtr<Oal>		Oal::create(const mbs& name)
		{
			ALCdevice* device;
			if(name.length())
				device=alcOpenDevice((ALchar*)name.c_str());
			else
				device=alcOpenDevice(NULL);
			if(!device)
			{
				syslog<<L"alcOpenDevice() fails";
				return NULL;
			}
			ALCcontext* context=alcCreateContext(device,NULL);
			if(!context)
			{
				syslog<<L"alcCreateContext() fails";
				alcCloseDevice(device);
				return NULL;
			}
			SPtr<Oal> oal=new Oal();
			oal->_device=device;
			oal->_context=context;
			oal->makeCurrent();

			const char* vendor=(const char*)alGetString(AL_VENDOR);
			const char* version=(const char*)alGetString(AL_VERSION);
			const char* renderer=(const char*)alGetString(AL_RENDERER);
			const char* al_extensions=(const char*)alGetString(AL_EXTENSIONS);
			if(vendor)
				syslog<<L"AL_VENDOR: " + MBSTOWCS(vendor);
			if(version)
				syslog<<L"AL_VERSION: " + MBSTOWCS(version);
			if(renderer)
				syslog<<L"AL_RENDERER: " + MBSTOWCS(renderer);
			wcs extensions;
			if(al_extensions)
			{
				syslog<<L"AL_EXTENSIONS: ";
				extensions=MBSTOWCS(al_extensions);
			}
			int p = 0;
			while(true)
			{
				int p2;
				if((p2 = (int)extensions.find(' ',p)) >= 0)
					syslog<<extensions.substr(p, p2-p);
				else
				{
					syslog<<extensions.substr(p);
					break;
				}
				p = p2 + 1;
			}

			return oal;
		}
		void		Oal::makeCurrent()
		{
			if(!this)
				return;
			if(isCurrent())
				return;
			alcMakeContextCurrent((ALCcontext*)_context);
			alcProcessContext((ALCcontext*)_context);

			singleton<Oal>::makeCurrent();
		}
		void		Oal::makeNotCurrent()
		{
			if(!this)
				return;
			if(!isCurrent())
				return;
			alcMakeContextCurrent(NULL);
			alcProcessContext(NULL);

			singleton<Oal>::makeNotCurrent();
		}
		bool		Oal::isCurrent() const
		{
			return alcGetCurrentContext()==_context;
		}
		void		Oal::setListenerPos(const Vec3& pos)
		{
			if(!this)
				return;
			if(pos.valid())
			{
				Vec3 v=pos;
				alListenerfv(AL_POSITION,(REAL*)&v);
			}
		}
		void		Oal::setListenerVel(const Vec3& vel)
		{
			if(!this)
				return;
			if(vel.valid())
			{
				Vec3 v=vel;
				alListenerfv(AL_VELOCITY,(REAL*)&v);
			}
		}
		void		Oal::setListenerOri(const Vec3& front,const Vec3& up)
		{
			if(!this)
				return;
			if(front.valid()&&up.valid())
			{
				float v[6];
				v[0]=front.x;
				v[1]=front.y;
				v[2]=front.z;
				v[3]=up.x;
				v[4]=up.y;
				v[5]=up.z;
				alListenerfv(AL_ORIENTATION,v);
			}
		}
		void		Oal::setListenerGain(REAL gain)
		{
			if(!this)
				return;
			if(FINITE(gain))
				alListenerf(AL_GAIN,gain);
		}
		Vec3		Oal::getListenerPos() const
		{
			if(!this)
				return Vec3::zero;
			float v[3];
			alGetListenerfv(AL_POSITION,v);
			return Vec3(v[0],v[1],v[2]);
		}
		Vec3		Oal::getListenerVel() const
		{
			if(!this)
				return Vec3::zero;
			float v[3];
			alGetListenerfv(AL_VELOCITY,v);
			return Vec3(v[0],v[1],v[2]);
		}
		void		Oal::getListenerOri(Vec3& front,Vec3& up) const
		{
			if(!this)
			{
				front=Vec3::zNegAxis;
				up=Vec3::yAxis;
				return;
			}
			float v[6];
			alGetListenerfv(AL_ORIENTATION,v);
			front=Vec3(v[0],v[1],v[2]);
			up=Vec3(v[3],v[4],v[5]);
		}
		REAL		Oal::getListenerGain() const
		{
			if(!this)
				return 0.0f;
			float v;
			alGetListenerfv(AL_GAIN,&v);
			return v;
		}
		void		Oal::setDM_None()
		{
			if(!this)
				return;
			alDistanceModel(AL_NONE);
		}
		void		Oal::setDM_Linear()
		{
			if(!this)
				return;
			alDistanceModel(AL_LINEAR_DISTANCE);
		}
		void		Oal::setDM_Inverse()
		{
			if(!this)
				return;
			alDistanceModel(AL_INVERSE_DISTANCE);
		}
		void		Oal::setDM_Exponent()
		{
			if(!this)
				return;
			alDistanceModel(AL_EXPONENT_DISTANCE);
		}
		unsigned int	Oal::genSource()
		{
			if(!this)
				return 0;
			ALuint name=0;
			alGenSources(1,&name);
			return name;
		}
		unsigned int	Oal::genBuffer()
		{
			if(!this)
				return 0;
			ALuint name=0;
			alGenBuffers(1,&name);
			return name;
		}
		bool	Oal::log_error(const wcs& prefix)
		{
			switch(alGetError())
			{
			case AL_NO_ERROR:
				return false;
			case AL_INVALID_NAME:
				syslog<<prefix+L"AL_INVALID_NAME";
				break;
			case AL_INVALID_ENUM:
				syslog<<prefix+L"AL_INVALID_ENUM";
				break;
			case AL_INVALID_VALUE:
				syslog<<prefix+L"AL_INVALID_VALUE";
				break;
			case AL_INVALID_OPERATION:
				syslog<<prefix+L"AL_INVALID_OPERATION";
				break;
			case AL_OUT_OF_MEMORY:
				syslog<<prefix+L"AL_OUT_OF_MEMORY";
				break;
			default:
				syslog<<prefix+L"unrecognized al error";
			}
			return true;
		}
	};
};

