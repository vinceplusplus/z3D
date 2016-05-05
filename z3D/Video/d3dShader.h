
#pragma once

#include "predecl.h"
#include "Shader.h"

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3d9types.h>

typedef interface ID3DXBuffer ID3DXBuffer;
typedef interface ID3DXConstantTable ID3DXConstantTable;

namespace z3D
{
	namespace Video
	{
		class d3dGfx;

		class d3dShader: public Shader
		{
		public:
			DECL_TYPEINFO();
		private:
			WPtr<d3dGfx>				_gfx;
			GfxEnum::SHADERTYPE			_type;
		private:
			union
			{
				IDirect3DVertexShader9*		_vs;
				IDirect3DPixelShader9*		_ps;
			};
			ID3DXConstantTable*			_ct;
			IDirect3DDevice9*			_device;
		private:
			class CONSTANT_DESC_STORAGE
			{
			public:
				pair<mbs, CONSTANT_DESC>	static_storage[32];
				size_t						static_count;
				vector<pair<mbs, CONSTANT_DESC>>
											dynamic_storage;
			}							_constant_desc_storage;

			pair<mbs, CONSTANT_DESC>*	_constant_descs;
			size_t						_constant_desc_count;

			class SEMANTICIZED_CONSTANT_DESC_STORAGE
			{
			public:
				pair<mbs, SEMANTICIZED_CONSTANT_DESC>
											static_storage[32];
				size_t						static_count;
				vector<pair<mbs, SEMANTICIZED_CONSTANT_DESC>>
											dynamic_storage;
			}							_semantic_desc_storage;

			pair<mbs, SEMANTICIZED_CONSTANT_DESC>*
										_semantic_descs;
			size_t						_semantic_desc_count;
		private:
			mbs							_source;
		protected:
			d3dShader(const wcs& filepath);
			~d3dShader();
		public:
			static SPtr<d3dShader>		loadFromSource(const SPtr<d3dGfx>& gfx, GfxEnum::SHADERTYPE type, const mbs& source);
			static SPtr<d3dShader>		loadFromFile(const SPtr<d3dGfx>& gfx, GfxEnum::SHADERTYPE type, const wcs& filepath);
		public:
			IDirect3DVertexShader9*		vs() const;
			IDirect3DPixelShader9*		ps() const;
		public:
			GfxEnum::SHADERTYPE			type() const;
		public:
			const mbs&					getSource() const;
		public:
			ID3DXConstantTable*			ct() const;
		public:
			IDirect3DDevice9*			device() const;
		public:
			CONSTANT_DESC				getConstantDesc(const mbs& name) const;
			CONSTANT_DESC				getConstantDesc(const char* name) const;
		public:
			void						getSemanticizedConstants(pair<mbs, SEMANTICIZED_CONSTANT_DESC>*& descs, size_t& count) const;
		};
	};
};

