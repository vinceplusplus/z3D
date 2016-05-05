// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//// Windows Header Files:
//#include <windows.h>
//// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>

// TODO: reference additional headers your program requires here

#include "../z3D/Core/Header.h"
#include "../z3D/IO/Header.h"
#include "../z3D/Video/Header.h"
#include "../z3D/Video/AVIRenderer.h"
#include "../z3D/SG/Header.h"
#include "../z3D/GameFrm/Header.h"
#include "../z3D/Audio/Header.h"
#include "../z3D/Physics/Header.h"

#include "../z3D/z3DCoreUIHelper/Header.h"

using namespace z3D::Core;
using namespace z3D::IO;
using namespace z3D::Video;
using namespace z3D::SG;
using namespace z3D::GameFrm;
using namespace z3D::Audio;
using namespace z3D::Physics;

#include <tchar.h>					// include this or results in unresolved external symbol _WinMain@16 referenced in function ___tmainCRTStartup
