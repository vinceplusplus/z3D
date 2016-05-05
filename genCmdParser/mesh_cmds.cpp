int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"resize_verts"]=++cmd_no;
cmd_map[L"resize_norms"]=++cmd_no;
cmd_map[L"resize_map_verts"]=++cmd_no;
cmd_map[L"resize_faces"]=++cmd_no;
cmd_map[L"resize_map_faces"]=++cmd_no;
cmd_map[L"resize_submeshes"]=++cmd_no;
cmd_map[L"set_vert"]=++cmd_no;
cmd_map[L"set_norm"]=++cmd_no;
cmd_map[L"set_map_vert"]=++cmd_no;
cmd_map[L"set_face"]=++cmd_no;
cmd_map[L"set_map_face"]=++cmd_no;
cmd_map[L"set_submesh"]=++cmd_no;

int cmd_resize_verts=cmd_map[L"resize_verts"];
int cmd_resize_norms=cmd_map[L"resize_norms"];
int cmd_resize_map_verts=cmd_map[L"resize_map_verts"];
int cmd_resize_faces=cmd_map[L"resize_faces"];
int cmd_resize_map_faces=cmd_map[L"resize_map_faces"];
int cmd_resize_submeshes=cmd_map[L"resize_submeshes"];
int cmd_set_vert=cmd_map[L"set_vert"];
int cmd_set_norm=cmd_map[L"set_norm"];
int cmd_set_map_vert=cmd_map[L"set_map_vert"];
int cmd_set_face=cmd_map[L"set_face"];
int cmd_set_map_face=cmd_map[L"set_map_face"];
int cmd_set_submesh=cmd_map[L"set_submesh"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_resize_verts]=L"d";
cmd_param_map[cmd_resize_norms]=L"d";
cmd_param_map[cmd_resize_map_verts]=L"d";
cmd_param_map[cmd_resize_faces]=L"d";
cmd_param_map[cmd_resize_map_faces]=L"d";
cmd_param_map[cmd_resize_submeshes]=L"d";
cmd_param_map[cmd_set_vert]=L"dfff";
cmd_param_map[cmd_set_norm]=L"dfff";
cmd_param_map[cmd_set_map_vert]=L"dff";
cmd_param_map[cmd_set_face]=L"ddddddd";
cmd_param_map[cmd_set_map_face]=L"dddd";
cmd_param_map[cmd_set_submesh]=L"ddddddd";

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
	if(cmd==cmd_resize_verts)
	{

	}
	else if(cmd==cmd_resize_norms)
	{

	}
	else if(cmd==cmd_resize_map_verts)
	{

	}
	else if(cmd==cmd_resize_faces)
	{

	}
	else if(cmd==cmd_resize_map_faces)
	{

	}
	else if(cmd==cmd_resize_submeshes)
	{

	}
	else if(cmd==cmd_set_vert)
	{

	}
	else if(cmd==cmd_set_norm)
	{

	}
	else if(cmd==cmd_set_map_vert)
	{

	}
	else if(cmd==cmd_set_face)
	{

	}
	else if(cmd==cmd_set_map_face)
	{

	}
	else if(cmd==cmd_set_submesh)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
