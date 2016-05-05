
#pragma once

#include "predecl.h"
#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class ShaderGen
		{
		public:
			virtual ~ShaderGen() {}
		public:
			virtual SPtr<Shader>			genVS(const ShaderGenConf& cfg) = 0;
			virtual SPtr<Shader>			genPS(const ShaderGenConf& cfg) = 0;
			virtual void					setShaderUniforms(const ShaderGenConf& cfg, ShaderUniforms& unis, bool vs = true, bool ps = true) = 0;
			virtual void					setShaderUniforms(const ShaderGenConf& cfg, ShaderUniforms2& unis, bool vs = true, bool ps = true) = 0;
		public:
			virtual void					clear() = 0;
		};
	};
};

