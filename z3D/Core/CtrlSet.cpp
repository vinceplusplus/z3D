
#include "CtrlSet.h"
#include "CtrlBase.h"
#include "AnimCtrl.h"
#include "CtrlUpdater.h"

#include "Constants.h"

namespace z3D
{
	namespace Core
	{
		CtrlSet::CtrlSet()
		{}
		void							CtrlSet::attachCtrl(const SPtr<CtrlBase>& ctrl)
		{
			class pred
			{
			public:
				bool						operator() (const SPtr<CtrlBase>& c0, const SPtr<CtrlBase>& c1)
				{
					return (c0->getPriority() > c1->getPriority());
				}
			};
			vector<SPtr<CtrlBase>>::iterator iter = upper_bound(_ctrls.begin(), _ctrls.end(), ctrl, pred());
			_ctrls.insert(iter, ctrl);
		}
		void							CtrlSet::detachCtrl(const SPtr<CtrlBase>& ctrl)
		{
			vector<SPtr<CtrlBase>>::iterator iter = find(_ctrls.begin(), _ctrls.end(), ctrl);
			if(iter != _ctrls.end())
				_ctrls.erase(iter);
			else
			{
				Z_ASSERT( false );
			}
		}
		const vector<SPtr<CtrlBase>>&	CtrlSet::getArray() const
		{
			return _ctrls;
		}
		void							CtrlSet::update(CtrlUpdater& ctrl_updater)
		{
			for_each<vector<SPtr<CtrlBase>>::iterator, CtrlUpdater&>(_ctrls.begin(), _ctrls.end(), ctrl_updater);
		}
		void							CtrlSet::set(const TimeAnimCfg& time_anim)
		{
			class fn
			{
			private:
				const TimeAnimCfg&			_time_anim;
			public:
				fn(const TimeAnimCfg& time_anim)
					: _time_anim(time_anim)
				{}
			public:
				void						operator() (const SPtr<CtrlBase>& ctrl)
				{
					AnimCtrl* c = ctrl->getAnimCtrl();
					if(c)
						c->set(_time_anim);
				}
			};

			for_each<vector<SPtr<CtrlBase>>::iterator, fn&>(_ctrls.begin(), _ctrls.end(), fn(time_anim));
		}
		void							CtrlSet::transit(const TimeAnimCfg& time_anim, REAL duration)
		{
			class fn
			{
			private:
				const TimeAnimCfg&			_time_anim;
				REAL						_duration;
			public:
				fn(const TimeAnimCfg& time_anim, REAL duration)
					: _time_anim(time_anim), _duration(duration)
				{}
			public:
				void						operator() (const SPtr<CtrlBase>& ctrl)
				{
					AnimCtrl* c = ctrl->getAnimCtrl();
					if(c)
						c->transit(_time_anim, _duration);
				}
			};

			for_each<vector<SPtr<CtrlBase>>::iterator, fn&>(_ctrls.begin(), _ctrls.end(), fn(time_anim, duration));
		}
		void							CtrlSet::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			start = INFINITY;
			end = -INFINITY;

			class fn
			{
			private:
				REAL&						_start;
				REAL&						_end;
			public:
				fn(REAL& start, REAL& end)
					: _start(start), _end(end)
				{}
			public:
				void						operator() (const SPtr<CtrlBase>& ctrl)
				{
					if(ctrl->hasAnimData())
					{
						REAL start = INFINITY, end = -INFINITY;
						ctrl->getAnimDataTimeRange(start, end);

						_start = min(_start, start);
						_end = max(_end, end);
					}
				}
			};

			for_each<vector<SPtr<CtrlBase>>::const_iterator, fn&>(_ctrls.begin(), _ctrls.end(), fn(start, end));
		}
		bool							CtrlSet::isStopped() const
		{
			bool stopped = true;
			class fn
			{
			private:
				bool&						_stopped;
			public:
				fn(bool& stopped)
					: _stopped(stopped)
				{}
			public:
				void						operator() (const SPtr<CtrlBase>& ctrl)
				{
					if(ctrl->getAnimCtrl())
						_stopped &= (ctrl->getAnimCtrl()->getStageCount() == 1 && ctrl->getAnimCtrl()->current_stage().isStopped());
				}
			};

			for_each<vector<SPtr<CtrlBase>>::const_iterator, fn&>(_ctrls.begin(), _ctrls.end(), fn(stopped));

			return stopped;
		}
		SPtr<CtrlBase>					CtrlSet::getFirstKindOf(const TypeInfo* type) const
		{
			class pred
			{
			private:
				const TypeInfo*				_type;
			public:
				pred(const TypeInfo* type)
					: _type(type)
				{}
			public:
				bool						operator() (const SPtr<CtrlBase>& ctrl) const
				{
					return ctrl->type_info()->kind_of(_type);
				}
			};
			vector<SPtr<CtrlBase>>::const_iterator iter = find_if(_ctrls.begin(), _ctrls.end(), pred(type));
			if(iter != _ctrls.end())
				return *iter;
			return NULL;
		}
		SPtr<CtrlBase>					CtrlSet::getFirstTypeOf(const TypeInfo* type) const
		{
			class pred
			{
			private:
				const TypeInfo*				_type;
			public:
				pred(const TypeInfo* type)
					: _type(type)
				{}
			public:
				bool						operator() (const SPtr<CtrlBase>& ctrl) const
				{
					return ctrl->type_info()->exactly(_type);
				}
			};
			vector<SPtr<CtrlBase>>::const_iterator iter = find_if(_ctrls.begin(), _ctrls.end(), pred(type));
			if(iter != _ctrls.end())
				return *iter;
			return NULL;
		}
		SPtr<CtrlBase>					CtrlSet::getFirstNamed(const wcs& name) const
		{
			class pred
			{
			private:
				const wcs&					_name;
			public:
				pred(const wcs& name)
					: _name(name)
				{}
			public:
				bool						operator() (const SPtr<CtrlBase>& ctrl) const
				{
					return ctrl->getName() == _name;
				}
			};
			vector<SPtr<CtrlBase>>::const_iterator iter = find_if(_ctrls.begin(), _ctrls.end(), pred(name));
			if(iter != _ctrls.end())
				return *iter;
			return NULL;
		}
	};
};

