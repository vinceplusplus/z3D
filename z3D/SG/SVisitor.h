
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace SG
	{
		class SVisitor
		{
		public:
			SVisitor();
			virtual ~SVisitor();
		public:
			void			traverse(const SPtr<SNode>& node, size_t levels_left = -1);
			void			traverseUp(const SPtr<SNode>& node, size_t levels_left = -1);
		private:
			void			__traverse(const SPtr<SNode>& node, size_t levels_left);
			void			__traverseUp(const SPtr<SNode>& node, size_t levels_left);
		protected:
			virtual bool	onVisit(const SPtr<SNode>& node) = 0;														// return true to continue traversing, otherwise not
		protected:
			virtual void	onPreTraverse();
			virtual void	onPostTraverse();
		};

		class SAABBIntersector: public SVisitor
		{
		protected:
			AABB		_intersector;
			TypeInfo*	_type_info;
		public:
			vector<SPtr<SNode>, frame_temp_allocator<SPtr<SNode>>>	array_node;
		public:
			SAABBIntersector(const AABB& intersector, const TypeInfo* type_info);
		public:
			void	restart(const AABB& intersector, const TypeInfo* type_info);
		public:
			void	clear();
		public:
			void	onPreTraverse();
		private:
			bool	onVisit(const SPtr<SNode>& node);
		};

		// selects the first hitting triangle from mesh node
		class STriangleSelector: public SVisitor
		{
		protected:
			Ray					_ray;
		public:
			//TriangleAux		tri;
			int32_t				tri_idx;
			SPtr<SMeshNode>		mn;
			REAL				time;								// not INFINITY means a triangle is selected
		public:
			STriangleSelector(const Ray& ray);
		public:
			void	clear();
			void	reset(const Ray& ray);
		private:
			void	onPreTraverse();
			bool	onVisit(const SPtr<SNode>& node);
		};
	};
};


