
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace SG
	{
		// make morph and skin inherit from VertexDeform
		//
		// inside morph / skin, check to see if it is the first enabled controller to do vertex deform, 
		// if so, take reference mesh as the source, otherwise, take the current buffer as source
		//
		// but it can be flawed if a controller which updates an object is not attached to that object
		//

		class TMCtrl: public CtrlBase
		{
			DECL_TYPEINFO();
		private:
			AnimCtrl						_anim_ctrl;
		private:
			AnimDataEval<Vec3>				_pos_eval;
			AnimDataEval<Quat>				_rot_eval;
			AnimDataEval<Vec3>				_scale_eval;
		private:
			Vec3							_pos_captured;
			Quat							_rot_captured;
			Vec3							_scale_captured;
		private:
			WPtr<SNode>						_node;
		private:
			LOD::INTERPOLATION				_interpolation;
		public:
			TMCtrl(const wcs& name, const SPtr<SNode>& node);
		protected:
			~TMCtrl();
		public:
			void							update(REAL dt);
		public:
			void							capture(AnimCtrl* animctrl, void* opaque);
		public:
			void							setPosAnimData(const SPtr<AnimData<Vec3>>& animdata);
			void							setRotAnimData(const SPtr<AnimData<Quat>>& animdata);
			void							setScaleAnimData(const SPtr<AnimData<Vec3>>& animdata);
		public:
			const SPtr<AnimData<Vec3>>&		getPosAnimData() const;
			const SPtr<AnimData<Quat>>&		getRotAnimData() const;
			const SPtr<AnimData<Vec3>>&		getScaleAnimData() const;
		public:
			bool							hasAnimData() const;
			void							getAnimDataTimeRange(REAL& start, REAL& end) const;
			AnimCtrl*						getAnimCtrl();
		public:
			SPtr<SNode>						getNode() const;
		public:
			void							setInterpolation(LOD::INTERPOLATION interpolation);
			LOD::INTERPOLATION				getInterpolation() const;
		public:
			ptrdiff_t						getPriority() const;
			bool							isUpdateDeferred() const;
		};

	};
};

