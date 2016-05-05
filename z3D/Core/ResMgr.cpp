
#include "ResMgr.h"
#include "ResBase.h"
#include "LoggerSTL.h"

namespace z3D
{
	namespace Core
	{
		ResMgr::ResMgr() {}

		ResMgr::~ResMgr()
		{
			map<wcs,ResBase*>::iterator end = _res_map.end();
			for(map<wcs, ResBase*>::iterator iter = _res_map.begin(); iter != end; ++iter)
				iter->second->setResMgr(NULL);
			if(_res_map.size())
			{
				syslog << L"ResMgr: memory may leak!!";
				syslog << L"report starts";
				for(map<wcs,ResBase*>::iterator iter = _res_map.begin(); iter != end; ++iter)
					syslog << iter->second->filepath();
				syslog << L"report ends";
			}
		}
		void ResMgr::add(ResBase* res)
		{
			Z_ASSERT( res );
			Z_ASSERT( res->filepath().length() );

			map<wcs, ResBase*>::iterator iter;
			iter = _res_map.find(res->filepath());
			if(iter == _res_map.end())
			{
				_res_map[res->filepath()] = res;
				res->setResMgr(this);
			}
		}
		void ResMgr::remove(ResBase* res)
		{
			map<wcs, ResBase*>::iterator iter;
			iter = _res_map.find(res->filepath());
			if(iter != _res_map.end())
			{
				iter->second->setResMgr(NULL);
				_res_map.erase(iter);
			}
		}
		ResBase*	ResMgr::find(const wcs& filepath)
		{
			map<wcs, ResBase*>::iterator iter;
			iter = _res_map.find(filepath);
			if(iter != _res_map.end())
				return iter->second;

			return NULL;
		}
	};
};

