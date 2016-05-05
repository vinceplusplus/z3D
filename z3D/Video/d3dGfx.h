
#pragma once

#include "predecl.h"
#include "GfxEnum.h"
#include "Gfx.h"

#include "rsView.h"
#include "rsShade.h"

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include "d3dShaderGen.h"

#include <d3d9.h>
#include <d3d9types.h>

typedef interface ID3DXConstantTable ID3DXConstantTable;
typedef interface ID3DXEffect ID3DXEffect;

namespace z3D
{
	namespace Video
	{
		class d3dGfx;
		class d3dVBO;

		// responsible for setting, getting, shadowing render state
		template<D3DRENDERSTATETYPE state, DWORD default_value>
		class d3dRS
		{
		private:
			DWORD					_value;
		public:
			void					init(IDirect3DDevice9* device);
			void					set(IDirect3DDevice9* device, DWORD value);
		};

		class d3dTextureState
		{
		private:
			IDirect3DBaseTexture9*	_value;
		public:
			void					init(IDirect3DDevice9* device, DWORD stage);
			void					set(IDirect3DDevice9* device, DWORD stage, IDirect3DBaseTexture9* value);
			IDirect3DBaseTexture9*	get() const;
		};

		class d3dTSS
		{
		public:
			template<D3DTEXTURESTAGESTATETYPE type, DWORD default_value>
			class TSSS
			{
			private:
				DWORD					_value;
			public:
				void					init(IDirect3DDevice9* device, DWORD stage);
				void					set(IDirect3DDevice9* device, DWORD stage, DWORD value);
			};

		public:
			TSSS<D3DTSS_COLOROP, D3DTOP_DISABLE>				color_op;
			TSSS<D3DTSS_COLORARG1, D3DTA_TEXTURE>				color_arg1;
			TSSS<D3DTSS_COLORARG2, D3DTA_CURRENT>				color_arg2;
			TSSS<D3DTSS_ALPHAOP, D3DTOP_DISABLE>				alpha_op;
			TSSS<D3DTSS_ALPHAARG1, D3DTA_TEXTURE>				alpha_arg1;
			TSSS<D3DTSS_ALPHAARG2, D3DTA_CURRENT>				alpha_arg2;
		public:
			void					init(IDirect3DDevice9* device, DWORD stage);
		};

		class d3dTCT
		{
		private:
			REAL					_value[4][4];
		public:
			void					init(IDirect3DDevice9* device, DWORD stage);
			void					set(IDirect3DDevice9* device, DWORD stage, const REAL value[4][4]);
		};

		class d3dSS
		{
		public:
			template<D3DSAMPLERSTATETYPE type, DWORD default_value>
			class SSS
			{
			private:
				DWORD					_value;
			public:
				void					init(IDirect3DDevice9* device, DWORD stage);
				void					set(IDirect3DDevice9* device, DWORD stage, DWORD value);
			};

		public:
			SSS<D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP>				wrap_u;
			SSS<D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP>				wrap_v;
			SSS<D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP>				wrap_w;
			SSS<D3DSAMP_MINFILTER, D3DTEXF_POINT>				filter_min;
			SSS<D3DSAMP_MAGFILTER, D3DTEXF_POINT>				filter_mag;
			SSS<D3DSAMP_MIPFILTER, D3DTEXF_NONE>					filter_mip;
		public:
			void					init(IDirect3DDevice9* device, DWORD stage);
		};

		class d3dLightState
		{
		private:
			D3DLIGHT9				_value;
		public:
			void				init(IDirect3DDevice9* device, DWORD index);
			void				set(IDirect3DDevice9* device, DWORD index, const D3DLIGHT9& value);
		};

		class d3dLightEnableState
		{
		private:
			BOOL					_value;
		public:
			void				init(IDirect3DDevice9* device, DWORD index);
			void				set(IDirect3DDevice9* device, DWORD index, BOOL value);
		};

		class d3dClipPlaneState
		{
		private:
			float					_value[4];
		public:
			void				init(IDirect3DDevice9* device, DWORD index);
			void				set(IDirect3DDevice9* device, DWORD index, const float value[4]);
		};

		class d3dScissorState
		{
		private:
			RECT					_value;
		public:
			void				init(IDirect3DDevice9* device);
			void				set(IDirect3DDevice9* device, const RECT& value);
		};

		class d3dViewportState
		{
		private:
			D3DVIEWPORT9			_value;
		public:
			void				init(IDirect3DDevice9* device);
			void				set(IDirect3DDevice9* device, const D3DVIEWPORT9& value);
		};

		class d3dMaterialState
		{
		private:
			D3DMATERIAL9			_value;
		public:
			void				init(IDirect3DDevice9* device);
			void				set(IDirect3DDevice9* device, const D3DMATERIAL9& value);
		};

