
#include "SBoneNode.h"

namespace z3D
{
	namespace SG
	{
		SBoneNode::SBoneNode(const AABB& local_bound)
		{
			_local_bound = local_bound;
		}
		SBoneNode::~SBoneNode() {}
		const AABB&			SBoneNode::local_bound()
		{
			return _local_bound;
		}
	};
};

