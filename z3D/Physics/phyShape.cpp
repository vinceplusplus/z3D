
#include "phyShape.h"

#include "../Core/allocators_inl.h"

#include "btHelper.h"

#include "../SG/Mesh.h"

#include "../Core/z_off_debug_new.h"
#include "../../bullet/btBulletDynamicsCommon.h"
#include "../../bullet/Extras/ConvexDecomposition/ConvexBuilder.h"
//#include "../../bullet/BulletCollision/CollisionShapes/btShapeHull.h"
#include "../../bullet/LinearMath/btConvexHull.h"
#include "../Core/z_on_debug_new.h"

using namespace z3D::SG;

namespace z3D
{
	namespace Physics
	{
		static btVector3			CONVERT(const Vec3& v)
		{
			return btVector3(v.x, v.y, v.z);
		}

		class ComputeMassProp
		{
		public:
			// taken from http://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdf
			template<size_t STRIDING>
			static void compute(const void* verts, const unsigned int* indices, size_t face_count, REAL& mass, Vec3& com, Mat3& inertia)
			{
				const REAL _1over6 = (REAL)(1.0/6.0);
				const REAL _1over24 = (REAL)(1.0/24.0);
				const REAL _1over60 = (REAL)(1.0/60.0);
				const REAL _1over120 = (REAL)(1.0/120.0);

				// order:  1, x, y, z, x^2, y^2, z^2, xy, yz, zx
				REAL integral[10] = { (REAL)0.0, (REAL)0.0, (REAL)0.0, (REAL)0.0,
					(REAL)0.0, (REAL)0.0, (REAL)0.0, (REAL)0.0, (REAL)0.0, (REAL)0.0 };

				const unsigned int* pi = indices - 1;
				for(size_t i = 0; i < face_count; i++)
				{
					// get vertices of triangle i
					const Vec3& v0 = *(Vec3*)((const char*)verts + (*(++pi)) * STRIDING);
					const Vec3& v1 = *(Vec3*)((const char*)verts + (*(++pi)) * STRIDING);
					const Vec3& v2 = *(Vec3*)((const char*)verts + (*(++pi)) * STRIDING);

					// get cross product of edges
					Vec3 n = (v1 - v0) ^ (v2 - v0);

					// compute integral terms
					REAL tmp0, tmp1, tmp2;
					REAL f1x, f2x, f3x, g0x, g1x, g2x;
					tmp0 = v0.x + v1.x;
					f1x = tmp0 + v2.x;
					tmp1 = v0.x * v0.x;
					tmp2 = tmp1 + v1.x * tmp0;
					f2x = tmp2 + v2.x * f1x;
					f3x = v0.x * tmp1 + v1.x * tmp2 + v2.x * f2x;
					g0x = f2x + v0.x * (f1x + v0.x);
					g1x = f2x + v1.x * (f1x + v1.x);
					g2x = f2x + v2.x * (f1x + v2.x);

					REAL f1y, f2y, f3y, g0y, g1y, g2y;
					tmp0 = v0.y + v1.y;
					f1y = tmp0 + v2.y;
					tmp1 = v0.y * v0.y;
					tmp2 = tmp1 + v1.y * tmp0;
					f2y = tmp2 + v2.y * f1y;
					f3y = v0.y * tmp1 + v1.y * tmp2 + v2.y * f2y;
					g0y = f2y + v0.y * (f1y + v0.y);
					g1y = f2y + v1.y * (f1y + v1.y);
					g2y = f2y + v2.y * (f1y + v2.y);

					REAL f1z, f2z, f3z, g0z, g1z, g2z;
					tmp0 = v0.z + v1.z;
					f1z = tmp0 + v2.z;
					tmp1 = v0.z * v0.z;
					tmp2 = tmp1 + v1.z * tmp0;
					f2z = tmp2 + v2.z * f1z;
					f3z = v0.z * tmp1 + v1.z * tmp2 + v2.z * f2z;
					g0z = f2z + v0.z * (f1z + v0.z);
					g1z = f2z + v1.z * (f1z + v1.z);
					g2z = f2z + v2.z * (f1z + v2.z);

					// update integrals
					integral[0] += n.x * f1x;
					integral[1] += n.x * f2x;
					integral[2] += n.y * f2y;
					integral[3] += n.z * f2z;
					integral[4] += n.x * f3x;
					integral[5] += n.y * f3y;
					integral[6] += n.z * f3z;
					integral[7] += n.x * (v0.y * g0x + v1.y * g1x + v2.y * g2x);
					integral[8] += n.y * (v0.z * g0y + v1.z * g1y + v2.z * g2y);
					integral[9] += n.z * (v0.x * g0z + v1.x * g1z + v2.x * g2z);
				}

				integral[0] *= _1over6;
				integral[1] *= _1over24;
				integral[2] *= _1over24;
				integral[3] *= _1over24;
				integral[4] *= _1over60;
				integral[5] *= _1over60;
				integral[6] *= _1over60;
				integral[7] *= _1over120;
				integral[8] *= _1over120;
				integral[9] *= _1over120;

				// mass
				mass = integral[0];

				// center of mass
				com = Vec3(integral[1], integral[2], integral[3]) / mass;

				// inertia relative to world origin
				inertia[0][0] = integral[5] + integral[6];
				inertia[0][1] = -integral[7];
				inertia[0][2] = -integral[9];
				inertia[1][0] = inertia[0][1];
				inertia[1][1] = integral[4] + integral[6];
				inertia[1][2] = -integral[8];
				inertia[2][0] = inertia[0][2];
				inertia[2][1] = inertia[1][2];
				inertia[2][2] = integral[4] + integral[5];

				// inertia relative to center of mass
				inertia[0][0] -= mass * (com.y * com.y + com.z * com.z);
				inertia[0][1] += mass * com.x * com.y;
				inertia[0][2] += mass * com.z * com.x;
				inertia[1][0] = inertia[0][1];
				inertia[1][1] -= mass * (com.z * com.z + com.x * com.x);
				inertia[1][2] += mass * com.y * com.z;
				inertia[2][0] = inertia[0][2];
				inertia[2][1] = inertia[1][2];
				inertia[2][2] -= mass * (com.x * com.x + com.y * com.y);
			}
		};
		phyShape::phyShape()
		{
			_btshape = NULL;
			_btmesh = NULL;
		}
		phyShape::~phyShape()
		{
			if(_btshape)
				delete _btshape;
			for(size_t i = 0; i < _btchildren.size(); ++i)
				delete _btchildren[i];
			if(_btmesh)
				delete _btmesh;
		}
		SPtr<phyShape>		phyShape::createBox(const Vec3& halflength)
		{
#include "../Core/z_off_debug_new.h"
			btCollisionShape* btshape = new btBoxShape(btVector3(halflength.x, halflength.y, halflength.z));
#include "../Core/z_on_debug_new.h"
			phyShape* shape = new phyShape();
			shape->_btshape = btshape;
			return shape;
		}
		SPtr<phyShape>		phyShape::createSphere(REAL radius)
		{
#include "../Core/z_off_debug_new.h"
			btCollisionShape* btshape = new btSphereShape(radius);
#include "../Core/z_on_debug_new.h"
			phyShape* shape = new phyShape();
			shape->_btshape = btshape;
			return shape;
		}
		SPtr<phyShape>		phyShape::createMesh(const SPtr<z3D::SG::Mesh>& mesh)
		{
			const vnFace* faces = mesh->get_faces();
			size_t face_count = mesh->get_face_count();
			const Vec3* verts = mesh->get_verts();

#include "../Core/z_off_debug_new.h"
			btTriangleMesh* btmesh = new btTriangleMesh();
#include "../Core/z_on_debug_new.h"
			for(size_t i = 0; i < face_count; ++i)
			{
				const vnFace& f = faces[i];
				const Vec3& v0 = verts[f.v[0]];
				const Vec3& v1 = verts[f.v[1]];
				const Vec3& v2 = verts[f.v[2]];
				btmesh->addTriangle(btVector3(v0.x, v0.y, v0.z), btVector3(v1.x, v1.y, v1.z), btVector3(v2.x, v2.y, v2.z));
			}
#include "../Core/z_off_debug_new.h"
			btCollisionShape* btshape = new btBvhTriangleMeshShape(btmesh,true);
#include "../Core/z_on_debug_new.h"
			phyShape* shape = new phyShape();
			shape->_btshape = btshape;
			shape->_btmesh = btmesh;
			return shape;
		}
		SPtr<phyShape>		phyShape::createConvexHull(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor)
		{
			Vec3 com;

			vector<Vec3, frame_temp_allocator<Vec3>> verts_com;					// TODO: optimize

			if(true)
			{
				const Vec3* verts = mesh->get_verts();
				size_t vert_count = mesh->get_vert_count();

				HullDesc hd;
				hd.mFlags = QF_TRIANGLES;
				hd.mVcount = (unsigned int)vert_count;
				hd.mVertices = (const btVector3*)verts;
				hd.mVertexStride = (unsigned int)sizeof(Vec3);

				HullLibrary hl;
				HullResult hr;
				if(hl.CreateConvexHull(hd, hr) == QE_FAIL)
				{
					Z_ASSERT( false );
					return NULL;
				}

				ComputeMassProp::compute<sizeof(btVector3)>(&hr.m_OutputVertices[0], &hr.m_Indices[0], hr.mNumFaces, mass, com, inertia_tensor);

				verts_com.resize((size_t)hr.mNumOutputVertices);
				for(size_t i = 0; i < (size_t)hr.mNumOutputVertices; ++i)
					verts_com[i] = (*(Vec3*)(&hr.m_OutputVertices[0] + i)) - com;

				hl.ReleaseResult(hr);
			}

#include "../Core/z_off_debug_new.h"
			btConvexHullShape* btshape = new btConvexHullShape((const btScalar*)VECTOR_BEGIN_PTR(verts_com), (unsigned int)verts_com.size(), sizeof(verts_com[0]));
#include "../Core/z_on_debug_new.h"
			phyShape* shape = new phyShape();
			shape->_btshape = btshape;

			offset = com;

			return shape;
		}
		SPtr<phyShape>		phyShape::createDecompConvexHull(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor)
		{
			class MyConvexDecomposition : public ConvexDecomposition::ConvexDecompInterface
			{
			public:
				vector<btCollisionShape*>&		btchildren;
				Vec3																	acc_mcom;
				REAL																	acc_mass;
				REAL																	acc_volume;
				vector<REAL, frame_temp_allocator<REAL>>								masses;
				vector<Vec3, frame_temp_allocator<Vec3>>								coms;
				vector<Mat3, frame_temp_allocator<Mat3>>								inertia_tensors;
			public:
				MyConvexDecomposition(vector<btCollisionShape*>& btchildren_)
					: btchildren(btchildren_)
				{
					acc_mass = 0.0f;
					acc_volume = 0.0f;
				}
			public:
				void ConvexDecompResult(ConvexDecomposition::ConvexResult &result)
				{
					REAL mass;
					Vec3 com;
					Mat3 inertia_tensor;

					ComputeMassProp::compute<sizeof(float[3])>(result.mHullVertices, result.mHullIndices, (size_t)result.mHullTcount, mass, com, inertia_tensor);
					acc_mass += mass;
					acc_mcom += mass * com;

					acc_volume +=result.mHullVolume;

#include "../Core/z_off_debug_new.h"
					btConvexHullShape* shape = new btConvexHullShape(result.mHullVertices, result.mHullVcount, sizeof(float[3]));
#include "../Core/z_on_debug_new.h"
					btchildren.push_back(shape);
					masses.push_back(mass);
					coms.push_back(com);
					inertia_tensors.push_back(inertia_tensor);
				}
			public:
				Mat3							outer_product(const Vec3& a, const Vec3& b)
				{
					return Mat3(a.x * b, a.y * b, a.z * b);
				}
			};

			SPtr<phyShape> shape = new phyShape();
			MyConvexDecomposition cb(shape->_btchildren);

			ConvexDecomposition::DecompDesc desc;
			desc.mVcount = (unsigned int)mesh->get_vert_count();
			desc.mVertices = (const float*)mesh->get_verts();
			desc.mTcount = (unsigned int)mesh->get_face_count();
			Z_STATIC_ASSERT( sizeof(size_t) == sizeof(unsigned int), WRONG_ASSUMPTION );
			vector<size_t, frame_temp_allocator<size_t>> indices;
			if(true)
			{
				const vnFace* faces = mesh->get_faces();
				size_t face_count = mesh->get_face_count();
				for(size_t i = 0; i < face_count; ++i)
				{
					indices.push_back(faces[i].v[0]);
					indices.push_back(faces[i].v[1]);
					indices.push_back(faces[i].v[2]);
				}
			}
			desc.mIndices = (unsigned int*)VECTOR_BEGIN_PTR(indices);
			desc.mDepth = 5;
			desc.mCpercent = 5;
			desc.mPpercent = 15;
			//desc.mMaxVertices = 32;
			desc.mMaxVertices = (unsigned int)mesh->get_face_count();
			desc.mSkinWidth = 0.0f;
			desc.mCallback = &cb;

			ConvexBuilder builder(&cb);
			builder.process(desc);

#include "../Core/z_off_debug_new.h"
			btCompoundShape* btshape = new btCompoundShape();
#include "../Core/z_on_debug_new.h"
			Vec3 o = cb.acc_mcom / cb.acc_mass;
			Mat3 it = Mat3::identity;

			for(size_t i = 0; i < cb.btchildren.size(); ++i)
			{
				btTransform m;
				btHelper::to(Mat4::translation(-o), m);
				btshape->addChildShape(m, cb.btchildren[i]);
				Vec3 r = cb.coms[i] - o;
				it += cb.inertia_tensors[i] + r.lengthsq() * Mat3::identity - cb.outer_product(r, r) * cb.masses[i];
			}

			offset = o;
			mass = cb.acc_mass;
			inertia_tensor = it;

			shape->_btshape = btshape;

			return shape;
		}
		SPtr<phyShape>		phyShape::createCompoundWrappedBox(const Vec3& halflength, const Vec3& offset)
		{
#include "../Core/z_off_debug_new.h"
			btCollisionShape* btshape = new btBoxShape(btVector3(halflength.x, halflength.y, halflength.z));
			btCompoundShape* btcompound = new btCompoundShape();
#include "../Core/z_on_debug_new.h"

			btTransform m;
			btHelper::to(Mat4::translation(offset), m);
			btcompound->addChildShape(m, btshape);

			phyShape* shape = new phyShape();
			shape->_btshape = btcompound;
			shape->_btchildren.push_back(btshape);
			return shape;
		}
		btCollisionShape*			phyShape::get_btCollisionShape() const
		{
			return _btshape;
		}
		REAL						phyShape::getMargin() const
		{
			return _btshape->getMargin();
		}
		void						phyShape::setMargin(REAL margin)
		{
			_btshape->setMargin(margin);
		}
	};
};

