
#pragma once

#include "predecl.h"

#include "ShaderGen.h"
#include "ShaderGenConf.h"

namespace z3D
{
	namespace Video
	{
		class d3dGfx;
		class d3dShader;

		class d3dShaderGen: public ShaderGen
		{
		public:
			typedef uint32_t ARRAY_TC;							// 2-bit each
			typedef uint32_t ARRAY_SAMPLER;						// 2-bit each
		public:
			class VSID
			{
			public:
				union
				{
					uint32_t			__LIGHTING;
					struct
					{
						uint8_t				dir: 4;
						uint8_t				point: 4;
						uint8_t				point_range: 4;
						uint8_t				spot: 4;
						uint8_t				spot_range: 4;
					}					lighting;
				};

				ARRAY_TC			array_tc;

				union
				{
					uint32_t			__ENABLE;
					struct
					{
						uint8_t				vertex_color : 2;
						uint8_t				specular : 1;
						uint8_t				sphere_mapping : 1;
						uint8_t				cube_mapping : 1;
						uint8_t				per_pixel_lighting : 1;
						uint8_t				normal_mapping : 2;
						uint8_t				lights_visibility : 1;
					}					enable;
				};
			public:
				VSID();
			public:
				bool				operator< (const VSID& id) const;
			};
			class PSID
			{
			public:
				ARRAY_TC			array_tc;
				ARRAY_SAMPLER		array_sampler;
				uint8_t				array_tex_fetch[ ShaderGenConf::MAX_TEX_FETCHES ];
				size_t				tex_fetches;
				union
				{
					uint32_t			__LIGHTING;
					struct
					{
						uint8_t				dir: 4;
						uint8_t				point: 4;
						uint8_t				point_range: 4;
						uint8_t				spot: 4;
						uint8_t				spot_range: 4;
					}					lighting;
				};
				uint32_t			lights_visibility;						// 4-bit each, binary 1111 means no visibility information
				union
				{
					uint32_t			__ENABLE;
					struct
					{
						uint8_t				per_pixel_lighting: 1;
						uint8_t				specular: 1;				// useful only if per_pixel_lighting is on
						uint8_t				normal_mapping: 1;
						uint8_t				cube_mapping: 1;
						uint8_t				two_sided_lighting : 1;
					}					enable;
				};
			public:
				PSID();
			public:
				bool				operator< (const PSID& id) const;
			};
		private:
			d3dGfx*					_gfx;
		private:
			typedef map<VSID, SPtr<d3dShader> >
									MAP_VSID;
			typedef map<PSID, map<mbs, SPtr<d3dShader> > >
									MAP_PSID;
			MAP_VSID				_map_id_vs;
			MAP_PSID				_map_id_ps;
		private:
			mbs						_code_lit_dir;
			mbs						_code_lit_dir_spec;
		private:
			mbs						_code_lit_point;
			mbs						_code_lit_point_spec;
			mbs						_code_lit_point_range;
			mbs						_code_lit_point_spec_range;
		private:
			mbs						_code_lit_spot;
			mbs						_code_lit_spot_spec;
			mbs						_code_lit_spot_range;
			mbs						_code_lit_spot_spec_range;
		public:
			d3dShaderGen();
		private:
			void					initShaderFragments();
			mbs						getFunc(const mbs& source, const mbs& id);
		public:
			VSID					genVSID(const ShaderGenConf& cfg);
			PSID					genPSID(const ShaderGenConf& cfg);
		public:
			SPtr<Shader>			genVS(const ShaderGenConf& cfg);
			SPtr<Shader>			genPS(const ShaderGenConf& cfg);
			void					setShaderUniforms(const ShaderGenConf& cfg, ShaderUniforms& unis, bool vs, bool ps);
			void					setShaderUniforms(const ShaderGenConf& cfg, ShaderUniforms2& unis, bool vs, bool ps);
		public:
			void					clear();
		public:
			friend class d3dGfx;
		};
	};
};

