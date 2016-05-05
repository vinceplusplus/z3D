
#include "Line.h"
#include "MathFunc.h"

namespace z3D
{
	namespace Core
	{
		Line::Line(const Vec3& p0,const Vec3& p1)
		{
			p[0]=p0;
			p[1]=p1;
		}
		Line::Line(const Vec3* const p)
		{
			this->p[0]=p[0];
			this->p[1]=p[1];
		}
		const Vec3& Line::operator[](int i) const
		{
			return p[i];
		}
		REAL Line::length() const
		{
			return (p[1]-p[0]).length();
		}
		bool Line::isParallel(const Line& l,REAL biasAngle) const
		{
			REAL a=ACOS(FABS((p[0]-p[1]).normal()*(l[0]-p[1]).normal()));
			if(a<=biasAngle)
				return true;
			return false;
		}
	};
};