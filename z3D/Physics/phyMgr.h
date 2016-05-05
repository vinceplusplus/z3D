
#pragma once

#include "predecl.h"

class btCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class btDynamicsWorld;

namespace z3D
{
	namespace SG
	{
		class SNode;
		class Mesh;
	};
};

namespace z3D
{
	namespace Physics
	{
		class phyMgr: public RefBase
		{
		protected:
			btCollisionConfiguration*	_collision_configuration;
			btCollisionDispatcher*		_dispatcher;
			btBroadphaseInterface*		_broad_phase;
			btConstraintSolver*			_solver;
			btDynamicsWorld*			_dynamics_world;
		protected:
			vector< SPtr<phyBody> >		_array_body;
		public:
			phyMgr();
		protected:
			~phyMgr();
		public:
			void						setGravity(const Vec3& gravity);
			Vec3						getGravity() const;
		public:
			SPtr<phyBody>				createBody(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& offset, REAL mass, bool skip_calc_inertia);
			SPtr<phyBody>				createBody(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& offset, REAL mass, const Vec3& inertia_override);
		public:
			SPtr<phyShape>				createBoxShape(const Vec3& halflength);
			SPtr<phyShape>				createSphereShape(REAL radius);
			SPtr<phyShape>				createMeshShape(const SPtr<z3D::SG::Mesh>& mesh);
			SPtr<phyShape>				createConvexHullShape(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor);
		public:
			SPtr<phyShape>				createCompoundWrappedBoxShape(const Vec3& halflength, const Vec3& offset);
		public:
			SPtr<phyShape>				createDecompConvexHullShape(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor);
		public:
			void						addBody(const SPtr<phyBody>& body);
			void						removeBody(const SPtr<phyBody>& body);
		public:
			void						step(REAL dt);
		public:
			void						removeAllBodies();
		public:
			void						parseUserText(const wcs& node_name, const SPtr<z3D::SG::SNode>& node, const wcs& user_text);
		public:
			btCollisionConfiguration*	get_btCollisionConfiguration() const;
			btCollisionDispatcher*		get_btCollisionDispatcher() const;
			btBroadphaseInterface*		get_btBroadphaseInterface() const;
			btConstraintSolver*			get_btConstraintSolver() const;
			btDynamicsWorld*			get_btDynamicsWorld() const;
		};
	};
};


