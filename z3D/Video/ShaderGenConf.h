
#pragma once

#include "predecl.h"
#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class ShaderGenConf
		{
		public:
			typedef REAL REAL3[3];
			typedef REAL REAL4[4];
			typedef REAL REAL44[4][4];
		public:
			enum
			{
				MAX_LIGHTS = 8,
				MAX_TEXCOORD_SETS = 8,
				MAX_SAMPLERS = MAX_TEXCOORD_SETS,
				MAX_TEX_FETCHES = 8,
				NAME_SIZE = 32,
				PS_CODE_SIZE = 2048,
			};
			typedef enum _TCGEN
			{
				TCGEN_UNUSED = 0,
				TCGEN_INTACT,
				TCGEN_XFORM,
				TCGEN_SPHERE_MAPPING,
				TCGEN_CUBE_MAPPING,
			} TCGEN;
		public:
			typedef struct _LIGHT
			{
				GfxEnum::LIGHTTYPE					type;
				REAL3								color;
				REAL3								pos;
				REAL3								dir;
				REAL								inner_cone;
				REAL								outer_cone;
				REAL								range;
				ptrdiff_t							visibility_channel;
			} LIGHT;
		public:
			REAL44								world;
			//REAL44								view;
			//REAL44								unadjusted_proj;
		public:
			struct MATERIAL
			{
			public:
				REAL3								ambient;
				REAL3								diffuse;
				REAL3								specular;
				REAL								shininess;
				REAL								opacity;
			} material;
			REAL3								light_ambient;
			LIGHT								array_light[ MAX_LIGHTS ];
			size_t								lights;
		public:
			REAL44								array_tc_xform[ MAX_TEXCOORD_SETS ];
			uint8_t								array_tc[ MAX_TEXCOORD_SETS ];
		public:
			uint8_t								array_sampler[ MAX_SAMPLERS ];
		public:
			struct _TEX_FETCH
			{
				uint8_t								tc_idx;
				uint8_t								sampler_idx;
			}									array_tex_fetch[ MAX_TEX_FETCHES ];
			size_t								tex_fetches;
		public:
			REAL44								nm_tc_xform;
		private:
			char								_ps_code[ PS_CODE_SIZE ];
			char*								_ps_code_end;
		public:
			union
			{
				size_t								__ENABLE;
				struct ENABLE
				{
					uint8_t								vertex_color: 2;					// 0 - disable, 1 - 3 components, 2 - 4 components
					uint8_t								per_pixel_lighting: 1;
					uint8_t								normal_mapping: 2;					// 0 - disable, 1 - enabled, w/o tc xform, 2 - enabled, w/ tc xform
					uint8_t								two_sided_lighting : 1;
				} enable;
			};
		public:
			//void								begin(const Mat4& world, const Mat4& view, const Mat4& unadjusted_proj);
			void								begin(const Mat4& world);
		public:
			void								setMaterial(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, REAL shininess, REAL opacity);
			void								setMaterialAmbient(const Vec3& ambient, REAL opacity);
			void								setLightAmbient(const Vec3& light_ambient);
			void								addDirLight(const Vec3& color, const Vec3& dir, ptrdiff_t visibility_channel = -1);
			void								addPointLight(const Vec3& color, const Vec3& pos, REAL range = INFINITY, ptrdiff_t visibility_channel = -1);
			void								addSpotLight(const Vec3& color, const Vec3& pos, const Vec3& dir, REAL inner_cone, REAL outer_cone, REAL range = INFINITY, ptrdiff_t visibility_channel = -1);
		public:
			void								useTexCoord(size_t index, const Mat4& xform = Mat4::identity);
			void								useTexCoord(size_t index, TCGEN tcgen);
		public:
			void								useTexSampler(size_t index, GfxEnum::TEXTARGET target);
			void								fetchTex(size_t sampler_idx, size_t tc_idx);
		public:
			void								addPSCode(const mbs& code);
			void								addPSCode(const char* code);
		public:
			const char*							getPSCode() const;					// DO NOT use this method to modify the ps code, use addPSCode instead
		public:
			void								enableVertexColor(bool has_alpha);
			void								enablePerPixelLighting();
			void								enableNormalMapping(const Mat4& xform = Mat4::identity);
			void								enableTwoSidedLighting();
		public:
			bool								queryUseSpecular() const;
		};
	};
};

