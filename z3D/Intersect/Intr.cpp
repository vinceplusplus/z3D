
#include "Intr.h"
#include "Test.h"

namespace z3D
{
	namespace Intersect
	{
		bool Intr::RayPlane(const Ray& r,const Plane& p,float& t)
		{
			REAL t2;
			if(!LinePlane(Line(r.origin(), r.origin() + r.direction()), p, t2))
				return false;
			if(t2 < 0)
				return false;
			t = t2;
			return true;
		}
		bool Intr::LinePlane(const Line& l,const Plane& p,float& t)
		{
			REAL numerator = (p.origin() - l[0]) * p.normal();
			REAL denominator = (l[1] - l[0]) * p.normal();
			if(FABS(denominator) <= EPSILON)
				return false;
			t = numerator / denominator;
			return true;
		}
		bool Intr::LineSegPlane(const Line& l,const Plane& p,float& t)
		{
			REAL t2;
			if(!LinePlane(l, p, t2))
				return false;
			if(t2 < 0 || t2 > 1)
				return false;
			t = t2;
			return true;
		}
		bool Intr::RayTriangle(const Ray& r,const Triangle& tri,float& t)
		{
			REAL t2;
			if(!LineTriangle(Line(r.origin(), r.origin() + r.direction()), tri, t2))
				return false;
			if(t2 < 0)
				return false;
			t = t2;
			return true;
		}
		bool Intr::LineTriangle(const Line& l,const Triangle& tri,float& t)
		{
			Vec3 w;
			Mat3 m(tri[1]-tri[0],tri[2]-tri[0],l[1]-l[0]);
			bool invertible=false;
			Mat3 inv=m.inverse(&invertible);
			if(!invertible)
				return false;
			w=(l[0]-tri[0])*inv;
			if(w[0]<0||w[0]>1||w[1]<0||w[1]>1||w[0]+w[1]>1) // test u, v range
				return false;
			t=-w[2];
			return true;
		}
		bool Intr::LineSegTriangle(const Line& l,const Triangle& tri,float& t)
		{
			REAL t2;
			if(!LineTriangle(l, tri, t2))
				return false;
			if(t2 < 0 || t2 > 1)
				return false;
			t = t2;
			return true;
		}
		bool Intr::RayTriangle(const Ray& r,const TriangleAux& tri,float& t)
		{
			REAL t2;
			if(!LineTriangle(Line(r.origin(), r.origin() + r.direction()), tri, t2))
				return false;
			if(t2 < 0)
				return false;
			t = t2;
			return true;
		}
		bool Intr::LineTriangle(const Line& l, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t)
		{
			Vec3 w;
			Mat3 m(v1 - v0, v2 - v0, l[1] - l[0]);
			bool invertible = false;
			Mat3 inv = m.inverse(&invertible);
			if(!invertible)
				return false;
			w = (l[0] - v0) * inv;
			if(w[0] < 0 || w[0] > 1 || w[1] < 0 || w[1] > 1 || w[0] + w[1] > 1) // test u, v range
				return false;
			t = -w[2];
			return true;
		}
		bool Intr::LineTriangle(const Vec3& p, const Vec3& d,const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t)
		{
			Vec3 w;
			Mat3 m(v1 - v0, v2 - v0, d);
			bool invertible = false;
			Mat3 inv = m.inverse(&invertible);
			if(!invertible)
				return false;
			w = (p - v0) * inv;
			if(w[0] < 0 || w[0] > 1 || w[1] < 0 || w[1] > 1 || w[0] + w[1] > 1) // test u, v range
				return false;
			t = -w[2];
			return true;
		}
		bool Intr::RayTriangle(const Ray& r, const Vec3& v0, const Vec3& v1, const Vec3& v2, REAL& t)
		{
			REAL t2;
			if(!LineTriangle(Line(r.origin(), r.origin() + r.direction()), v0, v1, v2, t2))
				return false;
			if(t2 < 0)
				return false;
			t = t2;
			return true;
		}
		bool Intr::LineTriangle(const Line& l,const TriangleAux& tri,float& t)
		{
			Vec3 w;
			Mat3 m(tri[1] - tri[0], tri[2] - tri[0], l[1] - l[0]);
			bool invertible = false;
			Mat3 inv = m.inverse(&invertible);
			if(!invertible)
				return false;
			w = (l[0] - tri[0]) * inv;
			if(w[0] < 0 || w[0] > 1 || w[1] < 0 || w[1] > 1 || w[0] + w[1] > 1) // test u, v range
				return false;
			t = -w[2];
			return true;
		}
		bool Intr::LineSegTriangle(const Line& l,const TriangleAux& tri,float& t)
		{
			REAL t2;
			if(!LineTriangle(l,tri,t2))
				return false;
			if(t2<0||t2>1)
				return false;
			t = t2;
			return true;
		}
		bool Intr::RaySphere(const Ray& r,const Sphere& s,float& t0,float& t1,int& roots)
		{
			REAL t0_, t1_;
			int roots_;
			if(!LineSphere(Line(r.origin(),r.origin()+r.direction()),s,t0_,t1_,roots_))
				return false;
			if(t0_<0)
				return false;
			if(roots_>1)
			{
				if(t1_<0)
				{
					roots=1;
					t0 = t0_;
					t1 = t1_;
					return true;
				}
			}
			roots = roots_;
			t0 = t0_;
			t1 = t1_;

			return true;
		}
		bool Intr::LineSphere(const Line& l,const Sphere& s,float& t0,float& t1,int& roots)
		{
			Vec3 p,d,q;
			REAL r;
			r=s.radius();
			p=l[0];
			d=l[1]-l[0];
			q=p-s.origin();
			REAL a,b,c;
			a=d*d;
			b=2*q*d;
			c=q*q-r*r;
			REAL delta=b*b-4*a*c;
			if(delta<0)
				return false;
			if(delta==0)
			{
				roots=1;
				t0=-b/(2*a);
				return true;
			}
			roots=2;
			delta=SQRT(delta);
			t0=(-b+delta)/(2*a);
			t1=(-b-delta)/(2*a);
			SORT(t0, t1);
			return true;
		}
		bool Intr::LineSegSphere(const Line& l,const Sphere& s,float& t0,float& t1,int& roots)
		{
			REAL t0_, t1_;
			int roots_;
			if(!LineSphere(l,s,t0_,t1_,roots_))
				return false;
			if(t0_<0||t0_>1)
			{
				if(roots_==1)
					return false;
				if(t1_<0||t1_>1)
					return false;
				roots=1;
				t0=t1_;
				return true;
			}
			if(roots>1)
			{
				if(t1<0||t1>1)
				{
					roots=1;
					t0 = t0_;
					t1 = t1_;
					return true;
				}
			}
			roots=roots_;
			t0 = t0_;
			t1 = t1_;

			return true;
		}
		bool Intr::AABBAABB(const AABB& aabb0,const AABB& aabb1,AABB& intr)
		{
			int i;
			Vec3 min0,max0;
			for(i=0;i<3;i++)
			{
				min0[i]=aabb0.minimum()[i]>aabb1.minimum()[i]?aabb0.minimum()[i]:aabb1.minimum()[i];
				max0[i]=aabb0.maximum()[i]<aabb1.maximum()[i]?aabb0.maximum()[i]:aabb1.maximum()[i];
				if(min0[i]>max0[i])
					return false;
			}
			intr=AABB(min0,max0);
			return true;
		}
		bool Intr::PlanePlane(const Plane& p0,const Plane& p1,Line& l)
		{
			if(p0.normal().isParallel(p1.normal(),EPSILON))
				return false;
			Vec3 n0,n1,cross=n0^n1;
			n0=p0.normal();
			n1=p1.normal();
			Mat3 m(n0,n1,cross);
			bool invertible;
			Mat3 m_inv=m.inverse(&invertible);
			if(!invertible)
				return false;
			Vec3 o=Vec3(-p0[3],-p1[3],0)*m_inv;
			l=Line(o,o+cross);
			return true;
		}
		bool Intr::SphereTriangle(const Sphere& s,const Triangle& tri,Vec3& collinormal,Vec3& collipoint)
		{
			REAL min_len=z3D::Core::INFINITY,len;
			Line l(Vec3::zero,Vec3::zero);
			REAL t;
			Vec3 tri_normal=tri.normal();
			if(LineSegTriangle(l=Line(s.origin()-tri_normal,s.origin()+tri_normal),tri,t))
			{
				Vec3 p=l[0]+t*(l[1]-l[0]);
				min_len=(p-s.origin()).lengthsq();
				collipoint=p;
				collinormal=(p-s.origin()).normal();
			}
			Vec3 d,x;

			l=Line(tri[0],tri[1]);
			d=l[1]-l[0];
			t=((s.origin()-l[0])*d)/(d*d);
			t=min(max(t,(REAL)0),(REAL)1);
			if((len=(s.origin()-(x=(l[0]+t*d))).lengthsq())<min_len)
			{
				min_len=len;
				collipoint=x;
				collinormal=(x-s.origin()).normal();
			}

			l=Line(tri[1],tri[2]);
			d=l[1]-l[0];
			t=((s.origin()-l[0])*d)/(d*d);
			t=min(max(t,(REAL)0),(REAL)1);
			if((len=(s.origin()-(x=(l[0]+t*d))).lengthsq())<min_len)
			{
				min_len=len;
				collipoint=x;
				collinormal=(x-s.origin()).normal();
			}

			l=Line(tri[2],tri[0]);
			d=l[1]-l[0];
			t=((s.origin()-l[0])*d)/(d*d);
			t=min(max(t,(REAL)0),(REAL)1);
			if((len=(s.origin()-(x=(l[0]+t*d))).lengthsq())<min_len)
			{
				min_len=len;
				collipoint=x;
				collinormal=(x-s.origin()).normal();
			}
			if(min_len!=z3D::Core::INFINITY)
				return true;
			return false;
		}
		bool Intr::AABBPlane(const AABB& aabb,const Plane& plane,REAL& depth)
		{
			Vec3 n=plane.normal();
			REAL ext=FABS(aabb.extent()[0]*n[0])+FABS(aabb.extent()[1]*n[1])+FABS(aabb.extent()[2]*n[2]);
			REAL d=plane.sub(aabb.center());
			if(d<=ext)
			{
				depth=ext-d;
				return true;
			}
			return false;
		}
		bool Intr::OBBPlane(const OBB& obb,const Plane& plane,REAL& depth)
		{
			Vec3 n=plane.normal();
			REAL ext=FABS(obb.axis()[0]*n)*obb.extent()[0]+FABS(obb.axis()[1]*n)*obb.extent()[1]+FABS(obb.axis()[2]*n)*obb.extent()[2];
			REAL d=plane.sub(obb.center());
			if(d<=ext)
			{
				depth=ext-d;
				return true;
			}
			return false;
		}
		bool Intr::OBBPlane2Side(const OBB& obb,const Plane& plane,Vec3& normal,REAL& depth)
		{
			REAL d=plane.sub(obb.center());
			if(d>0)
				normal=plane.normal();
			else
			{
				normal=-plane.normal();
				d=-d;
			}

			REAL ext=FABS(obb.axis()[0]*normal)*obb.extent()[0]+FABS(obb.axis()[1]*normal)*obb.extent()[1]+FABS(obb.axis()[2]*normal)*obb.extent()[2];
			if(d<=ext)
			{
				depth=ext-d;
				return true;
			}
			return false;
		}