		class d3dVertexDeclarationState
		{
		private:
			IDirect3DVertexDeclaration9*	_value;
		public:
			void				init(IDirect3DDevice9* device);
			void				set(IDirect3DDevice9* device, IDirect3DVertexDeclaration9* value);
		};

		class d3dVertexShaderState
		{
		private:
			IDirect3DVertexShader9*			_value;
		public:
			void							init(IDirect3DDevice9* device);
			void							set(IDirect3DDevice9* device, IDirect3DVertexShader9* value);
			IDirect3DVertexShader9*			get() const;
		};

		class d3dPixelShaderState
		{
		private:
			IDirect3DPixelShader9*			_value;
		public:
			void							init(IDirect3DDevice9* device);
			void							set(IDirect3DDevice9* device, IDirect3DPixelShader9* value);
			IDirect3DPixelShader9*			get() const;
		};

		class id3dRecreatable;

		Z_DEFINE_FIXED_BLOCK_ALLOCATOR(d3d_recreate_list_fba, 12, 4);
		typedef list<id3dRecreatable*, d3d_recreate_list_fba<id3dRecreatable>> D3D_RECREATABLE_LIST_TYPE;

		class id3dRecreatable
		{
		public:
			class LIST_TRACKING
			{
			private:
				D3D_RECREATABLE_LIST_TYPE*
									_list;
				D3D_RECREATABLE_LIST_TYPE::iterator
									_iter;
			public:
				LIST_TRACKING();
				~LIST_TRACKING();
			public:
				friend class d3dGfx;
			}						list_tracking;
		protected:
			~id3dRecreatable();
		protected:
			virtual void			prepare_recreate() = 0;
			virtual bool			recreate() = 0;
		public:
			virtual wcs				short_desc() const = 0;
		public:
			friend d3dGfx;
		};

		class d3dGfx: public Gfx
		{
		public:
			DECL_TYPEINFO();
		private:
			enum
			{
				MAX_SAMPLERS = 16,
				MAX_STAGES = 8,
				MAX_LIGHTS = 8,
				MAX_CLIP_PLANES = 6,
				MAX_VBO_BINDINGS = 16,
				MAX_VERTEX_ATTRIB_BINDINGS = 16,
			};
		public:
			class VERTEXATTRIB
			{
			public:
				d3dVBO*				vbo;						// use raw pointer for maximum optimization
				BYTE				stride;
				BYTE				Type;
				BYTE				Usage;
				BYTE				UsageIndex;
			};
		public:
			typedef enum _VERTEX_ATTRIB_SEMANTIC: uint8_t
			{
				VERTEX_ATTRIB_SEMANTIC_POSITION = 0,
				VERTEX_ATTRIB_SEMANTIC_NORMAL,
				VERTEX_ATTRIB_SEMANTIC_COLOR,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD0,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD1,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD2,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD3,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD4,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD5,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD6,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD7,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD8,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD9,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD10,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD11,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD12,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD13,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD14,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD15,
				VERTEX_ATTRIB_SEMANTIC_TEXCOORD_BOUND,



				VERTEX_ATTRIB_SEMANTIC_NUM_TEXCOORD = VERTEX_ATTRIB_SEMANTIC_TEXCOORD_BOUND - VERTEX_ATTRIB_SEMANTIC_TEXCOORD0,

			} VERTEX_ATTRIB_SEMANTIC;

			typedef struct _VBO_BINDING
			{
				d3dVBO*						vbo;
				size_t						offset;
				size_t						stride;

			} VBO_BINDING;

			typedef struct _VERTEX_ATTRIB_BINDING
			{
				uint8_t						stream;
				VERTEX_ATTRIB_SEMANTIC		semantic : 8;
				uint8_t						components;					// actual components - 1
				uint8_t						offset;

			} VERTEX_ATTRIB_BINDING;

