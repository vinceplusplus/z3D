
#include "AnimKey.h"

namespace z3D
{
	namespace Core
	{
		template<>
		REAL			AnimKey<REAL>::lerp(const AnimKey<REAL>& other, REAL now) const
		{
			if(time == other.time)
				return this->key;
			REAL t = (now - time) / (other.time - time);
			return key * (1 - t) + other.key * t;
		}

		template<>
		Vec3			AnimKey<Vec3>::lerp(const AnimKey<Vec3>& other, REAL now) const
		{
			if(time == other.time)
				return this->key;
			REAL t = (now - time) / (other.time - time);
			return (1 - t) * key + t * other.key;
		}

		template<>
		Quat			AnimKey<Quat>::lerp(const AnimKey<Quat>& other, REAL now) const
		{
			if(time == other.time)
				return this->key;
			REAL t = (now - time) / (other.time - time);
			return key.slerp(other.key, t);
		}
	};
};

