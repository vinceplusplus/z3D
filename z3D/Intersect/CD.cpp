
#include "CD.h"

#include "SAT.h"

namespace z3D
{
	namespace Intersect
	{
		bool CD::AABBTriangle(const AABB& aabb,const Vec3& w,const Triangle& tri,REAL& t)
		{
			const Vec3& aabb_ext=aabb.extent();
			const Vec3& aabb_center=aabb.center();

			const Vec3& tri_vert0=tri[0];
			const Vec3& tri_vert1=tri[1];
			const Vec3& tri_vert2=tri[2];

			Vec3 w_normal=w.normal();

			Vec3 edge0,edge1,edge2,tri_normal;
			edge0=tri_vert1-tri_vert0;edge0.normalize();
			edge1=tri_vert2-tri_vert1;edge1.normalize();
			edge2=tri_vert0-tri_vert2;edge2.normalize();
			tri_normal=edge0^edge1;

			int i;
			Vec3 axis[19],*axis_ptr;
			axis_ptr=axis;

			*axis_ptr=tri_normal;					++axis_ptr;
			*axis_ptr=Vec3::xAxis;					++axis_ptr;
			*axis_ptr=Vec3::yAxis;					++axis_ptr;
			*axis_ptr=Vec3::zAxis;					++axis_ptr;
			*axis_ptr=Vec3::xAxis^edge0;			++axis_ptr;
			*axis_ptr=Vec3::xAxis^edge1;			++axis_ptr;
			*axis_ptr=Vec3::xAxis^edge2;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^edge0;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^edge1;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^edge2;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^edge0;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^edge1;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^edge2;			++axis_ptr;
			*axis_ptr=Vec3::xAxis^w_normal;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^w_normal;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^w_normal;			++axis_ptr;
			*axis_ptr=w_normal^(edge0^tri_normal);	++axis_ptr;
			*axis_ptr=w_normal^(edge1^tri_normal);	++axis_ptr;
			*axis_ptr=w_normal^(edge2^tri_normal);

			REAL delta;
			REAL min0,max0,min1,max1;
			REAL t0,t1;
			REAL int0=0,int1=INFINITY;
			axis_ptr=axis;
			for(i=0;i<19;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				delta=w*cur_axis;
				REAL o=aabb_center*cur_axis;
				REAL e=FABS(aabb_ext[0]*cur_axis[0])+FABS(aabb_ext[1]*cur_axis[1])+FABS(aabb_ext[2]*cur_axis[2]);
				min0=o-e;
				max0=o+e;
				min1=tri_vert0*cur_axis;
				max1=min1;
				REAL tmp=tri_vert1*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				tmp=tri_vert2*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				if(FABS(delta)<=EPSILON)
				{
					if(Test::IntervalInterval(min0,max0,min1,max1))
					{
						continue;
						t0=-INFINITY;
						t1=INFINITY;
					}
					else
						return false;
				}
				else
				{
					REAL inv_delta=1/delta;
					t0=(min1-max0)*inv_delta;
					t1=(max1-min0)*inv_delta;
					SORT(t0,t1);
				}
				int0=max(int0,t0);
				int1=min(int1,t1);
				if(int0>int1)
					return false;
			}
			t=int0;
			return true;
		}
		bool CD::AABBTriangle(const AABB& aabb,const Vec3& w,const TriangleAux& tri,REAL& t)
		{
			const Vec3& aabb_ext=aabb.extent();
			const Vec3& aabb_center=aabb.center();

			const Vec3& tri_vert0=tri[0];
			const Vec3& tri_vert1=tri[1];
			const Vec3& tri_vert2=tri[2];

			const Vec3& edge0=tri.edge_dir(0);
			const Vec3& edge1=tri.edge_dir(1);
			const Vec3& edge2=tri.edge_dir(2);

			Vec3 w_normal=w.normal();

			int i;
			Vec3 axis[19],*axis_ptr;
			axis_ptr=axis;

			*axis_ptr=tri.normal();					++axis_ptr;
			*axis_ptr=Vec3::xAxis;					++axis_ptr;
			*axis_ptr=Vec3::yAxis;					++axis_ptr;
			*axis_ptr=Vec3::zAxis;					++axis_ptr;
			*axis_ptr=Vec3::xAxis^edge0;			++axis_ptr;
			*axis_ptr=Vec3::xAxis^edge1;			++axis_ptr;
			*axis_ptr=Vec3::xAxis^edge2;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^edge0;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^edge1;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^edge2;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^edge0;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^edge1;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^edge2;			++axis_ptr;
			*axis_ptr=Vec3::xAxis^w_normal;			++axis_ptr;
			*axis_ptr=Vec3::yAxis^w_normal;			++axis_ptr;
			*axis_ptr=Vec3::zAxis^w_normal;			++axis_ptr;
			*axis_ptr=w_normal^tri.edge_normal(0);	++axis_ptr;
			*axis_ptr=w_normal^tri.edge_normal(1);	++axis_ptr;
			*axis_ptr=w_normal^tri.edge_normal(2);

			REAL delta;
			REAL min0,max0,min1,max1;
			REAL t0,t1;
			REAL int0=0,int1=INFINITY;
			axis_ptr=axis;
			for(i=0;i<19;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				delta=w*cur_axis;
				REAL o=aabb_center*cur_axis;
				REAL e=FABS(aabb_ext[0]*cur_axis[0])+FABS(aabb_ext[1]*cur_axis[1])+FABS(aabb_ext[2]*cur_axis[2]);
				min0=o-e;
				max0=o+e;
				min1=tri_vert0*cur_axis;
				max1=min1;
				REAL tmp=tri_vert1*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				tmp=tri_vert2*cur_axis;
				min1=min(tmp,min1);max1=max(tmp,max1);
				if(FABS(delta)<=EPSILON)
				{
					if(Test::IntervalInterval(min0,max0,min1,max1))
					{
						continue;
						t0=-INFINITY;
						t1=INFINITY;
					}
					else
						return false;
				}
				else
				{
					REAL inv_delta=1/delta;
					t0=(min1-max0)*inv_delta;
					t1=(max1-min0)*inv_delta;
					SORT(t0,t1);
				}
				int0=max(int0,t0);
				int1=min(int1,t1);
				if(int0>int1)
					return false;
			}
			t=int0;
			return true;
		}
		bool CD::OBBTriangle(const OBB& obb,const Vec3& w,const Triangle& tri,REAL& t)
		{
			const Vec3& obb_axis0=obb.axis(0);
			const Vec3& obb_axis1=obb.axis(1);
			const Vec3& obb_axis2=obb.axis(2);
			const Vec3& obb_ext=obb.extent();
			const Vec3& obb_center=obb.center();
			Vec3 obb_eaxis0=obb_axis0*obb_ext[0];
			Vec3 obb_eaxis1=obb_axis1*obb_ext[1];
			Vec3 obb_eaxis2=obb_axis2*obb_ext[2];

			const Vec3& tri_vert0=tri[0];
			const Vec3& tri_vert1=tri[1];
			const Vec3& tri_vert2=tri[2];

			Vec3 w_normal=w.normal();

			int i;
			Vec3 axis[19],edge0,edge1,edge2,tri_normal,*axis_ptr;

			edge0=tri_vert1-tri_vert0;edge0.normalize();
			edge1=tri_vert2-tri_vert1;edge1.normalize();
			edge2=tri_vert0-tri_vert2;edge2.normalize();
			tri_normal=edge0^edge1;

			axis_ptr=axis;

			*axis_ptr=edge0^edge1;					++axis_ptr;
			*axis_ptr=obb_axis0;					++axis_ptr;
			*axis_ptr=obb_axis1;					++axis_ptr;
			*axis_ptr=obb_axis2;					++axis_ptr;
			*axis_ptr=obb_axis0^edge0;				++axis_ptr;
			*axis_ptr=obb_axis0^edge1;				++axis_ptr;
			*axis_ptr=obb_axis0^edge2;				++axis_ptr;
			*axis_ptr=obb_axis1^edge0;				++axis_ptr;
			*axis_ptr=obb_axis1^edge1;				++axis_ptr;
			*axis_ptr=obb_axis1^edge2;				++axis_ptr;
			*axis_ptr=obb_axis2^edge0;				++axis_ptr;
			*axis_ptr=obb_axis2^edge1;				++axis_ptr;
			*axis_ptr=obb_axis2^edge2;				++axis_ptr;
			*axis_ptr=obb_axis0^w_normal;			++axis_ptr;
			*axis_ptr=obb_axis1^w_normal;			++axis_ptr;
			*axis_ptr=obb_axis2^w_normal;			++axis_ptr;
			*axis_ptr=w_normal^(edge0^tri_normal);	++axis_ptr;
			*axis_ptr=w_normal^(edge1^tri_normal);	++axis_ptr;
			*axis_ptr=w_normal^(edge2^tri_normal);

			REAL delta;
			REAL min0,max0,min1,max1;
			REAL t0,t1;
			REAL int0=0,int1=INFINITY;
			axis_ptr=axis;
			for(i=0;i<19;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				delta=w*cur_axis;
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
				if(FABS(delta)<=EPSILON)
				{
					if(Test::IntervalInterval(min0,max0,min1,max1))
					{
						continue;
						t0=-INFINITY;
						t1=INFINITY;
					}
					else
						return false;
				}
				else
				{
					REAL inv_delta=1/delta;
					t0=(min1-max0)*inv_delta;
					t1=(max1-min0)*inv_delta;
					SORT(t0,t1);
				}
				int0=max(int0,t0);
				int1=min(int1,t1);
				if(int0>int1)
					return false;
			}
			t=int0;
			return true;
		}
		bool CD::OBBTriangle(const OBB& obb,const Vec3& w,const TriangleAux& tri,REAL& t0,REAL& t1)
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

