
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class LOD
		{
		public:
			typedef enum _INTERPOLATION
			{
				INTERPOLATION_NOT_SPECIFIED = 0,
				INTERPOLATION_STEP,
				INTERPOLATION_LINEAR,
			} INTERPOLATION;

			typedef enum _TRANSITION
			{
				TRANSITION_NOT_SPECIFIED = 0,
				TRANSITION_NONE,
				TRANSITION_BLEND_CAPTURED,
				TRANSITION_MULTISTAGE_BLEND,
			} TRANSITION;

		private:
			static INTERPOLATION			_force_interpolation;
			static TRANSITION				_force_transition;
		public:
			static void						setForcedInterpolation(LOD::INTERPOLATION interpolation);
			static LOD::INTERPOLATION		getForcedInterpolation();
			static bool						isForceInterpolation();
		public:
			static void						setForcedTransition(LOD::TRANSITION transition);
			static LOD::TRANSITION			getForcedTransition();
			static bool						isForceTransition();
		};
	};
};

