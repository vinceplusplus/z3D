
#pragma once

#include "predecl.h"
#include "Test.h"

namespace z3D
{
	namespace Intersect
	{
		class CD
		{
		public:
			static bool SphereTriangle(const Vec3& so, REAL sr, const Vec3& w, const Vec3& t0, const Vec3& t1, const Vec3& t2, REAL& toi, Vec3& collinormal, Vec3& collipoint);				// ignore edges and vertices, ignore last time contact
			static bool SphereLineSeg(const Vec3& so, REAL sr, const Vec3& w, const Vec3& l0, const Vec3& l1, REAL& toi, Vec3& collinormal, Vec3& collipoint);								// ignore last time contact
			static bool SphereSphere(const Vec3& so0, REAL sr0, const Vec3& w0, const Vec3& so1, REAL sr1, REAL& toi, Vec3& collinormal, Vec3& collipoint);									// ignore last time contact
			static bool SphereSphere(const Vec3& so0, REAL sr0, const Vec3& w0, const Vec3& so1, REAL sr1, const Vec3& w1, REAL& toi, Vec3& collinormal, Vec3& collipoint);					// ignore last time contact
			static bool SphereTriangleFull(const Vec3& so, REAL sr, const Vec3& w, const Vec3& t0, const Vec3& t1, const Vec3& t2, REAL& toi, Vec3& collinormal, Vec3& collipoint);			// include edges and vertices, ignore last time contact

			static bool SphereLineSeg(const Vec3& so, REAL sr, const Vec3& w, const Vec3& p, const Vec3& d, REAL& t0, REAL& t1, Vec3* a0, Vec3* a1);
			static bool SphereSphere(const Vec3& so0, REAL sr0, const Vec3& w0, const Vec3& so1, REAL sr1, REAL& t0, REAL& t1, Vec3* a0, Vec3* a1);
			static bool SphereTriangleFull2(const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis);

			static bool				OBBTriangle(const Vec3& obb_center, const Vec3 obb_eaxis[3], const Vec3& obb_velocity, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis);
			template<bool ACCURATE>
			static bool				SphereTriangleSAT(const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis);
		public:
			// obsolete
			static bool AABBTriangle(const AABB& aabb,const Vec3& w,const Triangle& tri,REAL& t);
			static bool AABBTriangle(const AABB& aabb,const Vec3& w,const TriangleAux& tri,REAL& t);
			static bool OBBTriangle(const OBB& obb,const Vec3& w,const Triangle& tri,REAL& t);
			static bool OBBTriangle(const OBB& obb,const Vec3& w,const TriangleAux& tri,REAL& t0,REAL& t1);
			static bool OBBTriangle(const OBB& obb,const Vec3& w,const TriangleAux& tri,REAL& t);
			static bool OBBTriangle(const OBB& obb,const Vec3& w,const TriangleAux& tri,REAL& t,Vec3& collinormal,Vec3& collipoint);
			static bool OBBOBB(const OBB& obb0,const Vec3& w,const OBB& obb1,REAL& t);
			static bool OBBOBB(const OBB& obb0,const Vec3& w,const OBB& obb1,REAL& t,Vec3& collinormal,Vec3& collipoint);
		};
	};
};

