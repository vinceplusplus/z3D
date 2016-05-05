
#pragma once

#include "predecl.h"
#include "Vector3.h"

namespace z3D
{
	namespace Core
	{
		// 4x4 matrix
		// row major
		// vectors as row vectors (matrices), similar to direct3d convention, in contrary to academic's and opengl's
		// transforming a vector is written as vector-matrix multiplication, therefore, the transformation applying order is said as from left to right in contrary to from right to left from academy and opengl
		// the reason to choose such convention is the intuitiveness, it looks more intuitive and so we can avoid unnecessary hair loss
		// but still at times, i derive math using the academic convention, then to really implement them i transpose and reverse the stuff

		class Mat4
		{
		public:
			union
			{
				REAL v[4][4];
				struct
				{
					REAL v00, v01, v02, v03;
					REAL v10, v11, v12, v13;
					REAL v20, v21, v22, v23;
					REAL v30, v31, v32, v33;
				};
				struct
				{
					REAL a11, a12, a13, a14;
					REAL a21, a22, a23, a24;
					REAL a31, a32, a33, a34;
					REAL a41, a42, a43, a44;
				};
			};
		public:
			static const Mat4 identity;
		public:
			Mat4();
			Mat4(const REAL v[4][4]);
			Mat4(const Mat4& src);
		public:
			operator Mat3() const;
		public:
			Mat4&			operator=(const Mat4& other);
			REAL*			operator[](ptrdiff_t row);
			const REAL*		operator[](ptrdiff_t row) const;
			Mat4&			setIdentity();
			Mat4			transpose() const;
			Mat4&			operator+=(const Mat4& right);
			Mat4			operator+(const Mat4& right) const;
			Mat4&			operator-=(const Mat4& right);
			Mat4			operator-(const Mat4& right) const;
			Mat4&			operator*=(const Mat4& right);
			Mat4			operator*(const Mat4& right) const;
			Mat4&			operator*=(REAL scalar);
			Mat4			operator*(REAL scalar) const;
			Mat4			inverse(bool* invertible = 0) const;
			REAL			det4() const;
			static Mat4		identityMatrix();
			static Mat4		translation(const Vec3& trans);
			static Mat4		translation(REAL tx, REAL ty, REAL tz);
			static Mat4		rotation(const Vec3& axis, REAL angle);
			static Mat4		scaling(REAL sx, REAL sy, REAL sz);
			static Mat4		scaling(const Vec3& scale);
			static Mat4		lookingAt(const Vec3& eye, const Vec3& at, const Vec3& up = Vec3::yAxis);

			//	use negative znear and zfar for right-handed coorindate system

			static Mat4		perspective(REAL left, REAL bottom, REAL right, REAL top, REAL znear, REAL zfar, REAL znear_map, REAL zfar_map, REAL sign);			// for right-handed ogl, e.g. perspective(...,-0.1f,-1000.0f,1.0f,-1.0f,1.0f), for right-handed d3d, e.g. perspective(...,-0.1f,-1000.0f,0.0f,1.0f,-1.0f), about the sign, because d3d needs transformed vertex's w to be positive(some kind of optimizations, if all w's of triangle vertices are negative, the whole triangle can be discarded, so all z/w need not to be calculated... )
			static Mat4		perspectiveFOV(REAL fovy, REAL aspect, REAL znear, REAL zfar, REAL znear_map, REAL zfar_map, REAL sign);
			static Mat4		ortho(REAL left, REAL bottom, REAL right, REAL top, REAL znear, REAL zfar, REAL znear_map, REAL zfar_map);

			static Mat4		adjustForObliqueNearClippingPlane(const Mat4& proj, const Plane& p, REAL znear_mapping, REAL zfar_mapping);

			static Mat4		mirror(const Plane& p);
			static Mat4		rotationX(REAL angle);
			static Mat4		rotationY(REAL angle);
			static Mat4		rotationZ(REAL angle);
			static Mat4		closest(const Plane& p);
			Mat4&			translate(const Vec3& trans);
			Mat4&			rotate(const Vec3& axis, REAL angle);
			Mat4&			scale(REAL sx, REAL sy, REAL sz);
			Mat4&			scale(const Vec3& scale);
			Mat4&			lookAt(const Vec3& eye, const Vec3& at, const Vec3& up = Vec3::yAxis);
			Mat4&			setTranslation(const Vec3& trans);
			Mat4&			setRotation(const Vec3& axis, REAL angle);
			Mat4&			setScaling(REAL sx, REAL sy, REAL sz);
			Mat4&			setScaling(const Vec3& scale);
			Mat4&			setLookingAt(const Vec3& eye, const Vec3& at, const Vec3& up = Vec3::yAxis);
			void			factorSRT(Vec3& scale, Mat3& rotation, Vec3& translation) const;
			void			factorSRT(Vec3& scale, Quat& rotation, Vec3& translation) const;
			void			decompose(Quat& so, Vec3& s, Quat& r, Vec3& t) const;								// not tested
			bool			testIdentity(REAL threshold = 0.0001f) const;
		public:
			friend Mat4		operator*(REAL scalar, const Mat4& right);
		};
	};
};

