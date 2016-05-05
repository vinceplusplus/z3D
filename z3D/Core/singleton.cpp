
#include "singleton.h"

namespace z3D
{
	namespace Core
	{
		singleton_base::singleton_base()
		{
			// register the singleton
			_array_singleton.push_back(this);
		}
		singleton_base::~singleton_base()
		{
			// unregister

			// it is naturally to find it in reverse order
			for(ptrdiff_t i = (ptrdiff_t)_array_singleton.size() - 1; i >= 0; --i)
				if(_array_singleton[i] == this)
				{
					_array_singleton.erase(_array_singleton.begin() + i);
					break;
				}
		}
		void							singleton_base::shutdown()
		{
			// delete all singletons, reversing the deletion because later ones may depend on the early ones
			while(_array_singleton.size())
				delete *(_array_singleton.end() - 1);
		}
	};
};

