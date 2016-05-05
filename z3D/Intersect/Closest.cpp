
#include "Closest.h"

namespace z3D
{
	namespace Intersect
	{
		void				Closest::PointTriangle(const Vec3& p, const Triangle& tri, Vec3& output)
		{
			Vec3 n = tri.normal();

			Mat3 m(n, tri[1] - tri[0], tri[2] - tri[0]);

			const Vec3& e0 = *(const Vec3*)m[1];
			const Vec3& e1 = *(const Vec3*)m[2];

			Vec3 w = (p - tri[0]) * m.inverse();
			if(w.x + w.y > 1)
			{
				if(w.x < 0)
				{
					output = tri[2];
					return;
				}
				if(w.y < 0)
				{
					output = tri[1];
					return;
				}
				REAL s = (1 + w.x - w.y) * 0.5f;
				output = tri[0] + s * e0 + (1 - s) * e1;
			}
			else
			{
				if(w.x < 0)
				{
					if(w.y < 0)
					{
						output = tri[0];
						return;
					}
					output = tri[0] + e1 * min(w.y, 1.0f);
					return;
				}
				if(w.y < 0)
				{
					output = tri[0] + e0 * min(w.x, 1.0f);
					return;
				}
				output = tri[0] + w.x * e0 + w.y * e1;
			}
		}
		void				Closest::PointAABB(const Vec3& p, const AABB& aabb, Vec3& output)
		{
			const Vec3& minimum = aabb.minimum();
			const Vec3& maximum = aabb.maximum();
			for(int i = 0; i < 3; ++i)
				(p[i] < minimum[i] && (output[i] = minimum[i], true)) || (p[i] > maximum[i] && (output[i] = maximum[i], true)) || (output[i] = p[i], true);		// sounds crazy right? learn from assert()
		}
	};
};

