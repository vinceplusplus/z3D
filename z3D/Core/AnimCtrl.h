
#pragma once

#include "predecl.h"

#include "LOD.h"

#include "TimeAnim.h"

#include "functor.h"

namespace z3D
{
	namespace Core
	{
		// TODO: add secondary stage support

		class AnimCtrl
		{
		public:
			class Clip: public TimeAnim
			{
			private:
				REAL							_weight;
				REAL							_max_weight;
				REAL							_weight_incr_rate;
				REAL							_time_left;									// for primary clip, it should be INFINITY
			public:
				Clip();
				Clip(REAL weight, REAL max_weight, REAL transition_time, REAL time_left);
			public:
				void							setup(REAL weight, REAL max_weight, REAL transition_time, REAL time_left);
			public:
				bool							advance(REAL dt);
			public:
				REAL							getWeight() const;
				REAL							getMaxWeight() const;
				REAL							getWeightIncrRate() const;
				REAL							getTimeLeft() const;
			public:
				bool							isEffective() const;
			};
		private:							// multi-stage blending
			Clip							_stages[8];											// primary stages
			size_t							_stage_count;
		private:
			Clip							_secondary_stages[8];
			size_t							_secondary_stage_count;
		private:
			LOD::TRANSITION					_transition;
		private:
			functor2<void, AnimCtrl*, void*>
											_capture_cb;
			void*							_capture_cb_opaque;
		public:
			AnimCtrl();
		public:
			const Clip&						current_stage() const;
			Clip&							current_stage();
			const Clip*						getStages() const;
			Clip*							getStages();
			size_t							getStageCount() const;
			const Clip*						getSecondaryStages() const;
			Clip*							getSecondaryStages();
			size_t							getSecondaryStageCount() const;
		public:
			void							set(const TimeAnimCfg& cfg);
		public:
			void							transit(const TimeAnimCfg& cfg, REAL duration);
		public:
			void							advance(REAL dt);					// will erase no longer needed stage
		public:
			void							setTransition(LOD::TRANSITION transition);
			LOD::TRANSITION					getTransition() const;
		public:
			void							setCaptureCallback(const functor2<void, AnimCtrl*, void*>& cb, void* opaque);
			const functor2<void, AnimCtrl*, void*>&
											getCaptureCallback() const;
			void*							getCaptureCallbackOpaque() const;
		};
	};
};

