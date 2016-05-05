
#pragma once

#include "predecl.h"
#include "Vector3.h"
#include "Plane.h"

namespace z3D
{
	namespace Core
	{
		class Triangle
		{
		protected:
			Vec3 p[3];
		public:
			Triangle(const Vec3& p0,const Vec3& p1,const Vec3& p2);
			Triangle(const Vec3* const p);
			Triangle();
		public:
			const Vec3&		operator[](int i) const;
		public:
			const Plane		plane() const;
			const Vec3		normal() const;
		public:
			const REAL		area() const;
		};

		class TriangleAux:protected Triangle
		{
		protected:
			Vec3 _normal;
			Vec3 _edge[3];
			Vec3 _edge_dir[3];
			Vec3 _edge_normal[3];
			Plane _plane;
		public:
			TriangleAux(const Vec3& p0,const Vec3& p1,const Vec3& p2);
			TriangleAux(const Vec3* const p);
			TriangleAux(const Triangle& t);
			TriangleAux();
		private:
			void postConstruct();
		public:
			const Vec3&		operator[](int i) const;
		public:
			const Vec3&		normal() const;
			const Vec3&		edge(int i) const;
			const Vec3&		edge_dir(int i) const;
			const Vec3&		edge_normal(int i) const;
			const Plane&	plane() const;
		public:
			const REAL		area() const;
		};
	};
};

