
#define _CRT_SECURE_NO_WARNINGS 1

#include "ShaderUniforms.h"
#include "ShaderUniforms2.h"

#include "d3dShaderGen.h"
#include "d3dGfx.h"
#include "d3dShader.h"

namespace z3D
{
	namespace Video
	{
		d3dShaderGen::VSID::VSID()
		{
			memset(this, 0, sizeof(VSID));
		}
		bool				d3dShaderGen::VSID::operator< (const VSID& id) const
		{
			return memcmp(this, &id, sizeof(VSID)) < 0;
		}

		d3dShaderGen::PSID::PSID()
		{
			memset(this, 0, sizeof(PSID));
		}
		bool				d3dShaderGen::PSID::operator< (const PSID& id) const
		{
			return memcmp(this, &id, sizeof(PSID)) < 0;
		}

		d3dShaderGen::d3dShaderGen()
		{
			initShaderFragments();
		}
		d3dShaderGen::VSID		d3dShaderGen::genVSID(const ShaderGenConf& cfg)
		{
			VSID id;

			id.enable.per_pixel_lighting = cfg.enable.per_pixel_lighting;
			id.enable.specular = (cfg.lights && (Vec3::zero != *(Vec3*)cfg.material.specular));
			for(size_t i = 0; i < cfg.lights; ++i)
			{
				if(cfg.array_light[i].range == INFINITY)
				{
					switch(cfg.array_light[i].type)
					{
					case GfxEnum::LIGHTTYPE_DIRECTIONAL:	++id.lighting.dir;			break;
					case GfxEnum::LIGHTTYPE_POINT:			++id.lighting.point;		break;
					case GfxEnum::LIGHTTYPE_SPOT:			++id.lighting.spot;			break;
					default:								Z_ASSERT( false );
					}
				}
				else
				{
					switch(cfg.array_light[i].type)
					{
					case GfxEnum::LIGHTTYPE_DIRECTIONAL:	++id.lighting.dir;			break;
					case GfxEnum::LIGHTTYPE_POINT:			++id.lighting.point_range;	break;
					case GfxEnum::LIGHTTYPE_SPOT:			++id.lighting.spot_range;	break;
					default:								Z_ASSERT( false );
					}
				}
				id.enable.lights_visibility |= (cfg.array_light[i].visibility_channel != -1);
			}

			Z_STATIC_ASSERT( sizeof(id.array_tc) * 8 / 4 >= ShaderGenConf::MAX_TEXCOORD_SETS, ASSUMPTION_WRONG );

			for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS; ++i)
			{
				ARRAY_TC tc = (ARRAY_TC)cfg.array_tc[i];
				id.array_tc |= (tc & 0x0f) << (i << 2);
				if(tc == ShaderGenConf::TCGEN_SPHERE_MAPPING)
					id.enable.sphere_mapping = true;
				else if(tc == ShaderGenConf::TCGEN_CUBE_MAPPING)
					id.enable.cube_mapping = true;
			}

			if(!id.enable.per_pixel_lighting)
				id.enable.vertex_color = cfg.enable.vertex_color;			// only vertex lighting supports vertex color

			id.enable.normal_mapping = (cfg.lights || id.enable.cube_mapping) ? cfg.enable.normal_mapping : 0;

			return id;
		}
		d3dShaderGen::PSID			d3dShaderGen::genPSID(const ShaderGenConf& cfg)
		{
			PSID id;

			for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS; ++i)
			{
				ARRAY_TC tc = (ARRAY_TC)cfg.array_tc[i];
				id.array_tc |= (tc & 0x0f) << (i << 2);
			}
			for(size_t i = 0; i < ShaderGenConf::MAX_SAMPLERS; ++i)
			{
				ARRAY_SAMPLER sampler= (ARRAY_SAMPLER)cfg.array_sampler[i];
				id.array_sampler |= (sampler & 0x0f) << (i << 2);
			}
			for(size_t i = 0; i < cfg.tex_fetches; ++i)
			{
				const ShaderGenConf::_TEX_FETCH& tf = cfg.array_tex_fetch[i];
				id.array_tex_fetch[i] = tf.tc_idx | (tf.sampler_idx << 4);
			}
			id.tex_fetches = cfg.tex_fetches;

			id.enable.per_pixel_lighting = cfg.enable.per_pixel_lighting;
			if(id.enable.per_pixel_lighting)
			{
				id.enable.specular = (cfg.lights && (Vec3::zero != *(Vec3*)cfg.material.specular));
				id.enable.two_sided_lighting = cfg.enable.two_sided_lighting;
				for(size_t i = 0; i < cfg.lights; ++i)
				{
					if(cfg.array_light[i].range == INFINITY)
					{
						switch(cfg.array_light[i].type)
						{
						case GfxEnum::LIGHTTYPE_DIRECTIONAL:	++id.lighting.dir;			break;
						case GfxEnum::LIGHTTYPE_POINT:			++id.lighting.point;		break;
						case GfxEnum::LIGHTTYPE_SPOT:			++id.lighting.spot;			break;
						default:								Z_ASSERT( false );
						}
					}
					else
					{
						switch(cfg.array_light[i].type)
						{
						case GfxEnum::LIGHTTYPE_DIRECTIONAL:	++id.lighting.dir;			break;
						case GfxEnum::LIGHTTYPE_POINT:			++id.lighting.point_range;	break;
						case GfxEnum::LIGHTTYPE_SPOT:			++id.lighting.spot_range;	break;
						default:								Z_ASSERT( false );
						}
					}
				}
				if(true)
				{
					id.lights_visibility = -1I32;

					size_t dir = 0;
					size_t point = dir + id.lighting.dir;
					size_t point_range = point + id.lighting.point;
					size_t spot = point_range + id.lighting.point_range;
					size_t spot_range = spot + id.lighting.spot;

					for(size_t i = 0; i < cfg.lights; ++i)
					{
						if(cfg.array_light[i].range == INFINITY)
						{
							switch(cfg.array_light[i].type)
							{
							case GfxEnum::LIGHTTYPE_DIRECTIONAL:	id.lights_visibility &= ((cfg.array_light[i].visibility_channel & 0x0f) << (dir << 2)) | ~(0x0fI32 << (dir << 2));							++dir;			break;
							case GfxEnum::LIGHTTYPE_POINT:			id.lights_visibility &= ((cfg.array_light[i].visibility_channel & 0x0f) << (point << 2)) | ~(0x0fI32 << (point << 2));						++point;		break;
							case GfxEnum::LIGHTTYPE_SPOT:			id.lights_visibility &= ((cfg.array_light[i].visibility_channel & 0x0f) << (spot << 2)) | ~(0x0fI32 << (spot << 2));						++spot;			break;
							default:								Z_ASSERT( false );
							}
						}
						else
						{
							switch(cfg.array_light[i].type)
							{
							case GfxEnum::LIGHTTYPE_DIRECTIONAL:	id.lights_visibility &= ((cfg.array_light[i].visibility_channel & 0x0f) << (dir << 2)) | ~(0x0fI32 << (dir << 2));							++dir;			break;
							case GfxEnum::LIGHTTYPE_POINT:			id.lights_visibility &= ((cfg.array_light[i].visibility_channel & 0x0f) << (point_range << 2)) | ~(0x0fI32 << (point_range << 2));			++point_range;	break;
							case GfxEnum::LIGHTTYPE_SPOT:			id.lights_visibility &= ((cfg.array_light[i].visibility_channel & 0x0f) << (spot_range << 2)) | ~(0x0fI32 << (spot_range << 2));			++spot_range;	break;
							default:								Z_ASSERT( false );
							}
						}
					}
				}
				for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS; ++i)
					if(cfg.array_tc[i] == ShaderGenConf::TCGEN_CUBE_MAPPING)
						id.enable.cube_mapping = true;

