
#pragma once

#include "predecl.h"
#include "Vector3.h"

namespace z3D
{
	namespace Core
	{
		class Line
		{
		private:
			Vec3 p[2];
		public:
			Line(const Vec3& p0,const Vec3& p1);
			Line(const Vec3* const p);
		public:
			const Vec3&		operator[](int i) const;
		public:
			REAL		length() const;
		public:
			bool		isParallel(const Line& l,REAL biasAngle) const; //
		};
	};
};

