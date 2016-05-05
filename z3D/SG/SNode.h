
#pragma once

#include "predecl.h"

#include "../Physics/phyBody.h"

namespace z3D
{
	namespace SG
	{
		// scene graph node as well as transformation node
		// doesn't support shearing, and most engines don't
		// transformations written as scaling x rotation x translation, SRT

		class SNode: public RefBase
		{
			DECL_TYPEINFO();
		private:
			SNode(const SNode&);
			SNode& operator=(const SNode&);
		private:
			static size_t			_next_id;

			static SPtr<btTriTree>	_empty_spt;
			//static SPtr<ocTriTree>	_empty_spt;
		private:
			size_t					_id;
			Vec3					_pos;
			Quat					_rot;
			Vec3					_scale;
			Mat4					_local_trans;
		private:
			Vec3					_world_pos;
			Quat					_world_rot;
			Vec3					_world_scale;
			Mat4					_world_trans;
			Mat4					_inv_world_trans;
			AABB					_world_bound;
			AABB					_tree_world_bound;
			Vec4					_color;
			Vec4					_final_color;
		private:
			WPtr<SNode>				_parent;
			vector<SPtr<SNode>>		_array_child;
		private:
			union
			{
				struct
				{
					bool					_dirty_world_trans : 1;
					bool					_dirty_world_bound : 1;
					bool					_dirty_tree_world_bound : 1;
					bool					_visible : 1;
					bool					_cast_shadow : 1;
					bool					_recv_shadow : 1;
					bool					_force_tree_local_bound : 1;
					bool					_force_local_bound : 1;
					bool					_tree_has_renderables : 1;
					bool					_dirty_tree_has_renderables : 1;
					bool					_dirty_final_color : 1;
					bool					_force_blend : 1;
					bool					_dirty_final_force_blend : 1;
					bool					_final_force_blend : 1;
				};
				uint32_t				__UNNAMED0;
			};
		private:
			CtrlSet					_ctrlset;
			WPtr<CtrlBase>			_tm_ctrl;
		private:
			SPtr<RefBase>			_userdata;
		private:
			AABB					_tree_local_bound;
			AABB					_local_bound;
		private:
			wcs						_name;
			vector<pair<wcs, WPtr<SNode>>>
									_name_to_node;
		private:
			WPtr<z3D::Physics::phyBody>
									_phy_body;
		private:
			void					___ctor();
		public:
			SNode();
		protected:
			~SNode();
		public:
			const Vec3&				pos() const;
			const Quat&				rot() const;
			const Vec3&				scale() const;
		public:
			SPtr<SNode>				root();
			SPtr<SNode>				parent() const;
			const SPtr<SNode>&		getChild(size_t index) const;
			SPtr<SNode>				getChildByName(const wcs& name);
			size_t					getChildCount() const;
			const vector<SPtr<SNode>>&
									getChildren() const;
			size_t					depth() const;
		public:
			size_t					id() const;
			bool					dirty_world_trans() const;
			bool					dirty_world_bound() const;
			bool					dirty_tree_world_bound() const;
			bool					visible() const;
		public:
			const Mat4&				local_trans();
			const Mat4&				world_trans();
			const Mat4&				inv_world_trans();
			const AABB&				tree_world_bound();
			const AABB&				world_bound();
			const Vec3&				world_pos();
			const Quat&				world_rot();
			const Vec3&				world_scale();
		public:
			virtual const AABB&		local_bound();		// derived nodes must override
		public:
			void					setName(const wcs& name);
			const wcs&				getName() const;
		private:
			void					markDirtyNameNodeMap();
		public:
			void					setPos(const Vec3& pos);
			void					setRot(const Quat& rot);
			void					setScale(const Vec3& scale);
			void					setSRT(const Vec3& pos, const Quat& rot, const Vec3& scale);
			void					setTransform(const Mat4& tm);
		public:
			void					setWorldPos(const Vec3& pos);
			void					setWorldRot(const Quat& rot);
			void					setWorldScale(const Vec3& scale);
			void					setWorldTransform(const Mat4& tm);
		public:
			void					translate(const Vec3& t);
			void					rotate(const Quat& q);
			void					scale(const Vec3& s);
		public:
			void					translateWorld(const Vec3& t);
			void					rotateWorld(const Quat& q);
			void					scaleWorld(const Vec3& s);
		protected:
			virtual void			updateTrans();
			virtual void			updateBound();
			virtual void			updateTreeBound();
		public:
			void					trueMarkDirtyTrans();
		public:
			virtual void			markDirtyTrans();
			virtual void			markDirtyBound();										// both world bound and tree world bound
			virtual void			markDirtyOnlyTreeWorldBound();
		public:
			void					setVisible(bool visible);
		public:
			void					setColor(const Vec4& color);
			const Vec4&				getColor() const;
			const Vec4&				getFinalColor();
		protected:
			void					markDirtyFinalColor();
		protected:
			void					updateFinalColor();
		public:
			void					forceTreeLocalBound(bool forced, const AABB& aabb);
			bool					forcedTreeLocalBound() const;
			const AABB&				getTreeLocalBound() const;
		public:
			void					forceLocalBound(bool forced, const AABB& aabb);
			bool					forcedLocalBound() const;
			const AABB&				getLocalBound() const;
		public:
			bool					treeHasRenderables();
		protected:
			void					updateTreeHasRenderables();
			void					markDirtyTreeHasRenderables();
		public:
			void					setForceBlend(bool force);
			bool					getForceBlend() const;
			bool					getFinalForceBlend();
		protected:
			void					markDirtyFinalForceBlend();
		protected:
			void					updateFinalForceBlend();
		public:
			void					addChild(const SPtr<SNode>& node, bool keep_world_trans_intact);
			void					removeChild(const SPtr<SNode>& node, bool keep_world_trans_intact);
			void					detachFromParent(bool keep_world_trans_intact);
		public:
			void					setCastShadow(bool cast);
			bool					castShadow() const;
			void					setRecvShadow(bool recv);
			bool					recvShadow() const;
		public:
			virtual bool			canCull() const;
			virtual bool			renderable() const;							// e.g. mesh nodes, light nodes
			virtual bool			canCastShadow() const;
			virtual bool			manifold_2();
		public:
			virtual void			cull(const Frustum& frustum, const SPtr<SCameraNode>& camera);
			virtual void			render(intptr_t parameter, SRenderInfo* ri);
			//virtual void			renderDepthOnly(int parameter);
		public:
			virtual void			preRender(SRenderInfo* ri);					// preprocessing for rendering, derived nodes must override, don't do add/remove child inside
			virtual void			preRenderLighting(SRenderInfo* ri);			// preprocessing for rendering, derived nodes must override, don't do add/remove child inside
//			virtual void			traverse(...);
			virtual size_t			polyCount();
		public:
			virtual void			buildControllers(const wcs& node_name/*, NodeCtrlSet& ctrl_set*/);
		public:
			bool					descendentOf(const SPtr<SNode>& ancestor);
		public:
			SPtr<SNode>				findNodeByID(size_t id);
		public:
			SPtr<SNode>				getAncestorKindOf(TypeInfo* typeinfo);
		//public:
		//	static size_t			registerUserProp(const wstring& name);
		//	static const wstring&	getUserPropName(size_t id);
		//public:
		//	void					setUserProp(size_t id,const SPtr<RefBase>& userprop);
		//	SPtr<RefBase>			getUserProp(size_t id);
		//public:
		//	template<class T>
		//	SPtr<T>					userprop(size_t id) {return getUserProp(id).cast<T>();}
		public:
			const CtrlSet&			getCtrlSet() const;
			CtrlSet&				getCtrlSet();
		public:
			void					setTMCtrl(const SPtr<CtrlBase>& ctrl);
			SPtr<CtrlBase>			getTMCtrl() const;
		public:
			void					setPhyBody(const SPtr<z3D::Physics::phyBody>& phy_body);
			SPtr<z3D::Physics::phyBody>
									getPhyBody() const;
		public:
			void					setUserData(const SPtr<RefBase>& userdata);
			const SPtr<RefBase>&	getUserData() const;
			template<class T>
			SPtr<T>					getUserData() const		{	return getUserData().cast<T>();		}
		public:
			virtual bool			supportsSPT() const;
		public:
			virtual void			buildSPT();
			virtual void			clearSPT();
		public:
			virtual const SPtr<btTriTree>&	spt();
			//virtual const SPtr<ocTriTree>&	spt();
		public:
			virtual wcs				toString();
		public:
			wcs						print_hierarchy(size_t max_depth = 0x7fffffff);
		};
	};
};

