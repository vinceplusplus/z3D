
#pragma once

#include "predecl.h"
#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class ShaderUniforms
		{
		public:
			enum
			{
				BUFFER_SIZE = 2048,
				MAX_ENTRIES = 64,
				NAME_SIZE = 32,
			};
			enum: uint8_t
			{
				TYPE_BOOL = 0,
				TYPE_INTEGER,
				TYPE_REAL,
			};
			typedef struct _ENTRY
			{
				char						name[ NAME_SIZE ];
				ptrdiff_t					value_addr;
				uint8_t						shader_type;
				uint8_t						type;
				uint8_t						count;								// 1 + real count
				uint8_t						array_form;
			} ENTRY;
		private:
			ENTRY						_array_entry [ MAX_ENTRIES ];
			size_t						_entries;
			uint8_t						_value_buffer[ BUFFER_SIZE ];
			uint8_t*					_value_ptr;
			uint8_t*					_value_buffer_end;
		public:
			ShaderUniforms();
		public:
			void						clear();
		private:
			bool						__set(GfxEnum::SHADERTYPE shader_type, const char* name, const void* value, size_t count, size_t total_size, uint8_t type, bool array_form);
		public:
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, bool value);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, int value);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, REAL value);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const bool* value, size_t count);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const int* value, size_t count);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const REAL* value, size_t count);
		public:
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec2& value);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec3& value);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const Vec4& value);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat2& value, bool transpose = false);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat3& value, bool transpose = false);
			bool						set(GfxEnum::SHADERTYPE shader_type, const char* name, const Mat4& value, bool transpose = false);
		public:
			const ENTRY*				array_entry() const;
			size_t						entries() const;
			const uint8_t*				value_buffer() const;
		};
	};
};