			const Vec3& edge0=tri.edge(0);
			const Vec3& edge1=tri.edge(1);
			const Vec3& edge2=tri.edge(2);

			Vec3 w_normal=w.normal();

			int i;
			Vec3 axis[19],*axis_ptr;
			axis_ptr=axis;

			*axis_ptr=tri.normal();					++axis_ptr;
			*axis_ptr=obb_axis0;					++axis_ptr;
			*axis_ptr=obb_axis1;					++axis_ptr;
			*axis_ptr=obb_axis2;					++axis_ptr;
			*axis_ptr=obb_axis0^edge0;				++axis_ptr;
			*axis_ptr=obb_axis0^edge1;				++axis_ptr;
			*axis_ptr=obb_axis0^edge2;				++axis_ptr;
			*axis_ptr=obb_axis1^edge0;				++axis_ptr;
			*axis_ptr=obb_axis1^edge1;				++axis_ptr;
			*axis_ptr=obb_axis1^edge2;				++axis_ptr;
			*axis_ptr=obb_axis2^edge0;				++axis_ptr;
			*axis_ptr=obb_axis2^edge1;				++axis_ptr;
			*axis_ptr=obb_axis2^edge2;				++axis_ptr;
			*axis_ptr=obb_axis0^w_normal;			++axis_ptr;
			*axis_ptr=obb_axis1^w_normal;			++axis_ptr;
			*axis_ptr=obb_axis2^w_normal;			++axis_ptr;
			*axis_ptr=tri.edge_normal(0)^w_normal;	++axis_ptr;
			*axis_ptr=tri.edge_normal(1)^w_normal;	++axis_ptr;
			*axis_ptr=tri.edge_normal(2)^w_normal;

