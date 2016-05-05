
#pragma once

#include "predecl.h"
#include "Vector3.h"

namespace z3D
{
	namespace Core
	{
		// 3x3 matrix
		// read about Matrix4.h for more information

		class Mat3
		{
		public:
			union
			{
				REAL v[3][3];
				struct
				{
					REAL v00, v01, v02;
					REAL v10, v11, v12;
					REAL v20, v21, v22;
				};
				struct
				{
					REAL a11, a12, a13;
					REAL a21, a22, a23;
					REAL a31, a32, a33;
				};
			};
		public:
			static const Mat3 identity;
		public:
			Mat3();
			Mat3(const REAL v[3][3]);
			Mat3(const Mat3& src);
			Mat3(const Vec3& v0, const Vec3& v1, const Vec3& v2, bool bRowByRow = true);
			Mat3(const Vec3* const v, bool bRowByRow = true);
		public:
			operator Mat4() const;
		public:
			Mat3&			operator=(const Mat3& other);
			REAL*			operator[](ptrdiff_t row);
			const REAL*		operator[](ptrdiff_t row) const;
			Mat3&			setIdentity();
			Mat3			transpose() const;
			Mat3&			operator+=(const Mat3& right);
			Mat3			operator+(const Mat3& right) const;
			Mat3&			operator-=(const Mat3& right);
			Mat3			operator-(const Mat3& right) const;
			Mat3&			operator*=(const Mat3& right);
			Mat3			operator*(const Mat3& right) const;
			Mat3&			operator*=(REAL scalar);
			Mat3			operator*(REAL scalar) const;
			Mat3			inverse(bool* invertible = 0) const;
			REAL			det3() const;
			static Mat3		identityMatrix();
			static Mat3		skew(const Vec3& u);
			static Mat3		rotation(const Vec3& axis, REAL angle);
			static Mat3		scaling(REAL sx, REAL sy, REAL sz);
			static Mat3		scaling(const Vec3& scale);
			static Mat3		mirror(const Vec3& n);
			static Mat3		rotationX(REAL angle);
			static Mat3		rotationY(REAL angle);
			static Mat3		rotationZ(REAL angle);
			Mat3&			rotate(const Vec3& axis, REAL angle);
			Mat3&			scale(REAL sx, REAL sy, REAL sz);
			Mat3&			scale(const Vec3& scale);
			Mat3&			setRotation(const Vec3& axis, REAL angle);
			Mat3&			setScaling(REAL sx, REAL sy, REAL sz);
			Mat3&			setScaling(const Vec3& scale);
			void			factorXYZ(REAL& rx, REAL& ry, REAL& rz) const;								// factor??? from 3D Game Engine Design, untested
			void			factorXZY(REAL& rx, REAL& rz, REAL& ry) const;
			void			factorYXZ(REAL& ry, REAL& rx, REAL& rz) const;
			void			factorYZX(REAL& ry, REAL& rz, REAL& rx) const;
			void			factorZXY(REAL& rz, REAL& rx, REAL& ry) const;
			void			factorZYX(REAL& rz, REAL& ry, REAL& rx) const;
			void			resolveOriRH(REAL& yaw, REAL& pitch) const;									// assuming zero roll
			void			factorSR(Vec3& scale, Mat3& rot) const;
			void			factorSR(Vec3& scale, Quat& rot) const;
			bool			testIdentity(REAL threshold = 0.0001f) const;
		public:
			friend Mat3		operator*(REAL scalar, const Mat3& right);
		};
	};
};

