
#pragma once

#include "predecl.h"

#include "btHelper.h"

class btRigidBody;
class btMotionState;

//namespace z3D
//{
//	namespace SG
//	{
//		class SNode;
//	};
//};
#include "../SG/predecl.h"

namespace z3D
{
	namespace Physics
	{
		class phyBody: public RefBase
		{
		public:
			typedef enum _ACTIVATION_STATE
			{
				AS_ACTIVE_TAG = 1,
				AS_ISLAND_SLEEPING,
				AS_WANTS_DEACTIVATION,
				AS_DISABLE_DEACTIVATION,
				AS_DISABLE_SIMULATION,
			} ACTIVATION_STATE;
		protected:
			class phybtMotionState;
		protected:
			typed_aligned_storage<btRigidBody, 528, 16, true>
										_btbody;
		protected:
			SPtr<phyShape>				_shape;
		protected:
			typed_aligned_storage<phybtMotionState, 140, 4, true>
										_ms;
		protected:
			phyBody();
			~phyBody();
		public:
			static SPtr<phyBody>		create(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& com_xform, REAL mass, bool skip_calc_inertia);
			static SPtr<phyBody>		create(const SPtr<z3D::SG::SNode>& node, const SPtr<phyShape>& shape, const Mat4& com_xform, REAL mass, const Vec3& inertia_override);
		public:
			const SPtr<phyShape>&		getShape() const;
		public:
			SPtr<z3D::SG::SNode>		getNode() const;
		public:
			btRigidBody*				get_btRigidBody() const;
		public:
			void						setActivationState(ACTIVATION_STATE as);
			ACTIVATION_STATE			getActivationState() const;
		public:
			void						setDeactivationTime(REAL t);
			REAL						getDeactivationTime() const;
		public:
			void						forceActivationState(ACTIVATION_STATE new_as);
			void						activate(bool force_activation = false);
			bool						isActive() const;
		public:
			void						setRestitution(REAL restitution);
			REAL						getRestitution() const;
		public:
			void						setFriction(REAL friction);
			REAL						getFriction() const;
		public:
			void						setWorldTransform(const Mat4& world_trans);
			Mat4						getWorldTransform() const;
		public:
			void						setHitFraction(REAL hit_fraction);
			REAL						getHitFraction() const;
		public:
			void						setGravity(const Vec3& gravity);
			Vec3						getGravity() const;
		public:
			void						setDamping(REAL linear_damping, REAL angular_damping);
			REAL						getLinearDamping() const;
			REAL						getAngularDamping() const;
		public:
			void						setMassProps(REAL mass, const Vec3& inertia);
			REAL						getInvMass() const;
			Mat3						getInvInertiaTensorWorld() const;
		public:
			void						setCenterOfMassTransform(const Mat4& xform);
			Mat4						getCenterOfMassTransform() const;
			Vec3						getCenterOfMassPosition() const;
			Quat						getOrientation() const;
		public:
			Vec3						getInvInertiaDiagLocal() const;
			void						setInvInertiaDiagLocal(const Vec3& diag_inv_inertia);
			void						updateInertiaTensor();
		public:
			void						applyCentralForce(const Vec3& force);
			void						applyTorque(const Vec3& torque);
		public:
			void						applyCentralImpulse(const Vec3& impulse);
			void						applyImpulse(const Vec3& impulse, const Vec3& rel_pos);
			void						applyTorqueImpulse(const Vec3& torque);
		public:
			void						setLinearVelocity(const Vec3& lin_vel);
			Vec3						getLinearVelocity() const;
			void						setAngularVelocity(const Vec3& ang_vel);
			Vec3						getAngularVelocity() const;
		public:
			Vec3						getVelocityInLocalPoint(const Vec3& rel_pos) const;
		public:
			AABB						getAabb() const;
		public:
			// TODO: add collision flags
		public:
			friend class				phyMgr;
		};
	};
};

