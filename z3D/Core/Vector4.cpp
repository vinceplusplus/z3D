
#include "MathFunc.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"

namespace z3D
{
	namespace Core
	{
		Vec4::Vec4()
		{
			(*this) = zero;
		}
		Vec4::Vec4(REAL x, REAL y, REAL z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			w = (REAL)1.0;
		}
		Vec4::Vec4(REAL x, REAL y, REAL z, REAL w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
		Vec4::Vec4(const REAL src[4])
		{
			x = src[0];
			y = src[1];
			z = src[2];
			w = src[3];
		}
		Vec4::Vec4(const Vec3& src)
		{
			x = src.x;
			y = src.y;
			z = src.z;
			w = (REAL)1.0;
		}
		Vec4::Vec4(const Vec4& src)
		{
			(*this) = src;
		}
		Vec4::Vec4(const Vec3& src,REAL w)
		{
			x = src.x;
			y = src.y;
			z = src.z;
			this->w = w;
		}
		Vec4::operator Vec3() const
		{
			Vec4 v = this->cartesian();
			return Vec3(v.x, v.y, v.z);
		}
		Vec4& Vec4::operator=(const Vec4& src)
		{
			memcpy(v, src.v, sizeof(v));
			return (*this);
		}
		REAL& Vec4::operator[](ptrdiff_t i)
		{
			return v[i];
		}
		REAL Vec4::operator[](ptrdiff_t i) const
		{
			return v[i];
		}
		REAL Vec4::operator*(const Vec4& right) const
		{
			return (x * right.x + y * right.y + z * right.z) / (w * right.w);
		}
		Vec4 Vec4::operator^(const Vec4& right) const
		{
			REAL inv_w0w1 = (REAL)1 / (w * right.w);
			return Vec4(	(y * right.z - z  *right.y) * inv_w0w1,
							(z * right.x - x * right.z) * inv_w0w1,
							(x * right.y - y * right.x) * inv_w0w1);
		}
		Vec4 Vec4::operator*(REAL scalar) const
		{
			return Vec4(x, y, z, w / scalar);
		}
		Vec4& Vec4::operator*=(REAL scalar)
		{
			return (*this) = operator*(scalar);
		}
		Vec4 Vec4::operator/(REAL scalar) const
		{
			return operator*((REAL)1 / scalar);
		}
		Vec4& Vec4::operator/=(REAL scalar)
		{
			return operator*=(1 / scalar);
		}
		Vec4 Vec4::operator+(const Vec4& right) const
		{
			return Vec4(((Vec3)(*this)) + ((Vec3)right));
		}
		Vec4& Vec4::operator+=(const Vec4& right)
		{
			return (*this) = operator+(right);
		}
		Vec4 Vec4::operator-(const Vec4& right) const
		{
			return Vec4(((Vec3)(*this)) - ((Vec3)right));
		}
		Vec4& Vec4::operator-=(const Vec4& right)
		{
			return (*this) = operator-(right);
		}
		Vec4 Vec4::operator-() const
		{
			return Vec4(-x, -y, -z, w);
		}
		Vec4 Vec4::operator*(const Mat4& right) const
		{
			return Vec4(	x*right.v00+y*right.v10+z*right.v20+w*right.v30,
							x*right.v01+y*right.v11+z*right.v21+w*right.v31,
							x*right.v02+y*right.v12+z*right.v22+w*right.v32,
							x*right.v03+y*right.v13+z*right.v23+w*right.v33);
			/*
			return Vec4(	v[0]*right.v[0][0]+v[1]*right.v[1][0]+v[2]*right.v[2][0]+v[3]*right.v[3][0],
							v[0]*right.v[0][1]+v[1]*right.v[1][1]+v[2]*right.v[2][1]+v[3]*right.v[3][1],
							v[0]*right.v[0][2]+v[1]*right.v[1][2]+v[2]*right.v[2][2]+v[3]*right.v[3][2],
							v[0]*right.v[0][3]+v[1]*right.v[1][3]+v[2]*right.v[2][3]+v[3]*right.v[3][3]);*/
		}
		Vec4& Vec4::operator*=(const Mat4& right)
		{
			return (*this)=operator*(right);
		}
		bool Vec4::operator==(const Vec4& right) const
		{
			return x==right.x&&y==right.y&&z==right.z&&w==right.w;
		}
		bool Vec4::operator!=(const Vec4& right) const
		{
			return !(operator==(right));
		}
		Vec4&			Vec4::setX(REAL x)
		{
			this->x = x;
			return *this;
		}
		Vec4&			Vec4::setY(REAL y)
		{
			this->y = y;
			return *this;
		}
		Vec4&			Vec4::setZ(REAL z)
		{
			this->z = z;
			return *this;
		}
		Vec4&			Vec4::setW(REAL w)
		{
			this->w = w;
			return *this;
		}
		Vec4			Vec4::withX(REAL x) const
		{
			return Vec4(x, y, z, w);
		}
		Vec4			Vec4::withY(REAL y) const
		{
			return Vec4(x, y, z, w);
		}
		Vec4			Vec4::withZ(REAL z) const
		{
			return Vec4(x, y, z, w);
		}
		Vec4			Vec4::withW(REAL w) const
		{
			return Vec4(x, y, z, w);
		}
		Vec4	Vec4::modulate(const Vec4& right) const
		{
			return Vec4(x*right.x,y*right.y,z*right.z,w*right.w);
		}
		Vec4 Vec4::rotateBy(const Vec4& axis,REAL angle) const
		{
			return Vec4(((Vec3)(*this)).rotateBy((Vec3)axis,angle));
		}
		Vec4 Vec4::rotateBy(const Quat& q) const
		{
			Quat p=q*Quat((Vec3)(*this))*q.conjugate();
			return Vec3(p.x,p.y,p.z);
		}
		Vec4 Vec4::rotateBy(const Vec4& axis,REAL angle,const Vec4& around) const
		{
			return ((*this)-around).rotateBy(axis,angle)+around;
		}
		Vec4 Vec4::rotateBy(const Quat& q,const Vec4& around) const
		{
			return ((*this)-around).rotateBy(q)+around;
		}
		Vec4 Vec4::cartesian() const
		{
			REAL inv_w=(REAL)1/w;
			return Vec4(x*inv_w,y*inv_w,z*inv_w);
		}
		Vec4& Vec4::cartesianize()
		{
			return (*this)=cartesian();
		}
		REAL Vec4::length() const
		{
			return ((Vec3)(*this)).length();
		}
		REAL Vec4::lengthsq() const
		{
			return ((Vec3)(*this)).lengthsq();
		}
		Vec4 Vec4::normal() const
		{
			return Vec4(((Vec3)(*this)).normal());
		}
		Vec4			Vec4::normal_safe() const
		{
			return Vec4(((Vec3)(*this)).normal_safe());
		}
		Vec4& Vec4::normalize()
		{
			return (*this)=normal();
		}
		Vec4& Vec4::normalize_safe()
		{
			return (*this)=normal_safe();
		}
		REAL Vec4::angle(const Vec4& other) const
		{
			return ((Vec3)(*this)).angle((Vec3)other);
		}
		bool Vec4::isEquivalent(const Vec4& other,REAL bias) const
		{
			return ((Vec3)(*this)).isEquivalent((Vec3)other,bias);
		}
		bool Vec4::isParallel(const Vec4& other,REAL biasAngle) const
		{
			return ((Vec3)(*this)).isParallel((Vec3)other,biasAngle);
		}
		bool Vec4::split2(const Vec4& v0,const Vec4& v1,float& r0,float& r1) const
		{
			return ((Vec3)*this).split2((Vec3)v0,(Vec3)v1,r0,r1);
		}
		bool Vec4::split3(const Vec4& v0,const Vec4& v1,const Vec4& v2,float& r0,float& r1,float& r2) const
		{
			return ((Vec3)*this).split3((Vec3)v0,(Vec3)v1,(Vec3)v2,r0,r1,r2);
		}
		bool Vec4::valid() const
		{
			return FINITE(v[0])&FINITE(v[1])&FINITE(v[2])&FINITE(v[3]);
		}
		Vec4 operator*(REAL scalar,const Vec4& right)
		{
			return right*scalar;
		}
	};
};