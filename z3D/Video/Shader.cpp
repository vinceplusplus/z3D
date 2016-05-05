
#include "Shader.h"
#include "Gfx.h"

namespace z3D
{
	namespace Video
	{
		Shader::Shader(const wcs& filepath):ResBase(filepath) {}
		Shader::~Shader() {}
		//SPtr<Shader>			Shader::load(const wstring& vs,const wstring& ps)
		//{
		//	return Gfx::current()->createShader(vs,ps);
		//}
		SPtr<Shader>			Shader::loadFromSource(GfxEnum::SHADERTYPE type, const mbs& source)
		{
			return Gfx::current()->createShaderFromSource(type, source);
		}
		SPtr<Shader>			Shader::loadFromFile(GfxEnum::SHADERTYPE type, const wcs& filepath)
		{
			return Gfx::current()->createShaderFromFile(type, filepath);
		}
	};
};

