
#include "AnimCtrl.h"
#include "Constants.h"
#include "MathFunc.h"
#include "functor_inl.h"

namespace z3D
{
	namespace Core
	{
		AnimCtrl::Clip::Clip()
		{
			setup(1.0f, 1.0f, 0.0f, INFINITY);
		}
		AnimCtrl::Clip::Clip(REAL weight, REAL max_weight, REAL transition_time, REAL time_left)
		{
			setup(weight, max_weight, transition_time, time_left);
		}
		void							AnimCtrl::Clip::setup(REAL weight, REAL max_weight, REAL transition_time, REAL time_left)
		{
			_weight = weight;
			_max_weight = max_weight;
			if(transition_time)
				_weight_incr_rate = max_weight / transition_time;
			else
				_weight_incr_rate = 0.0f;
			_time_left = time_left;
		}
		bool							AnimCtrl::Clip::advance(REAL dt)
		{
			if(_time_left > 0.0f)
			{
				bool ret = TimeAnim::advance(dt);

				_weight = CLAMP(_weight + _weight_incr_rate * dt, 0, _max_weight);
				if(_time_left != INFINITY)
					_time_left = max(0.0f, _time_left - dt);

				return ret;
			}
			else
			{
				if(_weight_incr_rate)
					_weight = CLAMP(_weight - _weight_incr_rate * dt, 0, _max_weight);
				else
					_weight = 0.0f;

				return false;
			}
		}
		REAL							AnimCtrl::Clip::getWeight() const
		{
			return _weight;
		}
		REAL							AnimCtrl::Clip::getMaxWeight() const
		{
			return _max_weight;
		}
		REAL							AnimCtrl::Clip::getWeightIncrRate() const
		{
			return _weight_incr_rate;
		}
		REAL							AnimCtrl::Clip::getTimeLeft() const
		{
			return _time_left;
		}
		bool							AnimCtrl::Clip::isEffective() const
		{
			return !!_weight;
		}

		AnimCtrl::AnimCtrl()
		{
			_transition = LOD::TRANSITION_MULTISTAGE_BLEND;

			_stage_count = 1;

			_secondary_stage_count = 0;

			_capture_cb_opaque = NULL;
		}
		const AnimCtrl::Clip&			AnimCtrl::current_stage() const
		{
			return _stages[_stage_count - 1];
		}
		AnimCtrl::Clip&					AnimCtrl::current_stage()
		{
			return _stages[_stage_count - 1];
		}
		const AnimCtrl::Clip*			AnimCtrl::getStages() const
		{
			return _stages;
		}
		AnimCtrl::Clip*					AnimCtrl::getStages()
		{
			return _stages;
		}
		size_t							AnimCtrl::getStageCount() const
		{
			return _stage_count;
		}
		const AnimCtrl::Clip*			AnimCtrl::getSecondaryStages() const
		{
			return _secondary_stages;
		}
		AnimCtrl::Clip*					AnimCtrl::getSecondaryStages()
		{
			return _secondary_stages;
		}
		size_t							AnimCtrl::getSecondaryStageCount() const
		{
			return _secondary_stage_count;
		}
		void							AnimCtrl::set(const TimeAnimCfg& cfg)
		{
			_stage_count = 1;
			_stages[0].set(cfg);
			_stages[0].setup(1.0f, 1.0f, 0.0f, INFINITY);
		}
		void							AnimCtrl::transit(const TimeAnimCfg& cfg, REAL duration)
		{
			if(!duration)
			{
				set(cfg);
				return;
			}

			LOD::TRANSITION transition = (LOD::isForceTransition() ? LOD::getForcedTransition() : _transition);
			switch(transition)
			{
			case LOD::TRANSITION_NONE:
				set(cfg);
				current_stage().setup(1.0f, 1.0f, 0.0f, INFINITY);
				break;
			case LOD::TRANSITION_BLEND_CAPTURED:
				if(_capture_cb.valid())
				{
					_capture_cb(this, _capture_cb_opaque);
					set(cfg);
					current_stage().setup(0.0f, 1.0f, duration, INFINITY);
				}
				else			// LOD::TRANSITION_NONE
				{
					set(cfg);
					current_stage().setup(1.0f, 1.0f, 0.0f, INFINITY);
				}
				break;
			case LOD::TRANSITION_MULTISTAGE_BLEND:
				if(_stage_count == _countof(_stages))
				{
					memcpy(_stages, _stages + 1, (_countof(_stages) - 1) * sizeof(_stages[0]));
					--_stage_count;
				}
				_stages[_stage_count] = Clip();
				++_stage_count;
				current_stage().set(cfg);
				current_stage().setup(0.0f, 1.0f, duration, INFINITY);
				break;
			}
		}
		void							AnimCtrl::advance(REAL dt)
		{
			for(size_t i = 0; i < _stage_count; ++i)
			{
				_stages[i].advance(dt);
				if(_stages[i].getWeight() == 1.0f && i)
				{
					memcpy(_stages, _stages + i, (_countof(_stages) - i) * sizeof(_stages[0]));
					_stage_count -= i;
					i = 0;
				}
			}
		}
		void							AnimCtrl::setTransition(LOD::TRANSITION transition)
		{
			_transition = transition;
		}
		LOD::TRANSITION					AnimCtrl::getTransition() const
		{
			return _transition;
		}
		void							AnimCtrl::setCaptureCallback(const functor2<void, AnimCtrl*, void*>& cb, void* opaque)
		{
			_capture_cb = cb;
			_capture_cb_opaque = opaque;
		}
		const functor2<void, AnimCtrl*, void*>&
										AnimCtrl::getCaptureCallback() const
		{
			return _capture_cb;
		}
		void*							AnimCtrl::getCaptureCallbackOpaque() const
		{
			return _capture_cb_opaque;
		}
	};
};

