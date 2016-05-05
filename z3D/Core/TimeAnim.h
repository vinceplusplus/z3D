
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class TimeAnim
		{
		private:
			REAL					_start;
			REAL					_end;
			ptrdiff_t				_loops;
			REAL					_loop_start;
			REAL					_loop_end;
		private:
			REAL					_speed;
		private:
			REAL					_time;
			size_t					_needs_update;
		private:
			size_t					_enable_auto_advance;
			uint64_t				_auto_advance_last_tick;
		private:
			size_t					_reverse;
		private:
			static Timer			_auto_advance_timer;
		public:
			TimeAnim();
			TimeAnim(const TimeAnimCfg& cfg);
		public:
			TimeAnim(const TimeAnim& rhs);
			TimeAnim&				operator=(const TimeAnim& rhs);
		public:
			REAL					getStart() const;
			REAL					getEnd() const;
			ptrdiff_t				getLoops() const;
			REAL					getLoopStart() const;
			REAL					getLoopEnd() const;
			REAL					getSpeed() const;
			REAL					getTime() const;
		public:
			bool					needsUpdate() const;
		public:
			bool					isAutoAdvanceEnabled() const;
		public:
			bool					isStopped() const;
		public:
			void					setLoops(ptrdiff_t loops);
			void					setSpeed(REAL speed);
			void					setTime(REAL time);
			void					enableAutoAdvance(bool enable);
		public:
			void					set(REAL start, REAL end, ptrdiff_t loops, REAL loop_start, REAL loop_end, REAL time);
		public:
			void					set(const TimeAnimCfg& cfg);
			void					set(const TimeAnim& rhs);
		public:
			void					setNeedsUpdate(bool needs);
		public:
			bool					advance(REAL dt);									// true - needs update
		public:
			bool					autoAdvance();
		};

		class TimeAnimCfg
		{
		public:
			union
			{
				uint32_t				_FLAGS;
				struct
				{
					uint8_t					_flag_start : 2;							// 0 - unspecified, 1 - specified, 2 - individual min, // OBSOLETE 3 - aggregate min
					uint8_t					_flag_end : 2;								// 0 - unspecified, 1 - specified, 2 - individual max, // OBSOLETE 3 - aggregate max
					uint8_t					_flag_loops : 1;							// 0 - unspedified, 1 - specified
					uint8_t					_flag_loop_start : 1;						// 0 - unspedified, 1 - specified
					uint8_t					_flag_loop_end : 1;							// 0 - unspedified, 1 - specified
					uint8_t					_flag_speed : 1;							// 0 - unspedified, 1 - specified
					uint8_t					_flag_time : 2;								// 0 - unspedified, 1 - specified, 2 - use start
					uint8_t					_flag_needs_update : 1;						// 0 - unspedified, 1 - specified
					uint8_t					_flag_enable_auto_advance : 1;				// 0 - unspedified, 1 - specified
				};
			};
		public:
			REAL					_start;
			REAL					_end;
			ptrdiff_t				_loops;
			REAL					_loop_start;
			REAL					_loop_end;
		public:
			REAL					_speed;
		public:
			REAL					_time;
			size_t					_needs_update;
		public:
			size_t					_enable_auto_advance;
		public:
			TimeAnimCfg();
			TimeAnimCfg(REAL start, REAL end, ptrdiff_t loops, REAL loop_start, REAL loop_end, REAL time);
			TimeAnimCfg(REAL start, REAL end, ptrdiff_t loops, REAL time);
			TimeAnimCfg(REAL start, REAL end, ptrdiff_t loops);
		private:
			void					rectify();
		public:
			TimeAnimCfg&			setLoops(ptrdiff_t loops);
			TimeAnimCfg&			setSpeed(REAL speed);
			TimeAnimCfg&			setTime(REAL t);
			TimeAnimCfg&			setNeedsUpdate(bool needs);
			TimeAnimCfg&			enableAutoAdvance(bool enable);
		public:
			TimeAnimCfg&			preserveTime();
		public:
			TimeAnimCfg&			setStartIndividualMin();
			//TimeAnimCfg&			setStartAggregateMin();
			TimeAnimCfg&			setEndIndividualMax();
			//TimeAnimCfg&			setEndAggregateMax();
			TimeAnimCfg&			setTimeToStart();
		};
	};
};

