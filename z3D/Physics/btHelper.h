
#pragma once

#include "predecl.h"

class btTransform;
class btMatrix3x3;

namespace z3D
{
	namespace Physics
	{
		class btHelper
		{
		public:
			static void					to(const btTransform& btxform, Mat4& xform);
			static void					to(const Mat4& xform, btTransform& btxform);
			static void					to(const btMatrix3x3& btm, Mat3& m);
			static void					to(const Mat4& m, btMatrix3x3& btm);
			static Vec3					calcInertia(const Vec3& extent, REAL mass);					// half extent in bullet terms
		};
	};
};

