
#include "CtrlUpdater.h"
#include "CtrlBase.h"

namespace z3D
{
	namespace Core
	{
		CtrlUpdater::CtrlUpdater(REAL dt, bool manual_exec_deferred_update)
			: _dt(dt), _manual_exec_deferred_update(manual_exec_deferred_update)
		{
		}
		void							CtrlUpdater::reset()
		{
			_deferred_ctrls.clear();
		}
		void							CtrlUpdater::operator() (const SPtr<CtrlBase>& ctrl)
		{
			if(!ctrl->isEnabled())
				return;
			if(ctrl->isUpdateDeferred())
				_deferred_ctrls.push_back(ctrl.get());
			else
				ctrl->update(_dt);
		}
		void							CtrlUpdater::execDeferredUpdate()
		{
			class fn
			{
			private:
				REAL						_dt;
			public:
				fn(REAL dt)
					: _dt(dt)
				{}
			public:
				void						operator() (CtrlBase* ctrl)
				{
					ctrl->update(_dt);
				}
			};
			for_each<vector<CtrlBase*>::iterator, fn&>(_deferred_ctrls.begin(), _deferred_ctrls.end(), fn(_dt));
		}
		REAL							CtrlUpdater::getDeltaTime() const
		{
			return _dt;
		}
		bool							CtrlUpdater::isManuallyExecDeferredUpdate() const
		{
			return !!_manual_exec_deferred_update;
		}
	};
};

