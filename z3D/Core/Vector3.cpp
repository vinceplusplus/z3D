
#include "Constants.h"
#include "MathFunc.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"

/*
__declspec(naked) void PIII_Mult10_3x3_3x1( float *src1, float *src2, float *dst)
{
	__asm
	{
		mov ecx, dword ptr [esp+8]
		mov edx, dword ptr [esp+4]
		mov eax, dword ptr [esp+0Ch]
		movss xmm0, dword ptr [ecx]
		movss xmm5, dword ptr [edx]
		movhps xmm5, qword ptr [edx+4]
		shufps xmm0, xmm0, 0
		movss xmm1, dword ptr [ecx+4]
		movss xmm3, dword ptr [edx+12]
		movhps xmm3, qword ptr [edx+16]
		shufps xmm1, xmm1, 0
		mulps xmm0, xmm5
		mulps xmm1, xmm3
		movss xmm2, dword ptr [ecx+8]
		shufps xmm2, xmm2, 0
		movss xmm4, dword ptr [edx+24]
		movhps xmm4, qword ptr [edx+28]
		addps xmm0, xmm1
		mulps xmm2, xmm4
		addps xmm0, xmm2
		movss dword ptr [eax], xmm0
		movhps qword ptr [eax+4], xmm0
	}
	__asm ret
}*/

