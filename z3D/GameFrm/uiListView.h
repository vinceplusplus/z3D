
#pragma once

#include "predecl.h"

#include "uiBase.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiListView: public uiBase
		{
		private:
			SPtr<uiHeader>				_header;
			SPtr<uiScrollbar>			_hsb;
			SPtr<uiScrollbar>			_vsb;
		private:
			int32_t						_rows;
			int32_t						_row_height;
		private:
			size_t						_use_hscroll;
			size_t						_use_vscroll;
			size_t						_disable_no_hscroll;
			size_t						_disable_no_vscroll;
		private:
			int32_t						_selection;
			CountDown					_countdown_last_click;
		private:
			SPtr<uiScrollbar>			_hsb_ext;
			SPtr<uiScrollbar>			_vsb_ext;
		private:
			wfunctor6<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t>
										_cb_on_draw_row;
			wfunctor7<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t>
										_cb_on_draw_cell;
			wfunctor6<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t>
										_cb_on_draw_selection;
			wfunctor1<void, const SPtr<uiListView>&>
										_cb_on_row_selected;
			wfunctor1<void, const SPtr<uiListView>&>
										_cb_on_row_double_click;
		public:
			uiListView(int32_t x, int32_t y, int32_t dx, int32_t dy);
		protected:
			~uiListView();
		protected:
			void						onDraw();
		protected:
			virtual void				onDrawRow(int32_t row, int32_t x, int32_t y, int32_t dx, int32_t dy);
			virtual void				onDrawCell(int32_t row, int32_t col, int32_t x, int32_t y, int32_t dx, int32_t dy);
			virtual void				onDrawSelection(int32_t row, int32_t x, int32_t y, int32_t dx, int32_t dy);
		protected:
			virtual void				onRowSelected();
			virtual void				onRowDoubleClick();
		protected:
			void						onScrollbarScroll(const SPtr<uiScrollbar>& sb);
		protected:
			void						onHeaderButtonResize(const SPtr<uiHeader>& header, size_t index);
		protected:
			void						onMousePressed(const Vec2& pos, uint32_t button);
		public:
			void						setOnDrawRow(const wfunctor6<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t>& cb);
			void						setOnDrawCell(const wfunctor7<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t>& cb);
			void						setOnDrawSelection(const wfunctor6<void, const uiListView*, int32_t, int32_t, int32_t, int32_t, int32_t>& cb);
			void						setOnRowSelected(const wfunctor1<void, const SPtr<uiListView>&>& cb);
			void						setOnRowDoubleClicked(const wfunctor1<void, const SPtr<uiListView>&>& cb);
		public:
			void						setHeaderHeight(int32_t height);
			int32_t						getHeaderHeight() const;
		public:
			void						setRowHeight(int32_t height);
			int32_t						getRowHeight() const;
		public:
			void						setRows(int32_t rows);
			int32_t						getRows() const;
		public:
			void						setUseHScrollbar(bool use);
			void						setUseVScrollbar(bool use);
			bool						getUseHScrollbar() const;
			bool						getUseVScrollbar() const;
		public:
			void						setDisableNoHScroll(bool disable);
			void						setDisableNoVscroll(bool disable);
			bool						getDisableNoHScroll() const;
			bool						getDisableNoVscroll() const;
		public:
			const SPtr<uiHeader>&		getHeader() const;
			const SPtr<uiScrollbar>&	getHScrollbar() const;
			const SPtr<uiScrollbar>&	getVScrollbar() const;
		public:
			void						setExtHScrollbar(const SPtr<uiScrollbar>& sb);
			void						setExtVScrollbar(const SPtr<uiScrollbar>& sb);
			const SPtr<uiScrollbar>&	getExtHScrollbar() const;
			const SPtr<uiScrollbar>&	getExtVScrollbar() const;
		public:
			void						doLayout();
		public:
			void						getDisplayArea(int32_t& x, int32_t& y, int32_t& dx, int32_t& dy) const;
		public:
			int32_t						getSelection() const;
			void						setSelection(int32_t index);
		public:
			int32_t						getHScrollPos() const;
			int32_t						getVScrollPos() const;
		public:
			int32_t						getVisibleHeaderHeight() const;
		};
	};
};

