
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace SG
	{
		class RenderParamQuery
		{
		public:
			typedef struct _LIGHTINFO
			{
				intptr_t						id;
				GfxEnum::LIGHTTYPE				type;
				Vec3							color;
				Vec3							pos;
				Vec3							dir;
				REAL							inner_cone;
				REAL							outer_cone;
				REAL							range;						// can be INFINITY for non-ranged
				ptrdiff_t						visibility_channel;
			} LIGHTINFO;
		public:
			virtual							~RenderParamQuery() {}
		public:
			virtual void					preSetupShadeState(rsShade& rs) = 0;
			virtual void					postSetupShadeState(rsShade& rs) = 0;
		public:
			virtual const Mat4&				getWorldTransform() = 0;
		public:
			virtual void					getLightingInfo(Vec3& ambient, size_t max_lights, LIGHTINFO* lightinfo_buffer, size_t& got_lights) = 0;
		public:
			virtual const SPtr<Tex2D>&		getLightsVisibility() = 0;
			virtual bool					recvShadow() const = 0;
		public:
			virtual const Vec4&				getControlColor() const = 0;
		};
	};
};