			REAL int0=0,int1=INFINITY;
			if(true)
			{
				REAL delta;
				REAL min0,max0,min1,max1;
				REAL t0,t1;
				axis_ptr=axis;
				for(i=0;i<19;++i,++axis_ptr)
				{
//					if((*axis_ptr).length()<0.01f)
//						continue;
					const Vec3& cur_axis=*axis_ptr;
					delta=w*cur_axis;
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
					if(FABS(delta)<=EPSILON)
					{
						if(Test::IntervalInterval(min0,max0,min1,max1))
						{
							continue;
							t0=-INFINITY;
							t1=INFINITY;
						}
						else
							return false;
					}
					else
					{
						REAL inv_delta=1/delta;
						t0=(min1-max0)*inv_delta;
						t1=(max1-min0)*inv_delta;
						SORT(t0,t1);
					}
					int0=max(int0,t0);
					int1=min(int1,t1);
					if(int0>int1)
						return false;
				}
			}
			t0=int0;
			t1=int1;
			return true;
		}
		bool CD::OBBTriangle(const OBB& obb,const Vec3& w,const TriangleAux& tri,REAL& t)
		{
			REAL t1;
			return OBBTriangle(obb,w,tri,t,t1);
		}
		bool CD::OBBOBB(const OBB& obb0,const Vec3& w,const OBB& obb1,REAL& t)
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

			Vec3 w_normal=w.normal();

			int i;
			Vec3 axis[21],*axis_ptr;
			axis_ptr=axis;

