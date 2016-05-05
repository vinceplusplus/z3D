
#include "CameraCtrl.h"

namespace z3D
{
	namespace SG
	{
		CameraCtrl::CameraCtrl(const wcs& name, const SPtr<SCameraNode>& node)
			: CtrlBase(name), _camera_node(node), _eval_yfov(NULL), _eval_znear(NULL), _eval_zfar(NULL), _eval_target_dist(NULL)
		{
		}
		CameraCtrl::~CameraCtrl()
		{
		}
		bool							CameraCtrl::hasAnimData() const
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
		void							CameraCtrl::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			start = INFINITY;
			start = min(getStartTime(_eval_yfov, start), start);
			start = min(getStartTime(_eval_znear, start), start);
			start = min(getStartTime(_eval_zfar, start), start);
			start = min(getStartTime(_eval_target_dist, start), start);

			end = -INFINITY;
			end = max(getEndTime(_eval_yfov, end), end);
			end = max(getEndTime(_eval_znear, end), end);
			end = max(getEndTime(_eval_zfar, end), end);
			end = max(getEndTime(_eval_target_dist, end), end);
		}
		AnimCtrl*						CameraCtrl::getAnimCtrl()
		{
			return &_anim_ctrl;
		}
		void							CameraCtrl::update(REAL dt)
		{
			if(!_camera_node.valid())
				return;

			Z_ASSERT( find(_camera_node.get_unsafe()->getCtrlSet().getArray().begin(), _camera_node.get_unsafe()->getCtrlSet().getArray().end(), this) != _camera_node.get_unsafe()->getCtrlSet().getArray().end());

			_anim_ctrl.advance(dt);

			SPtr<SCameraNode> c = _camera_node.lock();

			REAL t = _anim_ctrl.current_stage().getTime();

			REAL yfov = c->yfov();
			REAL znear = c->znear();
			REAL zfar = c->zfar();
			if(_eval_yfov.anim_data())
				yfov = _eval_yfov.sample(t);
			if(_eval_znear.anim_data())
				znear = _eval_znear.sample(t);
			if(_eval_zfar.anim_data())
				zfar = _eval_zfar.sample(t);

			if(c->isOrtho())
				c->setOrtho(yfov, znear, zfar);
			else
				c->setPerspective(yfov, znear, zfar);

			if(_eval_target_dist.anim_data())
				c->setTargetDist(_eval_target_dist.eval_lerp(t));
		}
		ptrdiff_t						CameraCtrl::getPriority() const
		{
			return 3000;
		}
		bool							CameraCtrl::isUpdateDeferred() const
		{
			return false;
		}
		void							CameraCtrl::setAnimDataYFov(const SPtr< AnimData<REAL> >& animdata)
		{
			_eval_yfov = animdata;
		}
		void							CameraCtrl::setAnimDataZNear(const SPtr< AnimData<REAL> >& animdata)
		{
			_eval_znear = animdata;
		}
		void							CameraCtrl::setAnimDataZFar(const SPtr< AnimData<REAL> >& animdata)
		{
			_eval_zfar = animdata;
		}
		void							CameraCtrl::setAnimDataTargetDist(const SPtr< AnimData<REAL> >& animdata)
		{
			_eval_target_dist = animdata;
		}
		const SPtr< AnimData<REAL> >&	CameraCtrl::getAnimDataYFov() const
		{
			return _eval_yfov.anim_data();
		}
		const SPtr< AnimData<REAL> >&	CameraCtrl::getAnimDataZNear() const
		{
			return _eval_znear.anim_data();
		}
		const SPtr< AnimData<REAL> >&	CameraCtrl::getAnimDataZFar() const
		{
			return _eval_zfar.anim_data();
		}
		const SPtr< AnimData<REAL> >&	CameraCtrl::getAnimDataTargetDist() const
		{
			return _eval_target_dist.anim_data();
		}
	};
};

