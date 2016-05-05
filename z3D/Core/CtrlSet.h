
#pragma once

#include "predecl.h"

#include "fast_string.h"

#include "SPtr.h"

namespace z3D
{
	namespace Core
	{
		class CtrlSet
		{
		private:
			vector<SPtr<CtrlBase>>			_ctrls;
		public:
			CtrlSet();
		public:
			void							attachCtrl(const SPtr<CtrlBase>& ctrl);
			void							detachCtrl(const SPtr<CtrlBase>& ctrl);
		public:
			const vector<SPtr<CtrlBase>>&	getArray() const;
		public:
			void							update(CtrlUpdater& ctrl_updater);
		public:
			void							set(const TimeAnimCfg& time_anim);
			void							transit(const TimeAnimCfg& time_anim, REAL duration);
		public:
			void							getAnimDataTimeRange(REAL& start, REAL& end) const;
		public:
			bool							isStopped() const;
		public:
			SPtr<CtrlBase>					getFirstKindOf(const TypeInfo* type) const;
			SPtr<CtrlBase>					getFirstTypeOf(const TypeInfo* type) const;
			SPtr<CtrlBase>					getFirstNamed(const wcs& name) const;
		};
	};
};

