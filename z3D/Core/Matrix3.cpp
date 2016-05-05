
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
		Mat3::Mat3()
		{
			memset(v, 0, sizeof(v));
		}
		Mat3::Mat3(const REAL v[3][3])
		{
			memcpy(this->v, v, sizeof(this->v));
		}
		Mat3::Mat3(const Mat3& src)
		{
			(*this) = src;
		}
		Mat3::Mat3(const Vec3& v0,const Vec3& v1,const Vec3& v2,bool bRowByRow)
		{
			if(bRowByRow)
			{
				v00=v0.x;v01=v0.y;v02=v0.z;
				v10=v1.x;v11=v1.y;v12=v1.z;
				v20=v2.x;v21=v2.y;v22=v2.z;
			}
			else
			{
				v00=v0.x;v01=v1.x;v02=v2.x;
				v10=v0.y;v11=v1.y;v12=v2.y;
				v20=v0.z;v21=v1.z;v22=v2.z;
			}
			/*
			if(bRowByRow)
			{
				v[0][0]=v0[0];v[0][1]=v0[1];v[0][2]=v0[2];
				v[1][0]=v1[0];v[1][1]=v1[1];v[1][2]=v1[2];
				v[2][0]=v2[0];v[2][1]=v2[1];v[2][2]=v2[2];
			}
			else
			{
				v[0][0]=v0[0];v[0][1]=v1[0];v[0][2]=v2[0];
				v[1][0]=v0[1];v[1][1]=v1[1];v[1][2]=v2[1];
				v[2][0]=v0[2];v[2][1]=v1[2];v[2][2]=v2[2];
			}*/
		}
		Mat3::Mat3(const Vec3* const v,bool bRowByRow)
		{
			int i,j;
			if(bRowByRow)
			{
				for(i=0;i<3;i++)
					for(j=0;j<3;j++)
						this->v[i][j]=v[i][j];
			}
			else
			{
				for(i=0;i<3;i++)
					for(j=0;j<3;j++)
						this->v[i][j]=v[j][i];
			}
		}
		Mat3::operator Mat4() const
		{
			Mat4 other;
			other.v00=v00;other.v01=v01;other.v02=v02;
			other.v10=v10;other.v11=v11;other.v12=v12;
			other.v20=v20;other.v21=v21;other.v22=v22;
			other.v33=1.0;
			/*
			other.v[0][0]=v[0][0];other.v[0][1]=v[0][1];other.v[0][2]=v[0][2];
			other.v[1][0]=v[1][0];other.v[1][1]=v[1][1];other.v[1][2]=v[1][2];
			other.v[2][0]=v[2][0];other.v[2][1]=v[2][1];other.v[2][2]=v[2][2];
			other.v[3][3]=1.0;*/
			return other;
		}
		Mat3& Mat3::operator=(const Mat3& other)
		{
			memcpy(v, other.v, sizeof(v));
			return (*this);
		}
		REAL* Mat3::operator[](ptrdiff_t row)
		{
			return v[row];
		}
		const REAL* Mat3::operator[](ptrdiff_t row) const
		{
			return v[row];
		}
		Mat3& Mat3::setIdentity()
		{
			v00=(REAL)1.0;v01=(REAL)0.0;v02=(REAL)0.0;
			v10=(REAL)0.0;v11=(REAL)1.0;v12=(REAL)0.0;
			v20=(REAL)0.0;v21=(REAL)0.0;v22=(REAL)1.0;
			/*
			v[0][0]=(REAL)1.0;v[0][1]=(REAL)0.0;v[0][2]=(REAL)0.0;
			v[1][0]=(REAL)0.0;v[1][1]=(REAL)1.0;v[1][2]=(REAL)0.0;
			v[2][0]=(REAL)0.0;v[2][1]=(REAL)0.0;v[2][2]=(REAL)1.0;*/
			return (*this);
		}
		Mat3 Mat3::transpose() const
		{
			Mat3 other;
			other.v00=v00;other.v01=v10;other.v02=v20;
			other.v10=v01;other.v11=v11;other.v12=v21;
			other.v20=v02;other.v21=v12;other.v22=v22;
			/*
			other.v[0][0]=v[0][0];other.v[0][1]=v[1][0];other.v[0][2]=v[2][0];
			other.v[1][0]=v[0][1];other.v[1][1]=v[1][1];other.v[1][2]=v[2][1];
			other.v[2][0]=v[0][2];other.v[2][1]=v[1][2];other.v[2][2]=v[2][2];*/
			return other;
		}
		Mat3& Mat3::operator+=(const Mat3& right)
		{
			v00+=right.v00;v01+=right.v01;v02+=right.v02;
			v10+=right.v10;v11+=right.v11;v12+=right.v12;
			v20+=right.v20;v21+=right.v21;v22+=right.v22;
			/*
			v[0][0]+=right.v[0][0];v[0][1]+=right.v[0][1];v[0][2]+=right.v[0][2];
			v[1][0]+=right.v[1][0];v[1][1]+=right.v[1][1];v[1][2]+=right.v[1][2];
			v[2][0]+=right.v[2][0];v[2][1]+=right.v[2][1];v[2][2]+=right.v[2][2];*/
			return (*this);
		}
		Mat3 Mat3::operator+(const Mat3& right) const
		{
			Mat3 p=(*this);
			return p+=right;
		}
		Mat3& Mat3::operator-=(const Mat3& right)
		{
			v00-=right.v00;v01-=right.v01;v02-=right.v02;
			v10-=right.v10;v11-=right.v11;v12-=right.v12;
			v20-=right.v20;v21-=right.v21;v22-=right.v22;
			/*
			v[0][0]-=right.v[0][0];v[0][1]-=right.v[0][1];v[0][2]-=right.v[0][2];
			v[1][0]-=right.v[1][0];v[1][1]-=right.v[1][1];v[1][2]-=right.v[1][2];
			v[2][0]-=right.v[2][0];v[2][1]-=right.v[2][1];v[2][2]-=right.v[2][2];*/
			return (*this);
		}
		Mat3 Mat3::operator-(const Mat3& right) const
		{
			Mat3 p=(*this);
			return p-=right;
		}
		Mat3& Mat3::operator*=(const Mat3& right)
		{
			return (*this)=(*this)*right;
		}
		Mat3 Mat3::operator*(const Mat3& right) const
		{
			Mat3 p;
			p.v00=v00*right.v00+v01*right.v10+v02*right.v20;
			p.v01=v00*right.v01+v01*right.v11+v02*right.v21;
			p.v02=v00*right.v02+v01*right.v12+v02*right.v22;
			p.v10=v10*right.v00+v11*right.v10+v12*right.v20;
			p.v11=v10*right.v01+v11*right.v11+v12*right.v21;
			p.v12=v10*right.v02+v11*right.v12+v12*right.v22;
			p.v20=v20*right.v00+v21*right.v10+v22*right.v20;
			p.v21=v20*right.v01+v21*right.v11+v22*right.v21;
			p.v22=v20*right.v02+v21*right.v12+v22*right.v22;
			/*
			p.v[0][0]=v[0][0]*right.v[0][0]+v[0][1]*right.v[1][0]+v[0][2]*right.v[2][0];
			p.v[0][1]=v[0][0]*right.v[0][1]+v[0][1]*right.v[1][1]+v[0][2]*right.v[2][1];
			p.v[0][2]=v[0][0]*right.v[0][2]+v[0][1]*right.v[1][2]+v[0][2]*right.v[2][2];
			p.v[1][0]=v[1][0]*right.v[0][0]+v[1][1]*right.v[1][0]+v[1][2]*right.v[2][0];
			p.v[1][1]=v[1][0]*right.v[0][1]+v[1][1]*right.v[1][1]+v[1][2]*right.v[2][1];
			p.v[1][2]=v[1][0]*right.v[0][2]+v[1][1]*right.v[1][2]+v[1][2]*right.v[2][2];
			p.v[2][0]=v[2][0]*right.v[0][0]+v[2][1]*right.v[1][0]+v[2][2]*right.v[2][0];
			p.v[2][1]=v[2][0]*right.v[0][1]+v[2][1]*right.v[1][1]+v[2][2]*right.v[2][1];
			p.v[2][2]=v[2][0]*right.v[0][2]+v[2][1]*right.v[1][2]+v[2][2]*right.v[2][2];*/
			return p;
		}
		Mat3& Mat3::operator*=(REAL scalar)
		{
			v00*=scalar;v01*=scalar;v02*=scalar;
			v10*=scalar;v11*=scalar;v12*=scalar;
			v20*=scalar;v21*=scalar;v22*=scalar;
			/*
			v[0][0]*=scalar;v[0][1]*=scalar;v[0][2]*=scalar;
			v[1][0]*=scalar;v[1][1]*=scalar;v[1][2]*=scalar;
			v[2][0]*=scalar;v[2][1]*=scalar;v[2][2]*=scalar;*/
			return (*this);
		}
		Mat3 Mat3::operator*(REAL scalar) const
		{
			return (Mat3(*this))*=scalar;
		}
		Mat3 Mat3::inverse(bool* invertible) const
		{
			REAL det = det3();

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
			Mat3 inv;
			inv.v00=(v11*v22-v21*v12)*inv_det;
			inv.v01=(v21*v02-v01*v22)*inv_det;
			inv.v02=(v01*v12-v11*v02)*inv_det;
			inv.v10=(v12*v20-v22*v10)*inv_det;
			inv.v11=(v22*v00-v02*v20)*inv_det;
			inv.v12=(v02*v10-v12*v00)*inv_det;
			inv.v20=(v10*v21-v20*v11)*inv_det;
			inv.v21=(v20*v01-v00*v21)*inv_det;
			inv.v22=(v00*v11-v10*v01)*inv_det;
			/*
			inv.v[0][0]=(v[1][1]*v[2][2]-v[2][1]*v[1][2])*inv_det;
			inv.v[0][1]=(v[2][1]*v[0][2]-v[0][1]*v[2][2])*inv_det;
			inv.v[0][2]=(v[0][1]*v[1][2]-v[1][1]*v[0][2])*inv_det;
			inv.v[1][0]=(v[1][2]*v[2][0]-v[2][2]*v[1][0])*inv_det;
			inv.v[1][1]=(v[2][2]*v[0][0]-v[0][2]*v[2][0])*inv_det;
			inv.v[1][2]=(v[0][2]*v[1][0]-v[1][2]*v[0][0])*inv_det;
			inv.v[2][0]=(v[1][0]*v[2][1]-v[2][0]*v[1][1])*inv_det;
			inv.v[2][1]=(v[2][0]*v[0][1]-v[0][0]*v[2][1])*inv_det;
			inv.v[2][2]=(v[0][0]*v[1][1]-v[1][0]*v[0][1])*inv_det;*/
			return inv;
		}
		REAL Mat3::det3() const
		{
			REAL det=(REAL)0.0;
			det+=v00*v11*v22;
			det+=v01*v12*v20;
			det+=v02*v10*v21;
			det-=v20*v11*v02;
			det-=v21*v12*v00;
			det-=v22*v10*v01;
			/*
			det+=v[0][0]*v[1][1]*v[2][2];
			det+=v[0][1]*v[1][2]*v[2][0];
			det+=v[0][2]*v[1][0]*v[2][1];
			det-=v[2][0]*v[1][1]*v[0][2];
			det-=v[2][1]*v[1][2]*v[0][0];
			det-=v[2][2]*v[1][0]*v[0][1];*/
			return det;
		}
		Mat3 Mat3::identityMatrix()
		{
			Mat3 i;
			i.v00=(REAL)1;i.v11=(REAL)1;i.v22=(REAL)1;
			/*
			i.v[0][0]=(REAL)1;i.v[1][1]=(REAL)1;i.v[2][2]=(REAL)1;*/
			return i;
		}
		Mat3 Mat3::skew(const Vec3& u)
		{
			Mat3 skew;
			skew.v01=u.z;
			skew.v02=-u.y;
			skew.v10=-u.z;
			skew.v12=u.x;
			skew.v20=u.y;
			skew.v21=-u.x;
			/*
			skew.v[0][1]=u[2];
			skew.v[0][2]=-u[1];
			skew.v[1][0]=-u[2];
			skew.v[1][2]=u[0];
			skew.v[2][0]=u[1];
			skew.v[2][1]=-u[0];*/
			return skew;
		}
		Mat3 Mat3::rotation(const Vec3& axis,REAL angle)
		{
			Mat3 s=skew(axis);
			return identity+SIN(angle)*s+((REAL)1-COS(angle))*s*s;
		}
		Mat3 Mat3::scaling(REAL sx,REAL sy,REAL sz)
		{
			Mat3 s;
			s.v00=sx;s.v11=sy;s.v22=sz;
			/*
			s.v[0][0]=sx;s.v[1][1]=sy;s.v[2][2]=sz;*/
			return s;
		}
		Mat3 Mat3::scaling(const Vec3& scale)
		{
			Mat3 s;
			s.v00=scale.x;s.v11=scale.y;s.v22=scale.z;
			/*
			s.v[0][0]=scale.x;s.v[1][1]=scale.y;s.v[2][2]=scale.z;*/
			return s;
		}
		Mat3 Mat3::mirror(const Vec3& n)
		{
			const REAL n1[3][3] = {{n.x, n.x, n.x}, {n.y, n.y, n.y}, {n.z, n.z, n.z}};
			const REAL n2[3][3] = {{n.x, 0, 0}, {0, n.y, 0}, {0, 0, n.z}};
			return Mat3::identity - 2 * Mat3(n1) * Mat3(n2);
		}
		Mat3		Mat3::rotationX(REAL angle)
		{
			REAL c = COS(angle);
			REAL s = SIN(angle);
			Mat3 m;
			m[0][0] = 1;
			m[1][1] = c;
			m[1][2] = s;
			m[2][1] = -s;
			m[2][2] = c;

			return m;
		}
		Mat3		Mat3::rotationY(REAL angle)
		{
			REAL c = COS(angle);
			REAL s = SIN(angle);
			Mat3 m;
			m[1][1] = 1;
			m[0][0] = c;
			m[0][2] = -s;
			m[2][0] = s;
			m[2][2] = c;

			return m;
		}
		Mat3		Mat3::rotationZ(REAL angle)
		{
			REAL c = COS(angle);
			REAL s = SIN(angle);
			Mat3 m;
			m[2][2] = 1;
			m[0][0] = c;
			m[0][1] = s;
			m[1][0] = -s;
			m[1][1] = c;

			return m;
		}
		Mat3& Mat3::rotate(const Vec3& axis, REAL angle)
		{
			return (*this) = (*this) * rotation(axis, angle);
		}
		Mat3& Mat3::scale(REAL sx, REAL sy, REAL sz)
		{
			return (*this) = (*this) * scaling(sx, sy, sz);
		}
		Mat3& Mat3::scale(const Vec3& scale)
		{
			return (*this) = (*this) * scaling(scale);
		}
		Mat3& Mat3::setRotation(const Vec3& axis, REAL angle)
		{
			return (*this) = rotation(axis, angle);
		}
		Mat3& Mat3::setScaling(REAL sx, REAL sy, REAL sz)
		{
			return (*this) = scaling(sx, sy, sz);
		}
		Mat3& Mat3::setScaling(const Vec3& scale)
		{
			return (*this) = scaling(scale);
		}
		void Mat3::factorXYZ(REAL& rx,REAL& ry,REAL& rz) const
		{
			ry=ASIN(v[2][0]);
			if(ry<90)
			{
				if(ry>-90)
				{
					rx=ATAN2(-v[2][1],v[2][2]);
					rz=ATAN2(-v[1][0],v[0][0]);
				}
				else
				{
					rx=-ATAN2(v[0][1],v[1][1]);
					rz=0;
				}
			}
			else
			{
				rx=ATAN2(v[0][1],v[1][1]);
				rz=0;
			}
		}
		void Mat3::factorXZY(REAL& rx,REAL& rz,REAL& ry) const
		{
			rz=ASIN(-v[1][0]);
			if(rz<90)
			{
				if(rz>-90)
				{
					rx=ATAN2(v[1][2],v[2][2]);
					ry=ATAN2(v[2][0],v[0][0]);
				}
				else
				{
					rx=-ATAN2(-v[0][2],v[2][2]);
					ry=0;
				}
			}
			else
			{
				rx=ATAN2(v[0][2],v[2][2]);
				ry=0;
			}
		}
		void Mat3::factorYXZ(REAL& ry,REAL& rx,REAL& rz) const
		{
			rx=ASIN(-v[2][1]);
			if(rx<90)
			{
				if(rx>-90)
				{
					ry=ATAN2(v[2][0],v[2][2]);
					rz=ATAN2(v[0][1],v[1][1]);
				}
				else
				{
					ry=ATAN2(-v[1][0],v[0][0]);
					rz=0;
				}
			}
			else
			{
				ry=ATAN2(v[1][0],v[0][0]);
				rz=0;
			}
		}
		void Mat3::factorYZX(REAL& ry,REAL& rz,REAL& rx) const
		{
			rz=ASIN(v[0][1]);
			if(rz<90)
			{
				if(rz>-90)
				{
					ry=ATAN2(-v[0][2],v[0][0]);
					rx=ATAN2(-v[2][1],v[1][1]);
				}
				else
				{
					ry=-ATAN2(v[1][2],v[2][2]);
					rx=0;
				}
			}
			else
			{
				ry=ATAN2(v[1][2],v[2][2]);
				rx=0;
			}
		}
		void Mat3::factorZXY(REAL& rz,REAL& rx,REAL& ry) const
		{
			rx=ASIN(v[1][2]);
			if(rx<90)
			{
				if(rx>-90)
				{
					rz=ATAN2(-v[1][0],v[1][1]);
					ry=ATAN2(-v[0][2],v[2][2]);
				}
				else
				{
					rz=-ATAN2(v[2][0],v[0][0]);
					ry=0;
				}
			}
			else
			{
				rz=ATAN2(v[2][0],v[0][0]);
				ry=0;
			}
		}
		void Mat3::factorZYX(REAL& rz,REAL& ry,REAL& rx) const
		{
			ry=ASIN(-v[0][2]);
			if(ry<90)
			{
				if(ry>-90)
				{
					rz=ATAN2(v[0][1],v[0][0]);
					rx=ATAN2(v[1][2],v[2][2]);
				}
				else
				{
					rz=-ATAN2(-v[1][0],-v[2][0]);
					rx=0;
				}
			}
			else
			{
				rz=-ATAN2(v[1][0],v[2][0]);
				rx=0;
			}
		}
		void			Mat3::resolveOriRH(REAL& yaw, REAL& pitch) const
		{
			const Vec3& x = *(const Vec3*)v[0];
			const Vec3& y = *(const Vec3*)v[1];
			const Vec3& z = *(const Vec3*)v[2];

			if((x ^ y) * z > 0.0f)
			{
				pitch = SETSIGN(ACOS(y.y), SIGNBIT(-z.y));
				yaw = SETSIGN(ACOS(x.x), SIGNBIT(-x.z));
			}
			else						// don't how to handle that case...
			{
				pitch = SETSIGN(ACOS(y.y), SIGNBIT(z.y));
				yaw = SETSIGN(ACOS(x.x), SIGNBIT(-x.z));
			}
		}
		void Mat3::factorSR(Vec3& scale, Mat3& rot) const
		{
			scale[0] = Vec3(v[0]).length();
			scale[1] = Vec3(v[1]).length();
			scale[2] = Vec3(v[2]).length();
			rot = Mat3(Vec3(v[0]) / scale[0], Vec3(v[1]) / scale[1], Vec3(v[2]) / scale[2]);

			// old method

			//rot=Mat3((Vec3::xAxis*(*this)).normal(),(Vec3::yAxis*(*this)).normal(),(Vec3::zAxis*(*this)).normal());
			//Mat3 s=(*this)*rot.transpose();
			//scale=Vec3(s[0][0],s[1][1],s[2][2]);
		}
		void Mat3::factorSR(Vec3& scale, Quat& rot) const
		{
			Vec3 v0 = Vec3(v[0]);
			Vec3 v1 = Vec3(v[1]);
			Vec3 v2 = Vec3(v[2]);
			scale[0] = v0.length();
			scale[1] = v1.length();
			scale[2] = v2.length();
			Vec3 n0 = v0 / scale[0];
			Vec3 n1 = v1 / scale[1];
			Vec3 n2 = v2 / scale[2];
			if((n0 ^ n1) * n2 < 0)
			{
				scale[2] = -scale[2];
				n2 = -n2;
			}
			rot = Quat(Mat3(n0, n1, n2));

			// old method

			//Mat3 r=Mat3((Vec3::xAxis*(*this)).normal(),(Vec3::yAxis*(*this)).normal(),(Vec3::zAxis*(*this)).normal());
			//Mat3 s=(*this)*r.transpose();
			//scale=Vec3(s[0][0],s[1][1],s[2][2]);
			//rot=Quat(r);
		}
		bool			Mat3::testIdentity(REAL threshold) const
		{
			REAL len = 0.0f;
			len = max(len, (Vec3::xAxis - Vec3::xAxis * (*this)).length());
			len = max(len, (Vec3::yAxis - Vec3::yAxis * (*this)).length());
			len = max(len, (Vec3::zAxis - Vec3::zAxis * (*this)).length());

			if(len <= threshold)
				return true;
			return false;
		}
		Mat3 operator*(REAL scalar, const Mat3& right)
		{
			return right * scalar;
		}
	};
};
