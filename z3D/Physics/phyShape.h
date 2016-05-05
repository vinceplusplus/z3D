
#pragma once

#include "predecl.h"

class btCollisionShape;
class btTriangleMesh;

namespace z3D
{
	namespace SG
	{
		class Mesh;
	};
};

namespace z3D
{
	namespace Physics
	{
		class phyShape: public RefBase
		{
		protected:
			btCollisionShape*			_btshape;
			//btCollisionShape*			_btchild;
			vector<btCollisionShape*>	_btchildren;
			btTriangleMesh*				_btmesh;
		protected:
			phyShape();
			~phyShape();
		private:
			static SPtr<phyShape>		createBox(const Vec3& halflength);
			static SPtr<phyShape>		createSphere(REAL radius);
			static SPtr<phyShape>		createMesh(const SPtr<z3D::SG::Mesh>& mesh);
			static SPtr<phyShape>		createConvexHull(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor);
			static SPtr<phyShape>		createDecompConvexHull(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor);
			// for creating real compound collision shape, add a function that accepts an array of child shapes
		public:
		public:
			static SPtr<phyShape>		createCompoundWrappedBox(const Vec3& halflength, const Vec3& offset);
		public:
			btCollisionShape*			get_btCollisionShape() const;
		public:
			REAL						getMargin() const;
			void						setMargin(REAL margin);
		public:
			friend class phyBody;
			friend class phyMgr;
		};
	};
};

