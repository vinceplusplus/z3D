
#include "Plane.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace z3D
{
	namespace Core
	{
		Plane::Plane()
		{
			//v[0]=v[1]=v[2]=v[3]=0;
			v0=v1=v2=v3=0;
		}
		Plane::Plane(REAL a,REAL b,REAL c,REAL d)
		{
			//v[0]=a;v[1]=b;v[2]=c;v[3]=d;
			v0=a;v1=b;v2=c;v3=d;
		}
		Plane::Plane(const REAL* const v)
		{
			//this->v[0]=v[0];this->v[1]=v[1];this->v[2]=v[2];this->v[3]=v[3];
			this->v0=v[0];this->v1=v[1];this->v2=v[2];this->v3=v[3];
		}
		Plane::Plane(const Vec3& p,const Vec3& n)
		{
			REAL d=p*n;
			v0=n.x;
			v1=n.y;
			v2=n.z;
			v3=-d;
			/*
			v[0]=n[0];
			v[1]=n[1];
			v[2]=n[2];
			v[3]=-d;*/
		}
		Plane::Plane(const Vec3& n,REAL d)
		{
			/*
			v[0]=n[0];
			v[1]=n[1];
			v[2]=n[2];
			v[3]=-d;*/
			v0=n.x;
			v1=n.y;
			v2=n.z;
			v3=-d;
		}
		Plane::Plane(const Vec3& p0,const Vec3& p1,const Vec3& p2,bool normalize)
		{
			if(normalize)
				(*this)=Plane(p0,((p2-p1)^(p0-p1)).normal());
			else
				(*this)=Plane(p0,((p2-p1)^(p0-p1)));
		}

		const REAL Plane::operator[](int i) const
		{
			return v[i];
		}
		Plane		Plane::operator-() const
		{
			return Plane(-a,-b,-c,-d);
		}
		const Vec3 Plane::normal() const
		{
			return Vec3(v);
		}
		const REAL Plane::distance() const
		{
			return -d / Vec3(v).length();
		}
		const Vec3 Plane::origin() const
		{
			return Vec3(v)*(-d)/(a*a+b*b+c*c);
		}
		const REAL Plane::sub(const Vec3& v) const
		{
			//return Vec3(this->v)*v+this->v[3];
			return v.x*this->v0+v.y*this->v1+v.z*this->v2+this->v3;
		}
		const REAL Plane::sub(const Vec4& v) const
		{
			//return Vec3(this->v)*v+this->v[3];
			return v.x*this->v0+v.y*this->v1+v.z*this->v2+v.w*this->v3;
		}
		bool Plane::isParallel(const Plane& p,REAL biasAngle) const
		{
			return normal().isParallel(p.normal(),biasAngle);
		}
		Plane		Plane::normalization() const
		{
			return Plane(origin(),normal().normal_safe());
		}
		void			Plane::normalize()
		{
			*this=normalization();
		}
		void			Plane::transformBy(const Mat4& m,bool inverse_transposed)
		{
			(*this)=transformedBy(m,inverse_transposed);
		}
		Plane			Plane::transformedBy(const Mat4& m,bool inverse_transposed) const
		{
			Z_ASSERT(((unsigned char*)&Vec4::zero.y-(unsigned char*)&Vec4::zero.x)==sizeof(REAL));
			if(inverse_transposed)
				return Plane(&((Vec4(&v0)*m).x)).normalization();
			return Plane(&((Vec4(&v0)*m.inverse().transpose()).x)).normalization();
		}
	};
};