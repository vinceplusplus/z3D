
#pragma once

#include "predecl.h"
#include "Intr.h"

namespace z3D
{
	namespace Intersect
	{
		class Test
		{
		public:
			static bool IntervalInterval(REAL min0,REAL max0,REAL min1,REAL max1);
		public:
			static bool PointAABB(const Vec3& v,const AABB& aabb);
			static bool PointSphere(const Vec3& v,const Sphere& s);
			static bool RayPlane(const Ray& r,const Plane& p);
			static bool LinePlane(const Line& l,const Plane& p);
			static bool LineSegPlane(const Line& l,const Plane& p);
			static bool RayTriangle(const Ray& r,const Triangle& tri);
			static bool LineTriangle(const Line& l,const Triangle& tri);
			static bool LineSegTriangle(const Line& l,const Triangle& tri);
			static bool LinearAABB(const Line& l,const AABB& aabb,REAL& t0,REAL& t1);
			static bool RayAABB(const Ray& r,const AABB& aabb);
			static bool LineAABB(const Line& l,const AABB& aabb);
			static bool LineSegAABB(const Line& l,const AABB& aabb);
			static bool LinearOBB(const Line& l,const OBB& obb,REAL& t0,REAL& t1);
			static bool RayOBB(const Ray& r,const OBB& obb,REAL& t);
			static bool LineOBB(const Line& l,const OBB& obb,REAL& t0,REAL& t1);
			static bool LineSegOBB(const Line& l,const OBB& obb,REAL& t0,REAL& t1);
			static bool RaySphere(const Ray& r,const Sphere& s);
			static bool LineSphere(const Line& l,const Sphere& s);
			static bool LineSegSphere(const Line& l,const Sphere& s);
			static bool PlaneAABB(const Plane& p,const AABB& aabb);
			static bool PlaneSphere(const Plane& p,const Sphere& s);
			static bool AABBAABB(const AABB& aabb0,const AABB& aabb1);
			static bool AABBSphere(const AABB& aabb,const Sphere& s);
			static bool TriangleSphere(const Triangle& tri,const Sphere& s);
			static bool SphereSphere(const Sphere& s0,const Sphere& s1);

			static bool				OBBTriangle(const Vec3& obb_center, const Vec3 obb_eaxis[3], const Vec3& v0, const Vec3& v1, const Vec3& v2);
			static bool SphereTriangle(const Vec3& center, REAL radius, const Vec3& p0, const Vec3& p1, const Vec3& p2);
		public:
			// obsolete
			static bool TriangleAABB(const Triangle& tri,const AABB& aabb);
			static bool TriangleAABB2(const Triangle& tri,const AABB& aabb);
			static bool TriangleTriangle(const Triangle& tri0,const Triangle& tri1);
			static bool OBBTriangle(const OBB& obb,const Triangle& tri);
			static bool OBBTriangle(const OBB& obb,const TriangleAux& tri);
			static bool OBBTriangle(const OBB& obb,const TriangleAux& tri,Vec3& axis,REAL& depth);
			static bool OBBOBB(const OBB& obb0,const OBB& obb1);
			static bool AABBOBB(const AABB& aabb,const OBB& obb);
			static bool TriangleSphereSAT(const TriangleAux& tri,const Sphere& s);
		private:
			static bool TriangleSphereSAT_SingleTest(const Vec3& t0,const Vec3& t1,const Vec3& t2,const Vec3& o,REAL radius,const Vec3& axis);
		};
	};
};

