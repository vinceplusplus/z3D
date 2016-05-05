// genCmdParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../z3D/Core/Header.h"
using namespace z3D::Core;

int _tmain(int argc, _TCHAR* argv[])
{
	string s;

	while(true)
	{
		char ch=getchar();
		if(ch==EOF)
			break;
		s+=ch;
	}
	string utf8_str=getTextUTF8((void*)s.c_str(),(int)s.length());

	LexScan scan(utf8_str);

	vector<wstring> array_cmd;
	vector<wstring> array_parameter;
	int ret;
	while((ret=scan.scan())!=0)
	{
		wstring id;
		if(ret==SCN_TOKEN_IDENTIFIER)
			id=scan.getIdentifier();
		ret=scan.scan();
		if(!ret)
			break;
		if(ret==SCN_TOKEN_STRING)
		{
			array_cmd.push_back(id);
			array_parameter.push_back(scan.getValueWString());
		}
	}

	printf("int cmd_no=0;\n");
	printf("map<wstring,int> cmd_map;\n");
	printf("vector<wstring> cmd_param_map;\n");
	printf("\n");

	int i;
	for(i=0;i<(int)array_cmd.size();++i)
		printf("cmd_map[L\"%s\"]=++cmd_no;\n",WCSTOMBS(array_cmd[i]).c_str());
	printf("\n");

	for(i=0;i<(int)array_cmd.size();++i)
		printf("int cmd_%s=cmd_map[L\"%s\"];\n",WCSTOMBS(array_cmd[i]).c_str(),WCSTOMBS(array_cmd[i]).c_str());
	printf("\n");

	printf("cmd_param_map.resize(cmd_map.size()+1);\n");
	printf("\n");
	for(i=0;i<(int)array_cmd.size();++i)
		printf("cmd_param_map[cmd_%s]=L\"%s\";\n",WCSTOMBS(array_cmd[i]).c_str(),WCSTOMBS(array_parameter[i]).c_str());
	printf("\n");

	printf("Tokenizer tokenizer(utf8_str);\n");
	printf("int token_idx;\n");

	printf("for(token_idx=0;token_idx<tokenizer.getTokenCount();)\n");
	printf("{\n");
	printf("\tif(tokenizer.getTokenType(token_idx)!=SCN_TOKEN_IDENTIFIER)\n");
	printf("\t{\n");
	printf("\t\t++token_idx;\n");
	printf("\t\tcontinue;\n");
	printf("\t}\n");
	printf("\n");
	printf("\tint cmd=cmd_map[tokenizer.getTokenWString(token_idx)];\n");
	printf("\tconst wstring& cmd_param=cmd_param_map[cmd];\n");
	printf("\n");
	printf("\tif(!cmd||!tokenizer.matchPattern(token_idx+1,cmd_param))\n");
	printf("\t{\n");
	printf("\t\t// unrecognized command or parameter pattern\n");
	printf("\n");
	printf("\t\t++token_idx;\n");
	printf("\t\tcontinue;\n");
	printf("\t}\n");
	for(i=0;i<(int)array_cmd.size();++i)
	{
		/*
		if(i==0)
			printf("\tif(cmd==cmd_%s)\n",WCSTOMBS(array_cmd[i]).c_str());
		else
			printf("\telse if(cmd==cmd_%s)\n",WCSTOMBS(array_cmd[i]).c_str());
		printf("\t{\n");
		printf("\t\tif(tokenizer.matchPattern(token_idx+1,param_%s))\n",WCSTOMBS(array_cmd[i]).c_str());
		printf("\t\t{\n\n");
		//printf("\t\t\ttoken_idx+=%d;\n",array_parameter[i].length()+1);
		printf("\t\t\ttoken_idx+=(int)param_%s.length()+1;\n",WCSTOMBS(array_cmd[i]).c_str());
		printf("\t\t\tcontinue;\n");
		printf("\t\t}\n");
		printf("\t}\n");*/
		if(i==0)
			printf("\tif(cmd==cmd_%s)\n",WCSTOMBS(array_cmd[i]).c_str());
		else
			printf("\telse if(cmd==cmd_%s)\n",WCSTOMBS(array_cmd[i]).c_str());
		printf("\t{\n\n");
		printf("\t}\n");
	}
	//printf("\t++token_idx;\n");
	printf("\ttoken_idx+=(int)cmd_param.length()+1;\n");
	printf("}\n");

	return 0;
}

