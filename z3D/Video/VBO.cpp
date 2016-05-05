
#include "VBO.h"
#include "Gfx.h"

namespace z3D
{
	namespace Video
	{
		VBO::VBO() {}
		VBO::~VBO() {}
		SPtr<VBO>				VBO::create(size_t length, bool dynamic) {return Gfx::current()->createVBO(length, dynamic);}
	};
};

