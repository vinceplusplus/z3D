
#include "LOD.h"

namespace z3D
{
	namespace Core
	{
		void								LOD::setForcedInterpolation(LOD::INTERPOLATION interpolation)
		{
			_force_interpolation = interpolation;
		}
		LOD::INTERPOLATION					LOD::getForcedInterpolation()
		{
			return _force_interpolation;
		}
		bool								LOD::isForceInterpolation()
		{
			return _force_interpolation != INTERPOLATION_NOT_SPECIFIED;
		}
		void								LOD::setForcedTransition(LOD::TRANSITION transition)
		{
			_force_transition = transition;
		}
		LOD::TRANSITION						LOD::getForcedTransition()
		{
			return _force_transition;
		}
		bool								LOD::isForceTransition()
		{
			return _force_transition != TRANSITION_NOT_SPECIFIED;
		}
	};
};

