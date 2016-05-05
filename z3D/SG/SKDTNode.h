
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		class SKDTNode: public SNode
		{
			DECL_TYPEINFO();
		public:
			void				buildTree(const vector< SPtr<SNode> >& array_detached_node,int nodes_per_leaf,int depth_left);
			void				cleanTree(vector< SPtr<SNode> >& array_detached_node);
		public:
			void				rebuildTree(int nodes_per_leaf,int depth_left);
		};
	};
};

