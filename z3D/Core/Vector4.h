
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// 2-component vector
		// read about Matrix4.h for more information

		class Vec4
		{
		public:
			union {
				REAL v[4];
				struct{REAL x; REAL y; REAL z; REAL w;};
			};
		public:
			static const Vec4 zero;
			static const Vec4 xAxis;
			static const Vec4 yAxis;
			static const Vec4 zAxis;
			static const Vec4 xNegAxis;
			static const Vec4 yNegAxis;
			static const Vec4 zNegAxis;
		public:
			Vec4();
			Vec4(REAL x, REAL y, REAL z);
			Vec4(REAL x, REAL y, REAL z, REAL w);
			Vec4(const REAL src[4]);
			Vec4(const Vec3& src);
			Vec4(const Vec4& src);
			Vec4(const Vec3& src, REAL w);
		public:
			operator Vec3() const;
		public:
			Vec4&			operator=(const Vec4& src);
			REAL&			operator[](ptrdiff_t i);
			REAL			operator[](ptrdiff_t i) const;
			REAL			operator*(const Vec4& right) const;
			Vec4			operator^(const Vec4& right) const;
			Vec4			operator*(REAL scalar) const;
			Vec4&			operator*=(REAL scalar);
			Vec4			operator/(REAL scalar) const;
			Vec4&			operator/=(REAL scalar);
			Vec4			operator+(const Vec4& right) const;
			Vec4&			operator+=(const Vec4& right);
			Vec4			operator-(const Vec4& right) const;
			Vec4&			operator-=(const Vec4& right);
			Vec4			operator-() const;
			Vec4			operator*(const Mat4& right) const;
			Vec4&			operator*=(const Mat4& right);
			bool			operator==(const Vec4& right) const;
			bool			operator!=(const Vec4& right) const;
			Vec4&			setX(REAL x);
			Vec4&			setY(REAL y);
			Vec4&			setZ(REAL z);
			Vec4&			setW(REAL w);
			Vec4			withX(REAL x) const;
			Vec4			withY(REAL y) const;
			Vec4			withZ(REAL z) const;
			Vec4			withW(REAL w) const;
			Vec4			modulate(const Vec4& right) const;
			Vec4			rotateBy(const Vec4& axis, REAL angle) const;
			Vec4			rotateBy(const Quat& q) const;
			Vec4			rotateBy(const Vec4& axis, REAL angle, const Vec4& around) const;
			Vec4			rotateBy(const Quat& q, const Vec4& around) const;
			Vec4			cartesian() const;
			Vec4&			cartesianize();
			REAL			length() const;
			REAL			lengthsq() const;
			Vec4			normal() const;
			Vec4			normal_safe() const;
			Vec4&			normalize();
			Vec4&			normalize_safe();
			REAL			angle(const Vec4& other) const;
			bool			isEquivalent(const Vec4& other, REAL bias) const;
			bool			isParallel(const Vec4& other, REAL biasAngle) const;
			bool			split2(const Vec4& v0, const Vec4& v1, float& r0, float& r1) const;
			bool			split3(const Vec4& v0, const Vec4& v1, const Vec4& v2, float& r0, float& r1, float& r2) const;
			bool			valid() const;
		public:
			friend Vec4		operator*(REAL scalar, const Vec4& right);
		};
	};
};

