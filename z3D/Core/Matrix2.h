
#pragma once

#include "predecl.h"
#include "Vector2.h"

namespace z3D
{
	namespace Core
	{
		// 2x2 matrix
		// read about Matrix4.h for more information

		class Mat2
		{
		public:
			union
			{
				REAL v[2][2];
				struct
				{
					REAL v00, v01;
					REAL v10, v11;
				};
				struct
				{
					REAL a11, a12;
					REAL a21, a22;
				};
			};
		public:
			static const Mat2 identity;
		public:
			Mat2();
			Mat2(const REAL v[2][2]);
			Mat2(const Mat2& src);
			Mat2(const Vec2& v0, const Vec2& v1, bool bRowByRow = true);
			Mat2(const Vec2* const v, bool bRowByRow = true);
		public:
			operator Mat3() const;
			operator Mat4() const;
		public:
			Mat2&			operator=(const Mat2& other);
			REAL*			operator[](ptrdiff_t row);
			const REAL*		operator[](ptrdiff_t row) const;
			Mat2&			setIdentity();
			Mat2			transpose() const;
			Mat2&			operator+=(const Mat2& right);
			Mat2			operator+(const Mat2& right) const;
			Mat2&			operator-=(const Mat2& right);
			Mat2			operator-(const Mat2& right) const;
			Mat2&			operator*=(const Mat2& right);
			Mat2			operator*(const Mat2& right) const;
			Mat2&			operator*=(REAL scalar);
			Mat2			operator*(REAL scalar) const;
			Mat2			inverse(bool* invertible = 0) const;
			REAL			det2() const;
			static Mat2		identityMatrix();
			static Mat2		rotation(REAL angle);
			static Mat2		scaling(REAL sx, REAL sy);
			Mat2&			rotate(REAL angle);
			Mat2&			scale(REAL sx, REAL sy);
			Mat2&			setRotation(REAL angle);
			Mat2&			setScaling(REAL sx, REAL sy);
		public:
			friend Mat2		operator*(REAL scalar, const Mat2& right);
		};
	};
};

