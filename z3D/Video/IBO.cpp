
#include "IBO.h"
#include "Gfx.h"

namespace z3D
{
	namespace Video
	{
		IBO::IBO(): _dirty(true) {}
		IBO::~IBO() {}
		SPtr<IBO>				IBO::create(size_t length, bool _32bit, bool dynamic) {return Gfx::current()->createIBO(length, _32bit, dynamic);}
		void					IBO::setDirty(bool dirty) {_dirty = dirty;}
		bool					IBO::dirty() const {return !!_dirty;}
	};
};

