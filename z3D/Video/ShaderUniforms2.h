
#pragma once

#include "predecl.h"
#include "GfxEnum.h"
#include "Shader.h"

namespace z3D
{
	namespace Video
	{
		class ShaderUniforms2
		{
		public:
			class BUFFERSET
			{
			public:
				class FLOAT4
				{
				public:
					float				buffer[512][4];
					size_t				effecting_count;
				}					float4;
				class INT4
				{
				public:
					int32_t				buffer[256][4];
					size_t				effecting_count;
				}					int4;
				class BOOLEAN
				{
				public:
					int32_t				buffer[256];
					size_t				effecting_count;
				}					boolean;
			};
			BUFFERSET*			buffersets;
		private:
			SPtr<Shader>		_shaders[2];
		private:
			static in_place_allocator<BUFFERSET[2]>
								_buffer_allocator;
		public:
			ShaderUniforms2(const SPtr<Shader>& vs, const SPtr<Shader>& ps);
			~ShaderUniforms2();
		public:
			void				reset(const SPtr<Shader>& vs, const SPtr<Shader>& ps);
			void				reset();
		public:
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, bool value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, int32_t value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, REAL value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec4& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat4& value);
		public:
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, bool value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, int32_t value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, REAL value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Vec2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Vec3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Vec4& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Mat2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Mat3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Mat4& value);
		public:
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, bool value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, int32_t value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, REAL value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Vec2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Vec3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Vec4& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Mat2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Mat3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Mat4& value);
		public:
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, bool value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, int32_t value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, REAL value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Vec2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Vec3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Vec4& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Mat2& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Mat3& value);
			void				set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Mat4& value);
		public:
								// example setter code can be found in MeshRenderer.cpp
			template<class T>
			void				setSemanticizedConstants(T& setter)
			{
				if(true)
				{
					pair<mbs, Shader::SEMANTICIZED_CONSTANT_DESC>* descs;
					size_t count;
					_shaders[0]->getSemanticizedConstants(descs, count);
					for(size_t i = 0; i < count; ++i)
						setter.T::set(*this, GfxEnum::SHADERTYPE_VERTEX, descs[i].first, descs[i].second);
				}
				if(true)
				{
					pair<mbs, Shader::SEMANTICIZED_CONSTANT_DESC>* descs;
					size_t count;
					_shaders[1]->getSemanticizedConstants(descs, count);
					for(size_t i = 0; i < count; ++i)
						setter.T::set(*this, GfxEnum::SHADERTYPE_VERTEX, descs[i].first, descs[i].second);
				}
			}
		};
	};
};

