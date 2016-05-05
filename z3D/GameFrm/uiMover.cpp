
#include "uiMover.h"
#include "uiMgr.h"

namespace z3D
{
	namespace GameFrm
	{
		uiMover::uiMover()
		{
			_movable_start = Vec2(-4096, -4096);
			_movable_extent = Vec2(8096, 8096);
		}
		uiMover::~uiMover()
		{}
		void				uiMover::onMousePressed(const Vec2& pos, uint32_t button)
		{
			if(button == 0)									// if it is a left click
			{
				if(getParent())
				{
					_last_pos = getAbsPos() + pos;
					_last_parent_pos = getParent()->getAbsPos();
				}
				uiMgr::current()->setCapture(this);					// and set exclusive mouse event listening
			}
		}
		void				uiMover::onMouseReleased(const Vec2& pos, uint32_t button)
		{
			if(uiMgr::current()->getCapture())
				uiMgr::current()->releaseCapture(this);					// release that capture if no longer useful
		}
		void				uiMover::onMouseMoved(const Vec2& pos)
		{
			if(uiMgr::current()->getCapture().get_unsafe() == this)					// when mouse move, if the capturing UI is this one then ...
			{
				if(getParent())
				{
					Vec2 new_pos = _last_parent_pos + (pos + getAbsPos()) - _last_pos - (getParent()->getParent() ? getParent()->getParent()->getAbsPos() : 0);
					Vec2 clip_pos = new_pos;
					clip_pos.x = min(max(_movable_start.x, clip_pos.x), _movable_start.x + _movable_extent.x);
					clip_pos.y = min(max(_movable_start.y, clip_pos.y), _movable_start.y + _movable_extent.y);
					getParent()->setPos(clip_pos);							// move the parent
					if(_cb_on_mover_move.valid())
						_cb_on_mover_move(this);
				}
			}
		}
		void				uiMover::setMovableRect(const Vec2& start, const Vec2& extent)
		{
			_movable_start = start;
			_movable_extent = extent;
		}
		void				uiMover::getMovableRect(Vec2& start, Vec2& extent)
		{
			start = _movable_start;
			extent = _movable_extent;
		}
		void				uiMover::setOnMoverMove(const wfunctor1<void, const SPtr<uiMover>&>& cb)
		{
			_cb_on_mover_move = cb;
		}
	};
};

