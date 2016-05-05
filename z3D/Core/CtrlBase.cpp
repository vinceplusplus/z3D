
#include "CtrlBase.h"
#include "Constants.h"

namespace z3D
{
	namespace Core
	{
		CtrlBase::CtrlBase(const wcs& name)
			: _name(name)
		{
			_enabled = true;
		}
		CtrlBase::~CtrlBase()
		{
		}
		const wcs&						CtrlBase::getName() const
		{
			return _name;
		}
		void							CtrlBase::setEnabled(bool enabled)
		{
			_enabled = enabled;
		}
		bool							CtrlBase::isEnabled() const
		{
			return !!_enabled;
		}
		bool							CtrlBase::hasAnimData() const
		{
			return false;
		}
		void							CtrlBase::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			start = INFINITY;
			end = -INFINITY;
		}
		AnimCtrl*						CtrlBase::getAnimCtrl()
		{
			return NULL;
		}
	};
};

