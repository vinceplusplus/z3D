
#pragma once

#include "predecl.h"

#include "TypeStuff.h"
#include "ResBase.h"

namespace z3D
{
	namespace Core
	{
		class CtrlBase: public RefBase
		{
			DECL_TYPEINFO();
		private:
			wcs								_name;
			size_t							_enabled;
		public:
			CtrlBase(const wcs& name);
		protected:
			~CtrlBase();
		public:
			const wcs&						getName() const;
		public:
			void							setEnabled(bool enabled);
			bool							isEnabled() const;
		public:
			virtual bool					hasAnimData() const;
			virtual void					getAnimDataTimeRange(REAL& start, REAL& end) const;
			virtual AnimCtrl*				getAnimCtrl();
		public:
			virtual void					update(REAL dt) = 0;
		public:
			virtual ptrdiff_t				getPriority() const = 0;
			virtual bool					isUpdateDeferred() const = 0;
		};
	};
};

