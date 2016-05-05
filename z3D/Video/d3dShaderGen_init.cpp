
#include "d3dShaderGen.h"

namespace z3D
{
	namespace Video
	{
		mbs						d3dShaderGen::getFunc(const mbs& source, const mbs& id)
		{
			size_t start = source.find(id);
			Z_ASSERT( start != mbs::npos );
			start += id.length();
			size_t end = source.find(id, start);
			Z_ASSERT( end != mbs::npos );

			return source.substr(start, end - start);
		}
		void					d3dShaderGen::initShaderFragments()
		{
			mbs source = 
"\
\n\
// ### lit_dir ###\n\
\n\
void lit_dir(\n\
	float3 in_nor,\n\
	float3 l_dir,\n\
	float3 l_diff,\n\
	inout float3 out_diff\n\
	)\n\
{\n\
	float diff_int;\n\
	float3 dir;\n\
\n\
	diff_int = dot(in_nor, dir = -l_dir);\n\
	out_diff += saturate(diff_int) * l_diff;\n\
}\n\
\n\
// ### lit_dir ###\n\
\n\
// ### lit_dir_spec ###\n\
\n\
void lit_dir_spec(\n\
	float3 in_nor,\n\
	float3 cam_vec,\n\
	float shin,\n\
	float3 l_dir,\n\
	float3 l_diff,\n\
	float3 l_spec,\n\
	inout float3 out_diff,\n\
	inout float3 out_spec\n\
	)\n\
{\n\
	float diff_int, spec_int;\n\
	float3 dir;\n\
	float3 half_vec;\n\
\n\
	diff_int = dot(in_nor, dir = -l_dir);\n\
	if(diff_int > 0.0)\n\
	{\n\
		out_diff += l_diff * diff_int;\n\
		half_vec = normalize(cam_vec + dir);\n\
		spec_int = dot(in_nor, half_vec);\n\
		out_spec += l_spec * pow(saturate(spec_int), shin);\n\
	}\n\
}\n\
\n\
// ### lit_dir_spec ###\n\
\n\
// ### lit_point ###\n\
\n\
void lit_point(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 l_pos,\n\
	float3 l_diff,\n\
	inout float3 out_diff\n\
	)\n\
{\n\
	float diff_int;\n\
	float3 dir;\n\
\n\
	diff_int = dot(in_nor, dir = normalize(l_pos - in_pos));\n\
	out_diff += saturate(diff_int) * l_diff;\n\
}\n\
\n\
// ### lit_point ###\n\
\n\
// ### lit_point_spec ###\n\
\n\
void lit_point_spec(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 cam_vec,\n\
	float shin,\n\
	float3 l_pos,\n\
	float3 l_diff,\n\
	float3 l_spec,\n\
	inout float3 out_diff,\n\
	inout float3 out_spec\n\
	)\n\
{\n\
	float diff_int, spec_int;\n\
	float3 dir;\n\
	float3 half_vec;\n\
\n\
	diff_int = dot(in_nor, dir = normalize(l_pos - in_pos));\n\
	if(diff_int > 0.0)\n\
	{\n\
		out_diff += l_diff * diff_int;\n\
		half_vec = normalize(cam_vec + dir);\n\
		spec_int = dot(in_nor, half_vec);\n\
		out_spec += l_spec * pow(saturate(spec_int), shin);\n\
	}\n\
}\n\
\n\
// ### lit_point_spec ###\n\
\n\
// ### lit_point_range ###\n\
\n\
void lit_point_range(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 l_pos,\n\
	float3 l_diff,\n\
	float2 l_range,\n\
	inout float3 out_diff\n\
	)\n\
{\n\
	float diff_int;\n\
	float3 dir;\n\
\n\
	diff_int = dot(in_nor, dir = normalize(l_pos - in_pos));\n\
	if(diff_int > 0.0)\n\
	{\n\
		float att = 1 - saturate(distance(l_pos, in_pos) * l_range[0]);\n\
\n\
		out_diff += diff_int * l_diff * att;\n\
	}\n\
}\n\
\n\
// ### lit_point_range ###\n\
\n\
// ### lit_point_spec_range ###\n\
\n\
void lit_point_spec_range(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 cam_vec,\n\
	float shin,\n\
	float3 l_pos,\n\
	float3 l_diff,\n\
	float3 l_spec,\n\
	float2 l_range,\n\
	inout float3 out_diff,\n\
	inout float3 out_spec\n\
	)\n\
{\n\
	float diff_int, spec_int;\n\
	float3 dir;\n\
	float3 half_vec;\n\
\n\
	diff_int = dot(in_nor, dir = normalize(l_pos - in_pos));\n\
	if(diff_int > 0.0)\n\
	{\n\
		float att = 1 - saturate(distance(l_pos, in_pos) * l_range[0]);\n\
\n\
		out_diff += l_diff * diff_int * att;\n\
		half_vec = normalize(cam_vec + dir);\n\
		spec_int = dot(in_nor, half_vec);\n\
		out_spec += l_spec * pow(saturate(spec_int), shin) * att;\n\
	}\n\
}\n\
\n\
// ### lit_point_spec_range ###\n\
\n\
// ### lit_spot ###\n\
\n\
void lit_spot(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 l_pos,\n\
	float3 l_dir,\n\
	float inner_cone,\n\
	float outer_cone,\n\
	float3 l_diff,\n\
	inout float3 out_diff\n\
	)\n\
{\n\
	float diff_int, angle, spot_eff;\n\
	float3 dir;\n\
\n\
	diff_int = dot(in_nor, dir = -l_dir);\n\
	if(diff_int > 0.0)\n\
	{\n\
		angle = dot(normalize(in_pos - l_pos), l_dir);\n\
		spot_eff = smoothstep(outer_cone, inner_cone, angle);\n\
		out_diff += l_diff * diff_int * spot_eff;\n\
	}\n\
}\n\
\n\
// ### lit_spot ###\n\
\n\
// ### lit_spot_spec ###\n\
\n\
void lit_spot_spec(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 cam_vec,\n\
	float shin,\n\
	float3 l_pos,\n\
	float3 l_dir,\n\
	float inner_cone,\n\
	float outer_cone,\n\
	float3 l_diff,\n\
	float3 l_spec,\n\
	inout float3 out_diff,\n\
	inout float3 out_spec\n\
	)\n\
{\n\
	float diff_int, spec_int, angle, spot_eff;\n\
	float3 dir;\n\
	float3 half_vec;\n\
\n\
	diff_int = dot(in_nor, dir = -l_dir);\n\
	if(diff_int > 0.0)\n\
	{\n\
		angle = dot(normalize(in_pos - l_pos), l_dir);\n\
		spot_eff = smoothstep(outer_cone, inner_cone, angle);\n\
		out_diff += l_diff * diff_int * spot_eff;\n\
		half_vec = normalize(cam_vec + dir);\n\
		spec_int = dot(in_nor, half_vec);\n\
		out_spec += l_spec * pow(saturate(spec_int), shin);\n\
	}\n\
}\n\
\n\
// ### lit_spot_spec ###\n\
\n\
// ### lit_spot_range ###\n\
\n\
void lit_spot_range(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 l_pos,\n\
	float3 l_dir,\n\
	float inner_cone,\n\
	float outer_cone,\n\
	float3 l_diff,\n\
	float2 l_range,\n\
	inout float3 out_diff\n\
	)\n\
{\n\
	float diff_int, angle, spot_eff;\n\
	float3 dir;\n\
\n\
	diff_int = dot(in_nor, dir = -l_dir);\n\
	if(diff_int > 0.0)\n\
	{\n\
		angle = dot(normalize(in_pos - l_pos), l_dir);\n\
		if(angle > outer_cone)\n\
		{\n\
			float att = 1 - saturate(distance(in_pos, l_pos) * l_range[0]);\n\
\n\
			spot_eff = smoothstep(outer_cone, inner_cone, angle);\n\
			out_diff += l_diff * diff_int * spot_eff * att;\n\
		}\n\
	}\n\
}\n\
\n\
// ### lit_spot_range ###\n\
\n\
// ### lit_spot_spec_range ###\n\
\n\
void lit_spot_spec_range(\n\
	float3 in_pos,\n\
	float3 in_nor,\n\
	float3 cam_vec,\n\
	float shin,\n\
	float3 l_pos,\n\
	float3 l_dir,\n\
	float inner_cone,\n\
	float outer_cone,\n\
	float3 l_diff,\n\
	float3 l_spec,\n\
	float2 l_range,\n\
	inout float3 out_diff,\n\
	inout float3 out_spec\n\
	)\n\
{\n\
	float diff_int, spec_int, angle, spot_eff;\n\
	float3 dir;\n\
	float3 half_vec;\n\
\n\
	diff_int = dot(in_nor, dir = -l_dir);\n\
	if(diff_int > 0.0)\n\
	{\n\
		angle = dot(normalize(in_pos - l_pos), l_dir);\n\
		if(angle > outer_cone)\n\
		{\n\
			float att = 1 - saturate(distance(in_pos, l_pos) * l_range[0]);\n\
\n\
			spot_eff = smoothstep(outer_cone, inner_cone, angle);\n\
			out_diff += l_diff * diff_int * spot_eff * att;\n\
			half_vec = normalize(cam_vec + dir);\n\
			spec_int = dot(in_nor, half_vec);\n\
			out_spec += l_spec * pow(saturate(spec_int), shin) * att;\n\
		}\n\
	}\n\
}\n\
\n\
// ### lit_spot_spec_range ###\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	float3 in_nor: NORMAL,\n\
	out float4 out_pos: POSITION,\n\
	out float4 out_diff: COLOR0,\n\
	out float4 out_spec: COLOR1,			// actually float3 is correct, using float4 makes it compatible with intel chipset\n\
	uniform float4x4 wvp,\n\
	uniform float4x4 world,\n\
	uniform float3x3 world_nm,\n\
	uniform float3 cam_pos,\n\
	uniform float shin,\n\
	uniform float opac,\n\
	uniform float3 ambient,\n\
	uniform float3 l0_dir,\n\
	uniform float3 l0_diff,\n\
	uniform float3 l0_spec,\n\
	uniform float3 l1_pos,\n\
	uniform float3 l1_diff,\n\
	uniform float3 l1_spec,\n\
	uniform float2 l1_range,\n\
	uniform float3 l2_pos,\n\
	uniform float3 l2_dir,\n\
	uniform float2 l2_cone,\n\
	uniform float3 l2_diff,\n\
	uniform float3 l2_spec,\n\
	uniform float4x4 wv,\n\
	uniform float3x3 wv_nm,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	out float4 out_tc_0: TEXCOORD0,\n\
	float4 in_tc_1: TEXCOORD1,\n\
	out float4 out_tc_1: TEXCOORD1,\n\
	uniform float4x4 xform_tc_1,\n\
	out float2 out_tc_2: TEXCOORD2,\n\
	out float3 out_tc_3: TEXCOORD3,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
\n\
	float3 world_pos = mul(in_pos, world).xyz;\n\
	float3 world_nor = normalize(mul(in_nor, world_nm));\n\
\n\
	float3 cam_vec = normalize(cam_pos - world_pos.xyz);\n\
\n\
	out_diff = float4(ambient, opac);\n\
	out_spec = float4(0.0, 0.0, 0.0, 0.0);\n\
\n\
	lit_dir_spec(world_nor, cam_vec, shin, l0_dir, l0_diff, l0_spec, out_diff.xyz, out_spec.xyz);\n\
\n\
	//lit_point_spec(world_pos, world_nor, cam_vec, shin, l1_pos, l1_diff, l1_spec, out_diff.xyz, out_spec.xyz);\n\
	lit_point_spec_range(world_pos, world_nor, cam_vec, shin, l1_pos, l1_diff, l1_spec, l1_range, out_diff.xyz, out_spec.xyz);\n\
\n\
	lit_spot_spec(world_pos, world_nor, cam_vec, shin, l2_pos, l2_dir, l2_cone[0], l2_cone[1], l2_diff, l2_spec, out_diff.xyz, out_spec.xyz);\n\
\n\
	out_tc_0 = in_tc_0;\n\
	out_tc_1 = mul(in_tc_1, xform_tc_1);\n\
	if(true)\n\
	{\n\
		float3 tc = reflect(normalize(mul(in_pos, wv).xyz), normalize(mul(in_nor, wv_nm)));\n\
		tc.z += 1.0;\n\
		out_tc_2 = normalize(tc).xy * 0.5 + float2(0.5, 0.5);\n\
	}\n\
	out_tc_3 = reflect(-cam_vec, world_nor);\n\
}\
";

			_code_lit_dir = getFunc(source, "// ### lit_dir ###");
			_code_lit_dir_spec = getFunc(source, "// ### lit_dir_spec ###");
			_code_lit_point = getFunc(source, "// ### lit_point ###");
			_code_lit_point_spec = getFunc(source, "// ### lit_point_spec ###");
			_code_lit_point_range = getFunc(source, "// ### lit_point_range ###");
			_code_lit_point_spec_range = getFunc(source, "// ### lit_point_spec_range ###");
			_code_lit_spot = getFunc(source, "// ### lit_spot ###");
			_code_lit_spot_spec = getFunc(source, "// ### lit_spot_spec ###");
			_code_lit_spot_range = getFunc(source, "// ### lit_spot_range ###");
			_code_lit_spot_spec_range = getFunc(source, "// ### lit_spot_spec_range ###");
		}
	};
};

