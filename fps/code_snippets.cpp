#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#undef min
#undef max

//////// std console ////////

class std_console
{
private:
	size_t				_alloc_console;
public:
	std_console()
	{
		_alloc_console = !!AllocConsole();
		if(_alloc_console)
		{
			freopen("CONOUT$", "w", stdout);
			freopen("CONIN$", "r", stdin);
			std::ios::sync_with_stdio();
		}
	}
	~std_console()
	{
		if(_alloc_console)
		{
			fclose(stdout);
			fclose(stdin);
			FreeConsole();
		}
	}
};

//////// std console ////////

//////// test binary tree ////////

class bnode
{
public:
	int32_t			num;
	bnode*			left;
	bnode*			right;
public:
	bnode(int32_t num)
	{
		this->num = num;
		left = NULL;
		right = NULL;
	}
	~bnode()
	{
		if(left)
			delete left;
		if(right)
			delete right;
	}
public:
	void			preorder()
	{
		printf("%d\n", num);
		if(left)
			left->preorder();
		if(right)
			right->preorder();
	}
	void			inorder()
	{
		if(left)
			left->inorder();
		printf("%d\n", num);
		if(right)
			right->inorder();
	}
	void			postorder()
	{
		if(left)
			left->postorder();
		if(right)
			right->postorder();
		printf("%d\n", num);
	}
	void			insert(int32_t n)
	{
		if(n == num)
			return;

		if(n < num)
		{
			if(left)
				left->insert(n);
			else
				left = new bnode(n);
		}
		else
		{
			if(right)
				right->insert(n);
			else
				right = new bnode(n);
		}
	}
	void			save(MemWriter& writer)
	{
		writer.write(num);
		uint8_t flag = (left ? 0x1: 0) | (right ? 0x2 : 0);
		writer.write(flag);
		if(left)
			left->save(writer);
		if(right)
			right->save(writer);
	}
	void			load(MemWriter& reader)
	{
		reader.read(num);
		uint8_t flag;
		reader.read(flag);

		if(flag & 0x1)
		{
			left = new bnode(0);
			left->load(reader);
		}
		if(flag & 0x2)
		{
			right = new bnode(0);
			right->load(reader);
		}
	}
};

void test_binary_tree()
{
	std_console console;

	lrng r(GetTickCount());

	bnode* n = NULL;
	for(size_t i = 0; i < 10; ++i)
	{
		if(!n)
			n = new bnode(r.next() % 100);
		else
			n->insert(r.next() % 100);
	}

	n->preorder();
	getchar();
	n->inorder();
	getchar();
	n->postorder();
	getchar();

	MemWriter writer;
	n->save(writer);

	writer.seek(0);

	bnode* n2 = new bnode(0);
	n2->load(writer);

	n2->preorder();
	getchar();
	n2->inorder();
	getchar();
	n2->postorder();
	getchar();

	delete n;
	delete n2;
}

//////// test binary tree ////////

//////// get the current module ///////
HANDLE get_current_module()
{
	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi, 0, sizeof(mbi));
	if(!VirtualQuery(get_current_module, &mbi, sizeof(mbi)))
		return NULL;
	return (HANDLE)mbi.AllocationBase;
}
//////// get the current module ///////

//////// call stack /////////////

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

void init_symbols()
{
	SymInitialize(GetCurrentProcess(), NULL, TRUE);
}

void stack_walk()
{
	CONTEXT ctx;
	RtlCaptureContext(&ctx);

	STACKFRAME64 stackframe;
	memset(&stackframe, 0, sizeof(stackframe));

	// initialize
	DWORD machine_type = 0;

#if defined(_M_IX86)
	machine_type = IMAGE_FILE_MACHINE_I386;
	stackframe.AddrPC.Offset = ctx.Eip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = ctx.Ebp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = ctx.Esp;
	stackframe.AddrStack.Mode = AddrModeFlat;
#elif defined(_M_IA64)
	machine_type = IMAGE_FILE_MACHINE_IA64;
	stackframe.AddrPC.Offset = ctx.StIIP;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = ctx.RsBSP;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = ctx.IntSp;
	stackframe.AddrStack.Mode = AddrModeFlat;
	stackframe.AddrBStore.Offset = ctx.RsBSP;
	stackframe.AddrBStore.Mode = AddrModeFlat;
#elif defined(_M_X64)
	machine_type = IMAGE_FILE_MACHINE_AMD64;
	stackframe.AddrPC.Offset = ctx.Rip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = ctx.Rbp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = ctx.Rsp;
	stackframe.AddrStack.Mode = AddrModeFlat;
#else
#	error platform not supported
#endif

	for(size_t i = 0; i < 16; ++i)
	{
		if(!StackWalk64(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &stackframe, &ctx, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
			break;

		DWORD64 disp = 0;
		char buffer[512];
		IMAGEHLP_SYMBOL64* sym = (IMAGEHLP_SYMBOL64*)buffer;
		memset(buffer, 0, sizeof(buffer));
		sym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		sym->MaxNameLength = sizeof(buffer) - sizeof(IMAGEHLP_SYMBOL64);
		if(!SymGetSymFromAddr64(GetCurrentProcess(), stackframe.AddrPC.Offset, &disp, sym))
		{
			OutputDebugString(L"??\r\n");
			continue;
		}
		IMAGEHLP_LINE64 line;
		memset(&line, 0, sizeof(line));
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		DWORD disp2 = 0;
		if(!SymGetLineFromAddr64(GetCurrentProcess(), stackframe.AddrPC.Offset, &disp2, &line))
		{
			OutputDebugString(L"??\r\n");
			continue;
		}

		//Z_STATIC_ASSERT( sizeof(sym->Name[0]) == sizeof(wchar_t), ASSUMPTION_WRONG );
		//Z_STATIC_ASSERT( sizeof(line.FileName[0]) == sizeof(wchar_t), ASSUMPTION_WRONG2 );

		OutputDebugString(FORMATW(L"%s! %s, Line %d\r\n", MBSTOWCS(line.FileName).c_str(), MBSTOWCS(sym->Name).c_str(), line.LineNumber).c_str());
	}
}

//////// call stack /////////////