
#include "SKDTNode.h"

namespace z3D
{
	namespace SG
	{
		void				SKDTNode::buildTree(const vector< SPtr<SNode> >& array_detached_node,int nodes_per_leaf,int depth_left)
		{
			if(!array_detached_node.size())
				return;
			if((int)array_detached_node.size() <= nodes_per_leaf || depth_left <= 0)
			{
				const SPtr<SNode>* end = VECTOR_END_PTR(array_detached_node);
				for(const SPtr<SNode>* iter = VECTOR_BEGIN_PTR(array_detached_node); iter != end; ++iter)
					addChild(*iter, true);
				return;
			}

			Stats stats[3];
			AABB bound;
			if(true)
			{
				const SPtr<SNode>* end = VECTOR_END_PTR(array_detached_node);
				for(const SPtr<SNode>* iter = VECTOR_BEGIN_PTR(array_detached_node); iter != end; ++iter)
				{
					const AABB& b = (*iter)->tree_world_bound();
					if(b.isEmpty())
					{
						const Vec3& p = (*iter)->world_pos();
						stats[0].input(p.x);
						stats[1].input(p.y);
						stats[2].input(p.z);
					}
					else
					{
						bound.fit(b);
						const Vec3& c = b.center();
						stats[0].input(c.x);
						stats[1].input(c.y);
						stats[2].input(c.z);
					}
				}
			}
			double sd;
			double largest_sd = -INFINITY;
			int index;
			for(int i = 0; i < 3; ++i)
				if((sd = stats[i].standard_deviation()) > largest_sd)
				{
					largest_sd = sd;
					index = i;
				}
			SPtr<SKDTNode> node0(new SKDTNode);
			SPtr<SKDTNode> node1(new SKDTNode);

			REAL mean = (REAL)stats[index].mean();
			Vec3 new_pos;
			new_pos = world_pos();
			new_pos[index] = (bound.maximum()[index] + mean) * 0.5f;
			node0->setWorldPos(new_pos);
			new_pos[index] = (bound.minimum()[index] + mean) * 0.5f;
			node1->setWorldPos(new_pos);

			// we must have two SKDTNode(s) if it is not the leaf node, and that the two nodes must be in the beginning of the child node array
			addChild(node0, true);
			addChild(node1, true);

			vector< SPtr<SNode> > sub0,sub1;
			if(true)
			{
				const SPtr<SNode>* end = VECTOR_END_PTR(array_detached_node);
				for(const SPtr<SNode>* iter = VECTOR_BEGIN_PTR(array_detached_node); iter != end; ++iter)
				{
					if((*iter)->getTMCtrl())
						continue;
					const AABB& b = (*iter)->tree_world_bound();
					if(b.isEmpty())
					{
						if((*iter)->world_pos()[index] >= mean)
							sub0.push_back(*iter);
						else
							sub1.push_back(*iter);
					}
					else
					{
						if(b.center()[index] >= mean)
							sub0.push_back(*iter);
						else
							sub1.push_back(*iter);
					}
				}
			}
			if(!sub0.size() || !sub1.size())
			{
				int k=sizeof(k);
			}
			node0->buildTree(sub0, nodes_per_leaf, depth_left - 1);
			node1->buildTree(sub1, nodes_per_leaf, depth_left - 1);
		}
		void				SKDTNode::cleanTree(vector< SPtr<SNode> >& array_detached_node)
		{
			if(!getChildCount())
				return;
			ptrdiff_t start_index = 0;

			// check for not leaf node
			if(getChild(0)->type_info()->kind_of(SKDTNode::type_info_static()))
				start_index=2;

			// anyhow, we still need to search all over the array
			for(ptrdiff_t i = (ptrdiff_t)(getChildCount() - 1); i >= start_index; --i)
			{
				SPtr<SNode> node = getChild(i);
				node->detachFromParent(true);
				array_detached_node.push_back(node);
			}
			if(start_index)
			{
				getChild(0).cast<SKDTNode>()->cleanTree(array_detached_node);
				getChild(1).cast<SKDTNode>()->cleanTree(array_detached_node);
			}
		}
		void				SKDTNode::rebuildTree(int nodes_per_leaf, int depth_left)
		{
			vector< SPtr<SNode> > array_detached_node;
			cleanTree(array_detached_node);
			for(ptrdiff_t i = (ptrdiff_t)(getChildCount() - 1); i >= 0; --i)
				getChild(i)->detachFromParent(false);

			buildTree(array_detached_node, nodes_per_leaf, depth_left);
		}
	};
};

