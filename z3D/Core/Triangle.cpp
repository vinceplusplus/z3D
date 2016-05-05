
#include "Triangle.h"
#include "Plane.h"
#include "Constants.h"

namespace z3D
{
	namespace Core
	{
		Triangle::Triangle(const Vec3& p0,const Vec3& p1,const Vec3& p2)
		{
			p[0]=p0;p[1]=p1;p[2]=p2;
		}
		Triangle::Triangle(const Vec3* const p)
		{
			this->p[0]=p[0];this->p[1]=p[1];this->p[2]=p[2];
		}
		Triangle::Triangle(){}

		const Vec3& Triangle::operator[](int i) const
		{
			return p[i];
		}

		const Plane Triangle::plane() const
		{
			return Plane(p[0],normal());
		}
		const Vec3 Triangle::normal() const
		{
			return ((p[2]-p[1])^(p[0]-p[1])).normal();
		}
		const REAL Triangle::area() const
		{
			Vec3 c=(p[2]-p[1])^(p[0]-p[1]);
			return (c*c)/2;
		}

		TriangleAux::TriangleAux(const Vec3& p0,const Vec3& p1,const Vec3& p2):Triangle(p0,p1,p2)
		{postConstruct();}
		TriangleAux::TriangleAux(const Vec3* const p):Triangle(p)
		{postConstruct();}
		TriangleAux::TriangleAux(const Triangle& t):Triangle(t[0],t[1],t[2])
		{postConstruct();}
		TriangleAux::TriangleAux():Triangle()
		{}
		void TriangleAux::postConstruct()
		{
			_edge[0]=p[1]-p[0];
			if(_edge[0].length()>EPSILON) _edge[0].normalize();
			_edge[1]=p[2]-p[1];
			if(_edge[1].length()>EPSILON) _edge[1].normalize();
			_edge[2]=p[0]-p[2];
			if(_edge[2].length()>EPSILON) _edge[2].normalize();
			_edge_dir[0]=_edge[0];
			if(_edge_dir[0].length()>EPSILON) _edge_dir[0].normalize();
			_edge_dir[1]=_edge[1];
			if(_edge_dir[1].length()>EPSILON) _edge_dir[1].normalize();
			_edge_dir[2]=_edge[2];
			if(_edge_dir[2].length()>EPSILON) _edge_dir[2].normalize();
			_normal=_edge[0]^_edge[1];
			if(_normal.length()>EPSILON) _normal.normalize();
			_edge_normal[0]=_edge[0]^_normal;
			if(_edge_normal[0].length()>EPSILON) _edge_normal[0].normalize();
			_edge_normal[1]=_edge[1]^_normal;
			if(_edge_normal[1].length()>EPSILON) _edge_normal[1].normalize();
			_edge_normal[2]=_edge[2]^_normal;
			if(_edge_normal[2].length()>EPSILON) _edge_normal[2].normalize();
			_plane=Plane(p[0],_normal);
		}
		const Vec3& TriangleAux::operator[](int i) const
		{
			return p[i];
		}
		const Vec3&		TriangleAux::normal() const {return _normal;}
		const Vec3&		TriangleAux::edge(int i) const {return _edge[i];}
		const Vec3&		TriangleAux::edge_dir(int i) const {return _edge_dir[i];}
		const Vec3&		TriangleAux::edge_normal(int i) const {return _edge_normal[i];}
		const Plane&		TriangleAux::plane() const {return _plane;}
		const REAL		TriangleAux::area() const
		{
			Vec3 c=(p[2]-p[1])^(p[0]-p[1]);
			return (c*c)/2;
		}
	};
};