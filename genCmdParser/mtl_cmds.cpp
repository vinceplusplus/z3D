int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"resize_mtls"]=++cmd_no;
cmd_map[L"set_mtl_render_type"]=++cmd_no;
cmd_map[L"set_mtl_render_method"]=++cmd_no;
cmd_map[L"set_mtl_ambient"]=++cmd_no;
cmd_map[L"set_mtl_diffuse"]=++cmd_no;
cmd_map[L"set_mtl_specular"]=++cmd_no;
cmd_map[L"set_mtl_specular_level"]=++cmd_no;
cmd_map[L"set_mtl_glossiness"]=++cmd_no;
cmd_map[L"set_mtl_two_side"]=++cmd_no;
cmd_map[L"set_mtl_blend_type"]=++cmd_no;
cmd_map[L"set_mtl_opacity"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map"]=++cmd_no;
cmd_map[L"set_mtl_normal_map"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map"]=++cmd_no;
cmd_map[L"set_mtl_toon_map"]=++cmd_no;
cmd_map[L"set_mtl_toon_outline"]=++cmd_no;
cmd_map[L"set_mtl_toon_outline_width"]=++cmd_no;
cmd_map[L"set_mtl_toon_outline_color"]=++cmd_no;
cmd_map[L"set_mtl_selfill"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_filter"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_u_wrap"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_v_wrap"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_filter"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_u_wrap"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_v_wrap"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_filter"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_u_wrap"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_v_wrap"]=++cmd_no;
cmd_map[L"set_mtl_reflect_amount"]=++cmd_no;
cmd_map[L"set_mtl_layer_index"]=++cmd_no;
cmd_map[L"create_am_controller"]=++cmd_no;
cmd_map[L"create_di_controller"]=++cmd_no;
cmd_map[L"create_sp_controller"]=++cmd_no;
cmd_map[L"create_sl_controller"]=++cmd_no;
cmd_map[L"create_gl_controller"]=++cmd_no;
cmd_map[L"create_op_controller"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_u_offset"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_v_offset"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_u_tiling"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_v_tiling"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_u_angle"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_v_angle"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_w_angle"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_u_offset"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_v_offset"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_u_tiling"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_v_tiling"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_u_angle"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_v_angle"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_w_angle"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_u_offset"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_v_offset"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_u_tiling"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_v_tiling"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_u_angle"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_v_angle"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_w_angle"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_u_offset_animdata"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_v_offset_animdata"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_u_tiling_animdata"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_v_tiling_animdata"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_u_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_v_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_diffuse_map_w_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_u_offset_animdata"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_v_offset_animdata"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_u_tiling_animdata"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_v_tiling_animdata"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_u_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_v_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_normal_map_w_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_u_offset_animdata"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_v_offset_animdata"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_u_tiling_animdata"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_v_tiling_animdata"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_u_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_v_angle_animdata"]=++cmd_no;
cmd_map[L"set_mtl_selfill_map_w_angle_animdata"]=++cmd_no;

int cmd_resize_mtls=cmd_map[L"resize_mtls"];
int cmd_set_mtl_render_type=cmd_map[L"set_mtl_render_type"];
int cmd_set_mtl_render_method=cmd_map[L"set_mtl_render_method"];
int cmd_set_mtl_ambient=cmd_map[L"set_mtl_ambient"];
int cmd_set_mtl_diffuse=cmd_map[L"set_mtl_diffuse"];
int cmd_set_mtl_specular=cmd_map[L"set_mtl_specular"];
int cmd_set_mtl_specular_level=cmd_map[L"set_mtl_specular_level"];
int cmd_set_mtl_glossiness=cmd_map[L"set_mtl_glossiness"];
int cmd_set_mtl_two_side=cmd_map[L"set_mtl_two_side"];
int cmd_set_mtl_blend_type=cmd_map[L"set_mtl_blend_type"];
int cmd_set_mtl_opacity=cmd_map[L"set_mtl_opacity"];
int cmd_set_mtl_diffuse_map=cmd_map[L"set_mtl_diffuse_map"];
int cmd_set_mtl_normal_map=cmd_map[L"set_mtl_normal_map"];
int cmd_set_mtl_selfill_map=cmd_map[L"set_mtl_selfill_map"];
int cmd_set_mtl_toon_map=cmd_map[L"set_mtl_toon_map"];
int cmd_set_mtl_toon_outline=cmd_map[L"set_mtl_toon_outline"];
int cmd_set_mtl_toon_outline_width=cmd_map[L"set_mtl_toon_outline_width"];
int cmd_set_mtl_toon_outline_color=cmd_map[L"set_mtl_toon_outline_color"];
int cmd_set_mtl_selfill=cmd_map[L"set_mtl_selfill"];
int cmd_set_mtl_diffuse_map_filter=cmd_map[L"set_mtl_diffuse_map_filter"];
int cmd_set_mtl_diffuse_map_u_wrap=cmd_map[L"set_mtl_diffuse_map_u_wrap"];
int cmd_set_mtl_diffuse_map_v_wrap=cmd_map[L"set_mtl_diffuse_map_v_wrap"];
int cmd_set_mtl_normal_map_filter=cmd_map[L"set_mtl_normal_map_filter"];
int cmd_set_mtl_normal_map_u_wrap=cmd_map[L"set_mtl_normal_map_u_wrap"];
int cmd_set_mtl_normal_map_v_wrap=cmd_map[L"set_mtl_normal_map_v_wrap"];
int cmd_set_mtl_selfill_map_filter=cmd_map[L"set_mtl_selfill_map_filter"];
int cmd_set_mtl_selfill_map_u_wrap=cmd_map[L"set_mtl_selfill_map_u_wrap"];
int cmd_set_mtl_selfill_map_v_wrap=cmd_map[L"set_mtl_selfill_map_v_wrap"];
int cmd_set_mtl_reflect_amount=cmd_map[L"set_mtl_reflect_amount"];
int cmd_set_mtl_layer_index=cmd_map[L"set_mtl_layer_index"];
int cmd_create_am_controller=cmd_map[L"create_am_controller"];
int cmd_create_di_controller=cmd_map[L"create_di_controller"];
int cmd_create_sp_controller=cmd_map[L"create_sp_controller"];
int cmd_create_sl_controller=cmd_map[L"create_sl_controller"];
int cmd_create_gl_controller=cmd_map[L"create_gl_controller"];
int cmd_create_op_controller=cmd_map[L"create_op_controller"];
int cmd_set_mtl_diffuse_map_u_offset=cmd_map[L"set_mtl_diffuse_map_u_offset"];
int cmd_set_mtl_diffuse_map_v_offset=cmd_map[L"set_mtl_diffuse_map_v_offset"];
int cmd_set_mtl_diffuse_map_u_tiling=cmd_map[L"set_mtl_diffuse_map_u_tiling"];
int cmd_set_mtl_diffuse_map_v_tiling=cmd_map[L"set_mtl_diffuse_map_v_tiling"];
int cmd_set_mtl_diffuse_map_u_angle=cmd_map[L"set_mtl_diffuse_map_u_angle"];
int cmd_set_mtl_diffuse_map_v_angle=cmd_map[L"set_mtl_diffuse_map_v_angle"];
int cmd_set_mtl_diffuse_map_w_angle=cmd_map[L"set_mtl_diffuse_map_w_angle"];
int cmd_set_mtl_normal_map_u_offset=cmd_map[L"set_mtl_normal_map_u_offset"];
int cmd_set_mtl_normal_map_v_offset=cmd_map[L"set_mtl_normal_map_v_offset"];
int cmd_set_mtl_normal_map_u_tiling=cmd_map[L"set_mtl_normal_map_u_tiling"];
int cmd_set_mtl_normal_map_v_tiling=cmd_map[L"set_mtl_normal_map_v_tiling"];
int cmd_set_mtl_normal_map_u_angle=cmd_map[L"set_mtl_normal_map_u_angle"];
int cmd_set_mtl_normal_map_v_angle=cmd_map[L"set_mtl_normal_map_v_angle"];
int cmd_set_mtl_normal_map_w_angle=cmd_map[L"set_mtl_normal_map_w_angle"];
int cmd_set_mtl_selfill_map_u_offset=cmd_map[L"set_mtl_selfill_map_u_offset"];
int cmd_set_mtl_selfill_map_v_offset=cmd_map[L"set_mtl_selfill_map_v_offset"];
int cmd_set_mtl_selfill_map_u_tiling=cmd_map[L"set_mtl_selfill_map_u_tiling"];
int cmd_set_mtl_selfill_map_v_tiling=cmd_map[L"set_mtl_selfill_map_v_tiling"];
int cmd_set_mtl_selfill_map_u_angle=cmd_map[L"set_mtl_selfill_map_u_angle"];
int cmd_set_mtl_selfill_map_v_angle=cmd_map[L"set_mtl_selfill_map_v_angle"];
int cmd_set_mtl_selfill_map_w_angle=cmd_map[L"set_mtl_selfill_map_w_angle"];
int cmd_set_mtl_diffuse_map_u_offset_animdata=cmd_map[L"set_mtl_diffuse_map_u_offset_animdata"];
int cmd_set_mtl_diffuse_map_v_offset_animdata=cmd_map[L"set_mtl_diffuse_map_v_offset_animdata"];
int cmd_set_mtl_diffuse_map_u_tiling_animdata=cmd_map[L"set_mtl_diffuse_map_u_tiling_animdata"];
int cmd_set_mtl_diffuse_map_v_tiling_animdata=cmd_map[L"set_mtl_diffuse_map_v_tiling_animdata"];
int cmd_set_mtl_diffuse_map_u_angle_animdata=cmd_map[L"set_mtl_diffuse_map_u_angle_animdata"];
int cmd_set_mtl_diffuse_map_v_angle_animdata=cmd_map[L"set_mtl_diffuse_map_v_angle_animdata"];
int cmd_set_mtl_diffuse_map_w_angle_animdata=cmd_map[L"set_mtl_diffuse_map_w_angle_animdata"];
int cmd_set_mtl_normal_map_u_offset_animdata=cmd_map[L"set_mtl_normal_map_u_offset_animdata"];
int cmd_set_mtl_normal_map_v_offset_animdata=cmd_map[L"set_mtl_normal_map_v_offset_animdata"];
int cmd_set_mtl_normal_map_u_tiling_animdata=cmd_map[L"set_mtl_normal_map_u_tiling_animdata"];
int cmd_set_mtl_normal_map_v_tiling_animdata=cmd_map[L"set_mtl_normal_map_v_tiling_animdata"];
int cmd_set_mtl_normal_map_u_angle_animdata=cmd_map[L"set_mtl_normal_map_u_angle_animdata"];
int cmd_set_mtl_normal_map_v_angle_animdata=cmd_map[L"set_mtl_normal_map_v_angle_animdata"];
int cmd_set_mtl_normal_map_w_angle_animdata=cmd_map[L"set_mtl_normal_map_w_angle_animdata"];
int cmd_set_mtl_selfill_map_u_offset_animdata=cmd_map[L"set_mtl_selfill_map_u_offset_animdata"];
int cmd_set_mtl_selfill_map_v_offset_animdata=cmd_map[L"set_mtl_selfill_map_v_offset_animdata"];
int cmd_set_mtl_selfill_map_u_tiling_animdata=cmd_map[L"set_mtl_selfill_map_u_tiling_animdata"];
int cmd_set_mtl_selfill_map_v_tiling_animdata=cmd_map[L"set_mtl_selfill_map_v_tiling_animdata"];
int cmd_set_mtl_selfill_map_u_angle_animdata=cmd_map[L"set_mtl_selfill_map_u_angle_animdata"];
int cmd_set_mtl_selfill_map_v_angle_animdata=cmd_map[L"set_mtl_selfill_map_v_angle_animdata"];
int cmd_set_mtl_selfill_map_w_angle_animdata=cmd_map[L"set_mtl_selfill_map_w_angle_animdata"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_resize_mtls]=L"d";
cmd_param_map[cmd_set_mtl_render_type]=L"ds";
cmd_param_map[cmd_set_mtl_render_method]=L"ds";
cmd_param_map[cmd_set_mtl_ambient]=L"dfff";
cmd_param_map[cmd_set_mtl_diffuse]=L"dfff";
cmd_param_map[cmd_set_mtl_specular]=L"dfff";
cmd_param_map[cmd_set_mtl_specular_level]=L"df";
cmd_param_map[cmd_set_mtl_glossiness]=L"df";
cmd_param_map[cmd_set_mtl_two_side]=L"dd";
cmd_param_map[cmd_set_mtl_blend_type]=L"ds";
cmd_param_map[cmd_set_mtl_opacity]=L"df";
cmd_param_map[cmd_set_mtl_diffuse_map]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map]=L"ds";
cmd_param_map[cmd_set_mtl_toon_map]=L"ds";
cmd_param_map[cmd_set_mtl_toon_outline]=L"dd";
cmd_param_map[cmd_set_mtl_toon_outline_width]=L"df";
cmd_param_map[cmd_set_mtl_toon_outline_color]=L"dfff";
cmd_param_map[cmd_set_mtl_selfill]=L"df";
cmd_param_map[cmd_set_mtl_diffuse_map_filter]=L"dd";
cmd_param_map[cmd_set_mtl_diffuse_map_u_wrap]=L"dd";
cmd_param_map[cmd_set_mtl_diffuse_map_v_wrap]=L"dd";
cmd_param_map[cmd_set_mtl_normal_map_filter]=L"dd";
cmd_param_map[cmd_set_mtl_normal_map_u_wrap]=L"dd";
cmd_param_map[cmd_set_mtl_normal_map_v_wrap]=L"dd";
cmd_param_map[cmd_set_mtl_selfill_map_filter]=L"dd";
cmd_param_map[cmd_set_mtl_selfill_map_u_wrap]=L"dd";
cmd_param_map[cmd_set_mtl_selfill_map_v_wrap]=L"dd";
cmd_param_map[cmd_set_mtl_reflect_amount]=L"df";
cmd_param_map[cmd_set_mtl_layer_index]=L"dd";
cmd_param_map[cmd_create_am_controller]=L"sds";
cmd_param_map[cmd_create_di_controller]=L"sds";
cmd_param_map[cmd_create_sp_controller]=L"sds";
cmd_param_map[cmd_create_sl_controller]=L"sds";
cmd_param_map[cmd_create_gl_controller]=L"sds";
cmd_param_map[cmd_create_op_controller]=L"sds";
cmd_param_map[cmd_set_mtl_diffuse_map_u_offset]=L"dn";
cmd_param_map[cmd_set_mtl_diffuse_map_v_offset]=L"dn";
cmd_param_map[cmd_set_mtl_diffuse_map_u_tiling]=L"dn";
cmd_param_map[cmd_set_mtl_diffuse_map_v_tiling]=L"dn";
cmd_param_map[cmd_set_mtl_diffuse_map_u_angle]=L"dn";
cmd_param_map[cmd_set_mtl_diffuse_map_v_angle]=L"dn";
cmd_param_map[cmd_set_mtl_diffuse_map_w_angle]=L"dn";
cmd_param_map[cmd_set_mtl_normal_map_u_offset]=L"dn";
cmd_param_map[cmd_set_mtl_normal_map_v_offset]=L"dn";
cmd_param_map[cmd_set_mtl_normal_map_u_tiling]=L"dn";
cmd_param_map[cmd_set_mtl_normal_map_v_tiling]=L"dn";
cmd_param_map[cmd_set_mtl_normal_map_u_angle]=L"dn";
cmd_param_map[cmd_set_mtl_normal_map_v_angle]=L"dn";
cmd_param_map[cmd_set_mtl_normal_map_w_angle]=L"dn";
cmd_param_map[cmd_set_mtl_selfill_map_u_offset]=L"dn";
cmd_param_map[cmd_set_mtl_selfill_map_v_offset]=L"dn";
cmd_param_map[cmd_set_mtl_selfill_map_u_tiling]=L"dn";
cmd_param_map[cmd_set_mtl_selfill_map_v_tiling]=L"dn";
cmd_param_map[cmd_set_mtl_selfill_map_u_angle]=L"dn";
cmd_param_map[cmd_set_mtl_selfill_map_v_angle]=L"dn";
cmd_param_map[cmd_set_mtl_selfill_map_w_angle]=L"dn";
cmd_param_map[cmd_set_mtl_diffuse_map_u_offset_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_diffuse_map_v_offset_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_diffuse_map_u_tiling_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_diffuse_map_v_tiling_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_diffuse_map_u_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_diffuse_map_v_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_diffuse_map_w_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map_u_offset_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map_v_offset_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map_u_tiling_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map_v_tiling_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map_u_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map_v_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_normal_map_w_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map_u_offset_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map_v_offset_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map_u_tiling_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map_v_tiling_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map_u_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map_v_angle_animdata]=L"ds";
cmd_param_map[cmd_set_mtl_selfill_map_w_angle_animdata]=L"ds";

Tokenizer tokenizer(utf8_str);
int token_idx;
for(token_idx=0;token_idx<tokenizer.getTokenCount();)
{
	if(tokenizer.getTokenType(token_idx)!=SCN_TOKEN_IDENTIFIER)
	{
		++token_idx;
		continue;
	}

	int cmd=cmd_map[tokenizer.getTokenWString(token_idx)];
	const wstring& cmd_param=cmd_param_map[cmd];

	if(!cmd||!tokenizer.matchPattern(token_idx+1,cmd_param))
	{
		// unrecognized command or parameter pattern

		++token_idx;
		continue;
	}
	if(cmd==cmd_resize_mtls)
	{

	}
	else if(cmd==cmd_set_mtl_render_type)
	{

	}
	else if(cmd==cmd_set_mtl_render_method)
	{

	}
	else if(cmd==cmd_set_mtl_ambient)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse)
	{

	}
	else if(cmd==cmd_set_mtl_specular)
	{

	}
	else if(cmd==cmd_set_mtl_specular_level)
	{

	}
	else if(cmd==cmd_set_mtl_glossiness)
	{

	}
	else if(cmd==cmd_set_mtl_two_side)
	{

	}
	else if(cmd==cmd_set_mtl_blend_type)
	{

	}
	else if(cmd==cmd_set_mtl_opacity)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map)
	{

	}
	else if(cmd==cmd_set_mtl_toon_map)
	{

	}
	else if(cmd==cmd_set_mtl_toon_outline)
	{

	}
	else if(cmd==cmd_set_mtl_toon_outline_width)
	{

	}
	else if(cmd==cmd_set_mtl_toon_outline_color)
	{

	}
	else if(cmd==cmd_set_mtl_selfill)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_filter)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_u_wrap)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_v_wrap)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_filter)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_u_wrap)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_v_wrap)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_filter)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_u_wrap)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_v_wrap)
	{

	}
	else if(cmd==cmd_set_mtl_reflect_amount)
	{

	}
	else if(cmd==cmd_set_mtl_layer_index)
	{

	}
	else if(cmd==cmd_create_am_controller)
	{

	}
	else if(cmd==cmd_create_di_controller)
	{

	}
	else if(cmd==cmd_create_sp_controller)
	{

	}
	else if(cmd==cmd_create_sl_controller)
	{

	}
	else if(cmd==cmd_create_gl_controller)
	{

	}
	else if(cmd==cmd_create_op_controller)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_u_offset)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_v_offset)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_u_tiling)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_v_tiling)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_u_angle)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_v_angle)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_w_angle)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_u_offset)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_v_offset)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_u_tiling)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_v_tiling)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_u_angle)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_v_angle)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_w_angle)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_u_offset)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_v_offset)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_u_tiling)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_v_tiling)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_u_angle)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_v_angle)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_w_angle)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_u_offset_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_v_offset_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_u_tiling_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_v_tiling_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_u_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_v_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_diffuse_map_w_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_u_offset_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_v_offset_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_u_tiling_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_v_tiling_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_u_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_v_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_normal_map_w_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_u_offset_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_v_offset_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_u_tiling_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_v_tiling_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_u_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_v_angle_animdata)
	{

	}
	else if(cmd==cmd_set_mtl_selfill_map_w_angle_animdata)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
