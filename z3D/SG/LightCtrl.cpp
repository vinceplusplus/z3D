
#include "LightCtrl.h"

namespace z3D
{
	namespace SG
	{
		LightCtrl::LightCtrl(const wcs& name, const SPtr<SLightNode>& node)
			: CtrlBase(name), _light_node(node), _eval_color(NULL), _eval_multiplier(NULL), _eval_radius(NULL), _eval_inner_angle(NULL), _eval_outer_angle(NULL)
		{
		}
		LightCtrl::~LightCtrl()
		{
		}
		bool							LightCtrl::hasAnimData() const
		{
			return true;
		}
		template<class T> static REAL	getStartTime(const AnimDataEval<T>& eval, REAL default_time)
		{
			if(!eval.anim_data() || !eval.anim_data()->getKeyCount())
				return default_time;
			return eval.anim_data()->getKey(0).time;
		}
		template<class T> static REAL	getEndTime(const AnimDataEval<T>& eval, REAL default_time)
		{
			if(!eval.anim_data() || !eval.anim_data()->getKeyCount())
				return default_time;
			return eval.anim_data()->getKey(eval.anim_data()->getKeyCount() - 1).time;
		}
		void							LightCtrl::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			start = INFINITY;
			start = min(getStartTime(_eval_color, start), start);
			start = min(getStartTime(_eval_multiplier, start), start);
			start = min(getStartTime(_eval_radius, start), start);
			start = min(getStartTime(_eval_inner_angle, start), start);
			start = min(getStartTime(_eval_outer_angle, start), start);

			end = -INFINITY;
			end = max(getEndTime(_eval_color, end), end);
			end = max(getEndTime(_eval_multiplier, end), end);
			end = max(getEndTime(_eval_radius, end), end);
			end = max(getEndTime(_eval_inner_angle, end), end);
			end = max(getEndTime(_eval_outer_angle, end), end);
		}
		AnimCtrl*						LightCtrl::getAnimCtrl()
		{
			return &_anim_ctrl;
		}
		void							LightCtrl::update(REAL dt)
		{
			if(!_light_node.valid())
				return;

			Z_ASSERT( find(_light_node.get_unsafe()->getCtrlSet().getArray().begin(), _light_node.get_unsafe()->getCtrlSet().getArray().end(), this) != _light_node.get_unsafe()->getCtrlSet().getArray().end());

			_anim_ctrl.advance(dt);

			SPtr<SLightNode> l = _light_node.lock();

			REAL t = _anim_ctrl.current_stage().getTime();

			if(_eval_color.anim_data())
				l->setColor(_eval_color.sample(t));
			if(_eval_multiplier.anim_data())
				l->setMultiplier(_eval_multiplier.sample(t));
			if(_eval_radius.anim_data())
				l->setRadius(_eval_radius.sample(t));
			if(_eval_inner_angle.anim_data())
				l->setInnerAngle(_eval_inner_angle.sample(t));
			if(_eval_outer_angle.anim_data())
				l->setOuterAngle(_eval_outer_angle.sample(t));
		}
		ptrdiff_t						LightCtrl::getPriority() const
		{
			return 4000;
		}
		bool							LightCtrl::isUpdateDeferred() const
		{
			return false;
		}
		void							LightCtrl::setAnimDataColor(const SPtr< AnimData<Vec3> >& animdata) {_eval_color = animdata;}
		void							LightCtrl::setAnimDataMultiplier(const SPtr< AnimData<REAL> >& animdata) {_eval_multiplier = animdata;}
		void							LightCtrl::setAnimDataRadius(const SPtr< AnimData<REAL> >& animdata) {_eval_radius = animdata;}
		void							LightCtrl::setAnimDataInnerAngle(const SPtr< AnimData<REAL> >& animdata) {_eval_inner_angle = animdata;}
		void							LightCtrl::setAnimDataOuterAngle(const SPtr< AnimData<REAL> >& animdata) {_eval_outer_angle = animdata;}
		const SPtr< AnimData<Vec3> >&	LightCtrl::getAnimDataColor() const {return _eval_color.anim_data();}
		const SPtr< AnimData<REAL> >&	LightCtrl::getAnimDataMultiplier() const {return _eval_multiplier.anim_data();}
		const SPtr< AnimData<REAL> >&	LightCtrl::getAnimDataRadius() const {return _eval_radius.anim_data();}
		const SPtr< AnimData<REAL> >&	LightCtrl::getAnimDataInnerAngle() const {return _eval_inner_angle.anim_data();}
		const SPtr< AnimData<REAL> >&	LightCtrl::getAnimDataOuterAngle() const {return _eval_outer_angle.anim_data();}
	};
};