			typedef struct _ALL_VERTEX_ATTRIB_BINDING
			{
				VERTEX_ATTRIB_BINDING	b [ MAX_VERTEX_ATTRIB_BINDINGS ];
			} ALL_VERTEX_ATTRIB_BINDING;
		private:
			HWND						_hwnd;
			HWND						_hwnd_present;
			IDirect3D9*					_d3d;
			IDirect3DDevice9*			_device;
			D3DPRESENT_PARAMETERS		_pp;
			D3DCAPS9					_caps;
			IDirect3DSurface9*			_back_buffer_color;
			IDirect3DSurface9*			_back_buffer_depthstencil;
			size_t						_multisamples;
			IDirect3DQuery9*			_query_flush;
		private:
			size_t						_framebuffer_dx;
			size_t						_framebuffer_dy;
			size_t						_bound_rendertarget;
			size_t						_bound_depthstencil;
		protected:
			aligned_storage<1056, 8>	_recreate_list_mp_storage;
			nt_mem_pool_keeper<d3d_recreate_list_fba<char>::block_size, d3d_recreate_list_fba<char>::block_alignment, false>
										_recreate_list_mp;
			D3D_RECREATABLE_LIST_TYPE	_list_recreatable;
		protected:
			d3dRS<D3DRS_ZENABLE, D3DZB_TRUE>								_rs_z_enable;
			d3dRS<D3DRS_FILLMODE, D3DFILL_SOLID>							_rs_fill_mode;
			d3dRS<D3DRS_SHADEMODE, D3DSHADE_GOURAUD>						_rs_shade_mode;
			d3dRS<D3DRS_ZWRITEENABLE, TRUE>									_rs_z_write_enable;
			d3dRS<D3DRS_ALPHATESTENABLE, FALSE>								_rs_alpha_test_enable;
			d3dRS<D3DRS_LASTPIXEL, TRUE>									_rs_last_pixel;
			d3dRS<D3DRS_SRCBLEND, D3DBLEND_ONE>								_rs_src_blend;
			d3dRS<D3DRS_DESTBLEND, D3DBLEND_ZERO>							_rs_dest_blend;
			d3dRS<D3DRS_CULLMODE, D3DCULL_CW>								_rs_cull_mode;
			d3dRS<D3DRS_ZFUNC, D3DCMP_LESSEQUAL>							_rs_z_func;
			d3dRS<D3DRS_ALPHAREF, 0>										_rs_alpha_ref;
			d3dRS<D3DRS_ALPHAFUNC, D3DCMP_ALWAYS>							_rs_alpha_func;
			d3dRS<D3DRS_DITHERENABLE, FALSE>								_rs_dither_enable;
			d3dRS<D3DRS_ALPHABLENDENABLE, FALSE>							_rs_alpha_blend_enable;
			d3dRS<D3DRS_FOGENABLE, FALSE>									_rs_fog_enable;
			d3dRS<D3DRS_SPECULARENABLE, FALSE>								_rs_specular_enable;
			d3dRS<D3DRS_FOGCOLOR, 0>										_rs_fog_color;
			d3dRS<D3DRS_FOGTABLEMODE, D3DFOG_NONE>							_rs_fog_table_mode;
			d3dRS<D3DRS_FOGSTART, 0>										_rs_fog_start;
			d3dRS<D3DRS_FOGEND, 1065353216UL>								_rs_fog_end;
			d3dRS<D3DRS_FOGDENSITY, 1065353216UL>							_rs_fog_density;
			d3dRS<D3DRS_RANGEFOGENABLE, FALSE>								_rs_range_for_enable;
			d3dRS<D3DRS_STENCILENABLE, FALSE>								_rs_stencil_enable;
			d3dRS<D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP>						_rs_stencil_fail;
			d3dRS<D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP>					_rs_stencil_zfail;
			d3dRS<D3DRS_STENCILPASS, D3DSTENCILOP_KEEP>						_rs_stencil_pass;
			d3dRS<D3DRS_STENCILFUNC, D3DCMP_ALWAYS>							_rs_stencil_func;
			d3dRS<D3DRS_STENCILREF, 0>										_rs_stencil_ref;
			d3dRS<D3DRS_STENCILMASK, 0xFFFFFFFFUL>							_rs_stencil_mask;
			d3dRS<D3DRS_STENCILWRITEMASK, 0xFFFFFFFFUL>						_rs_stencil_write_mask;
			d3dRS<D3DRS_TEXTUREFACTOR, 0xFFFFFFFFUL>						_rs_texture_factor;
			d3dRS<D3DRS_WRAP0, 0>											_rs_wrap0;
			d3dRS<D3DRS_WRAP1, 0>											_rs_wrap1;
			d3dRS<D3DRS_WRAP2, 0>											_rs_wrap2;
			d3dRS<D3DRS_WRAP3, 0>											_rs_wrap3;
			d3dRS<D3DRS_WRAP4, 0>											_rs_wrap4;
			d3dRS<D3DRS_WRAP5, 0>											_rs_wrap5;
			d3dRS<D3DRS_WRAP6, 0>											_rs_wrap6;
			d3dRS<D3DRS_WRAP7, 0>											_rs_wrap7;
			d3dRS<D3DRS_CLIPPING, TRUE>										_rs_clipping;
			d3dRS<D3DRS_LIGHTING, FALSE>									_rs_lighting;
			d3dRS<D3DRS_AMBIENT, 0>											_rs_ambient;
			d3dRS<D3DRS_FOGVERTEXMODE, D3DFOG_NONE>							_rs_fog_vertex_mode;
			d3dRS<D3DRS_COLORVERTEX, TRUE>									_rs_color_vertex;
			d3dRS<D3DRS_LOCALVIEWER, TRUE>									_rs_local_viewer;
			d3dRS<D3DRS_NORMALIZENORMALS,FALSE >							_rs_normalize_normals;
			d3dRS<D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1>				_rs_diffuse_material_source;
			d3dRS<D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2>				_rs_specular_material_source;
			d3dRS<D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL>				_rs_ambient_material_source;
			d3dRS<D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL>			_rs_emissive_material_source;
			d3dRS<D3DRS_VERTEXBLEND, D3DVBF_DISABLE>						_rs_vertex_blend;
			d3dRS<D3DRS_CLIPPLANEENABLE, 0>									_rs_clip_plane_enable;
			d3dRS<D3DRS_POINTSIZE, 1065353216UL>							_rs_point_size;
			d3dRS<D3DRS_POINTSIZE_MIN, 1065353216UL>						_rs_point_size_min;
			d3dRS<D3DRS_POINTSPRITEENABLE, FALSE>							_rs_point_sprite_enable;
			d3dRS<D3DRS_POINTSCALEENABLE, 1065353216UL>						_rs_point_scale_enable;
			d3dRS<D3DRS_POINTSCALE_A, 1065353216UL>							_rs_point_scale_a;
			d3dRS<D3DRS_POINTSCALE_B, 0>									_rs_point_scale_b;
			d3dRS<D3DRS_POINTSCALE_C, 0>									_rs_point_scale_c;
			d3dRS<D3DRS_MULTISAMPLEANTIALIAS, TRUE>							_rs_multisample_antialias;
			d3dRS<D3DRS_MULTISAMPLEMASK, 0xFFFFFFFFUL>						_rs_multisample_mask;
			d3dRS<D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE>				_rs_patch_monitor_token;
			d3dRS<D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE>					_rs_debug_monitor_token;
			d3dRS<D3DRS_POINTSIZE_MAX, 1115684864UL>						_rs_point_size_max;
			d3dRS<D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE>					_rs_indexed_vertex_blend_enable;
			d3dRS<D3DRS_COLORWRITEENABLE, 0x0000000f>						_rs_color_write_enable;
			d3dRS<D3DRS_TWEENFACTOR, 0>										_rs_tween_factor;
			d3dRS<D3DRS_BLENDOP, D3DBLENDOP_ADD>							_rs_blend_op;
			d3dRS<D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC>					_rs_position_degree;
			d3dRS<D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR>						_rs_normal_degree;
			d3dRS<D3DRS_SCISSORTESTENABLE, FALSE>							_rs_scissor_test_enable;
			d3dRS<D3DRS_SLOPESCALEDEPTHBIAS, 0>								_rs_slope_scale_depth_bias;
			d3dRS<D3DRS_ANTIALIASEDLINEENABLE, FALSE>						_rs_antialiased_line_enable;
			d3dRS<D3DRS_MINTESSELLATIONLEVEL, 1065353216UL>					_rs_min_tessellation_level;
			d3dRS<D3DRS_MAXTESSELLATIONLEVEL, 1065353216UL>					_rs_max_tessellation_level;
			d3dRS<D3DRS_ADAPTIVETESS_X, 0>									_rs_adaptive_tess_x;
			d3dRS<D3DRS_ADAPTIVETESS_Y, 0>									_rs_adaptive_tess_y;
			d3dRS<D3DRS_ADAPTIVETESS_Z, 1065353216UL>						_rs_adaptive_tess_z;
			d3dRS<D3DRS_ADAPTIVETESS_W, 0>									_rs_adaptive_tess_w;
			d3dRS<D3DRS_ENABLEADAPTIVETESSELLATION, FALSE>					_rs_enable_adaptive_tessellation;
			d3dRS<D3DRS_TWOSIDEDSTENCILMODE, FALSE>							_rs_two_side_stencil_mode;
			d3dRS<D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP>					_rs_ccw_stencil_fail;
			d3dRS<D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP>				_rs_ccw_stencil_zfail;
			d3dRS<D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP>					_rs_ccw_stencil_pass;
			d3dRS<D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS>						_rs_ccw_stencil_func;
			d3dRS<D3DRS_COLORWRITEENABLE1, 0>								_rs_color_write_enable1;
			d3dRS<D3DRS_COLORWRITEENABLE2, 0>								_rs_color_write_enable2;
			d3dRS<D3DRS_COLORWRITEENABLE3, 0>								_rs_color_write_enable3;
			d3dRS<D3DRS_BLENDFACTOR, 0xFFFFFFFFUL>							_rs_blend_factor;
			d3dRS<D3DRS_SRGBWRITEENABLE, 0>									_rs_srgb_write_enable;
			d3dRS<D3DRS_DEPTHBIAS, 0>										_rs_depth_bias;
			d3dRS<D3DRS_WRAP8, 0>											_rs_wrap8;
			d3dRS<D3DRS_WRAP9, 0>											_rs_wrap9;
			d3dRS<D3DRS_WRAP10, 0>											_rs_wrap10;
			d3dRS<D3DRS_WRAP11, 0>											_rs_wrap11;
			d3dRS<D3DRS_WRAP12, 0>											_rs_wrap12;
			d3dRS<D3DRS_WRAP13, 0>											_rs_wrap13;
			d3dRS<D3DRS_WRAP14, 0>											_rs_wrap14;
			d3dRS<D3DRS_WRAP15, 0>											_rs_wrap15;
			d3dRS<D3DRS_SEPARATEALPHABLENDENABLE, FALSE>					_rs_separate_alpha_blend_enable;
			d3dRS<D3DRS_SRCBLENDALPHA, D3DBLEND_ONE>						_rs_src_blend_alpha;
			d3dRS<D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO>						_rs_dest_blend_alpha;
			d3dRS<D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD>						_rs_blend_op_alpha;
		protected:
			d3dTextureState						_rs_ts [ MAX_STAGES ];
			d3dTSS								_rs_tss [ MAX_STAGES ];
			d3dTCT								_rs_tct [ MAX_STAGES ];
			d3dSS								_rs_ss [ MAX_SAMPLERS ];
			d3dLightState						_rs_ls [ MAX_LIGHTS ];
			d3dLightEnableState					_rs_les [ MAX_LIGHTS ];
			d3dClipPlaneState					_rs_cps [ MAX_CLIP_PLANES ];
			d3dScissorState						_rs_scissor;
			d3dViewportState					_rs_vps;
			d3dMaterialState					_rs_ms;
			d3dVertexDeclarationState			_rs_vds;
			d3dVertexShaderState				_rs_vss;
			d3dPixelShaderState					_rs_pss;
		protected:
			ID3DXConstantTable*					_ct_vs;
			ID3DXConstantTable*					_ct_ps;
		private:
			VBO_BINDING							_array_vbo_binding [ MAX_VBO_BINDINGS ];
			size_t								_vbo_bindings;
		private:
			VERTEX_ATTRIB_BINDING				_array_vertex_attrib_binding [ MAX_VERTEX_ATTRIB_BINDINGS ];
			size_t								_vertex_attrib_bindings;
		private:
			struct ALL_VERTEX_ATTRIB_BINDING_less : public std::binary_function <ALL_VERTEX_ATTRIB_BINDING, ALL_VERTEX_ATTRIB_BINDING, bool> 
			{bool operator()(const ALL_VERTEX_ATTRIB_BINDING& left, const ALL_VERTEX_ATTRIB_BINDING& right) const;};

