
#define _CRT_SECURE_NO_WARNINGS 1

#include "ShaderUniforms.h"

namespace z3D
{
	namespace Video
	{
		ShaderUniforms::ShaderUniforms()
		{
			_entries = 0;
			_value_ptr = _value_buffer;
			_value_buffer_end = _value_buffer + _countof(_value_buffer);
		}
		void						ShaderUniforms::clear()
		{
			_entries = 0;
			_value_ptr = _value_buffer;
			_value_buffer_end = _value_buffer + _countof(_value_buffer);
		}
		bool						ShaderUniforms::__set(GfxEnum::SHADERTYPE shader_type, const char* name, const void* value, size_t count, size_t total_size, uint8_t type, bool array_form)
		{
			Z_ASSERT( _entries < (size_t)MAX_ENTRIES );
			Z_ASSERT( _value_ptr + ((total_size + sizeof(size_t) - 1) / sizeof(size_t) * sizeof(size_t)) <= _value_buffer_end );
			Z_ASSERT( strlen(name) < (size_t)NAME_SIZE );
			Z_ASSERT( count <= (size_t)256 );

			if(_entries >= MAX_ENTRIES || _value_ptr + ((total_size + sizeof(size_t) - 1) / sizeof(size_t) * sizeof(size_t)) > _value_buffer_end || strlen(name) >= (size_t)NAME_SIZE || count > (size_t)256)
				return false;

			ENTRY& e = _array_entry[_entries];
			strcpy(e.name, name);
			e.shader_type = (uint8_t)shader_type;
			e.type = type;
			e.count = (uint8_t)(count - 1);
			e.array_form = array_form;
			e.value_addr = _value_ptr - _value_buffer;

			memcpy(_value_ptr, value, total_size);
			_value_ptr += ((total_size + sizeof(size_t) - 1) / sizeof(size_t) * sizeof(size_t));

			++_entries;

			return true;
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, bool value)
		{
			return __set(shader_type, name, &value, 1, sizeof(value), TYPE_BOOL, false);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, int value)
		{
			return __set(shader_type, name, &value, 1, sizeof(value), TYPE_INTEGER, false);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, REAL value)
		{
			return __set(shader_type, name, &value, 1, sizeof(value), TYPE_REAL, false);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const bool* value, size_t count)
		{
			return __set(shader_type, name, value, count, sizeof(*value) * count, TYPE_BOOL, true);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const int* value, size_t count)
		{
			return __set(shader_type, name, value, count, sizeof(*value) * count, TYPE_INTEGER, true);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const REAL* value, size_t count)
		{
			return __set(shader_type, name, value, count, sizeof(*value) * count, TYPE_REAL, true);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec2& value)
		{
			return __set(shader_type, name, &value, sizeof(value) / sizeof(REAL), sizeof(value), TYPE_REAL, true);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec3& value)
		{
			return __set(shader_type, name, &value, sizeof(value) / sizeof(REAL), sizeof(value), TYPE_REAL, true);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec4& value)
		{
			return __set(shader_type, name, &value, sizeof(value) / sizeof(REAL), sizeof(value), TYPE_REAL, true);
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat2& value, bool transpose)
		{
			if(!transpose)
				return __set(shader_type, name, &value, sizeof(value) / sizeof(REAL), sizeof(value), TYPE_REAL, true);
			else
			{
				Mat2 v = value.transpose();
				return __set(shader_type, name, &v, sizeof(v) / sizeof(REAL), sizeof(v), TYPE_REAL, true);
			}
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat3& value, bool transpose)
		{
			if(!transpose)
				return __set(shader_type, name, &value, sizeof(value) / sizeof(REAL), sizeof(value), TYPE_REAL, true);
			else
			{
				Mat3 v = value.transpose();
				return __set(shader_type, name, &v, sizeof(v) / sizeof(REAL), sizeof(v), TYPE_REAL, true);
			}
		}
		bool						ShaderUniforms::set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat4& value, bool transpose)
		{
			if(!transpose)
				return __set(shader_type, name, &value, sizeof(value) / sizeof(REAL), sizeof(value), TYPE_REAL, true);
			else
			{
				Mat4 v = value.transpose();
				return __set(shader_type, name, &v, sizeof(v) / sizeof(REAL), sizeof(v), TYPE_REAL, true);
			}
		}
		const ShaderUniforms::ENTRY*	ShaderUniforms::array_entry() const
		{
			return _array_entry;
		}
		size_t						ShaderUniforms::entries() const
		{
			return _entries;
		}
		const uint8_t*				ShaderUniforms::value_buffer() const
		{
			return _value_buffer;
		}
	};
};

