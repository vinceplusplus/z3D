
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		class SBoneNode: public SNode
		{
			DECL_TYPEINFO();
		protected:
			AABB		_local_bound;
		public:
			SBoneNode(const AABB& local_bound);
		protected:
			~SBoneNode();
		public:
			const AABB&			local_bound();
		};
	};
};


