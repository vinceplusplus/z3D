int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"create_tm_controller"]=++cmd_no;
cmd_map[L"resize_pos_keys"]=++cmd_no;
cmd_map[L"resize_rot_keys"]=++cmd_no;
cmd_map[L"resize_scale_keys"]=++cmd_no;
cmd_map[L"set_pos_key"]=++cmd_no;
cmd_map[L"set_rot_key"]=++cmd_no;
cmd_map[L"set_scale_key"]=++cmd_no;

int cmd_create_tm_controller=cmd_map[L"create_tm_controller"];
int cmd_resize_pos_keys=cmd_map[L"resize_pos_keys"];
int cmd_resize_rot_keys=cmd_map[L"resize_rot_keys"];
int cmd_resize_scale_keys=cmd_map[L"resize_scale_keys"];
int cmd_set_pos_key=cmd_map[L"set_pos_key"];
int cmd_set_rot_key=cmd_map[L"set_rot_key"];
int cmd_set_scale_key=cmd_map[L"set_scale_key"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_create_tm_controller]=L"ss";
cmd_param_map[cmd_resize_pos_keys]=L"d";
cmd_param_map[cmd_resize_rot_keys]=L"d";
cmd_param_map[cmd_resize_scale_keys]=L"d";
cmd_param_map[cmd_set_pos_key]=L"dnfff";
cmd_param_map[cmd_set_rot_key]=L"dnffff";
cmd_param_map[cmd_set_scale_key]=L"dnfff";

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
	if(cmd==cmd_create_tm_controller)
	{

	}
	else if(cmd==cmd_resize_pos_keys)
	{

	}
	else if(cmd==cmd_resize_rot_keys)
	{

	}
	else if(cmd==cmd_resize_scale_keys)
	{

	}
	else if(cmd==cmd_set_pos_key)
	{

	}
	else if(cmd==cmd_set_rot_key)
	{

	}
	else if(cmd==cmd_set_scale_key)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
