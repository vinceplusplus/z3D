
#pragma once

#include "predecl.h"
#include "SCameraNode.h"

namespace z3D
{
	namespace SG
	{
		class CameraCtrl: public CtrlBase
		{
		public:
			DECL_TYPEINFO();
		private:
			AnimDataEval<REAL>				_eval_yfov;
			AnimDataEval<REAL>				_eval_znear;
			AnimDataEval<REAL>				_eval_zfar;
			AnimDataEval<REAL>				_eval_target_dist;
		private:
			WPtr<SCameraNode>				_camera_node;
		private:
			AnimCtrl						_anim_ctrl;
		public:
			CameraCtrl(const wcs& name, const SPtr<SCameraNode>& node);
		protected:
			~CameraCtrl();
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
			void							setAnimDataYFov(const SPtr< AnimData<REAL> >& animdata);
			void							setAnimDataZNear(const SPtr< AnimData<REAL> >& animdata);
			void							setAnimDataZFar(const SPtr< AnimData<REAL> >& animdata);
			void							setAnimDataTargetDist(const SPtr< AnimData<REAL> >& animdata);
		public:
			const SPtr< AnimData<REAL> >&	getAnimDataYFov() const;
			const SPtr< AnimData<REAL> >&	getAnimDataZNear() const;
			const SPtr< AnimData<REAL> >&	getAnimDataZFar() const;
			const SPtr< AnimData<REAL> >&	getAnimDataTargetDist() const;
		};
	};
};

