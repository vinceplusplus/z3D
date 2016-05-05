
#pragma once

#include "predecl.h"

#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		class ResMgr
		{
		private:
			map<wcs, ResBase*>		_res_map;
		public:
			ResMgr();
			~ResMgr();
		public:
			void		add(ResBase* res);
			void		remove(ResBase* res);
			ResBase*	find(const wcs& filepath);
			template<class T>
			T*			find(const wcs& filepath) {ResBase* res = find(filepath); return res ? (T*)res : NULL;}
		};
	};
};

