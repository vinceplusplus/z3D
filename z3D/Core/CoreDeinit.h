
#pragma once

#include "TypeStuff.h"
#include "shared_count.h"

namespace z3D
{
	namespace Core
	{
		class CoreDeinit
		{
		private:
			static CoreDeinit			_instance;
		public:
			~CoreDeinit()
			{
				for(size_t i = 0; i < TypeInfo::_g_type_info.size(); ++i)
					delete TypeInfo::_g_type_info[i];
				TypeInfo::_g_type_info.clear();

				shared_count::_pool->dispose();
			}
		};
	};
};


