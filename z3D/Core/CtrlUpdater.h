
#pragma once

#include "predecl.h"

#include "SPtr.h"

namespace z3D
{
	namespace Core
	{
		class CtrlUpdater
		{
		private:
			const REAL						_dt;
			vector<CtrlBase*>				_deferred_ctrls;
			const size_t					_manual_exec_deferred_update;
		public:
			CtrlUpdater(REAL dt, bool manual_exec_deferred_update);
		public:
			void							reset();
		public:
			void							operator() (const SPtr<CtrlBase>& ctrl);
		public:
			void							execDeferredUpdate();
		public:
			REAL							getDeltaTime() const;
			bool							isManuallyExecDeferredUpdate() const;
		};
	};
};

