
#include "Matrix3.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MathFunc.h"
#include "Plane.h"
#include "Constants.h"

namespace z3D
{
	namespace Core
	{
		Mat4::Mat4()
		{
			memset(v, 0, sizeof(v));
		}
		Mat4::Mat4(const REAL v[4][4])
		{
			memcpy(this->v, v, sizeof(this->v));
		}
		Mat4::Mat4(const Mat4& src)
		{
			(*this) = src;
		}
		Mat4::operator Mat3() const
		{
			Mat3 other;

			other.v00=v00;other.v01=v01;other.v02=v02;
			other.v10=v10;other.v11=v11;other.v12=v12;
			other.v20=v20;other.v21=v21;other.v22=v22;

			/*
			other.v[0][0]=v[0][0];other.v[0][1]=v[0][1];other.v[0][2]=v[0][2];
			other.v[1][0]=v[1][0];other.v[1][1]=v[1][1];other.v[1][2]=v[1][2];
			other.v[2][0]=v[2][0];other.v[2][1]=v[2][1];other.v[2][2]=v[2][2];*/
			return other;
		}
		Mat4& Mat4::operator=(const Mat4& other)
		{
			memcpy(v, other.v, sizeof(v));
			return (*this);
		}
		REAL* Mat4::operator[](ptrdiff_t row)
		{
			return v[row];
		}
		const REAL* Mat4::operator[](ptrdiff_t row) const
		{
			return v[row];
		}
		Mat4& Mat4::setIdentity()
		{
			v00=(REAL)1.0;v01=(REAL)0.0;v02=(REAL)0.0;v03=(REAL)0.0;
			v10=(REAL)0.0;v11=(REAL)1.0;v12=(REAL)0.0;v13=(REAL)0.0;
			v20=(REAL)0.0;v21=(REAL)0.0;v22=(REAL)1.0;v23=(REAL)0.0;
			v30=(REAL)0.0;v31=(REAL)0.0;v32=(REAL)0.0;v33=(REAL)1.0;
			/*
			v[0][0]=(REAL)1.0;v[0][1]=(REAL)0.0;v[0][2]=(REAL)0.0;v[0][3]=(REAL)0.0;
			v[1][0]=(REAL)0.0;v[1][1]=(REAL)1.0;v[1][2]=(REAL)0.0;v[1][3]=(REAL)0.0;
			v[2][0]=(REAL)0.0;v[2][1]=(REAL)0.0;v[2][2]=(REAL)1.0;v[2][3]=(REAL)0.0;
			v[3][0]=(REAL)0.0;v[3][1]=(REAL)0.0;v[3][2]=(REAL)0.0;v[3][3]=(REAL)1.0;*/
			return (*this);
		}
		Mat4 Mat4::transpose() const
		{
			Mat4 other;
			other.v00=v00;other.v01=v10;other.v02=v20;other.v03=v30;
			other.v10=v01;other.v11=v11;other.v12=v21;other.v13=v31;
			other.v20=v02;other.v21=v12;other.v22=v22;other.v23=v32;
			other.v30=v03;other.v31=v13;other.v32=v23;other.v33=v33;
			/*
			other.v[0][0]=v[0][0];other.v[0][1]=v[1][0];other.v[0][2]=v[2][0];other.v[0][3]=v[3][0];
			other.v[1][0]=v[0][1];other.v[1][1]=v[1][1];other.v[1][2]=v[2][1];other.v[1][3]=v[3][1];
			other.v[2][0]=v[0][2];other.v[2][1]=v[1][2];other.v[2][2]=v[2][2];other.v[2][3]=v[3][2];
			other.v[3][0]=v[0][3];other.v[3][1]=v[1][3];other.v[3][2]=v[2][3];other.v[3][3]=v[3][3];*/
			return other;
		}
		Mat4& Mat4::operator+=(const Mat4& right)
		{
			v00+=right.v00;v01+=right.v01;v02+=right.v02;v03+=right.v03;
			v10+=right.v10;v11+=right.v11;v12+=right.v12;v13+=right.v13;
			v20+=right.v20;v21+=right.v21;v22+=right.v22;v23+=right.v23;
			v30+=right.v30;v31+=right.v31;v32+=right.v32;v33+=right.v33;
			/*
			v[0][0]+=right.v[0][0];v[0][1]+=right.v[0][1];v[0][2]+=right.v[0][2];v[0][3]+=right.v[0][3];
			v[1][0]+=right.v[1][0];v[1][1]+=right.v[1][1];v[1][2]+=right.v[1][2];v[1][3]+=right.v[1][3];
			v[2][0]+=right.v[2][0];v[2][1]+=right.v[2][1];v[2][2]+=right.v[2][2];v[2][3]+=right.v[2][3];
			v[3][0]+=right.v[3][0];v[3][1]+=right.v[3][1];v[3][2]+=right.v[3][2];v[3][3]+=right.v[3][3];*/
			return (*this);
		}
		Mat4 Mat4::operator+(const Mat4& right) const
		{
			Mat4 p=(*this);
			return p+=right;
		}
		Mat4& Mat4::operator-=(const Mat4& right)
		{
			v00-=right.v00;v01-=right.v01;v02-=right.v02;v03-=right.v03;
			v10-=right.v10;v11-=right.v11;v12-=right.v12;v13-=right.v13;
			v20-=right.v20;v21-=right.v21;v22-=right.v22;v23-=right.v23;
			v30-=right.v30;v31-=right.v31;v32-=right.v32;v33-=right.v33;
			/*
			v[0][0]-=right.v[0][0];v[0][1]-=right.v[0][1];v[0][2]-=right.v[0][2];v[0][3]-=right.v[0][3];
			v[1][0]-=right.v[1][0];v[1][1]-=right.v[1][1];v[1][2]-=right.v[1][2];v[1][3]-=right.v[1][3];
			v[2][0]-=right.v[2][0];v[2][1]-=right.v[2][1];v[2][2]-=right.v[2][2];v[2][3]-=right.v[2][3];
			v[3][0]-=right.v[3][0];v[3][1]-=right.v[3][1];v[3][2]-=right.v[3][2];v[3][3]-=right.v[3][3];*/
			return (*this);
		}
		Mat4 Mat4::operator-(const Mat4& right) const
		{
			Mat4 p=(*this);
			return p-=right;
		}
		Mat4& Mat4::operator*=(const Mat4& right)
		{
			return (*this)=(*this)*right;
		}
		Mat4 Mat4::operator*(const Mat4& right) const
		{
			Mat4 p;
			p.v00=v00*right.v00+v01*right.v10+v02*right.v20+v03*right.v30;
			p.v01=v00*right.v01+v01*right.v11+v02*right.v21+v03*right.v31;
			p.v02=v00*right.v02+v01*right.v12+v02*right.v22+v03*right.v32;
			p.v03=v00*right.v03+v01*right.v13+v02*right.v23+v03*right.v33;
			p.v10=v10*right.v00+v11*right.v10+v12*right.v20+v13*right.v30;
			p.v11=v10*right.v01+v11*right.v11+v12*right.v21+v13*right.v31;
			p.v12=v10*right.v02+v11*right.v12+v12*right.v22+v13*right.v32;
			p.v13=v10*right.v03+v11*right.v13+v12*right.v23+v13*right.v33;
			p.v20=v20*right.v00+v21*right.v10+v22*right.v20+v23*right.v30;
			p.v21=v20*right.v01+v21*right.v11+v22*right.v21+v23*right.v31;
			p.v22=v20*right.v02+v21*right.v12+v22*right.v22+v23*right.v32;
			p.v23=v20*right.v03+v21*right.v13+v22*right.v23+v23*right.v33;
			p.v30=v30*right.v00+v31*right.v10+v32*right.v20+v33*right.v30;
			p.v31=v30*right.v01+v31*right.v11+v32*right.v21+v33*right.v31;
			p.v32=v30*right.v02+v31*right.v12+v32*right.v22+v33*right.v32;
			p.v33=v30*right.v03+v31*right.v13+v32*right.v23+v33*right.v33;
			/*
			p.v[0][0]=v[0][0]*right.v[0][0]+v[0][1]*right.v[1][0]+v[0][2]*right.v[2][0]+v[0][3]*right.v[3][0];
			p.v[0][1]=v[0][0]*right.v[0][1]+v[0][1]*right.v[1][1]+v[0][2]*right.v[2][1]+v[0][3]*right.v[3][1];
			p.v[0][2]=v[0][0]*right.v[0][2]+v[0][1]*right.v[1][2]+v[0][2]*right.v[2][2]+v[0][3]*right.v[3][2];
			p.v[0][3]=v[0][0]*right.v[0][3]+v[0][1]*right.v[1][3]+v[0][2]*right.v[2][3]+v[0][3]*right.v[3][3];
			p.v[1][0]=v[1][0]*right.v[0][0]+v[1][1]*right.v[1][0]+v[1][2]*right.v[2][0]+v[1][3]*right.v[3][0];
			p.v[1][1]=v[1][0]*right.v[0][1]+v[1][1]*right.v[1][1]+v[1][2]*right.v[2][1]+v[1][3]*right.v[3][1];
			p.v[1][2]=v[1][0]*right.v[0][2]+v[1][1]*right.v[1][2]+v[1][2]*right.v[2][2]+v[1][3]*right.v[3][2];
			p.v[1][3]=v[1][0]*right.v[0][3]+v[1][1]*right.v[1][3]+v[1][2]*right.v[2][3]+v[1][3]*right.v[3][3];
			p.v[2][0]=v[2][0]*right.v[0][0]+v[2][1]*right.v[1][0]+v[2][2]*right.v[2][0]+v[2][3]*right.v[3][0];
			p.v[2][1]=v[2][0]*right.v[0][1]+v[2][1]*right.v[1][1]+v[2][2]*right.v[2][1]+v[2][3]*right.v[3][1];
			p.v[2][2]=v[2][0]*right.v[0][2]+v[2][1]*right.v[1][2]+v[2][2]*right.v[2][2]+v[2][3]*right.v[3][2];
			p.v[2][3]=v[2][0]*right.v[0][3]+v[2][1]*right.v[1][3]+v[2][2]*right.v[2][3]+v[2][3]*right.v[3][3];
			p.v[3][0]=v[3][0]*right.v[0][0]+v[3][1]*right.v[1][0]+v[3][2]*right.v[2][0]+v[3][3]*right.v[3][0];
			p.v[3][1]=v[3][0]*right.v[0][1]+v[3][1]*right.v[1][1]+v[3][2]*right.v[2][1]+v[3][3]*right.v[3][1];
			p.v[3][2]=v[3][0]*right.v[0][2]+v[3][1]*right.v[1][2]+v[3][2]*right.v[2][2]+v[3][3]*right.v[3][2];
			p.v[3][3]=v[3][0]*right.v[0][3]+v[3][1]*right.v[1][3]+v[3][2]*right.v[2][3]+v[3][3]*right.v[3][3];*/
			return p;
		}
		Mat4& Mat4::operator*=(REAL scalar)
		{
			v00*=scalar;v01*=scalar;v02*=scalar;v03*=scalar;
			v10*=scalar;v11*=scalar;v12*=scalar;v13*=scalar;
			v20*=scalar;v21*=scalar;v22*=scalar;v23*=scalar;
			v30*=scalar;v31*=scalar;v32*=scalar;v33*=scalar;
			/*
			v[0][0]*=scalar;v[0][1]*=scalar;v[0][2]*=scalar;v[0][3]*=scalar;
			v[1][0]*=scalar;v[1][1]*=scalar;v[1][2]*=scalar;v[1][3]*=scalar;
			v[2][0]*=scalar;v[2][1]*=scalar;v[2][2]*=scalar;v[2][3]*=scalar;
			v[3][0]*=scalar;v[3][1]*=scalar;v[3][2]*=scalar;v[3][3]*=scalar;*/
			return (*this);
		}
		Mat4 Mat4::operator*(REAL scalar) const
		{
			return (Mat4(*this))*=scalar;
		}
		Mat4 Mat4::inverse(bool* invertible) const
		{
			Mat4 inv;
			inv.v00=(v11*v22*v33+v12*v23*v31+v13*v21*v32-v31*v22*v13-v32*v23*v11-v33*v21*v12);
			inv.v10=-(v10*v22*v33+v12*v23*v30+v13*v20*v32-v30*v22*v13-v32*v23*v10-v33*v20*v12);
			inv.v20=(v10*v21*v33+v11*v23*v30+v13*v20*v31-v30*v21*v13-v31*v23*v10-v33*v20*v11);
			inv.v30=-(v10*v21*v32+v11*v22*v30+v12*v20*v31-v30*v21*v12-v31*v22*v10-v32*v20*v11);
			inv.v01=-(v01*v22*v33+v02*v23*v31+v03*v21*v32-v31*v22*v03-v32*v23*v01-v33*v21*v02);
			inv.v11=(v00*v22*v33+v02*v23*v30+v03*v20*v32-v30*v22*v03-v32*v23*v00-v33*v20*v02);
			inv.v21=-(v00*v21*v33+v01*v23*v30+v03*v20*v31-v30*v21*v03-v31*v23*v00-v33*v20*v01);
			inv.v31=(v00*v21*v32+v01*v22*v30+v02*v20*v31-v30*v21*v02-v31*v22*v00-v32*v20*v01);
			inv.v02=(v01*v12*v33+v02*v13*v31+v03*v11*v32-v31*v12*v03-v32*v13*v01-v33*v11*v02);
			inv.v12=-(v00*v12*v33+v02*v13*v30+v03*v10*v32-v30*v12*v03-v32*v13*v00-v33*v10*v02);
			inv.v22=(v00*v11*v33+v01*v13*v30+v03*v10*v31-v30*v11*v03-v31*v13*v00-v33*v10*v01);
			inv.v32=-(v00*v11*v32+v01*v12*v30+v02*v10*v31-v30*v11*v02-v31*v12*v00-v32*v10*v01);
			inv.v03=-(v01*v12*v23+v02*v13*v21+v03*v11*v22-v21*v12*v03-v22*v13*v01-v23*v11*v02);
			inv.v13=(v00*v12*v23+v02*v13*v20+v03*v10*v22-v20*v12*v03-v22*v13*v00-v23*v10*v02);
			inv.v23=-(v00*v11*v23+v01*v13*v20+v03*v10*v21-v20*v11*v03-v21*v13*v00-v23*v10*v01);
			inv.v33=(v00*v11*v22+v01*v12*v20+v02*v10*v21-v20*v11*v02-v21*v12*v00-v22*v10*v01);
			REAL det=v00*inv.v00+v01*inv.v10+v02*inv.v20+v03*inv.v30;

			if(FABS(det) < SMALLEST_INVERTIBLE_MATRIX_DET)
			{
				Z_ASSERT( invertible );
				if(invertible)
					*invertible = false;
				return *this;
			}
			if(invertible)
				*invertible = true;

			REAL inv_det=(REAL)1/det;
			inv.v00*=inv_det;inv.v01*=inv_det;inv.v02*=inv_det;inv.v03*=inv_det;
			inv.v10*=inv_det;inv.v11*=inv_det;inv.v12*=inv_det;inv.v13*=inv_det;
			inv.v20*=inv_det;inv.v21*=inv_det;inv.v22*=inv_det;inv.v23*=inv_det;
			inv.v30*=inv_det;inv.v31*=inv_det;inv.v32*=inv_det;inv.v33*=inv_det;
			return inv;
			/*
			Mat4 inv;
			inv.v[0][0]=(v[1][1]*v[2][2]*v[3][3]+v[1][2]*v[2][3]*v[3][1]+v[1][3]*v[2][1]*v[3][2]-v[3][1]*v[2][2]*v[1][3]-v[3][2]*v[2][3]*v[1][1]-v[3][3]*v[2][1]*v[1][2]);
			inv.v[1][0]=-(v[1][0]*v[2][2]*v[3][3]+v[1][2]*v[2][3]*v[3][0]+v[1][3]*v[2][0]*v[3][2]-v[3][0]*v[2][2]*v[1][3]-v[3][2]*v[2][3]*v[1][0]-v[3][3]*v[2][0]*v[1][2]);
			inv.v[2][0]=(v[1][0]*v[2][1]*v[3][3]+v[1][1]*v[2][3]*v[3][0]+v[1][3]*v[2][0]*v[3][1]-v[3][0]*v[2][1]*v[1][3]-v[3][1]*v[2][3]*v[1][0]-v[3][3]*v[2][0]*v[1][1]);
			inv.v[3][0]=-(v[1][0]*v[2][1]*v[3][2]+v[1][1]*v[2][2]*v[3][0]+v[1][2]*v[2][0]*v[3][1]-v[3][0]*v[2][1]*v[1][2]-v[3][1]*v[2][2]*v[1][0]-v[3][2]*v[2][0]*v[1][1]);
			inv.v[0][1]=-(v[0][1]*v[2][2]*v[3][3]+v[0][2]*v[2][3]*v[3][1]+v[0][3]*v[2][1]*v[3][2]-v[3][1]*v[2][2]*v[0][3]-v[3][2]*v[2][3]*v[0][1]-v[3][3]*v[2][1]*v[0][2]);
			inv.v[1][1]=(v[0][0]*v[2][2]*v[3][3]+v[0][2]*v[2][3]*v[3][0]+v[0][3]*v[2][0]*v[3][2]-v[3][0]*v[2][2]*v[0][3]-v[3][2]*v[2][3]*v[0][0]-v[3][3]*v[2][0]*v[0][2]);
			inv.v[2][1]=-(v[0][0]*v[2][1]*v[3][3]+v[0][1]*v[2][3]*v[3][0]+v[0][3]*v[2][0]*v[3][1]-v[3][0]*v[2][1]*v[0][3]-v[3][1]*v[2][3]*v[0][0]-v[3][3]*v[2][0]*v[0][1]);
			inv.v[3][1]=(v[0][0]*v[2][1]*v[3][2]+v[0][1]*v[2][2]*v[3][0]+v[0][2]*v[2][0]*v[3][1]-v[3][0]*v[2][1]*v[0][2]-v[3][1]*v[2][2]*v[0][0]-v[3][2]*v[2][0]*v[0][1]);
			inv.v[0][2]=(v[0][1]*v[1][2]*v[3][3]+v[0][2]*v[1][3]*v[3][1]+v[0][3]*v[1][1]*v[3][2]-v[3][1]*v[1][2]*v[0][3]-v[3][2]*v[1][3]*v[0][1]-v[3][3]*v[1][1]*v[0][2]);
			inv.v[1][2]=-(v[0][0]*v[1][2]*v[3][3]+v[0][2]*v[1][3]*v[3][0]+v[0][3]*v[1][0]*v[3][2]-v[3][0]*v[1][2]*v[0][3]-v[3][2]*v[1][3]*v[0][0]-v[3][3]*v[1][0]*v[0][2]);
			inv.v[2][2]=(v[0][0]*v[1][1]*v[3][3]+v[0][1]*v[1][3]*v[3][0]+v[0][3]*v[1][0]*v[3][1]-v[3][0]*v[1][1]*v[0][3]-v[3][1]*v[1][3]*v[0][0]-v[3][3]*v[1][0]*v[0][1]);
			inv.v[3][2]=-(v[0][0]*v[1][1]*v[3][2]+v[0][1]*v[1][2]*v[3][0]+v[0][2]*v[1][0]*v[3][1]-v[3][0]*v[1][1]*v[0][2]-v[3][1]*v[1][2]*v[0][0]-v[3][2]*v[1][0]*v[0][1]);
			inv.v[0][3]=-(v[0][1]*v[1][2]*v[2][3]+v[0][2]*v[1][3]*v[2][1]+v[0][3]*v[1][1]*v[2][2]-v[2][1]*v[1][2]*v[0][3]-v[2][2]*v[1][3]*v[0][1]-v[2][3]*v[1][1]*v[0][2]);
			inv.v[1][3]=(v[0][0]*v[1][2]*v[2][3]+v[0][2]*v[1][3]*v[2][0]+v[0][3]*v[1][0]*v[2][2]-v[2][0]*v[1][2]*v[0][3]-v[2][2]*v[1][3]*v[0][0]-v[2][3]*v[1][0]*v[0][2]);
			inv.v[2][3]=-(v[0][0]*v[1][1]*v[2][3]+v[0][1]*v[1][3]*v[2][0]+v[0][3]*v[1][0]*v[2][1]-v[2][0]*v[1][1]*v[0][3]-v[2][1]*v[1][3]*v[0][0]-v[2][3]*v[1][0]*v[0][1]);
			inv.v[3][3]=(v[0][0]*v[1][1]*v[2][2]+v[0][1]*v[1][2]*v[2][0]+v[0][2]*v[1][0]*v[2][1]-v[2][0]*v[1][1]*v[0][2]-v[2][1]*v[1][2]*v[0][0]-v[2][2]*v[1][0]*v[0][1]);
			REAL det=v[0][0]*inv.v[0][0]+v[0][1]*inv.v[1][0]+v[0][2]*inv.v[2][0]+v[0][3]*inv.v[3][0];
			if(det==(REAL)0)
				if(invertible)
				{
					*invertible=false;
					return *this;
				}
			if(invertible)
				*invertible=true;
			REAL inv_det=(REAL)1/det;
			inv.v[0][0]*=inv_det;inv.v[0][1]*=inv_det;inv.v[0][2]*=inv_det;inv.v[0][3]*=inv_det;
			inv.v[1][0]*=inv_det;inv.v[1][1]*=inv_det;inv.v[1][2]*=inv_det;inv.v[1][3]*=inv_det;
			inv.v[2][0]*=inv_det;inv.v[2][1]*=inv_det;inv.v[2][2]*=inv_det;inv.v[2][3]*=inv_det;
			inv.v[3][0]*=inv_det;inv.v[3][1]*=inv_det;inv.v[3][2]*=inv_det;inv.v[3][3]*=inv_det;
			return inv;*/
		}
		REAL Mat4::det4() const
		{
			Mat4 inv;
			inv.v00=(v11*v22*v33+v12*v23*v31+v13*v21*v32-v31*v22*v13-v32*v23*v11-v33*v21*v12);
			inv.v10=-(v10*v22*v33+v12*v23*v30+v13*v20*v32-v30*v22*v13-v32*v23*v10-v33*v20*v12);
			inv.v20=(v10*v21*v33+v11*v23*v30+v13*v20*v31-v30*v21*v13-v31*v23*v10-v33*v20*v11);
			inv.v30=-(v10*v21*v32+v11*v22*v30+v12*v20*v31-v30*v21*v12-v31*v22*v10-v32*v20*v11);
			REAL det=v00*inv.v00+v01*inv.v10+v02*inv.v20+v03*inv.v30;
			return det;
			/*
			Mat4 inv;
			inv.v[0][0]=(v[1][1]*v[2][2]*v[3][3]+v[1][2]*v[2][3]*v[3][1]+v[1][3]*v[2][1]*v[3][2]-v[3][1]*v[2][2]*v[1][3]-v[3][2]*v[2][3]*v[1][1]-v[3][3]*v[2][1]*v[1][2]);
			inv.v[1][0]=-(v[1][0]*v[2][2]*v[3][3]+v[1][2]*v[2][3]*v[3][0]+v[1][3]*v[2][0]*v[3][2]-v[3][0]*v[2][2]*v[1][3]-v[3][2]*v[2][3]*v[1][0]-v[3][3]*v[2][0]*v[1][2]);
			inv.v[2][0]=(v[1][0]*v[2][1]*v[3][3]+v[1][1]*v[2][3]*v[3][0]+v[1][3]*v[2][0]*v[3][1]-v[3][0]*v[2][1]*v[1][3]-v[3][1]*v[2][3]*v[1][0]-v[3][3]*v[2][0]*v[1][1]);
			inv.v[3][0]=-(v[1][0]*v[2][1]*v[3][2]+v[1][1]*v[2][2]*v[3][0]+v[1][2]*v[2][0]*v[3][1]-v[3][0]*v[2][1]*v[1][2]-v[3][1]*v[2][2]*v[1][0]-v[3][2]*v[2][0]*v[1][1]);
			REAL det=v[0][0]*inv.v[0][0]+v[0][1]*inv.v[1][0]+v[0][2]*inv.v[2][0]+v[0][3]*inv.v[3][0];
			return det;*/
		}
		Mat4 Mat4::identityMatrix()
		{
			Mat4 i;
			i.v00=(REAL)1;i.v11=(REAL)1;i.v22=(REAL)1;i.v33=(REAL)1;
			/*
			i.v[0][0]=(REAL)1;i.v[1][1]=(REAL)1;i.v[2][2]=(REAL)1;i.v[3][3]=(REAL)1;*/
			return i;
		}
		Mat4 Mat4::translation(const Vec3& trans)
		{
			Mat4 m=Mat4::identity;
			m.v30=trans.x;m.v31=trans.y;m.v32=trans.z;
			/*
			m.v[3][0]=trans.x;m.v[3][1]=trans.y;m.v[3][2]=trans.z;*/
			return m;
		}
		Mat4		Mat4::translation(REAL tx,REAL ty,REAL tz)
		{
			Mat4 m=Mat4::identity;
			m.v30=tx;m.v31=ty;m.v32=tz;
			return m;
		}
		Mat4 Mat4::rotation(const Vec3& axis,REAL angle)
		{
			Mat3 s=Mat3::skew(axis);
			return (Mat4)(Mat3::identity+SIN(angle)*s+(1-COS(angle))*s*s);
		}
		Mat4 Mat4::scaling(REAL sx,REAL sy,REAL sz)
		{
			Mat4 s;
			s.v00=sx;s.v11=sy;s.v22=sz;s.v33=(REAL)1;
			/*
			s.v[0][0]=sx;s.v[1][1]=sy;s.v[2][2]=sz;s.v[3][3]=(REAL)1;*/
			return s;
		}
		Mat4 Mat4::scaling(const Vec3& scale)
		{
			Mat4 s;
			s.v00=scale.x;s.v11=scale.y;s.v22=scale.z;s.v33=(REAL)1;
			/*
			s.v[0][0]=scale.x;s.v[1][1]=scale.y;s.v[2][2]=scale.z;s.v[3][3]=(REAL)1;*/
			return s;
		}
		Mat4 Mat4::lookingAt(const Vec3& eye,const Vec3& at,const Vec3& up)
		{
			Mat4 l;
			Vec3 x,y,z;
			z=(eye - at).normal_safe();
			x=(up ^ z).normal_safe();
			y=(z ^ x).normal_safe();
			l.v00=x.x;l.v01=y.x;l.v02=z.x;
			l.v10=x.y;l.v11=y.y;l.v12=z.y;
			l.v20=x.z;l.v21=y.z;l.v22=z.z;
			l.v30=-(eye*x);l.v31=-(eye*y);l.v32=-(eye*z);
			l.v33=1;
			/*
			l.v[0][0]=x.x;l.v[0][1]=y.x;l.v[0][2]=z.x;
			l.v[1][0]=x.y;l.v[1][1]=y.y;l.v[1][2]=z.y;
			l.v[2][0]=x.z;l.v[2][1]=y.z;l.v[2][2]=z.z;
			l.v[3][0]=-(eye*x);l.v[3][1]=-(eye*y);l.v[3][2]=-(eye*z);
			l.v[3][3]=1;*/
			return l;
		}
		//Mat4 Mat4::perspective(REAL left,REAL bottom,REAL right,REAL top,REAL znear,REAL zfar)
		//{
		//	Mat4 m;
		//	REAL inv_right_left=1/(right-left);
		//	REAL inv_top_bottom=1/(top-bottom);
		//	REAL inv_far_near=1/(zfar-znear);
		//	m.v00=2*znear*inv_right_left;
		//	m.v20=(right+left)*inv_right_left;
		//	m.v11=2*znear*inv_top_bottom;
		//	m.v21=(top+bottom)*inv_top_bottom;
		//	m.v22=-(zfar+znear)*inv_far_near;
		//	m.v32=-2*zfar*znear*inv_far_near;
		//	m.v23=-1;
		//	m.v33=0;
		//	return m;
		//}
		//Mat4 Mat4::perspective_inf(REAL left,REAL bottom,REAL right,REAL top,REAL znear)
		//{
		//	Mat4 m;
		//	REAL inv_right_left=1/(right-left);
		//	REAL inv_top_bottom=1/(top-bottom);
		//	m.v00=2*znear*inv_right_left;
		//	m.v20=(right+left)*inv_right_left;
		//	m.v11=2*znear*inv_top_bottom;
		//	m.v21=(top+bottom)*inv_top_bottom;
		//	m.v22=-1;
		//	m.v32=-2*znear;
		//	m.v23=-1;
		//	m.v33=0;
		//	return m;
		//}
		Mat4 Mat4::perspective(REAL left, REAL bottom, REAL right, REAL top, REAL znear, REAL zfar, REAL znear_map, REAL zfar_map, REAL sign)
		{
			Mat4 m;
			REAL inv_right_left = 1 / (right - left);
			REAL inv_top_bottom = 1 / (top - bottom);
			REAL inv_far_near = 1 / (zfar - znear);
			REAL _2znear = sign * 2 * znear;
			m.v00 = _2znear * inv_right_left;
			m.v20 = (right + left) * inv_right_left;
			m.v11 = _2znear * inv_top_bottom;
			m.v21 = (top + bottom) * inv_top_bottom;
			m.v22 = sign * (zfar_map * zfar - znear_map * znear) * inv_far_near;
			m.v32 = sign * znear * zfar * (znear_map - zfar_map) * inv_far_near;
			m.v23 = 1 * sign;
			m.v33 = 0;

			return m;
		}
		//Mat4 Mat4::perspectiveFOV(REAL fovy,REAL aspect,REAL znear,REAL zfar)
		//{
		//	REAL h=FABS(znear*TAN(fovy*(REAL)0.5));
		//	REAL w=aspect*h;
		//	Mat4 m=perspective(-w,-h,w,h,znear,zfar);
		//	return m;
		//}
		//Mat4 Mat4::perspectiveFOV_inf(REAL fovy,REAL aspect,REAL znear)
		//{
		//	REAL h=FABS(znear*TAN(fovy*(REAL)0.5));
		//	REAL w=aspect*h;
		//	Mat4 m=perspective_inf(-w,-h,w,h,znear);
		//	return m;
		//}
		Mat4 Mat4::perspectiveFOV(REAL fovy, REAL aspect, REAL znear, REAL zfar, REAL znear_map, REAL zfar_map, REAL sign)
		{
			REAL h = FABS(znear * TAN(fovy * (REAL)0.5));
			REAL w = aspect * h;
			Mat4 m = perspective(-w, -h, w, h, znear, zfar, znear_map, zfar_map, sign);
			return m;
		}
		//Mat4 Mat4::ortho(REAL left,REAL bottom,REAL right,REAL top,REAL znear,REAL zfar)
		//{
		//	Mat4 m;
		//	REAL inv_right_left=1/(right-left);
		//	REAL inv_top_bottom=1/(top-bottom);
		//	REAL inv_far_near=1/(zfar-znear);
		//	m.v00=2*inv_right_left;
		//	m.v30=-(right+left)*inv_right_left;
		//	m.v11=2*inv_top_bottom;
		//	m.v31=-(top+bottom)*inv_top_bottom;
		//	m.v22=-2*inv_far_near;
		//	m.v32=-(zfar+znear)*inv_far_near;
		//	m.v33=1;
		//	return m;
		//}
		//Mat4 Mat4::ortho_inf(REAL left,REAL bottom,REAL right,REAL top,REAL znear)
		//{
		//	Mat4 m;
		//	REAL inv_right_left=1/(right-left);
		//	REAL inv_top_bottom=1/(top-bottom);
		//	m.v00=2*inv_right_left;
		//	m.v30=-(right+left)*inv_right_left;
		//	m.v11=2*inv_top_bottom;
		//	m.v31=-(top+bottom)*inv_top_bottom;
		//	m.v22=0;
		//	m.v32=-1;
		//	m.v33=1;
		//	return m;
		//}
		Mat4 Mat4::ortho(REAL left,REAL bottom,REAL right,REAL top,REAL znear,REAL zfar,REAL znear_map,REAL zfar_map)
		{
			Mat4 m;
			REAL inv_right_left = 1 / (right - left);
			REAL inv_top_bottom = 1 / (top - bottom);
			REAL inv_far_near = 1 / (zfar - znear);
			m.v00 = 2 * inv_right_left;
			m.v30 = -(right + left) * inv_right_left;
			m.v11 = 2 * inv_top_bottom;
			m.v31 = -(top + bottom) * inv_top_bottom;
			m.v22 = (zfar_map - znear_map) * inv_far_near;
			m.v32 = (znear_map * zfar - zfar_map * znear) * inv_far_near;
			m.v33 = 1;
			return m;
		}
		Mat4		Mat4::adjustForObliqueNearClippingPlane(const Mat4& proj, const Plane& p, REAL znear_mapping, REAL zfar_mapping)
		{
			Mat4 inv = proj.inverse();

			Vec3 c = Vec3(0, 0, zfar_mapping) * inv;
			Vec3 r = Vec3(1, 0, zfar_mapping) * inv;
			Vec3 t = Vec3(0, 1, zfar_mapping) * inv;

			Vec3 x = r - c;
			Vec3 y = t - c;

			Vec3 q = c + x * SETSIGN(1.0f, SIGNBIT(x * p.normal())) + y * SETSIGN(1.0f, SIGNBIT(y * p.normal()));

			Mat4 m = proj;

			if(znear_mapping < zfar_mapping)
			{
				REAL f_n = zfar_mapping - znear_mapping;
				REAL m4[4] = {f_n * proj.a14, f_n * proj.a24, f_n * proj.a34, f_n * proj.a44};
				REAL num = q[0] * m4[0] + q[1] * m4[1] + q[2] * m4[2] + m4[3];
				REAL den = p.sub(q);
				REAL k = num / den;
				if(!FINITE(k))
					k = 1.0f;

				m.a13 = znear_mapping * proj.a14 + k * p.a;
				m.a23 = znear_mapping * proj.a24 + k * p.b;
				m.a33 = znear_mapping * proj.a34 + k * p.c;
				m.a43 = znear_mapping * proj.a44 + k * p.d;
			}
			else
			{
				REAL n_f = znear_mapping - zfar_mapping;
				REAL m4[4] = {n_f * proj.a14, n_f * proj.a24, n_f * proj.a34, n_f * proj.a44};
				REAL num = q[0] * m4[0] + q[1] * m4[1] + q[2] * m4[2] + m4[3];
				REAL den = p.sub(q);
				REAL k = num / den;
				if(!FINITE(k))
					k = 1.0f;

				m.a13 = znear_mapping * proj.a14 - k * p.a;
				m.a23 = znear_mapping * proj.a24 - k * p.b;
				m.a33 = znear_mapping * proj.a34 - k * p.c;
				m.a43 = znear_mapping * proj.a44 - k * p.d;
			}

			return m;
		}
		Mat4 Mat4::mirror(const Plane& p)
		{
			const REAL n1[3][3] = {{p.a, p.a, p.a}, {p.b, p.b, p.b}, {p.c, p.c, p.c}};
			const REAL n2[3][3] = {{p.a, 0, 0}, {0, p.b, 0}, {0, 0, p.c}};
			return ((Mat4)(Mat3::identity - 2 * Mat3(n1) * Mat3(n2))) * Mat4::translation(-2 * p.d * Vec3(p.a, p.b, p.c));
		}
		Mat4		Mat4::rotationX(REAL angle)
		{
			REAL c = COS(angle);
			REAL s = SIN(angle);
			Mat4 m;
			m[0][0] = 1;
			m[1][1] = c;
			m[1][2] = s;
			m[2][1] = -s;
			m[2][2] = c;
			m[3][3] = 1;

			return m;
		}
		Mat4		Mat4::rotationY(REAL angle)
		{
			REAL c = COS(angle);
			REAL s = SIN(angle);
			Mat4 m;
			m[1][1] = 1;
			m[0][0] = c;
			m[0][2] = -s;
			m[2][0] = s;
			m[2][2] = c;
			m[3][3] = 1;

			return m;
		}
		Mat4		Mat4::rotationZ(REAL angle)
		{
			REAL c = COS(angle);
			REAL s = SIN(angle);
			Mat4 m;
			m[2][2] = 1;
			m[0][0] = c;
			m[0][1] = s;
			m[1][0] = -s;
			m[1][1] = c;
			m[3][3] = 1;

			return m;
		}
		Mat4		Mat4::closest(const Plane& p)
		{
			Mat4 m;
			REAL aa = p.a * p.a;
			REAL ab = p.a * p.b;
			REAL ac = p.a * p.c;
			REAL ad = p.a * p.d;
			REAL bb = p.b * p.b;
			REAL bc = p.b * p.c;
			REAL bd = p.b * p.d;
			REAL cc = p.c * p.c;
			REAL cd = p.c * p.d;
			m.v00 = 1 - aa;
			m.v10 = -ab;
			m.v20 = -ac;
			m.v30 = -ad;
			m.v01 = -ab;
			m.v11 = 1 - bb;
			m.v21 = -bc;
			m.v31 = -bd;
			m.v02 = -ac;
			m.v12 = -bc;
			m.v22 = 1 - cc;
			m.v32 = -cd;
			m.v33 = 1;

			return m;
		}
		Mat4& Mat4::translate(const Vec3& trans)
		{
			return (*this) = (*this) * translation(trans);
		}
		Mat4& Mat4::rotate(const Vec3& axis,REAL angle)
		{
			return (*this) = (*this) * rotation(axis, angle);
		}
		Mat4& Mat4::scale(REAL sx, REAL sy, REAL sz)
		{
			return (*this) = (*this) * scaling(sx, sy, sz);
		}
		Mat4& Mat4::scale(const Vec3& scale)
		{
			return (*this) = (*this) * scaling(scale);
		}
		Mat4& Mat4::lookAt(const Vec3& eye, const Vec3& at, const Vec3& up)
		{
			return (*this) = (*this) * lookingAt(eye, at, up);
		}
		Mat4& Mat4::setTranslation(const Vec3& trans)
		{
			return (*this) = translation(trans);
		}
		Mat4& Mat4::setRotation(const Vec3& axis, REAL angle)
		{
			return (*this) = rotation(axis, angle);
		}
		Mat4& Mat4::setScaling(REAL sx, REAL sy, REAL sz)
		{
			return (*this) = scaling(sx,sy,sz);
		}
		Mat4& Mat4::setScaling(const Vec3& scale)
		{
			return (*this) = scaling(scale);
		}
		Mat4& Mat4::setLookingAt(const Vec3& eye, const Vec3& at, const Vec3& up)
		{
			return (*this) = lookingAt(eye, at, up);
		}
		void Mat4::factorSRT(Vec3& scale, Mat3& rotation, Vec3& translation) const
		{
			translation = Vec3(v[3]);
			((Mat3)*this).factorSR(scale, rotation);
		}
		void Mat4::factorSRT(Vec3& scale, Quat& rotation, Vec3& translation) const
		{
			translation = Vec3(v[3]);
			((Mat3)*this).factorSR(scale, rotation);
		}
		bool			Mat4::testIdentity(REAL threshold) const
		{
			REAL len = 0.0f;
			len = max(len, (Vec3::xAxis - Vec3::xAxis * (*this)).length());
			len = max(len, (Vec3::yAxis - Vec3::yAxis * (*this)).length());
			len = max(len, (Vec3::zAxis - Vec3::zAxis * (*this)).length());

			if(len <= threshold)
				return true;
			return false;
		}
		Mat4 operator*(REAL scalar, const Mat4& right)
		{
			return right * scalar;
		}
	};
};