
#include "VertexDeformCtrlBase.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		VertexDeformCtrlBase::VertexDeformCtrlBase(const wcs& name)
			: CtrlBase(name)
		{}
		VertexDeformCtrlBase::~VertexDeformCtrlBase()
		{}
		bool							VertexDeformCtrlBase::isFirstDeformer(const SPtr<SNode>& node, ATTRIB_TYPE attrib_type) const
		{
			if(!deforms(attrib_type))
				return false;
			const vector<SPtr<CtrlBase>>& ctrls = node->getCtrlSet().getArray();

			for(vector<SPtr<CtrlBase>>::const_iterator iter = ctrls.begin(); iter != ctrls.end(); ++iter)
			{
				if(!(*iter)->type_info()->kind_of(VertexDeformCtrlBase::type_info_static()))
					continue;
				if((*iter).get() == this)
					return true;
				if((*iter).cast<VertexDeformCtrlBase>()->deforms(attrib_type))
					return false;
			}

			Z_ASSERT( false );						// this controller is not attached to the node, it is a usage error

			return false;
		}
	};
};

