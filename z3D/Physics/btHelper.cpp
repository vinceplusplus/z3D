
#include "btHelper.h"

#include "../Core/z_off_debug_new.h"
#include "../../bullet/btBulletDynamicsCommon.h"
#include "../Core/z_on_debug_new.h"

namespace z3D
{
	namespace Physics
	{
		void					btHelper::to(const btTransform& btxform, Mat4& xform)
		{
			const btMatrix3x3& m = btxform.getBasis();
			const btVector3& v = btxform.getOrigin();

			xform[0][0] = m[0][0];xform[0][1] = m[1][0];xform[0][2] = m[2][0];
			xform[1][0] = m[0][1];xform[1][1] = m[1][1];xform[1][2] = m[2][1];
			xform[2][0] = m[0][2];xform[2][1] = m[1][2];xform[2][2] = m[2][2];
			xform[3][0] = v[0];xform[3][1] = v[1];xform[3][2] = v[2];
			xform[3][3] = 1;
			xform[0][3] = 0;
			xform[1][3] = 0;
			xform[2][3] = 0;
		}
		void					btHelper::to(const Mat4& xform, btTransform& btxform)
		{
			btMatrix3x3& m = btxform.getBasis();
			btVector3& v = btxform.getOrigin();

			m[0][0] = xform[0][0];m[1][0] = xform[0][1];m[2][0] = xform[0][2];
			m[0][1] = xform[1][0];m[1][1] = xform[1][1];m[2][1] = xform[1][2];
			m[0][2] = xform[2][0];m[1][2] = xform[2][1];m[2][2] = xform[2][2];
			v[0] = xform[3][0];v[1] = xform[3][1];v[2] = xform[3][2];
		}
		void					btHelper::to(const btMatrix3x3& btm, Mat3& m)
		{
			m[0][0] = btm[0][0]; m[0][1] = btm[1][0]; m[0][2] = btm[2][0];
			m[1][0] = btm[0][1]; m[1][1] = btm[1][1]; m[1][2] = btm[2][1];
			m[2][0] = btm[0][2]; m[2][1] = btm[1][2]; m[2][2] = btm[2][2];
		}
		void					btHelper::to(const Mat4& m, btMatrix3x3& btm)
		{
			btm[0][0] = m[0][0]; btm[1][0] = m[0][1]; btm[2][0] = m[0][2];
			btm[0][1] = m[1][0]; btm[1][1] = m[1][1]; btm[2][1] = m[1][2];
			btm[0][2] = m[2][0]; btm[1][2] = m[2][1]; btm[2][2] = m[2][2];
		}

		Vec3					btHelper::calcInertia(const Vec3& extent, REAL mass)
		{
			// code from bullet
			// approximate by box

			Vec3 inertia;

			REAL lx = 2.0f * extent.x;
			REAL ly = 2.0f * extent.y;
			REAL lz = 2.0f * extent.z;

			inertia[0] = mass / 12.0f * (ly * ly + lz * lz);
			inertia[1] = mass / 12.0f * (lx * lx + lz * lz);
			inertia[2] = mass / 12.0f * (lx * lx + ly * ly);

			return inertia;
		}
	};
};

