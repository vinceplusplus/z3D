int cmd_no=0;
map<wstring,int> cmd_map;
vector<wstring> cmd_param_map;

cmd_map[L"set_user_text"]=++cmd_no;

int cmd_set_user_text=cmd_map[L"set_user_text"];

cmd_param_map.resize(cmd_map.size()+1);

cmd_param_map[cmd_set_user_text]=L"ss";

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
	if(cmd==cmd_set_user_text)
	{

	}
	token_idx+=(int)cmd_param.length()+1;
}