			Z_DEFINE_FIXED_BLOCK_ALLOCATOR(vertex_decl_map_fba, 84, 4);

			aligned_storage<11296, 8>			_vertex_decl_map_mp_storage;
			nt_mem_pool_keeper<vertex_decl_map_fba<char>::block_size, vertex_decl_map_fba<char>::block_alignment, false>
												_vertex_decl_map_mp;

			typedef map<ALL_VERTEX_ATTRIB_BINDING,
				IDirect3DVertexDeclaration9*,
				ALL_VERTEX_ATTRIB_BINDING_less,
				vertex_decl_map_fba<pair<const ALL_VERTEX_ATTRIB_BINDING, IDirect3DVertexDeclaration9*>>>
												VERTEX_DECL_MAP_TYPE;

				VERTEX_DECL_MAP_TYPE			_map_d3d_vertex_decl;
		private:
			size_t								_last_used_vbos;
		private:
			GfxEnum::RENDER_SETUP				_cur_render_setup;
		private:
			rsView								_view_state;
			size_t								_empty_viewport;
			Mat4								_proj_adjust;
		private:
			Mat4								_view_transform;
			Mat4								_proj_transform;
		private:
			//rsShade								_shade_state;
		private:
			Mat4								_world_transform;
		private:
			mutable Mat4						_adjusted_proj_transform;
			mutable Mat4						_view_proj_transform;
			mutable Mat4						_adjusted_view_proj_transform;
			mutable Mat4						_world_view_proj_transform;
			mutable Mat4						_adjusted_world_view_proj_transform;
			mutable Mat4						_inv_view_transform;