		bool Intr::LineCone(const Vec3& p, const Vec3& d, const Vec3& v, const Vec3& a, REAL angle, REAL& t0, REAL& t1)
		{
			const Vec3& P = p;
			const Vec3& D = d;
			const Vec3& V = v;
			const Vec3& A = a;
			const REAL cosine2 = SQR(COS(angle));

			const REAL AdotD = A * D;
			const REAL DdotD = D * D;
			const Vec3 P_V = P - V;
			const REAL A_dot_P_V = A * P_V;

			REAL _a = AdotD * AdotD - cosine2 * DdotD;
			REAL _b = 2 * (A_dot_P_V * AdotD - cosine2 * (P_V * D));
			REAL _c = A_dot_P_V * A_dot_P_V - cosine2 * (P_V * P_V);

			if(FABS(_a) < EPSILON)
				return false;

			REAL delta = _b * _b - 4 * _a * _c;
			if(delta < 0.0f)
				return false;

			REAL delta_sqrt = SQRT(delta);
			REAL _1_2a = 1.0f / (2.0f * _a);

			t0 = (-_b - delta_sqrt) * _1_2a;
			t1 = (-_b + delta_sqrt) * _1_2a;

			// get rid of the reflected cone
			if(A * ((P + t0 * D) - V) < 0.0f)
				t0 = INFINITY;
			if(A * ((P + t1 * D) - V) < 0.0f)
				t1 = INFINITY;

			SORT(t0, t1);

			if(t0 == INFINITY)
				return false;

			return true;
		}
		bool Intr::RayConeFull(const Vec3& p, const Vec3& d, const Vec3& v, const Vec3& a, REAL angle, REAL h, REAL& t)
		{
			REAL t0, t1, t2;
			if(!LineCone(p, d, v, a, angle, t0, t1))
				t0 = t1 = INFINITY;
			else
			{
				if(t0 < 0.0f || (p + t0 * d - v) * a > h)
					t0 = INFINITY;
				if(t1 < 0.0f || (t1 != INFINITY && (p + t1 * d - v) * a > h))
					t1 = INFINITY;
			}

			if(true)
			{
				REAL DdotN = d * a;
				if(FABS(DdotN) < EPSILON)
					t2 = INFINITY;
				else
				{
					const Vec3 center = v + a * h;
					t2 = ((center - p) * a) / DdotN;
					if(t2 < 0.0f || (p + t2 * d - center).lengthsq() > SQR(h * TAN(angle)))
						t2 = INFINITY;
				}
			}
			SORT(t1, t2);
			SORT(t0, t1);
			if(t0 != INFINITY)
				t = t0;
			else if(t1 != INFINITY)
				t = t1;
			else if(t2 != INFINITY)
				t = t2;
			else
				return false;

			return true;
		}

