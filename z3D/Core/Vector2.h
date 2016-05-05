
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		// 2-component vector
		// read about Matrix4.h for more information

		class Vec2
		{
		public:
			union {
				REAL v[2];
				struct{REAL x; REAL y;};
			};
		public:
			static const Vec2 zero;
			static const Vec2 xAxis;
			static const Vec2 yAxis;
			static const Vec2 xNegAxis;
			static const Vec2 yNegAxis;
		public:
			Vec2();
			Vec2(REAL x,REAL y);
			Vec2(const REAL src[2]);
			Vec2(const Vec2& src);
		public:
			operator Vec3() const;
			operator Vec4() const;
		public:
			Vec2&			operator=		(const Vec2& src);
			REAL&			operator[]		(ptrdiff_t i);
			REAL			operator[]		(ptrdiff_t i) const;
			REAL			operator*		(const Vec2& right) const;
			Vec2			operator*		(REAL scalar) const;
			Vec2&			operator*=		(REAL scalar);
			Vec2			operator/		(REAL scalar) const;
			Vec2&			operator/=		(REAL scalar);
			Vec2			operator+		(const Vec2& right) const;
			Vec2&			operator+=		(const Vec2& right);
			Vec2			operator-		(const Vec2& right) const;
			Vec2&			operator-=		(const Vec2& right);
			Vec2			operator-		() const;
			Vec2			operator*(const Mat2& right) const;
			Vec2&			operator*=(const Mat2& right);
			bool			operator==		(const Vec2& right) const;
			bool			operator!=		(const Vec2& right) const;
			Vec2&			setX			(REAL x);
			Vec2&			setY			(REAL y);
			Vec2			withX			(REAL x) const;
			Vec2			withY			(REAL y) const;
			Vec2			modulate		(const Vec2& right) const;
			Vec2			rotateBy		(REAL angle) const;
			REAL			length			() const;
			REAL			lengthsq		() const;
			Vec2			normal			() const;
			Vec2			normal_safe		() const;
			Vec2&			normalize		();
			Vec2&			normalize_safe	();
			REAL			angle			() const;
			REAL			angle			(const Vec2& other) const;
			bool			isEquivalent	(const Vec2& other, REAL bias) const;
			bool			isParallel		(const Vec2& other, REAL biasAngle) const;
			bool			split2			(const Vec2& v0, const Vec2& v1, float& r0, float& r1) const;
			bool			valid() const;
			static Vec2		linear_mapping	(const Vec2& x0, const Vec2& x1, const Vec2& y0, const Vec2& y1, const Vec2& x);
		public:
			friend Vec2		operator*		(REAL scalar, const Vec2& right);
		};
	};
};