			mutable struct XFORM_DIRTIES
			{
				union
				{
					size_t								FLAGS;
					struct
					{
						uint8_t								adjusted_proj_transform : 1;
						uint8_t								view_proj_transform : 1;
						uint8_t								adjusted_view_proj_transform : 1;
						uint8_t								world_view_proj_transform : 1;
						uint8_t								adjusted_world_view_proj_transform : 1;
						uint8_t								inv_view_transform : 1;
					};
				};
			}									_xform_dirties;
		private:
			struct _JOTTER
			{
				SPtr<VBO>							vbo;
				size_t								used_length;
			}									_jotter;

			struct _DIBO16							// dynamic 16-bit ibo
			{
				SPtr<IBO>							ibo;
				size_t								used_length;
			}									_dibo16;

			struct _DIBO32							// dynamic 32-bit ibo
			{
				SPtr<IBO>							ibo;
				size_t								used_length;
			}									_dibo32;
		private:
			ResMgr								_res_mgr_vs;
			ResMgr								_res_mgr_ps;
			ResMgr								_res_mgr_tex2d;
			ResMgr								_res_mgr_texcube;
			ResMgr								_res_mgr_tex2d_avi_cf;
			ResMgr								_res_mgr_tex2d_gif_cf;
		private:
			d3dShaderGen						_shader_gen;
		private:
			SPtr<Shader>						_vs_texfont;
			SPtr<Shader>						_ps_texfont;

