
#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3dx9shader.h>
#include <dxerr.h>

#include "predecl.h"

#if defined(Z3D_ENABLE_VIDEO_D3D)

#include "d3dShader.h"
#include "d3dGfx.h"

namespace z3D
{
	namespace Video
	{
		d3dShader::d3dShader(const wcs& filepath): Shader(filepath)
		{
			_type = GfxEnum::SHADERTYPE_VERTEX;
			_vs = NULL;
			_ct = NULL;
			_device = NULL;
		}
		d3dShader::~d3dShader()
		{
			if(_ct)
				_ct->Release();
			switch(_type)
			{
			case GfxEnum::SHADERTYPE_VERTEX:
				{
					if(_gfx.valid())
						_gfx.get_unsafe()->removeVSRef(_vs);
					if(_vs)
						_vs->Release();
				}
				break;
			case GfxEnum::SHADERTYPE_PIXEL:
				{
					if(_gfx.valid())
						_gfx.get_unsafe()->removePSRef(_ps);
					if(_ps)
						_ps->Release();
				}
				break;
			}
			if(_device)
				_device->Release();
		}
		SPtr<d3dShader>				d3dShader::loadFromSource(const SPtr<d3dGfx>& gfx, GfxEnum::SHADERTYPE type, const mbs& source)
		{
			Z_ASSERT( type == GfxEnum::SHADERTYPE_VERTEX || type == GfxEnum::SHADERTYPE_PIXEL );

			IDirect3DDevice9* device = gfx->device();

			///////////////////////////////////////////////////
			// don't use D3DXGetVertexShaderProfile() or D3DXGetPixelShaderProfile(), because different version of vertex and pixel shader may not be mixed to use, e.g. vs_3_0 cannot mix with ps_2_0 on GMA 950
			///////////////////////////////////////////////////

			const char* profile = (type == GfxEnum::SHADERTYPE_VERTEX ? gfx.cast<d3dGfx>()->getVSProfile() : gfx.cast<d3dGfx>()->getPSProfile());

			mbs compile_entry_name = "main";

			SPtr<d3dShader> p = new d3dShader(L"");
			mbs new_source = source + FORMATA(
				"\
technique shader_compilation_generated_technique\n\
{\n\
	pass p\n\
	{\n\
		%s = compile %s %s();\n\
	}\n\
}\n\
				",
				(type == GfxEnum::SHADERTYPE_VERTEX ? "VertexShader" : "PixelShader"),
				profile,
				compile_entry_name.c_str()
				);
			ID3DXEffect* effect = NULL;
			ID3DXBuffer* error = NULL;
			HRESULT ret = S_OK;
			if(FAILED(ret = D3DXCreateEffectEx(device, new_source.c_str(), (UINT)new_source.length(), NULL, NULL, NULL, D3DXSHADER_NO_PRESHADER, NULL, &effect, &error)))
			{
				if(error && error->GetBufferPointer())
					syslog << MBSTOWCS((const char*)error->GetBufferPointer());				// correct me if the type of the data can be determined
				if(error)
					error->Release();
				if(effect)
					effect->Release();
				return NULL;
			}

			device->AddRef();
			p->_gfx = gfx;
			p->_device = device;
			p->_type = type;
			p->_source = source;

			bool succ = false;
			do
			{
				D3DXHANDLE tech = effect->GetTechniqueByName("shader_compilation_generated_technique");
				if(!tech)
					break;
				D3DXHANDLE pass = effect->GetPass(tech, 0);
				if(!pass)
					break;

				D3DXPASS_DESC pass_desc;
				if(FAILED(ret = effect->GetPassDesc(pass, &pass_desc)))
					break;
				if(type == GfxEnum::SHADERTYPE_VERTEX)
				{
					if(FAILED(ret = device->CreateVertexShader(pass_desc.pVertexShaderFunction, &p->_vs)))
						break;
					if(FAILED(ret = D3DXGetShaderConstantTable(pass_desc.pVertexShaderFunction, &p->_ct)))
						break;
				}
				else
				{
					if(FAILED(ret = device->CreatePixelShader(pass_desc.pPixelShaderFunction, &p->_ps)))
						break;
					if(FAILED(ret = D3DXGetShaderConstantTable(pass_desc.pPixelShaderFunction, &p->_ct)))
						break;
				}

				if(true)
				{
					p->_constant_desc_storage.static_count = 0;

					ID3DXConstantTable* ct = p->_ct;
					D3DXCONSTANTTABLE_DESC desc;
					ct->GetDesc(&desc);
					for(size_t i = 0; i < (size_t)desc.Constants; ++i)
					{
						D3DXCONSTANT_DESC c;
						D3DXHANDLE handle = ct->GetConstant(NULL, (UINT)i);
						if(!handle)
							break;
						UINT buffer_count = 1;
						if(FAILED(ct->GetConstantDesc(handle, &c, &buffer_count)))
							break;
						if(!buffer_count)
							continue;
						if(c.RegisterSet == D3DXRS_SAMPLER)
							continue;
						if(c.Class == D3DXPC_OBJECT || c.Class == D3DXPC_STRUCT)
							continue;
						Z_ASSERT( c.RegisterIndex < 4096 );								// if so, need revise
						Z_ASSERT( c.RegisterCount < 4096 );								// if so, need revise
						Z_ASSERT( c.RegisterCount / c.Elements < 32 );					// if so, need revise
						Z_ASSERT( c.Class != D3DXPC_OBJECT );							// unsupported type
						Z_ASSERT( c.Class != D3DXPC_STRUCT );							// unsupported type
						if(p->_constant_desc_storage.static_count == _countof(p->_constant_desc_storage.static_storage))
						{
							if(p->_constant_desc_storage.static_count)
								p->_constant_desc_storage.dynamic_storage.insert(p->_constant_desc_storage.dynamic_storage.begin(), p->_constant_desc_storage.static_storage + 0, p->_constant_desc_storage.static_storage + p->_constant_desc_storage.static_count);
							// at least free the string memory
							for(size_t i = 0; i < p->_constant_desc_storage.static_count; ++i)
								p->_constant_desc_storage.static_storage[i].first = "";
							p->_constant_desc_storage.static_count = 0;
						}
						CONSTANT_DESC c2;
						c2.register_index = (int32_t)c.RegisterIndex;
						c2.register_count = (uint32_t)c.RegisterCount;
						c2.element_size = (uint32_t)(c.RegisterCount / c.Elements);
						switch(c.RegisterSet)
						{
						case D3DXRS_BOOL:
							c2.register_set = 2;
							break;
						case D3DXRS_INT4:
							c2.register_set = 1;
							break;
						case D3DXRS_FLOAT4:
						default:
							c2.register_set = 0;
							break;
						}
						c2.column_major = (uint32_t)(c.Class == D3DXPC_MATRIX_COLUMNS);
						if(p->_constant_desc_storage.dynamic_storage.size())
							p->_constant_desc_storage.dynamic_storage.push_back(pair<mbs, CONSTANT_DESC>(c.Name, c2));
						else
						{
							p->_constant_desc_storage.static_storage[p->_constant_desc_storage.static_count] = pair<mbs, CONSTANT_DESC>(c.Name, c2);
							++p->_constant_desc_storage.static_count;
						}
					}
					if(p->_constant_desc_storage.dynamic_storage.size())
					{
						p->_constant_descs = &*p->_constant_desc_storage.dynamic_storage.begin();
						p->_constant_desc_count = p->_constant_desc_storage.dynamic_storage.size();
					}
					else
					{
						p->_constant_descs = p->_constant_desc_storage.static_storage;
						p->_constant_desc_count = p->_constant_desc_storage.static_count;
					}
				}
				if(true)
				{
					p->_semantic_desc_storage.static_count = 0;

					D3DXEFFECT_DESC desc;
					effect->GetDesc(&desc);
					for(size_t i = 0; i < (size_t)desc.Parameters; ++i)
					{
						D3DXHANDLE param = effect->GetParameter(NULL, (UINT)i);
						if(!param)
							continue;
						if(!effect->IsParameterUsed(param, tech))
							continue;
						D3DXPARAMETER_DESC param_desc;
						effect->GetParameterDesc(param, &param_desc);
						if(!param_desc.Semantic)
							continue;
						CONSTANT_DESC c = p->getConstantDesc(param_desc.Name);
						if(c.register_index == (int32_t)-1)
							continue;

						SEMANTICIZED_CONSTANT_DESC sc;
						(CONSTANT_DESC&)sc = c;
						if(true)
						{
							sc.semantic = GfxEnum::SCS_UNDEFINED;
							sc.semantic_text = param_desc.Semantic;
							mbs text = TOLOWER(param_desc.Semantic);
							if(text == "world")
								sc.semantic = GfxEnum::SCS_WORLD;
							else if(text == "view")
								sc.semantic = GfxEnum::SCS_VIEW;
							else if(text == "projection")
								sc.semantic = GfxEnum::SCS_PROJECTION;
							else if(text == "worldview" || text == "world_view")
								sc.semantic = GfxEnum::SCS_WORLDVIEW;
							else if(text == "worldviewprojection" || text == "world_view_projection" || text == "worldviewproj" || text == "world_view_proj")
								sc.semantic = GfxEnum::SCS_WORLDVIEWPROJECTION;
							else if(text == "worldinversetranspose" || text == "world_inverse_transpose" || text == "worldnm" || text == "world_nm")
								sc.semantic = GfxEnum::SCS_WORLDINVERSETRANSPOSE;
							else if(text == "worldviewinversetranspose" || text == "world_view_inverse_transpose" || text == "worldviewnm" || text == "worldview_nm")
								sc.semantic = GfxEnum::SCS_WORLDVIEWINVERSETRANSPOSE;
							else if(text == "worldinverse" || text == "world_inverse" || text == "worldinv" || text == "world_inv")
								sc.semantic = GfxEnum::SCS_WORLDINVERSE;
							else if(text == "viewinverse" || text == "view_inverse" || text == "viewinv" || text == "view_inv")
								sc.semantic = GfxEnum::SCS_VIEWINVERSE;
							else if(text == "projectioninverse" || text == "projection_inverse" || text == "projinv" || text == "proj_inv")
								sc.semantic = GfxEnum::SCS_PROJECTIONINVERSE;
							else if(text == "worldviewprojectioninverse" || text == "world_view_projection_inverse" || text == "worldviewprojinv" || text == "world_view_proj_inv")
								sc.semantic = GfxEnum::SCS_WORLDVIEWPROJECTIONINVERSE;
							else if(text == "worldcameraposition" || text == "world_camera_position" || text == "campos" || text == "cam_pos")
								sc.semantic = GfxEnum::SCS_WORLDCAMERAPOSITION;
							else if(text == "localcameraposition" || text == "local_camera_position" || text == "localcampos" || text == "local_cam_pos")
								sc.semantic = GfxEnum::SCS_LOCALCAMERAPOSITION;
							else if(text == "timeinfo" || text == "time_info")
								sc.semantic = GfxEnum::SCS_TIMEINFO;
						}
						sc.semantic_text = param_desc.Semantic;
						if(p->_semantic_desc_storage.dynamic_storage.size())
							p->_semantic_desc_storage.dynamic_storage.push_back(pair<mbs, SEMANTICIZED_CONSTANT_DESC>(param_desc.Name, sc));
						else
						{
							p->_semantic_desc_storage.static_storage[p->_semantic_desc_storage.static_count] = pair<mbs, SEMANTICIZED_CONSTANT_DESC>(param_desc.Name, sc);
							++p->_semantic_desc_storage.static_count;
						}
					}
					if(p->_semantic_desc_storage.dynamic_storage.size())
					{
						p->_semantic_descs = &*p->_semantic_desc_storage.dynamic_storage.begin();
						p->_semantic_desc_count = p->_semantic_desc_storage.dynamic_storage.size();
					}
					else
					{
						p->_semantic_descs = p->_semantic_desc_storage.static_storage;
						p->_semantic_desc_count = p->_semantic_desc_storage.static_count;
					}
				}
				if(true)
				{
					class pred
					{
					public:
						bool					operator() (const pair<mbs, CONSTANT_DESC>& e0, const pair<mbs, CONSTANT_DESC>& e1) const
						{
							return e0.first < e1.first;
						}
					};
					std::sort(p->_constant_descs, p->_constant_descs + p->_constant_desc_count, pred());
					std::sort(p->_semantic_descs, p->_semantic_descs + p->_semantic_desc_count, pred());
				}

				succ = true;
			}
			while(false);
			effect->Release();
			if(!succ)
				return NULL;

			return p;
		}
		SPtr<d3dShader>				d3dShader::loadFromFile(const SPtr<d3dGfx>& gfx, GfxEnum::SHADERTYPE type, const wcs& filepath)
		{
			ResMgr* mgr = NULL;
			if(true)
			{
				switch(type)
				{
				case GfxEnum::SHADERTYPE_VERTEX:
					mgr = &gfx->getResMgrVS();
					break;
				case GfxEnum::SHADERTYPE_PIXEL:
					mgr = &gfx->getResMgrPS();
					break;
				default:
					Z_ASSERT( false );
					return NULL;
				}
				d3dShader* s = mgr->find<d3dShader>(filepath);
				if(s)
					return s;
			}

			mbs source = FileSystem::loadUTF8FromAll(filepath);
			if(!source.length())
				return NULL;
			SPtr<d3dShader> s = loadFromSource(gfx, type, source);
			if(!s)
				return NULL;
			s->set_filepath(filepath);
			mgr->add(s.get());

			return s;
		}
		IDirect3DVertexShader9*		d3dShader::vs() const
		{
			Z_ASSERT( _type == GfxEnum::SHADERTYPE_VERTEX );

			return _vs;
		}
		IDirect3DPixelShader9*		d3dShader::ps() const
		{
			Z_ASSERT( _type == GfxEnum::SHADERTYPE_PIXEL );

			return _ps;
		}
		GfxEnum::SHADERTYPE			d3dShader::type() const
		{
			return _type;
		}
		const mbs&					d3dShader::getSource() const
		{
			return _source;
		}
		ID3DXConstantTable*			d3dShader::ct() const
		{
			return _ct;
		}
		IDirect3DDevice9*			d3dShader::device() const
		{
			return _device;
		}
		Shader::CONSTANT_DESC		d3dShader::getConstantDesc(const mbs& name) const
		{
			return getConstantDesc(name.c_str());
		}
		Shader::CONSTANT_DESC		d3dShader::getConstantDesc(const char* name) const
		{
			class pred
			{
			public:
				bool					operator() (const char* e0, const pair<mbs, CONSTANT_DESC>& e1) const
				{
					return strcmp(e0, e1.first.c_str()) < 0;
				}
				bool					operator() (const pair<mbs, CONSTANT_DESC>& e0, const char* e1) const
				{
					return strcmp(e0.first.c_str(), e1) < 0;
				}
				bool					operator() (const pair<mbs, CONSTANT_DESC>& e0, const pair<mbs, CONSTANT_DESC>& e1) const
				{
					return e0.first < e1.first;
				}
			};
			pair<mbs, CONSTANT_DESC>* end = _constant_descs + _constant_desc_count;
			pair<mbs, CONSTANT_DESC>* iter = std::lower_bound(_constant_descs, end, name, pred());
			if(iter != end && strcmp(iter->first.c_str(), name) == 0)
				return iter->second;
			CONSTANT_DESC d;
			d.register_index = -1;
			d.register_count = 0;
			d.register_set = 0;
			d.column_major = 0;
			return d;
		}
		void						d3dShader::getSemanticizedConstants(pair<mbs, SEMANTICIZED_CONSTANT_DESC>*& descs, size_t& count) const
		{
			descs = _semantic_descs;
			count = _semantic_desc_count;
		}
	};
};

#endif


