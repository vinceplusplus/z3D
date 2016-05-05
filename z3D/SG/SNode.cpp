
#include "SNode.h"
#include "SCameraNode.h"
#include "SMeshNode.h"
#include "Mesh.h"

#define SAFE_DELETE(x) if(x) {delete x;x=NULL;}

namespace z3D
{
	namespace SG
	{
		SNode::SNode(const SNode&) {}
		SNode& SNode::operator=(const SNode&) {return *this;}

		void					SNode::___ctor()
		{
			_id = _next_id++;
			_dirty_world_trans = true;
			_dirty_world_bound = true;
			_dirty_tree_world_bound = true;
			_visible = true;
			_cast_shadow = false;
			_recv_shadow = false;
			_rot = Quat(Mat3::identity);
			_scale = Vec3(1, 1, 1);
			_color = Vec4(1, 1, 1, 1);

			_force_tree_local_bound = false;
			_force_local_bound = false;

			_tree_has_renderables = false;
			_dirty_tree_has_renderables = true;
			_dirty_final_color = true;

			_force_blend = false;
			_dirty_final_force_blend = true;
			_final_force_blend = false;
		}
		SNode::SNode()
		{
			___ctor();
		}
		SNode::~SNode()
		{
			_array_child.clear();
		}
		const Vec3&		SNode::pos() const {return _pos;}
		const Quat&		SNode::rot() const {return _rot;}
		const Vec3&		SNode::scale() const {return _scale;}
		void				SNode::setPos(const Vec3& pos)
		{
			//if(memcmp(&_pos, &pos, sizeof(Vec3)) == 0)
			//	return;
			if(_pos == pos)
				return;
			_pos = pos;
			trueMarkDirtyTrans();
		}
		void				SNode::setRot(const Quat& rot)
		{
			//if(memcmp(&_rot, &rot, sizeof(Quat)) == 0)
			//	return;
			if(_rot == rot)
				return;
			_rot = rot;
			trueMarkDirtyTrans();
		}
		void				SNode::setScale(const Vec3& scale)
		{
			//if(memcmp(&_scale, &scale, sizeof(Vec3)) == 0)
			//	return;
			if(_scale == scale)
				return;
			_scale = scale;
			trueMarkDirtyTrans();
		}
		void				SNode::setSRT(const Vec3& pos,const Quat& rot,const Vec3& scale)
		{
			_pos = pos;
			_rot = rot;
			_scale = scale;
			trueMarkDirtyTrans();
		}
		void				SNode::setTransform(const Mat4& tm)
		{
			tm.factorSRT(_scale, _rot, _pos);
			trueMarkDirtyTrans();
		}
		void				SNode::setWorldPos(const Vec3& pos)
		{
			if(!_parent.valid())
				_pos = pos;
			else
			{
				Mat4 new_world = Mat4::scaling(world_scale()) * world_rot().asMat4() * Mat4::translation(pos);
				Mat4 new_local = new_world * _parent.get_unsafe()->inv_world_trans();
				Vec3 scale;
				Quat rot;
				new_local.factorSRT(scale, rot, _pos);
			}
			trueMarkDirtyTrans();
		}
		void				SNode::setWorldRot(const Quat& rot)
		{
			if(!_parent.valid())
				_rot = rot;
			else
			{
				Mat4 new_world = Mat4::scaling(world_scale()) * rot.asMat4() * Mat4::translation(world_pos());
				Mat4 new_local = new_world * _parent.get_unsafe()->inv_world_trans();
				Vec3 scale,pos;
				new_local.factorSRT(scale, _rot, pos);
			}
			trueMarkDirtyTrans();
		}
		void				SNode::setWorldScale(const Vec3& scale)
		{
			if(!_parent.valid())
				_scale = scale;
			else
			{
				Mat4 new_world = Mat4::scaling(scale) * world_rot().asMat4() * Mat4::translation(world_pos());
				Mat4 new_local = new_world * _parent.get_unsafe()->inv_world_trans();
				Quat rot;
				Vec3 pos;
				new_local.factorSRT(_scale, rot, pos);
			}
			trueMarkDirtyTrans();
		}
		void				SNode::setWorldTransform(const Mat4& tm)
		{
			if(!_parent.valid())
				setTransform(tm);
			else
			{
				Mat4 new_local = tm * _parent.get_unsafe()->inv_world_trans();
				setTransform(new_local);
			}
		}
		void				SNode::translate(const Vec3& t)
		{
			(local_trans() * Mat4::translation(t)).factorSRT(_scale, _rot, _pos);
			trueMarkDirtyTrans();
		}
		void				SNode::rotate(const Quat& q)
		{
			(local_trans() * q.asMat4()).factorSRT(_scale, _rot, _pos);
			trueMarkDirtyTrans();
		}
		void				SNode::scale(const Vec3& s)
		{
			(local_trans() * Mat4::scaling(s)).factorSRT(_scale, _rot, _pos);
			trueMarkDirtyTrans();
		}
		void				SNode::translateWorld(const Vec3& t)
		{
			if(!_parent.valid())
				translate(t);
			else
			{
				((world_trans() * Mat4::translation(t)) * _parent.get_unsafe()->inv_world_trans()).factorSRT(_scale, _rot, _pos);
				trueMarkDirtyTrans();
			}
		}
		void				SNode::rotateWorld(const Quat& q)
		{
			if(!_parent.valid())
				rotate(q);
			else
			{
				((world_trans() * q.asMat4()) * _parent.get_unsafe()->inv_world_trans()).factorSRT(_scale, _rot, _pos);
				trueMarkDirtyTrans();
			}
		}
		void				SNode::scaleWorld(const Vec3& s)
		{
			if(!_parent.valid())
				scale(s);
			else
			{
				((world_trans() * Mat4::scaling(s)) * _parent.get_unsafe()->inv_world_trans()).factorSRT(_scale, _rot, _pos);
				trueMarkDirtyTrans();
			}
		}
		void				SNode::updateTrans()
		{
			if(!_dirty_world_trans)
				return;

			_local_trans = Mat4::scaling(_scale) * _rot.asMat4() * Mat4::translation(_pos);
			if(_parent.valid())
				_world_trans = _local_trans * _parent.get_unsafe()->world_trans();
			else
				_world_trans = _local_trans;
			_world_trans.factorSRT(_world_scale, _world_rot, _world_pos);
			_inv_world_trans = _world_trans.inverse();

			_dirty_world_trans = false;
		}
		void				SNode::updateBound()
		{
			if(!_dirty_world_bound)
				return;

			if(_force_local_bound)
				_world_bound = _local_bound.transformedBy(world_trans());
			else
				_world_bound = local_bound().transformedBy(world_trans());

			_dirty_world_bound = false;
		}
		void				SNode::updateTreeBound()
		{
			if(!_dirty_tree_world_bound)
				return;

			if(_force_tree_local_bound)
			{
				_tree_world_bound = _tree_local_bound.transformedBy(world_trans());

				_dirty_tree_world_bound = false;
				return;
			}

			_tree_world_bound = world_bound();

			class fn
			{
			private:
				AABB&		_b;
			public:
				fn(AABB& b) : _b(b) {}
				void		operator() (const SPtr<SNode>& n)
				{
					if(n->visible())
						_b.fit(n->tree_world_bound());
				}
			};

			for_each<vector<SPtr<SNode>>::iterator, fn&>(_array_child.begin(), _array_child.end(), fn(_tree_world_bound));

			_dirty_tree_world_bound = false;
		}
		void				SNode::trueMarkDirtyTrans()
		{
			markDirtyTrans();
			markDirtyBound();
		}
		void				SNode::markDirtyTrans()
		{
			_dirty_world_trans = true;
			_dirty_tree_world_bound = true;
			_dirty_world_bound = true;

			class fn
			{public:
				void		operator() (const SPtr<SNode>& n)
				{
					n->markDirtyTrans();
				}
			};

			for_each<vector<SPtr<SNode>>::iterator, fn&>(_array_child.begin(), _array_child.end(), fn());
		}
		void				SNode::markDirtyBound()
		{
			_dirty_tree_world_bound = true;
			_dirty_world_bound = true;
			if(_parent.valid() && !_parent.get_unsafe()->forcedTreeLocalBound())
				_parent.get_unsafe()->markDirtyBound();
		}
		void				SNode::markDirtyOnlyTreeWorldBound()
		{
			_dirty_tree_world_bound = true;
			if(_parent.valid() && !_parent.get_unsafe()->forcedTreeLocalBound())
				_parent.get_unsafe()->markDirtyOnlyTreeWorldBound();
		}
		void				SNode::setVisible(bool visible)
		{
			if(_visible != visible)
			{
				_visible = visible;
				if(_parent.valid() && !_parent.get_unsafe()->forcedTreeLocalBound())
					_parent.get_unsafe()->markDirtyOnlyTreeWorldBound();
			}
		}
		void				SNode::setColor(const Vec4& color)
		{
			_color;
			markDirtyFinalColor();
		}
		const Vec4&			SNode::getColor() const
		{
			return _color;
		}
		const Vec4&			SNode::getFinalColor()
		{
			updateFinalColor();
			return _final_color;
		}
		void				SNode::markDirtyFinalColor()
		{
			_dirty_final_color = true;

			class fn
			{public:
				void		operator() (const SPtr<SNode>& n)
				{
					n->markDirtyFinalColor();
				}
			};

			for_each<vector<SPtr<SNode>>::iterator, fn&>(_array_child.begin(), _array_child.end(), fn());
		}
		void				SNode::updateFinalColor()
		{
			if(!_dirty_final_color)
				return;

			if(_parent.valid())
				_final_color = _color.modulate(_parent.get_unsafe()->getFinalColor());
			else
				_final_color = _color;

			_dirty_final_color = false;
		}
		void				SNode::forceTreeLocalBound(bool forced, const AABB& aabb)
		{
			_force_tree_local_bound = forced;
			_tree_local_bound = aabb;

			markDirtyOnlyTreeWorldBound();
		}
		bool				SNode::forcedTreeLocalBound() const
		{
			return _force_tree_local_bound;
		}
		const AABB&			SNode::getTreeLocalBound() const
		{
			return _tree_local_bound;
		}
		void				SNode::forceLocalBound(bool forced, const AABB& aabb)
		{
			_force_local_bound = forced;
			_local_bound = aabb;

			markDirtyBound();
		}
		bool				SNode::forcedLocalBound() const
		{
			return _force_local_bound;
		}
		const AABB&			SNode::getLocalBound() const
		{
			return _local_bound;
		}
		bool				SNode::treeHasRenderables()
		{
			updateTreeHasRenderables();
			return _tree_has_renderables;
		}
		void				SNode::updateTreeHasRenderables()
		{
			if(!_dirty_tree_has_renderables)
				return;
			bool tree_has_renderables = renderable();
			if(_array_child.size())
			{
				//SPtr<SNode>* end = VECTOR_END_PTR(_array_child);
				//for(SPtr<SNode>* iter = VECTOR_BEGIN_PTR(_array_child); iter != end && !tree_has_renderables; ++iter)
				//	tree_has_renderables |= (*iter)->treeHasRenderables();

				class fn
				{
				private:
					bool&		_b;
				public:
					fn(bool& b) : _b(b) {}
					void		operator() (const SPtr<SNode>& n)
					{
						_b |= n->treeHasRenderables();
					}
				};

				for_each<vector<SPtr<SNode>>::iterator, fn&>(_array_child.begin(), _array_child.end(), fn(tree_has_renderables));
			}

			_tree_has_renderables = tree_has_renderables;
			_dirty_tree_has_renderables = false;
		}
		void				SNode::markDirtyTreeHasRenderables()
		{
			_dirty_tree_has_renderables = true;
			if(_parent.valid())
				_parent.get_unsafe()->markDirtyTreeHasRenderables();
		}
		void				SNode::setForceBlend(bool force)
		{
			_force_blend = force;
			markDirtyFinalForceBlend();
		}
		bool				SNode::getForceBlend() const
		{
			return _force_blend;
		}
		bool				SNode::getFinalForceBlend()
		{
			updateFinalForceBlend();
			return _final_force_blend;
		}
		void				SNode::markDirtyFinalForceBlend()
		{
			_dirty_final_force_blend = true;

			class fn
			{public:
				void		operator() (const SPtr<SNode>& n)
				{
					n->markDirtyFinalForceBlend();
				}
			};

			for_each<vector<SPtr<SNode>>::iterator, fn&>(_array_child.begin(), _array_child.end(), fn());
		}
		void				SNode::updateFinalForceBlend()
		{
			if(!_dirty_final_force_blend)
				return;

			_final_force_blend = _force_blend;

			if(_parent.valid())
				_final_force_blend &= _parent.get_unsafe()->getFinalForceBlend();

			_dirty_final_force_blend = false;
		}
		size_t				SNode::id() const {return _id;}
		bool				SNode::dirty_world_trans() const {return _dirty_world_trans;}
		bool				SNode::dirty_world_bound() const {return _dirty_world_bound;}
		bool				SNode::dirty_tree_world_bound() const {return _dirty_tree_world_bound;}
		bool				SNode::visible() const {return _visible;}
		SPtr<SNode>			SNode::root()
		{
			if(_parent.valid())
				return _parent.get_unsafe()->root();
			return SPtr<SNode>(this);
		}
		SPtr<SNode>		SNode::parent() const
		{
			return _parent.lock();
		}
		const SPtr<SNode>&		SNode::getChild(size_t index) const
		{
			return _array_child[index];
		}
		SPtr<SNode>				SNode::getChildByName(const wcs& name)
		{
			class less
			{
			public:
				bool		operator() (const pair<wcs, WPtr<SNode>>& e0, const pair<wcs, WPtr<SNode>>& e1) const
				{
					return e0.first < e1.first;
				}
				bool		operator() (const pair<wcs, WPtr<SNode>>& e0, const wcs& e1) const
				{
					return e0.first < e1;
				}
				bool		operator() (const wcs& e0, const pair<wcs, WPtr<SNode>>& e1) const
				{
					return e0 < e1.first;
				}
			};
			if(_array_child.size() != _name_to_node.size())
			{
				_name_to_node.clear();
				class fn
				{
				private:
					vector<pair<wcs, WPtr<SNode>>>
								_name_to_node;
				public:
					fn(vector<pair<wcs, WPtr<SNode>>>& name_to_node)
						: _name_to_node(name_to_node)
					{}
				public:
					void		operator() (const SPtr<SNode>& n)
					{
						_name_to_node.push_back(pair<wcs, WPtr<SNode>>(n->getName(), n));
					}
				};

				for_each<vector<SPtr<SNode>>::iterator, fn&>(_array_child.begin(), _array_child.end(), fn(_name_to_node));
				std::sort(_name_to_node.begin(), _name_to_node.end(), less());
			}
			vector<pair<wcs, WPtr<SNode>>>::iterator iter = std::lower_bound(_name_to_node.begin(), _name_to_node.end(), name, less());
			if(iter == _name_to_node.end() || iter->first != name)
				return NULL;
			return iter->second.lock();
		}
		size_t					SNode::getChildCount() const {return _array_child.size();}
		const vector<SPtr<SNode>>&
								SNode::getChildren() const {return _array_child;}
		size_t			SNode::depth() const
		{
			size_t d = 0;
			for(WPtr<SNode> p = _parent; p.valid(); p = p.get_unsafe()->parent())
				++d;
			return d;
		}
		const Mat4&		SNode::local_trans()
		{
			updateTrans();
			return _local_trans;
		}
		const Mat4&		SNode::world_trans()
		{
			updateTrans();
			return _world_trans;
		}
		const Mat4&		SNode::inv_world_trans()
		{
			updateTrans();
			return _inv_world_trans;
		}
		const AABB&		SNode::tree_world_bound()
		{
			updateTreeBound();
			return _tree_world_bound;
		}
		const AABB&				SNode::world_bound()
		{
			updateBound();
			return _world_bound;
		}
		const Vec3&		SNode::world_pos()
		{
			updateTrans();
			return _world_pos;
		}
		const Quat&		SNode::world_rot()
		{
			updateTrans();
			return _world_rot;
		}
		const Vec3&		SNode::world_scale()
		{
			updateTrans();
			return _world_scale;
		}
		const AABB&		SNode::local_bound()
		{
			return AABB::empty_bound;
		}
		void					SNode::setName(const wcs& name)
		{
			_name = name;
			if(_parent.valid())
				_parent.get_unsafe()->markDirtyNameNodeMap();
		}
		const wcs&				SNode::getName() const
		{
			return _name;
		}
		void					SNode::markDirtyNameNodeMap()
		{
			_name_to_node.clear();
		}
		void				SNode::addChild(const SPtr<SNode>& node,bool keep_world_trans_intact)
		{
			if(!node)
				return;
			if(node->_parent.valid() && node->_parent.get_unsafe() == this)
				return;
			node->detachFromParent(keep_world_trans_intact);
			_array_child.push_back(node);
			node->_parent = SPtr<SNode>(this);
			if(keep_world_trans_intact)
			{
				node->updateTrans();
				node->_local_trans = node->_local_trans * inv_world_trans();
				node->_local_trans.factorSRT(node->_scale, node->_rot, node->_pos);
			}
			node->trueMarkDirtyTrans();
			node->markDirtyFinalColor();
			node->markDirtyFinalForceBlend();

			markDirtyOnlyTreeWorldBound();

			markDirtyTreeHasRenderables();

			markDirtyNameNodeMap();
		}
		void				SNode::removeChild(const SPtr<SNode>& node, bool keep_world_trans_intact)
		{
			if(!node)
				return;
			if(!node->_parent.valid() || node->_parent.get_unsafe() != this)
				return;
			for(ptrdiff_t i = (ptrdiff_t)_array_child.size() - 1; i >= 0; --i)
				if(getChild(i) == node)
				{
					const SPtr<SNode>& child = getChild(i);
					child->updateTrans();
					if(keep_world_trans_intact)
					{
						child->_pos = child->_world_pos;
						child->_rot = child->_world_rot;
						child->_scale = child->_world_scale;
						child->_local_trans = child->_world_trans;
					}
					child->_parent.reset();
					child->trueMarkDirtyTrans();
					child->markDirtyFinalColor();
					child->markDirtyFinalForceBlend();
					markDirtyOnlyTreeWorldBound();

					_array_child.erase(_array_child.begin() + i);
					//if(true)
					//{
					//	list<SPtr<SNode>>::iterator iter = _array_child.begin();
					//	advance(iter, i);
					//	_array_child.erase(iter);
					//}
					break;
				}

			markDirtyTreeHasRenderables();

			markDirtyNameNodeMap();
		}
		void				SNode::detachFromParent(bool keep_world_trans_intact)
		{
			if(_parent.valid())
				_parent.get_unsafe()->removeChild(SPtr<SNode>(this), keep_world_trans_intact);
		}
		void		SNode::setCastShadow(bool cast) {_cast_shadow=cast;}
		bool		SNode::castShadow() const {return _cast_shadow;}
		void		SNode::setRecvShadow(bool recv) {_recv_shadow=recv;}
		bool		SNode::recvShadow() const {return _recv_shadow;}
		bool		SNode::canCull() const {return false;}
		bool		SNode::renderable() const {return false;}
		bool		SNode::canCastShadow() const {return false;}
		bool		SNode::manifold_2() {return false;}
		void		SNode::cull(const Frustum& frustum,const SPtr<SCameraNode>& camera) {}
		void		SNode::render(intptr_t parameter, SRenderInfo* ri) {}
		//void		SNode::renderDepthOnly(int parameter) {}
		void		SNode::preRender(SRenderInfo* ri) {}
		void		SNode::preRenderLighting(SRenderInfo* ri) {}
		bool		SNode::descendentOf(const SPtr<SNode>& ancestor)
		{
			if(!_parent.valid())
				return false;
			if(_parent.get_unsafe() == ancestor.get())
				return true;
			return _parent.get_unsafe()->descendentOf(ancestor);
		}
		SPtr<SNode>	SNode::findNodeByID(size_t id)
		{
			if(_id == id)
				return this;
			SPtr<SNode> node;
			for(size_t i = 0; i < _array_child.size(); ++i)
				if((node = getChild(i)->findNodeByID(id)))
					return node;
			return NULL;
		}
		SPtr<SNode>	SNode::getAncestorKindOf(TypeInfo* typeinfo)
		{
			if(!_parent.valid())
				return NULL;
			if(_parent.get_unsafe()->type_info()->kind_of(typeinfo))
				return _parent.lock();
			return _parent.get_unsafe()->getAncestorKindOf(typeinfo);
		}
		size_t	SNode::polyCount() {return 0;}
		void	SNode::buildControllers(const wcs& node_name) {}
		const CtrlSet&			SNode::getCtrlSet() const
		{
			return _ctrlset;
		}
		CtrlSet&				SNode::getCtrlSet()
		{
			return _ctrlset;
		}
		void					SNode::setTMCtrl(const SPtr<CtrlBase>& ctrl)
		{
			if(_tm_ctrl.valid())
				getCtrlSet().detachCtrl(_tm_ctrl.lock());
			if(ctrl)
				getCtrlSet().attachCtrl(ctrl);
			_tm_ctrl = ctrl;
		}
		SPtr<CtrlBase>			SNode::getTMCtrl() const
		{
			return _tm_ctrl.lock();
		}
		void					SNode::setPhyBody(const SPtr<z3D::Physics::phyBody>& phy_body)
		{
			_phy_body = phy_body;
		}
		SPtr<z3D::Physics::phyBody>
								SNode::getPhyBody() const
		{
			return _phy_body.lock();
		}
		void					SNode::setUserData(const SPtr<RefBase>& userdata)
		{
			_userdata = userdata;
		}
		const SPtr<RefBase>&	SNode::getUserData() const
		{
			return _userdata;
		}
		bool					SNode::supportsSPT() const
		{
			return false;
		}
		void					SNode::buildSPT()
		{
		}
		void					SNode::clearSPT()
		{
		}
		const SPtr<btTriTree>&	SNode::spt()
		//const SPtr<ocTriTree>&	SNode::spt()
		{
			return _empty_spt;
		}
		wcs						SNode::toString()
		{
			return FORMATW(L"%5d, %s, (%f, %f, %f), (%f %f %f %f %f %f)",id(),type_info()->name().c_str(),world_pos().x,world_pos().y,world_pos().z,
				world_bound().center().x,
				world_bound().center().y,
				world_bound().center().z,
				world_bound().extent().x,
				world_bound().extent().y,
				world_bound().extent().z);
		}
		wcs						SNode::print_hierarchy(size_t max_depth)
		{
			size_t root_depth = depth();

			stack_any s;
			vector<SNode*> n;

			n.push_back(this);

			if(getChildCount())
			{
				s.push(getChild(0).get());
				s.push((int)0);
			}
			
			while(s.available())
			{
				SNode* sn;
				size_t index;
				s.pop(index);
				s.pop(sn);
				n.push_back(sn);
				if(sn->parent() && sn->parent()->getChildCount() > index + 1)
				{
					s.push(sn->parent()->getChild(index + 1).get());
					s.push((int)(index + 1));
				}
				if(sn->depth() - root_depth <= max_depth && sn->getChildCount())
				{
					s.push(sn->getChild((size_t)0).get());
					s.push(0);
				}
			}
			wcs str;
			for(size_t i = 0; i < n.size(); ++i)
			{
				SNode* sn=n[i];
				size_t d = sn->depth() - root_depth;
				for(size_t j = 0; j < d; ++j)
					str += L" ";
				for(size_t j = 0; j < d; ++j)
					str += L"*";
				str += L" ";
				str += sn->toString();
				str += L"\r\n";
			}
			return str;
		}
	};
};

