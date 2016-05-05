
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// 2-component vector
		// read about Matrix4.h for more information

		class Vec3
		{
		public:
			union {
				REAL v[3];
				struct{REAL x; REAL y; REAL z;};
			};
		public:
			static const Vec3 zero;
			static const Vec3 xAxis;
			static const Vec3 yAxis;
			static const Vec3 zAxis;
			static const Vec3 xNegAxis;
			static const Vec3 yNegAxis;
			static const Vec3 zNegAxis;
		public:
			Vec3();
			Vec3(REAL x, REAL y, REAL z);
			Vec3(const REAL src[3]);
			Vec3(const Vec3& src);
		public:
			operator Vec4() const;
		public:
			Vec3&			operator=(const Vec3& src);
			REAL&			operator[](ptrdiff_t i);
			REAL			operator[](ptrdiff_t i) const;
			REAL			operator*(const Vec3& right) const;
			Vec3			operator^(const Vec3& right) const;
			Vec3			operator*(REAL scalar) const;
			Vec3&			operator*=(REAL scalar);
			Vec3			operator/(REAL scalar) const;
			Vec3&			operator/=(REAL scalar);
			Vec3			operator+(const Vec3& right) const;
			Vec3&			operator+=(const Vec3& right);
			Vec3			operator-(const Vec3& right) const;
			Vec3&			operator-=(const Vec3& right);
			Vec3			operator-() const;
			Vec3			operator*(const Mat3& right) const;
			Vec3&			operator*=(const Mat3& right);
			Vec3			operator*(const Mat4& right) const;
			Vec3&			operator*=(const Mat4& right);
			bool			operator==(const Vec3& right) const;
			bool			operator!=(const Vec3& right) const;
			Vec3&			setX(REAL x);
			Vec3&			setY(REAL y);
			Vec3&			setZ(REAL z);
			Vec3			withX(REAL x) const;
			Vec3			withY(REAL y) const;
			Vec3			withZ(REAL z) const;
			Vec3			modulate(const Vec3& right) const;
			Vec3			rotateBy(const Vec3& axis, REAL angle) const;
			Vec3			rotateBy(const Vec3& axis, REAL angle, const Vec3& around) const;
			Vec3			rotateBy(const Quat& q) const;
			Vec3			rotateBy(const Quat& q, const Vec3& around) const;
			REAL			length() const;
			REAL			lengthsq() const;
			Vec3			normal() const;
			Vec3			normal_safe() const;
			Vec3&			normalize();
			Vec3&			normalize_safe();
			REAL			angle(const Vec3& other) const;
			bool			isEquivalent(const Vec3& other, REAL bias) const;
			bool			isParallel(const Vec3& other, REAL biasAngle) const;
			bool			split2(const Vec3& v0, const Vec3& v1, float& r0, float& r1) const;
			bool			split3(const Vec3& v0, const Vec3& v1, const Vec3& v2, float& r0, float& r1, float& r2) const;
			REAL			resolveAngle(const Vec3& axis, const Vec3& start_pos) const;
			void			resolveOriRH(REAL& yaw, REAL& pitch) const;
			Vec3			lerp(const Vec3& v1, REAL t) const;
			Vec3			slerp(const Vec3& v, REAL t) const; // assume unit vectors
			bool			valid() const;
			bool			testZeroFast(REAL threshold) const;
			Vec3			slideAlong(const Vec3& plane_normal) const;
			Vec3			reflect(const Vec3& N) const;
			Vec3			refract(const Vec3& N, float eta);
			static Vec3		linear_mapping	(const Vec3& x0, const Vec3& x1, const Vec3& y0, const Vec3& y1, const Vec3& x);
		public:
			friend Vec3		operator*(REAL scalar, const Vec3& right);
		};
	};
};

