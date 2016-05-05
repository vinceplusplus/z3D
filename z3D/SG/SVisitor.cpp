
#include "predecl.h"

#include "../Core/allocators_inl.h"

#include "SVisitor.h"
#include "SNode.h"
#include "SMeshNode.h"

#include "Mesh.h"

namespace z3D
{
	namespace SG
	{
		SVisitor::SVisitor() {}
		SVisitor::~SVisitor() {}
		void SVisitor::traverse(const SPtr<SNode>& node, size_t levels_left)
		{
			onPreTraverse();

			__traverse(node, levels_left);

			onPostTraverse();
		}
		void SVisitor::traverseUp(const SPtr<SNode>& node, size_t levels_left)
		{
			onPreTraverse();

			__traverseUp(node, levels_left);

			onPostTraverse();
		}
		void	SVisitor::__traverse(const SPtr<SNode>& node, size_t levels_left)
		{
			if(!onVisit(node))
				return;

			if(levels_left == 0)
				return;
			if(levels_left != -1)
				--levels_left;

			class fn
			{
			private:
				SVisitor&		_t;
				size_t			_levels_left;
			public:
				fn(SVisitor& t, size_t levels_left)
					: _t(t), _levels_left(levels_left) {}
				void			operator()(const SPtr<SNode>& n)
				{
					_t.__traverse(n, _levels_left);
				}
			};

			for_each<vector<SPtr<SNode>>::const_iterator, fn&>(node->getChildren().begin(), node->getChildren().end(), fn(*this, levels_left));
		}

		void	SVisitor::__traverseUp(const SPtr<SNode>& node, size_t levels_left)
		{
			if(!onVisit(node))
				return;

			if(levels_left == 0)
				return;
			if(levels_left != -1)
				--levels_left;

			if(node->parent())
				__traverse(node->parent(), levels_left);
		}

		void	SVisitor::onPreTraverse() {}
		void	SVisitor::onPostTraverse() {}

		SAABBIntersector::SAABBIntersector(const AABB& intersector, const TypeInfo* type_info)
		{
			restart(intersector, type_info);
		}
		void	SAABBIntersector::restart(const AABB& intersector, const TypeInfo* type_info)
		{
			_intersector = intersector;
			_type_info = (TypeInfo*)type_info;
		}
		void	SAABBIntersector::clear()
		{
			array_node.clear();
		}
		void	SAABBIntersector::onPreTraverse()
		{
			clear();
		}
		bool	SAABBIntersector::onVisit(const SPtr<SNode>& node)
		{
			if(!node->visible())
				return false;
			if(node->getChildCount())
			{
				if(!Test::AABBAABB(_intersector, node->tree_world_bound()))
					return false;
				if(!Test::AABBAABB(_intersector, node->world_bound()))
					return true;
			}
			else
			{
				if(!Test::AABBAABB(_intersector, node->world_bound()))
					return false;
			}

			if(!_type_info || (_type_info && node->type_info()->kind_of(_type_info)))
				array_node.push_back(node);

			return true;
		}
		STriangleSelector::STriangleSelector(const Ray& ray)
		{
			_ray = ray;
			clear();
		}
		void	STriangleSelector::clear()
		{
			tri_idx = 0;
			mn = NULL;
			time = INFINITY;
		}
		void	STriangleSelector::reset(const Ray& ray)
		{
			_ray = ray;
			clear();
		}
		void	STriangleSelector::onPreTraverse()
		{
			clear();
		}
		bool	STriangleSelector::onVisit(const SPtr<SNode>& node)
		{
			if(!node->visible())
				return false;
			if(node->getChildCount())
			{
				if(!Test::RayAABB(_ray, node->tree_world_bound()))
					return false;
				if(!Test::RayAABB(_ray, node->world_bound()))
					return true;
			}
			else
			{
				if(!Test::RayAABB(_ray, node->world_bound()))
					return false;
			}

			if(node->type_info()->kind_of(SMeshNode::type_info_static()))
			{
				SPtr<SMeshNode>& mn = node.cast<SMeshNode>();
				REAL time_scale = (_ray.direction() * ((Mat3)mn->world_trans())).length();
				Ray ray(_ray.origin() * mn->inv_world_trans(), (_ray.direction() * ((Mat3)mn->inv_world_trans())).normal());

				mn->buildSPT();
				if(!mn->spt())
					return true;

				vector<int32_t, frame_temp_allocator<int32_t>> array_triidx;
				array_triidx.reserve(32);
				mn->spt()->intersect(ray, array_triidx);
				if(!array_triidx.size())
					return true;

				const Vec3* verts = mn->mesh()->get_verts();
				const vnFace* faces = mn->mesh()->get_faces();

				const int32_t* triidx_end = VECTOR_END_PTR(array_triidx);
				for(const int32_t* iter = VECTOR_BEGIN_PTR(array_triidx); iter != triidx_end; ++iter)
				{
					const int32_t& triidx = *iter;
					const vnFace& f = faces[triidx];
					REAL t;
					if(Intr::RayTriangle(ray, verts[f.v[0]], verts[f.v[1]], verts[f.v[2]], t))
					{
						t *= time_scale;
						if(t < time)
						{
							tri_idx = triidx;
							this->mn = mn;
							time = t;
						}
					}
				}
			}

			return true;
		}
	};
};

