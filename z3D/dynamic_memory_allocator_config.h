
#pragma once

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif

#include <malloc.h>
#include <stdlib.h>

#ifdef _DEBUG
	#include <crtdbg.h>
#endif

