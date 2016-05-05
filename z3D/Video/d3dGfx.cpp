
#define _CRT_SECURE_NO_WARNINGS 1

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3dx9shader.h>

#include "predecl.h"

#if defined(Z3D_ENABLE_VIDEO_D3D)

#include "../Core/allocators_inl.h"

#include "rsView.h"
#include "rsShade.h"

#include "Jotter.h"
#include "ShaderUniforms.h"
#include "ShaderUniforms2.h"

#include "d3dGfx.h"
#include "d3dVBO.h"
#include "d3dIBO.h"
#include "d3dTex2D.h"
#include "d3dTexCube.h"
#include "d3dShader.h"
#include "d3dTexCube.h"
#include "d3dRenderbuffer.h"

#include "../Core/nt_mem_pool_inl.h"

// calculation results in 1.8446743e+019, minus a small value to guard against round-off
#define FLT_MAX_SQRT		1.8446742e+019F

#undef min
#undef max

namespace z3D
{
	namespace Video
	{
		template<D3DRENDERSTATETYPE state, DWORD default_value>
		void					d3dRS<state, default_value>::init(IDirect3DDevice9* device)
		{
			device->SetRenderState(state, _value = default_value);
		}
		template<D3DRENDERSTATETYPE state, DWORD default_value>
		void					d3dRS<state, default_value>::set(IDirect3DDevice9* device, DWORD value)
		{
			if(value != _value)
				device->SetRenderState(state, _value=value);
		}

		void					d3dTextureState::init(IDirect3DDevice9* device, DWORD stage)
		{
			device->SetTexture(stage, _value = NULL);
		}
		void					d3dTextureState::set(IDirect3DDevice9* device, DWORD stage, IDirect3DBaseTexture9* value)
		{
			if(_value != value)
				device->SetTexture(stage, _value = value);
		}
		IDirect3DBaseTexture9*	d3dTextureState::get() const
		{
			return _value;
		}

		template<D3DTEXTURESTAGESTATETYPE type, DWORD default_value>
		void					d3dTSS::TSSS<type, default_value>::init(IDirect3DDevice9* device, DWORD stage)
		{
			device->SetTextureStageState(stage, type, _value = default_value);
		}
		template<D3DTEXTURESTAGESTATETYPE type, DWORD default_value>
		void					d3dTSS::TSSS<type, default_value>::set(IDirect3DDevice9* device, DWORD stage, DWORD value)
		{
			if(value != _value)
				device->SetTextureStageState(stage, type, _value = value);
		}

		void					d3dTSS::init(IDirect3DDevice9* device, DWORD stage)
		{
			color_op.init(device, stage);
			color_arg1.init(device, stage);
			color_arg2.init(device, stage);
			alpha_op.init(device, stage);
			alpha_arg1.init(device, stage);
			alpha_arg2.init(device, stage);
		}

		void					d3dTCT::init(IDirect3DDevice9* device, DWORD stage)
		{
			REAL value[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
			memcpy(_value, value, sizeof(value));
			device->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), (const D3DMATRIX*)value);
		}
		void					d3dTCT::set(IDirect3DDevice9* device, DWORD stage, const REAL value[4][4])
		{
			if(memcmp(_value, value, sizeof(value)))
			{
				memcpy(_value, value, sizeof(value));
				device->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), (const D3DMATRIX*)value);
			}
		}

		template<D3DSAMPLERSTATETYPE type, DWORD default_value>
		void					d3dSS::SSS<type, default_value>::init(IDirect3DDevice9* device, DWORD stage)
		{
			device->SetSamplerState(stage, type, _value = default_value);
		}
		template<D3DSAMPLERSTATETYPE type, DWORD default_value>
		void					d3dSS::SSS<type, default_value>::set(IDirect3DDevice9* device, DWORD stage, DWORD value)
		{
			if(value != _value)
				device->SetSamplerState(stage, type, _value = value);
		}

		void					d3dSS::init(IDirect3DDevice9* device, DWORD stage)
		{
			wrap_u.init(device, stage);
			wrap_v.init(device, stage);
			wrap_w.init(device, stage);
			filter_min.init(device, stage);
			filter_mag.init(device, stage);
			filter_mip.init(device, stage);
		}

		void				d3dLightState::init(IDirect3DDevice9* device, DWORD index)
		{
			D3DLIGHT9 v = {D3DLIGHT_DIRECTIONAL, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 0}, {0, 0, -1}, FLT_MAX_SQRT, 1.0f, 1, 0, 0, 0, 0};
			device->SetLight(index, &(_value = v));
		}
		void				d3dLightState::set(IDirect3DDevice9* device, DWORD index, const D3DLIGHT9& value)
		{
			if(memcmp(&_value, &value, sizeof(D3DLIGHT9)))
				device->SetLight(index, &(_value = value));
		}

		void				d3dLightEnableState::init(IDirect3DDevice9* device, DWORD index)
		{
			device->LightEnable(index, _value = FALSE);
		}
		void				d3dLightEnableState::set(IDirect3DDevice9* device, DWORD index, BOOL value)
		{
			if(_value != value)
				device->LightEnable(index, _value = value);
		}

		void				d3dClipPlaneState::init(IDirect3DDevice9* device, DWORD index)
		{
			_value[0] = 0; _value[1] = 0; _value[2] = 1; _value[3] = 0;
			device->SetClipPlane(index, _value);
		}
		void				d3dClipPlaneState::set(IDirect3DDevice9* device, DWORD index, const float value[4])
		{
			if(memcmp(_value, value, sizeof(_value)))
			{
				memcpy(_value, value, sizeof(_value));
				device->SetClipPlane(index, _value);
			}
		}

		void				d3dScissorState::init(IDirect3DDevice9* device)
		{
			RECT v = {0, 0, 1, 1};
			device->SetScissorRect(&(_value = v));
		}
		void				d3dScissorState::set(IDirect3DDevice9* device, const RECT& value)
		{
			if(memcmp(&_value, &value, sizeof(_value)))
				device->SetScissorRect(&(_value = value));
		}

		void				d3dViewportState::init(IDirect3DDevice9* device)
		{
			device->GetViewport(&_value);
		}
		void				d3dViewportState::set(IDirect3DDevice9* device, const D3DVIEWPORT9& value)
		{
			if(memcmp(&_value, &value, sizeof(_value)))
				device->SetViewport(&(_value = value));
		}

		void				d3dMaterialState::init(IDirect3DDevice9* device)
		{
			D3DMATERIAL9 v = {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, 1};
			device->SetMaterial(&(_value = v));
		}
		void				d3dMaterialState::set(IDirect3DDevice9* device, const D3DMATERIAL9& value)
		{
			if(memcmp(&_value, &value, sizeof(_value)))
				device->SetMaterial(&(_value = value));
		}

		void				d3dVertexDeclarationState::init(IDirect3DDevice9* device)
		{
			device->SetVertexDeclaration(_value = NULL);
		}
		void				d3dVertexDeclarationState::set(IDirect3DDevice9* device, IDirect3DVertexDeclaration9* value)
		{
			if(_value != value)
				device->SetVertexDeclaration(_value = value);
		}

		void							d3dVertexShaderState::init(IDirect3DDevice9* device)
		{
			device->SetVertexShader(_value = NULL);
		}
		void							d3dVertexShaderState::set(IDirect3DDevice9* device, IDirect3DVertexShader9* value)
		{
			if(_value != value)
				device->SetVertexShader(_value = value);
		}
		IDirect3DVertexShader9*			d3dVertexShaderState::get() const
		{
			return _value;
		}

		void							d3dPixelShaderState::init(IDirect3DDevice9* device)
		{
			device->SetPixelShader(_value = NULL);
		}
		void							d3dPixelShaderState::set(IDirect3DDevice9* device, IDirect3DPixelShader9* value)
		{
			if(_value != value)
				device->SetPixelShader(_value = value);
		}
		IDirect3DPixelShader9*			d3dPixelShaderState::get() const
		{
			return _value;
		}


		id3dRecreatable::LIST_TRACKING::LIST_TRACKING()
		{
			_list = NULL;
		}
		id3dRecreatable::LIST_TRACKING::~LIST_TRACKING()
		{
#ifndef _MSC_VER
#	error this impl only supports msvc stl impl
#else
#	if _MSC_VER != 1400 && _MSC_VER != 1500
#		error currently, only msvc 2005 and msvc 2008 are supported
#	endif
#endif
			if(_list)
				_list->erase(_iter);
		}

		id3dRecreatable::~id3dRecreatable() {}

		bool d3dGfx::ALL_VERTEX_ATTRIB_BINDING_less::operator()(const d3dGfx::ALL_VERTEX_ATTRIB_BINDING& left, const d3dGfx::ALL_VERTEX_ATTRIB_BINDING& right) const
		{
			return memcmp(&left, &right, sizeof(left)) < 0;
		}

		d3dGfx::d3dGfx()
			:	_recreate_list_mp(temp_int<64>::temp_int(), _recreate_list_mp_storage),
				_list_recreatable(_recreate_list_mp.get()),
				_vertex_decl_map_mp(temp_int<128>::temp_int(), _vertex_decl_map_mp_storage),
				_map_d3d_vertex_decl(VERTEX_DECL_MAP_TYPE::key_compare(), _vertex_decl_map_mp.get())
		{
			_framebuffer_dx = 1;
			_framebuffer_dy = 1;

			_bound_rendertarget = false;
			_bound_depthstencil = false;

			resetAccumPolycount();

			_shader_gen._gfx = this;

			_xform_dirties.FLAGS = ~((size_t)0);

			Z_STATIC_ASSERT( _countof(_rt_n_ds_stack.rts) == _countof(_rt_n_ds_stack.dss), ASSUMPTION_WRONG );

			memset(&_rt_n_ds_stack, 0, sizeof(_rt_n_ds_stack));
		}

		d3dGfx::~d3dGfx()
		{
			while(_list_recreatable.size())
			{
				D3D_RECREATABLE_LIST_TYPE::iterator iter = _list_recreatable.begin();
				(*iter)->list_tracking._list = NULL;
				_list_recreatable.erase(iter);
			}

			for(size_t i = 0; i < _rt_n_ds_stack.size; ++i)
			{
				for(size_t j = 0; j < _countof(_rt_n_ds_stack.rts[i]); ++j)
					if(_rt_n_ds_stack.rts[i][j])
						_rt_n_ds_stack.rts[i][j]->Release();
				if(_rt_n_ds_stack.dss[i])
					_rt_n_ds_stack.dss[i]->Release();
			}

			if(_query_flush)
				_query_flush->Release();

			if(_back_buffer_color)
				_back_buffer_color->Release();
			if(_back_buffer_depthstencil)
				_back_buffer_depthstencil->Release();

			clearD3DVertexDecls();

			if(_device)
				_device->Release();
			if(_d3d)
				_d3d->Release();
		}
		IDirect3D9*				d3dGfx::d3d() const {return _d3d;}
		IDirect3DDevice9*		d3dGfx::device() const {return _device;}
		const D3DCAPS9&			d3dGfx::caps() const {return _caps;}
		SPtr<d3dGfx>			d3dGfx::create(HWND hwnd, bool vsync)
		{
			IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
			if(!d3d)
				return NULL;
			int k = d3d->GetAdapterCount();
			D3DPRESENT_PARAMETERS pp;
			memset(&pp, 0, sizeof(pp));
			if(false)
			{
				RECT rt;
				GetClientRect(hwnd, &rt);
				pp.BackBufferWidth = rt.right;
				pp.BackBufferHeight = rt.bottom;
			}
			pp.Windowed = TRUE;
			pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			pp.BackBufferFormat = D3DFMT_X8R8G8B8;
			pp.BackBufferCount = 1;
			pp.EnableAutoDepthStencil = TRUE;
			pp.AutoDepthStencilFormat = D3DFMT_D24S8;
			if(vsync)
				pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			else
				pp.PresentationInterval = /*D3DPRESENT_DONOTWAIT | */D3DPRESENT_INTERVAL_IMMEDIATE;

			char multisample_types[16];
			size_t multisample_type_count = 0;

			for(char i = 0; i < 16; ++i)
			{
				DWORD levels = 0;
				if(SUCCEEDED(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pp.BackBufferFormat, true, (D3DMULTISAMPLE_TYPE)i, &levels)))
					if(SUCCEEDED(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pp.AutoDepthStencilFormat, true, D3DMULTISAMPLE_8_SAMPLES, &levels)))
					{
						multisample_types[multisample_type_count] = i;
						++multisample_type_count;
					}
			}

			size_t multisamples = 16;
			if(multisamples != 0)
			{
				char* end = multisample_types + multisample_type_count;
				char* iter = lower_bound(multisample_types, end, (char)multisamples);

				if(iter == end)
					if(multisample_type_count)
						iter = end - 1;

				if(iter != end)
				{
					if(*iter == multisamples)
						pp.MultiSampleType = (D3DMULTISAMPLE_TYPE)*iter;
					else
						pp.MultiSampleType = (D3DMULTISAMPLE_TYPE)*iter;
				}
			}

			pp.MultiSampleType = D3DMULTISAMPLE_NONE;
			pp.MultiSampleQuality = 0;

			pp.Flags = D3DPRESENTFLAG_DEVICECLIP;
			// do not use D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL, change of depthstencil surface may occur in the middle during rendering the whole scene
			//pp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

			D3DCAPS9 caps;
			d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

			IDirect3DDevice9* device = NULL;
			DWORD behavior_flags = 0;
			if((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0 && (caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)))
				behavior_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
			else
				behavior_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			d3d->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				hwnd,
				behavior_flags,
				&pp,
				&device
				);
			if(!device)
			{
				d3d->Release();
				return NULL;
			}

			SPtr<d3dGfx> gfx = new d3dGfx();
			gfx->_hwnd = hwnd;
			gfx->_hwnd_present = hwnd;
			gfx->_d3d = d3d;
			gfx->_device = device;
			gfx->_pp = pp;
			device->GetDeviceCaps(&gfx->_caps);
			if(!(gfx->_caps.TextureCaps & D3DPTEXTURECAPS_POW2) && !(gfx->_caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL))
			{
				int k = sizeof(k);
			}
			gfx->_multisamples = (size_t)pp.MultiSampleType;

			gfx->makeCurrent();

			gfx->__init();

			return gfx;
		}
		size_t					d3dGfx::getWindowDX() const
		{
			RECT rt;
			GetClientRect(_hwnd, &rt);
			return rt.right;
		}
		size_t					d3dGfx::getWindowDY() const
		{
			RECT rt;
			GetClientRect(_hwnd, &rt);
			return rt.bottom;
		}
		size_t					d3dGfx::getFramebufferDX() const
		{
			//return _pp.BackBufferWidth;
			return _framebuffer_dx;
		}
		size_t					d3dGfx::getFramebufferDY() const
		{
			//return _pp.BackBufferHeight;
			return _framebuffer_dy;
		}
		void					d3dGfx::clear(uint32_t x, uint32_t y, uint32_t dx, uint32_t dy, bool clear_color, bool clear_depth, bool clear_stencil, const Vec4& color, REAL depth, uint32_t stencil)
		{
			D3DRECT rt;
			rt.x1 = x;
			rt.y1 = y;
			rt.x2 = x + dx;
			rt.y2 = y + dy;
			_device->Clear(1, &rt, (clear_color ? D3DCLEAR_TARGET : 0) | (clear_depth ? D3DCLEAR_ZBUFFER : 0) | (clear_stencil ? D3DCLEAR_STENCIL : 0),toD3DCOLOR(color), depth, stencil);
		}
		void					d3dGfx::clear(bool clear_color, bool clear_depth, bool clear_stencil, const Vec4& color, REAL depth, uint32_t stencil)
		{
			_device->Clear(0, NULL, (clear_color ? D3DCLEAR_TARGET : 0) | (clear_depth ? D3DCLEAR_ZBUFFER : 0) | (clear_stencil ? D3DCLEAR_STENCIL : 0), toD3DCOLOR(color), depth, stencil);
		}
		void					d3dGfx::beginScene()
		{
			_device->BeginScene();
		}
		void					d3dGfx::endScene()
		{
			_device->EndScene();
		}
		void					d3dGfx::flush()
		{
			if(!_query_flush)
				return;

			_query_flush->Issue(D3DISSUE_END);

			HRESULT hr;

			while((hr = _query_flush->GetData(NULL, 0, D3DGETDATA_FLUSH)) != S_OK)
				if(hr == D3DERR_DEVICELOST)
					break;
		}
		void					d3dGfx::present()
		{
			Z_ASSERT( _rt_n_ds_stack.size == 0 );

			HRESULT ret = S_OK;
			if(_hwnd_present == _hwnd)
				ret = _device->Present(NULL, NULL, NULL, NULL);
			else
				ret = _device->Present(NULL, NULL, _hwnd_present, NULL);

			if(ret != S_OK)
			{
				Z_ASSERT( ret == D3DERR_DEVICELOST || ret == D3DERR_DEVICENOTRESET);
				if(ret == D3DERR_DEVICELOST || ret == D3DERR_DEVICENOTRESET)
					handleDeviceLost();
				else
				{
					syslog << L"d3d present error, program will be terminated";
					ExitProcess(0);
				}
			}
		}
		void					d3dGfx::present(intptr_t hwnd, uint32_t src_x, uint32_t src_y, uint32_t src_dx, uint32_t src_dy, uint32_t dst_x, uint32_t dst_y, uint32_t dst_dx, uint32_t dst_dy)
		{
			Z_ASSERT( _rt_n_ds_stack.size == 0 );

			HRESULT ret = S_OK;

			RECT src = {src_x, src_y, src_x + src_dx, src_y + src_dy};
			RECT dst = {dst_x, dst_y, dst_x + dst_dx, dst_y + dst_dy};

			ret = _device->Present(&src, &dst, (HWND)hwnd, NULL);

			if(ret != S_OK)
			{
				Z_ASSERT( ret == D3DERR_DEVICELOST || ret == D3DERR_DEVICENOTRESET);
				if(ret == D3DERR_DEVICELOST || ret == D3DERR_DEVICENOTRESET)
					handleDeviceLost();
				else
				{
					syslog << L"d3d present error, program will be terminated";
					ExitProcess(0);
				}
			}
		}
		bool					d3dGfx::copyToTexture(const SPtr<Tex2D>& tex, uint32_t src_x, uint32_t src_y, uint32_t src_dx, uint32_t src_dy, uint32_t dst_x, uint32_t dst_y, uint32_t dst_dx, uint32_t dst_dy)
		{
			Z_ASSERT( tex && tex->type_info()->exactly(d3dTex2D::type_info_static()) );
			RECT srt = {src_x, src_y, src_x + src_dx, src_y + src_dy};
			RECT drt = {dst_x, dst_y, dst_x + dst_dx, dst_y + dst_dy};
			IDirect3DSurface9* src = NULL;
			IDirect3DSurface9* dst = NULL;
			_device->GetRenderTarget(0, &src);
			tex.cast<d3dTex2D>()->d3d_tex()->GetSurfaceLevel(0, &dst);
			bool ret = false;
			if(src && dst)
				ret = (_device->StretchRect(src, &srt, dst, &drt, D3DTEXF_NONE) == S_OK);

			if(dst)	dst->Release();
			if(src)	src->Release();

			return ret;
		}
		void					d3dGfx::notifyDisplaySettingsChanged()
		{
			handleDeviceLost();
		}
		void					d3dGfx::handleDeviceLost()
		{
			HRESULT ret = _device->TestCooperativeLevel();
			if(!FAILED(ret))
				return;

			if(ret != D3DERR_DEVICELOST && ret != D3DERR_DEVICENOTRESET)
			{
				syslog << L"d3d driver internal error, program will be terminated";
				ExitProcess(0);
			}

			_device->SetRenderTarget(0, NULL);
			_device->SetDepthStencilSurface(NULL);

			if(true)
			{
				size_t c = maxSamplers();
				for(size_t i = 0; i < c; ++i)
					_device->SetTexture((DWORD)i, NULL);
			}

			for(int i = 0; i < (int)_caps.MaxStreams; ++i)
				_device->SetStreamSource(i, NULL, 0, 0);

			_device->SetIndices(NULL);

			if(true)
			{
				D3D_RECREATABLE_LIST_TYPE::iterator end_iter = _list_recreatable.end();
				for(D3D_RECREATABLE_LIST_TYPE::iterator iter = _list_recreatable.begin(); iter != end_iter; ++iter)
					(*iter)->prepare_recreate();
			}

			if(_query_flush)
			{
				_query_flush->Release();
				_query_flush = NULL;
			}
			if(_back_buffer_color)
			{
				_back_buffer_color->Release();
				_back_buffer_color = NULL;
			}
			if(_back_buffer_depthstencil)
			{
				_back_buffer_depthstencil->Release();
				_back_buffer_depthstencil = NULL;
			}
			clearD3DVertexDecls();

			while(true)
			{
				while(_device->TestCooperativeLevel() != D3DERR_DEVICENOTRESET)
					Sleep(100);

				HRESULT hr = _device->Reset(&_pp);
				if(FAILED(hr))
				{
					//int k=sizeof(k);
					//Z_ASSERT(false);
					//syslog<<L"cannot reset d3d device, program will be terminated";
					//ExitProcess(0);

					continue;
				}

				break;
			}

			__init();

			if(true)
			{
				D3D_RECREATABLE_LIST_TYPE::iterator end_iter = _list_recreatable.end();
				for(D3D_RECREATABLE_LIST_TYPE::iterator iter = _list_recreatable.begin(); iter != end_iter; ++iter)
					if(!(*iter)->recreate())
					{
						Z_ASSERT( false );
						syslog<<L"cannot recreate: " + (*iter)->short_desc() + L", program will be terminated";
						ExitProcess(0);
					}
			}
		}
		void					d3dGfx::removeVBORef(IDirect3DVertexBuffer9* ref)
		{
			UINT offset, stride;
			for(size_t i = 0; i < (size_t)_caps.MaxStreams; ++i)
			{
				IDirect3DVertexBuffer9* r = NULL;
				_device->GetStreamSource((UINT)i, &r, &offset, &stride);
				if(r == ref)
					_device->SetStreamSource((UINT)i, NULL, 0, 0);
				if(r)
					r->Release();
			}
		}
		void					d3dGfx::removeIBORef(IDirect3DIndexBuffer9* ref)
		{
			IDirect3DIndexBuffer9* r = NULL;
			_device->GetIndices(&r);
			if(r == ref)
				_device->SetIndices(NULL);
			if(r)
				r->Release();
		}
		void					d3dGfx::removeTexRef(IDirect3DBaseTexture9* ref)
		{
			size_t c = maxSamplers();
			for(size_t i = 0; i < c; ++i)
			{
				if(_rs_ts[i].get() == ref)
					_rs_ts[i].set(_device, (DWORD)i, NULL);
			}
		}
		void					d3dGfx::removeVSRef(IDirect3DVertexShader9* ref)
		{
			if(_rs_vss.get() == ref)
				_rs_vss.set(_device, NULL);
		}
		void					d3dGfx::removePSRef(IDirect3DPixelShader9* ref)
		{
			if(_rs_pss.get() == ref)
				_rs_pss.set(_device, NULL);
		}
		void					d3dGfx::enumMultiSampleTypeSupport(GfxEnum::PIXELFORMAT format, bool support[16])
		{
			D3DFORMAT fmt = map_D3DFORMAT(format);
			for(size_t i = 0; i < 16; ++i)
				support[i] = (_d3d->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, fmt, TRUE, (D3DMULTISAMPLE_TYPE)(D3DMULTISAMPLE_NONMASKABLE + i), NULL) == S_OK);
		}
		IDirect3DTexture9*		d3dGfx::_createTex2D(const Tex2DCreateParams& cp)
		{
			DWORD usage =	(cp.auto_gen_mipmap ? D3DUSAGE_AUTOGENMIPMAP : 0) |
							(cp.depth_stencil ? D3DUSAGE_DEPTHSTENCIL : 0) |
							(cp.dynamic ? D3DUSAGE_DYNAMIC : 0) |
							(cp.render_target ? D3DUSAGE_RENDERTARGET : 0);

			IDirect3DTexture9* tex = NULL;
			_device->CreateTexture((UINT)cp.dx, (UINT)cp.dy, (UINT)cp.level_count, usage, d3dGfx::map_D3DFORMAT(format), d3dGfx::map_D3DPOOL(pool), &tex, NULL);

			return tex;
		}
		SPtr<VBO>				d3dGfx::createVBO(size_t length, bool dynamic)
		{
			return d3dVBO::create(this, length, dynamic);
		}
		SPtr<IBO>				d3dGfx::createIBO(size_t length, bool _32bit, bool dynamic)
		{
			return d3dIBO::create(this, length, _32bit, dynamic);
		}
		SPtr<Tex2D>				d3dGfx::createTex2D(size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		{
			return d3dTex2D::create(this, dx, dy, levels, format, rendertarget);
		}
		SPtr<Tex2D>				d3dGfx::createTex2D(const Tex2DCreateParams& cp)
		{
			IDirect3DTexture9* tex = _createTex2D(cp);
			if(!tex)
				return NULL;

			return new d3dTex2D(this, tex, _device, cp);
		}
		SPtr<TexCube>			d3dGfx::createTexCube(size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget)
		{
			return d3dTexCube::create(this, length, levels, format, rendertarget);
		}
		SPtr<Shader>			d3dGfx::createShaderFromSource(GfxEnum::SHADERTYPE type, const mbs& source)
		{
			return d3dShader::loadFromSource(this, type, source);
		}
		SPtr<Shader>			d3dGfx::createShaderFromFile(GfxEnum::SHADERTYPE type, const wcs& filepath)
		{
			return d3dShader::loadFromFile(this, type, filepath);
		}
		SPtr<Renderbuffer>		d3dGfx::createRenderTargetRenderbuffer(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			return d3dRenderbuffer::createRenderTarget(this, dx, dy, format, multisample_type);
		}
		SPtr<Renderbuffer>		d3dGfx::createDepthStencilRenderbuffer(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type)
		{
			return d3dRenderbuffer::createDepthStencil(this, dx, dy, format, multisample_type);
		}
		ShaderGen*				d3dGfx::getShaderGen()
		{
			return &_shader_gen;
		}
		void					d3dGfx::getTexFontShaders(bool use_decal, SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			if(use_decal)
			{
				if(_vs_texfont_use_decal && _ps_texfont_use_decal)
				{
					vs = _vs_texfont_use_decal;
					ps = _ps_texfont_use_decal;
					return;
				}
				mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	float4 in_color: COLOR,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	float4 in_tc_1: TEXCOORD1,\n\
	out float4 out_pos: POSITION,\n\
	out float4 out_color: COLOR,\n\
	out float4 out_tc_0: TEXCOORD0,\n\
	out float4 out_tc_1: TEXCOORD1,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_color = in_color;\n\
	out_tc_0 = in_tc_0;\n\
	out_tc_1 = in_tc_1;\n\
}\n\
\
";
				mbs ps_source = 
"\
\n\
sampler2D sampler_0: register(s0);\n\
sampler2D sampler_1: register(s1);\n\
\n\
void main(\n\
	float4 in_color: COLOR,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	float4 in_tc_1: TEXCOORD1,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = in_color;\n\
	out_color.a *= tex2D(sampler_0, in_tc_0).a;\n\
	out_color *= tex2D(sampler_1, in_tc_1);\n\
}\
";
				_vs_texfont_use_decal = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
				_ps_texfont_use_decal = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
				Z_ASSERT( _vs_texfont_use_decal && _ps_texfont_use_decal);
				vs = _vs_texfont_use_decal;
				ps = _ps_texfont_use_decal;
			}
			else
			{
				if(_vs_texfont && _ps_texfont)
				{
					vs = _vs_texfont;
					ps = _ps_texfont;
					return;
				}
				mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	float4 in_color: COLOR,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	out float4 out_pos: POSITION,\n\
	out float4 out_color: COLOR,\n\
	out float4 out_tc_0: TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_color = in_color;\n\
	out_tc_0 = in_tc_0;\n\
}\n\
\
";
				mbs ps_source = 
"\
\n\
sampler2D sampler_0: register(s0);\n\
\n\
void main(\n\
	float4 in_color: COLOR,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = in_color;\n\
	out_color.a *= tex2D(sampler_0, in_tc_0).a;\n\
}\
";
				_vs_texfont = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
				_ps_texfont = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
				Z_ASSERT( _vs_texfont && _ps_texfont);
				vs = _vs_texfont;
				ps = _ps_texfont;
			}
		}
		void					d3dGfx::getImg2DShaders(SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			if(_vs_img2d && _ps_img2d)
			{
				vs = _vs_img2d;
				ps = _ps_img2d;
				return;
			}
			mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	float4 in_color: COLOR,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	out float4 out_pos: POSITION,\n\
	out float4 out_color: COLOR,\n\
	out float4 out_tc_0: TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_color = in_color;\n\
	out_tc_0 = in_tc_0;\n\
}\n\
\
";
			mbs ps_source = 
