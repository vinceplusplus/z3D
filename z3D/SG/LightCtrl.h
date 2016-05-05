
#pragma once

#include "predecl.h"
#include "SLightNode.h"

namespace z3D
{
	namespace SG
	{
		class LightCtrl: public CtrlBase
		{
		public:
			DECL_TYPEINFO();
		private:
			AnimDataEval<Vec3>				_eval_color;
			AnimDataEval<REAL>				_eval_multiplier;
			AnimDataEval<REAL>				_eval_radius;
			AnimDataEval<REAL>				_eval_inner_angle;
			AnimDataEval<REAL>				_eval_outer_angle;
		private:
			WPtr<SLightNode>				_light_node;
		private:
			AnimCtrl						_anim_ctrl;
		public:
			LightCtrl(const wcs& name, const SPtr<SLightNode>& node);
		protected:
			~LightCtrl();
		public:
			bool							hasAnimData() const;
			void							getAnimDataTimeRange(REAL& start, REAL& end) const;
			AnimCtrl*						getAnimCtrl();
		public:
			void							update(REAL dt);
		public:
			ptrdiff_t						getPriority() const;
			bool							isUpdateDeferred() const;
		public:
			void							setAnimDataColor(const SPtr< AnimData<Vec3> >& animdata);
			void							setAnimDataMultiplier(const SPtr< AnimData<REAL> >& animdata);
			void							setAnimDataRadius(const SPtr< AnimData<REAL> >& animdata);
			void							setAnimDataInnerAngle(const SPtr< AnimData<REAL> >& animdata);
			void							setAnimDataOuterAngle(const SPtr< AnimData<REAL> >& animdata);
		public:
			const SPtr< AnimData<Vec3> >&	getAnimDataColor() const;
			const SPtr< AnimData<REAL> >&	getAnimDataMultiplier() const;
			const SPtr< AnimData<REAL> >&	getAnimDataRadius() const;
			const SPtr< AnimData<REAL> >&	getAnimDataInnerAngle() const;
			const SPtr< AnimData<REAL> >&	getAnimDataOuterAngle() const;
		};
	};
};

