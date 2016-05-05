int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"resize_used_bones"]=++cmd_no;
cmd_map[L"set_used_bone"]=++cmd_no;
cmd_map[L"resize_skin_verts"]=++cmd_no;
cmd_map[L"set_skin_vert"]=++cmd_no;

int cmd_resize_used_bones=cmd_map[L"resize_used_bones"];
int cmd_set_used_bone=cmd_map[L"set_used_bone"];
int cmd_resize_skin_verts=cmd_map[L"resize_skin_verts"];
int cmd_set_skin_vert=cmd_map[L"set_skin_vert"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_resize_used_bones]=L"d";
cmd_param_map[cmd_set_used_bone]=L"ds";
cmd_param_map[cmd_resize_skin_verts]=L"d";
cmd_param_map[cmd_set_skin_vert]=L"dddf";

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
	if(cmd==cmd_resize_used_bones)
	{

	}
	else if(cmd==cmd_set_used_bone)
	{

	}
	else if(cmd==cmd_resize_skin_verts)
	{

	}
	else if(cmd==cmd_set_skin_vert)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