				id.enable.normal_mapping = (cfg.lights || id.enable.cube_mapping) ? (!!cfg.enable.normal_mapping) : 0;
			}

			return id;
		}
		SPtr<Shader>			d3dShaderGen::genVS(const ShaderGenConf& cfg)
		{
			VSID id = genVSID(cfg);

			MAP_VSID::iterator iter;
			if((iter = _map_id_vs.find(id)) != _map_id_vs.end())
				return iter->second;

			mbs source;

			source += "\n";

			if(!id.enable.per_pixel_lighting)			// vertex lighting
			{
				if(!id.enable.specular)					// non specular lighting code
				{
					if(id.lighting.dir)
						source += _code_lit_dir;					// directional
					if(id.lighting.point)
						source += _code_lit_point;					// point
					if(id.lighting.point_range)
						source += _code_lit_point_range;			// ranged point
					if(id.lighting.spot)
						source += _code_lit_spot;					// spot
					if(id.lighting.spot_range)
						source += _code_lit_spot_range;				// ranged spot
				}
				else									// specular lighting code
				{
					if(id.lighting.dir)
						source += _code_lit_dir_spec;				// directional
					if(id.lighting.point)
						source += _code_lit_point_spec;				// point
					if(id.lighting.point_range)
						source += _code_lit_point_spec_range;		// ranged point
					if(id.lighting.spot)
						source += _code_lit_spot_spec;				// spot
					if(id.lighting.spot_range)
						source += _code_lit_spot_spec_range;		// ranged spot
				}

				// uniforms ...
				source += "float4x4 wvp;\n";
				if(id.enable.specular || (id.lighting.point || id.lighting.point_range || id.lighting.spot || id.lighting.spot_range) || id.enable.cube_mapping)
					source += "float4x4 world;\n";
				if(cfg.lights || id.enable.cube_mapping)
					source += "float3x3 world_nm;\n";
				if(id.enable.specular || id.enable.cube_mapping)
					source += "float3 cam_pos;\n";
				if(id.enable.specular)
					source += "float shin;\n";
				source += "float opac;\n";
				source += "float3 ambient;\n";
				if(id.enable.specular)
				{
					size_t lights = 0;
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_dir;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
",
								 lights, lights, lights);

					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
",
								 lights, lights, lights);

					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
",
								 lights, lights, lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights, lights, lights, lights);

				}
				else
				{
					size_t lights = 0;
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_dir;\n\
float3 l%d_diff;\n\
",
								 lights, lights);

					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
",
								 lights, lights);

					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
