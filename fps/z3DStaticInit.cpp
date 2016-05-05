
#ifdef NDEBUG
#define Z3D_USE_NEDMALLOC
#endif

#include <new.h>

#define Z3D_ENABLE_FREEIMAGE
#define Z3D_ENABLE_CORE
#define Z3D_ENABLE_IO
#define Z3D_ENABLE_VIDEO
#define Z3D_ENABLE_SG
#define Z3D_ENABLE_INTERSECT
#define Z3D_ENABLE_PHYSICS
#define Z3D_ENABLE_GAMEFRM
#define Z3D_ENABLE_TCPFRM

#include "../z3D/static_initializers.h"

