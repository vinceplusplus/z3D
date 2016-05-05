int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"resize_curves"]=++cmd_no;
cmd_map[L"add_curve_point"]=++cmd_no;

int cmd_resize_curves=cmd_map[L"resize_curves"];
int cmd_add_curve_point=cmd_map[L"add_curve_point"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_resize_curves]=L"d";
cmd_param_map[cmd_add_curve_point]=L"dfffffff";

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
	if(cmd==cmd_resize_curves)
	{

	}
	else if(cmd==cmd_add_curve_point)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
