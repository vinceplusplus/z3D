
#include "Constants.h"
#include "Quaternion.h"
#include "MathFunc.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Matrix4.h"

#include <fvec.h>

namespace z3D
{
	namespace Core
	{
		Quat::Quat()
		{
			memset(&v, 0, sizeof(v));
		}
		Quat::Quat(REAL x, REAL y, REAL z)
		{
			this->x = x; this->y = y; this->z = z; w = (REAL)0;
		}
		Quat::Quat(REAL x, REAL y, REAL z, REAL w)
		{
			this->x = x; this->y = y; this->z = z; this->w = w;
		}
		Quat::Quat(const REAL src[4])
		{
			memcpy(v, src, sizeof(v));
		}
		Quat::Quat(const Vec3& v)
		{
			x = v.x; y = v.y; z = v.z; w = (REAL)0;
		}
		Quat::Quat(const Vec3& axis, REAL angle)
		{
			REAL s;
			s = SIN(angle * 0.5f);
			w = COS(angle * 0.5f);
			x = s * axis.x;
			y = s * axis.y;
			z = s * axis.z;
		}
		static F32vec4 rsqrt_nr_x2(const F32vec4 &x)
		{
			static const F32vec4 fvecf0pt5(0.5f);
			static const F32vec4 fvecf3pt0(3.0f);
			F32vec4 y = _mm_rsqrt_ps(x);
			y = (fvecf0pt5 * y) * (fvecf3pt0 - (x * y) * y);
			return (fvecf0pt5 * y) * (fvecf3pt0 - (x * y) * y);
		}
		Quat::Quat(const Mat3& m)
		{
			if(false)
			{
				// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
				// from Christian. i would say that it is very robust, taking all values into account
				// later i found that its not that robust, given Mat3::rotationX(90.0f) * Mat3::rotationY(180.00052f), it doesn't calculate the correct values

				w = SQRT( max( 0.0f, 1 + m.v00 + m.v11 + m.v22 ) ) * 0.5f;
				x = SQRT( max( 0.0f, 1 + m.v00 - m.v11 - m.v22 ) ) * 0.5f;
				y = SQRT( max( 0.0f, 1 - m.v00 + m.v11 - m.v22 ) ) * 0.5f;
				z = SQRT( max( 0.0f, 1 - m.v00 - m.v11 + m.v22 ) ) * 0.5f;
				x = COPYSIGN( x, m.v12 - m.v21 );
				y = COPYSIGN( y, m.v20 - m.v02 );
				z = COPYSIGN( z, m.v01 - m.v10 );
			}
			else if(true)
			{
				// when using F32vec4, 4-component initializer takes components in reverse order, ie 3 2 1 0
				F32vec4 q;
				REAL t;

				REAL tr = m.v00 + m.v11 + m.v22;
				if(tr > EPSILON)
				{
					t = tr + 1.0f;
					q = F32vec4(t, m.v01 - m.v10, m.v20 - m.v02, m.v12 - m.v21);
				}
				else
				{
					if(m.v00 > max(m.v11, m.v22))
					{
						t = m.v00 - m.v11 - m.v22 + 1.0f;
						q = F32vec4(m.v12 - m.v21, m.v02 + m.v20, m.v01 + m.v10, t);
					}
					else if(m.v11 > m.v22)
					{
						t = m.v11 - m.v22 - m.v00 + 1.0f;
						q = F32vec4(m.v20 - m.v02, m.v12 + m.v21, t, m.v10 + m.v01);
					}
					else
					{
						t = m.v22 - m.v00 - m.v11 + 1.0f;
						q = F32vec4(m.v01 - m.v10, t, m.v21 + m.v12, m.v20 + m.v02);
					}
				}
				F32vec4 s = rsqrt_nr_x2(F32vec4(t)) * F32vec4(0.5f);						// only rsqrt_nr will give us enough precision, while rsqrt and and rcp(sqrt()) suck very very much, but to make it more accurate and thus more stable, do 2 iterations
				q *= s;

				// normalize
				q *= rsqrt_nr_x2(F32vec4(add_horizontal(q * q)));

				x = q[0];
				y = q[1];
				z = q[2];
				w = q[3];
			}
			else
			{
				REAL q[4];
				REAL tr, s, qt[4];
				int i, j, k;

				int next[3] = {1, 2, 0};

				tr = m[0][0] + m[1][1] + m[2][2];

				if(tr > EPSILON)
				{
					s = SQRT(tr + 1.0f);
					q[3] = s / 2.0f;
					s = 0.5f / s;
					q[0] = (m[1][2] - m[2][1]) * s;
					q[1] = (m[2][0] - m[0][2]) * s;
					q[2] = (m[0][1] - m[1][0]) * s;
				}
				else
				{		
					i = 0;
					if(m[1][1] > m[0][0]) i = 1;
					if(m[2][2] > m[i][i]) i = 2;
					j = next[i];
					k = next[j];

					s = SQRT((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

					qt[i] = s * 0.5f;

					if(s != 0.0f) s = 0.5f / s;

					qt[3] = (m[j][k] - m[k][j]) * s;
					qt[j] = (m[i][j] + m[j][i]) * s;
					qt[k] = (m[i][k] + m[k][i]) * s;

					q[0] = qt[0];
					q[1] = qt[1];
					q[2] = qt[2];
					q[3] = qt[3];
				}
				v[0] = (REAL)q[0]; v[1] = (REAL)q[1]; v[2] = (REAL)q[2]; v[3] = (REAL)q[3];
				// accumulating round-off errors make the quaternion unstable, so normalize it if necessary (it will not be handled here, do it outside)
				//normalize();
			}
		}
		Quat::Quat(const Mat4& m)
		{
			if(false)
			{
				// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
				// from Christian. i would say that it is very robust, taking all values into account
				// later i found that its not that robust, given Mat3::rotationX(90.0f) * Mat3::rotationY(180.00052f), it doesn't calculate the correct values

				w = SQRT( max( 0.0f, 1 + m.v00 + m.v11 + m.v22 ) ) * 0.5f;
				x = SQRT( max( 0.0f, 1 + m.v00 - m.v11 - m.v22 ) ) * 0.5f;
				y = SQRT( max( 0.0f, 1 - m.v00 + m.v11 - m.v22 ) ) * 0.5f;
				z = SQRT( max( 0.0f, 1 - m.v00 - m.v11 + m.v22 ) ) * 0.5f;
				x = COPYSIGN( x, m.v12 - m.v21 );
				y = COPYSIGN( y, m.v20 - m.v02 );
				z = COPYSIGN( z, m.v01 - m.v10 );
			}
			else if(true)
			{
				// when using F32vec4, 4-component initializer takes components in reverse order, ie 3 2 1 0
				F32vec4 q;
				REAL t;

				REAL tr = m.v00 + m.v11 + m.v22;
				if(tr > EPSILON)
				{
					t = tr + 1.0f;
					q = F32vec4(t, m.v01 - m.v10, m.v20 - m.v02, m.v12 - m.v21);
				}
				else
				{
					if(m.v00 > max(m.v11, m.v22))
					{
						t = m.v00 - m.v11 - m.v22 + 1.0f;
						q = F32vec4(m.v12 - m.v21, m.v02 + m.v20, m.v01 + m.v10, t);
					}
					else if(m.v11 > m.v22)
					{
						t = m.v11 - m.v22 - m.v00 + 1.0f;
						q = F32vec4(m.v20 - m.v02, m.v12 + m.v21, t, m.v10 + m.v01);
					}
					else
					{
						t = m.v22 - m.v00 - m.v11 + 1.0f;
						q = F32vec4(m.v01 - m.v10, t, m.v21 + m.v12, m.v20 + m.v02);
					}
				}
				F32vec4 s = rsqrt_nr_x2(F32vec4(t)) * F32vec4(0.5f);						// only rsqrt_nr will give us enough precision, while rsqrt and and rcp(sqrt()) suck very very much, but to make it more accurate and thus more stable, do 2 iterations
				q *= s;

				// normalize
				q *= rsqrt_nr_x2(F32vec4(add_horizontal(q * q)));

				x = q[0];
				y = q[1];
				z = q[2];
				w = q[3];
			}
			else
			{
				REAL q[4];
				REAL tr, s, qt[4];
				int i, j, k;

				int next[3] = {1, 2, 0};

				tr = m[0][0] + m[1][1] + m[2][2];

				if(tr > EPSILON)
				{
					s = SQRT(tr + 1.0f);
					q[3] = s / 2.0f;
					s = 0.5f / s;
					q[0] = (m[1][2] - m[2][1]) * s;
					q[1] = (m[2][0] - m[0][2]) * s;
					q[2] = (m[0][1] - m[1][0]) * s;
				}
				else
				{		
					i = 0;
					if(m[1][1] > m[0][0]) i = 1;
					if(m[2][2] > m[i][i]) i = 2;
					j = next[i];
					k = next[j];

					s = SQRT((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

					qt[i] = s * 0.5f;

					if(s != 0.0f) s = 0.5f / s;

					qt[3] = (m[j][k] - m[k][j]) * s;
					qt[j] = (m[i][j] + m[j][i]) * s;
					qt[k] = (m[i][k] + m[k][i]) * s;

					q[0] = qt[0];
					q[1] = qt[1];
					q[2] = qt[2];
					q[3] = qt[3];
				}
				v[0] = (REAL)q[0]; v[1] = (REAL)q[1]; v[2] = (REAL)q[2]; v[3] = (REAL)q[3];
				// accumulating round-off errors make the quaternion unstable, so normalize it if necessary (it will not be handled here, do it outside)
				//normalize();
			}
		}
		Quat& Quat::operator=(const Quat& src)
		{
			memcpy(v, src.v, sizeof(v));
			return (*this);
		}
		REAL& Quat::operator[](int i)
		{
			return v[i];
		}
		REAL Quat::operator[](int i) const
		{
			return v[i];
		}
		Quat Quat::operator*(const Quat& right) const
		{
			Vec3 v0(x, y, z), v1(right.x, right.y, right.z);
			Vec3 v = w * v1 + right.w * v0 + (v0 ^ v1);
			return Quat(v.x, v.y, v.z, w * right.w - v0 * v1);
		}
		Quat& Quat::operator*=(const Quat& right)
		{
			return (*this) = (*this) * right;
		}
		Quat Quat::operator*(REAL scalar) const
		{
			return Quat(x * scalar, y * scalar, z * scalar, w * scalar);
		}
		Quat Quat::operator+(const Quat& right) const
		{
			return Quat(x + right.x, y + right.y, z + right.z, w + right.w);
		}
		Quat& Quat::operator+=(const Quat& right)
		{
			return (*this) = (*this) + right;
		}
		Quat Quat::operator-(const Quat& right) const
		{
			return Quat(x - right.x, y - right.y, z - right.z, w - right.w);
		}
		Quat& Quat::operator-=(const Quat& right)
		{
			return (*this) = (*this) - right;
		}
		Quat Quat::operator-() const
		{
			return Quat(-x, -y, -z, -w);
		}
		bool Quat::operator==(const Quat& right) const
		{
			return x == right.x && y == right.y && z == right.z && w == right.w;
		}
		bool Quat::operator!=(const Quat& right) const
		{
			return !(operator==(right));
		}
		REAL Quat::dotProduct(const Quat& right) const
		{
			return x * right.x + y * right.y + z * right.z + w * right.w;
		}
		Quat Quat::conjugate() const
		{
			return Quat(-x, -y, -z, w);
		}
		REAL Quat::norm() const
		{
			return x * x + y * y + z * z + w * w;
		}
		Quat Quat::normal() const
		{
			REAL inv_norm = 1.0f / norm();
			return Quat(x * inv_norm, y * inv_norm, z * inv_norm, w * inv_norm);
		}
		Quat& Quat::normalize()
		{
			return (*this) = normal();
		}
		Mat3 Quat::asMat3() const
		{
			Mat3 m;
			REAL xx2 = 2 * x * x, yy2 = 2 * y * y, zz2 = 2 * z * z;
			REAL xy2 = 2 * x * y, xz2 = 2 * x * z, wx2 = 2 * w * x;
			REAL yz2 = 2 * y * z, wy2 = 2 * w * y;
			REAL wz2 = 2 * w * z;

			m[0][0] = 1 - yy2 - zz2;
			m[0][1] = xy2 + wz2;
			m[0][2] = xz2 - wy2;
			m[1][0] = xy2 - wz2;
			m[1][1] = 1 - xx2 - zz2;
			m[1][2] = yz2 + wx2;
			m[2][0] = xz2 + wy2;
			m[2][1] = yz2 - wx2;
			m[2][2] = 1 - xx2 - yy2;
			return m;
		}
		Mat4 Quat::asMat4() const
		{
			Mat4 m;
			REAL xx2 = 2 * x * x, yy2 = 2 * y * y, zz2 = 2 * z * z;
			REAL xy2 = 2 * x * y, xz2 = 2 * x * z, wx2 = 2 * w * x;
			REAL yz2 = 2 * y * z, wy2 = 2 * w * y;
			REAL wz2 = 2 * w * z;

			m[0][0] = 1 - yy2 - zz2;
			m[0][1] = xy2 + wz2;
			m[0][2] = xz2 - wy2;
			m[1][0] = xy2 - wz2;
			m[1][1] = 1 - xx2 - zz2;
			m[1][2] = yz2 + wx2;
			m[2][0] = xz2 + wy2;
			m[2][1] = yz2 - wx2;
			m[2][2] = 1 - xx2 - yy2;
			m[3][3] = (REAL)1;
			return m;
		}
		void Quat::toAngleAxis(Vec3& axis, REAL& angle) const
		{
			float s, t;
			s = SIN(t = ACOS(v[3]));
			if(s == 0.0f)
			{
				axis[0] = 0.0f;
				axis[1] = 0.0f;
				axis[2] = 1.0f;
				angle = 0.0f;
			}
			else
			{
				axis[0] = v[0] / s;
				axis[1] = v[1] / s;
				axis[2] = v[2] / s;
				angle = 2 * t;
			}
		}
		Quat Quat::slerp(const Quat& q1, REAL t) const
		{
			REAL dot = dotProduct(q1);
			Quat a, b;
			a = *this;
			if(dot < (REAL)0)
				b = -q1, dot = -dot;
			else
				b = q1;
			REAL s0, s1;
			if((REAL)1 - dot > EPSILON)
			{
				REAL theta = ACOS(dot);
				REAL s = SIN(theta);
				s0 = SIN(((REAL)1 - t) * theta) / s;
				s1 = SIN(t * theta) / s;
			}
			else
			{
				s0 = (REAL)1 - t;
				s1 = t;
			}
			return s0 * a + s1 * b;
		}
		Quat operator*(REAL scalar, const Quat& right)
		{
			return right * scalar;
		}
		bool Quat::valid() const
		{
			return FINITE(v[0]) & FINITE(v[1]) & FINITE(v[2]) & FINITE(v[3]);
		}
	};
};