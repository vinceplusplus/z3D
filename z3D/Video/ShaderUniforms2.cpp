
#include "ShaderUniforms2.h"

#include "../Core/allocators_inl.h"

#include "Shader.h"

namespace z3D
{
	namespace Video
	{
		ShaderUniforms2::ShaderUniforms2(const SPtr<Shader>& vs, const SPtr<Shader>& ps)
		{
			// BUFFERSET must not have any constructor or destructor
			buffersets = (BUFFERSET*)_buffer_allocator.allocate(1);
			reset(vs, ps);
		}
		ShaderUniforms2::~ShaderUniforms2()
		{
			_buffer_allocator.deallocate((BUFFERSET(*)[2])buffersets, 1);
		}
		void				ShaderUniforms2::reset(const SPtr<Shader>& vs, const SPtr<Shader>& ps)
		{
			reset();
			_shaders[0] = vs;
			_shaders[1] = ps;
		}
		void				ShaderUniforms2::reset()
		{
			buffersets[0].float4.effecting_count = 0;
			buffersets[0].int4.effecting_count = 0;
			buffersets[0].boolean.effecting_count = 0;

			buffersets[1].float4.effecting_count = 0;
			buffersets[1].int4.effecting_count = 0;
			buffersets[1].boolean.effecting_count = 0;
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, bool value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, int32_t value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, REAL value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec2& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec3& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec4& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat2& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat3& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat4& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, bool value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, int32_t value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, REAL value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Vec2& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Vec3& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Vec4& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Mat2& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Mat3& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const char* name, size_t element, const Mat4& value)
		{
			Z_ASSERT( shader_type < 2 );
			Shader::CONSTANT_DESC desc;
			desc = _shaders[shader_type]->getConstantDesc(name);

			set(shader_type, desc, element, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, bool value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			if(cd.register_set == 2)
				buffersets[shader_type].boolean.buffer[cd.register_index] = (int32_t)value;
			else
			{
				switch(cd.register_set)
				{
				case 0:
					return set(shader_type, cd, (REAL)value);
				case 1:
					return set(shader_type, cd, (int32_t)value);
				}
			}

			buffersets[shader_type].boolean.effecting_count = max(buffersets[shader_type].boolean.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, int32_t value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			if(cd.register_set == 1)
				buffersets[shader_type].int4.buffer[cd.register_index][0] = value;
			else
			{
				switch(cd.register_set)
				{
				case 0:
					return set(shader_type, cd, (REAL)value);
				case 2:
					return set(shader_type, cd, (bool)(value ? true : false));
				}
			}

			buffersets[shader_type].int4.effecting_count = max(buffersets[shader_type].int4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, REAL value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			if(cd.register_set == 0)
				buffersets[shader_type].float4.buffer[cd.register_index][0] = value;
			else
			{
				switch(cd.register_set)
				{
				case 1:
					return set(shader_type, cd, (int32_t)value);
				case 2:
					return set(shader_type, cd, (bool)(value ? true : false));
				}
			}

			buffersets[shader_type].float4.effecting_count = max(buffersets[shader_type].float4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Vec2& value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			Z_ASSERT( cd.register_set == 0 );
			*(Vec2*)buffersets[shader_type].float4.buffer[cd.register_index] = value;

			buffersets[shader_type].float4.effecting_count = max(buffersets[shader_type].float4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Vec3& value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			Z_ASSERT( cd.register_set == 0 );
			*(Vec3*)buffersets[shader_type].float4.buffer[cd.register_index] = value;

			buffersets[shader_type].float4.effecting_count = max(buffersets[shader_type].float4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Vec4& value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			Z_ASSERT( cd.register_set == 0 );
			*(Vec4*)buffersets[shader_type].float4.buffer[cd.register_index] = value;

			buffersets[shader_type].float4.effecting_count = max(buffersets[shader_type].float4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Mat2& value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			Z_ASSERT( cd.register_set == 0 );
			if(!cd.column_major)
			{
				typedef float FLOAT4[4];
				switch(cd.register_count)
				{
				case 0:
					break;
				case 1:
					*(Vec2*)buffersets[shader_type].float4.buffer[cd.register_index] = *(Vec2*)value[0];
					break;
				case 2:
				default:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						*(Vec2*)ptr[0] = *(Vec2*)value[0];
						*(Vec2*)ptr[1] = *(Vec2*)value[1];
					}
					break;
				}
			}
			else
			{
				typedef float FLOAT4[4];
				switch(cd.register_count)
				{
				case 0:
					break;
				case 1:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];
					}
					break;
				case 2:
				default:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];
						ptr[1][0] = value[0][1];	ptr[1][1] = value[1][1];
					}
					break;
				}
			}

			buffersets[shader_type].float4.effecting_count = max(buffersets[shader_type].float4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Mat3& value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			Z_ASSERT( cd.register_set == 0 );
			if(!cd.column_major)
			{
				typedef float FLOAT4[4];
				switch(cd.register_count)
				{
				case 0:
					break;
				case 1:
					*(Vec3*)buffersets[shader_type].float4.buffer[cd.register_index] = *(Vec3*)value[0];
					break;
				case 2:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						*(Vec3*)ptr[0] = *(Vec3*)value[0];
						*(Vec3*)ptr[1] = *(Vec3*)value[1];
					}
					break;
				case 3:
				default:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						*(Vec3*)ptr[0] = *(Vec3*)value[0];
						*(Vec3*)ptr[1] = *(Vec3*)value[1];
						*(Vec3*)ptr[2] = *(Vec3*)value[2];
					}
					break;
				}
			}
			else
			{
				typedef float FLOAT4[4];
				switch(cd.register_count)
				{
				case 0:
					break;
				case 1:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];	ptr[0][2] = value[2][0];
					}
					break;
				case 2:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];	ptr[0][2] = value[2][0];
						ptr[1][0] = value[0][1];	ptr[1][1] = value[1][1];	ptr[1][2] = value[2][1];
					}
					break;
				case 3:
				default:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];	ptr[0][2] = value[2][0];
						ptr[1][0] = value[0][1];	ptr[1][1] = value[1][1];	ptr[1][2] = value[2][1];
						ptr[2][0] = value[0][2];	ptr[2][1] = value[1][2];	ptr[2][2] = value[2][2];
					}
					break;
				}
			}

			buffersets[shader_type].float4.effecting_count = max(buffersets[shader_type].float4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, const Mat4& value)
		{
			if(cd.register_index == (int32_t)-1)
			{
				Z_ASSERT( false );
				return;
			}
			Z_ASSERT( cd.register_set == 0 );
			if(!cd.column_major)
			{
				typedef float FLOAT4[4];
				FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
				switch(cd.register_count)
				{
				case 0:
					break;
				case 1:
					memcpy(ptr[0], &value, sizeof(Vec4) * 1);
					break;
				case 2:
					memcpy(ptr[0], &value, sizeof(Vec4) * 2);
					break;
				case 3:
					memcpy(ptr[0], &value, sizeof(Vec4) * 3);
					break;
				case 4:
				default:
					memcpy(ptr[0], &value, sizeof(Vec4) * 4);
					break;
				}
			}
			else
			{
				typedef float FLOAT4[4];
				switch(cd.register_count)
				{
				case 0:
					break;
				case 1:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];	ptr[0][2] = value[2][0];	ptr[0][3] = value[3][0];
					}
					break;
				case 2:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];	ptr[0][2] = value[2][0];	ptr[0][3] = value[3][0];
						ptr[1][0] = value[0][1];	ptr[1][1] = value[1][1];	ptr[1][2] = value[2][1];	ptr[1][3] = value[3][1];
					}
					break;
				case 3:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];	ptr[0][2] = value[2][0];	ptr[0][3] = value[3][0];
						ptr[1][0] = value[0][1];	ptr[1][1] = value[1][1];	ptr[1][2] = value[2][1];	ptr[1][3] = value[3][1];
						ptr[2][0] = value[0][2];	ptr[2][1] = value[1][2];	ptr[2][2] = value[2][2];	ptr[2][3] = value[3][2];
					}
					break;
				case 4:
				default:
					{
						FLOAT4* ptr = (FLOAT4*)buffersets[shader_type].float4.buffer[cd.register_index];
						ptr[0][0] = value[0][0];	ptr[0][1] = value[1][0];	ptr[0][2] = value[2][0];	ptr[0][3] = value[3][0];
						ptr[1][0] = value[0][1];	ptr[1][1] = value[1][1];	ptr[1][2] = value[2][1];	ptr[1][3] = value[3][1];
						ptr[2][0] = value[0][2];	ptr[2][1] = value[1][2];	ptr[2][2] = value[2][2];	ptr[2][3] = value[3][2];
						ptr[3][0] = value[0][3];	ptr[3][1] = value[1][3];	ptr[3][2] = value[2][3];	ptr[3][3] = value[3][3];
					}
					break;
				}
			}

			buffersets[shader_type].float4.effecting_count = max(buffersets[shader_type].float4.effecting_count, cd.register_index + cd.register_count);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, bool value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, int32_t value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, REAL value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Vec2& value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Vec3& value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Vec4& value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Mat2& value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Mat3& value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
		void				ShaderUniforms2::set(GfxEnum::SHADERTYPE shader_type, const Shader::CONSTANT_DESC& cd, size_t element, const Mat4& value)
		{
			Shader::CONSTANT_DESC cd2 = cd;
			cd2.register_count = cd2.element_size;
			cd2.register_index += cd2.element_size * element;

			set(shader_type, cd2, value);
		}
	};
};

