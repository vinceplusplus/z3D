
#pragma once

#include "predecl.h"

#include "uiBase.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiMover: public uiBase
		{
		private:
			Vec2				_last_pos;
			Vec2				_last_parent_pos;
		private:
			Vec2				_movable_start;
			Vec2				_movable_extent;
		private:
			wfunctor1<void, const SPtr<uiMover>&>
							_cb_on_mover_move;
		public:
			uiMover();
		protected:
			~uiMover();
		protected:
			void				onMousePressed(const Vec2& pos, uint32_t button);
			void				onMouseReleased(const Vec2& pos, uint32_t button);
			void				onMouseMoved(const Vec2& pos);
		public:
			void				setMovableRect(const Vec2& start, const Vec2& extent);
			void				getMovableRect(Vec2& start, Vec2& extent);
		public:
			void				setOnMoverMove(const wfunctor1<void, const SPtr<uiMover>&>& cb);
		};
	};
};