			SPtr<Shader>						_vs_texfont_use_decal;
			SPtr<Shader>						_ps_texfont_use_decal;

			SPtr<Shader>						_vs_img2d;
			SPtr<Shader>						_ps_img2d;

			SPtr<Shader>						_vs_pass_thru;
			SPtr<Shader>						_ps_pass_thru;

			SPtr<Shader>						_vs_vsm_screen_depth;
			SPtr<Shader>						_ps_vsm_screen_depth;

			SPtr<Shader>						_vs_vsm_screen_depth_alpha;
			SPtr<Shader>						_ps_vsm_screen_depth_alpha;

			SPtr<Shader>						_vs_vsm_gen_shadowmap;
			SPtr<Shader>						_ps_vsm_gen_shadowmap;

			SPtr<Shader>						_vs_vsm_gen_shadowmap_alpha;
			SPtr<Shader>						_ps_vsm_gen_shadowmap_alpha;

			SPtr<Shader>						_vs_vsm_mark_shadow;
			SPtr<Shader>						_ps_vsm_mark_shadow;

			SPtr<Shader>						_vs_vsm_mark_shadow2[4];
			SPtr<Shader>						_ps_vsm_mark_shadow2[4];

			SPtr<Shader>						_vs_gaussian_blur_5;
			SPtr<Shader>						_ps_gaussian_blur_5;

			SPtr<Shader>						_vs_gaussian_blur_3;
			SPtr<Shader>						_ps_gaussian_blur_3;

		private:
			struct
			{
				IDirect3DSurface9*					rts[32][4];				// render target
				IDirect3DSurface9*					dss[32];				// depth stencil

