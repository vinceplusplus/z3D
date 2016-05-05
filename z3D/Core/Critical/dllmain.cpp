
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

#include "Logger.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		z3D::Core::syslog_basic.flush();
		break;
	}
    return TRUE;
}

