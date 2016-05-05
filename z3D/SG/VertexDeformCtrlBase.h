
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace SG
	{
		class VertexDeformCtrlBase: public CtrlBase
		{
			DECL_TYPEINFO();
		public:
			typedef enum _ATTRIB_TYPE
			{
				ATTRIB_TYPE_POSITION = 0,
				ATTRIB_TYPE_NORMAL,
				ATTRIB_TYPE_UV,
			} ATTRIB_TYPE;
		public:
			VertexDeformCtrlBase(const wcs& name);
		protected:
			~VertexDeformCtrlBase();
		public:
			bool							isFirstDeformer(const SPtr<SNode>& node, ATTRIB_TYPE attrib_type) const;
		public:
			virtual bool					deforms(ATTRIB_TYPE attrib_type) const = 0;
		};
	};
};

