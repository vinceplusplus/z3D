
#include "AnimInstSet.h"
#include "Constants.h"

#define SAFE_DELETE(x) if(x) {delete x;x=NULL;}

namespace z3D
{
	namespace Core
	{
		/*
		inline AnimInstSet::~AnimInstSet()
		{
			clear();
		}
		inline void				AnimInstSet::clear()
		{
			AnimInst* animinst;
			for(animinst=first();animinst!=NULL;animinst=next())
				delete animinst;
			_map_name_animinst.clear();
			_map_animinst_name.clear();
			_map_animinst_order.clear();
			_map_order_animinst.clear();
		}
		inline int				AnimInstSet::size() const
		{
			return (int)_map_order_animinst.size();
		}
		inline AnimInst*		AnimInstSet::get(const wstring& ai_name)
		{
			hash_map<wstring,AnimInst*>::iterator iter=_map_name_animinst.find(ai_name);
			if(iter==_map_name_animinst.end())
				return NULL;
			return iter->second;
		}
		inline AnimInst*		AnimInstSet::first()
		{
			_iter=_map_order_animinst.begin();
			if(_iter==_map_order_animinst.end())
				return NULL;
			return _iter->second;
		}
		inline AnimInst*		AnimInstSet::next()
		{
			if(_iter==_map_order_animinst.end())
				return NULL;
			++_iter;
			if(_iter==_map_order_animinst.end())
				return NULL;
			return _iter->second;
		}
		inline const wstring	AnimInstSet::getName(AnimInst* animinst)
		{
			map<AnimInst*,wstring>::iterator iter=_map_animinst_name.find(animinst);
			if(iter==_map_animinst_name.end())
				return L"";
			return iter->second;
		}
		inline bool				AnimInstSet::addAnimInst(const wstring& ai_name,AnimInst* animinst)
		{
			if(!ai_name.length()||!animinst)
				return false;
			if(get(ai_name))
				return false;
			_map_name_animinst[ai_name]=animinst;
			_map_animinst_name[animinst]=ai_name;
			int order=1;
			map<int,AnimInst*>::iterator iter;
			if(_map_order_animinst.size())
				order=(--(iter=_map_order_animinst.end()))->first+1;
			_map_order_animinst[order]=animinst;
			_map_animinst_order[animinst]=order;
			return true;
		}
		inline AnimInst*		AnimInstSet::removeAnimInst(const wstring& ai_name)
		{
			hash_map<wstring,AnimInst*>::iterator iter=_map_name_animinst.find(ai_name);
			if(iter==_map_name_animinst.end())
				return NULL;
			AnimInst* animinst=iter->second;
			_map_name_animinst.erase(iter);
			_map_animinst_name.erase(animinst);
			int order=_map_animinst_order[animinst];
			_map_animinst_order.erase(animinst);
			_map_order_animinst.erase(order);

			return animinst;
		}
		inline bool				AnimInstSet::deleteAnimInst(const wstring& ai_name)
		{
			AnimInst* animinst=removeAnimInst(ai_name);
			if(!animinst)
				return false;
			delete animinst;
			return true;
		}
		inline bool				AnimInstSet::stopped() const
		{
			map<int,AnimInst*>::const_iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					if(!iter->second->stopped())
						return false;
			return true;
		}
		inline void				AnimInstSet::set(REAL start,REAL end,int loops)
		{
			set(start,end,loops,start,end,start);
		}
		inline void				AnimInstSet::set(REAL start,REAL end,int loops,REAL time)
		{
			set(start,end,loops,start,end,time);
		}
		inline void				AnimInstSet::set(const AnimInst& other)
		{
			set(other.start(),other.end(),other.loops(),other.loop_start(),other.loop_end(),other.time());
		}
		inline void				AnimInstSet::set(REAL start,REAL end,int loops,REAL loop_start,REAL loop_end,REAL time)
		{
			AnimInst::set(start,end,loops,loop_start,loop_end,time);
			map<int,AnimInst*>::iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					iter->second->set(start,end,loops,loop_start,loop_end,time);
		}
		inline void				AnimInstSet::setSpeed(REAL speed)
		{
			AnimInst::setSpeed(speed);
			map<int,AnimInst*>::iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					iter->second->setSpeed(speed);
		}
		inline void				AnimInstSet::setNeedUpdate(bool need_update)
		{
			AnimInst::setNeedUpdate(need_update);
			map<int,AnimInst*>::iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					iter->second->setNeedUpdate(need_update);
		}
		inline void				AnimInstSet::advance(REAL dt)
		{
			AnimInst::advance(dt);
			REAL dt2=dt*_speed;
			map<int,AnimInst*>::iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					iter->second->advance(dt2);
		}
		inline void				AnimInstSet::setTime(REAL time)
		{
			AnimInst::setTime(time);
			map<int,AnimInst*>::iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					iter->second->setTime(time);
		}
		inline void				AnimInstSet::setByAnimDataRange()
		{
			map<int,AnimInst*>::iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					iter->second->setByAnimDataRange();
		}
		inline void				AnimInstSet::update()
		{
			map<int,AnimInst*>::iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					iter->second->update();
		}
		inline REAL				AnimInstSet::getAnimDataStartTime() const
		{
			REAL start_time=INFINITY;
			map<int,AnimInst*>::const_iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					start_time=min(start_time,iter->second->getAnimDataStartTime());
			return start_time;
		}
		inline REAL				AnimInstSet::getAnimDataEndTime() const
		{
			REAL end_time=0.0f;
			map<int,AnimInst*>::const_iterator iter;
			for(iter=_map_order_animinst.begin();iter!=_map_order_animinst.end();++iter)
				if(iter->second)
					end_time=max(end_time,iter->second->getAnimDataEndTime());
			return end_time;
		}
		*/
	};
};

