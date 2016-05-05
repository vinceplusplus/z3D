
#include "phyMgr.h"
#include "phyBody.h"
#include "phyShape.h"
#include "btHelper.h"

#include "../SG/SNode.h"
#include "../SG/SMeshNode.h"

#include "../Core/z_off_debug_new.h"
#include "../../bullet/btBulletDynamicsCommon.h"
#include "../Core/z_on_debug_new.h"

namespace z3D
{
	namespace Physics
	{
		static btVector3			CONVERT(const Vec3& v)
		{
			return btVector3(v.x, v.y, v.z);
		}
		static Vec3					CONVERT(const btVector3& v)
		{
			return Vec3(v.x(), v.y(), v.z());
		}
		phyMgr::phyMgr()
		{
			_collision_configuration = new btDefaultCollisionConfiguration();
			_dispatcher = new btCollisionDispatcher(_collision_configuration);
#include "../Core/z_off_debug_new.h"
			_broad_phase = new btAxisSweep3(btVector3(-10000, -10000, -10000), btVector3(10000, 10000, 10000));
#include "../Core/z_on_debug_new.h"
			_solver = new btSequentialImpulseConstraintSolver();
			_dynamics_world = new btDiscreteDynamicsWorld(_dispatcher, _broad_phase, _solver, _collision_configuration);

			setGravity(Vec3(0, -10, 0));
		}
		phyMgr::~phyMgr()
		{
			removeAllBodies();
			delete _dynamics_world;
			delete _solver;
			delete _broad_phase;
			delete _dispatcher;
			delete _collision_configuration;
		}
		void						phyMgr::setGravity(const Vec3& gravity)
		{
			_dynamics_world->setGravity(CONVERT(gravity));
		}
		Vec3						phyMgr::getGravity() const
		{
			return CONVERT(_dynamics_world->getGravity());
		}
		SPtr<phyBody>				phyMgr::createBody(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& offset, REAL mass, bool skip_calc_inertia)
		{
			return phyBody::create(node, shape, offset, mass, skip_calc_inertia);
		}
		SPtr<phyBody>				phyMgr::createBody(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& offset, REAL mass, const Vec3& inertia_override)
		{
			return phyBody::create(node, shape, offset, mass, inertia_override);
		}
		SPtr<phyShape>				phyMgr::createBoxShape(const Vec3& halflength)
		{
			return phyShape::createBox(halflength);
		}
		SPtr<phyShape>				phyMgr::createSphereShape(REAL radius)
		{
			return phyShape::createSphere(radius);
		}
		SPtr<phyShape>				phyMgr::createMeshShape(const SPtr<z3D::SG::Mesh>& mesh)
		{
			return phyShape::createMesh(mesh);
		}
		SPtr<phyShape>				phyMgr::createConvexHullShape(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor)
		{
			return phyShape::createConvexHull(mesh, offset, mass, inertia_tensor);
		}
		SPtr<phyShape>				phyMgr::createCompoundWrappedBoxShape(const Vec3& halflength, const Vec3& offset)
		{
			return phyShape::createCompoundWrappedBox(halflength, offset);
		}
		SPtr<phyShape>				phyMgr::createDecompConvexHullShape(const SPtr<z3D::SG::Mesh>& mesh, Vec3& offset, REAL& mass, Mat3& inertia_tensor)
		{
			return phyShape::createDecompConvexHull(mesh, offset, mass, inertia_tensor);
		}
		void						phyMgr::addBody(const SPtr<phyBody>& body)
		{
			_dynamics_world->addRigidBody(&body->_btbody.get());
			_array_body.push_back(body);
		}
		void						phyMgr::removeBody(const SPtr<phyBody>& body)
		{
			for(size_t i = 0; i < _array_body.size(); ++i)
			{
				if(_array_body[i] == body)
				{
					_dynamics_world->removeRigidBody(&body->_btbody.get());
					_array_body.erase(_array_body.begin() + i);

					return;
				}
			}
		}
		void						phyMgr::step(REAL dt)
		{
			_dynamics_world->stepSimulation(dt, 2);
		}
		void						phyMgr::removeAllBodies()
		{
			while(_array_body.size())
				removeBody(_array_body[0]);
		}
		void						phyMgr::parseUserText(const wcs& node_name, const SPtr<z3D::SG::SNode>& node, const wcs& user_text)
		{
			Config* cfg = Config::fromWCS(user_text);
			if(cfg->exists(L"collision_shape"))
			{
				wcs shape_name = cfg->getWString(L"collision_shape");
				REAL mass = cfg->getFloat(L"mass");
				int32_t compound = cfg->getInt32(L"compound");

				if(shape_name == L"box")
				{
					if(compound)
					{
						SPtr<phyShape> shape = createCompoundWrappedBoxShape(node->local_bound().extent(), node->local_bound().center());
						SPtr<phyBody> body = createBody(node, shape, Mat4::identity, mass, false);
						addBody(body);

						node->setPhyBody(body);
					}
					else
					{
						SPtr<phyShape> shape = createBoxShape(node->local_bound().extent());
						SPtr<phyBody> body = createBody(node, shape, Mat4::translation(Vec3(node->local_bound().center())), mass, false);
						addBody(body);

						node->setPhyBody(body);
					}
				}
				else if(shape_name == L"sphere")
				{
					Vec3 ext = node->local_bound().extent();
					SPtr<phyShape> shape = createSphereShape((ext[0] + ext[1] + ext[2]) / 3);
					SPtr<phyBody> body = createBody(node, shape, Mat4::translation(Vec3(node->local_bound().center())), mass, false);
					addBody(body);

					node->setPhyBody(body);
				}
				else if(shape_name == L"mesh")
				{
					if(node->type_info()->kind_of(z3D::SG::SMeshNode::type_info_static()))
					{
						SPtr<phyShape> shape = createMeshShape(node.cast<z3D::SG::SMeshNode>()->mesh());
						SPtr<phyBody> body = createBody(node, shape, Mat4::identity, mass, true);
						addBody(body);

						node->setPhyBody(body);
					}
				}
				else if(shape_name == L"convex_hull")
				{
					if(node->type_info()->kind_of(z3D::SG::SMeshNode::type_info_static()))
					{
						Vec3 offset;
						REAL computed_mass;
						Mat3 inertia_tensor;
						SPtr<phyShape> shape = createConvexHullShape(node.cast<z3D::SG::SMeshNode>()->mesh(), offset, computed_mass, inertia_tensor);
						SPtr<phyBody> body = createBody(node, shape, Mat4::translation(offset), mass, mass / computed_mass * Vec3(inertia_tensor[0][0], inertia_tensor[1][1], inertia_tensor[2][2]));

						addBody(body);

						node->setPhyBody(body);
					}
				}
				else if(shape_name == L"convex_decomp")
				{
					if(node->type_info()->kind_of(z3D::SG::SMeshNode::type_info_static()))
					{
						Vec3 offset;
						REAL computed_mass;
						Mat3 inertia_tensor;
						SPtr<phyShape> shape = createDecompConvexHullShape(node.cast<z3D::SG::SMeshNode>()->mesh(), offset, computed_mass, inertia_tensor);
						SPtr<phyBody> body = createBody(node, shape, Mat4::translation(offset), mass, mass / computed_mass * Vec3(inertia_tensor[0][0], inertia_tensor[1][1], inertia_tensor[2][2]));

						addBody(body);

						node->setPhyBody(body);
					}
				}
			}
			delete cfg;
		}
		btCollisionConfiguration*	phyMgr::get_btCollisionConfiguration() const
		{
			return _collision_configuration;
		}
		btCollisionDispatcher*		phyMgr::get_btCollisionDispatcher() const
		{
			return _dispatcher;
		}
		btBroadphaseInterface*		phyMgr::get_btBroadphaseInterface() const
		{
			return _broad_phase;
		}
		btConstraintSolver*			phyMgr::get_btConstraintSolver() const
		{
			return _solver;
		}
		btDynamicsWorld*			phyMgr::get_btDynamicsWorld() const
		{
			return _dynamics_world;
		}
	};
};