			*axis_ptr=obb0_axis0;					++axis_ptr;
			*axis_ptr=obb0_axis1;					++axis_ptr;
			*axis_ptr=obb0_axis2;					++axis_ptr;
			*axis_ptr=obb1_axis0;					++axis_ptr;
			*axis_ptr=obb1_axis1;					++axis_ptr;
			*axis_ptr=obb1_axis2;					++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis0;		++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis1;		++axis_ptr;
			*axis_ptr=obb0_axis0^obb1_axis2;		++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis0;		++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis1;		++axis_ptr;
			*axis_ptr=obb0_axis1^obb1_axis2;		++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis0;		++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis1;		++axis_ptr;
			*axis_ptr=obb0_axis2^obb1_axis2;		++axis_ptr;
			*axis_ptr=w_normal^obb0_axis0;			++axis_ptr;
			*axis_ptr=w_normal^obb0_axis1;			++axis_ptr;
			*axis_ptr=w_normal^obb0_axis2;			++axis_ptr;
			*axis_ptr=w_normal^obb1_axis0;			++axis_ptr;
			*axis_ptr=w_normal^obb1_axis1;			++axis_ptr;
			*axis_ptr=w_normal^obb1_axis2;			++axis_ptr;

			REAL delta;
			REAL min0,max0,min1,max1;
			REAL t0,t1;
			REAL int0=0,int1=INFINITY;
			axis_ptr=axis;
			for(i=0;i<21;++i,++axis_ptr)
			{
				const Vec3& cur_axis=*axis_ptr;
				delta=w*cur_axis;
				REAL o0=obb0_center*cur_axis;
				REAL e0=FABS(obb0_eaxis0*cur_axis)+FABS(obb0_eaxis1*cur_axis)+FABS(obb0_eaxis2*cur_axis);
				min0=o0-e0;
				max0=o0+e0;
				REAL o1=obb1_center*cur_axis;
				REAL e1=FABS(obb1_eaxis0*cur_axis)+FABS(obb1_eaxis1*cur_axis)+FABS(obb1_eaxis2*cur_axis);
				min1=o1-e1;
				max1=o1+e1;
				if(FABS(delta)<=EPSILON)
				{
					if(Test::IntervalInterval(min0,max0,min1,max1))
					{
						continue;
						t0=-INFINITY;
						t1=INFINITY;
					}
					else
						return false;
				}
				else
				{
					REAL inv_delta=1/delta;
					t0=(min1-max0)*inv_delta;
					t1=(max1-min0)*inv_delta;
					SORT(t0,t1);
				}
				int0=max(int0,t0);
				int1=min(int1,t1);
				if(int0>int1)
					return false;
			}
			t=int0;
			return true;
		}
		bool				CD::SphereTriangle(const Vec3& so, REAL sr, const Vec3& w, const Vec3& t0, const Vec3& t1, const Vec3& t2, REAL& toi, Vec3& collinormal, Vec3& collipoint)
		{
			Vec3 nor = ((t1 - t0) ^ (t2 - t0)).normal_safe();
			REAL t;
			Ray r;
			if(!Intr::RayTriangle(r = Ray(so - nor * sr, w), t0, t1, t2, t))
				return false;
			toi = t;
			collinormal = nor * SETSIGN(1.0f, SIGNBIT((so - t0) * nor));
			collipoint = r.origin() + r.direction() * t;
			return true;
		}
		bool				CD::SphereLineSeg(const Vec3& so, REAL sr, const Vec3& w, const Vec3& l0, const Vec3& l1, REAL& toi, Vec3& collinormal, Vec3& collipoint)
		{
			const Vec3& P = l0;
			const Vec3 D = l1 - l0;
			const Vec3& C = so;
			const Vec3& W = w;

			REAL DdotD = D * D;

			if(DdotD < EPSILON)
				return false;

			REAL WdotD = W * D;
			REAL CdotD = C * D;
			REAL PdotD = P * D;

			Vec3 m = D * WdotD - W * DdotD;
			Vec3 n = (P - C) * DdotD + D * CdotD - D * PdotD;

			REAL a = m * m;
			REAL b = 2 * m * n;
			REAL c = n * n - sr * sr * DdotD * DdotD;

			if(a < EPSILON)
				return false;

			REAL delta = b * b - 4 * a * c;
			if(delta < 0.0f)
				return false;

			REAL delta_sqrt = SQRT(delta);
			REAL _1_2a = 1.0f / (2.0f * a);

			REAL t0 = (-b - delta_sqrt) * _1_2a;
			REAL t1 = (-b + delta_sqrt) * _1_2a;
			SORT(t0, t1);
			if(t0 >= 0.0f)
				toi = t0;
			else if(t1 >= 0.0f)
				toi = t1;
			else
				return false;

			REAL s = (CdotD + toi * WdotD - PdotD) / DdotD;
			if(s < 0.0f || s > 1.0f)
				return false;

			collipoint = P + s * D;
			collinormal = (C + toi * W - collipoint).normal_safe();

			return true;
		}
		bool				CD::SphereSphere(const Vec3& so0, REAL sr0, const Vec3& w0, const Vec3& so1, REAL sr1, REAL& toi, Vec3& collinormal, Vec3& collipoint)
		{
			Vec3 c1_c2 = so0 - so1;
			REAL r = sr0 + sr1;
			REAL a, b, c;
			a = w0 * w0;
			b = (c1_c2) * w0 * 2;
			c = c1_c2 * c1_c2 - r * r;

			if(a < EPSILON)
				return false;

			REAL delta = b * b - 4 * a * c;
			if(delta < 0.0f)
				return false;

			REAL delta_sqrt = SQRT(delta);
			REAL _1_2a = 1.0f / (2.0f * a);

			REAL t0 = (-b - delta_sqrt) * _1_2a;
			REAL t1 = (-b + delta_sqrt) * _1_2a;
			SORT(t0, t1);
			if(t0 >= 0.0f)
				toi = t0;
			else if(t1 >= 0.0f)
				toi = t1;
			else
				return false;
			collinormal = (so0 + toi * w0) - so1;
			collinormal.normalize_safe();
			collipoint = so1 + collinormal * sr1;
			return true;
		}
		bool				CD::SphereSphere(const Vec3& so0, REAL sr0, const Vec3& w0, const Vec3& so1, REAL sr1, const Vec3& w1, REAL& toi, Vec3& collinormal, Vec3& collipoint)
		{
			Vec3 w = w0 - w1;
			REAL len = w.length();
			if(len < z3D::Core::EPSILON)
				return false;
			if(!SphereSphere(so0, sr0, w, so1, sr1, toi, collinormal, collipoint))
				return false;
			collipoint += w1 * toi;
			return true;
		}
		bool				CD::SphereTriangleFull(const Vec3& so, REAL sr, const Vec3& w, const Vec3& t0, const Vec3& t1, const Vec3& t2, REAL& toi, Vec3& collinormal, Vec3& collipoint)
		{
			toi = INFINITY;

			REAL toi2;
			Vec3 collinormal2, collipoint2;

			if(SphereTriangle(so, sr, w, t0, t1, t2, toi2, collinormal2, collipoint2) && toi2 < toi)
			{
				toi = toi2;
				collinormal = collinormal2;
				collipoint = collipoint2;
			}
			if(SphereLineSeg(so, sr, w, t0, t1, toi2, collinormal2, collipoint2) && toi2 < toi)
			{
				toi = toi2;
				collinormal = collinormal2;
				collipoint = collipoint2;
			}
			if(SphereLineSeg(so, sr, w, t1, t2, toi2, collinormal2, collipoint2) && toi2 < toi)
			{
				toi = toi2;
				collinormal = collinormal2;
				collipoint = collipoint2;
			}
			if(SphereLineSeg(so, sr, w, t2, t0, toi2, collinormal2, collipoint2) && toi2 < toi)
			{
				toi = toi2;
				collinormal = collinormal2;
				collipoint = collipoint2;
			}
			if(SphereSphere(so, sr, w, t0, 0.0f, toi2, collinormal2, collipoint2) && toi2 < toi)
			{
				toi = toi2;
				collinormal = collinormal2;
				collipoint = collipoint2;
			}
			if(SphereSphere(so, sr, w, t1, 0.0f, toi2, collinormal2, collipoint2) && toi2 < toi)
			{
				toi = toi2;
				collinormal = collinormal2;
				collipoint = collipoint2;
			}
			if(SphereSphere(so, sr, w, t2, 0.0f, toi2, collinormal2, collipoint2) && toi2 < toi)
			{
				toi = toi2;
				collinormal = collinormal2;
				collipoint = collipoint2;
			}
			if(toi == INFINITY)
				return false;

			return true;
		}

		bool				CD::SphereLineSeg(const Vec3& so, REAL sr, const Vec3& w, const Vec3& p, const Vec3& d, REAL& t0, REAL& t1, Vec3* a0, Vec3* a1)
		{
			// TODO: alter to compute s first?
			const Vec3& P = p;
			const Vec3 D = d;
			const Vec3& C = so;
			const Vec3& W = w;

			REAL DdotD = D * D;

			if(DdotD < EPSILON)
				return false;

			REAL WdotD = W * D;
			REAL CdotD = C * D;
			REAL PdotD = P * D;

			Vec3 m = D * WdotD - W * DdotD;
			Vec3 n = (P - C) * DdotD + D * CdotD - D * PdotD;

			REAL a = m * m;
			REAL b = 2 * m * n;
			REAL c = n * n - sr * sr * DdotD * DdotD;

			if(a < EPSILON)
				return false;

			REAL delta = b * b - 4 * a * c;
			if(delta < 0.0f)
				return false;

			REAL delta_sqrt = SQRT(delta);
			REAL _1_2a = 1.0f / (2.0f * a);

			REAL _t0 = (-b - delta_sqrt) * _1_2a;
			REAL _t1 = (-b + delta_sqrt) * _1_2a;
			SORT(_t0, _t1);

			REAL s0 = (CdotD + _t0 * WdotD - PdotD) / DdotD;
			REAL s1 = (CdotD + _t1 * WdotD - PdotD) / DdotD;
			if(s0 < 0.0f || s0 > 1.0f)
			{
				if(s1 < 0.0f || s1 > 1.0f)
				{
					return false;
				}
				else
				{
					t0 = t1 = _t1;

					Vec3 a = C + _t1 * W - (P + s1 * D);
					if(a0)
						*a0 = a;
					if(a1)
						*a1 = a;
				}
			}
			else
			{
				if(s1 < 0.0f || s1 > 1.0f)
				{
					t0 = t1 = _t0;

					Vec3 a = C + _t0 * W - (P + s0 * D);
					if(a0)
						*a0 = a;
					if(a1)
						*a1 = a;
				}
				else
				{
					t0 = _t0;
					t1 = _t1;

					if(a0)
						*a0 = C + _t0 * W - (P + s0 * D);
					if(a1)
						*a1 = C + _t1 * W - (P + s1 * D);
				}
			}

			return true;
		}
		bool				CD::SphereSphere(const Vec3& so0, REAL sr0, const Vec3& w0, const Vec3& so1, REAL sr1, REAL& t0, REAL& t1, Vec3* a0, Vec3* a1)
		{
			Vec3 c1_c2 = so0 - so1;
			REAL r = sr0 + sr1;
			REAL a, b, c;
			a = w0 * w0;
			b = (c1_c2) * w0 * 2;
			c = c1_c2 * c1_c2 - r * r;

			if(a < EPSILON)
				return false;

			REAL delta = b * b - 4 * a * c;
			if(delta < 0.0f)
				return false;

			REAL delta_sqrt = SQRT(delta);
			REAL _1_2a = 1.0f / (2.0f * a);

			t0 = (-b - delta_sqrt) * _1_2a;
			t1 = (-b + delta_sqrt) * _1_2a;
			SORT(t0, t1);

			if(a0)
				*a0 = (so0 + t0 * w0) - so1;
			if(a1)
				*a1 = (so0 + t1 * w0) - so1;

			return true;
		}
		bool				CD::SphereTriangleFull2(const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis)
		{
			Vec3 fa, la;
			REAL _t0, _t1;
			_t0 = INFINITY;
			_t1 = -INFINITY;
			if(true)
			{
				Vec3 nor = ((v1 - v0) ^ (v2 - v0)).normal_safe();
				Vec3 off = nor * sr * SETSIGN(1.0f, SIGNBIT(nor * w));
				if(Intr::LineTriangle(so + off, w, v0, v1, v2, _t0))
					if(first_time_contact_axis)
						fa = nor;
				Intr::LineTriangle(so - off, w, v0, v1, v2, _t1);
					if(last_time_contact_axis)
						la = nor;
			}
			if(_t0 == INFINITY || _t1 == -INFINITY)
			{
				REAL _tt0, _tt1;
				Vec3 fa2, la2;
				Vec3* pfa = (first_time_contact_axis && _t0 == INFINITY) ? &fa2 : NULL;
				Vec3* pla = (last_time_contact_axis && _t1 == -INFINITY) ? &la2 : NULL;
				if(SphereLineSeg(so, sr, w, v0, v1 - v0, _tt0, _tt1, pfa, pla))
				{
					if(_tt0 < _t0)
					{
						_t0 = _tt0;
						if(pfa)
							fa = fa2;
					}
					if(_tt1 > _t1)
					{
						_t1 = _tt1;
						if(pla)
							la = la2;
					}
				}
				if(SphereLineSeg(so, sr, w, v1, v2 - v1, _tt0, _tt1, pfa, pla))
				{
					if(_tt0 < _t0)
					{
						_t0 = _tt0;
						if(pfa)
							fa = fa2;
					}
					if(_tt1 > _t1)
					{
						_t1 = _tt1;
						if(pla)
							la = la2;
					}
				}
				if(SphereLineSeg(so, sr, w, v2, v0 - v2, _tt0, _tt1, pfa, pla))
				{
					if(_tt0 < _t0)
					{
						_t0 = _tt0;
						if(pfa)
							fa = fa2;
					}
					if(_tt1 > _t1)
					{
						_t1 = _tt1;
						if(pla)
							la = la2;
					}
				}
				if(SphereSphere(so, sr, w, v0, 0, _tt0, _tt1, pfa, pla))
				{
					if(_tt0 < _t0)
					{
						_t0 = _tt0;
						if(pfa)
							fa = fa2;
					}
					if(_tt1 > _t1)
					{
						_t1 = _tt1;
						if(pla)
							la = la2;
					}
				}
				if(SphereSphere(so, sr, w, v1, 0, _tt0, _tt1, pfa, pla))
				{
					if(_tt0 < _t0)
					{
						_t0 = _tt0;
						if(pfa)
							fa = fa2;
					}
					if(_tt1 > _t1)
					{
						_t1 = _tt1;
						if(pla)
							la = la2;
					}
				}
				if(SphereSphere(so, sr, w, v2, 0, _tt0, _tt1, pfa, pla))
				{
					if(_tt0 < _t0)
					{
						_t0 = _tt0;
						if(pfa)
							fa = fa2;
					}
					if(_tt1 > _t1)
					{
						_t1 = _tt1;
						if(pla)
							la = la2;
					}
				}
			}
			if(_t0 == INFINITY || _t1 == -INFINITY)
				return false;

			t0 = _t0;
			t1 = _t1;

			if(first_time_contact_axis)
				*first_time_contact_axis = fa;
			if(last_time_contact_axis)
				*last_time_contact_axis = la;

			return true;
		}

		bool				CD::OBBTriangle(const Vec3& obb_center, const Vec3 obb_eaxis[3], const Vec3& obb_velocity, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis)
		{
			t0 = -INFINITY;
			t1 = INFINITY;

			Vec3 e0 = v1 - v0;
			Vec3 e1 = v2 - v1;
			Vec3 tri_nor = e0 ^ e1;

			// triangle face normal
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(tri_nor), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;

			// obb faces' axis
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(obb_eaxis[0]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(obb_eaxis[1]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(obb_eaxis[2]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;

			Vec3 e2 = v0 - v2;
			// cross products of edges from both
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e0 ^ obb_eaxis[0]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e1 ^ obb_eaxis[0]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e2 ^ obb_eaxis[0]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e0 ^ obb_eaxis[1]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e1 ^ obb_eaxis[1]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e2 ^ obb_eaxis[1]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e0 ^ obb_eaxis[2]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e1 ^ obb_eaxis[2]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;
			if(!SAT::cd_1axis_moving_obb_tri(SAT::stablize_axis(e2 ^ obb_eaxis[2]), obb_center, obb_eaxis, obb_velocity, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;

			if(t0 > t1)
				return false;
			return true;
		}
		template<bool ACCURATE>
		bool				CD::SphereTriangleSAT(const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis)
		{
			t0 = -INFINITY;
			t1 = INFINITY;

			Vec3 e0 = v1 - v0;
			Vec3 e1 = v2 - v1;
			Vec3 e2 = v0 - v2;
			Vec3 tri_nor = (e0 ^ e1).normal_safe();

			// triangle face normal
			if(!SAT::cd_1axis_moving_sphere_tri(tri_nor, so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
				return false;

			// TODO: actually, if we use the accurate version, can the following static cases be skipped?

			if(!ACCURATE)
			{
				// edge cases
				if(!SAT::cd_1axis_moving_sphere_tri(((e0 ^ (so - v0)) ^ e0).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
					return false;
				if(!SAT::cd_1axis_moving_sphere_tri(((e1 ^ (so - v1)) ^ e1).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
					return false;
				if(!SAT::cd_1axis_moving_sphere_tri(((e2 ^ (so - v2)) ^ e2).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
					return false;

				// vertex cases
				if(!SAT::cd_1axis_moving_sphere_tri((so - v0).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
					return false;
				if(!SAT::cd_1axis_moving_sphere_tri((so - v1).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
					return false;
				if(!SAT::cd_1axis_moving_sphere_tri((so - v2).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
					return false;
			}

			if(ACCURATE)
			{
				REAL sr_sq = sr * sr;

				if(true)
				{
					Vec3 pn = (e0 ^ w).normal_safe();
					REAL d = (so - v0) * pn;
					if(FABS(d) > sr)
					{
						if(!SAT::cd_1axis_moving_sphere_tri(pn, so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
					else
					{
						Vec3 h = (e0 ^ pn).normal_safe() * SQRT(max(0.0f, sr_sq - d * d));
						if(!SAT::cd_1axis_moving_sphere_tri((-d * pn + h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
						if(!SAT::cd_1axis_moving_sphere_tri((-d * pn - h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}

					pn = (e1 ^ w).normal_safe();
					d = (so - v1) * pn;
					if(FABS(d) > sr)
					{
						if(!SAT::cd_1axis_moving_sphere_tri(pn, so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
					else
					{
						Vec3 h = (e1 ^ pn).normal_safe() * SQRT(max(0.0f, sr_sq - d * d));
						if(!SAT::cd_1axis_moving_sphere_tri((-d * pn + h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
						if(!SAT::cd_1axis_moving_sphere_tri((-d * pn - h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}

					pn = (e2 ^ w).normal_safe();
					d = (so - v2) * pn;
					if(FABS(d) > sr)
					{
						if(!SAT::cd_1axis_moving_sphere_tri(pn, so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
					else
					{
						Vec3 h = (e2 ^ pn).normal_safe() * SQRT(max(0.0f, sr_sq - d * d));
						if(!SAT::cd_1axis_moving_sphere_tri((-d * pn + h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
						if(!SAT::cd_1axis_moving_sphere_tri((-d * pn - h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
				}

				Vec3 w_normal = w.normal_safe();

				if(true)
				{
					Vec3 d = v0 - so;
					Vec3 v = d - (d * w_normal) * w_normal;
					REAL lsq = v.lengthsq();
					if(lsq > sr_sq)
					{
						if(!SAT::cd_1axis_moving_sphere_tri(v.normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
					else
					{
						Vec3 h = w_normal * SQRT(max(0.0f, sr_sq - lsq));
						if(!SAT::cd_1axis_moving_sphere_tri((v + h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
						if(!SAT::cd_1axis_moving_sphere_tri((v - h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}

					d = v1 - so;
					v = d - (d * w_normal) * w_normal;
					lsq = v.lengthsq();
					if(lsq > sr_sq)
					{
						if(!SAT::cd_1axis_moving_sphere_tri(v.normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
					else
					{
						Vec3 h = w_normal * SQRT(max(0.0f, sr_sq - lsq));
						if(!SAT::cd_1axis_moving_sphere_tri((v + h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
						if(!SAT::cd_1axis_moving_sphere_tri((v - h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}

					d = v2 - so;
					v = d - (d * w_normal) * w_normal;
					lsq = v.lengthsq();
					if(lsq > sr_sq)
					{
						if(!SAT::cd_1axis_moving_sphere_tri(v.normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
					else
					{
						Vec3 h = w_normal * SQRT(max(0.0f, sr_sq - lsq));
						if(!SAT::cd_1axis_moving_sphere_tri((v + h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
						if(!SAT::cd_1axis_moving_sphere_tri((v - h).normal_safe(), so, sr, w, v0, v1, v2, t0, t1, first_time_contact_axis, last_time_contact_axis))
							return false;
					}
				}
			}

			if(t0 > t1)
				return false;
			return true;
		}

		// explicit instantiation
		template
		bool				CD::SphereTriangleSAT<false>(const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis);
		template
		bool				CD::SphereTriangleSAT<true>(const Vec3& so, REAL sr, const Vec3& w, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t0, REAL& t1, Vec3* first_time_contact_axis, Vec3* last_time_contact_axis);
	};
};