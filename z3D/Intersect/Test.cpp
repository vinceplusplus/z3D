
#include "Test.h"

#include <math.h>
#include "jgt-tribox.inl"

#include "SAT.h"

namespace z3D
{
	namespace Intersect
	{
		bool Test::IntervalInterval(REAL min0,REAL max0,REAL min1,REAL max1)
		{
			if(min0>max1||min1>max0)
				return false;
			return true;
		}
		bool Test::PointAABB(const Vec3& v,const AABB& aabb)
		{
			int i;
			for(i=0;i<3;i++)
				if(v[i]<aabb.minimum()[i]||v[i]>aabb.maximum()[i])
					return false;
			return true;
		}
		bool Test::PointSphere(const Vec3& v,const Sphere& s)
		{
			if((v-s.origin()).lengthsq()>s.radius()*s.radius())
				return false;
			return true;
		}
		bool Test::RayPlane(const Ray& r,const Plane& p)
		{
			REAL t;
			return Intr::RayPlane(r,p,t);
		}
		bool Test::LinePlane(const Line& l,const Plane& p)
		{
			REAL t;
			return Intr::LinePlane(l,p,t);
		}
		bool Test::LineSegPlane(const Line& l,const Plane& p)
		{
			REAL t;
			return Intr::LineSegPlane(l,p,t);
		}
		bool Test::RayTriangle(const Ray& r,const Triangle& tri)
		{
			REAL t;
			return Intr::RayTriangle(r,tri,t);
		}
		bool Test::LineTriangle(const Line& l,const Triangle& tri)
		{
			REAL t;
			return Intr::LineTriangle(l,tri,t);
		}
		bool Test::LineSegTriangle(const Line& l,const Triangle& tri)
		{
			REAL t;
			return Intr::LineSegTriangle(l,tri,t);
		}
		bool Test::LinearAABB(const Line& l,const AABB& aabb,REAL& t0,REAL& t1)
		{
			t0=-INFINITY;
			t1=INFINITY;
			Vec3 min0=aabb.minimum(),max0=aabb.maximum();
			Vec3 d=l[1]-l[0];
			const Vec3& p=l[0];
			int i;
			for(i=0;i<3;i++)
			{
				if(FABS(d[i])<EPSILON)
				{
					if(p[i]>=min0[i]&&p[i]<=max0[i])
						continue;
					else
						return false;
				}
				else
				{
					REAL tmp0,tmp1,tmp2;
					tmp0=(min0[i]-p[i])/d[i];
					tmp1=(max0[i]-p[i])/d[i];
					if(tmp0>tmp1){tmp2=tmp0;tmp0=tmp1;tmp1=tmp2;}
					t0=tmp0>t0?tmp0:t0;
					t1=tmp1<t1?tmp1:t1;
				}
				if(t1<t0)
					return false;
			}
			return true;
		}
		bool Test::RayAABB(const Ray& r,const AABB& aabb)
		{
			REAL t0,t1;
			Line l(r.origin(),r.origin()+r.direction());
			if(!LinearAABB(l,aabb,t0,t1))
				return false;
			if(t1<0.0f)
				return false;
			return true;
		}
		bool Test::LineAABB(const Line& l,const AABB& aabb)
		{
			REAL t0,t1;
			if(!LinearAABB(l,aabb,t0,t1))
				return false;
			return true;
		}
		bool Test::LineSegAABB(const Line& l,const AABB& aabb)
		{
			REAL t0,t1;
			if(!LinearAABB(l,aabb,t0,t1))
				return false;
			if(IntervalInterval(0,1,t0,t1))
				return true;
			return false;
		}
		bool Test::LinearOBB(const Line& l,const OBB& obb,REAL& t0,REAL& t1)
		{
			t0=-INFINITY;
			t1=INFINITY;

			int i;
			for(i=0;i<3;++i)
			{
				REAL tmp0,tmp1,tmp2;

				tmp0=INFINITY;
				tmp1=-INFINITY;
				Plane p0(obb.center()-obb.axis(i)*obb.extent()[i],obb.axis(i));
				if(!Intr::LinePlane(l,p0,tmp0))
					if(p0.sub(l[0])<0&&p0.sub(l[1])<0)
						return false;
				Plane p1(obb.center()+obb.axis(i)*obb.extent()[i],-obb.axis(i));
				if(!Intr::LinePlane(l,p1,tmp1))
					if(p1.sub(l[0])<0&&p1.sub(l[1])<0)
						return false;
				if(tmp0>tmp1){tmp2=tmp0;tmp0=tmp1;tmp1=tmp2;}
				t0=tmp0>t0?tmp0:t0;
				t1=tmp1<t1?tmp1:t1;

				if(t0>t1)
					return false;
			}
			return true;
		}
		bool Test::RayOBB(const Ray& r,const OBB& obb,REAL& t)
		{
			REAL t0,t1;
			Line l(r.origin(),r.origin()+r.direction());
			if(!LinearOBB(l,obb,t0,t1))
				return false;
			t0=max(0.0f,t0);
			if(t0>t1)
				return false;
			t=t1;
			return true;
		}
		bool Test::LineOBB(const Line& l,const OBB& obb,REAL& t0,REAL& t1)
		{
			if(!LinearOBB(l,obb,t0,t1))
				return false;
			return true;
		}
		bool Test::LineSegOBB(const Line& l,const OBB& obb,REAL& t0,REAL& t1)
		{
			if(!LinearOBB(l,obb,t0,t1))
				return false;
			t0=max(0.0f,t0);
			t1=min(1.0f,t1);
			if(t0>t1)
				return false;
			return true;
		}
		bool Test::RaySphere(const Ray& r,const Sphere& s)
		{
			REAL t0,t1;
			int roots;
			return Intr::RaySphere(r,s,t0,t1,roots);
		}
		bool Test::LineSphere(const Line& l,const Sphere& s)
		{
			REAL t0,t1;
			int roots;
			return Intr::LineSphere(l,s,t0,t1,roots);
		}
		bool Test::LineSegSphere(const Line& l,const Sphere& s)
		{
			REAL t0,t1;
			int roots;
			return Intr::LineSegSphere(l,s,t0,t1,roots);
		}
		bool Test::PlaneAABB(const Plane& p,const AABB& aabb)
		{
			Vec3 c=aabb.center(),e=aabb.extent();
			REAL d=FABS(e*p.normal());
			return d>=FABS(c*p.normal()-p.distance());	// need to use GeoMethods
		}
		bool Test::PlaneSphere(const Plane& p,const Sphere& s)
		{
			return FABS(s.origin()*p.normal()-p.distance())<=s.radius();
		}
		REAL BoxInterval(const Vec3& extent,const Vec3& axis)
		{
			return FABS(extent[0]*axis[0])+FABS(extent[1]*axis[1])+FABS(extent[2]*axis[2]);
		}
		REAL BoxInterval(const AABB& aabb,const Vec3& axis)
		{
			return BoxInterval(aabb.extent(),axis);
		}
		bool SeparatingAxisTest(const Vec3& axis,REAL box_interval,const Vec3& origin,const Triangle& tri)
		{
/*			int i=0;
			for(i=0;i<3;i++)
				if(FABS((tri[i]-origin)*axis)<=box_interval)
					return true;
			return false;*/
			REAL int0,int1,int2;
			int0=(tri[0]-origin)*axis;int1=(tri[1]-origin)*axis;int2=(tri[2]-origin)*axis;
			REAL intmin,intmax;
			intmin=min(int0,min(int1,int2));intmax=max(int0,max(int1,int2));
			return Test::IntervalInterval(intmin,intmax,-box_interval,box_interval);
		}
		bool SeparatingAxisTest(const Vec3& unit_axis,const Vec3& origin,const Vec3& extent,const Triangle& tri)
		{
			return SeparatingAxisTest(unit_axis,BoxInterval(extent,unit_axis),origin,tri);
		}
		const Vec3 AABBTRI_AXES[3]={Vec3::xAxis,Vec3::yAxis,Vec3::zAxis};
		bool Test::TriangleAABB(const Triangle& tri,const AABB& aabb)
		{
			Vec3 c=aabb.center();
			Vec3 e=aabb.extent();
			if(!SeparatingAxisTest(Vec3::xAxis,c,e,tri))
				return false;
			if(!SeparatingAxisTest(Vec3::yAxis,c,e,tri))
				return false;
			if(!SeparatingAxisTest(Vec3::zAxis,c,e,tri))
				return false;
			if(!SeparatingAxisTest(tri.normal(),c,e,tri))
				return false;
			Vec3 edge[3]={tri[1]-tri[0],tri[2]-tri[1],tri[0]-tri[2]};
			int i,j;
			for(i=0;i<3;i++)
				for(j=0;j<3;j++)
					if(!SeparatingAxisTest(AABBTRI_AXES[i]^edge[j],c,e,tri))
						return false;
			return true;
		}
		bool Test::TriangleAABB2(const Triangle& tri,const AABB& aabb)
		{
			Vec3 center,extent;
			center=aabb.center();
			extent=aabb.extent();
			float c[3],e[3];
			c[0]=(REAL)center[0];c[1]=(REAL)center[1];c[2]=(REAL)center[2];
			e[0]=(REAL)extent[0];e[1]=(REAL)extent[1];e[2]=(REAL)extent[2];
			Vec3 vertex[3];
			vertex[0]=tri[0];vertex[1]=tri[1];vertex[2]=tri[2];
			float v[3][3];
			v[0][0]=vertex[0][0];v[0][1]=vertex[0][1];v[0][2]=vertex[0][2];
			v[1][0]=vertex[1][0];v[1][1]=vertex[1][1];v[1][2]=vertex[1][2];
			v[2][0]=vertex[2][0];v[2][1]=vertex[2][1];v[2][2]=vertex[2][2];
			return triBoxOverlap(c,e,v)?true:false;
		}
		bool Test::TriangleSphere(const Triangle& tri,const Sphere& s)
		{
			Vec3 o=s.origin();
			REAL interval=s.radius();
			if(!SeparatingAxisTest(tri.normal(),interval,o,tri))
				return false;
			if(!SeparatingAxisTest((((tri[0]-o)^(tri[1]-o))^(tri[0]-tri[1])).normal(),interval,o,tri))
				return false;
			if(!SeparatingAxisTest((((tri[1]-o)^(tri[2]-o))^(tri[1]-tri[2])).normal(),interval,o,tri))
				return false;
			if(!SeparatingAxisTest((((tri[2]-o)^(tri[0]-o))^(tri[2]-tri[0])).normal(),interval,o,tri))
				return false;
			return true;
		}
		bool TriTri_SeparatingAxisTest(const Vec3& axis,const Triangle& tri0,const Triangle& tri1)
		{
			REAL min0,max0,min1,max1,t;
			min0=max0=tri0[0]*axis;
			t=tri0[1]*axis;min0=min(min0,t);max0=max(max0,t);
			t=tri0[2]*axis;min0=min(min0,t);max0=max(max0,t);
			min1=max1=tri1[0]*axis;
			t=tri1[1]*axis;min1=min(min1,t);max1=max(max1,t);
			t=tri1[2]*axis;min1=min(min1,t);max1=max(max1,t);
			return Test::IntervalInterval(min0,max0,min1,max1);
		}
		bool Test::TriangleTriangle(const Triangle& tri0,const Triangle& tri1)
		{
			Vec3 axis;
			axis=(tri0[2]-tri0[1])^(tri0[0]-tri0[1]);
			if(!TriTri_SeparatingAxisTest(axis,tri0,tri1))
				return false;
			axis=(tri1[2]-tri1[1])^(tri1[0]-tri1[1]);
			if(!TriTri_SeparatingAxisTest(axis,tri0,tri1))
				return false;
			int i,j;
			for(i=0;i<3;i++)
				for(j=0;j<3;j++)
				{
					axis=(tri0[i]-tri0[(i+1)%3])^(tri1[j]-tri1[(j+1)%3]);
					if(!TriTri_SeparatingAxisTest(axis,tri0,tri1))
						return false;
				}
			return true;
		}
		bool Test::AABBSphere(const AABB& aabb,const Sphere& s)
		{
			Vec3 d;
			int i;
			Vec3 o=s.origin(),min0=aabb.minimum(),max0=aabb.maximum();
			REAL r=s.radius();
			for(i=0;i<3;i++)
			{
				if(o[i]>max0[i])
					d[i]=o[i]-max0[i];
				else if(o[i]<min0[i])
					d[i]=min0[i]-o[i];
			}
			if(d*d>r*r)
				return false;
			return true;
		}
		bool Test::AABBAABB(const AABB& aabb0,const AABB& aabb1)
		{
			int i;
			REAL min0,max0;
			for(i=0;i<3;i++)
			{
				min0=max(aabb0.minimum()[i],aabb1.minimum()[i]);
				max0=min(aabb0.maximum()[i],aabb1.maximum()[i]);
				if(min0>max0)
					return false;
			}
			return true;
		}
		bool Test::SphereSphere(const Sphere& s0,const Sphere& s1)
		{
			REAL r=s0.radius()+s1.radius();
			if((s0.origin()-s1.origin()).lengthsq()>r*r)
				return false;
			return true;
		}
		bool Test::OBBTriangle(const OBB& obb,const Triangle& tri)
		{
			const Vec3& obb_axis0=obb.axis(0);
			const Vec3& obb_axis1=obb.axis(1);
			const Vec3& obb_axis2=obb.axis(2);
			const Vec3& obb_ext=obb.extent();
			const Vec3& obb_center=obb.center();
			Vec3& obb_eaxis0=obb_axis0*obb_ext[0];
			Vec3& obb_eaxis1=obb_axis1*obb_ext[1];
			Vec3& obb_eaxis2=obb_axis2*obb_ext[2];

			const Vec3& tri_vert0=tri[0];
			const Vec3& tri_vert1=tri[1];
			const Vec3& tri_vert2=tri[2];

			int i;
			Vec3 axis[13],edge[3],*axis_ptr;

			edge[0]=tri_vert1-tri_vert0;
			edge[1]=tri_vert2-tri_vert1;
			edge[2]=tri_vert0-tri_vert2;

			axis_ptr=axis;

			*axis_ptr=(edge[0])^(edge[1]);		++axis_ptr;
			*axis_ptr=obb_axis0;				++axis_ptr;
			*axis_ptr=obb_axis1;				++axis_ptr;
			*axis_ptr=obb_axis2;				++axis_ptr;
			*axis_ptr=obb_axis0^edge[0];		++axis_ptr;
			*axis_ptr=obb_axis0^edge[1];		++axis_ptr;
			*axis_ptr=obb_axis0^edge[2];		++axis_ptr;
			*axis_ptr=obb_axis1^edge[0];		++axis_ptr;
			*axis_ptr=obb_axis1^edge[1];		++axis_ptr;
			*axis_ptr=obb_axis1^edge[2];		++axis_ptr;
			*axis_ptr=obb_axis2^edge[0];		++axis_ptr;
			*axis_ptr=obb_axis2^edge[1];		++axis_ptr;
			*axis_ptr=obb_axis2^edge[2];

			REAL min0,max0,min1,max1;
			axis_ptr=axis;
			for(i=0;i<13;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				REAL o=obb_center*cur_axis;
				REAL e=FABS(obb_eaxis0*cur_axis)+FABS(obb_eaxis1*cur_axis)+FABS(obb_eaxis2*cur_axis);
				min0=o-e;
				max0=o+e;
				min1=tri_vert0*cur_axis;
				max1=min1;
				REAL tmp=tri_vert1*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				tmp=tri_vert2*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				if(!Test::IntervalInterval(min0,max0,min1,max1))
					return false;
			}
			return true;
		}
		bool Test::OBBTriangle(const OBB& obb,const TriangleAux& tri)
		{
			const Vec3& obb_axis0=obb.axis(0);
			const Vec3& obb_axis1=obb.axis(1);
			const Vec3& obb_axis2=obb.axis(2);
			const Vec3& obb_ext=obb.extent();
			const Vec3& obb_center=obb.center();
			Vec3& obb_eaxis0=obb_axis0*obb_ext[0];
			Vec3& obb_eaxis1=obb_axis1*obb_ext[1];
			Vec3& obb_eaxis2=obb_axis2*obb_ext[2];

			const Vec3& tri_vert0=tri[0];
			const Vec3& tri_vert1=tri[1];
			const Vec3& tri_vert2=tri[2];

			int i;
			Vec3 axis[13],*axis_ptr;

			const Vec3& edge0=tri.edge_dir(0);
			const Vec3& edge1=tri.edge_dir(1);
			const Vec3& edge2=tri.edge_dir(2);

			axis_ptr=axis;

			*axis_ptr=tri.normal();				++axis_ptr;
			*axis_ptr=obb_axis0;				++axis_ptr;
			*axis_ptr=obb_axis1;				++axis_ptr;
			*axis_ptr=obb_axis2;				++axis_ptr;
			*axis_ptr=obb_axis0^edge0;			++axis_ptr;
			*axis_ptr=obb_axis0^edge1;			++axis_ptr;
			*axis_ptr=obb_axis0^edge2;			++axis_ptr;
			*axis_ptr=obb_axis1^edge0;			++axis_ptr;
			*axis_ptr=obb_axis1^edge1;			++axis_ptr;
			*axis_ptr=obb_axis1^edge2;			++axis_ptr;
			*axis_ptr=obb_axis2^edge0;			++axis_ptr;
			*axis_ptr=obb_axis2^edge1;			++axis_ptr;
			*axis_ptr=obb_axis2^edge2;

			REAL min0,max0,min1,max1;
			axis_ptr=axis;
			for(i=0;i<13;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				REAL o=obb_center*cur_axis;
				REAL e=FABS(obb_eaxis0*cur_axis)+FABS(obb_eaxis1*cur_axis)+FABS(obb_eaxis2*cur_axis);
				min0=o-e;
				max0=o+e;
				min1=tri_vert0*cur_axis;
				max1=min1;
				REAL tmp=tri_vert1*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				tmp=tri_vert2*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				if(!Test::IntervalInterval(min0,max0,min1,max1))
					return false;
			}
			return true;
		}
		bool Test::OBBTriangle(const OBB& obb,const TriangleAux& tri,Vec3& axis,REAL& depth)
		{
			const Vec3& obb_axis0=obb.axis(0);
			const Vec3& obb_axis1=obb.axis(1);
			const Vec3& obb_axis2=obb.axis(2);
			const Vec3& obb_ext=obb.extent();
			const Vec3& obb_center=obb.center();
			Vec3& obb_eaxis0=obb_axis0*obb_ext[0];
			Vec3& obb_eaxis1=obb_axis1*obb_ext[1];
			Vec3& obb_eaxis2=obb_axis2*obb_ext[2];

			const Vec3& tri_vert0=tri[0];
			const Vec3& tri_vert1=tri[1];
			const Vec3& tri_vert2=tri[2];

			int i;
			Vec3 array_axis[13],*axis_ptr;

			const Vec3& edge0=tri.edge_dir(0);
			const Vec3& edge1=tri.edge_dir(1);
			const Vec3& edge2=tri.edge_dir(2);

			axis_ptr=array_axis;

			*axis_ptr=tri.normal();				++axis_ptr;
			*axis_ptr=obb_axis0;				++axis_ptr;
			*axis_ptr=obb_axis1;				++axis_ptr;
			*axis_ptr=obb_axis2;				++axis_ptr;
			*axis_ptr=obb_axis0^edge0;			++axis_ptr;
			*axis_ptr=obb_axis0^edge1;			++axis_ptr;
			*axis_ptr=obb_axis0^edge2;			++axis_ptr;
			*axis_ptr=obb_axis1^edge0;			++axis_ptr;
			*axis_ptr=obb_axis1^edge1;			++axis_ptr;
			*axis_ptr=obb_axis1^edge2;			++axis_ptr;
			*axis_ptr=obb_axis2^edge0;			++axis_ptr;
			*axis_ptr=obb_axis2^edge1;			++axis_ptr;
			*axis_ptr=obb_axis2^edge2;

			Vec3 min_axis;
			REAL min_depth=INFINITY;

			REAL min0,max0,min1,max1;
			axis_ptr=array_axis;
			for(i=0;i<13;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				if(cur_axis.length()<0.9f)
					continue;
				REAL o=obb_center*cur_axis;
				REAL e=FABS(obb_eaxis0*cur_axis)+FABS(obb_eaxis1*cur_axis)+FABS(obb_eaxis2*cur_axis);
				min0=o-e;
				max0=o+e;
				min1=tri_vert0*cur_axis;
				max1=min1;
				REAL tmp=tri_vert1*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				tmp=tri_vert2*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				if(!Test::IntervalInterval(min0,max0,min1,max1))
					return false;

				REAL mi0,mi1,ma0,ma1;
				mi0=min0;mi1=min1;ma0=max0;ma1=max1;
				//REAL d=(ma0-mi0+ma1-mi1);
				//SORT(mi0,mi1);
				//SORT(ma0,ma1);
				//d-=ma1-mi0;

				REAL d;

				REAL md=INFINITY;
				if(max0>=min1 && (d=max0-min1)<md)
					md=d;
				if(max1>=min0 && (d=max1-min0)<md)
					md=d;

				if(md==max0-min1)
				{
					d=max0-min1;
					if(d<min_depth)
					{
						min_depth=d;
						min_axis=-cur_axis;
					}
				}
				else if(md==max1-min0)
				{
					d=max1-min0;
					if(d<min_depth)
					{
						min_depth=d;
						min_axis=cur_axis;
					}
				}

				/*
				REAL d=min(min(min(FABS(mi0-mi1),FABS(mi0-ma1)),FABS(ma0-mi1)),FABS(ma0-ma1));
				if(d<min_depth)
				{
					min_depth=d;
					min_axis=cur_axis;
					if(d==FABS(mi0-mi1))
					{
						if(d==mi0-mi1)
							min_axis=-min_axis;
						else
							1;
					}
					else if(d==FABS(mi0-ma1))
					{
						if(d==mi0-ma1)
							min_axis=-min_axis;
						else
							1;
					}
					else if(d==FABS(ma0-mi1))
					{
						if(d==ma0-mi1)
							min_axis=-min_axis;
						else
							1;
					}
					else
					{
						if(d==ma0-ma1)
							min_axis=-min_axis;
						else
							1;
					}
				}*/

				/*
				if(d<min_depth)
				{
					min_depth=d;
					min_axis=cur_axis;
					if(min0<min1)
						min_axis=-min_axis;
				}*/
			}
			axis=min_axis;
			depth=min_depth;
			return true;
		}
		bool Test::OBBOBB(const OBB& obb0,const OBB& obb1)
		{
			const Vec3& obb0_axis0=obb0.axis(0);
			const Vec3& obb0_axis1=obb0.axis(1);
			const Vec3& obb0_axis2=obb0.axis(2);
			const Vec3& obb0_ext=obb0.extent();
			const Vec3& obb0_center=obb0.center();
			Vec3 obb0_eaxis0=obb0_axis0*obb0_ext[0];
			Vec3 obb0_eaxis1=obb0_axis1*obb0_ext[1];
			Vec3 obb0_eaxis2=obb0_axis2*obb0_ext[2];

			const Vec3& obb1_axis0=obb1.axis(0);
			const Vec3& obb1_axis1=obb1.axis(1);
			const Vec3& obb1_axis2=obb1.axis(2);
			const Vec3& obb1_ext=obb1.extent();
			const Vec3& obb1_center=obb1.center();
			Vec3 obb1_eaxis0=obb1_axis0*obb1_ext[0];
			Vec3 obb1_eaxis1=obb1_axis1*obb1_ext[1];
			Vec3 obb1_eaxis2=obb1_axis2*obb1_ext[2];

			int i;
			Vec3 axis[15],*axis_ptr;

			axis_ptr=axis;

			*axis_ptr=obb0_axis0;				++axis_ptr;
			*axis_ptr=obb0_axis1;				++axis_ptr;
			*axis_ptr=obb0_axis2;				++axis_ptr;
			*axis_ptr=obb1_axis0;				++axis_ptr;
			*axis_ptr=obb1_axis1;				++axis_ptr;
			*axis_ptr=obb1_axis2;				++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis0;	++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis1;	++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis2;	++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis0;	++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis1;	++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis2;	++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis0;	++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis1;	++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis2;

			Vec3 centers_disp=obb0.center()-obb1.center();
			//REAL min0,max0,min1,max1;
			axis_ptr=axis;
			for(i=0;i<15;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				/*
				REAL o0=obb0_center*cur_axis;
				REAL e0=FABS(obb0_eaxis0*cur_axis)+FABS(obb0_eaxis1*cur_axis)+FABS(obb0_eaxis2*cur_axis);
				min0=o0-e0;
				max0=o0+e0;
				REAL o1=obb1_center*cur_axis;
				REAL e1=FABS(obb1_eaxis0*cur_axis)+FABS(obb1_eaxis1*cur_axis)+FABS(obb1_eaxis2*cur_axis);
				min1=o1-e1;
				max1=o1+e1;
				if(!Test::IntervalInterval(min0,max0,min1,max1))
					return false;*/
				REAL e0=FABS(obb0_eaxis0*cur_axis)+FABS(obb0_eaxis1*cur_axis)+FABS(obb0_eaxis2*cur_axis);
				REAL e1=FABS(obb1_eaxis0*cur_axis)+FABS(obb1_eaxis1*cur_axis)+FABS(obb1_eaxis2*cur_axis);
				if(FABS(centers_disp*cur_axis)>e0+e1)
					return false;
			}
			return true;
		}