		bool Intr::RayCylinderFull(const Vec3& p, const Vec3& d, const Vec3& c, const Vec3& w, REAL r, REAL h, REAL& t)
		{
			const Vec3& P = p;
			const Vec3& D = d;
			const Vec3& C = c;
			const Vec3& W = w;

			const Vec3 P_C = P - C;

			Vec3 m = D - (D * W) * W;
			Vec3 n = P_C - (P_C * W) * W;

			REAL _a = m * m;
			REAL _b = 2.0f * m * n;
			REAL _c = n * n - r * r;

			if(FABS(_a) < EPSILON)
				return false;

			REAL delta = _b * _b - 4 * _a * _c;
			if(delta < 0.0f)
				return false;

			REAL delta_sqrt = SQRT(delta);
			REAL _1_2a = 1.0f / (2.0f * _a);

			REAL t0 = (-_b - delta_sqrt) * _1_2a;
			REAL t1 = (-_b + delta_sqrt) * _1_2a;

			SORT(t0, t1);
			if(t0 >= 0.0f)
				t = t0;
			else if(t1 >= 0.0f)
				t = t1;
			else
				return false;

			Vec3 X = P + t * D;
			REAL s = (X - C) * W;
			if(s < 0.0f)
			{
				REAL DdotN = d * w;
				if(FABS(DdotN) < EPSILON)
					return false;
				else
				{
					const Vec3 center = c;
					REAL t2 = ((center - p) * w) / DdotN;
					if(t2 < 0.0f || (p + t2 * d - center).lengthsq() > r * r)
						return false;
					t = t2;
				}
			}
			else if(s > 1.0f)
			{
				REAL DdotN = d * w;
				if(FABS(DdotN) < EPSILON)
					return false;
				else
				{
					const Vec3 center = c + w * h;
					REAL t2 = ((center - p) * w) / DdotN;
					if(t2 < 0.0f || (p + t2 * d - center).lengthsq() > r * r)
						return false;
					t = t2;
				}
			}

			return true;
		}