"\
\n\
sampler2D sampler_0: register(s0);\n\
\n\
void main(\n\
	float4 in_color: COLOR,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = in_color * tex2D(sampler_0, in_tc_0);\n\
}\
";
			_vs_img2d = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
			_ps_img2d = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
			Z_ASSERT( _vs_img2d && _ps_img2d);
			vs = _vs_img2d;
			ps = _ps_img2d;
		}
		void					d3dGfx::getPassThruShaders(SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			if(_vs_pass_thru && _ps_pass_thru)
			{
				vs = _vs_pass_thru;
				ps = _ps_pass_thru;
				return;
			}
			mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	float4 in_color: COLOR,\n\
	out float4 out_pos: POSITION,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_color = in_color;\n\
}\n\
\
";
			mbs ps_source = 
"\
\n\
void main(\n\
	float4 in_color: COLOR,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = in_color;\n\
}\
";
			_vs_pass_thru = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
			_ps_pass_thru = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
			Z_ASSERT( _vs_pass_thru && _ps_pass_thru);
			vs = _vs_pass_thru;
			ps = _ps_pass_thru;
		}
		void					d3dGfx::getVSMScreenDepth(bool alpha, SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			if(!alpha)
			{
				if(_vs_vsm_screen_depth && _ps_vsm_screen_depth)
				{
					vs = _vs_vsm_screen_depth;
					ps = _ps_vsm_screen_depth;
					return;
				}
				mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	out float4 out_pos: POSITION,\n\
	out float4 out_pos2: TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_pos2 = out_pos;\n\
}\n\
\
";
				mbs ps_source = 
"\
\n\
void main(\n\
	float4 in_pos: TEXCOORD0,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = in_pos.z / in_pos.w;\n\
}\
";
				_vs_vsm_screen_depth = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
				_ps_vsm_screen_depth = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
				Z_ASSERT( _vs_vsm_screen_depth && _ps_vsm_screen_depth);
				vs = _vs_vsm_screen_depth;
				ps = _ps_vsm_screen_depth;
			}
			else
			{
				if(_vs_vsm_screen_depth_alpha && _ps_vsm_screen_depth_alpha)
				{
					vs = _vs_vsm_screen_depth_alpha;
					ps = _ps_vsm_screen_depth_alpha;
					return;
				}
				mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
float4x4 xform_tc;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	float4 in_tc: TEXCOORD0,\n\
	out float4 out_pos: POSITION,\n\
	out float4 out_pos2: TEXCOORD0,\n\
	out float4 out_tc: TEXCOORD1,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_pos2 = out_pos;\n\
	out_tc = mul(in_tc, xform_tc);\n\
}\n\
\
";
				mbs ps_source = 
"\
float alpha_ref;\n\
\n\
sampler2D sampler_diffuse: register(s0);\n\
\n\
void main(\n\
	float4 in_pos: TEXCOORD0,\n\
	float4 in_tc: TEXCOORD1,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	//if(tex2D(sampler_diffuse, in_tc).w <= alpha_ref)\n\
	//	clip(-1);\n\
	clip((tex2D(sampler_diffuse, in_tc).w > alpha_ref) - 0.5);\n\
	out_color = in_pos.z / in_pos.w;\n\
}\
";
				_vs_vsm_screen_depth_alpha = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
				_ps_vsm_screen_depth_alpha = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
				Z_ASSERT( _vs_vsm_screen_depth_alpha && _ps_vsm_screen_depth_alpha);
				vs = _vs_vsm_screen_depth_alpha;
				ps = _ps_vsm_screen_depth_alpha;
			}
		}
		void					d3dGfx::getVSMGenShadowMap(bool alpha, SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			if(!alpha)
			{
				if(_vs_vsm_gen_shadowmap && _ps_vsm_gen_shadowmap)
				{
					vs = _vs_vsm_gen_shadowmap;
					ps = _ps_vsm_gen_shadowmap;
					return;
				}
				mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
float4 depth_space;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	out float4 out_pos: POSITION,\n\
	out float out_depth: TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_depth = dot(in_pos, depth_space);\n\
}\n\
\
";
				mbs ps_source = 
"\
\n\
void main(\n\
	float in_depth: TEXCOORD0,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	float depth = in_depth;\n\
	//float depth = exp(32 * (in_depth - 1));\n\
	//float depth = log(in_depth) / 64;\n\
	out_color = float4(depth, depth * depth, 0, 0);\n\
}\
";
				_vs_vsm_gen_shadowmap = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
				_ps_vsm_gen_shadowmap = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
				Z_ASSERT( _vs_vsm_gen_shadowmap && _ps_vsm_gen_shadowmap);
				vs = _vs_vsm_gen_shadowmap;
				ps = _ps_vsm_gen_shadowmap;
			}
			else
			{
				if(_vs_vsm_gen_shadowmap_alpha && _ps_vsm_gen_shadowmap_alpha)
				{
					vs = _vs_vsm_gen_shadowmap_alpha;
					ps = _ps_vsm_gen_shadowmap_alpha;
					return;
				}
				mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
float4 depth_space;\n\
float4x4 xform_tc;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	float4 in_tc: TEXCOORD0,\n\
	out float4 out_pos: POSITION,\n\
	out float out_depth: TEXCOORD0,\n\
	out float4 out_tc: TEXCOORD1,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_depth = dot(in_pos, depth_space);\n\
	out_tc = mul(in_tc, xform_tc);\n\
}\n\
\
";
				mbs ps_source = 