namespace z3D
{
	namespace Core
	{
		Vec3::Vec3()
		{
			(*this) = zero;
		}
		Vec3::Vec3(REAL x, REAL y, REAL z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
		Vec3::Vec3(const REAL src[3])
		{
			x = src[0];
			y = src[1];
			z = src[2];
		}
		Vec3::Vec3(const Vec3& src)
		{
			(*this) = src;
		}
		Vec3::operator Vec4() const
		{
			return Vec4(x, y, z);
		}
		Vec3& Vec3::operator=(const Vec3& src)
		{
			memcpy(v, src.v, sizeof(v));
			return (*this);
		}
		REAL& Vec3::operator[](ptrdiff_t i)
		{
			return v[i];
		}
		REAL Vec3::operator[](ptrdiff_t i) const
		{
			return v[i];
		}
		REAL Vec3::operator*(const Vec3& right) const
		{
			return x * right.x + y * right.y + z * right.z;
		}
		Vec3 Vec3::operator^(const Vec3& right) const
		{
			return Vec3(y * right.z - z * right.y, z * right.x - x * right.z, x * right.y - y * right.x);
		}
		Vec3 Vec3::operator*(REAL scalar) const
		{
			return Vec3(scalar * x, scalar * y, scalar * z);
		}
		Vec3& Vec3::operator*=(REAL scalar)
		{
			return (*this) = operator*(scalar);
		}
		Vec3 Vec3::operator/(REAL scalar) const
		{
			return operator*((REAL)1 / scalar);
		}
		Vec3& Vec3::operator/=(REAL scalar)
		{
			return operator*=(1 / scalar);
		}
		Vec3 Vec3::operator+(const Vec3& right) const
		{
			return Vec3(x + right.x, y + right.y, z + right.z);
		}
		Vec3& Vec3::operator+=(const Vec3& right)
		{
			return (*this)=operator + (right);
		}
		Vec3 Vec3::operator-(const Vec3& right) const
		{
			return Vec3(x - right.x, y - right.y, z - right.z);
		}
		Vec3& Vec3::operator-=(const Vec3& right)
		{
			return (*this) = operator-(right);
		}
		Vec3 Vec3::operator-() const
		{
			return Vec3(-x, -y, -z);
		}
		Vec3 Vec3::operator*(const Mat3& right) const
		{
			// no need for optimization, because vc compiler optimization is even faster than using intel sse optimization, about 3 times faster
			/*
			REAL dst[3];
			PIII_Mult10_3x3_3x1((REAL*)&right,(REAL*)this,dst);
			return Vec3(dst);*/

			return Vec3(	x * right.v00 + y * right.v10 + z * right.v20,
							x * right.v01 + y * right.v11 + z * right.v21,
							x * right.v02 + y * right.v12 + z * right.v22);
			/*
			return Vec3(	v[0]*right.v[0][0]+v[1]*right.v[1][0]+v[2]*right.v[2][0],
							v[0]*right.v[0][1]+v[1]*right.v[1][1]+v[2]*right.v[2][1],
							v[0]*right.v[0][2]+v[1]*right.v[1][2]+v[2]*right.v[2][2]);*/
		}
		Vec3& Vec3::operator*=(const Mat3& right)
		{
			return (*this) = operator*(right);
		}
		Vec3 Vec3::operator*(const Mat4& right) const
		{
			return ((Vec4)(*this)) * right;
		}
		Vec3& Vec3::operator*=(const Mat4& right)
		{
			return (*this) = operator*(right);
		}
		bool Vec3::operator==(const Vec3& right) const
		{
			return x == right.x && y == right.y && z == right.z;
		}
		bool Vec3::operator!=(const Vec3& right) const
		{
			return !(operator==(right));
		}
		Vec3&			Vec3::setX(REAL x)
		{
			this->x = x;
			return *this;
		}
		Vec3&			Vec3::setY(REAL y)
		{
			this->y = y;
			return *this;
		}
		Vec3&			Vec3::setZ(REAL z)
		{
			this->z = z;
			return *this;
		}
		Vec3			Vec3::withX(REAL x) const
		{
			return Vec3(x, y, z);
		}
		Vec3			Vec3::withY(REAL y) const
		{
			return Vec3(x, y, z);
		}
		Vec3			Vec3::withZ(REAL z) const
		{
			return Vec3(x, y, z);
		}
		Vec3	Vec3::modulate(const Vec3& right) const
		{
			return Vec3(x * right.x, y * right.y, z * right.z);
		}
		Vec3 Vec3::rotateBy(const Vec3& axis,REAL angle) const
		{
			REAL c,s,dot,k;
			Vec3 cross=axis^(*this);
			c=COS(angle);
			s=SIN(angle);
			dot=axis*(*this);
			k=(1-c)*dot;
			return (*this)*c+axis*k+cross*s;
		}
		Vec3 Vec3::rotateBy(const Quat& q) const
		{
			Quat p=q*Quat(*this)*q.conjugate();
			return Vec3(p.x,p.y,p.z);
		}
		Vec3 Vec3::rotateBy(const Vec3& axis,REAL angle,const Vec3& around) const
		{
			return ((*this)-around).rotateBy(axis,angle)+around;
		}
		Vec3 Vec3::rotateBy(const Quat& q,const Vec3& around) const
		{
			return ((*this)-around).rotateBy(q)+around;
		}
		REAL Vec3::length() const
		{
			return SQRT(x*x+y*y+z*z);
		}
		REAL Vec3::lengthsq() const
		{
			return x*x+y*y+z*z;
		}
		Vec3 Vec3::normal() const
		{
			return (Vec3(*this))*((REAL)1/length());
			//return (Vec3(*this))*INVSQRT(lengthsq());
		}
		Vec3			Vec3::normal_safe() const
		{
			REAL lensq = lengthsq();
			if(lensq > SMALLEST_NORMALIZABLE_VECTOR_LENGTHSQ)
			{
				Vec3 n = (*this) * (1 / SQRT(lensq));
				if(n.valid())
					return n;
			}
			return Vec3::zero;
		}
		Vec3& Vec3::normalize()
		{
			return (*this)=normal();
		}
		Vec3&			Vec3::normalize_safe()
		{
			return (*this)=normal_safe();
		}
		REAL Vec3::angle(const Vec3& other) const
		{
			return ACOS(((*this)*other)/(length()*other.length()));
		}
		bool Vec3::isEquivalent(const Vec3& other,REAL bias) const
		{
			Vec3 d=(*this)-other;
			if(FABS(d.x)<=bias&&
				FABS(d.y)<=bias&&
				FABS(d.z)<=bias)
				return true;
			return false;
		}
		bool Vec3::isParallel(const Vec3& other,REAL biasAngle) const
		{
			REAL a=angle(other);
			if(a<=biasAngle)
				return true;
			return false;
		}
		bool Vec3::split2(const Vec3& v0,const Vec3& v1,float& r0,float& r1) const
		{
			Mat3 m(v0,v1,Vec3::zero,false);
			bool bsucc=false;
			Mat3 inv_m=m.inverse(&bsucc);
			if(!bsucc)
				return false;
			Vec3 r=(*this)*inv_m;
			r0=r[0];
			r1=r[1];
			return true;
		}
		bool Vec3::split3(const Vec3& v0,const Vec3& v1,const Vec3& v2,float& r0,float& r1,float& r2) const
		{
			Mat3 m(v0,v1,v2,false);
			bool bsucc=false;
			Mat3 inv_m=m.inverse(&bsucc);
			if(!bsucc)
				return false;
			Vec3 r=(*this)*inv_m;
			r0=r[0];
			r1=r[1];
			r2=r[2];
			return true;
		}
		REAL	Vec3::resolveAngle(const Vec3& axis,const Vec3& start_pos) const
		{
			Vec3 ref=(*this)*Mat3(start_pos.normal(),(axis^start_pos).normal(),axis.normal(),false);
			return Vec2(ref.x,ref.y).angle();
		}
		void Vec3::resolveOriRH(REAL& yaw, REAL& pitch) const
		{
			Vec2 unit(-v[2], -v[0]);
			if((*this).length() <= EPSILON || unit.length() <= EPSILON)
				yaw = 0;
			else
			{
				unit.normalize();
				unit.normalize();
				yaw = SIGNBIT(unit[1]) ? -ACOS(unit[0]) : ACOS(unit[0]);
			}
			Vec3 ori = (*this).rotateBy(Vec3::yAxis, -yaw);
			Vec2 unit2(-ori[2], ori[1]);
			unit2.normalize();
			unit2.normalize();
			pitch = SIGNBIT(unit2[1]) ? -ACOS(unit2[0]) : ACOS(unit2[0]);
		}
		Vec3 Vec3::lerp(const Vec3& v1,REAL t) const
		{
			return (*this)*(1-t)+v1*t;
		}
		Vec3 Vec3::slerp(const Vec3& v,REAL t) const
		{
			const Vec3& a=(*this);
			const Vec3& b=v;
			REAL s,w,dot,angle,theta,phi;
			dot=a*b;
			if(dot<1-z3D::Core::EPSILON)
			{
				angle=ACOS(dot);
				theta=t*angle;
				phi=angle-theta;
				s=SIN(angle*0.5f)-COS((theta+phi)*0.5f)*TAN((phi-theta)*0.5f);
				w=SIN(angle*0.5f)*2;
			}
			else
			{
				s=0;
				w=1;
			}
			return ((w-s)*a+s*b).normal();
		}
		bool Vec3::valid() const
		{
			return FINITE(v[0])&FINITE(v[1])&FINITE(v[2]);
		}
		bool	Vec3::testZeroFast(REAL threshold) const
		{
			return x*x+y*y+z*z<threshold;
		}
		Vec3	Vec3::slideAlong(const Vec3& plane_normal) const
		{
			Vec3 slide_dir = ((plane_normal ^ (*this)) ^ plane_normal);
			slide_dir.normalize_safe();
			return slide_dir * (slide_dir * (*this));
		}
		Vec3	Vec3::reflect(const Vec3& N) const
		{
			return (*this) - (((*this) * N) * N) * 2.0f;
		}
		Vec3			Vec3::refract(const Vec3& N, float eta)
		{
			const Vec3& I = *this;
			REAL IdotN = I * N;
			REAL IdotI = I * I;
			REAL eta2 = eta * eta;
			REAL cos2Tovercos2I = eta2 + (1 - eta2) / (IdotN * IdotN / IdotI);
			Vec3 T = eta * I - (eta - SQRT(FABS(cos2Tovercos2I))) * IdotN * N;
			return T * (cos2Tovercos2I >= 0.0f ? 1.0f : 0.0f);
		}
		Vec3		Vec3::linear_mapping	(const Vec3& x0, const Vec3& x1, const Vec3& y0, const Vec3& y1, const Vec3& x)
		{
			return Vec3(
				LINEAR_MAPPING(x0[0], x1[0], y0[0], y1[0], x[0]),
				LINEAR_MAPPING(x0[1], x1[1], y0[1], y1[1], x[1]),
				LINEAR_MAPPING(x0[2], x1[2], y0[2], y1[2], x[2])
				);
		}
		Vec3 operator*(REAL scalar,const Vec3& right)
		{
			return right*scalar;
		}
	};
};