",
								 lights, lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights, lights, lights);

				}
				if(id.enable.sphere_mapping)
					source += "\
float4x4 wv;\n\
float3x3 wv_nm;\n\
";
				if(true)
				{
					d3dShaderGen::ARRAY_TC tc = id.array_tc;
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS && tc; ++i)
					{
						if((tc & 0x0f) == ShaderGenConf::TCGEN_XFORM)
						{
							source += FORMATA("\
float4x4 tc_xform_%d;\n\
", i);
						}
						tc >>= 4;
					}
				}
				source += "\n";

				// code ...
				source += "void main(\n";
				source += "	float4 in_pos: POSITION,\n";
				if(cfg.lights || id.enable.sphere_mapping || id.enable.cube_mapping)
					source += "	float3 in_nor: NORMAL,\n";
				if(id.enable.vertex_color)
				{
					source += "	float4 in_color: COLOR,\n";
					//switch(id.enable.vertex_color)
					//{
					//case 1:	source += "	float3 in_color: COLOR,\n"; break;
					//case 2:	source += "	float4 in_color: COLOR,\n"; break;
					//}
				}
				source += "	out float4 out_pos: POSITION,\n";
				source += "	out float4 out_diff: COLOR0,\n";
				if(id.enable.specular || true)					// simplify pixel shader branching
					source += "	out float4 out_spec: COLOR1,\n";

				if(true)
				{
					d3dShaderGen::ARRAY_TC tc = id.array_tc;
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS && tc; ++i)
					{
						switch(tc & 0x0f)
						{
						case ShaderGenConf::TCGEN_UNUSED:
							break;
						case ShaderGenConf::TCGEN_INTACT:
							source += FORMATA("\
	float4 in_tc_%d: TEXCOORD%d,\n\
	out float4 out_tc_%d: TEXCOORD%d,\n\
", i, i, i, i);
							break;
						case ShaderGenConf::TCGEN_XFORM:
							source += FORMATA("\
	float4 in_tc_%d: TEXCOORD%d,\n\
	out float4 out_tc_%d: TEXCOORD%d,\n\
", i, i, i, i);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							source += FORMATA("	out float2 out_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_CUBE_MAPPING:
							source += FORMATA("	out float3 out_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						}
						tc >>= 4;
					}
				}

				source += "\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
";
				source += "	out_pos = mul(in_pos, wvp);\n";

				if(id.enable.specular || (id.lighting.point || id.lighting.point_range || id.lighting.spot || id.lighting.spot_range) || id.enable.cube_mapping)
					source += "\
	float3 world_pos = mul(in_pos, world).xyz;\n\
";
				if(cfg.lights || id.enable.cube_mapping)
					source += "\
	float3 world_nor = normalize(mul(in_nor, world_nm));\n\
";
				if(id.enable.specular || id.enable.cube_mapping)
					source += "	float3 cam_vec = normalize(cam_pos - world_pos.xyz);\n";

				source += "	out_diff = float4(ambient, opac);\n";
				if(id.enable.specular || true)					// simplify pixel shader branching
					source += "	out_spec = float4(0.0, 0.0, 0.0, 0.0);\n";

				if(!id.enable.specular)
				{
					size_t lights = 0;
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						source += FORMATA("	lit_dir(world_nor, l%d_dir, l%d_diff, out_diff.xyz);\n", lights, lights);
					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						source += FORMATA("	lit_point(world_pos, world_nor, l%d_pos, l%d_diff, out_diff.xyz);\n", lights, lights);
					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						source += FORMATA("	lit_point_range(world_pos, world_nor, l%d_pos, l%d_diff, l%d_range, out_diff.xyz);\n", lights, lights, lights);
					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						source += FORMATA("	lit_spot(world_pos, world_nor, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, out_diff.xyz);\n", lights, lights, lights, lights, lights);
					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						source += FORMATA("	lit_spot_range(world_pos, world_nor, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, l%d_range, out_diff.xyz);\n", lights, lights, lights, lights, lights, lights);
				}
				else
				{
					size_t lights = 0;
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						source += FORMATA("	lit_dir_spec(world_nor, cam_vec, shin, l%d_dir, l%d_diff, l%d_spec, out_diff.xyz, out_spec.xyz);\n", lights, lights, lights);
					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						source += FORMATA("	lit_point_spec(world_pos, world_nor, cam_vec, shin, l%d_pos, l%d_diff, l%d_spec, out_diff.xyz, out_spec.xyz);\n", lights, lights, lights);
					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						source += FORMATA("	lit_point_spec_range(world_pos, world_nor, cam_vec, shin, l%d_pos, l%d_diff, l%d_spec, l%d_range, out_diff.xyz, out_spec.xyz);\n", lights, lights, lights, lights);
					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						source += FORMATA("	lit_spot_spec(world_pos, world_nor, cam_vec, shin, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, l%d_spec, out_diff.xyz, out_spec.xyz);\n", lights, lights, lights, lights, lights, lights);
					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						source += FORMATA("	lit_spot_spec_range(world_pos, world_nor, cam_vec, shin, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, l%d_spec, l%d_range, out_diff.xyz, out_spec.xyz);\n", lights, lights, lights, lights, lights, lights, lights);
				}

				if(id.enable.vertex_color)
				{
					switch(id.enable.vertex_color)
					{
					case 1:	source += "	out_diff.xyz *= in_color.xyz;\n"; break;
					case 2:	source += "	out_diff *= in_color;\n"; break;
					}
				}

				if(true)
				{
					d3dShaderGen::ARRAY_TC tc = id.array_tc;
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS && tc; ++i)
					{
						switch(tc & 0x0f)
						{
						case ShaderGenConf::TCGEN_UNUSED:
							break;
						case ShaderGenConf::TCGEN_INTACT:
							source += FORMATA("	out_tc_%d = in_tc_%d;\n", i, i);
							break;
						case ShaderGenConf::TCGEN_XFORM:
							source += FORMATA("	out_tc_%d = mul(in_tc_%d, tc_xform_%d);\n", i, i, i);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							source += FORMATA("\
	if(true)\n\
	{\n\
		float3 tc = reflect(normalize(mul(in_pos, wv).xyz), normalize(mul(in_nor, wv_nm)));\n\
		tc.z += 1.0;\n\
		out_tc_%d = normalize(tc).xy * 0.5 + float2(0.5, 0.5);\n\
	}\n\
", i);
							break;
						case ShaderGenConf::TCGEN_CUBE_MAPPING:
							source += FORMATA("	out_tc_%d = reflect(-cam_vec, world_nor);\n", i);
							break;
						}
						tc >>= 4;
					}
				}

				source += "}\n";
			}
			else
			{
				source += "float4x4 wvp;\n";
				if(id.enable.lights_visibility)
					source += "float4x4 light_vis_space_xform;\n";
				if((!id.enable.normal_mapping && (cfg.lights || id.enable.cube_mapping)) || id.enable.normal_mapping)
					source += "float4x4 world;\n";
				if((!id.enable.normal_mapping && (cfg.lights || id.enable.cube_mapping)) || id.enable.normal_mapping)
					source += "float3x3 world_nm;\n";

				if(!id.enable.normal_mapping && id.enable.cube_mapping)
					source += "float3 cam_pos;\n";
				if(id.enable.sphere_mapping)
					source += "\
float4x4 wv;\n\
float3x3 wv_nm;\n\
";
				if(true)
				{
					d3dShaderGen::ARRAY_TC tc = id.array_tc;
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS && tc; ++i)
					{
						if((tc & 0x0f) == ShaderGenConf::TCGEN_XFORM)
							source += FORMATA("float4x4 tc_xform_%d;\n", i);
						tc >>= 4;
					}
				}

				if(id.enable.normal_mapping == 2)
					source += "float4x4 tc_xform_nm;\n";

				source += "\n";
				source += "void main(\n";

				source += "	float4 in_pos: POSITION,\n";
				if((!id.enable.normal_mapping && (cfg.lights || id.enable.cube_mapping)) || id.enable.normal_mapping || id.enable.sphere_mapping)
					source += "	float3 in_nor: NORMAL,\n";
				source += "	out float4 out_pos: POSITION,\n";
				if((!id.enable.normal_mapping && cfg.lights) || id.enable.normal_mapping)
					source += "	out float3 out_world_pos: TEXCOORD8,\n";
				if((!id.enable.normal_mapping && cfg.lights) || id.enable.normal_mapping)
					source += "	out float3 out_world_nor: TEXCOORD9,\n";

				if(true)
				{
					d3dShaderGen::ARRAY_TC tc = id.array_tc;
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS && tc; ++i)
					{
						switch(tc & 0x0f)
						{
						case ShaderGenConf::TCGEN_UNUSED:
							break;
						case ShaderGenConf::TCGEN_INTACT:
							source += FORMATA("\
	float4 in_tc_%d: TEXCOORD%d,\n\
	out float4 out_tc_%d: TEXCOORD%d,\n\
", i, i, i, i);
							break;
						case ShaderGenConf::TCGEN_XFORM:
							source += FORMATA("\
	float4 in_tc_%d: TEXCOORD%d,\n\
	out float4 out_tc_%d: TEXCOORD%d,\n\
", i, i, i, i);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							source += FORMATA("	out float2 out_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_CUBE_MAPPING:
							if(!id.enable.normal_mapping)
								source += FORMATA("	out float3 out_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						}
						tc >>= 4;
					}
				}

				if(id.enable.normal_mapping)
					source += "\
	float4 in_nm_tangent: TEXCOORD10,\n\
	out float4 out_nm_tangent: TEXCOORD10,\n\
	float4 in_tc_nm: TEXCOORD11,\n\
	out float4 out_tc_nm: TEXCOORD11,\n\
";
				if(id.enable.lights_visibility)
					source += "\
	out float4 out_light_vis_tc: TEXCOORD12,\n\
";

				source += "	uniform float4 END_PARAMS = 0\n";
				source += "	)\n";

				source += "{\n";

				source += "	out_pos = mul(in_pos, wvp);\n";
				if(id.enable.lights_visibility)
					source += "	out_light_vis_tc = mul(in_pos, light_vis_space_xform);\n";
				if((!id.enable.normal_mapping && cfg.lights) || id.enable.normal_mapping)
					source += "	out_world_pos = mul(in_pos, world).xyz;\n";
				if((!id.enable.normal_mapping && cfg.lights) || id.enable.normal_mapping)
					source += "	out_world_nor = normalize(mul(in_nor, world_nm));\n";

				source += "\n";
				if(true)
				{
					d3dShaderGen::ARRAY_TC tc = id.array_tc;
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS && tc; ++i)
					{
						switch(tc & 0x0f)
						{
						case ShaderGenConf::TCGEN_UNUSED:
							break;
						case ShaderGenConf::TCGEN_INTACT:
							source += FORMATA("	out_tc_%d = in_tc_%d;\n", i, i);
							break;
						case ShaderGenConf::TCGEN_XFORM:
							source += FORMATA("	out_tc_%d = mul(in_tc_%d, tc_xform_%d);\n", i, i, i);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							source += FORMATA("\
	if(true)\n\
	{\n\
		float3 tc = reflect(normalize(mul(in_pos, wv).xyz), normalize(mul(in_nor, wv_nm)));\n\
		tc.z += 1.0;\n\
		out_tc_%d = normalize(tc).xy * 0.5 + float2(0.5, 0.5);\n\
	}\n\
", i);
							break;
						case ShaderGenConf::TCGEN_CUBE_MAPPING:
							if(!id.enable.normal_mapping)
								source += FORMATA("	out_tc_%d = reflect(normalize(mul(in_pos, world).xyz - cam_pos), normalize(mul(in_nor, world_nm)));\n", i);
							break;
						}
						tc >>= 4;
					}

				}
				if(id.enable.normal_mapping)
				{
					source += "\
	out_nm_tangent = float4(normalize(mul(normalize(in_nm_tangent.xyz), world_nm)), in_nm_tangent.w * sign(dot(cross(world_nm[0], world_nm[1]), world_nm[2])));\n\
";
					switch(id.enable.normal_mapping)
					{
					case 1:
						source += "	out_tc_nm = in_tc_nm;\n";
						break;
					case 2:
						source += "	out_tc_nm = mul(in_tc_nm, tc_xform_nm);\n";
						break;
					}
				}

				source += "}\n";
			}

			SPtr<Shader> shader = _gfx->createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, source);
			if(!shader)
			{
				FILE* fp = NULL;
				fopen_s(&fp, "problem_vs.txt", "w");
				if(fp)
				{
					fwrite(source.c_str(), source.length(), 1, fp);
					fclose(fp);
				}
				Z_ASSERT( false );
				return NULL;
			}
			if(false)
			{
				FILE* fp = NULL;
				fopen_s(&fp, "problem_vs.txt", "w");
				//_wfopen_s(&fp, FNGen::gen(L"problem_vs_", 5, L".txt").c_str(), L"w");
				if(fp)
				{
					fwrite(source.c_str(), source.length(), 1, fp);
					fclose(fp);
				}
			}
			_map_id_vs[id] = shader.cast<d3dShader>();

			return shader;
		}
		SPtr<Shader>			d3dShaderGen::genPS(const ShaderGenConf& cfg)
		{
			PSID id = genPSID(cfg);
			if(true)
			{
				MAP_PSID::iterator iter = _map_id_ps.find(id);
				if(iter != _map_id_ps.end())
				{
					map<mbs, SPtr<d3dShader> >::iterator iter2 = iter->second.find(cfg.getPSCode());
					if(iter2 != iter->second.end())
						return iter2->second;
				}
			}

			mbs source;

			source += "\n";

			if(!id.enable.per_pixel_lighting)
			{
				if(true)
				{
					ARRAY_SAMPLER s = id.array_sampler;
					size_t i = 0;
					while(s)
					{
						switch(s & 0xf)
						{
						case GfxEnum::TEXTARGET_2D:
							source += FORMATA("sampler2D sampler_%d: register(s%d);\n", i, i);
							break;
						case GfxEnum::TEXTARGET_3D:
							source += FORMATA("sampler3D sampler_%d: register(s%d);\n", i, i);
							break;
						case GfxEnum::TEXTARGET_CUBE:
							source += FORMATA("samplerCUBE sampler_%d: register(s%d);\n", i, i);
							break;
						}
						s >>= 4;
						++i;
					}
				}

				source += "\n";

				source += "\
void main(\n\
	float4 in_diff: COLOR0,\n\
	float3 in_spec: COLOR1,\n\
	out float4 out_color: COLOR,\n\
";
				if(true)
				{
					ARRAY_TC tc = id.array_tc;
					size_t i = 0;
					while(tc)
					{
						switch(tc & 0xf)
						{
						case ShaderGenConf::TCGEN_INTACT:
							source += FORMATA("	float4 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_XFORM:
							source += FORMATA("	float4 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							source += FORMATA("	float2 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_CUBE_MAPPING:
							source += FORMATA("	float3 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						}
						tc >>= 4;
						++i;
					}
				}
				source += "\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
";
				for(size_t i = 0; i < id.tex_fetches; ++i)
				{
					uint8_t tf = id.array_tex_fetch[i];
					uint8_t tc = tf & 0xf;
					uint8_t s = (tf >> 4);
					source += FORMATA("	float4 tf_%d_%d = ", s, tc);
					switch((id.array_sampler >> (s << 2)) & 0xf)
					{
					case GfxEnum::TEXTARGET_2D:
						source += FORMATA("tex2D(sampler_%d, in_tc_%d);\n", s, tc);
						break;
					case GfxEnum::TEXTARGET_3D:
						source += FORMATA("tex3D(sampler_%d, in_tc_%d);\n", s, tc);
						break;
					case GfxEnum::TEXTARGET_CUBE:
						source += FORMATA("texCUBE(sampler_%d, in_tc_%d);\n", s, tc);
						break;
					default:
						Z_ASSERT( false );
					}
				}
				source += "\n\n";
				source += cfg.getPSCode();
				source += "\n\n";
				source += "}\n";
			}
			else
			{
				if(!id.enable.specular)				// non specular lighting code
				{
					if(id.lighting.dir)
						source += _code_lit_dir;					// directional
					if(id.lighting.point)
						source += _code_lit_point;					// point
					if(id.lighting.point_range)
						source += _code_lit_point_range;			// ranged point
					if(id.lighting.spot)
						source += _code_lit_spot;					// spot
					if(id.lighting.spot_range)
						source += _code_lit_spot_range;				// ranged spot
				}
				else									// specular lighting code
				{
					if(id.lighting.dir)
						source += _code_lit_dir_spec;				// directional
					if(id.lighting.point)
						source += _code_lit_point_spec;				// point
					if(id.lighting.point_range)
						source += _code_lit_point_spec_range;		// ranged point
					if(id.lighting.spot)
						source += _code_lit_spot_spec;				// spot
					if(id.lighting.spot_range)
						source += _code_lit_spot_spec_range;		// ranged spot
				}

				// uniforms
				if((!id.enable.normal_mapping && id.enable.specular) || (id.enable.normal_mapping && (id.enable.specular || id.enable.cube_mapping)))
					source += "float3 cam_pos;\n";
				if(id.enable.specular)
					source += "float shin;\n";
				source += "\
float opac;\n\
float3 ambient;\n\
";

				if(id.enable.specular)
				{
					size_t lights = 0;
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_dir;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
",
								 lights, lights, lights);

					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
",
								 lights, lights, lights);

					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
",
								 lights, lights, lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
float3 l%d_spec;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights, lights, lights, lights);

				}
				else
				{
					size_t lights = 0;
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_dir;\n\
float3 l%d_diff;\n\
",
								 lights, lights);

					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
",
								 lights, lights);

					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_diff;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
",
								 lights, lights, lights, lights);

					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						source +=
							FORMATA(
							"\
float3 l%d_pos;\n\
float3 l%d_dir;\n\
float2 l%d_cone;\n\
float3 l%d_diff;\n\
float2 l%d_range;\n\
",
								 lights, lights, lights, lights, lights);

				}

				source += "\n";

				if(true)
				{
					ARRAY_SAMPLER s = id.array_sampler;
					size_t i = 0;
					while(s)
					{
						switch(s & 0xf)
						{
						case GfxEnum::TEXTARGET_2D:
							source += FORMATA("sampler2D sampler_%d: register(s%d);\n", i, i);
							break;
						case GfxEnum::TEXTARGET_3D:
							source += FORMATA("sampler3D sampler_%d: register(s%d);\n", i, i);
							break;
						case GfxEnum::TEXTARGET_CUBE:
							source += FORMATA("samplerCUBE sampler_%d: register(s%d);\n", i, i);
							break;
						}
						s >>= 4;
						++i;
					}
				}

				if(id.enable.normal_mapping)
					source += "sampler2D sampler_nm: register(s8);\n";

				if(id.lights_visibility != 0xffffffff)
					source += "sampler2D sampler_light_vis: register(s9);\n";

				source += "\n";

				// maybe adding uniforms??

				source += "\
void main(\n\
	out float4 out_color: COLOR,\n\
";
				if(cfg.lights)
					source += "	float3 in_world_pos: TEXCOORD8,\n";
				if((!id.enable.normal_mapping && cfg.lights) || id.enable.normal_mapping)
					source += "	float3 in_world_nor: TEXCOORD9,\n";

				if(true)
				{
					ARRAY_TC tc = id.array_tc;
					size_t i = 0;
					while(tc)
					{
						switch(tc & 0xf)
						{
						case ShaderGenConf::TCGEN_INTACT:
							source += FORMATA("	float4 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_XFORM:
							source += FORMATA("	float4 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							source += FORMATA("	float2 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						case ShaderGenConf::TCGEN_CUBE_MAPPING:
							if(!id.enable.normal_mapping)
								source += FORMATA("	float3 in_tc_%d: TEXCOORD%d,\n", i, i);
							break;
						}
						tc >>= 4;
						++i;
					}
				}
				if(id.enable.normal_mapping)
					source += "\
	float4 in_nm_tangent: TEXCOORD10,\n\
	float4 in_tc_nm: TEXCOORD11,\n\
";
				if(id.lights_visibility != 0xffffffff)
					source += "\
	float4 in_light_vis_tc: TEXCOORD12,\n\
";
				if(id.enable.two_sided_lighting)
					source += "\
	float facing: VFACE,\n\
";
				source += "\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
";
				if(id.enable.normal_mapping)
					source += "\
	in_world_nor = normalize(in_world_nor);\n\
	in_nm_tangent.xyz = normalize(in_nm_tangent.xyz);\n\
	float3 in_nm_bitangent = normalize(in_nm_tangent.w * cross(in_world_nor, in_nm_tangent.xyz));\n\
	in_world_nor = mul((tex2D(sampler_nm, in_tc_nm).xyz - 0.5) * float3(2, -2, 2), float3x3(in_nm_tangent.xyz, in_nm_bitangent, in_world_nor));\n\
";
				source += "\
	float4 in_diff = float4(ambient, opac);\n\
	float3 in_spec = float3(0.0, 0.0, 0.0);\n\n\
";

				if(id.enable.two_sided_lighting && ((!id.enable.normal_mapping && cfg.lights) || id.enable.normal_mapping))
					source += "	in_world_nor *= -sign(facing);\n\n";

				if((!id.enable.normal_mapping && cfg.lights) || id.enable.normal_mapping)
					source += "	in_world_nor = normalize(in_world_nor);\n\n";

				if(id.lights_visibility != 0xffffffff)
				{
					source += " float4 lights_vis = tex2D(sampler_light_vis, (in_light_vis_tc / in_light_vis_tc.w));\n\n";
					// more if supporting > 4 dynamic lights casting shadow
				}

				if(!id.enable.specular)
				{
					size_t lights = 0;
					size_t ch = -1;

					// dir ...
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_dir(in_world_nor, l%d_dir, l%d_diff, in_diff.xyz);\n\n", lights, lights);
						else
							source += FORMATA("	lit_dir(in_world_nor, l%d_dir, l%d_diff * lights_vis[%d], in_diff.xyz);\n\n", lights, lights, ch);

					// point ...
					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_point(in_world_pos, in_world_nor, l%d_pos, l%d_diff, in_diff.xyz);\n\n", lights, lights);
						else
							source += FORMATA("	lit_point(in_world_pos, in_world_nor, l%d_pos, l%d_diff * lights_vis[%d], in_diff.xyz);\n\n", lights, lights, ch);

					// point range ...
					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_point_range(in_world_pos, in_world_nor, l%d_pos, l%d_diff, l%d_range, in_diff.xyz);\n\n", lights, lights, lights);
						else
							source += FORMATA("	lit_point_range(in_world_pos, in_world_nor, l%d_pos, l%d_diff * lights_vis[%d], l%d_range, in_diff.xyz);\n\n", lights, lights, ch, lights);

					// spot ...
					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_spot(in_world_pos, in_world_nor, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, in_diff.xyz);\n\n", lights, lights, lights, lights, lights);
						else
							source += FORMATA("	lit_spot(in_world_pos, in_world_nor, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff * lights_vis[%d], in_diff.xyz);\n\n", lights, lights, lights, lights, lights, ch);

					// spot range ...
					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_spot_range(in_world_pos, in_world_nor, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, l%d_range, in_diff.xyz);\n\n", lights, lights, lights, lights, lights, lights);
						else
							source += FORMATA("	lit_spot_range(in_world_pos, in_world_nor, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff * lights_vis[%d], l%d_range, in_diff.xyz);\n\n", lights, lights, lights, lights, lights, ch, lights);
				}
				else
				{
					size_t lights = 0;
					size_t ch = -1;

					// dir ...
					for(size_t i = 0; i < id.lighting.dir; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_dir_spec(in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_dir, l%d_diff, l%d_spec, in_diff.xyz, in_spec.xyz);\n\n", lights, lights, lights);
						else
							source += FORMATA("	lit_dir_spec(in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_dir, l%d_diff * lights_vis[%d], l%d_spec * lights_vis[%d], in_diff.xyz, in_spec.xyz);\n\n", lights, lights, ch, lights, ch);

					// point ...
					for(size_t i = 0; i < id.lighting.point; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_point_spec(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_diff, l%d_spec, in_diff.xyz, in_spec.xyz);\n\n", lights, lights, lights);
						else
							source += FORMATA("	lit_point_spec(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_diff * lights_vis[%d], l%d_spec * lights_vis[%d], in_diff.xyz, in_spec.xyz);\n\n", lights, lights, ch, lights, ch);

					// point range ...
					for(size_t i = 0; i < id.lighting.point_range; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_point_spec_range(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_diff, l%d_spec, l%d_range, in_diff.xyz, in_spec.xyz);\n\n", lights, lights, lights, lights);
						else
							source += FORMATA("	lit_point_spec_range(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_diff * lights_vis[%d], l%d_spec * lights_vis[%d], l%d_range, in_diff.xyz, in_spec.xyz);\n\n", lights, lights, ch, lights, ch, lights);

					// spot ...
					for(size_t i = 0; i < id.lighting.spot; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_spot_spec(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, l%d_spec, in_diff.xyz, in_spec.xyz);\n\n", lights, lights, lights, lights, lights, lights);
						else
							source += FORMATA("	lit_spot_spec(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff * lights_vis[%d], l%d_spec * lights_vis[%d], in_diff.xyz, in_spec.xyz);\n\n", lights, lights, lights, lights, lights, ch, lights, ch);

					// spot range ...
					for(size_t i = 0; i < id.lighting.spot_range; ++i, ++lights)
						if((ch = (id.lights_visibility >> (lights << 2)) & 0x0f) == 0x0f)
							source += FORMATA("	lit_spot_spec_range(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff, l%d_spec, l%d_range, in_diff.xyz, in_spec.xyz);\n\n", lights, lights, lights, lights, lights, lights, lights);
						else
							source += FORMATA("	lit_spot_spec_range(in_world_pos, in_world_nor, normalize(cam_pos - in_world_pos), shin, l%d_pos, l%d_dir, l%d_cone[0], l%d_cone[1], l%d_diff * lights_vis[%d], l%d_spec * lights_vis[%d], l%d_range, in_diff.xyz, in_spec.xyz);\n\n", lights, lights, lights, lights, lights, ch, lights, ch, lights);
				}

				for(size_t i = 0; i < id.tex_fetches; ++i)
				{
					uint8_t tf = id.array_tex_fetch[i];
					uint8_t tc = tf & 0xf;
					uint8_t s = (tf >> 4);
					if(id.enable.normal_mapping && (((id.array_tc >> (tc << 2)) & 0xf) == ShaderGenConf::TCGEN_CUBE_MAPPING))
						source += FORMATA("	float3 in_tc_%d = reflect(normalize(in_world_pos - cam_pos), in_world_nor);", tc);
					source += FORMATA("	float4 tf_%d_%d = ", s, tc);
					switch((id.array_sampler >> (s << 2)) & 0xf)
					{
					case GfxEnum::TEXTARGET_2D:
						source += FORMATA("tex2D(sampler_%d, in_tc_%d);\n", s, tc);
						break;
					case GfxEnum::TEXTARGET_3D:
						source += FORMATA("tex3D(sampler_%d, in_tc_%d);\n", s, tc);
						break;
					case GfxEnum::TEXTARGET_CUBE:
						source += FORMATA("texCUBE(sampler_%d, in_tc_%d);\n", s, tc);
						break;
					default:
						Z_ASSERT( false );
					}
				}
				source += "\n\n";
				source += cfg.getPSCode();
				source += "\n\n";
				source += "}\n";
			}

			SPtr<Shader> shader = _gfx->createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, source);
			if(!shader)
			{
				FILE* fp = NULL;
				fopen_s(&fp, "problem_ps.txt", "w");
				if(fp)
				{
					fwrite(source.c_str(), source.length(), 1, fp);
					fclose(fp);
				}
				Z_ASSERT( false );
				return NULL;
			}
			if(false)
			{
				FILE* fp = NULL;
				fopen_s(&fp, "problem_ps.txt", "w");
				if(fp)
				{
					fwrite(source.c_str(), source.length(), 1, fp);
					fclose(fp);
				}
			}
			_map_id_ps[id][cfg.getPSCode()] = shader.cast<d3dShader>();

			return shader;
		}
		void					d3dShaderGen::setShaderUniforms(const ShaderGenConf& cfg, ShaderUniforms& unis, bool vs, bool ps)
		{
			d3dGfx* gfx = _gfx;
			if(!cfg.enable.per_pixel_lighting)
			{
				if(vs)
				{
					VSID id = genVSID(cfg);
					//unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", (*(Mat4*)cfg.world) * (*(Mat4*)cfg.view) * gfx->adjustProjectionTransform((*(Mat4*)cfg.unadjusted_proj)));
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", (*(Mat4*)cfg.world) * gfx->getAdjustedViewProjectionTransform());
					if(id.enable.specular || id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_pos", Vec3(gfx->getInverseViewTransform()[3]));
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "ambient", (*(Vec3*)cfg.material.ambient).modulate(*(Vec3*)cfg.light_ambient));
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "opac", cfg.material.opacity);

					if(cfg.lights)
					{
						if(id.enable.specular)
							unis.set(GfxEnum::SHADERTYPE_VERTEX, "shin", cfg.material.shininess);
					}

					if(id.enable.specular || (id.lighting.point || id.lighting.point_range || id.lighting.spot || id.lighting.spot_range) || id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world", (*(Mat4*)cfg.world));
					if(cfg.lights || id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world_nm", ((Mat3)(*(Mat4*)cfg.world)).inverse().transpose());

					if(cfg.lights)
					{
						size_t dir_off = 0;
						size_t point_off = dir_off + id.lighting.dir;
						size_t point_range_off = point_off + id.lighting.point;
						size_t spot_off = point_range_off + id.lighting.point_range;
						size_t spot_range_off = spot_off + id.lighting.spot;

						char name[32];
						Vec2 cone;
						Vec2 range;

						for(size_t i = 0; i < cfg.lights; ++i)
						{
							size_t off;
							const ShaderGenConf::LIGHT& l = cfg.array_light[i];
							switch(l.type)
							{
							case GfxEnum::LIGHTTYPE_DIRECTIONAL:
								sprintf(name, "l%d_dir", dir_off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_diff", dir_off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", dir_off);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								++dir_off;
								break;
							case GfxEnum::LIGHTTYPE_POINT:
								if(l.range == INFINITY)
									off = point_off;
								else
									off = point_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, range);
								}
								if(l.range == INFINITY)
									++point_off;
								else
									++point_range_off;
								break;
							case GfxEnum::LIGHTTYPE_SPOT:
								if(l.range == INFINITY)
									off = spot_off;
								else
									off = spot_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_dir", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_cone", off);
								cone[0] = COS(l.inner_cone * 0.5f);
								cone[1] = COS(l.outer_cone * 0.5f);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, cone);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, range);
								}
								if(l.range == INFINITY)
									++spot_off;
								else
									++spot_range_off;
								break;
							}
						}
					}
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS; ++i)
					{
						char name[32];
						switch(cfg.array_tc[i])
						{
						case ShaderGenConf::TCGEN_XFORM:
							sprintf(name, "tc_xform_%d", i);
							unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Mat4*)cfg.array_tc_xform[i]);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							{
								Mat4 world_view = (*(Mat4*)cfg.world) * gfx->getViewTransform();
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv", world_view);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv_nm", ((Mat3)world_view).inverse().transpose());
							}
							break;
						}
					}
				}
			}
			else
			{
				if(vs)
				{
					VSID id = genVSID(cfg);
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", (*(Mat4*)cfg.world) * gfx->getAdjustedViewProjectionTransform());
					if(id.enable.lights_visibility)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "light_vis_space_xform", (*(Mat4*)cfg.world) * gfx->getUnadjustedViewProjectionTransform() * Mat4::scaling(0.5f, -0.5f, 1) * Mat4::translation(0.5f, 0.5f, 0));
					if(!id.enable.normal_mapping && id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_pos", Vec3(gfx->getInverseViewTransform()[3]));

					if((!id.enable.normal_mapping && (cfg.lights || id.enable.cube_mapping)) || id.enable.normal_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world", (*(Mat4*)cfg.world));
					if((!id.enable.normal_mapping && (cfg.lights || id.enable.cube_mapping)) || id.enable.normal_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world_nm", ((Mat3)(*(Mat4*)cfg.world)).inverse().transpose());
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS; ++i)
					{
						char name[32];
						switch(cfg.array_tc[i])
						{
						case ShaderGenConf::TCGEN_XFORM:
							sprintf(name, "tc_xform_%d", i);
							unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Mat4*)cfg.array_tc_xform[i]);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							{
								Mat4 world_view = (*(Mat4*)cfg.world) * gfx->getViewTransform();
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv", world_view);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv_nm", ((Mat3)world_view).inverse().transpose());
							}
							break;
						}
					}
					if(id.enable.normal_mapping == 2)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "tc_xform_nm", *(Mat4*)cfg.nm_tc_xform);
				}
				if(ps)
				{
					PSID id = genPSID(cfg);
					if((!id.enable.normal_mapping && id.enable.specular) || (id.enable.normal_mapping && (id.enable.specular || id.enable.cube_mapping)))
						unis.set(GfxEnum::SHADERTYPE_PIXEL, "cam_pos", Vec3(gfx->getInverseViewTransform()[3]));
					if(id.enable.specular)
						unis.set(GfxEnum::SHADERTYPE_PIXEL, "shin", cfg.material.shininess);
					unis.set(GfxEnum::SHADERTYPE_PIXEL, "ambient", (*(Vec3*)cfg.material.ambient).modulate(*(Vec3*)cfg.light_ambient));
					unis.set(GfxEnum::SHADERTYPE_PIXEL, "opac", cfg.material.opacity);

					if(cfg.lights)
					{
						size_t dir_off = 0;
						size_t point_off = dir_off + id.lighting.dir;
						size_t point_range_off = point_off + id.lighting.point;
						size_t spot_off = point_range_off + id.lighting.point_range;
						size_t spot_range_off = spot_off + id.lighting.spot;

						char name[32];
						Vec2 cone;
						Vec2 range;

						for(size_t i = 0; i < cfg.lights; ++i)
						{
							size_t off;
							const ShaderGenConf::LIGHT& l = cfg.array_light[i];
							switch(l.type)
							{
							case GfxEnum::LIGHTTYPE_DIRECTIONAL:
								sprintf(name, "l%d_dir", dir_off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_diff", dir_off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", dir_off);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								++dir_off;
								break;
							case GfxEnum::LIGHTTYPE_POINT:
								if(l.range == INFINITY)
									off = point_off;
								else
									off = point_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, range);
								}
								if(l.range == INFINITY)
									++point_off;
								else
									++point_range_off;
								break;
							case GfxEnum::LIGHTTYPE_SPOT:
								if(l.range == INFINITY)
									off = spot_off;
								else
									off = spot_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_dir", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_cone", off);
								cone[0] = COS(l.inner_cone * 0.5f);
								cone[1] = COS(l.outer_cone * 0.5f);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, cone);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, range);
								}
								if(l.range == INFINITY)
									++spot_off;
								else
									++spot_range_off;
								break;
							}
						}
					}
				}
			}
		}
		void					d3dShaderGen::setShaderUniforms(const ShaderGenConf& cfg, ShaderUniforms2& unis, bool vs, bool ps)
		{
			d3dGfx* gfx = _gfx;
			if(!cfg.enable.per_pixel_lighting)
			{
				if(vs)
				{
					VSID id = genVSID(cfg);
					//unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", (*(Mat4*)cfg.world) * (*(Mat4*)cfg.view) * gfx->adjustProjectionTransform((*(Mat4*)cfg.unadjusted_proj)));
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", (*(Mat4*)cfg.world) * gfx->getAdjustedViewProjectionTransform());
					if(id.enable.specular || id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_pos", Vec3(gfx->getInverseViewTransform()[3]));
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "ambient", (*(Vec3*)cfg.material.ambient).modulate(*(Vec3*)cfg.light_ambient));
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "opac", cfg.material.opacity);

					if(cfg.lights)
					{
						if(id.enable.specular)
							unis.set(GfxEnum::SHADERTYPE_VERTEX, "shin", cfg.material.shininess);
					}

					if(id.enable.specular || (id.lighting.point || id.lighting.point_range || id.lighting.spot || id.lighting.spot_range) || id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world", (*(Mat4*)cfg.world));
					if(cfg.lights || id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world_nm", ((Mat3)(*(Mat4*)cfg.world)).inverse().transpose());

					if(cfg.lights)
					{
						size_t dir_off = 0;
						size_t point_off = dir_off + id.lighting.dir;
						size_t point_range_off = point_off + id.lighting.point;
						size_t spot_off = point_range_off + id.lighting.point_range;
						size_t spot_range_off = spot_off + id.lighting.spot;

						char name[32];
						Vec2 cone;
						Vec2 range;

						for(size_t i = 0; i < cfg.lights; ++i)
						{
							size_t off;
							const ShaderGenConf::LIGHT& l = cfg.array_light[i];
							switch(l.type)
							{
							case GfxEnum::LIGHTTYPE_DIRECTIONAL:
								sprintf(name, "l%d_dir", dir_off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_diff", dir_off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", dir_off);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								++dir_off;
								break;
							case GfxEnum::LIGHTTYPE_POINT:
								if(l.range == INFINITY)
									off = point_off;
								else
									off = point_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, range);
								}
								if(l.range == INFINITY)
									++point_off;
								else
									++point_range_off;
								break;
							case GfxEnum::LIGHTTYPE_SPOT:
								if(l.range == INFINITY)
									off = spot_off;
								else
									off = spot_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_dir", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_cone", off);
								cone[0] = COS(l.inner_cone * 0.5f);
								cone[1] = COS(l.outer_cone * 0.5f);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, cone);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_VERTEX, name, range);
								}
								if(l.range == INFINITY)
									++spot_off;
								else
									++spot_range_off;
								break;
							}
						}
					}
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS; ++i)
					{
						char name[32];
						switch(cfg.array_tc[i])
						{
						case ShaderGenConf::TCGEN_XFORM:
							sprintf(name, "tc_xform_%d", i);
							unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Mat4*)cfg.array_tc_xform[i]);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							{
								Mat4 world_view = (*(Mat4*)cfg.world) * gfx->getViewTransform();
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv", world_view);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv_nm", ((Mat3)world_view).inverse().transpose());
							}
							break;
						}
					}
				}
			}
			else
			{
				if(vs)
				{
					VSID id = genVSID(cfg);
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", (*(Mat4*)cfg.world) * gfx->getAdjustedViewProjectionTransform());
					if(id.enable.lights_visibility)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "light_vis_space_xform", (*(Mat4*)cfg.world) * gfx->getUnadjustedViewProjectionTransform() * Mat4::scaling(0.5f, -0.5f, 1) * Mat4::translation(0.5f, 0.5f, 0));
					if(!id.enable.normal_mapping && id.enable.cube_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_pos", Vec3(gfx->getInverseViewTransform()[3]));

					if((!id.enable.normal_mapping && (cfg.lights || id.enable.cube_mapping)) || id.enable.normal_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world", (*(Mat4*)cfg.world));
					if((!id.enable.normal_mapping && (cfg.lights || id.enable.cube_mapping)) || id.enable.normal_mapping)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "world_nm", ((Mat3)(*(Mat4*)cfg.world)).inverse().transpose());
					for(size_t i = 0; i < ShaderGenConf::MAX_TEXCOORD_SETS; ++i)
					{
						char name[32];
						switch(cfg.array_tc[i])
						{
						case ShaderGenConf::TCGEN_XFORM:
							sprintf(name, "tc_xform_%d", i);
							unis.set(GfxEnum::SHADERTYPE_VERTEX, name, *(Mat4*)cfg.array_tc_xform[i]);
							break;
						case ShaderGenConf::TCGEN_SPHERE_MAPPING:
							{
								Mat4 world_view = (*(Mat4*)cfg.world) * gfx->getViewTransform();
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv", world_view);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wv_nm", ((Mat3)world_view).inverse().transpose());
							}
							break;
						}
					}
					if(id.enable.normal_mapping == 2)
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "tc_xform_nm", *(Mat4*)cfg.nm_tc_xform);
				}
				if(ps)
				{
					PSID id = genPSID(cfg);
					if((!id.enable.normal_mapping && id.enable.specular) || (id.enable.normal_mapping && (id.enable.specular || id.enable.cube_mapping)))
						unis.set(GfxEnum::SHADERTYPE_PIXEL, "cam_pos", Vec3(gfx->getInverseViewTransform()[3]));
					if(id.enable.specular)
						unis.set(GfxEnum::SHADERTYPE_PIXEL, "shin", cfg.material.shininess);
					unis.set(GfxEnum::SHADERTYPE_PIXEL, "ambient", (*(Vec3*)cfg.material.ambient).modulate(*(Vec3*)cfg.light_ambient));
					unis.set(GfxEnum::SHADERTYPE_PIXEL, "opac", cfg.material.opacity);

					if(cfg.lights)
					{
						size_t dir_off = 0;
						size_t point_off = dir_off + id.lighting.dir;
						size_t point_range_off = point_off + id.lighting.point;
						size_t spot_off = point_range_off + id.lighting.point_range;
						size_t spot_range_off = spot_off + id.lighting.spot;

						char name[32];
						Vec2 cone;
						Vec2 range;

						for(size_t i = 0; i < cfg.lights; ++i)
						{
							size_t off;
							const ShaderGenConf::LIGHT& l = cfg.array_light[i];
							switch(l.type)
							{
							case GfxEnum::LIGHTTYPE_DIRECTIONAL:
								sprintf(name, "l%d_dir", dir_off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_diff", dir_off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", dir_off);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								++dir_off;
								break;
							case GfxEnum::LIGHTTYPE_POINT:
								if(l.range == INFINITY)
									off = point_off;
								else
									off = point_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, range);
								}
								if(l.range == INFINITY)
									++point_off;
								else
									++point_range_off;
								break;
							case GfxEnum::LIGHTTYPE_SPOT:
								if(l.range == INFINITY)
									off = spot_off;
								else
									off = spot_range_off;
								sprintf(name, "l%d_pos", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.pos);
								sprintf(name, "l%d_dir", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, *(Vec3*)l.dir);
								sprintf(name, "l%d_cone", off);
								cone[0] = COS(l.inner_cone * 0.5f);
								cone[1] = COS(l.outer_cone * 0.5f);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, cone);
								sprintf(name, "l%d_diff", off);
								unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.diffuse));
								if(id.enable.specular)
								{
									sprintf(name, "l%d_spec", off);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, (*(Vec3*)l.color).modulate(*(Vec3*)cfg.material.specular));
								}
								if(l.range != INFINITY)
								{
									sprintf(name, "l%d_range", off);
									range[0] = 1.0f / l.range;
									range[1] = 1.0f / (l.range * l.range);
									unis.set(GfxEnum::SHADERTYPE_PIXEL, name, range);
								}
								if(l.range == INFINITY)
									++spot_off;
								else
									++spot_range_off;
								break;
							}
						}
					}
				}
			}
		}
		void					d3dShaderGen::clear()
		{
			_map_id_vs.clear();
			_map_id_ps.clear();
		}
	};
};