		bool Test::AABBOBB(const AABB& aabb,const OBB& obb)
		{
			const Vec3& obb0_axis0=Vec3::xAxis;
			const Vec3& obb0_axis1=Vec3::yAxis;
			const Vec3& obb0_axis2=Vec3::zAxis;
			const Vec3& obb0_ext=aabb.extent();
			const Vec3& obb0_center=aabb.center();
			Vec3 obb0_eaxis0=obb0_axis0*obb0_ext[0];
			Vec3 obb0_eaxis1=obb0_axis1*obb0_ext[1];
			Vec3 obb0_eaxis2=obb0_axis2*obb0_ext[2];

			const Vec3& obb1_axis0=obb.axis(0);
			const Vec3& obb1_axis1=obb.axis(1);
			const Vec3& obb1_axis2=obb.axis(2);
			const Vec3& obb1_ext=obb.extent();
			const Vec3& obb1_center=obb.center();
			Vec3 obb1_eaxis0=obb1_axis0*obb1_ext[0];
			Vec3 obb1_eaxis1=obb1_axis1*obb1_ext[1];
			Vec3 obb1_eaxis2=obb1_axis2*obb1_ext[2];

			int i;
			Vec3 axis[15],*axis_ptr;

			axis_ptr=axis;

			*axis_ptr=obb0_axis0;				++axis_ptr;
			*axis_ptr=obb0_axis1;				++axis_ptr;
			*axis_ptr=obb0_axis2;				++axis_ptr;
			*axis_ptr=obb1_axis0;				++axis_ptr;
			*axis_ptr=obb1_axis1;				++axis_ptr;
			*axis_ptr=obb1_axis2;				++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis0;	++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis1;	++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis2;	++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis0;	++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis1;	++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis2;	++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis0;	++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis1;	++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis2;

			REAL min0,max0,min1,max1;
			axis_ptr=axis;
			for(i=0;i<15;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				REAL o0=obb0_center*cur_axis;
				REAL e0=FABS(obb0_eaxis0*cur_axis)+FABS(obb0_eaxis1*cur_axis)+FABS(obb0_eaxis2*cur_axis);
				min0=o0-e0;
				max0=o0+e0;
				REAL o1=obb1_center*cur_axis;
				REAL e1=FABS(obb1_eaxis0*cur_axis)+FABS(obb1_eaxis1*cur_axis)+FABS(obb1_eaxis2*cur_axis);
				min1=o1-e1;
				max1=o1+e1;
				if(!Test::IntervalInterval(min0,max0,min1,max1))
					return false;
			}
			return true;
		}
		bool Test::TriangleSphereSAT(const TriangleAux& tri,const Sphere& s)
		{
			const Vec3& t0=tri[0];
			const Vec3& t1=tri[1];
			const Vec3& t2=tri[2];
			const Vec3& o=s.origin();
			REAL radius=s.radius();
			if(!TriangleSphereSAT_SingleTest(t0,t1,t2,o,radius,tri.normal()))
				return false;
			if(!TriangleSphereSAT_SingleTest(t0,t1,t2,o,radius,(t0-o).normal_safe()))
				return false;
			if(!TriangleSphereSAT_SingleTest(t0,t1,t2,o,radius,(t1-o).normal_safe()))
				return false;
			if(!TriangleSphereSAT_SingleTest(t0,t1,t2,o,radius,(t2-o).normal_safe()))
				return false;
			if(!TriangleSphereSAT_SingleTest(t0,t1,t2,o,radius,(((t0-o)^(t1-o))^(t0-t1)).normal_safe()))
				return false;
			if(!TriangleSphereSAT_SingleTest(t0,t1,t2,o,radius,(((t1-o)^(t2-o))^(t1-t2)).normal_safe()))
				return false;
			if(!TriangleSphereSAT_SingleTest(t0,t1,t2,o,radius,(((t2-o)^(t0-o))^(t2-t0)).normal_safe()))
				return false;
			return true;
		}
		bool Test::TriangleSphereSAT_SingleTest(const Vec3& t0,const Vec3& t1,const Vec3& t2,const Vec3& o,REAL radius,const Vec3& axis)
		{
			REAL min0,max0;
			REAL t;
			t=t0*axis;
			min0=t;
			max0=t;

			t=t1*axis;
			min0=min(min0,t);
			max0=max(max0,t);

			t=t2*axis;
			min0=min(min0,t);
			max0=max(max0,t);

			REAL min1,max1;
			t=o*axis;
			min1=t-radius;
			max1=t+radius;

			return IntervalInterval(min0,max0,min1,max1);
		}
		bool				Test::OBBTriangle(const Vec3& obb_center, const Vec3 obb_eaxis[3], const Vec3& v0, const Vec3& v1, const Vec3& v2)
		{
			Vec3 e0 = v1 - v0;
			Vec3 e1 = v2 - v1;
			Vec3 tri_nor = e0 ^ e1;

			// triangle face normal
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(tri_nor), obb_center, obb_eaxis, v0, v1, v2))
				return false;

