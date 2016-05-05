
#include "Distance.h"
#include "Vector3.h"
#include "Line.h"
#include "AABB.h"
#include "Matrix3.h"
#include "OBB.h"
#include "MathFunc.h"
#include "Triangle.h"

namespace z3D
{
	namespace Core
	{
		REAL Distance::PointLine(const Vec3& p, const Line& l)
		{
			Vec3 d = l[1] - l[0];
			REAL t = ((p - l[0]) * d) / (d * d);
			return (p - (l[0] + t * d)).length();
		}
		REAL Distance::PointLineSeg(const Vec3& p, const Line& l)
		{
			Vec3 d = l[1] - l[0];
			REAL t = ((p - l[0]) * d) / (d * d);
			t = min(max(t, (REAL)0), (REAL)1);
			return (p - (l[0] + t * d)).length();
		}
		REAL Distance::PointAABB(const Vec3& p, const AABB& aabb)
		{
			int i;
			REAL dist_sq = 0;
			const Vec3& minimum = aabb.minimum();
			const Vec3& maximum = aabb.maximum();
			REAL tmp;
			for(i = 0; i < 3; ++i)
				if((tmp = p[i] - minimum[i]) < 0 || (tmp = p[i] - maximum[i]) > 0)		// is it crazy !?
					dist_sq += tmp * tmp;
			return SQRT(dist_sq);
		}
		REAL Distance::PointOBB(const Vec3& p, const OBB& obb)
		{
			return PointAABB((p - obb.center()) * Mat3(obb.axis(0), obb.axis(1), obb.axis(2), false), AABB(-obb.extent(), obb.extent()));		// its crazy too, hmm ...
		}
		REAL Distance::PointTriangle(const Vec3& p, const Triangle& tri)
		{
			REAL t;
			Line l(p, p + tri.normal());
			Vec3 w;
			Mat3 m(tri[1] - tri[0], tri[2] - tri[0], l[1] - l[0]);
			bool invertible = false;
			Mat3 inv = m.inverse(&invertible);
			if(invertible)
			{
				w = (l[0] - tri[0]) * inv;
				if(w[0] >= 0 && w[0] <= 1 && w[1] >= 0 && w[1] <= 1 && w[0] + w[1] <= 1) // test u, v range
				{
					t = -w[2];
					return FABS(t);
				}
			}
			return min(PointLine(p, Line(tri[0], tri[1])), min(PointLine(p, Line(tri[1], tri[2])), PointLine(p, Line(tri[2], tri[0]))));
		}
		REAL Distance::PointTriangle(const Vec3& p, const TriangleAux& tri)
		{
			REAL t;
			Line l(p, p + tri.normal());
			Vec3 w;
			Mat3 m(tri[1] - tri[0], tri[2] - tri[0], l[1] - l[0]);
			bool invertible = false;
			Mat3 inv = m.inverse(&invertible);
			if(invertible)
			{
				w = (l[0] - tri[0]) * inv;
				if(w[0] >= 0 && w[0] <= 1 && w[1] >= 0 && w[1] <= 1 && w[0] + w[1] <= 1) // test u, v range
				{
					t = -w[2];
					return FABS(t);
				}
			}
			return min(PointLine(p, Line(tri[0], tri[1])), min(PointLine(p, Line(tri[1], tri[2])), PointLine(p, Line(tri[2], tri[0]))));
		}
	};
};
