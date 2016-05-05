int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"set_polygon_mode"]=++cmd_no;
cmd_map[L"set_max_shadow_lights"]=++cmd_no;
cmd_map[L"set_glow_steps"]=++cmd_no;
cmd_map[L"set_fog_mode"]=++cmd_no;
cmd_map[L"set_fog_color"]=++cmd_no;
cmd_map[L"set_fog_start"]=++cmd_no;
cmd_map[L"set_fog_end"]=++cmd_no;
cmd_map[L"set_fog_density"]=++cmd_no;
cmd_map[L"set_clear_color"]=++cmd_no;

int cmd_set_polygon_mode=cmd_map[L"set_polygon_mode"];
int cmd_set_max_shadow_lights=cmd_map[L"set_max_shadow_lights"];
int cmd_set_glow_steps=cmd_map[L"set_glow_steps"];
int cmd_set_fog_mode=cmd_map[L"set_fog_mode"];
int cmd_set_fog_color=cmd_map[L"set_fog_color"];
int cmd_set_fog_start=cmd_map[L"set_fog_start"];
int cmd_set_fog_end=cmd_map[L"set_fog_end"];
int cmd_set_fog_density=cmd_map[L"set_fog_density"];
int cmd_set_clear_color=cmd_map[L"set_clear_color"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_set_polygon_mode]=L"n";
cmd_param_map[cmd_set_max_shadow_lights]=L"n";
cmd_param_map[cmd_set_glow_steps]=L"n";
cmd_param_map[cmd_set_fog_mode]=L"n";
cmd_param_map[cmd_set_fog_color]=L"nnn";
cmd_param_map[cmd_set_fog_start]=L"n";
cmd_param_map[cmd_set_fog_end]=L"n";
cmd_param_map[cmd_set_fog_density]=L"n";
cmd_param_map[cmd_set_clear_color]=L"nnn";

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
	if(cmd==cmd_set_polygon_mode)
	{

	}
	else if(cmd==cmd_set_max_shadow_lights)
	{

	}
	else if(cmd==cmd_set_glow_steps)
	{

	}
	else if(cmd==cmd_set_fog_mode)
	{

	}
	else if(cmd==cmd_set_fog_color)
	{

	}
	else if(cmd==cmd_set_fog_start)
	{

	}
	else if(cmd==cmd_set_fog_end)
	{

	}
	else if(cmd==cmd_set_fog_density)
	{

	}
	else if(cmd==cmd_set_clear_color)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
