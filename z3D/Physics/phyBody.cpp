
#include "phyBody.h"

#include "../Core/aligned_storage_inl.h"
#include "phyShape.h"
#include "btHelper.h"

#include "../SG/SNode.h"

#include "../Core/z_off_debug_new.h"
#include "../../bullet/btBulletDynamicsCommon.h"
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
		static Vec3					CONVERT(const btVector3& v)
		{
			return Vec3(v.x(), v.y(), v.z());
		}
		static btQuaternion			CONVERT(const Quat& v)
		{
			return btQuaternion(v.x, v.y, v.z, v.w);
		}
		static Quat					CONVERT(const btQuaternion& v)
		{
			return Quat(v.x(), v.y(), v.z(), v.w());
		}
		static btTransform			CONVERT(const Mat4& m)
		{
			btTransform m2;
			btHelper::to(m, m2);
			return m2;
		}
		static Mat4					CONVERT(const btTransform& m)
		{
			Mat4 m2;
			btHelper::to(m, m2);
			return m2;
		}
		static btMatrix3x3			CONVERT(const Mat3& m)
		{
			btMatrix3x3 m2;
			btHelper::to(m, m2);
			return m2;
		}
		static Mat3					CONVERT(const btMatrix3x3& m)
		{
			Mat3 m2;
			btHelper::to(m, m2);
			return m2;
		}
		class phyBody::phybtMotionState: public btMotionState
		{
		public:
			WPtr<SNode>				node;
			Mat4					offset;
			Mat4					offset_inv;
		public:
			void					getWorldTransform(btTransform& worldTrans) const
			{
				if(!node.valid())
					return;
				btHelper::to(offset * node.get_unsafe()->world_trans(), worldTrans);
			}
			void					setWorldTransform(const btTransform& worldTrans)
			{
				if(!node.valid())
					return;
				Mat4 m;
				btHelper::to(worldTrans, m);
				node.get_unsafe()->setWorldTransform(offset_inv * m);
			}
		};

		phyBody::phyBody()
		{
			((phybtMotionState*)_ms.allocate())->phybtMotionState::phybtMotionState();
		}
		phyBody::~phyBody()
		{
		}
		SPtr<phyBody>		phyBody::create(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& com_xform, REAL mass, bool skip_calc_inertia)
		{
			btVector3 inertia(0, 0, 0);
			if(!skip_calc_inertia)
				shape->_btshape->calculateLocalInertia(mass, inertia);
			else
			{
				inertia.setX(0);
				inertia.setY(0);
				inertia.setZ(0);
			}

			return create(node, shape, com_xform, mass, Vec3(inertia.x(), inertia.y(), inertia.z()));
		}
		SPtr<phyBody>		phyBody::create(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& com_xform, REAL mass, const Vec3& inertia_override)
		{
			SPtr<phyBody> body = new phyBody();
			phybtMotionState* ms = &body->_ms.get();
			ms->node = node;
			ms->offset = com_xform;
			ms->offset_inv = com_xform.inverse();

			btVector3 inertia(inertia_override.x, inertia_override.y, inertia_override.z);

			((btRigidBody*)body->_btbody.allocate())->btRigidBody::btRigidBody(mass, &body->_ms.get(), shape->_btshape, inertia);

			body->_shape = shape;

			return body;
		}
		const SPtr<phyShape>&		phyBody::getShape() const
		{
			return _shape;
		}
		SPtr<z3D::SG::SNode>		phyBody::getNode() const
		{
			return _ms.get().node.lock();
		}
		btRigidBody*				phyBody::get_btRigidBody() const
		{
			return (btRigidBody*)&_btbody.get();
		}
		void						phyBody::setActivationState(ACTIVATION_STATE as)
		{
			_btbody.get().setActivationState((int)as);
		}
		phyBody::ACTIVATION_STATE	phyBody::getActivationState() const
		{
			return (phyBody::ACTIVATION_STATE)_btbody.get().getActivationState();
		}
		void						phyBody::setDeactivationTime(REAL t)
		{
			_btbody.get().setDeactivationTime(t);
		}
		REAL						phyBody::getDeactivationTime() const
		{
			return _btbody.get().getDeactivationTime();
		}
		void						phyBody::forceActivationState(ACTIVATION_STATE new_as)
		{
			_btbody.get().forceActivationState((int)new_as);
		}
		void						phyBody::activate(bool force_activation)
		{
			_btbody.get().activate(force_activation);
		}
		bool						phyBody::isActive() const
		{
			return _btbody.get().isActive();
		}

		void						phyBody::setRestitution(REAL restitution)
		{
			_btbody.get().setRestitution(restitution);
		}
		REAL						phyBody::getRestitution() const
		{
			return _btbody.get().getRestitution();
		}
		void						phyBody::setFriction(REAL friction)
		{
			_btbody.get().setFriction(friction);
		}
		REAL						phyBody::getFriction() const
		{
			return _btbody.get().getFriction();
		}
		void						phyBody::setWorldTransform(const Mat4& world_trans)
		{
			btTransform m;
			btHelper::to(world_trans, m);
			_btbody.get().setWorldTransform(m);
		}
		Mat4						phyBody::getWorldTransform() const
		{
			const btTransform& world_trans = _btbody.get().getWorldTransform();
			Mat4 m;
			btHelper::to(world_trans, m);
			return m;
		}
		void						phyBody::setHitFraction(REAL hit_fraction)
		{
			_btbody.get().setHitFraction(hit_fraction);
		}
		REAL						phyBody::getHitFraction() const
		{
			return _btbody.get().getHitFraction();
		}
		void						phyBody::setGravity(const Vec3& gravity)
		{
			_btbody.get().setGravity(CONVERT(gravity));
		}
		Vec3						phyBody::getGravity() const
		{
			return CONVERT(_btbody.get().getGravity());
		}
		void						phyBody::setDamping(REAL linear_damping, REAL angular_damping)
		{
			_btbody.get().setDamping(linear_damping, angular_damping);
		}
		REAL						phyBody::getLinearDamping() const
		{
			return _btbody.get().getLinearDamping();
		}
		REAL						phyBody::getAngularDamping() const
		{
			return _btbody.get().getAngularDamping();
		}
		void						phyBody::setMassProps(REAL mass, const Vec3& inertia)
		{
			_btbody.get().setMassProps(mass, CONVERT(inertia));
		}
		REAL						phyBody::getInvMass() const
		{
			return _btbody.get().getInvMass();
		}
		Mat3						phyBody::getInvInertiaTensorWorld() const
		{
			return CONVERT(_btbody.get().getInvInertiaTensorWorld());
		}
		void						phyBody::setCenterOfMassTransform(const Mat4& xform)
		{
			_btbody.get().setCenterOfMassTransform(CONVERT(xform));
		}
		Mat4						phyBody::getCenterOfMassTransform() const
		{
			return CONVERT(_btbody.get().getCenterOfMassTransform());
		}
		Vec3						phyBody::getCenterOfMassPosition() const
		{
			return CONVERT(_btbody.get().getCenterOfMassPosition());
		}
		Quat						phyBody::getOrientation() const
		{
			return CONVERT(_btbody.get().getOrientation());
		}
		Vec3						phyBody::getInvInertiaDiagLocal() const
		{
			return CONVERT(_btbody.get().getInvInertiaDiagLocal());
		}
		void						phyBody::setInvInertiaDiagLocal(const Vec3& diag_inv_inertia)
		{
			_btbody.get().setInvInertiaDiagLocal(CONVERT(diag_inv_inertia));
		}
		void						phyBody::updateInertiaTensor()
		{
			_btbody.get().updateInertiaTensor();
		}
		void						phyBody::applyCentralForce(const Vec3& force)
		{
			_btbody.get().applyCentralForce(CONVERT(force));
		}
		void						phyBody::applyTorque(const Vec3& torque)
		{
			_btbody.get().applyTorque(CONVERT(torque));
		}
		void						phyBody::applyCentralImpulse(const Vec3& impulse)
		{
			_btbody.get().applyCentralImpulse(CONVERT(impulse));
		}
		void						phyBody::applyImpulse(const Vec3& impulse, const Vec3& rel_pos)
		{
			_btbody.get().applyImpulse(CONVERT(impulse), CONVERT(rel_pos));
		}
		void						phyBody::applyTorqueImpulse(const Vec3& torque)
		{
			_btbody.get().applyTorqueImpulse(CONVERT(torque));
		}
		void						phyBody::setLinearVelocity(const Vec3& lin_vel)
		{
			_btbody.get().setLinearVelocity(CONVERT(lin_vel));
		}
		Vec3						phyBody::getLinearVelocity() const
		{
			return CONVERT(_btbody.get().getLinearVelocity());
		}
		void						phyBody::setAngularVelocity(const Vec3& ang_vel)
		{
			_btbody.get().setAngularVelocity(CONVERT(ang_vel));
		}
		Vec3						phyBody::getAngularVelocity() const
		{
			return CONVERT(_btbody.get().getAngularVelocity());
		}
		Vec3						phyBody::getVelocityInLocalPoint(const Vec3& rel_pos) const
		{
			return CONVERT(_btbody.get().getVelocityInLocalPoint(CONVERT(rel_pos)));
		}
		AABB						phyBody::getAabb() const
		{
			btVector3 min_, max_;
			_btbody.get().getAabb(min_, max_);
			return AABB(CONVERT(min_), CONVERT(max_));
		}
	};
};