				size_t								size;
			}									_rt_n_ds_stack;
		private:
			size_t								_polycount;
		private:
			char								_vs_profile[16];
			char								_ps_profile[16];
		private:
			bool								_supports_sm_2;
			bool								_supports_sm_3;
		protected:
			d3dGfx();
			~d3dGfx();
		public:
			IDirect3D9*				d3d() const;
			IDirect3DDevice9*		device() const;
			const D3DCAPS9&			caps() const;
		public:
			static SPtr<d3dGfx>		create(HWND hwnd, bool vsync);
		public:
			size_t					getWindowDX() const;
			size_t					getWindowDY() const;
			size_t					getFramebufferDX() const;
			size_t					getFramebufferDY() const;
		public:
			void					clear(uint32_t x, uint32_t y, uint32_t dx, uint32_t dy, bool clear_color, bool clear_depth, bool clear_stencil, const Vec4& color, REAL depth, uint32_t stencil);
			void					clear(bool clear_color, bool clear_depth, bool clear_stencil, const Vec4& color, REAL depth, uint32_t stencil);
		public:
			void					beginScene();
			void					endScene();
		public:
			void					flush();								// as it appears that it always stutters because of switching render target, the flush will hurt the performance, but its needed only under windowed mode, and as expected, windowed mode is slightly less performant than full screen mode, http://it.gps678.com/9/cc3a7810d13059a7.html
		public:
			void					present();
		public:
			void					present(intptr_t hwnd, uint32_t src_x, uint32_t src_y, uint32_t src_dx, uint32_t src_dy, uint32_t dst_x, uint32_t dst_y, uint32_t dst_dx, uint32_t dst_dy);
		public:
			bool					copyToTexture(const SPtr<Tex2D>& tex, uint32_t src_x, uint32_t src_y, uint32_t src_dx, uint32_t src_dy, uint32_t dst_x, uint32_t dst_y, uint32_t dst_dx, uint32_t dst_dy);
		public:
			void					notifyDisplaySettingsChanged();
		private:
			void					handleDeviceLost();
		public:
			void					removeVBORef(IDirect3DVertexBuffer9* ref);
			void					removeIBORef(IDirect3DIndexBuffer9* ref);
			void					removeTexRef(IDirect3DBaseTexture9* ref);
			void					removeVSRef(IDirect3DVertexShader9* ref);
			void					removePSRef(IDirect3DPixelShader9* ref);
		public:
			void					enumMultiSampleTypeSupport(GfxEnum::PIXELFORMAT format, bool support[16]);
		public:
			IDirect3DTexture9*		_createTex2D(const Tex2DCreateParams& cp);
		public:
			SPtr<VBO>				createVBO(size_t length, bool dynamic);
			SPtr<IBO>				createIBO(size_t length, bool _32bit, bool dynamic);
			SPtr<Tex2D>				createTex2D(size_t dx, size_t dy, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
			SPtr<Tex2D>				createTex2D(const Tex2DCreateParams& cp);
			SPtr<TexCube>			createTexCube(size_t length, size_t levels, GfxEnum::PIXELFORMAT format, bool rendertarget);
			SPtr<Shader>			createShaderFromSource(GfxEnum::SHADERTYPE type, const mbs& source);
			SPtr<Shader>			createShaderFromFile(GfxEnum::SHADERTYPE type, const wcs& filepath);
			SPtr<Renderbuffer>		createRenderTargetRenderbuffer(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
			SPtr<Renderbuffer>		createDepthStencilRenderbuffer(size_t dx, size_t dy, GfxEnum::PIXELFORMAT format, size_t multisample_type);
			ShaderGen*				getShaderGen();
		public:
			void					getTexFontShaders(bool use_decal, SPtr<Shader>& vs, SPtr<Shader>& ps);
			void					getImg2DShaders(SPtr<Shader>& vs, SPtr<Shader>& ps);
			void					getPassThruShaders(SPtr<Shader>& vs, SPtr<Shader>& ps);
			void					getVSMScreenDepth(bool alpha, SPtr<Shader>& vs, SPtr<Shader>& ps);
			void					getVSMGenShadowMap(bool alpha, SPtr<Shader>& vs, SPtr<Shader>& ps);
			void					getVSMMarkShadow(size_t light_count, SPtr<Shader>& vs, SPtr<Shader>& ps);
			void					getGaussianBlur5(SPtr<Shader>& vs, SPtr<Shader>& ps);
			void					getGaussianBlur3(SPtr<Shader>& vs, SPtr<Shader>& ps);
		public:
			ResMgr&					getResMgrVS();
			ResMgr&					getResMgrPS();
			ResMgr&					getResMgrTex2D();
			ResMgr&					getResMgrTexCube();
			ResMgr&					getResMgrTex2DAVICF();
			ResMgr&					getResMgrTex2DGIFCF();
		public:
			bool					bindRenderTarget(size_t index, const SPtr<Tex2D>& tex2d);
			bool					bindRenderTarget(size_t index, const SPtr<TexCube>& texcube, GfxEnum::CUBEMAP_FACE face);
			bool					bindRenderTarget(size_t index, const SPtr<Renderbuffer>& renderbuffer);
			bool					bindDefaultRenderTarget();
			bool					unbindRenderTarget(size_t index);
			void					unbindAllRenderTargets();
		public:
			bool					bindDepthStencil(const SPtr<Tex2D>& tex2d);
			bool					bindDepthStencil(const SPtr<Renderbuffer>& renderbuffer);
			bool					bindDefaultDepthStencil();
			bool					unbindDepthStencil();
		public:
			bool					boundRenderTarget() const;
			bool					boundDepthStencil() const;
		private:
			bool					bindRenderTarget(size_t index, IDirect3DSurface9* surface);
			bool					bindDepthStencil(IDirect3DSurface9* surface);
			void					postBindRenderTarget(size_t index, IDirect3DSurface9* surface);
		public:
			void					push_RT_n_DS();
			void					pop_RT_n_DS();
		public:
			static D3DCOLOR			toD3DCOLOR						(const Vec4& v);
			static D3DCOLORVALUE	toD3DCOLORVALUE					(const Vec4& v);
		public:
			bool					supportsNonPower2Tex() const;
			bool					supportsShaderModel2() const;
			bool					supportsShaderModel3() const;
			const char*				getVSProfile() const;
			const char*				getPSProfile() const;
			size_t					maxTextures() const;
			size_t					maxTextureStages() const;
			size_t					maxSamplers() const;
			size_t					maxLights() const;
			size_t					maxUserClipPlanes() const;
			size_t					maxTextureSize() const;
		public:
			REAL					getZNearMapping() const;
			REAL					getZFarMapping() const;
			REAL					getWMultiplier() const;
		private:
			void					__init();
		public:
			void					addRecreatable(id3dRecreatable* recreatable);
		public:
			HWND					getDefaultPresentTarget() const;
			void					setPresentTarget(HWND hwnd);
			HWND					getPresentTarget() const;
		public:
			size_t					getAccumPolycount() const;
			void					resetAccumPolycount();
		public:
			void								setViewState						(const rsView& rs);
			const rsView&						getViewState						() const;
			bool								isEmptyRenderRect					() const;
		public:
			void								setViewProjTransform				(const Mat4& view, const Mat4& proj);
			void								setWorldViewProjTransform			(const Mat4& world, const Mat4& view, const Mat4& proj);
			void								setViewProjTransform2D				(const Mat4& world);
			Mat4								adjustProjectionTransform			(const Mat4& proj) const;
			const Mat4&							getViewTransform					() const;
			const Mat4&							getUnadjustedProjectionTransform	() const;
			const Mat4&							getAdjustedProjectionTransform		() const;
			const Mat4&							getUnadjustedViewProjectionTransform() const;
			const Mat4&							getAdjustedViewProjectionTransform	() const;
			const Mat4&							getInverseViewTransform				() const;
		public:
			void								useFixedPipeline					();
			void								useProgrammablePipeline				(const SPtr<Shader>& vs, const SPtr<Shader>& ps);
			//void								useShader							(const SPtr<Shader>& shader);
		public:
			void								setWorldTransform					(const Mat4& world);
			const Mat4&							getWorldTransform					() const;
			const Mat4&							getUnadjustedWorldViewProjectionTransform() const;
			const Mat4&							getAdjustedWorldViewProjectionTransform() const;
		public:
			void								setShadeState						(const rsShade& rs);
			//const rsShade&						getShadeState						() const;
		public:
			void								bindTexture							(size_t stage, const SPtr<TexBase>& tex);
		public:
			void								bindVBO								(const SPtr<VBO>& vbo, uint8_t stride, size_t offset);
			void								bindVertexPosition					(uint8_t offset, uint8_t comps);
			void								bindVertexNormal					(uint8_t offset);
			void								bindVertexColor						(uint8_t offset);
			void								bindVertexTexCoord					(uint8_t offset, uint8_t index, uint8_t comps);
		private:
			void								bindVertexAttrib					(uint8_t offset, VERTEX_ATTRIB_SEMANTIC semantic, uint8_t comps);
		public:
			void								setShaderUniforms					(const ShaderUniforms& s);
			void								setShaderUniforms					(const ShaderUniforms2& s);
		private:
			IDirect3DVertexDeclaration9*		realizeD3DVertexDecl				(const ALL_VERTEX_ATTRIB_BINDING& binding);
			void								clearD3DVertexDecls					();
		private:
			bool								realizeStreamsAndVertDecl			(size_t numVertsOffset);
			void								clearVBOAndVertexAttribBindings		();
		public:
			void								drawPrimitives						(GfxEnum::PRIMTYPE pt, size_t start_vert, size_t prim_count);
			void								drawPrimitives						(GfxEnum::PRIMTYPE pt, const Jotter& jotter);
			void								drawIndexedPrimitives				(GfxEnum::PRIMTYPE pt, const SPtr<IBO>& ibo, size_t start_index, size_t prim_count);
			void								drawIndexedPrimitives				(GfxEnum::PRIMTYPE pt, const uint32_t* indices, size_t index_count);
		private:
			void								__draw_indexed_16					(D3DPRIMITIVETYPE pt, const uint32_t* indices, size_t index_count, size_t prim_count, uint32_t min_index, uint32_t max_index);
		public:
			static D3DSHADEMODE					map_D3DSHADEMODE			(GfxEnum::SHADEMODE			v);
			static D3DFILLMODE					map_D3DFILLMODE				(GfxEnum::POLYGONMODE		v);
			static D3DFOGMODE					map_D3DFOGMODE				(GfxEnum::FOGMODE			v);
			static D3DCMPFUNC					map_D3DCMPFUNC				(GfxEnum::CMPFUNC			v);
			static D3DPRIMITIVETYPE				map_D3DPRIMITIVETYPE		(GfxEnum::PRIMTYPE			v);
			static D3DTEXTUREFILTERTYPE			map_D3DTEXTUREFILTERTYPE	(GfxEnum::FILTERMODE		v);
			static D3DTEXTUREADDRESS			map_D3DTEXTUREADDRESS		(GfxEnum::WRAPMODE			v);
			static D3DBLEND						map_D3DBLEND				(GfxEnum::BLENDFACTOR		v);
			static D3DBLENDOP					map_D3DBLENDOP				(GfxEnum::BLENDOPERATION	v);
			static D3DSTENCILOP					map_D3DSTENCILOP			(GfxEnum::STENCILOP			v);
			static D3DCULL						map_D3DCULL					(GfxEnum::CULLMODE			v);
			static D3DLIGHTTYPE					map_D3DLIGHTTYPE			(GfxEnum::LIGHTTYPE			v);
			static D3DFORMAT					map_D3DFORMAT				(GfxEnum::PIXELFORMAT		v);
			static D3DPOOL						map_D3DPOOL					(GfxEnum::POOL				v);
			static D3DTEXTUREOP					map_D3DTEXTUREOP			(GfxEnum::TEXTUREOP			v);
			static DWORD						map_D3DTA					(GfxEnum::TEXTUREARG		v);
			static D3DCUBEMAP_FACES				map_D3DCUBEMAP_FACES		(GfxEnum::CUBEMAP_FACE		v);
		};
	};
};

