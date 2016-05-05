
#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "MathFunc.h"
#include "Quaternion.h"
#include "Constants.h"

namespace z3D
{
	namespace Core
	{
		Mat2::Mat2()
		{
			memset(v, 0, sizeof(v));
		}
		Mat2::Mat2(const REAL v[2][2])
		{
			memcpy(this->v, v, sizeof(this->v));
		}
		Mat2::Mat2(const Mat2& src)
		{
			(*this) = src;
		}
		Mat2::Mat2(const Vec2& v0,const Vec2& v1,bool bRowByRow)
		{
			if(bRowByRow)
			{
				v[0][0]=v0[0];v[0][1]=v0[1];
				v[1][0]=v1[0];v[1][1]=v1[1];
			}
			else
			{
				v[0][0]=v0[0];v[0][1]=v1[0];
				v[1][0]=v0[1];v[1][1]=v1[1];
			}
		}
		Mat2::Mat2(const Vec2* const v,bool bRowByRow)
		{
			int i,j;
			if(bRowByRow)
			{
				for(i=0;i<2;i++)
					for(j=0;j<2;j++)
						this->v[i][j]=v[i][j];
			}
			else
			{
				for(i=0;i<2;i++)
					for(j=0;j<2;j++)
						this->v[i][j]=v[j][i];
			}
		}
		Mat2::operator Mat3() const
		{
			Mat3 other;
			other.v[0][0]=v[0][0];other.v[0][1]=v[0][1];
			other.v[1][0]=v[1][0];other.v[1][1]=v[1][1];

			other.v[2][2]=1.0;
			return other;
		}
		Mat2::operator Mat4() const
		{
			Mat4 other;
			other.v[0][0]=v[0][0];other.v[0][1]=v[0][1];
			other.v[1][0]=v[1][0];other.v[1][1]=v[1][1];

			other.v[2][2]=1.0;
			other.v[3][3]=1.0;
			return other;
		}
		Mat2&			Mat2::operator=(const Mat2& other)
		{
			memcpy(v, other.v, sizeof(v));
			return (*this);
		}
		REAL*			Mat2::operator[](ptrdiff_t row)
		{
			return v[row];
		}
		const REAL*		Mat2::operator[](ptrdiff_t row) const
		{
			return v[row];
		}
		Mat2&			Mat2::setIdentity()
		{
			v[0][0]=(REAL)1.0;v[0][1]=(REAL)0.0;
			v[1][0]=(REAL)0.0;v[1][1]=(REAL)1.0;
			return (*this);
		}
		Mat2				Mat2::transpose() const
		{
			Mat2 other;
			other.v[0][0]=v[0][0];other.v[0][1]=v[1][0];
			other.v[1][0]=v[0][1];other.v[1][1]=v[1][1];
			return other;
		}
		Mat2&			Mat2::operator+=(const Mat2& right)
		{
			v[0][0]+=right.v[0][0];v[0][1]+=right.v[0][1];
			v[1][0]+=right.v[1][0];v[1][1]+=right.v[1][1];
			return (*this);
		}
		Mat2				Mat2::operator+(const Mat2& right) const
		{
			Mat2 p=(*this);
			return p+=right;
		}
		Mat2&			Mat2::operator-=(const Mat2& right)
		{
			v[0][0]-=right.v[0][0];v[0][1]-=right.v[0][1];
			v[1][0]-=right.v[1][0];v[1][1]-=right.v[1][1];
			return (*this);
		}
		Mat2				Mat2::operator-(const Mat2& right) const
		{
			Mat2 p=(*this);
			return p-=right;
		}
		Mat2&			Mat2::operator*=(const Mat2& right)
		{
			return (*this)=(*this)*right;
		}
		Mat2				Mat2::operator*(const Mat2& right) const
		{
			Mat2 p;
			p.v[0][0]=v[0][0]*right.v[0][0]+v[0][1]*right.v[1][0];
			p.v[0][1]=v[0][0]*right.v[0][1]+v[0][1]*right.v[1][1];
			p.v[1][0]=v[1][0]*right.v[0][0]+v[1][1]*right.v[1][0];
			p.v[1][1]=v[1][0]*right.v[0][1]+v[1][1]*right.v[1][1];
			return p;
		}
		Mat2&			Mat2::operator*=(REAL scalar)
		{
			v[0][0]*=scalar;v[0][1]*=scalar;
			v[1][0]*=scalar;v[1][1]*=scalar;
			return (*this);
		}
		Mat2				Mat2::operator*(REAL scalar) const
		{
			return (Mat2(*this))*=scalar;
		}
		Mat2				Mat2::inverse(bool* invertible) const
		{
			REAL det=det2();

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
			Mat2 inv;
			inv.v[0][0]=v[1][1]*inv_det;
			inv.v[0][1]=-v[0][1]*inv_det;
			inv.v[1][0]=-v[1][0]*inv_det;
			inv.v[1][1]=v[0][0]*inv_det;
			return inv;
		}
		REAL				Mat2::det2() const
		{
			return v[0][0]*v[1][1]-v[0][1]*v[1][0];
		}
		Mat2				Mat2::identityMatrix()
		{
			Mat2 m;
			m[0][0]=m[1][1]=1;

			return m;
		}
		Mat2				Mat2::rotation(REAL angle)
		{
			Mat2 m;
			REAL c=COS(angle);
			REAL s=SIN(angle);
			m[0][0]=c;
			m[0][1]=s;
			m[1][0]=-s;
			m[1][1]=c;

			return m;
		}
		Mat2				Mat2::scaling(REAL sx,REAL sy)
		{
			Mat2 m;
			m[0][0]=sx;
			m[1][1]=sy;

			return m;
		}
		Mat2&			Mat2::rotate(REAL angle)
		{
			return (*this)=(*this)*rotation(angle);
		}
		Mat2&			Mat2::scale(REAL sx,REAL sy)
		{
			return (*this)=(*this)*scaling(sx,sy);
		}
		Mat2&			Mat2::setRotation(REAL angle)
		{
			return (*this)=rotation(angle);
		}
		Mat2&			Mat2::setScaling(REAL sx,REAL sy)
		{
			return (*this)=scaling(sx,sy);
		}
		Mat2		operator*(REAL scalar,const Mat2& right)
		{
			return right*scalar;
		}
	};
};