		bool Intr::OBBTriangle(const OBB& obb,const TriangleAux& tri,Vec3& normal,REAL& depth)
		{
			const Vec3& obb_axis0=obb.axis(0);
			const Vec3& obb_axis1=obb.axis(1);
			const Vec3& obb_axis2=obb.axis(2);
			const Vec3& obb_ext=obb.extent();
			const Vec3& obb_center=obb.center();
			Vec3& obb_eaxis0=obb_axis0*obb_ext[0];
			Vec3& obb_eaxis1=obb_axis1*obb_ext[1];
			Vec3& obb_eaxis2=obb_axis2*obb_ext[2];

			int i;
			Vec3 axis[13],*axis_ptr;

			const Vec3& tri_vert0=tri[0];
			const Vec3& tri_vert1=tri[1];
			const Vec3& tri_vert2=tri[2];

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

			depth=INFINITY;

			REAL min0,max0,min1,max1;
			axis_ptr=axis;
			for(i=0;i<13;++i,++axis_ptr)
			{
				Vec3& cur_axis=*axis_ptr;
				if(cur_axis.lengthsq()<0.0001f)
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
				REAL d=(max0-min0+max1-min1)*0.5f-FABS((max0+min0)*0.5f-(max1+min1)*0.5f);
				REAL inv_axis_length=1/cur_axis.length();
				cur_axis*=inv_axis_length;
				d*=inv_axis_length;
				if(d<depth)
				{
					depth=d;
					normal=cur_axis;
				}
			}
			return true;
		}
	};
};