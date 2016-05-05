
#pragma once

#include "predecl.h"
#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class rsShade
		{
		public:
			typedef REAL					REAL4[4];
			typedef REAL					REAL3[3];
			typedef REAL					REAL44[4][4];
		public:
			enum
			{
				MAX_LIGHTS = 8,
				MAX_CLIP_PLANES = 6,
				MAX_STAGES = 8,
				MAX_SAMPLERS = 16,
			};
		public:
			union
			{
				size_t																					__UNNAMED00;
				struct
				{
					bool					fixed_pipeline : 1;
				};
			};
		public:
			union
			{
				uint64_t																				__UNNAMED01;
				struct
				{
					GfxEnum::CULLMODE			cull_mode : 4;

					GfxEnum::SHADEMODE			shade_mode : 4;

					GfxEnum::POLYGONMODE		polygon_mode : 4;

					GfxEnum::CMPFUNC			depth_func : 4;
					GfxEnum::CMPFUNC			alpha_func : 4;

					GfxEnum::BLENDFACTOR		src_blend_factor : 4;
					GfxEnum::BLENDFACTOR		dst_blend_factor : 4;

					GfxEnum::BLENDOPERATION		blend_operation : 4;
				};
			};

			REAL					alpha_ref;

			struct
			{
				GfxEnum::CMPFUNC		front_func;
				GfxEnum::CMPFUNC		back_func;
				int						ref;
				unsigned int			mask;

			}						stencil;

			union
			{
				uint32_t																				__UNNAMED02;
				struct
				{
					GfxEnum::STENCILOP		front_fail : 4;
					GfxEnum::STENCILOP		front_zfail : 4;
					GfxEnum::STENCILOP		front_pass : 4;

					GfxEnum::STENCILOP		back_fail : 4;
					GfxEnum::STENCILOP		back_zfail : 4;
					GfxEnum::STENCILOP		back_pass : 4;

				}						stencil_op;
			};

			REAL4					clip_planes [ MAX_CLIP_PLANES ];
			size_t					clip_plane_count;

			struct
			{
				int32_t					x;
				int32_t					y;
				int32_t					dx;
				int32_t					dy;
			}						scissor;

			union																					SS
			{
				uint32_t																				__UNNAMED03;
				struct
				{
					GfxEnum::FILTERMODE			min_filter:4;
					GfxEnum::FILTERMODE			mag_filter:4;
					GfxEnum::FILTERMODE			mip_filter:4;

					GfxEnum::WRAPMODE			u_wrap:4;
					GfxEnum::WRAPMODE			v_wrap:4;
					GfxEnum::WRAPMODE			w_wrap:4;

				};
			}						ss [ MAX_SAMPLERS ];

			size_t					sampler_use_bitset;

			struct
			{
				union
				{
					uint32_t																				__UNNAMED04;
					struct
					{
						bool					R : 1;
						bool					G : 1;
						bool					B : 1;
						bool					A : 1;
						bool					depth : 1;
					};
				};
				unsigned int			stencil;
			}						write;

			union
			{
				size_t																					__UNNAMED05;
				struct
				{
					uint8_t					cull : 1;
					uint8_t					depth_test : 1;
					uint8_t					alpha_test : 1;
					uint8_t					blend : 1;
					uint8_t					stencil : 1;
					uint8_t					two_side_stencil : 1;
					uint8_t					scissor_test : 1;
				}						enable;
			};
		public:
			struct																					FIXED
			{
				REAL4					default_color;

				struct
				{
					GfxEnum::FOGMODE		mode;
					REAL					density;
					REAL					start;
					REAL					end;
					REAL4					color;

				}						fog;

				struct																					LIGHTING
				{
					struct
					{
						REAL4					ambient;
						REAL4					diffuse;
						REAL4					specular;
						REAL4					emissive;
						REAL					shininess;

					}						material;

					REAL4					light_ambient;

					struct																					LIGHT
					{
						GfxEnum::LIGHTTYPE		type;
						REAL4					ambient;
						REAL4					diffuse;
						REAL4					specular;
						REAL3					position;
						REAL3					direction;
						REAL					outer_cone;				// in degrees
						REAL					inner_cone;				// in degrees
						REAL					falloff;
						REAL3					attenuation;

					}						lights [ MAX_LIGHTS ];

					size_t					light_count;

				} lighting;

				struct																					TSS
				{
					GfxEnum::TEXTUREOP			color_op : 4;
					GfxEnum::TEXTUREARG			color_arg1 : 4;
					GfxEnum::TEXTUREARG			color_arg2 : 4;

					GfxEnum::TEXTUREOP			alpha_op : 4;
					GfxEnum::TEXTUREARG			alpha_arg1 : 4;
					GfxEnum::TEXTUREARG			alpha_arg2 : 4;

				}						tss [ MAX_STAGES ];

				REAL44					tct [ MAX_STAGES ];

				size_t					texture_stage_count;

				union
				{
					size_t																				__UNNAMED06;
					struct
					{
						uint8_t					lighting : 1;
						uint8_t					fog : 1;
						uint8_t					normalize : 1;
						uint8_t					specular : 1;

					}						enable;
				};

			}						fixed;
		public:
			rsShade(bool fixed_pipeline);
			rsShade(const rsShade& rhs);
		public:
			rsShade&					operator=(const rsShade& rhs);
		private:
			void						__assign(const rsShade& rhs);
		public:
			void						enableWriteColor								(bool red, bool green, bool blue, bool alpha);
			void						enableWriteDepth								(bool write);
			void						enableWriteStencil								(unsigned int mask);
		public:
			void						enableCull										(GfxEnum::CULLMODE mode = GfxEnum::CULLMODE_CW);
			void						enableDepthTest									(GfxEnum::CMPFUNC func = GfxEnum::CMPFUNC_LESS);
			void						enableAlphaTest									(GfxEnum::CMPFUNC func = GfxEnum::CMPFUNC_GREATER, REAL ref = 0.5f);
			void						enableBlend										(GfxEnum::BLENDFACTOR sbf = GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR dbf = GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GfxEnum::BLENDOPERATION bo = GfxEnum::BLENDOPERATION_ADD);
			void						enableStencilTest								(GfxEnum::CMPFUNC front_func, int ref, int mask, GfxEnum::STENCILOP front_fail, GfxEnum::STENCILOP front_zfail, GfxEnum::STENCILOP front_pass);
			void						enableTwoSideStencilTest						(GfxEnum::CMPFUNC back_func, GfxEnum::STENCILOP back_fail, GfxEnum::STENCILOP back_zfail, GfxEnum::STENCILOP back_pass);
			void						enableScissorTest								(int32_t x, int32_t y, int32_t dx, int32_t dy);
			void						enableFixedFog									(GfxEnum::FOGMODE mode, REAL density, REAL start, REAL end, const Vec4& color);
			void						enableFixedLighting								(const Vec4& light_ambient, const Vec4& mtl_ambient, const Vec4& mtl_diffuse, const Vec4& mtl_specular, const Vec4& mtl_emissive, REAL mtl_shininess);
			void						enableFixedNormalize							();
			void						enableFixedSpecular								();
		public:
			void						setShadeMode									(GfxEnum::SHADEMODE mode);
			void						setPolygonMode									(GfxEnum::POLYGONMODE mode);
			void						setFixedDefaultColor							(const Vec4& color);
		public:
			bool						addClipPlane									(const Plane& p);
			bool						setSampler										(size_t index, GfxEnum::FILTERMODE min_filter = GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE mag_filter = GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE mip_filter = GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE u_wrap = GfxEnum::WRAPMODE_REPEAT, GfxEnum::WRAPMODE v_wrap = GfxEnum::WRAPMODE_REPEAT, GfxEnum::WRAPMODE w_wrap = GfxEnum::WRAPMODE_REPEAT);
			bool						addFixedLight									(GfxEnum::LIGHTTYPE type, const Vec4& ambient, const Vec4& diffuse, const Vec4& specular, const Vec3& position, const Vec3& direction, REAL outer_cone, REAL inner_cone, REAL falloff, const Vec3& attenuation);
			bool						addFixedTextureStage							(GfxEnum::TEXTUREOP color_op, GfxEnum::TEXTUREARG color_arg1, GfxEnum::TEXTUREARG color_arg2, GfxEnum::TEXTUREOP alpha_op, GfxEnum::TEXTUREARG alpha_arg1, GfxEnum::TEXTUREARG alpha_arg2, const Mat4& texcoord_transform = Mat4::identity);
		public:
			void						disableWriteColor								();
			void						disableWriteDepth								();
		public:
			void						disableCull										();
			void						disableDepthTest								();
			void						disableAlphaTest								();
			void						disableBlend									();
		public:
			void						reverseCull										();
		public:
			void						reset											(bool fixed_pipeline);
		};
	};
};

