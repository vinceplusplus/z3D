int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"create_node"]=++cmd_no;
cmd_map[L"create_mesh_node"]=++cmd_no;
cmd_map[L"create_ambient_light_node"]=++cmd_no;
cmd_map[L"create_dir_light_node"]=++cmd_no;
cmd_map[L"create_point_light_node"]=++cmd_no;
cmd_map[L"create_spot_light_node"]=++cmd_no;
cmd_map[L"create_camera_node"]=++cmd_no;
cmd_map[L"set_camera_ortho"]=++cmd_no;
cmd_map[L"create_bone_node"]=++cmd_no;
cmd_map[L"set_node_pos"]=++cmd_no;
cmd_map[L"set_node_rot"]=++cmd_no;
cmd_map[L"set_node_scale"]=++cmd_no;
cmd_map[L"set_node_parent"]=++cmd_no;
cmd_map[L"set_node_shadow_mode"]=++cmd_no;
cmd_map[L"set_camera_node_yfov_animdata"]=++cmd_no;
cmd_map[L"set_camera_node_znear_animdata"]=++cmd_no;
cmd_map[L"set_camera_node_zfar_animdata"]=++cmd_no;
cmd_map[L"set_light_node_color_animdata"]=++cmd_no;
cmd_map[L"set_light_node_multiplier_animdata"]=++cmd_no;
cmd_map[L"set_light_node_radius_animdata"]=++cmd_no;
cmd_map[L"set_light_node_inner_angle_animdata"]=++cmd_no;
cmd_map[L"set_light_node_outer_angle_animdata"]=++cmd_no;

int cmd_create_node=cmd_map[L"create_node"];
int cmd_create_mesh_node=cmd_map[L"create_mesh_node"];
int cmd_create_ambient_light_node=cmd_map[L"create_ambient_light_node"];
int cmd_create_dir_light_node=cmd_map[L"create_dir_light_node"];
int cmd_create_point_light_node=cmd_map[L"create_point_light_node"];
int cmd_create_spot_light_node=cmd_map[L"create_spot_light_node"];
int cmd_create_camera_node=cmd_map[L"create_camera_node"];
int cmd_set_camera_ortho=cmd_map[L"set_camera_ortho"];
int cmd_create_bone_node=cmd_map[L"create_bone_node"];
int cmd_set_node_pos=cmd_map[L"set_node_pos"];
int cmd_set_node_rot=cmd_map[L"set_node_rot"];
int cmd_set_node_scale=cmd_map[L"set_node_scale"];
int cmd_set_node_parent=cmd_map[L"set_node_parent"];
int cmd_set_node_shadow_mode=cmd_map[L"set_node_shadow_mode"];
int cmd_set_camera_node_yfov_animdata=cmd_map[L"set_camera_node_yfov_animdata"];
int cmd_set_camera_node_znear_animdata=cmd_map[L"set_camera_node_znear_animdata"];
int cmd_set_camera_node_zfar_animdata=cmd_map[L"set_camera_node_zfar_animdata"];
int cmd_set_light_node_color_animdata=cmd_map[L"set_light_node_color_animdata"];
int cmd_set_light_node_multiplier_animdata=cmd_map[L"set_light_node_multiplier_animdata"];
int cmd_set_light_node_radius_animdata=cmd_map[L"set_light_node_radius_animdata"];
int cmd_set_light_node_inner_angle_animdata=cmd_map[L"set_light_node_inner_angle_animdata"];
int cmd_set_light_node_outer_angle_animdata=cmd_map[L"set_light_node_outer_angle_animdata"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_create_node]=L"s";
cmd_param_map[cmd_create_mesh_node]=L"ss";
cmd_param_map[cmd_create_ambient_light_node]=L"sffffd";
cmd_param_map[cmd_create_dir_light_node]=L"sffffffdd";
cmd_param_map[cmd_create_point_light_node]=L"sfffffdd";
cmd_param_map[cmd_create_spot_light_node]=L"sfffffffdd";
cmd_param_map[cmd_create_camera_node]=L"sffff";
cmd_param_map[cmd_set_camera_ortho]=L"s";
cmd_param_map[cmd_create_bone_node]=L"sffffff";
cmd_param_map[cmd_set_node_pos]=L"sfff";
cmd_param_map[cmd_set_node_rot]=L"sffff";
cmd_param_map[cmd_set_node_scale]=L"sfff";
cmd_param_map[cmd_set_node_parent]=L"ss";
cmd_param_map[cmd_set_node_shadow_mode]=L"sdd";
cmd_param_map[cmd_set_camera_node_yfov_animdata]=L"ss";
cmd_param_map[cmd_set_camera_node_znear_animdata]=L"ss";
cmd_param_map[cmd_set_camera_node_zfar_animdata]=L"ss";
cmd_param_map[cmd_set_light_node_color_animdata]=L"ss";
cmd_param_map[cmd_set_light_node_multiplier_animdata]=L"ss";
cmd_param_map[cmd_set_light_node_radius_animdata]=L"ss";
cmd_param_map[cmd_set_light_node_inner_angle_animdata]=L"ss";
cmd_param_map[cmd_set_light_node_outer_angle_animdata]=L"ss";

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
	if(cmd==cmd_create_node)
	{

	}
	else if(cmd==cmd_create_mesh_node)
	{

	}
	else if(cmd==cmd_create_ambient_light_node)
	{

	}
	else if(cmd==cmd_create_dir_light_node)
	{

	}
	else if(cmd==cmd_create_point_light_node)
	{

	}
	else if(cmd==cmd_create_spot_light_node)
	{

	}
	else if(cmd==cmd_create_camera_node)
	{

	}
	else if(cmd==cmd_set_camera_ortho)
	{

	}
	else if(cmd==cmd_create_bone_node)
	{

	}
	else if(cmd==cmd_set_node_pos)
	{

	}
	else if(cmd==cmd_set_node_rot)
	{

	}
	else if(cmd==cmd_set_node_scale)
	{

	}
	else if(cmd==cmd_set_node_parent)
	{

	}
	else if(cmd==cmd_set_node_shadow_mode)
	{

	}
	else if(cmd==cmd_set_camera_node_yfov_animdata)
	{

	}
	else if(cmd==cmd_set_camera_node_znear_animdata)
	{

	}
	else if(cmd==cmd_set_camera_node_zfar_animdata)
	{

	}
	else if(cmd==cmd_set_light_node_color_animdata)
	{

	}
	else if(cmd==cmd_set_light_node_multiplier_animdata)
	{

	}
	else if(cmd==cmd_set_light_node_radius_animdata)
	{

	}
	else if(cmd==cmd_set_light_node_inner_angle_animdata)
	{

	}
	else if(cmd==cmd_set_light_node_outer_angle_animdata)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
