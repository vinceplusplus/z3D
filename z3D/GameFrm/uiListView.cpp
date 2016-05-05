
#include "uiListView.h"
#include "uiMgr.h"

#include "uiHeader.h"
#include "uiScrollbar.h"

#include "../Core/functor_weak_inl.h"

namespace z3D
{
	namespace GameFrm
	{
		uiListView::uiListView(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			_use_hscroll = true;
			_use_vscroll = true;
			_disable_no_hscroll = false;
			_disable_no_vscroll = false;

			_rows = 0;
			_row_height = 20;
			_selection = -1;
			_countdown_last_click.setCountDown(0.4f);
			_countdown_last_click.setRemaining(-1.0f);

			setPos(x, y);
			setExt(dx, dy);
			setClip(true);

			_header = new uiHeader(0, 0 , 0 , 20);
			_header->setOnHeaderButtonResize(wfunctor2<void, const SPtr<uiHeader>&, size_t>(this, &uiListView::onHeaderButtonResize));
			addChild(_header);

			_hsb = new uiScrollbar(0, 0 , 20, 20, 0, 0);
			_hsb->setTopMost(true);
			_hsb->doLayout_HScrollbar();
			_hsb->setVisible(false);
			_hsb->setOnScrollbarScroll(wfunctor1<void, const SPtr<uiScrollbar>&>(this, &uiListView::onScrollbarScroll));
			addChild(_hsb);

			_vsb = new uiScrollbar(0, 0, 20, 20, 0, 0);
			_vsb->setTopMost(true);
			_vsb->doLayout_VScrollbar();
			_vsb->setVisible(false);
			_vsb->setOnScrollbarScroll(wfunctor1<void, const SPtr<uiScrollbar>&>(this, &uiListView::onScrollbarScroll));
			addChild(_vsb);
		}
		uiListView::~uiListView()
		{}
		void				uiListView::onDraw()
		{
			int32_t disp_x, disp_y, disp_dx, disp_dy;
			getDisplayArea(disp_x, disp_y, disp_dx, disp_dy);

			Vec2 abspos = getAbsPos();
			int32_t absx = (int32_t)(abspos.x);
			int32_t absy = (int32_t)(abspos.y);

			Mat4 world = Mat4::translation((Vec3)abspos);

			// TODO: get rid of uiMgr::current()
			uiMgr* uimgr = uiMgr::current();
			//uimgr->setScissorLT_push(absx+disp_x,absy+disp_y,disp_dx,disp_dy);

			uimgr->pushViewState_intersectClipRect(absx + disp_x, absy + disp_y, disp_dx, disp_dy);
			uimgr->setTransforms(world);

			int32_t hscroll_pos = getHScrollPos();
			int32_t vscroll_pos = getVScrollPos();

			int32_t row_start = vscroll_pos / _row_height;
			int32_t row_end = (vscroll_pos + disp_dy) / _row_height;
			row_start = max(0, row_start);
			row_end = min(_rows - 1, row_end);

			int32_t cols = (int32_t)_header->getHeaderBtnCount();

			for(int32_t j = row_start; j <= row_end; ++j)
			{
				int32_t y = j * _row_height;

				//if(_listview_render_handler)
				//{
				//	uimgr->setScissorLT_push(absx,absy+getVisibleHeaderHeight()+y-vscroll_pos,disp_dx,_row_height);
				//	_listview_render_handler->listview_draw_row(this,j,0,getVisibleHeaderHeight()+y-vscroll_pos,disp_dx,_row_height);
				//	uimgr->setScissorLT_pop();
				//}
				uimgr->pushViewState_intersectClipRect(absx, absy + getVisibleHeaderHeight() + y - vscroll_pos, disp_dx, _row_height);
				uimgr->setTransforms(world);
				onDrawRow(j, 0, getVisibleHeaderHeight() + y - vscroll_pos, disp_dx, _row_height);
				uimgr->popViewState();

				for(int32_t i = 0; i < cols; ++i)
				{
					SPtr<uiButton> headerbtn = _header->getHeaderBtn(i);

					//uimgr->setScissorLT_push(absx+headerbtn->getX()-hscroll_pos,absy+getVisibleHeaderHeight()+y-vscroll_pos,headerbtn->getDX(),_row_height);

					//if(_listview_render_handler)
					//	_listview_render_handler->listview_draw_cell(this,j,i,headerbtn->getX()-hscroll_pos,getVisibleHeaderHeight()+y-vscroll_pos,headerbtn->getDX(),_row_height);

					//uimgr->setScissorLT_pop();

					uimgr->pushViewState_intersectClipRect(absx + headerbtn->getX() - hscroll_pos, absy + getVisibleHeaderHeight() + y - vscroll_pos, headerbtn->getDX(), _row_height);
					uimgr->setTransforms(world);
					onDrawCell(j, i, headerbtn->getX() - hscroll_pos, getVisibleHeaderHeight() + y - vscroll_pos, headerbtn->getDX(), _row_height);
					uimgr->popViewState();
				}
			}
			if(_selection >= 0)
			{
				uimgr->setTransforms(world);
				onDrawSelection(_selection, 0, getVisibleHeaderHeight() + _selection * _row_height - vscroll_pos, disp_dx, _row_height);
			}
			//if(_listview_render_handler)
			//	if(_selection>=0)
			//		_listview_render_handler->listview_draw_selection(this,_selection,0,getVisibleHeaderHeight()+_selection*_row_height-vscroll_pos,disp_dx,_row_height);

			uimgr->popViewState();
			//uimgr->setScissorLT_pop();
		}
		void				uiListView::onDrawRow(int32_t row, int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			if(_cb_on_draw_row.valid())
				_cb_on_draw_row(this, row, x, y, dx, dy);
		}
		void				uiListView::onDrawCell(int32_t row, int32_t col, int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			if(_cb_on_draw_cell.valid())
				_cb_on_draw_cell(this, row, col, x, y, dx, dy);
		}
		void				uiListView::onDrawSelection(int32_t row, int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			if(_cb_on_draw_selection.valid())
				_cb_on_draw_selection(this, row, x, y, dx, dy);
		}
		void				uiListView::onRowSelected()
		{
			if(_cb_on_row_selected.valid())
				_cb_on_row_selected(this);
		}
		void				uiListView::onRowDoubleClick()
		{
			if(_cb_on_row_double_click.valid())
				_cb_on_row_double_click(this);
		}
		void						uiListView::onScrollbarScroll(const SPtr<uiScrollbar>& sb)
		{
			doLayout();
		}
		void						uiListView::onHeaderButtonResize(const SPtr<uiHeader>& header, size_t index)
		{
			doLayout();
		}
		void						uiListView::onMousePressed(const Vec2& pos, uint32_t button)
		{
			int32_t sel = (int32_t)((pos.y - getVisibleHeaderHeight() + getVScrollPos()) / _row_height);
			if(sel < 0 || sel >= _rows)
				sel = -1;
			_selection = sel;
			_countdown_last_click.update();
			if(_countdown_last_click.remaining() > 0.0f)
			{
				_countdown_last_click.setRemaining(-1.0f);

				if(_selection != -1)
					onRowDoubleClick();
			}
			else
			{
				_countdown_last_click.start();

				onRowSelected();
			}
		}
		void						uiListView::setOnDrawRow(const wfunctor6<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t>& cb)
		{
			_cb_on_draw_row = cb;
		}
		void						uiListView::setOnDrawCell(const wfunctor7<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t>& cb)
		{
			_cb_on_draw_cell = cb;
		}
		void						uiListView::setOnDrawSelection(const wfunctor6<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t>& cb)
		{
			_cb_on_draw_selection = cb;
		}
		void						uiListView::setOnRowSelected(const wfunctor1<void, const SPtr<uiListView>&>& cb)
		{
			_cb_on_row_selected = cb;
		}
		void						uiListView::setOnRowDoubleClicked(const wfunctor1<void, const SPtr<uiListView>&>& cb)
		{
			_cb_on_row_double_click = cb;
		}
		void				uiListView::setHeaderHeight(int32_t height)
		{
			_header->setExt(_header->getDX(), height);
			_header->doLayout();
			doLayout();
		}
		int32_t				uiListView::getHeaderHeight() const
		{
			return _header->getDY();
		}
		void				uiListView::setRowHeight(int32_t height)
		{
			_row_height = height;
			doLayout();
		}
		int32_t				uiListView::getRowHeight() const
		{
			return _row_height;
		}
		void				uiListView::setRows(int32_t rows)
		{
			_rows = rows;
			doLayout();
		}
		int32_t				uiListView::getRows() const
		{
			return _rows;
		}
		void				uiListView::setUseHScrollbar(bool use)
		{
			_use_hscroll = use;
			doLayout();
		}
		void				uiListView::setUseVScrollbar(bool use)
		{
			_use_vscroll = use;
			doLayout();
		}
		bool				uiListView::getUseHScrollbar() const
		{
			return !!_use_hscroll;
		}
		bool				uiListView::getUseVScrollbar() const
		{
			return !!_use_vscroll;
		}
		void				uiListView::setDisableNoHScroll(bool disable)
		{
			_disable_no_hscroll = disable;
			doLayout();
		}
		void				uiListView::setDisableNoVscroll(bool disable)
		{
			_disable_no_vscroll = disable;
			doLayout();
		}
		bool				uiListView::getDisableNoHScroll() const
		{
			return !!_disable_no_hscroll;
		}
		bool				uiListView::getDisableNoVscroll() const
		{
			return !!_disable_no_vscroll;
		}
		const SPtr<uiHeader>&		uiListView::getHeader() const
		{
			return _header;
		}
		const SPtr<uiScrollbar>&	uiListView::getHScrollbar() const
		{
			return _hsb;
		}
		const SPtr<uiScrollbar>&	uiListView::getVScrollbar() const
		{
			return _vsb;
		}
		void				uiListView::setExtHScrollbar(const SPtr<uiScrollbar>& sb)
		{
			if(_hsb_ext)
				_hsb_ext->setOnScrollbarScroll(wfunctor1<void, const SPtr<uiScrollbar>&>());

			_hsb_ext = sb;
			_hsb_ext->setOnScrollbarScroll(wfunctor1<void, const SPtr<uiScrollbar>&>(this, &uiListView::onScrollbarScroll));
			doLayout();
		}
		void				uiListView::setExtVScrollbar(const SPtr<uiScrollbar>& sb)
		{
			if(_vsb_ext)
				_vsb_ext->setOnScrollbarScroll(wfunctor1<void, const SPtr<uiScrollbar>&>());

			_vsb_ext = sb;
			_vsb_ext->setOnScrollbarScroll(wfunctor1<void, const SPtr<uiScrollbar>&>(this, &uiListView::onScrollbarScroll));
			doLayout();
		}
		const SPtr<uiScrollbar>&			uiListView::getExtHScrollbar() const
		{
			return _hsb_ext;
		}
		const SPtr<uiScrollbar>&			uiListView::getExtVScrollbar() const
		{
			return _vsb_ext;
		}
		void				uiListView::doLayout()
		{
			_header->doLayout();
			_header->setPos(0, 0);

			bool hsb = false;
			bool vsb = false;

			if(_use_hscroll)
				hsb = !!_disable_no_hscroll;
			if(_use_vscroll)
				vsb = !!_disable_no_vscroll;

			if(_use_hscroll && !_disable_no_hscroll)
			{
				if(_header->getTotalLength() > getDX())
					hsb = true;
			}
			if(_use_vscroll && !_disable_no_vscroll)
			{
				if(_rows * _row_height > getDY() - getVisibleHeaderHeight() - (hsb ? _hsb->getDY() : 0))
					vsb = true;
			}
			if(_use_hscroll && !_disable_no_hscroll)
			{
				if(_header->getTotalLength() > getDX() - (vsb ? _vsb->getDX() : 0))
					hsb = true;
			}

			if(hsb)
			{
				_hsb->setVisible(true);
				_hsb->setPos(0, getDY() - _hsb->getDY());
				_hsb->setExt(getDX() - (vsb ? _vsb->getDX() : 0), _hsb->getDY());
				_hsb->setScrollInfo(0, _header->getTotalLength() - getDX() + (vsb ? _vsb->getDX() : 0), 1, 1);
				_hsb->doLayout();
			}
			else
			{
				_hsb->setScrollPos(0);
				_hsb->setVisible(false);
			}

			if(vsb)
			{
				_vsb->setVisible(true);
				_vsb->setPos(getDX() - _vsb->getDX(), 0);
				_vsb->setExt(_vsb->getDX(), getDY() - (hsb ? _hsb->getDY() : 0));
				_vsb->setScrollInfo(0, _rows * _row_height - (getDY() - (hsb ? _hsb->getDY() : 0) - getVisibleHeaderHeight()), 1, 1);
				_vsb->doLayout();
			}
			else
			{
				_vsb->setScrollPos(0);
				_vsb->setVisible(false);
			}

			if(_hsb_ext)
			{
				_hsb_ext->setScrollInfo(0, _header->getTotalLength() - getDX() - (vsb ? _vsb->getDX() : 0), 1, 1);
				_hsb_ext->doLayout();
			}

			if(_vsb_ext)
			{
				_vsb_ext->setScrollInfo(0, _rows * _row_height - (getDY() - (hsb ? _hsb->getDY() : 0) - getVisibleHeaderHeight()), 1, 1);
				_vsb_ext->doLayout();
			}

			_header->setPos(-(_hsb_ext ? _hsb_ext->getScrollPos() : (_hsb->isVisible() ? _hsb->getScrollPos() : 0)), 0);
		}
		void				uiListView::getDisplayArea(int32_t& x, int32_t& y, int32_t& dx, int32_t& dy) const
		{
			x = 0;
			y = 0;
			dx = getDX();
			dy = getDY();

			// subtract header area
			y += getVisibleHeaderHeight();
			dy -= getVisibleHeaderHeight();

			// subtract horizontal scrollbar
			dx -= _vsb->isVisible() ? _vsb->getDX() : 0;

			// subtract vertical scrollbar
			dy -= _hsb->isVisible() ? _hsb->getDY() : 0;
		}
		int32_t				uiListView::getSelection() const
		{
			return _selection;
		}
		void				uiListView::setSelection(int32_t index)
		{
			_selection = index;
		}
		int32_t						uiListView::getHScrollPos() const
		{
			return (_hsb_ext ? _hsb_ext->getScrollPos() : (_hsb->isVisible() ? _hsb->getScrollPos() : 0));
		}
		int32_t						uiListView::getVScrollPos() const
		{
			return (_vsb_ext ? _vsb_ext->getScrollPos() : (_vsb->isVisible() ? _vsb->getScrollPos() : 0));
		}
		int32_t						uiListView::getVisibleHeaderHeight() const
		{
			return _header->isVisible() ? _header->getDY() : 0;
		}
	};
};

