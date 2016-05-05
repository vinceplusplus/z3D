
#pragma once

#include "predecl.h"

#include "ResBase.h"
#include "TypeStuff.h"

namespace z3D
{
	namespace Core
	{
		class AnimInst
		{
			DECL_TYPEINFO();
		private:
			REAL			_start;
			REAL			_end;
			int32_t			_loops;
			REAL			_loop_start;
			REAL			_loop_end;
		private:
			REAL			_speed;
		private:
			REAL			_time;
			bool			_need_update;
		private:
			size_t			_auto_advance;
			int64_t			_auto_advance_last;
		private:
			size_t			_reverse;
		private:
			static Timer	_auto_advance_timer;
		private:
			void			___ctor(REAL start, REAL end, int32_t loops, REAL loop_start, REAL loop_end, REAL time);
		public:
			AnimInst();
			AnimInst(REAL start, REAL end, int32_t loops);
			AnimInst(REAL start, REAL end, int32_t loops, REAL time);
			AnimInst(REAL start, REAL end, int32_t loops, REAL loop_start, REAL loop_end, REAL time);
			virtual ~AnimInst();
		public:
			REAL			getStart() const;
			REAL			getEnd() const;
			int32_t			getLoops() const;
			REAL			getLoopStart() const;
			REAL			getLoopEnd() const;
			REAL			getSpeed() const;
			REAL			getTime() const;
			bool			autoAdvanced() const;
			bool			need_update() const;
		public:
			void			setLoops(int32_t loops);
			void			setSpeed(REAL speed);
			void			setTime(REAL time);
			void			autoAdvance(bool auto_advance);
		public:
			void			set(REAL start, REAL end, int32_t loops, REAL loop_start, REAL loop_end, REAL time);
			void			set(REAL start, REAL end, int32_t loops);
			void			set(REAL start, REAL end, int32_t loops, REAL time);
			void			set(const AnimInst& other);
		public:
			void			setNeedUpdate(bool need_update);
		public:
			void			advance(REAL dt);
		public:
			void			update();
		public:
			bool			stopped() const;
		};
	};
};