"\
float alpha_ref;\n\
\n\
sampler2D sampler_diffuse: register(s0);\n\
\n\
void main(\n\
	float in_depth: TEXCOORD0,\n\
	float4 in_tc: TEXCOORD1,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	//if(tex2D(sampler_diffuse, in_tc).w <= alpha_ref)\n\
	//	clip(-1);\n\
	clip((tex2D(sampler_diffuse, in_tc).w > alpha_ref) - 0.5);\n\
	float depth = in_depth;\n\
	//float depth = exp(32 * (in_depth - 1));\n\
	//float depth = log(in_depth) / 64;\n\
	out_color = float4(depth, depth * depth, 0, 0);\n\
}\
";
				_vs_vsm_gen_shadowmap_alpha = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
				_ps_vsm_gen_shadowmap_alpha = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
				Z_ASSERT( _vs_vsm_gen_shadowmap_alpha && _ps_vsm_gen_shadowmap_alpha);
				vs = _vs_vsm_gen_shadowmap_alpha;
				ps = _ps_vsm_gen_shadowmap_alpha;
			}
		}
		void					d3dGfx::getVSMMarkShadow(size_t light_count, SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			Z_ASSERT( light_count <= 4 );
			if(_vs_vsm_mark_shadow2[light_count - 1] && _ps_vsm_mark_shadow2[light_count - 1])
			{
				vs = _vs_vsm_mark_shadow2[light_count - 1];
				ps = _ps_vsm_mark_shadow2[light_count - 1];
				return;
			}
			mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	out float4 out_pos: POSITION,\n\
	float2 in_pos2: TEXCOORD0,\n\
	out float2 out_pos2: TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_pos2 = in_pos2;\n\
}\n\
\
";
			mbs ps_source = 
"\
\n\
float2 sample_moments(sampler2D s, float2 uv)\n\
{\n\
	return tex2D(s, uv);\n\
}\n\
\n\
float2 sample_moments_emu_bilinear(sampler2D s, float2 uv, float2 tex_dim, float2 inv_tex_dim)\n\
{\n\
	float2 lerp_amt = frac(uv * tex_dim);\n\
	float2 uv2 = uv - lerp_amt * inv_tex_dim;\n\
	float2 off = inv_tex_dim;\n\
	return lerp(lerp(tex2D(s, uv2), tex2D(s, uv2 + float2(off.x, 0)), lerp_amt.x), lerp(tex2D(s, uv2 + float2(0, off.y)), tex2D(s, uv2 + float2(off.x, off.y)), lerp_amt.x), lerp_amt.y).xy;\n\
}\n\
\n\
float vsm_core(float depth, float2 moments)\n\
{\n\
	float p_max;\n\
	if( 1 )\n\
	{\n\
		float mean = moments.x;\n\
		float E_x = moments.x;\n\
		float E_x2 = moments.y;\n\
		float variance = E_x2 - E_x * E_x;\n\
		variance = saturate(variance + 0.002);					// for fp16, 0.002, for fp32, 0.0002\n\
		p_max = variance / (variance + saturate(depth - mean) * saturate(depth - mean));\n\
		p_max = smoothstep(0.95, 1.0, p_max);\n\
		//p_max = 1.0 - 1.5 * saturate(depth - mean) / sqrt(variance);\n\
	}\n\
\n\
	float p_max_tricked;\n\
	if( 1 )\n\
	{\n\
		float mean = moments.x;\n\
		float E_x = moments.x;\n\
		float E_x2 = moments.y;\n\
\n\
		const float MAX_DIFF = 1.0 / 16777216;\n\
		E_x = clamp(E_x, sqrt(saturate(depth * depth - MAX_DIFF)), sqrt(saturate(depth * depth + MAX_DIFF)));\n\
		E_x2 = clamp(E_x2, saturate(depth * depth - MAX_DIFF), saturate(depth * depth + MAX_DIFF));\n\
\n\
		float variance = E_x2 - E_x * E_x;\n\
		float denominator = variance + saturate(depth - mean) * saturate(depth - mean);\n\
		p_max_tricked = (denominator > 0.0001) ? (variance / denominator) : 1;\n\
	}\n\
	return p_max;\n\
}\n\
\n\
float vsm(float4 world_pos, float4x4 sm_uv_xform, float4 light_space_depth_xform, sampler2D s)\n\
{\n\
	float4 uv = mul(world_pos, sm_uv_xform);\n\
	uv /= uv.w;\n\
	float depth = dot(world_pos, light_space_depth_xform);\n\
	float2 moments = sample_moments(s, uv);\n\
	return vsm_core(depth, moments);\n\
}\n\
\n\
float vsm_emu_bilinear(float4 world_pos, float4x4 sm_uv_xform, float4 light_space_depth_xform, sampler2D s, float2 tex_dim, float2 inv_tex_dim)\n\
{\n\
	float4 uv = mul(world_pos, sm_uv_xform);\n\
	uv /= uv.w;\n\
	float depth = dot(world_pos, light_space_depth_xform);\n\
	float2 moments = sample_moments_emu_bilinear(s, uv, tex_dim, inv_tex_dim);\n\
	return vsm_core(depth, moments);\n\
}\n\
\n\
\n\
float4x4 world_xform;\n\
{UNIFORMS}\n\
\n\
sampler2D sampler_screen_depth: register(s0);\n\
{SAMPLERS}\n\
\n\
void main(\n\
	float2 in_pos: TEXCOORD0,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
float4 world_pos = mul(float4(in_pos, tex2D(sampler_screen_depth, (in_pos * 0.5 + 0.5) * float2(1, -1) + float2(0, 1)).x, 1), world_xform);\n\
	world_pos /= world_pos.w;\n\
	out_color = float4(0, 0, 0, 0);\n\
	{CODE}\n\
}\
";
			if(true)
			{
				mbs s;
				for(size_t i = 0; i < light_count; ++i)
				{
					s += FORMATA("\
float4x4 sm_uv_xform_%Id;\n\
float4 light_space_depth_xform_%Id;\n\
", i, i);
				}
				ps_source = REPLACE(ps_source, "{UNIFORMS}", s);
			}
			if(true)
			{
				mbs s;
				for(size_t i = 0; i < light_count; ++i)
				{
					s += FORMATA("\
sampler2D sampler_shadow_map_%Id: register(s%Id);\n\
", i, i + 1);
				}
				ps_source = REPLACE(ps_source, "{SAMPLERS}", s);
			}
			if(true)
			{
				mbs s;
				for(size_t i = 0; i < light_count; ++i)
				{
					s += FORMATA("\
	out_color[%Id] = vsm(world_pos, sm_uv_xform_%Id, light_space_depth_xform_%Id, sampler_shadow_map_%Id);\n\
", i, i, i, i);
				}
				ps_source = REPLACE(ps_source, "{CODE}", s);
			}
			_vs_vsm_mark_shadow2[light_count - 1] = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
			_ps_vsm_mark_shadow2[light_count - 1] = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
			Z_ASSERT( _vs_vsm_mark_shadow2[light_count - 1] && _ps_vsm_mark_shadow2[light_count - 1]);
			vs = _vs_vsm_mark_shadow2[light_count - 1];
			ps = _ps_vsm_mark_shadow2[light_count - 1];
		}
		void					d3dGfx::getGaussianBlur5(SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			if(_vs_gaussian_blur_5 && _ps_gaussian_blur_5)
			{
				vs = _vs_gaussian_blur_5;
				ps = _ps_gaussian_blur_5;
				return;
			}
			mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	out float4 out_pos: POSITION,\n\
	float4 in_tc0: TEXCOORD0,\n\
	out float4 out_tc0: TEXCOORD0,\n\
	float4 in_tc1: TEXCOORD1,\n\
	out float4 out_tc1: TEXCOORD1,\n\
	float4 in_tc2: TEXCOORD2,\n\
	out float4 out_tc2: TEXCOORD2,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_tc0 = in_tc0;\n\
	out_tc1 = in_tc1;\n\
	out_tc2 = in_tc2;\n\
}\n\
\
";
			mbs ps_source = 
"\
\n\
sampler2D sampler_0: register(s0);\n\
\n\
void main(\n\
	float4 in_tc0: TEXCOORD0,\n\
	float4 in_tc1: TEXCOORD1,\n\
	float4 in_tc2: TEXCOORD2,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color =\n\
			tex2D(sampler_0, in_tc0) * 0.402619946 +\n\
			(tex2D(sampler_0, in_tc1) + tex2D(sampler_0, 2 * in_tc0 - in_tc1)) * 0.244201342 +\n\
			(tex2D(sampler_0, in_tc2) + tex2D(sampler_0, 2 * in_tc0 - in_tc2)) * 0.054488685;\n\
}\
";
			_vs_gaussian_blur_5 = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
			_ps_gaussian_blur_5 = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
			Z_ASSERT( _vs_gaussian_blur_5 && _ps_gaussian_blur_5);
			vs = _vs_gaussian_blur_5;
			ps = _ps_gaussian_blur_5;
		}
		void					d3dGfx::getGaussianBlur3(SPtr<Shader>& vs, SPtr<Shader>& ps)
		{
			if(_vs_gaussian_blur_3 && _ps_gaussian_blur_3)
			{
				vs = _vs_gaussian_blur_3;
				ps = _ps_gaussian_blur_3;
				return;
			}
			mbs vs_source = 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	out float4 out_pos: POSITION,\n\
	float4 in_tc0: TEXCOORD0,\n\
	out float4 out_tc0: TEXCOORD0,\n\
	float4 in_tc1: TEXCOORD1,\n\
	out float4 out_tc1: TEXCOORD1,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_tc0 = in_tc0;\n\
	out_tc1 = in_tc1;\n\
}\n\
\
";
			mbs ps_source = 
