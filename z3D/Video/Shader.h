
#pragma once

#include "predecl.h"

#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class Shader: public ResBase
		{
		public:
			DECL_TYPEINFO();
		public:
			class CONSTANT_DESC
			{
			public:
				int32_t						register_index : 12;
				uint32_t					register_count : 12;
				uint32_t					element_size : 5;
				uint32_t					register_set : 2;
				uint32_t					column_major : 1;
			};

			class SEMANTICIZED_CONSTANT_DESC: public CONSTANT_DESC
			{
			public:
				GfxEnum::SHADER_CONSTANT_SEMANTIC
											semantic;
				mbs							semantic_text;
			};

		protected:
			Shader(const wcs& filepath);
			~Shader();
		public:
			static SPtr<Shader>			loadFromSource(GfxEnum::SHADERTYPE type, const mbs& source);
			static SPtr<Shader>			loadFromFile(GfxEnum::SHADERTYPE type, const wcs& filepath);
		public:
			virtual GfxEnum::SHADERTYPE	type() const = 0;
		public:
			virtual const mbs&			getSource() const = 0;
		public:
			virtual CONSTANT_DESC		getConstantDesc(const char* name) const = 0;
			virtual CONSTANT_DESC		getConstantDesc(const mbs& name) const = 0;
		public:
			virtual void				getSemanticizedConstants(pair<mbs, SEMANTICIZED_CONSTANT_DESC>*& descs, size_t& count) const = 0;
		};
	};
};

