
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class Quat
		{
		public:
			union {
				REAL v[4];
				struct{REAL x; REAL y; REAL z; REAL w;};
			};
		public:
			Quat();
			Quat(REAL x, REAL y, REAL z);
			Quat(REAL x, REAL y, REAL z, REAL w);
			Quat(const REAL src[4]);
			Quat(const Vec3& v);
			Quat(const Vec3& axis, REAL angle);
			Quat(const Mat3& m);
			Quat(const Mat4& m);
		public:
			Quat&			operator	=	(const Quat& src);
			REAL&			operator	[]	(int i);
			REAL			operator	[]	(int i) const;
			Quat			operator	*	(const Quat& right) const;
			Quat&			operator	*=	(const Quat& right);
			Quat			operator	*	(REAL scalar) const;
			Quat			operator	+	(const Quat& right) const;
			Quat&			operator	+=	(const Quat& right);
			Quat			operator	-	(const Quat& right) const;
			Quat&			operator	-=	(const Quat& right);
			Quat			operator	-	() const;
			bool			operator	==	(const Quat& right) const;
			bool			operator	!=	(const Quat& right) const;
			REAL			dotProduct(const Quat& right) const;
			Quat			conjugate() const;
			REAL			norm() const;
			Quat			normal() const;
			Quat&			normalize();
			Mat3			asMat3() const;
			Mat4			asMat4() const;
			void			toAngleAxis(Vec3& axis, REAL& angle) const;
			Quat			slerp(const Quat& q1, REAL t) const;
			friend Quat		operator*(REAL scalar, const Quat& right);
			bool			valid() const;
		};
	};
};

