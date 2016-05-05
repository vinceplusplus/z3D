
#include "TMCtrl.h"

#include "../Core/functor_inl.h"

#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		TMCtrl::TMCtrl(const wcs& name, const SPtr<SNode>& node)
			: CtrlBase(name)
		{
			_node = node;

			_interpolation = LOD::INTERPOLATION_LINEAR;

			_anim_ctrl.setCaptureCallback(functor2<void, AnimCtrl*, void*>(this, &TMCtrl::capture), NULL);
		}
		TMCtrl::~TMCtrl() {}
		void							TMCtrl::update(REAL dt)
		{
			if(!_node.valid())
				return;

			Z_ASSERT( find(_node.get_unsafe()->getCtrlSet().getArray().begin(), _node.get_unsafe()->getCtrlSet().getArray().end(), this) != _node.get_unsafe()->getCtrlSet().getArray().end());

			_anim_ctrl.advance(dt);

			LOD::TRANSITION transition = LOD::isForceTransition() ? LOD::getForcedTransition() : _anim_ctrl.getTransition();
			LOD::INTERPOLATION interpolation = LOD::isForceInterpolation() ? LOD::getForcedInterpolation() : getInterpolation();

			if(interpolation == LOD::INTERPOLATION_LINEAR)
			{
				switch(transition)
				{
				case LOD::TRANSITION_NONE:
					{
						AnimCtrl::Clip& stage = _anim_ctrl.current_stage();
						if(getPosAnimData())
							_node.get_unsafe()->setPos(_pos_eval.sample(stage.getTime()));
						if(getRotAnimData())
							_node.get_unsafe()->setRot(_rot_eval.sample(stage.getTime()));
						if(getScaleAnimData())
							_node.get_unsafe()->setScale(_scale_eval.sample(stage.getTime()));
					}
					break;
				case LOD::TRANSITION_BLEND_CAPTURED:
					{
						AnimCtrl::Clip& stage = _anim_ctrl.current_stage();
						if(getPosAnimData())
							_node.get_unsafe()->setPos(_pos_eval.sample(stage.getTime()).lerp(_pos_captured, 1 - stage.getWeight()));
						if(getRotAnimData())
							_node.get_unsafe()->setRot(_rot_eval.sample(stage.getTime()).slerp(_rot_captured, 1 - stage.getWeight()));
						if(getScaleAnimData())
							_node.get_unsafe()->setScale(_scale_eval.sample(stage.getTime()).lerp(_scale_captured, 1 - stage.getWeight()));
					}
					break;
				case LOD::TRANSITION_MULTISTAGE_BLEND:
					{
						Vec3 pos;
						Quat rot;
						Vec3 scale;
						bool enable_pos = !!getPosAnimData();
						bool enable_rot = !!getRotAnimData();
						bool enable_scale = !!getScaleAnimData();
						if(true)
						{
							size_t c = _anim_ctrl.getStageCount();
							AnimCtrl::Clip* stages = _anim_ctrl.getStages();
							for(size_t i = 0; i < c; ++i)
							{
								const AnimCtrl::Clip& stage = stages[i];
								if(i == 0)
								{
									if(enable_pos)
										pos = _pos_eval.sample(stage.getTime());
									if(enable_rot)
										rot = _rot_eval.sample(stage.getTime());
									if(enable_scale)
										scale = _scale_eval.sample(stage.getTime());
									continue;
								}
								if(enable_pos)
									pos = pos.lerp(_pos_eval.sample(stage.getTime()), stage.getWeight());
								if(enable_rot)
									rot = rot.slerp(_rot_eval.sample(stage.getTime()), stage.getWeight());
								if(enable_scale)
									scale = scale.lerp(_scale_eval.sample(stage.getTime()), stage.getWeight());
							}
							if(enable_pos)
								_node.get_unsafe()->setPos(pos);
							if(enable_rot)
								_node.get_unsafe()->setRot(rot);
							if(enable_scale)
								_node.get_unsafe()->setScale(scale);
						}
					}
					break;
				}
			}
			else
			{
				switch(transition)
				{
				case LOD::TRANSITION_NONE:
					{
						AnimCtrl::Clip& stage = _anim_ctrl.current_stage();
						if(getPosAnimData())
							_node.get_unsafe()->setPos(_pos_eval.eval_step(stage.getTime()));
						if(getRotAnimData())
							_node.get_unsafe()->setRot(_rot_eval.eval_step(stage.getTime()));
						if(getScaleAnimData())
							_node.get_unsafe()->setScale(_scale_eval.eval_step(stage.getTime()));
					}
					break;
				case LOD::TRANSITION_BLEND_CAPTURED:
					{
						AnimCtrl::Clip& stage = _anim_ctrl.current_stage();
						if(getPosAnimData())
							_node.get_unsafe()->setPos(_pos_eval.eval_step(stage.getTime()).lerp(_pos_captured, 1 - stage.getWeight()));
						if(getRotAnimData())
							_node.get_unsafe()->setRot(_rot_eval.eval_step(stage.getTime()).slerp(_rot_captured, 1 - stage.getWeight()));
						if(getScaleAnimData())
							_node.get_unsafe()->setScale(_scale_eval.eval_step(stage.getTime()).lerp(_scale_captured, 1 - stage.getWeight()));
					}
					break;
				case LOD::TRANSITION_MULTISTAGE_BLEND:
					{
						Vec3 pos;
						Quat rot;
						Vec3 scale;
						bool enable_pos = !!getPosAnimData();
						bool enable_rot = !!getRotAnimData();
						bool enable_scale = !!getScaleAnimData();
						if(true)
						{
							size_t c = _anim_ctrl.getStageCount();
							AnimCtrl::Clip* stages = _anim_ctrl.getStages();
							for(size_t i = 0; i < c; ++i)
							{
								const AnimCtrl::Clip& stage = stages[i];
								if(i == 0)
								{
									if(enable_pos)
										pos = _pos_eval.eval_step(stage.getTime());
									if(enable_rot)
										rot = _rot_eval.eval_step(stage.getTime());
									if(enable_scale)
										scale = _scale_eval.eval_step(stage.getTime());
									continue;
								}
								if(enable_pos)
									pos = pos.lerp(_pos_eval.eval_step(stage.getTime()), stage.getWeight());
								if(enable_rot)
									rot = rot.slerp(_rot_eval.eval_step(stage.getTime()), stage.getWeight());
								if(enable_scale)
									scale = scale.lerp(_scale_eval.eval_step(stage.getTime()), stage.getWeight());
							}
							if(enable_pos)
								_node.get_unsafe()->setPos(pos);
							if(enable_rot)
								_node.get_unsafe()->setRot(rot);
							if(enable_scale)
								_node.get_unsafe()->setScale(scale);
						}
					}
					break;
				}
			}
		}
		void							TMCtrl::capture(AnimCtrl* animctrl, void* opaque)
		{
			if(!_node.valid())
				return;

			SNode* n = _node.get_unsafe();
			_pos_captured = n->pos();
			_rot_captured = n->rot();
			_scale_captured = n->scale();
		}
		void							TMCtrl::setPosAnimData(const SPtr<AnimData<Vec3>>& animdata)
		{
			_pos_eval = animdata;
		}
		void							TMCtrl::setRotAnimData(const SPtr<AnimData<Quat>>& animdata)
		{
			_rot_eval = animdata;
		}
		void							TMCtrl::setScaleAnimData(const SPtr<AnimData<Vec3>>& animdata)
		{
			_scale_eval = animdata;
		}
		const SPtr<AnimData<Vec3>>&		TMCtrl::getPosAnimData() const
		{
			return _pos_eval.anim_data();
		}
		const SPtr<AnimData<Quat>>&		TMCtrl::getRotAnimData() const
		{
			return _rot_eval.anim_data();
		}
		const SPtr<AnimData<Vec3>>&		TMCtrl::getScaleAnimData() const
		{
			return _scale_eval.anim_data();
		}
		bool							TMCtrl::hasAnimData() const
		{
			return true;
		}
		void							TMCtrl::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			const SPtr< AnimData<Vec3> >& pos = getPosAnimData();
			const SPtr< AnimData<Quat> >& rot = getRotAnimData();
			const SPtr< AnimData<Vec3> >& scale = getScaleAnimData();

			start = INFINITY;
			start = min(start, (pos && pos->getKeyCount()) ? pos->getKey(0).time : start);
			start = min(start, (rot && rot->getKeyCount()) ? rot->getKey(0).time : start);
			start = min(start, (scale && scale->getKeyCount()) ? scale->getKey(0).time : start);

			end = -INFINITY;
			end = max(end, (pos && pos->getKeyCount()) ? pos->getKey(pos->getKeyCount() - 1).time : end);
			end = max(end, (rot && rot->getKeyCount()) ? rot->getKey(rot->getKeyCount() - 1).time : end);
			end = max(end, (scale && scale->getKeyCount()) ? scale->getKey(scale->getKeyCount() - 1).time : end);

			start =  (start == INFINITY) ? 0 : start;
			end =  (end == -INFINITY) ? 0 : end;
		}
		AnimCtrl*						TMCtrl::getAnimCtrl()
		{
			return &_anim_ctrl;
		}
		SPtr<SNode>						TMCtrl::getNode() const
		{
			return _node.lock();
		}
		void							TMCtrl::setInterpolation(LOD::INTERPOLATION interpolation)
		{
			_interpolation = interpolation;
		}
		LOD::INTERPOLATION				TMCtrl::getInterpolation() const
		{
			return _interpolation;
		}
		ptrdiff_t						TMCtrl::getPriority() const
		{
			return 30000;
		}
		bool							TMCtrl::isUpdateDeferred() const
		{
			return false;
		}
	};
};