			// obb faces' axis
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(obb_eaxis[0]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(obb_eaxis[1]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(obb_eaxis[2]), obb_center, obb_eaxis, v0, v1, v2))
				return false;

			Vec3 e2 = v0 - v2;
			// cross products of edges from both
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e0 ^ obb_eaxis[0]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e1 ^ obb_eaxis[0]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e2 ^ obb_eaxis[0]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e0 ^ obb_eaxis[1]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e1 ^ obb_eaxis[1]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e2 ^ obb_eaxis[1]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e0 ^ obb_eaxis[2]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e1 ^ obb_eaxis[2]), obb_center, obb_eaxis, v0, v1, v2))
				return false;
			if(SAT::test_1axis_obb_tri_sep(SAT::stablize_axis(e2 ^ obb_eaxis[2]), obb_center, obb_eaxis, v0, v1, v2))
				return false;

			return true;
		}
		bool				Test::SphereTriangle(const Vec3& center, REAL radius, const Vec3& p0, const Vec3& p1, const Vec3& p2)
		{
			Vec3 n = ((p1 - p0) ^ (p2 - p0)).normal_safe();

			REAL d = (center - p0) * n;
			if(FABS(d) > radius)
				return false;

			Vec3 proj_center = center - d * n;
			REAL proj_radius = SQRT(max(0.0f, radius * radius - d * d));

			Vec3 axis;
			REAL c;
			REAL t0, t1, t2;

			axis = ((p1 - p0) ^ n).normal_safe(); c = proj_center * axis; t0 = p0 * axis; t1 = p1 * axis; t2 = p2 * axis;
			if(SAT::test_intervals_sep(c - proj_radius, c + proj_radius, min(min(t0, t1), t2), max(max(t0, t1), t2)))
				return false;
			axis = ((p2 - p1) ^ n).normal_safe(); c = proj_center * axis; t0 = p0 * axis; t1 = p1 * axis; t2 = p2 * axis;
			if(SAT::test_intervals_sep(c - proj_radius, c + proj_radius, min(min(t0, t1), t2), max(max(t0, t1), t2)))
				return false;
			axis = ((p0 - p2) ^ n).normal_safe(); c = proj_center * axis; t0 = p0 * axis; t1 = p1 * axis; t2 = p2 * axis;
			if(SAT::test_intervals_sep(c - proj_radius, c + proj_radius, min(min(t0, t1), t2), max(max(t0, t1), t2)))
				return false;
			axis = (p0 - proj_center).normal_safe(); c = proj_center * axis; t0 = p0 * axis; t1 = p1 * axis; t2 = p2 * axis;
			if(SAT::test_intervals_sep(c - proj_radius, c + proj_radius, min(min(t0, t1), t2), max(max(t0, t1), t2)))
				return false;
			axis = (p1 - proj_center).normal_safe(); c = proj_center * axis; t0 = p0 * axis; t1 = p1 * axis; t2 = p2 * axis;
			if(SAT::test_intervals_sep(c - proj_radius, c + proj_radius, min(min(t0, t1), t2), max(max(t0, t1), t2)))
				return false;
			axis = (p2 - proj_center).normal_safe(); c = proj_center * axis; t0 = p0 * axis; t1 = p1 * axis; t2 = p2 * axis;
			if(SAT::test_intervals_sep(c - proj_radius, c + proj_radius, min(min(t0, t1), t2), max(max(t0, t1), t2)))
				return false;
			return true;
		}
	};
};