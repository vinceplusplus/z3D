
#pragma once

#include "predecl.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace GameFrm
	{
		class W32Wnd
		{
		protected:
			static map<HWND,W32Wnd*>	_map_hwnd_obj;
		protected:
			HWND						_hwnd;
			WNDPROC						_old_wndproc;
			bool						_subclass;
			//bool						_suppress_destroy;
		protected:
			bool						_enable_wndmover;
			bool						_wndmover_skip_def_process;
			RECT						_rt_wndmover;
			POINT						_last_pt_wndmover;
			bool						_wndmoving;
			bool						_wndmover_restrict;
			RECT						_wndmover_restrict_rt;
		protected:
			unsigned long				_msg_loop_sleep;
		protected:
			static HCURSOR				_default_cursor;
			//static HDC					_printclient_hdc;
			//static HRGN					_printclient_rgn;
			//static HRGN					_empty_rgn;
			//static bool					_printclienting;
			//static HDC					_backup_hdc;
			//static HBITMAP				_backup_hbitmap;
			//static HBITMAP				_offscreen_hbitmap;
		public:
			W32Wnd();
			virtual ~W32Wnd();
		public:
			virtual HWND				getHWND() const;
		public:
			static HFONT				createFont(const wcs& face,int height,int weight,bool italic,bool underline,bool strikeout);
		protected:
			static LRESULT CALLBACK		WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			static bool					real_wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,LRESULT& ret);
			static W32Wnd*				getWndObj(HWND hWnd);
			//static void					restore_printclient_region();
			//static bool					printclienting();
		protected:			// some delegation...
			virtual void				post_create();
			virtual bool				owner_draw(HDC hdc,int id,const DRAWITEMSTRUCT& dis);			// return false if not custom draw item
			virtual bool				notify_loopback(UINT code,NMHDR& hdr,LRESULT& ret);
			//virtual bool				printclient_empty_region() const;								// return true to indicate before printclient, set an empty region, use restore_printclient_region() to restore the correct region
			//virtual bool				need_backup() const;
		public:
			virtual void				msg_loop();
			virtual bool				msg_hook(const MSG& msg);										// return false to skip processing in the default way
		protected:
			virtual void				user_proc();
		protected:			// NOTE the return value is important, return false if you want to handle the message by DefWindowProc
			virtual bool				wm_create(HWND hwnd,const CREATESTRUCT& cs);
			virtual bool				wm_close();
			virtual bool				wm_sizing(WPARAM wParam,RECT& rt);
			virtual bool				wm_size(WPARAM wParam,int width,int height);
			virtual bool				wm_destroy();
			virtual bool				wm_paint();												// implementation should be calling BeginPaint(...), wm_printclient(...) and EndPaint(...)
			virtual bool				wm_printclient(HDC hdc);								// MUST PROCESS THIS MESSAGE
			virtual bool				wm_lbuttondown(WPARAM wParam,int x,int y);
			virtual bool				wm_lbuttonup(WPARAM wParam,int x,int y);
			virtual bool				wm_lbuttondblclk(WPARAM wParam,int x,int y);
			virtual bool				wm_rbuttondown(WPARAM wParam,int x,int y);
			virtual bool				wm_rbuttonup(WPARAM wParam,int x,int y);
			virtual bool				wm_rbuttondblclk(WPARAM wParam,int x,int y);
			virtual bool				wm_mbuttondown(WPARAM wParam,int x,int y);
			virtual bool				wm_mbuttonup(WPARAM wParam,int x,int y);
			virtual bool				wm_mbuttondblclk(WPARAM wParam,int x,int y);
			virtual bool				wm_mousemove(WPARAM wParam,int x,int y);
			virtual bool				wm_mouseleave();
			virtual bool				wm_ncpaint(HRGN region);
			virtual bool				wm_capturechanged(HWND hwnd);
			virtual bool				wm_drawitem(int id,const DRAWITEMSTRUCT& dis);
			virtual bool				wm_erasebkgnd(HDC hdc);
			virtual bool				wm_command(HWND hwnd,WPARAM wParam,LPARAM lParam);
			virtual bool				wm_notify(UINT code,NMHDR& hdr,LRESULT& ret);
			virtual bool				wm_keydown(WPARAM wParam,LPARAM lParam);
			virtual bool				wm_keyup(WPARAM wParam,LPARAM lParam);
			virtual bool				wm_char(WPARAM wParam,LPARAM lParam);
			virtual bool				wm_timer(WPARAM wParam,LPARAM lParam);
			virtual bool				wm_nchittest(int x,int y,LRESULT& ret);
		protected:			// when the messages you want to handle do not fall into the above choices, use wm_unknown to handle them
			virtual bool				wm_unknown(UINT message,WPARAM wParam,LPARAM lParam,LRESULT& ret);
		protected:
			bool						def_mousemove(WPARAM wParam,int x,int y);
			bool						def_lbuttondown(WPARAM wParam,int x,int y);
			bool						def_lbuttonup(WPARAM wParam,int x,int y);
			bool						def_capturechanged(HWND hwnd);
			bool						def_drawitem(int id,const DRAWITEMSTRUCT& dis);
			bool						def_notify(UINT code,NMHDR& hdr,LRESULT& ret);
		protected:
			//virtual void				_paint_wnd();											// masterpiece function to make double-buffering PERFECT !!
			//static bool					_paint_wnd_real(HWND current,HWND target,HBITMAP hbitmap,RECT rt);		// rt is a screen rectangle
		public:				// note that z3D W32 controls don't take non-client stuff into account
			long						getWidth() const;
			long						getHeight() const;
			long						getWindowWidth() const;
			long						getWindowHeight() const;
			long						getRelX() const;
			long						getRelY() const;
			POINT						getClientOriginAbs() const;
			static HWND					getParentTilWindow(HWND hwnd);
		public:
			bool						enabledWndMover() const;
			bool						restrictedWndMover() const;
			long						getWndMoverX() const;
			long						getWndMoverY() const;
			long						getWndMoverDX() const;
			long						getWndMoverDY() const;
			long						getWndMoverSkipDefProcessing() const;
			long						getWndMoverRestrictX() const;
			long						getWndMoverRestrictY() const;
			long						getWndMoverRestrictDX() const;
			long						getWndMoverRestrictDY() const;
			bool						isVisible() const;
			bool						isEnabled() const;
			//bool						suppressed_destroy() const;
		public:
			static RECT					getActualVisibleRect(HWND hwnd);						// in screen-space
		public:
			void						setRelPos(long x,long y);
			void						setSize(long dx,long dy);
			void						setVisible(bool visible);
			void						setEnabled(bool enabled);
			void						setFocus();
			//void						suppress_destroy(bool suppress);
		public:
			virtual bool				createCustom(const wcs& classname,HICON icon,HCURSOR cursor,HBRUSH background,const wcs& windowname,int style,int ex_style,int x,int y,int width,int height,HWND parent,HMENU menu,void* param);
			virtual bool				createSubclass(const wcs& classname,const wcs& windowname,int style,int ex_style,int x,int y,int width,int height,HWND parent,HMENU menu,void* param);
			virtual bool				createSubclassExisting(HWND hwnd);
		public:
			void						enableWndMover(bool enable,int x,int y,int width,int height,bool skip_def_processing=true);
			void						restrictWndMover(bool restrict,int x,int y,int width,int height);
		public:
			HCURSOR						getCursor() const;
			void						setCursor(HCURSOR cursor);
		public:
			static HCURSOR				getDefaultCursor();
			static void					setDefaultCursor(HCURSOR cursor);
		public:
			const wcs					getWindowText() const;
			void						setWindowText(const wcs& text) const;
			const wcs				getClassName() const;
		public:
			void						bringToFront();
			void						alignCenter();
		public:
			void						forceUpdateWindow();
		public:
			LRESULT						sendMessage(UINT message,WPARAM wParam,LPARAM lParam);
		public:
			void						setMsgLoopSleep(unsigned long sleep);
			unsigned long				getMsgLoopSleep() const;
		};
	};
};
