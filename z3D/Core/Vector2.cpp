
#include "Constants.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2.h"
#include "MathFunc.h"

namespace z3D
{
	namespace Core
	{
		Vec2::Vec2()
		{
			(*this) = zero;
		}
		Vec2::Vec2(REAL x, REAL y)
		{
			this->x = x;
			this->y = y;
		}
		Vec2::Vec2(const REAL src[2])
		{
			x = src[0];
			y = src[1];
		}
		Vec2::Vec2(const Vec2& src)
		{
			(*this) = src;
		}
		Vec2::operator Vec3() const
		{
			return Vec3(x, y, 0);
		}
		Vec2::operator Vec4() const
		{
			return Vec4(x, y, 0);
		}
		Vec2& Vec2::operator=(const Vec2& src)
		{
			memcpy(v, src.v, sizeof(v));
			return (*this);
		}
		REAL& Vec2::operator[](ptrdiff_t i)
		{
			return v[i];
		}
		REAL Vec2::operator[](ptrdiff_t i) const
		{
			return v[i];
		}
		REAL Vec2::operator*(const Vec2& right) const
		{
			return x * right.x + y * right.y;
		}
		Vec2 Vec2::operator*(REAL scalar) const
		{
			return Vec2(scalar * x, scalar * y);
		}
		Vec2& Vec2::operator*=(REAL scalar)
		{
			return (*this) = operator*(scalar);
		}
		Vec2 Vec2::operator/(REAL scalar) const
		{
			return operator*((REAL)1 / scalar);
		}
		Vec2& Vec2::operator/=(REAL scalar)
		{
			return operator*=((REAL)1 / scalar);
		}
		Vec2 Vec2::operator+(const Vec2& right) const
		{
			return Vec2(x + right.x, y + right.y);
		}
		Vec2& Vec2::operator+=(const Vec2& right)
		{
			return (*this) = operator+(right);
		}
		Vec2 Vec2::operator-(const Vec2& right) const
		{
			return Vec2(x - right.x, y - right.y);
		}
		Vec2& Vec2::operator-=(const Vec2& right)
		{
			return (*this) = operator-(right);
		}
		Vec2 Vec2::operator-() const
		{
			return Vec2(-x, -y);
		}
		Vec2		Vec2::operator*(const Mat2& right) const
		{
			return Vec2(	x * right.v00 + y * right.v10,
							x * right.v01 + y * right.v11);
		}
		Vec2&	Vec2::operator*=(const Mat2& right)
		{
			return (*this) = operator*(right);
		}
		bool Vec2::operator==(const Vec2& right) const
		{
			if(x == right.x && y == right.y)
				return true;
			else
				return false;
		}
		bool Vec2::operator!=(const Vec2& right) const
		{
			return !(operator==(right));
		}
		Vec2&			Vec2::setX			(REAL x)
		{
			this->x = x;
			return *this;
		}
		Vec2&			Vec2::setY			(REAL y)
		{
			this->y = y;
			return *this;
		}
		Vec2			Vec2::withX			(REAL x) const
		{
			return Vec2(x, y);
		}
		Vec2			Vec2::withY			(REAL y) const
		{
			return Vec2(x, y);
		}
		Vec2	Vec2::modulate(const Vec2& right) const
		{
			return Vec2(x * right.x, y * right.y);
		}
		Vec2 Vec2::rotateBy(REAL angle) const
		{
			REAL c,s;
			c=COS(angle);
			s=SIN(angle);
			return Vec2(x*c-y*s,y*c+x*s);
		}
		REAL Vec2::length() const
		{
			return SQRT(x*x+y*y);
		}
		REAL Vec2::lengthsq() const
		{
			return x*x+y*y;
		}
		Vec2 Vec2::normal() const
		{
			return (Vec2(*this))*((REAL)1/length());
			//return (Vec2(*this))*INVSQRT(lengthsq());
		}
		Vec2			Vec2::normal_safe		() const
		{
			REAL lensq = lengthsq();
			if(lensq > SMALLEST_NORMALIZABLE_VECTOR_LENGTHSQ)
			{
				Vec2 n = (*this) * (1 / SQRT(lensq));
				if(n.valid())
					return n;
			}
			return Vec2::zero;
		}
		Vec2& Vec2::normalize()
		{
			return (*this)=normal();
		}
		Vec2& Vec2::normalize_safe()
		{
			return (*this)=normal_safe();
		}
		REAL	Vec2::angle() const
		{
			Vec2 n=this->normal();
			REAL angle=ACOS(n.x);
			return SETSIGN(angle,SIGNBIT(n.y));
		}
		REAL Vec2::angle(const Vec2& other) const
		{
			return ACOS(((*this)*other)/(length()*other.length()));
		}
		bool Vec2::isEquivalent(const Vec2& other,REAL bias) const
		{
			Vec2 d=(*this)-other;
			if(FABS(d.x)<=bias&&
				FABS(d.y)<=bias)
				return true;
			return false;
		}
		bool Vec2::isParallel(const Vec2& other,REAL biasAngle) const
		{
			REAL a=angle(other);
			if(a<=biasAngle)
				return true;
			return false;
		}
		bool Vec2::split2(const Vec2& v0,const Vec2& v1,float& r0,float& r1) const
		{
			Mat2 m(v0,v1,false);
			bool bsucc=false;
			Mat2 inv_m=m.inverse(&bsucc);
			if(!bsucc)
				return false;
			Vec2 r=(*this)*inv_m;
			r0=r[0];
			r1=r[1];
			return true;
		}
		bool Vec2::valid() const
		{
			return FINITE(v[0])&FINITE(v[1]);
		}
		Vec2		Vec2::linear_mapping	(const Vec2& x0,const Vec2& x1,const Vec2& y0,const Vec2& y1,const Vec2& x)
		{
			return Vec2(
				LINEAR_MAPPING(x0[0],x1[0],y0[0],y1[0],x[0]),
				LINEAR_MAPPING(x0[1],x1[1],y0[1],y1[1],x[1])
				);
		}
		Vec2 operator*(REAL scalar,const Vec2& right)
		{
			return right*scalar;
		}
	};
};