"\
\n\
sampler2D sampler_0: register(s0);\n\
\n\
void main(\n\
	float4 in_tc0: TEXCOORD0,\n\
	float4 in_tc1: TEXCOORD1,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color =\n\
			tex2D(sampler_0, in_tc0) * 0.606315991 +\n\
			(tex2D(sampler_0, in_tc1) + tex2D(sampler_0, 2 * in_tc0 - in_tc1)) * 0.196842005;\n\
}\
";
			_vs_gaussian_blur_3 = createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs_source);
			_ps_gaussian_blur_3 = createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps_source);
			Z_ASSERT( _vs_gaussian_blur_3 && _ps_gaussian_blur_3);
			vs = _vs_gaussian_blur_3;
			ps = _ps_gaussian_blur_3;
		}
		ResMgr&					d3dGfx::getResMgrVS()
		{
			return _res_mgr_vs;
		}
		ResMgr&					d3dGfx::getResMgrPS()
		{
			return _res_mgr_ps;
		}
		ResMgr&					d3dGfx::getResMgrTex2D()
		{
			return _res_mgr_tex2d;
		}
		ResMgr&					d3dGfx::getResMgrTexCube()
		{
			return _res_mgr_texcube;
		}
		ResMgr&					d3dGfx::getResMgrTex2DAVICF()
		{
			return _res_mgr_tex2d_avi_cf;
		}
		ResMgr&					d3dGfx::getResMgrTex2DGIFCF()
		{
			return _res_mgr_tex2d_gif_cf;
		}
		bool					d3dGfx::bindRenderTarget(size_t index, const SPtr<Tex2D>& tex2d)
		{
			Z_ASSERT( tex2d );

			IDirect3DSurface9* s = NULL;
			tex2d.cast<d3dTex2D>()->d3d_tex()->GetSurfaceLevel(0, &s);
			if(!s)
				return false;
			bool ret = bindRenderTarget(index, s);
			s->Release();

			return ret;
		}
		bool					d3dGfx::bindRenderTarget(size_t index, const SPtr<TexCube>& texcube, GfxEnum::CUBEMAP_FACE face)
		{
			Z_ASSERT( texcube );

			IDirect3DSurface9* s = NULL;
			texcube.cast<d3dTexCube>()->d3d_tex()->GetCubeMapSurface(map_D3DCUBEMAP_FACES(face), 0, &s);
			if(!s)
				return false;
			bool ret = bindRenderTarget(index, s);
			s->Release();

			return ret;
		}
		bool					d3dGfx::bindRenderTarget(size_t index, const SPtr<Renderbuffer>& renderbuffer)
		{
			Z_ASSERT( renderbuffer );

			return bindRenderTarget(index, renderbuffer.cast<d3dRenderbuffer>()->d3d_surface());
		}
		bool					d3dGfx::bindDefaultRenderTarget()
		{
			return bindRenderTarget(0, (IDirect3DSurface9*)NULL);
		}
		bool					d3dGfx::unbindRenderTarget(size_t index)
		{
			return bindRenderTarget(index, (IDirect3DSurface9*)NULL);
		}
		void					d3dGfx::unbindAllRenderTargets()
		{
			for(ptrdiff_t i = (ptrdiff_t)_caps.NumSimultaneousRTs - 1; i >= 0; --i)
				unbindRenderTarget(i);
		}
		bool					d3dGfx::bindDepthStencil(const SPtr<Tex2D>& tex2d)
		{
			Z_ASSERT( tex2d );

			IDirect3DSurface9* s = NULL;
			tex2d.cast<d3dTex2D>()->d3d_tex()->GetSurfaceLevel(0, &s);
			if(!s)
				return false;
			bool ret = bindDepthStencil(s);
			s->Release();

			return ret;
		}
		bool					d3dGfx::bindDepthStencil(const SPtr<Renderbuffer>& renderbuffer)
		{
			Z_ASSERT( renderbuffer );

			return bindDepthStencil(renderbuffer.cast<d3dRenderbuffer>()->d3d_surface());
		}
		bool					d3dGfx::bindDefaultDepthStencil()
		{
			return bindDepthStencil(_back_buffer_depthstencil);
		}
		bool					d3dGfx::unbindDepthStencil()
		{
			return bindDepthStencil(NULL);
		}
		bool					d3dGfx::boundRenderTarget() const
		{
			return !!_bound_rendertarget;
		}
		bool					d3dGfx::boundDepthStencil() const
		{
			return !!_bound_depthstencil;
		}
		bool					d3dGfx::bindRenderTarget(size_t index, IDirect3DSurface9* surface)
		{
			bool ret = (_device->SetRenderTarget((DWORD)index, surface ? surface : (index == 0 ? _back_buffer_color : NULL)) == D3D_OK);

			//_bound_rendertarget = (surface != NULL && surface != _back_buffer_color);

			postBindRenderTarget(index, surface ? surface : _back_buffer_color);

			return ret;
		}
		bool					d3dGfx::bindDepthStencil(IDirect3DSurface9* surface)
		{
			bool ret = (_device->SetDepthStencilSurface(surface) == D3D_OK);

			//_bound_depthstencil = (surface != NULL && surface != _back_buffer_depthstencil);

			return ret;
		}
		void					d3dGfx::postBindRenderTarget(size_t index, IDirect3DSurface9* surface)
		{
			if(index == 0)
			{
				Z_ASSERT( surface );
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);
				_framebuffer_dx = (size_t)desc.Width;
				_framebuffer_dy = (size_t)desc.Height;
			}

			_rs_vps.init(_device);

			_cur_render_setup = GfxEnum::RENDER_SETUP_1_RENDERTARGET;

			rsView rs;
			rs.init(this);

			setViewState(rs);
		}
		void					d3dGfx::push_RT_n_DS()
		{
			Z_ASSERT( _rt_n_ds_stack.size < _countof(_rt_n_ds_stack.rts) );

			IDirect3DSurface9* rts[4] = {0, 0, 0, 0};
			IDirect3DSurface9* ds = NULL;

			_device->GetRenderTarget(0, rts + 0);
			_device->GetRenderTarget(1, rts + 1);
			_device->GetRenderTarget(2, rts + 2);
			_device->GetRenderTarget(3, rts + 3);
			_device->GetDepthStencilSurface(&ds);

			memcpy(_rt_n_ds_stack.rts[_rt_n_ds_stack.size], rts, sizeof(rts));
			_rt_n_ds_stack.dss[_rt_n_ds_stack.size] = ds;
			++_rt_n_ds_stack.size;
		}
		void					d3dGfx::pop_RT_n_DS()
		{
			Z_STATIC_ASSERT(_countof(_rt_n_ds_stack.rts[_rt_n_ds_stack.size]) == 4, WRONG_ASSUMPTION);
			Z_ASSERT( _rt_n_ds_stack.size > 0 );

			--_rt_n_ds_stack.size;
			IDirect3DSurface9* (&rts)[4] = _rt_n_ds_stack.rts[_rt_n_ds_stack.size];
			IDirect3DSurface9* ds = _rt_n_ds_stack.dss[_rt_n_ds_stack.size];

			bindRenderTarget(0, rts[0]);
			bindRenderTarget(1, rts[1]);
			bindRenderTarget(2, rts[2]);
			bindRenderTarget(3, rts[3]);
			bindDepthStencil(ds);

			for(size_t i = 0; i < _countof(rts); ++i)
				if(rts[i]) rts[i]->Release();
			if(ds) ds->Release();
		}

		D3DCOLOR				d3dGfx::toD3DCOLOR(const Vec4& v)
		{
			return	(((DWORD)(v[2] * 255)) & 0xff) |
					((((DWORD)(v[1] * 255)) & 0xff) << 8) |
					((((DWORD)(v[0] * 255)) & 0xff) << 16) |
					((((DWORD)(v[3] * 255)) & 0xff) << 24);
		}
		D3DCOLORVALUE			d3dGfx::toD3DCOLORVALUE(const Vec4& v)
		{
			D3DCOLORVALUE v2;
			memcpy(&v2, &v, sizeof(D3DCOLORVALUE));
			return v2;
		}
		bool					d3dGfx::supportsNonPower2Tex() const
		{
			return (!(_caps.TextureCaps & D3DPTEXTURECAPS_POW2) && !(_caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL));
		}
		bool					d3dGfx::supportsShaderModel2() const
		{
			return _supports_sm_2;
		}
		bool					d3dGfx::supportsShaderModel3() const
		{
			//return false;
			return _supports_sm_3;
		}
		const char*				d3dGfx::getVSProfile() const
		{
			//return "vs_2_0";
			return _vs_profile;
		}
		const char*				d3dGfx::getPSProfile() const
		{
			//return "ps_2_0";
			return _ps_profile;
		}
		size_t					d3dGfx::maxTextures() const
		{
			return caps().MaxSimultaneousTextures;
		}
		size_t					d3dGfx::maxTextureStages() const
		{
			return caps().MaxTextureBlendStages;
		}
		size_t					d3dGfx::maxSamplers() const
		{
			return max((size_t)caps().MaxTextureBlendStages, (caps().VertexShaderVersion >= D3DVS_VERSION(3, 0)) ? (size_t)16 : (size_t)0);
		}
		size_t					d3dGfx::maxLights() const
		{
			return caps().MaxActiveLights;
		}
		size_t					d3dGfx::maxUserClipPlanes() const
		{
			return caps().MaxUserClipPlanes;
		}
		size_t					d3dGfx::maxTextureSize() const
		{
			return min(caps().MaxTextureWidth, caps().MaxTextureHeight);
		}
		REAL					d3dGfx::getZNearMapping() const {return 0.0f;}
		REAL					d3dGfx::getZFarMapping() const {return 1.0f;}
		REAL					d3dGfx::getWMultiplier() const {return -1.0f;}

		void					d3dGfx::__init()
		{
			_back_buffer_color = NULL;
			_back_buffer_depthstencil = NULL;
			_query_flush = NULL;
			_device->GetRenderTarget(0, &_back_buffer_color);
			_device->GetDepthStencilSurface(&_back_buffer_depthstencil);
			_device->CreateQuery(D3DQUERYTYPE_EVENT, &_query_flush);

			_device->GetDeviceCaps(&_caps);

			_rs_z_enable.init(_device);
			_rs_fill_mode.init(_device);
			_rs_shade_mode.init(_device);
			_rs_z_write_enable.init(_device);
			_rs_alpha_test_enable.init(_device);
			_rs_last_pixel.init(_device);
			_rs_src_blend.init(_device);
			_rs_dest_blend.init(_device);
			_rs_cull_mode.init(_device);
			_rs_z_func.init(_device);
			_rs_alpha_ref.init(_device);
			_rs_alpha_func.init(_device);
			_rs_dither_enable.init(_device);
			_rs_alpha_blend_enable.init(_device);
			_rs_fog_enable.init(_device);
			_rs_specular_enable.init(_device);
			_rs_fog_color.init(_device);
			_rs_fog_table_mode.init(_device);
			_rs_fog_start.init(_device);
			_rs_fog_end.init(_device);
			_rs_fog_density.init(_device);
			_rs_range_for_enable.init(_device);
			_rs_stencil_enable.init(_device);
			_rs_stencil_fail.init(_device);
			_rs_stencil_zfail.init(_device);
			_rs_stencil_pass.init(_device);
			_rs_stencil_func.init(_device);
			_rs_stencil_ref.init(_device);
			_rs_stencil_mask.init(_device);
			_rs_stencil_write_mask.init(_device);
			_rs_texture_factor.init(_device);
			_rs_wrap0.init(_device);
			_rs_wrap1.init(_device);
			_rs_wrap2.init(_device);
			_rs_wrap3.init(_device);
			_rs_wrap4.init(_device);
			_rs_wrap5.init(_device);
			_rs_wrap6.init(_device);
			_rs_wrap7.init(_device);
			_rs_clipping.init(_device);
			_rs_lighting.init(_device);
			_rs_ambient.init(_device);
			_rs_fog_vertex_mode.init(_device);
			_rs_color_vertex.init(_device);
			_rs_local_viewer.init(_device);
			_rs_normalize_normals.init(_device);
			_rs_diffuse_material_source.init(_device);
			_rs_specular_material_source.init(_device);
			_rs_ambient_material_source.init(_device);
			_rs_emissive_material_source.init(_device);
			_rs_vertex_blend.init(_device);
			_rs_clip_plane_enable.init(_device);
			_rs_point_size.init(_device);
			_rs_point_size_min.init(_device);
			_rs_point_sprite_enable.init(_device);
			_rs_point_scale_enable.init(_device);
			_rs_point_scale_a.init(_device);
			_rs_point_scale_b.init(_device);
			_rs_point_scale_c.init(_device);
			_rs_multisample_antialias.init(_device);
			_rs_multisample_mask.init(_device);
			_rs_patch_monitor_token.init(_device);
			_rs_debug_monitor_token.init(_device);
			_rs_point_size_max.init(_device);
			_rs_indexed_vertex_blend_enable.init(_device);
			_rs_color_write_enable.init(_device);
			_rs_tween_factor.init(_device);
			_rs_blend_op.init(_device);
			_rs_position_degree.init(_device);
			_rs_normal_degree.init(_device);
			_rs_scissor_test_enable.init(_device);
			_rs_slope_scale_depth_bias.init(_device);
			_rs_antialiased_line_enable.init(_device);
			_rs_min_tessellation_level.init(_device);
			_rs_max_tessellation_level.init(_device);
			_rs_adaptive_tess_x.init(_device);
			_rs_adaptive_tess_y.init(_device);
			_rs_adaptive_tess_z.init(_device);
			_rs_adaptive_tess_w.init(_device);
			_rs_enable_adaptive_tessellation.init(_device);
			_rs_two_side_stencil_mode.init(_device);
			_rs_ccw_stencil_fail.init(_device);
			_rs_ccw_stencil_zfail.init(_device);
			_rs_ccw_stencil_pass.init(_device);
			_rs_ccw_stencil_func.init(_device);
			_rs_color_write_enable1.init(_device);
			_rs_color_write_enable2.init(_device);
			_rs_color_write_enable3.init(_device);
			_rs_blend_factor.init(_device);
			_rs_srgb_write_enable.init(_device);
			_rs_depth_bias.init(_device);
			_rs_wrap8.init(_device);
			_rs_wrap9.init(_device);
			_rs_wrap10.init(_device);
			_rs_wrap11.init(_device);
			_rs_wrap12.init(_device);
			_rs_wrap13.init(_device);
			_rs_wrap14.init(_device);
			_rs_wrap15.init(_device);
			_rs_separate_alpha_blend_enable.init(_device);
			_rs_src_blend_alpha.init(_device);
			_rs_dest_blend_alpha.init(_device);
			_rs_blend_op_alpha.init(_device);

			// initialize texture stage state and cache supported states
			if(true)
			{
				size_t c = min(maxTextureStages(), (size_t)MAX_STAGES);
				for(size_t i = 0; i < c; ++i)
				{
					_rs_ts[i].init(_device, (DWORD)i);
					_rs_tss[i].init(_device, (DWORD)i);
					_rs_tct[i].init(_device, (DWORD)i);
				}
				c = min(maxSamplers(), (size_t)MAX_SAMPLERS);
				for(size_t i = 0; i < c; ++i)
				{
					_rs_ts[i].init(_device, (DWORD)i);
					_rs_ss[i].init(_device, (DWORD)i);
				}
			}
			if(true)
			{
				size_t c = min(maxLights(), (size_t)MAX_LIGHTS);
				for(size_t i = 0; i < c; ++i)
				{
					_rs_ls[i].init(_device, (DWORD)i);
					_rs_les[i].init(_device, (DWORD)i);
				}
			}
			if(true)
			{
				size_t c = min(maxUserClipPlanes(), (size_t)MAX_CLIP_PLANES);
				for(size_t i = 0; i < c; ++i)
					_rs_cps[i].init(_device, (DWORD)i);
			}

			_rs_scissor.init(_device);
			_rs_vps.init(_device);
			_rs_ms.init(_device);
			_rs_vds.init(_device);
			_rs_vss.init(_device);
			_rs_pss.init(_device);

			_ct_vs = NULL;
			_ct_ps = NULL;

			_vbo_bindings = 0;
			memset(_array_vbo_binding, 0, sizeof(_array_vbo_binding));
			_vertex_attrib_bindings = 0;
			memset(_array_vertex_attrib_binding, 0, sizeof(_array_vertex_attrib_binding));

			_last_used_vbos = 0;

			_jotter.used_length = 0;

			_dibo16.used_length = 0;

			_dibo32.used_length = 0;

			postBindRenderTarget(0, _back_buffer_color);

			if(true)
			{
				rsView view;
				view.init(this);
				view.setDepthRange(0, 1);

				setViewState(view);
			}

			_world_transform = Mat4::identity;
			_view_transform = Mat4::identity;
			_proj_transform = Mat4::identity;

			if(true)
			{
				strcpy(_vs_profile, "");
				strcpy(_ps_profile, "");

				const D3DCAPS9& caps = this->caps();
				uint32_t ver = min(D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion), D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion));
				if(ver == 3)
				{
					strcpy(_vs_profile, "vs_3_0");
					strcpy(_ps_profile, "ps_3_0");
				}
				else if(ver == 2)
				{
					if(caps.VS20Caps.Caps && caps.VS20Caps.DynamicFlowControlDepth >= 24 && caps.VS20Caps.NumTemps >= 13 && caps.VS20Caps.StaticFlowControlDepth)
						strcpy(_vs_profile, "vs_2_a");
					else
						strcpy(_vs_profile, "vs_2_0");

					if(caps.PS20Caps.NumTemps >= 32 && caps.PS20Caps.NumInstructionSlots >= 512)
						strcpy(_ps_profile, "ps_2_b");
					else if(caps.PS20Caps.Caps && caps.PS20Caps.NumTemps >= 22 && caps.PS20Caps.NumInstructionSlots >= 512)
						strcpy(_ps_profile, "ps_2_a");
					else
						strcpy(_ps_profile, "ps_2_0");
				}

				_supports_sm_2 = false;
				_supports_sm_3 = false;
				if(true)
				{
					if((!strcmp(D3DXGetVertexShaderProfile(_device), "vs_2_0") || !strcmp(D3DXGetVertexShaderProfile(_device), "vs_2_a") || !strcmp(D3DXGetVertexShaderProfile(_device), "vs_3_0")) &&
						(!strcmp(D3DXGetPixelShaderProfile(_device), "ps_2_0") || !strcmp(D3DXGetPixelShaderProfile(_device), "ps_2_a") || !strcmp(D3DXGetPixelShaderProfile(_device), "ps_2_b") || !strcmp(D3DXGetPixelShaderProfile(_device), "ps_3_0")))
						_supports_sm_2 = true;
					if(!strcmp(D3DXGetVertexShaderProfile(_device), "vs_3_0") &&
						!strcmp(D3DXGetPixelShaderProfile(_device), "ps_3_0"))
						_supports_sm_3 = true;
				}
			}
		}
		void					d3dGfx::addRecreatable(id3dRecreatable* recreatable)
		{
			recreatable->list_tracking._iter = _list_recreatable.insert(_list_recreatable.end(), recreatable);
			recreatable->list_tracking._list = &_list_recreatable;
		}
		HWND					d3dGfx::getDefaultPresentTarget() const
		{
			return _hwnd;
		}
		void					d3dGfx::setPresentTarget(HWND hwnd)
		{
			_hwnd_present = hwnd;
		}
		HWND					d3dGfx::getPresentTarget() const
		{
			return _hwnd_present;
		}
		size_t					d3dGfx::getAccumPolycount() const
		{
			return _polycount;
		}
		void					d3dGfx::resetAccumPolycount()
		{
			_polycount = 0;
		}
		void								d3dGfx::setViewState					(const rsView& rs)
		{
			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_1_RENDERTARGET );

			_view_state = rs;

			int32_t x, y, dx, dy;
			intersect_area(0, 0, (int32_t)getFramebufferDX(), (int32_t)getFramebufferDY(), rs.viewport.x, rs.viewport.y, rs.viewport.dx, rs.viewport.dy, x, y, dx, dy);
			intersect_area(x, y, dx, dy, rs.cliprect.x + rs.viewport.x, rs.cliprect.y + rs.viewport.y, rs.cliprect.dx, rs.cliprect.dy);

			_empty_viewport = (!dx || !dy);

			if(!_empty_viewport)
			{
				Vec3 s((REAL)rs.viewport.dx / dx, (REAL)rs.viewport.dy / dy, 1.0f);
				Vec3 t(((rs.viewport.x + rs.viewport.dx * 0.5f) - (x + dx * 0.5f)) / dx * 2, ((y + dy * 0.5f) - (rs.viewport.y + rs.viewport.dy * 0.5f)) / dy * 2, 0);

				_proj_adjust = Mat4::scaling(s) * Mat4::translation(t);

				// correct d3d rendertarget upside down issue
				//if(boundRenderTarget())
				//	_proj_adjust *= Mat4::scaling(1, -1, 1);

				// d3d direct mapping texels to pixels
				REAL offx = -1.0f / dx;				//	-0.5f / (dx * 0.5f);
				REAL offy = 1.0f / dy;				//	0.5f / (dy * 0.5f);
				_proj_adjust *= Mat4::translation(Vec3(offx, offy, 0));

				// set device viewport now
				D3DVIEWPORT9 vp;
				vp.X = x;
				vp.Y = y;
				vp.Width = dx;
				vp.Height = dy;
				vp.MinZ = rs.minZ;
				vp.MaxZ = rs.maxZ;

				// correct d3d rendertarget upside down issue
				//if(boundRenderTarget())
				//	vp.Y = (DWORD)getFramebufferDY() - vp.Y - vp.Height;

				_rs_vps.set(_device, vp);
			}
			else
			{
				_proj_adjust = Mat4::identity;
			}

			_cur_render_setup = GfxEnum::RENDER_SETUP_2_VIEWPORT_AND_CLIPRECT;
		}
		const rsView&						d3dGfx::getViewState					() const
		{
			return _view_state;
		}
		bool								d3dGfx::isEmptyRenderRect				() const
		{
			return !!_empty_viewport;
		}
		void								d3dGfx::setViewProjTransform				(const Mat4& view, const Mat4& proj)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_2_VIEWPORT_AND_CLIPRECT );

			_view_transform = view;
			_proj_transform = proj;

			_xform_dirties.FLAGS = ~((size_t)0);

			//_adjusted_proj_transform = adjustProjectionTransform(proj);

			//_adjusted_view_proj_transform = view * _adjusted_proj_transform;
			//_adjusted_world_view_proj_transform = _adjusted_view_proj_transform;

			_cur_render_setup = GfxEnum::RENDER_SETUP_3_VIEWPROJ;
		}
		void								d3dGfx::setWorldViewProjTransform			(const Mat4& world, const Mat4& view, const Mat4& proj)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_2_VIEWPORT_AND_CLIPRECT );

			_world_transform = world;
			_view_transform = view;
			_proj_transform = proj;

			_xform_dirties.FLAGS = ~((size_t)0);

			_cur_render_setup = GfxEnum::RENDER_SETUP_3_VIEWPROJ;
		}
		void								d3dGfx::setViewProjTransform2D				(const Mat4& world)
		{
			setWorldViewProjTransform(world, Mat4::identity, Mat4::ortho(0, (REAL)_view_state.viewport.dy, (REAL)_view_state.viewport.dx, 0, 0, 1, getZNearMapping(), getZFarMapping()));
		}
		Mat4								d3dGfx::adjustProjectionTransform			(const Mat4& proj) const
		{
			return proj * _proj_adjust;
		}
		const Mat4&							d3dGfx::getViewTransform					() const
		{
			return _view_transform;
		}
		const Mat4&							d3dGfx::getUnadjustedProjectionTransform	() const
		{
			return _proj_transform;
		}
		const Mat4&							d3dGfx::getAdjustedProjectionTransform		() const
		{
			if(_xform_dirties.adjusted_proj_transform)
			{
				_adjusted_proj_transform = adjustProjectionTransform(_proj_transform);
				_xform_dirties.adjusted_proj_transform = 0;
			}
			return _adjusted_proj_transform;
		}
		const Mat4&							d3dGfx::getUnadjustedViewProjectionTransform() const
		{
			if(_xform_dirties.view_proj_transform)
			{
				_view_proj_transform = _view_transform * _proj_transform;
				_xform_dirties.view_proj_transform = 0;
			}
			return _view_proj_transform;
		}
		const Mat4&							d3dGfx::getAdjustedViewProjectionTransform	() const
		{
			if(_xform_dirties.adjusted_view_proj_transform)
			{
				_adjusted_view_proj_transform = _view_transform * getAdjustedProjectionTransform();
				_xform_dirties.adjusted_view_proj_transform = 0;
			}
			return _adjusted_view_proj_transform;
		}
		const Mat4&							d3dGfx::getInverseViewTransform				() const
		{
			if(_xform_dirties.inv_view_transform)
			{
				_inv_view_transform = _view_transform.inverse();
				_xform_dirties.inv_view_transform = 0;
			}
			return _inv_view_transform;
		}
		void								d3dGfx::useFixedPipeline				()
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_3_VIEWPROJ );

			_rs_vss.set(_device, NULL);
			_rs_pss.set(_device, NULL);

			_ct_vs = NULL;
			_ct_ps = NULL;

			_device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&_view_transform);
			_device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&_adjusted_proj_transform);

			_cur_render_setup = GfxEnum::RENDER_SETUP_4_PIPELINE;
		}
		void								d3dGfx::useProgrammablePipeline			(const SPtr<Shader>& vs, const SPtr<Shader>& ps)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_3_VIEWPROJ );

			if(!vs && !ps)
				return useFixedPipeline();

			Z_ASSERT( (vs && vs->type() == GfxEnum::SHADERTYPE_VERTEX) && (ps && ps->type() == GfxEnum::SHADERTYPE_PIXEL) );

			if(vs)
			{
				Z_ASSERT( vs->type_info()->exactly(d3dShader::type_info_static()) );

				d3dShader* s = vs.cast<d3dShader>().get();

				Z_ASSERT( s->device() == _device );

				_rs_vss.set(_device, s->vs());
				_ct_vs = s->ct();
			}
			else
			{
				_rs_vss.set(_device, NULL);
				_ct_vs = NULL;
			}
			if(ps)
			{
				Z_ASSERT( ps->type_info()->exactly(d3dShader::type_info_static()) );

				d3dShader* s = ps.cast<d3dShader>().get();

				Z_ASSERT( s->device() == _device );

				_rs_pss.set(_device, s->ps());
				_ct_ps = s->ct();
			}
			else
			{
				_rs_pss.set(_device, NULL);
				_ct_ps = NULL;
			}
			if(!_rs_vss.get() || !_rs_pss.get())
			{
				_device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&_world_transform);			// most of the time redundant, but keep consistency, can use a set flag to keep track whether its changed by setWorldViewProjectionTransform(), but just no waste of efforts here
				_device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&_view_transform);
				_device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&_adjusted_proj_transform);
			}

			_cur_render_setup = GfxEnum::RENDER_SETUP_4_PIPELINE;
		}
		void								d3dGfx::setWorldTransform					(const Mat4& world)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_4_PIPELINE );

			_world_transform = world;

			if(!_rs_vss.get() || _rs_pss.get())
				_device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&world);

			_xform_dirties.world_view_proj_transform = 1;
			_xform_dirties.adjusted_world_view_proj_transform = 1;

			//_world_view_proj_transform = _world_transform * _view_proj_transform;
			//_adjusted_world_view_proj_transform = _world_transform * _adjusted_view_proj_transform;

			_cur_render_setup = GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO;
		}
		const Mat4&							d3dGfx::getWorldTransform					() const
		{
			return _world_transform;
		}
		const Mat4&							d3dGfx::getUnadjustedWorldViewProjectionTransform() const
		{
			if(_xform_dirties.world_view_proj_transform)
			{
				_world_view_proj_transform = _world_transform * getUnadjustedViewProjectionTransform();
				_xform_dirties.world_view_proj_transform = 0;
			}
			return _world_view_proj_transform;
		}
		const Mat4&							d3dGfx::getAdjustedWorldViewProjectionTransform() const
		{
			if(_xform_dirties.adjusted_world_view_proj_transform)
			{
				_adjusted_world_view_proj_transform = _world_transform * getAdjustedViewProjectionTransform();
				_xform_dirties.adjusted_world_view_proj_transform = 0;
			}
			return _adjusted_world_view_proj_transform;
		}
		void								d3dGfx::setShadeState					(const rsShade& rs)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_4_PIPELINE );

			_rs_shade_mode.set(_device, map_D3DSHADEMODE(rs.shade_mode));
			_rs_fill_mode.set(_device, map_D3DFILLMODE(rs.polygon_mode));

			_rs_cull_mode.set(_device, rs.enable.cull ? map_D3DCULL(rs.cull_mode) : D3DCULL_NONE);
			if(!rs.enable.depth_test && !rs.write.depth)
				_rs_z_enable.set(_device, D3DZB_FALSE);
			else
			{
				_rs_z_enable.set(_device, D3DZB_TRUE);
				_rs_z_func.set(_device, rs.enable.depth_test ? map_D3DCMPFUNC(rs.depth_func) : D3DCMP_ALWAYS);
				_rs_z_write_enable.set(_device, rs.write.depth ? TRUE : FALSE);
			}
			if(rs.enable.alpha_test)
			{
				_rs_alpha_test_enable.set(_device, TRUE);
				_rs_alpha_func.set(_device, map_D3DCMPFUNC(rs.alpha_func));
				_rs_alpha_ref.set(_device, min(max((DWORD)0, (DWORD)(rs.alpha_ref * 255)), (DWORD)255));
			}
			else
				_rs_alpha_test_enable.set(_device, FALSE);
			if(rs.enable.blend)
			{
				_rs_alpha_blend_enable.set(_device, TRUE);
				_rs_src_blend.set(_device, map_D3DBLEND(rs.src_blend_factor));
				_rs_dest_blend.set(_device, map_D3DBLEND(rs.dst_blend_factor));
				_rs_blend_op.set(_device, map_D3DBLENDOP(rs.blend_operation));
			}
			else
				_rs_alpha_blend_enable.set(_device, FALSE);
			if(rs.enable.stencil)
			{
				_rs_stencil_enable.set(_device, TRUE);
				_rs_stencil_ref.set(_device, rs.stencil.ref);
				_rs_stencil_mask.set(_device, rs.stencil.mask);
				_rs_stencil_write_mask.set(_device, rs.write.stencil);
				if(rs.enable.two_side_stencil && !rs.enable.cull)
				{
					_rs_two_side_stencil_mode.set(_device, TRUE);

					_rs_ccw_stencil_func.set(_device, map_D3DCMPFUNC(rs.stencil.front_func));
					_rs_ccw_stencil_fail.set(_device, map_D3DSTENCILOP(rs.stencil_op.front_fail));
					_rs_ccw_stencil_zfail.set(_device, map_D3DSTENCILOP(rs.stencil_op.front_zfail));
					_rs_ccw_stencil_pass.set(_device, map_D3DSTENCILOP(rs.stencil_op.front_pass));

					_rs_stencil_func.set(_device, map_D3DCMPFUNC(rs.stencil.back_func));
					_rs_stencil_fail.set(_device, map_D3DSTENCILOP(rs.stencil_op.back_fail));
					_rs_stencil_zfail.set(_device, map_D3DSTENCILOP(rs.stencil_op.back_zfail));
					_rs_stencil_pass.set(_device, map_D3DSTENCILOP(rs.stencil_op.back_pass));
				}
				else
				{
					_rs_two_side_stencil_mode.set(_device, FALSE);

					_rs_stencil_func.set(_device, map_D3DCMPFUNC(rs.stencil.front_func));
					_rs_stencil_fail.set(_device, map_D3DSTENCILOP(rs.stencil_op.front_fail));
					_rs_stencil_zfail.set(_device, map_D3DSTENCILOP(rs.stencil_op.front_zfail));
					_rs_stencil_pass.set(_device, map_D3DSTENCILOP(rs.stencil_op.front_pass));
				}
			}
			else
				_rs_stencil_enable.set(_device, FALSE);

			_rs_color_write_enable.set(_device, (rs.write.R ? D3DCOLORWRITEENABLE_RED : 0) | (rs.write.G ? D3DCOLORWRITEENABLE_GREEN : 0) | (rs.write.B ? D3DCOLORWRITEENABLE_BLUE : 0) | (rs.write.A ? D3DCOLORWRITEENABLE_ALPHA : 0));

			if(rs.enable.scissor_test)
			{
				_rs_scissor_test_enable.set(_device, TRUE);

				// clamp otherwise problematic
				int32_t x, y, x2, y2;
				x = min(max(0, rs.scissor.x), _view_state.viewport.dx);
				y = min(max(0, rs.scissor.y), _view_state.viewport.dy);
				x2 = min(max(x, x + rs.scissor.dx), _view_state.viewport.dx);
				y2 = min(max(y, y + rs.scissor.dy), _view_state.viewport.dy);

				// makes scissor effect inside viewport
				x += _view_state.viewport.x;
				y += _view_state.viewport.y;
				x2 += _view_state.viewport.x;
				y2 += _view_state.viewport.y;

				// correct d3d bound rendertarget up-side down issue
				//if(boundRenderTarget())
				//{
				//	int32_t t = y;
				//	y = _view_state.viewport.y + _view_state.viewport.dy - y2;
				//	y2 = _view_state.viewport.y + _view_state.viewport.dy - t;
				//}

				RECT v = {x, y, x2, y2};
				_rs_scissor.set(_device, v);
			}
			else
				_rs_scissor_test_enable.set(_device, FALSE);
			if(rs.clip_plane_count)
			{
				if(_rs_vss.get())
				{
					// using vertex shader, need transforming the world-space clip planes to projection-space ones
					Mat4 m = (_adjusted_view_proj_transform).inverse().transpose();

					size_t c = min(rs.clip_plane_count, (size_t)d3dGfx::MAX_CLIP_PLANES);
					for(size_t i = 0; i < c; ++i)
					{
						Plane p = ((Plane*)rs.clip_planes[i])->transformedBy(m, true);
						_rs_cps[i].set(_device, (DWORD)i, (REAL*)&p);
					}
				}
				else
				{
					size_t c = min(rs.clip_plane_count, (size_t)d3dGfx::MAX_CLIP_PLANES);
					for(size_t i = 0; i < c; ++i)
						_rs_cps[i].set(_device, (DWORD)i, rs.clip_planes[i]);
				}

				_rs_clip_plane_enable.set(_device, (1 << (1 + rs.clip_plane_count)) - 1);
			}
			else
				_rs_clip_plane_enable.set(_device, 0);

			if(rs.sampler_use_bitset)
			{
				size_t base = 0;
				size_t b = rs.sampler_use_bitset;
				while(b)
				{
					if(!(b & 0xff))
					{
						b >>= 8;
						base += 8;
						continue;
					}
					size_t p = (size_t)FIRSTBIT((uint8_t)(b & 0xff));
					size_t index = base + p;
					b &= ~(1 << p);

					if(index >= d3dGfx::MAX_SAMPLERS)
						break;

					d3dSS& d3dss = _rs_ss[index];
					const rsShade::SS& ss = rs.ss[index];

					d3dss.wrap_u.set(_device, (DWORD)index, map_D3DTEXTUREADDRESS(ss.u_wrap));
					d3dss.wrap_v.set(_device, (DWORD)index, map_D3DTEXTUREADDRESS(ss.v_wrap));
					d3dss.wrap_w.set(_device, (DWORD)index, map_D3DTEXTUREADDRESS(ss.w_wrap));

					d3dss.filter_min.set(_device, (DWORD)index, map_D3DTEXTUREFILTERTYPE(ss.min_filter));
					d3dss.filter_mag.set(_device, (DWORD)index, map_D3DTEXTUREFILTERTYPE(ss.mag_filter));
					d3dss.filter_mip.set(_device, (DWORD)index, map_D3DTEXTUREFILTERTYPE(ss.mip_filter));
				}
			}

			// check if using fixed pipeline
			if(!_rs_vss.get() || !_rs_pss.get())
			{
				Z_ASSERT( rs.fixed_pipeline );

				_rs_texture_factor.set(_device, toD3DCOLOR(*(Vec4*)rs.fixed.default_color));

				if(rs.fixed.enable.lighting)
				{
					_rs_lighting.set(_device, TRUE);

					D3DMATERIAL9 m;
					// order is different, cannot use just 1 memcpy
					m.Diffuse = *(D3DCOLORVALUE*)rs.fixed.lighting.material.diffuse;
					m.Ambient = *(D3DCOLORVALUE*)rs.fixed.lighting.material.ambient;
					m.Specular = *(D3DCOLORVALUE*)rs.fixed.lighting.material.specular;
					m.Emissive = *(D3DCOLORVALUE*)rs.fixed.lighting.material.emissive;
					m.Power = rs.fixed.lighting.material.shininess;
					_rs_ms.set(_device, m);

					_rs_ambient.set(_device, toD3DCOLOR(*(Vec4*)rs.fixed.lighting.light_ambient));

					size_t c = min(rs.fixed.lighting.light_count, (size_t)d3dGfx::MAX_LIGHTS);
					for(size_t i = 0; i < c; ++i)
					{
						const rsShade::FIXED::LIGHTING::LIGHT& l = rs.fixed.lighting.lights[i];
						D3DLIGHT9 d3dl;
						d3dl.Type = map_D3DLIGHTTYPE(l.type);
						d3dl.Diffuse = *(D3DCOLORVALUE*)l.diffuse;
						d3dl.Specular = *(D3DCOLORVALUE*)l.specular;
						d3dl.Ambient = *(D3DCOLORVALUE*)l.ambient;
						d3dl.Position = *(D3DVECTOR*)l.position;
						d3dl.Direction = *(D3DVECTOR*)l.direction;
						d3dl.Range = FLT_MAX_SQRT;
						d3dl.Falloff = l.falloff;
						d3dl.Attenuation0 = l.attenuation[0];
						d3dl.Attenuation1 = l.attenuation[1];
						d3dl.Attenuation2 = l.attenuation[2];
						d3dl.Theta = l.inner_cone * DEG_TO_RAD;
						d3dl.Phi = l.outer_cone * DEG_TO_RAD;

						_rs_ls[i].set(_device, (DWORD)i, d3dl);
						_rs_les[i].set(_device, (DWORD)i, TRUE);
					}

					for(size_t i = rs.fixed.lighting.light_count; i < (size_t)d3dGfx::MAX_LIGHTS; ++i)
						_rs_les[i].set(_device, (DWORD)i, FALSE);
				}
				else
				{
					_rs_lighting.set(_device, FALSE);
				}

				if(rs.fixed.enable.fog)
				{
					_rs_fog_enable.set(_device, TRUE);

					_rs_fog_table_mode.set(_device, map_D3DFOGMODE(rs.fixed.fog.mode));
					_rs_fog_density.set(_device, *(DWORD*)&rs.fixed.fog.density);
					_rs_fog_start.set(_device, *(DWORD*)&rs.fixed.fog.start);
					_rs_fog_end.set(_device, *(DWORD*)&rs.fixed.fog.end);
					_rs_fog_color.set(_device, toD3DCOLOR(rs.fixed.fog.color));
				}
				else
				{
					_rs_fog_enable.set(_device, FALSE);
				}

				_rs_normalize_normals.set(_device, rs.fixed.enable.normalize? TRUE: FALSE);
				_rs_specular_enable.set(_device, rs.fixed.enable.specular? TRUE: FALSE);

				size_t c = min(rs.fixed.texture_stage_count, (size_t)d3dGfx::MAX_STAGES);
				for(size_t i = 0; i < c; ++i)
				{
					d3dTSS& d3dtss = _rs_tss[i];
					const rsShade::FIXED::TSS& tss = rs.fixed.tss[i];

					d3dtss.color_op.set(_device, (DWORD)i, map_D3DTEXTUREOP(tss.color_op));
					d3dtss.color_arg1.set(_device, (DWORD)i, map_D3DTA(tss.color_arg1));
					d3dtss.color_arg2.set(_device, (DWORD)i, map_D3DTA(tss.color_arg2));
					d3dtss.alpha_op.set(_device, (DWORD)i, map_D3DTEXTUREOP(tss.alpha_op));
					d3dtss.alpha_arg1.set(_device, (DWORD)i, map_D3DTA(tss.alpha_arg1));
					d3dtss.alpha_arg2.set(_device, (DWORD)i, map_D3DTA(tss.alpha_arg2));

					_rs_tct[i].set(_device, (DWORD)i, rs.fixed.tct[i]);
				}
				if(rs.fixed.texture_stage_count < (size_t)d3dGfx::MAX_STAGES)
				{
					_rs_tss[rs.fixed.texture_stage_count].color_op.set(_device, (DWORD)rs.fixed.texture_stage_count, D3DTOP_DISABLE);
				}
			}

			//_shade_state = rs;

			_cur_render_setup = GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO;
		}
		//const rsShade&						d3dGfx::getShadeState					() const
		//{
		//	return _shade_state;
		//}
		void								d3dGfx::bindTexture						(size_t stage, const SPtr<TexBase>& tex)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_4_PIPELINE );

			if(stage < d3dGfx::MAX_SAMPLERS)
			{
				SPtr<TexBase> t = tex;
				while(t)
				{
					SPtr<TexBase> s = t->getRealTexture();
					if(s == t)
						break;
					t = s;
				}

				if(t)
				{
					switch(t->tex_target())
					{
					case GfxEnum::TEXTARGET_2D:
						Z_ASSERT( t->type_info()->exactly(d3dTex2D::type_info_static()) );
						Z_ASSERT( t.cast<d3dTex2D>()->device() == _device );
						_rs_ts[stage].set(_device, (DWORD)stage, t.cast<d3dTex2D>()->d3d_tex());
						break;
					case GfxEnum::TEXTARGET_3D:
						break;
					case GfxEnum::TEXTARGET_CUBE:
						Z_ASSERT( t->type_info()->exactly(d3dTexCube::type_info_static()) );
						Z_ASSERT( t.cast<d3dTexCube>()->device() == _device );
						_rs_ts[stage].set(_device, (DWORD)stage, t.cast<d3dTexCube>()->d3d_tex());
						break;
					}
				}
				else
					_rs_ts[stage].set(_device, (DWORD)stage, NULL);
			}

			_cur_render_setup = GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO;
		}
		void					d3dGfx::bindVBO(const SPtr<VBO>& vbo, uint8_t stride, size_t offset)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_4_PIPELINE );

			Z_ASSERT( vbo );

			Z_ASSERT( vbo->type_info()->exactly(d3dVBO::type_info_static()) );

			Z_ASSERT( stride );

			Z_ASSERT(_vbo_bindings < MAX_VBO_BINDINGS);

			if(_vbo_bindings < MAX_VBO_BINDINGS)
			{
				VBO_BINDING& binding = *(_array_vbo_binding + _vbo_bindings);

				binding.vbo = (d3dVBO*)vbo.get();
				binding.offset = offset;
				binding.stride = stride;

				++_vbo_bindings;
			}

			_cur_render_setup = GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO;
		}
		void					d3dGfx::bindVertexAttrib(uint8_t offset, d3dGfx::VERTEX_ATTRIB_SEMANTIC semantic, uint8_t comps)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_4_PIPELINE );

			Z_ASSERT( _vbo_bindings );
			Z_ASSERT( offset + comps * sizeof(REAL) <= _array_vbo_binding[_vbo_bindings - 1].stride );

			Z_ASSERT(_vertex_attrib_bindings < MAX_VERTEX_ATTRIB_BINDINGS);
			if(_vertex_attrib_bindings >= MAX_VERTEX_ATTRIB_BINDINGS)
				return;

			VERTEX_ATTRIB_BINDING& binding = *(_array_vertex_attrib_binding + _vertex_attrib_bindings);

			binding.stream = (uint8_t)(_vbo_bindings - 1);
			binding.semantic = semantic;
			binding.components = comps;
			binding.offset = offset;

			++_vertex_attrib_bindings;

			_cur_render_setup = GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO;
		}
		void					d3dGfx::bindVertexPosition				(uint8_t offset, uint8_t comps)
		{
			Z_ASSERT(comps >= 2 && comps <= 4);
			if(comps < 2 || comps > 4)
				return;

			bindVertexAttrib(offset, VERTEX_ATTRIB_SEMANTIC_POSITION, comps - 1);
		}
		void					d3dGfx::bindVertexNormal				(uint8_t offset)
		{
			bindVertexAttrib(offset, VERTEX_ATTRIB_SEMANTIC_NORMAL, 3 - 1);
		}
		void					d3dGfx::bindVertexColor					(uint8_t offset)
		{
			bindVertexAttrib(offset, VERTEX_ATTRIB_SEMANTIC_COLOR, 4 - 1);
		}
		void					d3dGfx::bindVertexTexCoord				(uint8_t offset, uint8_t index, uint8_t comps)
		{
			Z_STATIC_ASSERT( VERTEX_ATTRIB_SEMANTIC_NUM_TEXCOORD == MAX_SAMPLERS, UNMATCH_PLEASE_FIX_IT );

			Z_ASSERT(comps >= 1 && comps <= 4);
			if(comps < 1 || comps > 4)
				return;

			Z_ASSERT(index >= 0 && index < MAX_SAMPLERS);
			if(index < 0 || index >= MAX_SAMPLERS)
				return;

			bindVertexAttrib(offset, (VERTEX_ATTRIB_SEMANTIC)(VERTEX_ATTRIB_SEMANTIC_TEXCOORD0 + index), comps - 1);
		}
		void								d3dGfx::setShaderUniforms					(const ShaderUniforms& s)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_4_PIPELINE );

			Z_STATIC_ASSERT(sizeof(REAL) == sizeof(FLOAT), ASSUMPTION_WRONG1);
			Z_STATIC_ASSERT(sizeof(((ShaderUniforms::ENTRY*)0)->count) == 1, ASSUMPTION_WRONG2);

			const ShaderUniforms::ENTRY* e = s.array_entry();
			size_t count = s.entries();
			const uint8_t* buffer = s.value_buffer();
			//char name[2048] = "$";

			HRESULT ret = S_OK;

			for(size_t i = 0; i < count; ++i)
			{
				const ShaderUniforms::ENTRY& c = *e;

				//strcpy(name + 1, c.name);
				const char* name = c.name;

				switch(c.type)
				{
				case ShaderUniforms::TYPE_BOOL:
					if(c.array_form)
					{
						const bool* array_value = (const bool*)(buffer + c.value_addr);
						BOOL array_value2[256];
						for(size_t j = 0; j <= (size_t)c.count; ++j)
							array_value2[j] = array_value[j];
						switch(c.shader_type)
						{
						case GfxEnum::SHADERTYPE_VERTEX: ret = _ct_vs->SetBoolArray(_device, name, array_value2, (UINT)(c.count + 1)); break;
						case GfxEnum::SHADERTYPE_PIXEL: ret = _ct_ps->SetBoolArray(_device, name, array_value2, (UINT)(c.count + 1)); break;
						default: Z_ASSERT( false ); break;
						}
					}
					else
					{
						switch(c.shader_type)
						{
						case GfxEnum::SHADERTYPE_VERTEX: ret = _ct_vs->SetBool(_device, name, *(const bool*)(buffer + c.value_addr)); break;
						case GfxEnum::SHADERTYPE_PIXEL: ret = _ct_ps->SetBool(_device, name, *(const bool*)(buffer + c.value_addr)); break;
						default: Z_ASSERT( false ); break;
						}
					}
					break;
				case ShaderUniforms::TYPE_INTEGER:
					if(c.array_form)
					{
						const int* array_value = (const int*)(buffer + c.value_addr);
						INT array_value2[256];
						for(size_t j = 0; j <= (size_t)c.count; ++j)
							array_value2[j] = array_value[j];
						switch(c.shader_type)
						{
						case GfxEnum::SHADERTYPE_VERTEX: ret = _ct_vs->SetIntArray(_device, name, array_value2, (UINT)(c.count + 1)); break;
						case GfxEnum::SHADERTYPE_PIXEL: ret = _ct_ps->SetIntArray(_device, name, array_value2, (UINT)(c.count + 1)); break;
						default: Z_ASSERT( false ); break;
						}
					}
					else
					{
						switch(c.shader_type)
						{
						case GfxEnum::SHADERTYPE_VERTEX: ret = _ct_vs->SetInt(_device, name, *(const int*)(buffer + c.value_addr)); break;
						case GfxEnum::SHADERTYPE_PIXEL: ret = _ct_ps->SetInt(_device, name, *(const int*)(buffer + c.value_addr)); break;
						default: Z_ASSERT( false ); break;
						}
					}
					break;
				case ShaderUniforms::TYPE_REAL:
					if(c.array_form)
					{
						switch(c.shader_type)
						{
						case GfxEnum::SHADERTYPE_VERTEX: ret = _ct_vs->SetFloatArray(_device, name, (const REAL*)(buffer + c.value_addr), (UINT)(c.count + 1)); break;
						case GfxEnum::SHADERTYPE_PIXEL: ret = _ct_ps->SetFloatArray(_device, name, (const REAL*)(buffer + c.value_addr), (UINT)(c.count + 1)); break;
						default: Z_ASSERT( false ); break;
						}
						if(ret)
						{
							int k = sizeof(k);
						}
					}
					else
					{
						switch(c.shader_type)
						{
						case GfxEnum::SHADERTYPE_VERTEX: ret = _ct_vs->SetFloat(_device, name, *(const REAL*)(buffer + c.value_addr)); break;
						case GfxEnum::SHADERTYPE_PIXEL: ret = _ct_ps->SetFloat(_device, name, *(const REAL*)(buffer + c.value_addr)); break;
						default: Z_ASSERT( false ); break;
						}
					}
					break;
				default:
					Z_ASSERT( false );
					break;
				}

				Z_ASSERT( !FAILED( ret ) );

				++e;
			}

			_cur_render_setup = GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO;
		}
		void								d3dGfx::setShaderUniforms					(const ShaderUniforms2& s)
		{
			Z_STATIC_ASSERT( sizeof(BOOL) == sizeof(s.buffersets[0].boolean.buffer[0]), WRONG_ASSUMPTION );

			if(s.buffersets[0].float4.effecting_count)
				_device->SetVertexShaderConstantF((UINT)0, &s.buffersets[0].float4.buffer[0][0], (UINT)s.buffersets[0].float4.effecting_count);
			if(s.buffersets[0].int4.effecting_count)
				_device->SetVertexShaderConstantI((UINT)0, &s.buffersets[0].int4.buffer[0][0], (UINT)s.buffersets[0].int4.effecting_count);
			if(s.buffersets[0].boolean.effecting_count)
				_device->SetVertexShaderConstantI((UINT)0, (BOOL*)&s.buffersets[0].boolean.buffer[0], (UINT)s.buffersets[0].boolean.effecting_count);

			if(s.buffersets[1].float4.effecting_count)
				_device->SetPixelShaderConstantF((UINT)0, &s.buffersets[1].float4.buffer[0][0], (UINT)s.buffersets[1].float4.effecting_count);
			if(s.buffersets[1].int4.effecting_count)
				_device->SetPixelShaderConstantI((UINT)0, &s.buffersets[1].int4.buffer[0][0], (UINT)s.buffersets[1].int4.effecting_count);
			if(s.buffersets[1].boolean.effecting_count)
				_device->SetPixelShaderConstantI((UINT)0, (BOOL*)&s.buffersets[1].boolean.buffer[0], (UINT)s.buffersets[1].boolean.effecting_count);
		}
		IDirect3DVertexDeclaration9*		d3dGfx::realizeD3DVertexDecl(const d3dGfx::ALL_VERTEX_ATTRIB_BINDING& binding)
		{
			VERTEX_DECL_MAP_TYPE::iterator iter = _map_d3d_vertex_decl.find(binding);
			if(iter != _map_d3d_vertex_decl.end())
				return iter->second;

			D3DVERTEXELEMENT9 ele[ MAX_VERTEX_ATTRIB_BINDINGS + 1];
			D3DVERTEXELEMENT9* e = ele;
			d3dGfx::VERTEX_ATTRIB_BINDING* a = _array_vertex_attrib_binding;
			for(size_t i = 0; i < _vertex_attrib_bindings; ++i)
			{
				e->Stream = a->stream;
				e->Offset = a->offset;
				e->Type = (BYTE)(D3DDECLTYPE_FLOAT1 + a->components);
				e->Method = D3DDECLMETHOD_DEFAULT;
				switch(a->semantic)
				{
				case d3dGfx::VERTEX_ATTRIB_SEMANTIC_POSITION:
					e->Usage = D3DDECLUSAGE_POSITION;
					e->UsageIndex = 0;
					break;
				case d3dGfx::VERTEX_ATTRIB_SEMANTIC_NORMAL:
					e->Usage = D3DDECLUSAGE_NORMAL;
					e->UsageIndex = 0;
					break;
				case d3dGfx::VERTEX_ATTRIB_SEMANTIC_COLOR:
					e->Usage = D3DDECLUSAGE_COLOR;
					e->UsageIndex = 0;
					break;
				default:
					if(a->semantic >= d3dGfx::VERTEX_ATTRIB_SEMANTIC_TEXCOORD0 && a->semantic < d3dGfx::VERTEX_ATTRIB_SEMANTIC_TEXCOORD_BOUND)
					{
						e->Usage = D3DDECLUSAGE_TEXCOORD;
						e->UsageIndex = (BYTE)(a->semantic - d3dGfx::VERTEX_ATTRIB_SEMANTIC_TEXCOORD0);
					}
					break;
				}
			
				++e;
				++a;
			}
			D3DVERTEXELEMENT9 end = D3DDECL_END();
			*e = end;

			IDirect3DVertexDeclaration9* vert_decl = NULL;
			_device->CreateVertexDeclaration(ele, &vert_decl);
			if(!vert_decl)
				return NULL;
			_map_d3d_vertex_decl[binding] = vert_decl;
			return vert_decl;
		}
		void								d3dGfx::clearD3DVertexDecls()
		{
			VERTEX_DECL_MAP_TYPE::iterator iter;
			for(iter = _map_d3d_vertex_decl.begin(); iter != _map_d3d_vertex_decl.end(); ++iter)
				if(iter->second)
					iter->second->Release();
			_map_d3d_vertex_decl.clear();
		}
		bool								d3dGfx::realizeStreamsAndVertDecl			(size_t numVertsOffset)
		{
			if(!_vbo_bindings || !_vertex_attrib_bindings)
				return false;

			memset(_array_vertex_attrib_binding + _vertex_attrib_bindings, 0, sizeof(VERTEX_ATTRIB_BINDING) * (_countof(_array_vertex_attrib_binding) - _vertex_attrib_bindings));
			IDirect3DVertexDeclaration9* vert_decl = realizeD3DVertexDecl(*(ALL_VERTEX_ATTRIB_BINDING*)_array_vertex_attrib_binding);
			if(!vert_decl)
			{
				_vbo_bindings = 0;
				_vertex_attrib_bindings = 0;

				return false;
			}

			if(!numVertsOffset)
			{
				VBO_BINDING* binding = _array_vbo_binding;
				size_t i;
				for(i = 0; i < _vbo_bindings; ++i)
				{
					_device->SetStreamSource((UINT)i, binding->vbo->handle(), (UINT)binding->offset, (UINT)binding->stride);
					++binding;
				}

				// maybe not be necessary
				//for(; i < _last_used_vbos; ++i)
				//	_device->SetStreamSource((UINT)i, NULL, 0, 0);

				_last_used_vbos = _vbo_bindings;
			}
			else
			{
				VBO_BINDING* binding = _array_vbo_binding;
				size_t i;
				for(i = 0; i < _vbo_bindings; ++i)
				{
					_device->SetStreamSource((UINT)i, binding->vbo->handle(), (UINT)binding->offset + (UINT)(binding->stride * numVertsOffset), (UINT)binding->stride);
					++binding;
				}

				_last_used_vbos = _vbo_bindings;
			}

			_rs_vds.set(_device, vert_decl);

			return true;
		}
		void								d3dGfx::clearVBOAndVertexAttribBindings		()
		{
			_vbo_bindings = 0;
			_vertex_attrib_bindings = 0;
		}
		void								d3dGfx::drawPrimitives(GfxEnum::PRIMTYPE pt, size_t start_vert, size_t prim_count)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO );

			if(!realizeStreamsAndVertDecl(0))
			{
				Z_ASSERT( false );
				return;
			}

			if(true)
			{
				size_t max_prim_count = _caps.MaxPrimitiveCount;
				switch(pt)
				{
				case GfxEnum::PRIMTYPE_POINTS:
					{
						if((size_t)_caps.MaxPrimitiveCount >= prim_count)
						{
							HRESULT ret = _device->DrawPrimitive(D3DPT_POINTLIST, (UINT)start_vert, (UINT)prim_count);
							Z_ASSERT( ret == S_OK );
							break;
						}
						for(size_t i = 0; i < prim_count;)
						{
							size_t batch = min(prim_count - i, max_prim_count);
							HRESULT ret = _device->DrawPrimitive(D3DPT_POINTLIST, (UINT)i, (UINT)batch);
							Z_ASSERT( ret == S_OK );

							i += batch;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_LINES:
					{
						if((size_t)_caps.MaxPrimitiveCount >= (prim_count << 1))
						{
							HRESULT ret = _device->DrawPrimitive(D3DPT_LINELIST, (UINT)start_vert, (UINT)prim_count);
							Z_ASSERT( ret == S_OK );
							break;
						}
						for(size_t i = 0; i < prim_count;)
						{
							size_t batch = min(prim_count - i, max_prim_count >> 1);
							HRESULT ret = _device->DrawPrimitive(D3DPT_LINELIST, (UINT)(i << 1), (UINT)batch);
							Z_ASSERT( ret == S_OK );

							i += batch;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_LINE_STRIP:
					{
						if((size_t)_caps.MaxPrimitiveCount >= prim_count + 1)
						{
							HRESULT ret = _device->DrawPrimitive(D3DPT_LINESTRIP, (UINT)start_vert, (UINT)prim_count);
							Z_ASSERT( ret == S_OK );
							break;
						}
						size_t start_vert2 = start_vert;
						for(size_t i = 0; i < prim_count;)
						{
							size_t batch = min(prim_count - i, max_prim_count - 1);
							HRESULT ret = _device->DrawPrimitive(D3DPT_LINESTRIP, (UINT)start_vert2, (UINT)batch);
							Z_ASSERT( ret == S_OK );

							i += batch;
							start_vert2 += batch;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_TRIANGLES:
					{
						if((size_t)_caps.MaxPrimitiveCount >= ((prim_count << 1) + prim_count))
						{
							HRESULT ret = _device->DrawPrimitive(D3DPT_TRIANGLELIST, (UINT)start_vert, (UINT)prim_count);
							Z_ASSERT( ret == S_OK );
							break;
						}
						for(size_t i = 0; i < prim_count;)
						{
							size_t batch = min(prim_count - i, max_prim_count / 3);
							HRESULT ret = _device->DrawPrimitive(D3DPT_TRIANGLELIST, (UINT)((i << 1) + i), (UINT)batch);
							Z_ASSERT( ret == S_OK );

							i += batch;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_TRIANGLE_STRIP:
					{
						if((size_t)_caps.MaxPrimitiveCount >= prim_count + 2)
						{
							HRESULT ret = _device->DrawPrimitive(D3DPT_TRIANGLESTRIP, (UINT)start_vert, (UINT)prim_count);
							Z_ASSERT( ret == S_OK );
							break;
						}
						size_t start_vert2 = start_vert;
						for(size_t i = 0; i < prim_count;)
						{
							size_t batch = min(prim_count - i, max_prim_count - 2);
							if(i + batch != prim_count && (batch & 1))
								batch &= (size_t)(~(size_t)1);
							HRESULT ret = _device->DrawPrimitive(D3DPT_TRIANGLESTRIP, (UINT)start_vert2, (UINT)batch);
							Z_ASSERT( ret == S_OK );

							i += batch;
							start_vert2 += batch;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_TRIANGLE_FAN:
					{
						// unsupported

						Z_ASSERT( false );
					}
					break;
				}
			}

			clearVBOAndVertexAttribBindings();

			_polycount += prim_count;
		}
		void								d3dGfx::drawPrimitives						(GfxEnum::PRIMTYPE pt, const Jotter& jotter)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO );

			Z_ASSERT( jotter.vertices() );

			uint32_t s = jotter.vertex_size() * jotter.vertices();

			bool no_overwrite = true;
			if(!_jotter.vbo || _jotter.vbo->length() < s)
			{
				_jotter.vbo = createVBO(max(s, (uint32_t)8192), true);
				_jotter.used_length = 0;
			}
			else if(_jotter.vbo->length() - _jotter.used_length < s)
			{
				_jotter.used_length = 0;
				no_overwrite = false;
			}

			Z_ASSERT( _jotter.vbo );

			_jotter.vbo->lock(_jotter.used_length, s, no_overwrite);
			memcpy(_jotter.vbo->pointer(), jotter.buffer(), s);
			_jotter.vbo->unlock();

			bindVBO(_jotter.vbo, (uint8_t)jotter.vertex_size(), _jotter.used_length);

			_jotter.used_length += s;

			jotter.bindVertexAttribs(this);
			size_t count = 0;
			switch(pt)
			{
			case GfxEnum::PRIMTYPE_POINTS:							count = jotter.vertices();				break;
			case GfxEnum::PRIMTYPE_LINES:							count = (jotter.vertices() >> 1);		break;
			case GfxEnum::PRIMTYPE_LINE_STRIP:						count = jotter.vertices() - 1;			break;
			case GfxEnum::PRIMTYPE_TRIANGLES:						count = jotter.vertices() / 3;			break;
			case GfxEnum::PRIMTYPE_TRIANGLE_STRIP:					count = jotter.vertices() - 2;			break;
			case GfxEnum::PRIMTYPE_TRIANGLE_FAN:					count = jotter.vertices() - 2;			break;
			default:												Z_ASSERT( false );						break;
			}

			// demonstrate of infinite loop
			//for(size_t k = 1; k >= 0; --k)
			//{
			//}

			Z_ASSERT( count );

			drawPrimitives(pt, 0, count);
		}
		void								d3dGfx::drawIndexedPrimitives				(GfxEnum::PRIMTYPE pt, const SPtr<IBO>& ibo, size_t start_index, size_t prim_count)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO );

			Z_ASSERT( ibo );

			Z_ASSERT( ibo->type_info()->exactly(d3dIBO::type_info_static()) );

			if(!realizeStreamsAndVertDecl(0))
			{
				Z_ASSERT( false );
				return;
			}

			size_t max_verts = 0x7fffffff;

			if(true)
			{
				VBO_BINDING* end_binding = _array_vbo_binding + _vbo_bindings;
				for(VBO_BINDING* binding = _array_vbo_binding; binding != end_binding; ++binding)
					max_verts = min(max_verts, (binding->vbo->length() - binding->offset) / binding->stride);
			}

			Z_ASSERT( max_verts && max_verts < 0x7fffffff );

			_device->SetIndices(((d3dIBO*)ibo.get())->handle());

			HRESULT ret = _device->DrawIndexedPrimitive(map_D3DPRIMITIVETYPE(pt), 0, 0, (UINT)max_verts, (UINT)start_index, (UINT)prim_count);
			Z_ASSERT( ret == S_OK );

			clearVBOAndVertexAttribBindings();

			_polycount += prim_count;
		}
		void								d3dGfx::drawIndexedPrimitives				(GfxEnum::PRIMTYPE pt, const uint32_t* indices, size_t index_count)
		{
			if(_empty_viewport)
				return;

			Z_ASSERT( _cur_render_setup >= GfxEnum::RENDER_SETUP_5_TRANSFORM_SHADESTATE_SHADERCONSTANT_TEXTURE_VBO );

			if(_caps.MaxVertexIndex < 65536 || _caps.MaxPrimitiveCount < 65536)
			{
				uint32_t max_vertex_index = (uint32_t)min((uint32_t)_caps.MaxVertexIndex, (uint32_t)65535);
				uint32_t max_prim = (uint32_t)min((uint32_t)_caps.MaxPrimitiveCount, (uint32_t)65535);
				switch(pt)
				{
				case GfxEnum::PRIMTYPE_POINTS:
					{
						Z_ASSERT( false );
					}
					break;
				case GfxEnum::PRIMTYPE_LINES:
					{
						const uint32_t* start = indices;
						const uint32_t* end = indices + ((index_count >> 1) << 1);
						while(true)
						{
							bool restart = false;

							if(start == end)
								break;

							const uint32_t* p = start;

							uint32_t min_index = (uint32_t)min(p[0], p[1]);
							uint32_t max_index = (uint32_t)max(p[0], p[1]);
							uint32_t prim_count = 1;

							if(max_index - min_index > max_vertex_index)
							{
								// cannot realize this primitive
								start += 2;
								continue;
							}
							for(p += 2; p != end;)
							{
								uint32_t min_index2 = (uint32_t)min(min_index, min(p[0], p[1]));
								uint32_t max_index2 = (uint32_t)max(max_index, max(p[0], p[1]));

								if(max_index2 - min_index2 > max_vertex_index || prim_count >= max_prim)
								{
									// flush...
									__draw_indexed_16(D3DPT_LINELIST, start, prim_count << 1, prim_count, min_index, max_index);

									start = p;
									restart = true;
									break;
								}

								min_index = min_index2;
								max_index = max_index2;

								++prim_count;
								p += 2;
							}
							if(restart)
								continue;
							// flush...
							__draw_indexed_16(D3DPT_LINELIST, start, prim_count << 1, prim_count, min_index, max_index);

							break;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_LINE_STRIP:
					{
						const uint32_t* start = indices;
						const uint32_t* end = indices + index_count;

						while(true)
						{
							bool restart = false;

							if(end - start < 2)
								break;
							const uint32_t* p = start;

							uint32_t min_index = (uint32_t)min(p[0], p[1]);
							uint32_t max_index = (uint32_t)max(p[0], p[1]);
							uint32_t prim_count = 1;

							if(max_index - min_index > max_vertex_index)
							{
								// cannot realize this primitive
								start += 1;
								continue;
							}
							for(p += 2; p != end;)
							{
								uint32_t min_index2 = (uint32_t)min(min_index, p[0]);
								uint32_t max_index2 = (uint32_t)max(max_index, p[0]);

								if(max_index2 - min_index2 > max_vertex_index || prim_count >= max_prim)
								{
									// flush...
									__draw_indexed_16(D3DPT_LINESTRIP, start, prim_count + 1, prim_count, min_index, max_index);

									start = p - 1;
									restart = true;
									break;
								}

								min_index = min_index2;
								max_index = max_index2;

								++prim_count;
								++p;
							}
							if(restart)
								continue;
							// flush...
							__draw_indexed_16(D3DPT_LINESTRIP, start, prim_count + 1, prim_count, min_index, max_index);

							break;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_TRIANGLES:
					{
						const uint32_t* start = indices;
						const uint32_t* end = indices + ((index_count / 3) * 3);
						while(true)
						{
							bool restart = false;

							if(start == end)
								break;

							const uint32_t* p = start;

							uint32_t min_index = (uint32_t)min(p[0], min(p[1], p[2]));
							uint32_t max_index = (uint32_t)max(p[0], max(p[1], p[2]));
							uint32_t prim_count = 1;

							if(max_index - min_index > max_vertex_index)
							{
								// cannot realize this primitive
								start += 3;
								continue;
							}
							for(p += 3; p != end;)
							{
								uint32_t min_index2 = (uint32_t)min(min_index, min(p[0], min(p[1], p[2])));
								uint32_t max_index2 = (uint32_t)max(max_index, max(p[0], max(p[1], p[2])));

								if(max_index2 - min_index2 > max_vertex_index || prim_count >= max_prim)
								{
									// flush...
									__draw_indexed_16(D3DPT_TRIANGLELIST, start, prim_count * 3, prim_count, min_index, max_index);

									start = p;
									restart = true;
									break;
								}

								min_index = min_index2;
								max_index = max_index2;

								++prim_count;
								p += 3;
							}
							if(restart)
								continue;
							// flush...
							__draw_indexed_16(D3DPT_TRIANGLELIST, start, prim_count * 3, prim_count, min_index, max_index);

							break;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_TRIANGLE_STRIP:
					{
						const uint32_t* start = indices;
						const uint32_t* end = indices + index_count;

						while(true)
						{
							bool restart = false;

							if(end - start < 4)
								break;
							const uint32_t* p = start;

							uint32_t min_index = (uint32_t)min(p[0], min(p[1], min(p[2], p[3])));
							uint32_t max_index = (uint32_t)max(p[0], max(p[1], max(p[2], p[3])));
							uint32_t prim_count = 1;

							if(max_index - min_index > max_vertex_index)
							{
								// cannot realize these 2 primitives
								start += 4;
								continue;
							}
							const uint32_t* end2 = p + (((end - p) << 1) >> 1);
							for(p += 4; p != end2;)
							{
								uint32_t min_index2 = (uint32_t)min(min_index, min(p[0], p[1]));
								uint32_t max_index2 = (uint32_t)max(max_index, max(p[0], p[1]));

								if(max_index2 - min_index2 > max_vertex_index || prim_count >= max_prim)
								{
									// flush...
									__draw_indexed_16(D3DPT_TRIANGLESTRIP, start, prim_count + 2, prim_count, min_index, max_index);

									start = p - 2;
									restart = true;
									break;
								}

								min_index = min_index2;
								max_index = max_index2;

								++prim_count;
								p += 2;
							}
							if(restart)
								continue;
							// flush...
							__draw_indexed_16(D3DPT_TRIANGLESTRIP, start, prim_count + 2, prim_count, min_index, max_index);

							break;
						}
					}
					break;
				case GfxEnum::PRIMTYPE_TRIANGLE_FAN:
					{
						// unsupported

						Z_ASSERT( false );
					}
					break;
				default:
					{
						Z_ASSERT( false );
					}
					break;
				}

				clearVBOAndVertexAttribBindings();
			}
			else
			{
				uint32_t s = (uint32_t)(index_count << 2);

				bool no_overwrite = true;
				if(!_dibo32.ibo || _dibo32.ibo->length() < s)
				{
					_dibo32.ibo = createIBO(max(s, (uint32_t)8192), true, true);
					_dibo32.used_length = 0;
				}
				else if(_dibo32.ibo->length() - _dibo32.used_length < s)
				{
					_dibo32.used_length = 0;
					no_overwrite = false;
				}

				Z_ASSERT( _dibo32.ibo );

				_dibo32.ibo->lock(_dibo32.used_length, s, no_overwrite);
				memcpy(_dibo32.ibo->pointer(), indices, index_count << 2);
				_dibo32.ibo->unlock();

				UINT start_index = (UINT)(_dibo32.used_length >> 2);

				_dibo32.used_length += s;

				realizeStreamsAndVertDecl(0);

				size_t max_verts = 0x7fffffff;

				if(true)
				{
					VBO_BINDING* end_binding = _array_vbo_binding + _vbo_bindings;
					for(VBO_BINDING* binding = _array_vbo_binding; binding != end_binding; ++binding)
						max_verts = min(max_verts, (binding->vbo->length() - binding->offset) / binding->stride);
				}

				Z_ASSERT( max_verts && max_verts < 0x7fffffff );

				_device->SetIndices(_dibo32.ibo.cast<d3dIBO>()->handle());

				size_t prim_count = 0;
				switch(pt)
				{
				case GfxEnum::PRIMTYPE_POINTS:							prim_count = index_count;				break;
				case GfxEnum::PRIMTYPE_LINES:							prim_count = (index_count >> 1);		break;
				case GfxEnum::PRIMTYPE_LINE_STRIP:						prim_count = (index_count - 1);			break;
				case GfxEnum::PRIMTYPE_TRIANGLES:						prim_count = (index_count / 3);			break;
				case GfxEnum::PRIMTYPE_TRIANGLE_STRIP:					prim_count = (index_count - 2);			break;
				case GfxEnum::PRIMTYPE_TRIANGLE_FAN:					prim_count = (index_count - 2);			break;
				default:												Z_ASSERT( false );						break;
				}

				HRESULT ret = _device->DrawIndexedPrimitive(map_D3DPRIMITIVETYPE(pt), 0, (UINT)0, (UINT)max_verts, start_index, (UINT)prim_count);
				Z_ASSERT( ret == S_OK );

				clearVBOAndVertexAttribBindings();
			}
		}
		void								d3dGfx::__draw_indexed_16					(D3DPRIMITIVETYPE pt, const uint32_t* indices, size_t index_count, size_t prim_count, uint32_t min_index, uint32_t max_index)
		{
			uint32_t s = (uint32_t)(index_count << 1);

			bool no_overwrite = true;
			if(!_dibo16.ibo || _dibo16.ibo->length() < s)
			{
				_dibo16.ibo = createIBO(max(s, (uint32_t)8192), false, true);
				_dibo16.used_length = 0;
			}
			else if(_dibo16.ibo->length() - _dibo16.used_length < s)
			{
				_dibo16.used_length = 0;
				no_overwrite = false;
			}

			Z_ASSERT( _dibo16.ibo );

			_dibo16.ibo->lock(_dibo16.used_length, s, no_overwrite);
			if(min_index == 0)
			{
				uint16_t* idx = _dibo16.ibo->data<uint16_t>();
				const uint32_t* end = indices + index_count;
				for(const uint32_t* p = indices; p != end; ++p, ++idx)
					*idx = (uint16_t)*p;
			}
			else
			{
				uint16_t* idx = _dibo16.ibo->data<uint16_t>();
				const uint32_t* end = indices + index_count;
				for(const uint32_t* p = indices; p != end; ++p, ++idx)
					*idx = (uint16_t)(*p - min_index);
			}
			_dibo16.ibo->unlock();

			UINT start_index = (UINT)(_dibo16.used_length >> 1);

			_dibo16.used_length += s;

			realizeStreamsAndVertDecl(min_index);

			_device->SetIndices(_dibo16.ibo.cast<d3dIBO>()->handle());

			HRESULT ret = _device->DrawIndexedPrimitive(pt, 0, (UINT)0, (UINT)(max_index - min_index + 1), start_index, (UINT)prim_count);
			Z_ASSERT( ret == S_OK );
		}

		D3DSHADEMODE					d3dGfx::map_D3DSHADEMODE			(GfxEnum::SHADEMODE			v)
		{
			switch(v)
			{
			case GfxEnum::SHADEMODE_FLAT:							return D3DSHADE_FLAT;
			case GfxEnum::SHADEMODE_GOURAUD:						return D3DSHADE_GOURAUD;
			default:												Z_ASSERT( false );
			}
			return D3DSHADE_GOURAUD;
		}
		D3DFILLMODE						d3dGfx::map_D3DFILLMODE				(GfxEnum::POLYGONMODE		v)
		{
			switch(v)
			{
			case GfxEnum::POLYGONMODE_POINT:						return D3DFILL_POINT;
			case GfxEnum::POLYGONMODE_LINE:							return D3DFILL_WIREFRAME;
			case GfxEnum::POLYGONMODE_FILL:							return D3DFILL_SOLID;
			default:												Z_ASSERT( false );
			}
			return D3DFILL_SOLID;
		}
		D3DFOGMODE						d3dGfx::map_D3DFOGMODE				(GfxEnum::FOGMODE			v)
		{
			switch(v)
			{
			case GfxEnum::FOGMODE_LINEAR:							return D3DFOG_LINEAR;
			case GfxEnum::FOGMODE_EXP:								return D3DFOG_EXP;
			case GfxEnum::FOGMODE_EXP2:								return D3DFOG_EXP2;
			default:												Z_ASSERT( false );
			}
			return D3DFOG_LINEAR;
		}
		D3DCMPFUNC						d3dGfx::map_D3DCMPFUNC				(GfxEnum::CMPFUNC			v)
		{
			switch(v)
			{
			case GfxEnum::CMPFUNC_NEVER:							return D3DCMP_NEVER;
			case GfxEnum::CMPFUNC_LESS:								return D3DCMP_LESS;
			case GfxEnum::CMPFUNC_EQUAL:							return D3DCMP_EQUAL;
			case GfxEnum::CMPFUNC_LEQUAL:							return D3DCMP_LESSEQUAL;
			case GfxEnum::CMPFUNC_GREATER:							return D3DCMP_GREATER;
			case GfxEnum::CMPFUNC_NOTEQUAL:							return D3DCMP_NOTEQUAL;
			case GfxEnum::CMPFUNC_GEQUAL:							return D3DCMP_GREATEREQUAL;
			case GfxEnum::CMPFUNC_ALWAYS:							return D3DCMP_ALWAYS;
			default:												Z_ASSERT( false );
			}
			return D3DCMP_NEVER;
		}
		D3DPRIMITIVETYPE				d3dGfx::map_D3DPRIMITIVETYPE		(GfxEnum::PRIMTYPE			v)
		{
			switch(v)
			{
			case GfxEnum::PRIMTYPE_POINTS:							return D3DPT_POINTLIST;
			case GfxEnum::PRIMTYPE_LINES:							return D3DPT_LINELIST;
			case GfxEnum::PRIMTYPE_LINE_STRIP:						return D3DPT_LINESTRIP;
			case GfxEnum::PRIMTYPE_TRIANGLES:						return D3DPT_TRIANGLELIST;
			case GfxEnum::PRIMTYPE_TRIANGLE_STRIP:					return D3DPT_TRIANGLESTRIP;
			case GfxEnum::PRIMTYPE_TRIANGLE_FAN:					return D3DPT_TRIANGLEFAN;
			default:												Z_ASSERT( false );
			}
			return D3DPT_POINTLIST;
		}
		D3DTEXTUREFILTERTYPE			d3dGfx::map_D3DTEXTUREFILTERTYPE	(GfxEnum::FILTERMODE		v)
		{
			switch(v)
			{
			case GfxEnum::FILTERMODE_NONE:							return D3DTEXF_NONE;
			case GfxEnum::FILTERMODE_NEAREST:						return D3DTEXF_POINT;
			case GfxEnum::FILTERMODE_LINEAR:						return D3DTEXF_LINEAR;
			case GfxEnum::FILTERMODE_ANISOTROPIC:					return D3DTEXF_ANISOTROPIC;
			default:												Z_ASSERT( false );
			}
			return D3DTEXF_POINT;
		}
		D3DTEXTUREADDRESS				d3dGfx::map_D3DTEXTUREADDRESS		(GfxEnum::WRAPMODE			v)
		{
			switch(v)
			{
			case GfxEnum::WRAPMODE_REPEAT:							return D3DTADDRESS_WRAP;
			case GfxEnum::WRAPMODE_CLAMP:							return D3DTADDRESS_CLAMP;
			case GfxEnum::WRAPMODE_MIRROR:							return D3DTADDRESS_MIRROR;
			case GfxEnum::WRAPMODE_MIRROR_ONCE:						return D3DTADDRESS_MIRRORONCE;
			default:												Z_ASSERT( false );
			}
			return D3DTADDRESS_WRAP;
		}
		D3DBLEND						d3dGfx::map_D3DBLEND				(GfxEnum::BLENDFACTOR		v)
		{
			switch(v)
			{
			case GfxEnum::BLENDFACTOR_ZERO:							return D3DBLEND_ZERO;
			case GfxEnum::BLENDFACTOR_ONE:							return D3DBLEND_ONE;
			case GfxEnum::BLENDFACTOR_SRC_COLOR:					return D3DBLEND_SRCCOLOR;
			case GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_COLOR:			return D3DBLEND_INVSRCCOLOR;
			case GfxEnum::BLENDFACTOR_SRC_ALPHA:					return D3DBLEND_SRCALPHA;
			case GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_ALPHA:			return D3DBLEND_INVSRCALPHA;
			case GfxEnum::BLENDFACTOR_DEST_ALPHA:					return D3DBLEND_DESTALPHA;
			case GfxEnum::BLENDFACTOR_ONE_MINUS_DEST_ALPHA:			return D3DBLEND_INVDESTALPHA;
			case GfxEnum::BLENDFACTOR_DEST_COLOR:					return D3DBLEND_DESTCOLOR;
			case GfxEnum::BLENDFACTOR_ONE_MINUS_DEST_COLOR:			return D3DBLEND_INVDESTCOLOR;
			case GfxEnum::BLENDFACTOR_SRC_ALPHA_SAT:				return D3DBLEND_SRCALPHASAT;
			default:												Z_ASSERT( false );
			}
			return D3DBLEND_ONE;
		}
		D3DBLENDOP						d3dGfx::map_D3DBLENDOP				(GfxEnum::BLENDOPERATION	v)
		{
			switch(v)
			{
			case GfxEnum::BLENDOPERATION_ADD:						return D3DBLENDOP_ADD;
			case GfxEnum::BLENDOPERATION_SUBTRACT:					return D3DBLENDOP_SUBTRACT;
			case GfxEnum::BLENDOPERATION_REVERSE_SUBTRACT:			return D3DBLENDOP_REVSUBTRACT;
			case GfxEnum::BLENDOPERATION_MIN:						return D3DBLENDOP_MIN;
			case GfxEnum::BLENDOPERATION_MAX:						return D3DBLENDOP_MAX;
			default:												Z_ASSERT( false );
			}
			return D3DBLENDOP_ADD;
		}
		D3DSTENCILOP					d3dGfx::map_D3DSTENCILOP			(GfxEnum::STENCILOP			v)
		{
			switch(v)
			{
			case GfxEnum::STENCILOP_KEEP:							return D3DSTENCILOP_KEEP;
			case GfxEnum::STENCILOP_ZERO:							return D3DSTENCILOP_ZERO;
			case GfxEnum::STENCILOP_REPLACE:						return D3DSTENCILOP_REPLACE;
			case GfxEnum::STENCILOP_INCR_SAT:						return D3DSTENCILOP_INCRSAT;
			case GfxEnum::STENCILOP_DECR_SAT:						return D3DSTENCILOP_DECRSAT;
			case GfxEnum::STENCILOP_INVERT:							return D3DSTENCILOP_INVERT;
			case GfxEnum::STENCILOP_INCR:							return D3DSTENCILOP_INCR;
			case GfxEnum::STENCILOP_DECR:							return D3DSTENCILOP_DECR;
			default:												Z_ASSERT( false );
			}
			return D3DSTENCILOP_KEEP;
		}
		D3DCULL							d3dGfx::map_D3DCULL					(GfxEnum::CULLMODE			v)
		{
			switch(v)
			{
			case GfxEnum::CULLMODE_CW:								return D3DCULL_CW;
			case GfxEnum::CULLMODE_CCW:								return D3DCULL_CCW;
			default:												Z_ASSERT( false );
			}
			return D3DCULL_CW;
		}
		D3DLIGHTTYPE					d3dGfx::map_D3DLIGHTTYPE			(GfxEnum::LIGHTTYPE			v)
		{
			switch(v)
			{
			case GfxEnum::LIGHTTYPE_POINT:							return D3DLIGHT_POINT;
			case GfxEnum::LIGHTTYPE_DIRECTIONAL:					return D3DLIGHT_DIRECTIONAL;
			case GfxEnum::LIGHTTYPE_SPOT:							return D3DLIGHT_SPOT;
			default:												Z_ASSERT( false );
			}
			return D3DLIGHT_DIRECTIONAL;
		}
		D3DFORMAT						d3dGfx::map_D3DFORMAT				(GfxEnum::PIXELFORMAT		v)
		{
			switch(v)
			{
			case GfxEnum::PIXELFORMAT_UNKNOWN:						return D3DFMT_UNKNOWN;
			case GfxEnum::PIXELFORMAT_R8G8B8:						return D3DFMT_R8G8B8;
			case GfxEnum::PIXELFORMAT_A8R8G8B8:						return D3DFMT_A8R8G8B8;
			case GfxEnum::PIXELFORMAT_X8R8G8B8:						return D3DFMT_X8R8G8B8;
			case GfxEnum::PIXELFORMAT_A4R4G4B4:						return D3DFMT_A4R4G4B4;
			case GfxEnum::PIXELFORMAT_R5G6B5:						return D3DFMT_R5G6B5;
			case GfxEnum::PIXELFORMAT_A8:							return D3DFMT_A8;
			case GfxEnum::PIXELFORMAT_L8:							return D3DFMT_L8;
			case GfxEnum::PIXELFORMAT_D16:							return D3DFMT_D16;
			case GfxEnum::PIXELFORMAT_D32:							return D3DFMT_D32;
			case GfxEnum::PIXELFORMAT_D24S8:						return D3DFMT_D24S8;
			case GfxEnum::PIXELFORMAT_D24:							return D3DFMT_D24X8;
			case GfxEnum::PIXELFORMAT_R32F:							return D3DFMT_R32F;
			case GfxEnum::PIXELFORMAT_G32R32F:						return D3DFMT_G32R32F;
			case GfxEnum::PIXELFORMAT_G16R16F:						return D3DFMT_G16R16F;
			case GfxEnum::PIXELFORMAT_A16B16G16R16F:				return D3DFMT_A16B16G16R16F;
			default:												Z_ASSERT( false );
			}
			return D3DFMT_UNKNOWN;
		}
		D3DPOOL							d3dGfx::map_D3DPOOL					(GfxEnum::POOL				v)
		{
			switch(v)
			{
			case GfxEnum::POOL_DEFAULT:								return D3DPOOL_DEFAULT;
			case GfxEnum::POOL_MANAGED:								return D3DPOOL_MANAGED;
			case GfxEnum::POOL_SYSTEMMEM:							return D3DPOOL_SYSTEMMEM;
			case GfxEnum::POOL_SCRATCH:								return D3DPOOL_SCRATCH;
			}
			return D3DPOOL_DEFAULT;
		}
		D3DTEXTUREOP					d3dGfx::map_D3DTEXTUREOP			(GfxEnum::TEXTUREOP			v)
		{
			switch(v)
			{
			case GfxEnum::TEXTUREOP_DISABLE:						return D3DTOP_DISABLE;
			case GfxEnum::TEXTUREOP_SELECT_ARG1:					return D3DTOP_SELECTARG1;
			case GfxEnum::TEXTUREOP_SELECT_ARG2:					return D3DTOP_SELECTARG2;
			case GfxEnum::TEXTUREOP_MODULATE:						return D3DTOP_MODULATE;
			case GfxEnum::TEXTUREOP_ADD:							return D3DTOP_ADD;
			case GfxEnum::TEXTUREOP_ADD_SIGNED:						return D3DTOP_ADDSIGNED;
			case GfxEnum::TEXTUREOP_SUBTRACT:						return D3DTOP_SUBTRACT;
			default:												Z_ASSERT( false );
			}
			return D3DTOP_DISABLE;
		}
		DWORD							d3dGfx::map_D3DTA					(GfxEnum::TEXTUREARG		v)
		{
			switch(v)
			{
			case GfxEnum::TEXTUREARG_DIFFUSE:						return D3DTA_DIFFUSE;
			case GfxEnum::TEXTUREARG_TEXTURE:						return D3DTA_TEXTURE;
			case GfxEnum::TEXTUREARG_PREVIOUS:						return D3DTA_CURRENT;
			case GfxEnum::TEXTUREARG_DEFAULT_COLOR:					return D3DTA_TFACTOR;
			default:												Z_ASSERT( false );
			}
			return D3DTA_CURRENT;
		}
		D3DCUBEMAP_FACES				d3dGfx::map_D3DCUBEMAP_FACES		(GfxEnum::CUBEMAP_FACE		v)
		{
			switch(v)
			{
			case GfxEnum::CUBEMAP_FACE_POSITIVE_X:					return D3DCUBEMAP_FACE_POSITIVE_X;
			case GfxEnum::CUBEMAP_FACE_NEGATIVE_X:					return D3DCUBEMAP_FACE_NEGATIVE_X;
			case GfxEnum::CUBEMAP_FACE_POSITIVE_Y:					return D3DCUBEMAP_FACE_POSITIVE_Y;
			case GfxEnum::CUBEMAP_FACE_NEGATIVE_Y:					return D3DCUBEMAP_FACE_NEGATIVE_Y;
			case GfxEnum::CUBEMAP_FACE_POSITIVE_Z:					return D3DCUBEMAP_FACE_POSITIVE_Z;
			case GfxEnum::CUBEMAP_FACE_NEGATIVE_Z:					return D3DCUBEMAP_FACE_NEGATIVE_Z;
			default:												Z_ASSERT( false );
			}
			return D3DCUBEMAP_FACE_POSITIVE_X;
		}
	};
};

#endif

