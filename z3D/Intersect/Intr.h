
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Intersect
	{
		class Intr
		{
		public:
			static bool RayPlane(const Ray& r,const Plane& p,float& t);
			static bool LinePlane(const Line& l,const Plane& p,float& t);
			static bool LineSegPlane(const Line& l,const Plane& p,float& t);
			static bool RayTriangle(const Ray& r,const Triangle& tri,float& t);
			static bool LineTriangle(const Line& l,const Triangle& tri,float& t);
			static bool LineSegTriangle(const Line& l,const Triangle& tri,float& t);
			static bool RayTriangle(const Ray& r,const TriangleAux& tri,float& t);
			static bool LineTriangle(const Line& l, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t);
			static bool LineTriangle(const Vec3& p, const Vec3& d,const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t);
			static bool RayTriangle(const Ray& r, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t);
			static bool LineTriangle(const Line& l,const TriangleAux& tri,float& t);
			static bool LineSegTriangle(const Line& l,const TriangleAux& tri,float& t);
			static bool RaySphere(const Ray& r,const Sphere& s,float& t0,float& t1,int& roots);
			static bool LineSphere(const Line& l,const Sphere& s,float& t0,float& t1,int& roots);
			static bool LineSegSphere(const Line& l,const Sphere& s,float& t0,float& t1,int& roots);
			static bool AABBAABB(const AABB& aabb0,const AABB& aabb1,AABB& intr);
			static bool PlanePlane(const Plane& p0,const Plane& p1,Line& l);
			static bool SphereTriangle(const Sphere& s,const Triangle& tri,Vec3& collinormal,Vec3& collipoint);
			static bool AABBPlane(const AABB& aabb,const Plane& plane,REAL& depth);
			static bool OBBPlane(const OBB& obb,const Plane& plane,REAL& depth);
			static bool OBBPlane2Side(const OBB& obb,const Plane& plane,Vec3& normal,REAL& depth);

			static bool LineCone(const Vec3& p, const Vec3& d, const Vec3& v, const Vec3& a, REAL angle, REAL& t0, REAL& t1);					// infinite cone, if return true, t1 may be infinity meaning that the root is not applicable
			static bool RayConeFull(const Vec3& p, const Vec3& d, const Vec3& v, const Vec3& a, REAL angle, REAL h, REAL& t);					// only care the first time, will care the circle of the cone too, v - the tip of the cone, a - the direction from v the cone to grow

			static bool RayCylinderFull(const Vec3& p, const Vec3& d, const Vec3& c, const Vec3& w, REAL r, REAL h, REAL& t);					// only care the first time, c - center of the cylinder, w - unit-length axis direction, will care the flat caps of the cylinder too
		public:
			// obsolete
			static bool OBBTriangle(const OBB& obb,const TriangleAux& tri,Vec3& normal,REAL& depth);
		};
	};
};

