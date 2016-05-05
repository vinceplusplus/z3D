
//#define _WIN32_WINNT	0x0500

#include "W32Wnd.h"

namespace z3D
{
	namespace GameFrm
	{
		W32Wnd::W32Wnd()
		{
			_hwnd = NULL;
			_subclass = false;
			_old_wndproc = NULL;

			//_suppress_destroy = false;

			_enable_wndmover = false;
			memset(&_rt_wndmover, 0, sizeof(_rt_wndmover));
			_wndmoving = false;
			_wndmover_skip_def_process = true;
			_wndmover_restrict = false;
			memset(&_wndmover_restrict_rt,0,sizeof(_wndmover_restrict_rt));

			/*
			if(!_offscreen_hbitmap)
			{
				HDC hdc=GetDC(GetDesktopWindow());
				_offscreen_hbitmap=CreateCompatibleBitmap(hdc,2048,2048);
				ReleaseDC(GetDesktopWindow(),hdc);
			}*/
		}
		W32Wnd::~W32Wnd()
		{
			DestroyWindow(_hwnd);
			_hwnd=NULL;
		}
		HWND					W32Wnd::getHWND() const {return _hwnd;}
		HFONT					W32Wnd::createFont(const wcs& face,int height,int weight,bool italic,bool underline,bool strikeout)
		{
			return CreateFont(height,0,0,0,weight*100,italic?TRUE:FALSE,underline?TRUE:FALSE,strikeout?TRUE:FALSE,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_DONTCARE|DEFAULT_PITCH,face.c_str());
		}
		LRESULT CALLBACK		W32Wnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			W32Wnd* wnd=getWndObj(hWnd);
			if(!wnd)
				return DefWindowProc(hWnd,message,wParam,lParam);
			LRESULT ret=0;
			if(real_wndproc(hWnd,message,wParam,lParam,ret))
			{
				if(wnd->_subclass)
					return 0;
				return ret;
			}
			if(!wnd->_subclass)
				return DefWindowProc(hWnd,message,wParam,lParam);
			return CallWindowProc(wnd->_old_wndproc,hWnd,message,wParam,lParam);
		}
		bool					W32Wnd::real_wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,LRESULT& ret)
		{
			W32Wnd* wnd=getWndObj(hWnd);
			if(!wnd->wm_unknown(message,wParam,lParam,ret))
			{
				switch(message)
				{
				case WM_CREATE:
					return wnd->wm_create(hWnd,*(CREATESTRUCT*)lParam);
				case WM_PAINT:
					return wnd->wm_paint();
					/*
					{
						wnd->_paint_wnd();
						return true;
					}*/
				case WM_PRINTCLIENT:
					return wnd->wm_printclient((HDC)wParam);
				case WM_CLOSE:
					return wnd->wm_close();
				case WM_SIZING:
					return wnd->wm_sizing(wParam,*(RECT*)lParam);
				case WM_SIZE:
					return wnd->wm_size(wParam,(short)LOWORD(lParam),(short)HIWORD(lParam));
				case WM_DESTROY:
					{
						bool ret=wnd->wm_destroy();
						_map_hwnd_obj.erase(hWnd);
						return ret;
					}
				case WM_LBUTTONDOWN:
					{
						int x=(short)LOWORD(lParam);
						int y=(short)HIWORD(lParam);

						if(!wnd->def_lbuttondown(wParam,x,y))
							return wnd->wm_lbuttondown(wParam,x,y);
						return true;
					}
				case WM_LBUTTONUP:
					{
						int x=(short)LOWORD(lParam);
						int y=(short)HIWORD(lParam);

						if(!wnd->def_lbuttonup(wParam,x,y))
							return wnd->wm_lbuttonup(wParam,(short)LOWORD(lParam),(short)HIWORD(lParam));
						return true;
					}
				case WM_RBUTTONDOWN:
					return wnd->wm_rbuttondown(wParam,(short)LOWORD(lParam),(short)HIWORD(lParam));
				case WM_RBUTTONUP:
					return wnd->wm_rbuttonup(wParam,(short)LOWORD(lParam),(short)HIWORD(lParam));
				case WM_MBUTTONDOWN:
					return wnd->wm_mbuttondown(wParam,(short)LOWORD(lParam),(short)HIWORD(lParam));
				case WM_MBUTTONUP:
					return wnd->wm_mbuttonup(wParam,(short)LOWORD(lParam),(short)HIWORD(lParam));
				case WM_MOUSEMOVE:
					{
						int x=(short)LOWORD(lParam);
						int y=(short)HIWORD(lParam);

						if(!wnd->def_mousemove(wParam,x,y))
							return wnd->wm_mousemove(wParam,(short)LOWORD(lParam),(short)HIWORD(lParam));
						return true;
					}
				case WM_MOUSELEAVE:
					return wnd->wm_mouseleave();
				case WM_NCPAINT:
					return wnd->wm_ncpaint((HRGN)wParam);
				case WM_CAPTURECHANGED:
					{
						if(!wnd->def_capturechanged((HWND)lParam))
							return wnd->wm_capturechanged((HWND)lParam);
						return true;
					}
				case WM_DRAWITEM:
					{
						if(!wnd->def_drawitem((int)wParam,*(DRAWITEMSTRUCT*)lParam))
							if(wnd->wm_drawitem((int)wParam,*(DRAWITEMSTRUCT*)lParam))
							{
								ret=TRUE;
								return true;
							}
						else
							ret=TRUE;
						return true;
					}
				case WM_ERASEBKGND:
					return wnd->wm_erasebkgnd((HDC)wParam);
				case WM_COMMAND:
					return wnd->wm_command((HWND)lParam,wParam,lParam);
				case WM_NOTIFY:
					{
						if(!wnd->def_notify(((NMHDR*)lParam)->code,*(NMHDR*)lParam,ret))
							return wnd->wm_notify(((NMHDR*)lParam)->code,*(NMHDR*)lParam,ret);
						return true;
					}
				case WM_KEYDOWN:
					return wnd->wm_keydown(wParam,lParam);
				case WM_KEYUP:
					return wnd->wm_keyup(wParam,lParam);
				case WM_CHAR:
					return wnd->wm_char(wParam,lParam);
				case WM_TIMER:
					return wnd->wm_timer(wParam,lParam);
				case WM_NCHITTEST:
					return wnd->wm_nchittest((short)LOWORD(lParam),(short)HIWORD(lParam),ret);
				}
			}
			return false;
		}
		W32Wnd*				W32Wnd::getWndObj(HWND hWnd)
		{
			map<HWND,W32Wnd*>::iterator iter = _map_hwnd_obj.find(hWnd);
			if(iter!=_map_hwnd_obj.end())
				return iter->second;
			return NULL;
		}
		/*
		void				W32Wnd::restore_printclient_region()
		{
			SelectClipRgn(_printclient_hdc,_printclient_rgn);
		}
		*/
		//bool				W32Wnd::printclienting() {return _printclienting;}
		void				W32Wnd::post_create() {}
		bool				W32Wnd::owner_draw(HDC hdc,int id,const DRAWITEMSTRUCT& dis) {return false;}
		bool				W32Wnd::notify_loopback(UINT code,NMHDR& hdr,LRESULT& ret) {return false;}
		//bool				W32Wnd::printclient_empty_region() const {return false;}
		//bool				W32Wnd::need_backup() const {return false;}
		void				W32Wnd::msg_loop()
		{
			MSG msg;
			while(true)
			{
				while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				{
					if(!msg_hook(msg))
						continue;
					if(msg.message==WM_QUIT)
						return;
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				user_proc();
				Sleep(10);
			}
		}
		bool				W32Wnd::msg_hook(const MSG& msg)
		{
			return true;
		}
		void				W32Wnd::user_proc() {}
		bool				W32Wnd::wm_create(HWND hwnd,const CREATESTRUCT& cs) {return false;}
		bool				W32Wnd::wm_close() {return false;}
		bool				W32Wnd::wm_sizing(WPARAM wParam,RECT& rt) {return false;}
		bool				W32Wnd::wm_size(WPARAM wParam,int width,int height) {return false;}
		bool				W32Wnd::wm_destroy() {return false;}
		bool				W32Wnd::wm_paint() {return false;}
		bool				W32Wnd::wm_printclient(HDC hdc) {return false;}
		bool				W32Wnd::wm_lbuttondown(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_lbuttonup(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_lbuttondblclk(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_rbuttondown(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_rbuttonup(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_rbuttondblclk(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_mbuttondown(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_mbuttonup(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_mbuttondblclk(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_mousemove(WPARAM wParam,int x,int y) {return false;}
		bool				W32Wnd::wm_mouseleave() {return false;}
		bool				W32Wnd::wm_ncpaint(HRGN region) {return false;}
		bool				W32Wnd::wm_capturechanged(HWND hwnd) {return false;}
		bool				W32Wnd::wm_drawitem(int id,const DRAWITEMSTRUCT& dis) {return false;}
		bool				W32Wnd::wm_erasebkgnd(HDC hdc) {return false;}
		bool				W32Wnd::wm_command(HWND hwnd,WPARAM wParam,LPARAM lParam) {return false;}
		bool				W32Wnd::wm_notify(UINT code,NMHDR& hdr,LRESULT& ret) {return false;}
		bool				W32Wnd::wm_keydown(WPARAM wParam,LPARAM lParam) {return false;}
		bool				W32Wnd::wm_keyup(WPARAM wParam,LPARAM lParam) {return false;}
		bool				W32Wnd::wm_char(WPARAM wParam,LPARAM lParam) {return false;}
		bool				W32Wnd::wm_timer(WPARAM wParam,LPARAM lParam) {return false;}
		bool				W32Wnd::wm_nchittest(int x,int y,LRESULT& ret) {return false;}
		bool				W32Wnd::wm_unknown(UINT message,WPARAM wParam,LPARAM lParam,LRESULT& ret) {return false;}
		bool				W32Wnd::def_mousemove(WPARAM wParam,int x,int y)
		{
			if(_enable_wndmover)
			{
				if(_wndmoving)
				{
					if(GetCapture()==getHWND())
					{
						POINT cur;
						cur.x=x;
						cur.y=y;
						ClientToScreen(getHWND(),&cur);
						GetCursorPos(&cur);

						if(cur.x==_last_pt_wndmover.x&&cur.y==_last_pt_wndmover.y)
							return _wndmover_skip_def_process;

						POINT p;
						p.x=getRelX()+cur.x-_last_pt_wndmover.x;
						p.y=getRelY()+cur.y-_last_pt_wndmover.y;
						if(_wndmover_restrict)
						{
							int x2=getRelX()+cur.x-_last_pt_wndmover.x;
							int x3=min(max(x2,(int)_wndmover_restrict_rt.left),(int)(_wndmover_restrict_rt.right-1));
							int y2=getRelY()+cur.y-_last_pt_wndmover.y;
							int y3=min(max(y2,(int)_wndmover_restrict_rt.top),(int)(_wndmover_restrict_rt.bottom-1));
							p.x+=x3-x2;
							p.y+=y3-y2;
							cur.x+=x3-x2;
							cur.y+=y3-y2;
						}
						if(getRelX()!=p.x||getRelY()!=p.y)
							SetWindowPos(getHWND(),NULL,p.x,p.y,0,0,SWP_NOZORDER|SWP_NOSIZE);
						_last_pt_wndmover=cur;
						return _wndmover_skip_def_process;
					}
					else
						_wndmoving = false;
				}
				if(x>=_rt_wndmover.left && y>=_rt_wndmover.top && x<_rt_wndmover.right && y<_rt_wndmover.bottom)
					return _wndmover_skip_def_process;
			}
			return false;
		}
		bool				W32Wnd::def_lbuttondown(WPARAM wParam,int x,int y)
		{
			if(_enable_wndmover && x>=_rt_wndmover.left && y>=_rt_wndmover.top && x<_rt_wndmover.right && y<_rt_wndmover.bottom)
			{
				_wndmoving = true;

				SetCapture(getHWND());
				_last_pt_wndmover.x=x;
				_last_pt_wndmover.y=y;
				ClientToScreen(getHWND(),&_last_pt_wndmover);
				GetCursorPos(&_last_pt_wndmover);
				return _wndmover_skip_def_process;
			}
			else
				_wndmoving = false;
			return false;
		}
		bool				W32Wnd::def_lbuttonup(WPARAM wParam,int x,int y)
		{
			if(_enable_wndmover && _wndmoving)
			{
				if(GetCapture()==getHWND())
					ReleaseCapture();
				_wndmoving = false;
				return _wndmover_skip_def_process;
			}
			return false;
		}
		bool				W32Wnd::def_capturechanged(HWND hwnd)
		{
			if(GetCapture()!=hwnd)
				_wndmoving = false;
			return false;
		}
		bool				W32Wnd::def_drawitem(int id,const DRAWITEMSTRUCT& dis)
		{
			W32Wnd* w=getWndObj(dis.hwndItem);
			if(!w)
				return false;
			if(!w->owner_draw(dis.hDC,id,dis))
				return false;
			return true;
		}
		bool				W32Wnd::def_notify(UINT code,NMHDR& hdr,LRESULT& ret)
		{
			W32Wnd* w=getWndObj(hdr.hwndFrom);
			if(!w)
				return false;
			if(!w->notify_loopback(code,hdr,ret))
				return false;
			return true;
		}
		/*
		void				W32Wnd::_paint_wnd()
		{
			RECT rt,crt;
			if(!GetUpdateRect(getHWND(),&rt,FALSE))
				return;

			if(false)
			{
				RECT r=rt;
				RECT r2,r3;
				ClientToScreen(getHWND(),(POINT*)&r.left);
				ClientToScreen(getHWND(),(POINT*)&r.right);

				HWND hwnd=getHWND();
				do
				{
					GetClientRect(hwnd,&r2);
					ClientToScreen(hwnd,(POINT*)&r2.left);
					ClientToScreen(hwnd,(POINT*)&r2.right);
					if(!IntersectRect(&r3,&r,&r2))
						return;
					r=r3;
					hwnd=getParentTilWindow(hwnd);
				}while(hwnd);
				ScreenToClient(getHWND(),(POINT*)&r.left);
				ScreenToClient(getHWND(),(POINT*)&r.right);
				rt=r;
			}

			crt=rt;

			//if(GetParent(getHWND()))
				//return;
			//if(rt.right!=1008)
				//return;

			ClientToScreen(getHWND(),(POINT*)&rt.left);
			ClientToScreen(getHWND(),(POINT*)&rt.right);

			HDC desktopdc=GetDC(GetDesktopWindow());

			HBITMAP hbitmap=CreateCompatibleBitmap(desktopdc,rt.right-rt.left,rt.bottom-rt.top);
			//HBITMAP hbitmap=_offscreen_hbitmap;

			HWND ancestor=getHWND();
			//while(GetParent(ancestor))
			//while(GetAncestor(ancestor,GA_PARENT))
			while(getParentTilWindow(ancestor))
				ancestor=getParentTilWindow(ancestor);
			//ancestor=GetWindow(ancestor,GW_HWNDLAST);

			if(ancestor)
			{
				_paint_wnd_real(ancestor,getHWND(),hbitmap,rt);
			}

			HDC hdc=CreateCompatibleDC(desktopdc);
			SelectObject(hdc,hbitmap);

			ReleaseDC(GetDesktopWindow(),desktopdc);

			PAINTSTRUCT ps;
			HDC hdc2=BeginPaint(getHWND(),&ps);
			//HDC hdc2=GetDC(getHWND());
			BitBlt(hdc2,crt.left,crt.top,crt.right-crt.left,crt.bottom-crt.top,hdc,0,0,SRCCOPY);
			//SetBkMode(hdc2,TRANSPARENT);
			//TextOut(hdc2,crt.left,crt.top,L"1",1);
			//ReleaseDC(getHWND(),hdc2);
			EndPaint(getHWND(),&ps);

			//GetUpdateRect(getHWND(),&rt,FALSE);
			//ValidateRect(getHWND(),&rt);

			DeleteDC(hdc);
			DeleteObject(hbitmap);

			//if(crt.right!=getWidth()||crt.bottom!=getHeight()||crt.left!=0||crt.top!=0)
				//InvalidateRect(getHWND(),NULL,FALSE);
		}
		*/
		/*
		bool				W32Wnd::_paint_wnd_real(HWND current,HWND target,HBITMAP hbitmap,RECT rt)
		{
			RECT r,r2;
			GetClientRect(current,&r);
			ClientToScreen(current,(POINT*)&r.left);
			ClientToScreen(current,(POINT*)&r.right);
			if(!IntersectRect(&r2,&rt,&r))
				return false;

			HDC desktopdc=GetDC(GetDesktopWindow());
			HDC hdc=CreateCompatibleDC(desktopdc);
			ReleaseDC(GetDesktopWindow(),desktopdc);
			SelectObject(hdc,hbitmap);

			RECT rt2=getActualVisibleRect(current);
			RECT rt3;
			if(!IntersectRect(&rt3,&rt,&rt2))
			{
				DeleteDC(hdc);
				return false;
			}
			HRGN hrgn=CreateRectRgn(rt3.left-rt.left,rt3.top-rt.top,rt3.right-rt.left,rt3.bottom-rt.top);
			SelectClipRgn(hdc,hrgn);

			SetGraphicsMode(hdc,GM_ADVANCED);

			XFORM xform;
			xform.eDx = (FLOAT)-(rt.left-r.left);
			xform.eDy = (FLOAT)-(rt.top-r.top);
			xform.eM11 = 1;
			xform.eM12 = 0;
			xform.eM21 = 0;
			xform.eM22 = 1;
			SetWorldTransform(hdc,&xform);

			//SetViewportOrgEx(hdc,-(rt.left-r.left),-(rt.top-r.top),NULL);
			//OffsetViewportOrgEx(hdc,rt.left-r.left,rt.top-r.top,NULL);

			_printclient_hdc=hdc;
			_printclient_rgn=hrgn;

			_printclienting = true;

			if(true)
			{
				W32Wnd* wnd=getWndObj(current);
				if(wnd && wnd->printclient_empty_region())
				{
					SelectClipRgn(hdc,_empty_rgn);
				}
				if(wnd && wnd->need_backup())
				{
					desktopdc=GetDC(GetDesktopWindow());
					_backup_hdc=CreateCompatibleDC(desktopdc);
					_backup_hbitmap=CreateCompatibleBitmap(desktopdc,rt3.right-rt3.left,rt3.bottom-rt3.top);
					ReleaseDC(GetDesktopWindow(),desktopdc);
					SelectObject(_backup_hdc,_backup_hbitmap);

					BitBlt(_backup_hdc,rt3.left-r.left,rt3.top-r.top,rt3.right-rt3.left,rt3.bottom-rt3.top,hdc,0,0,SRCCOPY);

					SetGraphicsMode(_backup_hdc,GM_ADVANCED);
					XFORM xform;
					xform.eDx = (FLOAT)(rt3.left-r.left);
					xform.eDy = (FLOAT)(rt3.top-r.top);
					xform.eM11 = 1;
					xform.eM12 = 0;
					xform.eM21 = 0;
					xform.eM22 = 1;
					SetWorldTransform(_backup_hdc,&xform);
				}
			}

			SendMessage(current,WM_PRINTCLIENT,(WPARAM)hdc,PRF_CLIENT);

			if(true)
			{
				_printclient_hdc=NULL;
				_printclient_rgn=NULL;

				if(_backup_hdc)
				{
					DeleteDC(_backup_hdc);
					DeleteObject(_backup_hbitmap);

					_backup_hdc=NULL;
					_backup_hbitmap=NULL;
				}
			}

			_printclienting = false;

			//TextOut(hdc,0,0,L"1",1);

			//if(current == target)
				//return true;

			DeleteDC(hdc);

			DeleteObject(hrgn);

			HWND hwnd;
			// fucking GW_CHILD|GW_HWNDLAST is not allowed, this almost causes me a quater-day
			for(hwnd=GetWindow(GetWindow(current,GW_CHILD),GW_HWNDLAST);hwnd!=NULL;hwnd=GetWindow(hwnd,GW_HWNDPREV))
				if(IsWindowVisible(hwnd) && _paint_wnd_real(hwnd,target,hbitmap,rt))
					return true;

			return false;
		}
		*/
		long				W32Wnd::getWidth() const
		{
			RECT rt;
			GetClientRect(getHWND(),&rt);
			return rt.right;
		}
		long				W32Wnd::getHeight() const
		{
			RECT rt;
			GetClientRect(getHWND(),&rt);
			return rt.bottom;
		}
		long				W32Wnd::getWindowWidth() const
		{
			RECT rt;
			GetWindowRect(getHWND(),&rt);
			return rt.right-rt.left;
		}
		long				W32Wnd::getWindowHeight() const
		{
			RECT rt;
			GetWindowRect(getHWND(),&rt);
			return rt.bottom-rt.top;
		}
		long				W32Wnd::getRelX() const
		{
			RECT rt,rt2;
			memset(&rt,0,sizeof(rt));
			memset(&rt2,0,sizeof(rt2));
			GetClientRect(getHWND(),&rt);
			ClientToScreen(getHWND(),(POINT*)&rt);
			HWND parent=getParentTilWindow(getHWND());

			if(parent)
			{
				GetClientRect(parent,&rt2);
				ClientToScreen(parent,(POINT*)&rt2);
			}
			return rt.left-rt2.left;
		}
		long				W32Wnd::getRelY() const
		{
			RECT rt,rt2;
			memset(&rt,0,sizeof(rt));
			memset(&rt2,0,sizeof(rt2));
			GetClientRect(getHWND(),&rt);
			ClientToScreen(getHWND(),(POINT*)&rt);
			HWND parent=getParentTilWindow(getHWND());

			if(parent)
			{
				GetClientRect(parent,&rt2);
				ClientToScreen(parent,(POINT*)&rt2);
			}
			return rt.top-rt2.top;
		}
		POINT				W32Wnd::getClientOriginAbs() const
		{
			POINT p;
			p.x=p.y=0;
			ClientToScreen(getHWND(),&p);
			return p;
		}
		HWND				W32Wnd::getParentTilWindow(HWND hwnd)
		{
			if(!(GetWindowLong(hwnd,GWL_STYLE)&WS_CHILD))
				return NULL;
			if(GetWindowLong(hwnd,GWL_STYLE)&WS_POPUP)
				return NULL;
			if(GetWindowLong(hwnd,GWL_STYLE)&WS_OVERLAPPED)
				return NULL;

			return GetParent(hwnd);
		}
		bool				W32Wnd::enabledWndMover() const {return _enable_wndmover;}
		bool				W32Wnd::restrictedWndMover() const {return _wndmover_restrict;}
		long				W32Wnd::getWndMoverX() const {return _rt_wndmover.left;}
		long				W32Wnd::getWndMoverY() const {return _rt_wndmover.top;}
		long				W32Wnd::getWndMoverDX() const {return _rt_wndmover.right-_rt_wndmover.left;}
		long				W32Wnd::getWndMoverDY() const {return _rt_wndmover.bottom-_rt_wndmover.top;}
		long				W32Wnd::getWndMoverSkipDefProcessing() const {return _wndmover_skip_def_process;}
		long				W32Wnd::getWndMoverRestrictX() const {return _wndmover_restrict_rt.left;}
		long				W32Wnd::getWndMoverRestrictY() const {return _wndmover_restrict_rt.top;}
		long				W32Wnd::getWndMoverRestrictDX() const {return _wndmover_restrict_rt.right-_wndmover_restrict_rt.left;}
		long				W32Wnd::getWndMoverRestrictDY() const {return _wndmover_restrict_rt.bottom-_wndmover_restrict_rt.top;}
		bool				W32Wnd::isVisible() const {return (GetWindowLong(getHWND(),GWL_STYLE)&WS_VISIBLE)?true:false;}
		bool				W32Wnd::isEnabled() const {return IsWindowEnabled(getHWND())?true:false;}
		//bool				W32Wnd::suppressed_destroy() const {return _suppress_destroy;}
		RECT				W32Wnd::getActualVisibleRect(HWND hwnd)
		{
			RECT empty;
			memset(&empty,0,sizeof(empty));
			RECT r,r2,r3;
			GetClientRect(hwnd,&r);
			ClientToScreen(hwnd,(POINT*)&r.left);
			ClientToScreen(hwnd,(POINT*)&r.right);
			do
			{
				GetClientRect(hwnd,&r2);
				ClientToScreen(hwnd,(POINT*)&r2.left);
				ClientToScreen(hwnd,(POINT*)&r2.right);
				if(!IntersectRect(&r3,&r,&r2))
					return empty;
				r=r3;
				hwnd=getParentTilWindow(hwnd);
			}while(hwnd);
			return r;
		}
		void				W32Wnd::setRelPos(long x,long y)
		{
			RECT rt;
			GetWindowRect(getHWND(),&rt);
			SetWindowPos(getHWND(),NULL,/*rt.left+*/x/*-getRelX()*/,/*rt.top+*/y/*-getRelY()*/,0,0,SWP_NOZORDER|SWP_NOSIZE);
		}
		void				W32Wnd::setSize(long dx,long dy)
		{
			SetWindowPos(getHWND(),NULL,0,0,dx,dy,SWP_NOZORDER|SWP_NOMOVE);
		}
		void				W32Wnd::setVisible(bool visible)
		{
			if(isVisible()!=visible)
				ShowWindow(getHWND(),visible?SW_SHOW:SW_HIDE);
		}
		void				W32Wnd::setEnabled(bool enabled)
		{
			EnableWindow(getHWND(),enabled?TRUE:FALSE);
		}
		void				W32Wnd::setFocus()
		{
			SetFocus(getHWND());
		}
		/*
		void				W32Wnd::suppress_destroy(bool suppress)
		{
			_suppress_destroy=suppress;
		}
		*/
		bool				W32Wnd::createCustom(const wcs& classname,HICON icon,HCURSOR cursor,HBRUSH background,const wcs& windowname,int style,int ex_style,int x,int y,int width,int height,HWND parent,HMENU menu,void* param)
		{
			if(_hwnd)
			{
				DestroyWindow(_hwnd);
				_hwnd=NULL;
			}

			WNDCLASS wc;

			wc.style			= CS_HREDRAW | CS_VREDRAW/* | CS_PARENTDC*/;
			wc.lpfnWndProc		= (WNDPROC)W32Wnd::WndProc;
			wc.cbClsExtra		= 0;
			wc.cbWndExtra		= 0;
			wc.hInstance		= (HINSTANCE)GetModuleHandle(NULL);
			wc.hIcon			= icon;
			wc.hCursor			= cursor;
			wc.hbrBackground	= background;
			wc.lpszMenuName		= NULL;
			wc.lpszClassName	= classname.c_str();

			RegisterClass(&wc);

			//style &= ~(WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
			style |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

			//ex_style |= WS_EX_COMPOSITED;

			_hwnd=CreateWindowEx(ex_style,classname.c_str(),windowname.c_str(),style,x,y,width,height,parent,menu,(HINSTANCE)GetModuleHandle(NULL),param);
			/*
			if(!_hwnd)
			{
				ex_style &= ~WS_EX_COMPOSITED;
				_hwnd=CreateWindowEx(ex_style,classname.c_str(),windowname.c_str(),style,x,y,width,height,parent,menu,(HINSTANCE)GetModuleHandle(NULL),param);
			}*/
			if(!_hwnd)
			{
				UnregisterClass(classname.c_str(),(HINSTANCE)GetModuleHandle(NULL));
				return false;
			}
			_map_hwnd_obj[_hwnd]=this;

			_subclass = false;

			if(_default_cursor)
				setCursor(_default_cursor);

			post_create();

			return true;
		}
		bool				W32Wnd::createSubclass(const wcs& classname,const wcs& windowname,int style,int ex_style,int x,int y,int width,int height,HWND parent,HMENU menu,void* param)
		{
			if(_hwnd)
			{
				DestroyWindow(_hwnd);
				_hwnd=NULL;
			}

			//style &= ~(WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
			style |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

			//ex_style |= WS_EX_COMPOSITED;

			HWND hwnd=CreateWindowEx(ex_style,classname.c_str(),windowname.c_str(),style,x,y,width,height,parent,menu,(HINSTANCE)GetModuleHandle(NULL),param);
			/*
			if(!hwnd)
			{
				ex_style &= ~WS_EX_COMPOSITED;
				hwnd=CreateWindowEx(ex_style,classname.c_str(),windowname.c_str(),style,x,y,width,height,parent,menu,(HINSTANCE)GetModuleHandle(NULL),param);
			}*/
			if(!hwnd)
				return false;
			return createSubclassExisting(hwnd);
		}
		bool				W32Wnd::createSubclassExisting(HWND hwnd)
		{
			if(_hwnd)
			{
				DestroyWindow(_hwnd);
				_hwnd=NULL;
			}

			if(!hwnd)
				return false;
			_hwnd = hwnd;
			_map_hwnd_obj[_hwnd] = this;
			_subclass = true;
			_old_wndproc = (WNDPROC)(LONG_PTR)SetWindowLongPtr(_hwnd, GWL_WNDPROC, (LONG_PTR)W32Wnd::WndProc);

			//SetClassLong(getHWND(), GCL_STYLE, GetClassLong(getHWND(), GCL_STYLE) | CS_PARENTDC);

			if(_default_cursor)
				setCursor(_default_cursor);

			post_create();

			return true;
		}
		void				W32Wnd::enableWndMover(bool enable,int x,int y,int width,int height,bool skip_def_processing)
		{
			if(enable)
			{
				_enable_wndmover = true;
				_rt_wndmover.left = x;
				_rt_wndmover.top = y;
				_rt_wndmover.right = x + width;
				_rt_wndmover.bottom = y + height;
				_wndmoving = false;
				_wndmover_skip_def_process = skip_def_processing;
			}
			else
			{
				_enable_wndmover = false;
				_wndmoving = false;
			}
		}

		void				W32Wnd::restrictWndMover(bool restrict,int x,int y,int width,int height)
		{
			_wndmover_restrict = restrict;
			_wndmover_restrict_rt.left=x;
			_wndmover_restrict_rt.top=y;
			_wndmover_restrict_rt.right=x+width;
			_wndmover_restrict_rt.bottom=y+height;
		}
		HCURSOR				W32Wnd::getCursor() const
		{
			return (HCURSOR)GetClassLongPtr(getHWND(), (LONG_PTR)GCL_HCURSOR);
		}
		void				W32Wnd::setCursor(HCURSOR cursor)
		{
			SetClassLongPtr(getHWND(), GCL_HCURSOR, (LONG_PTR)cursor);
		}

		HCURSOR				W32Wnd::getDefaultCursor()
		{
			return _default_cursor;
		}
		void				W32Wnd::setDefaultCursor(HCURSOR cursor)
		{
			_default_cursor=cursor;
		}
		const wcs		W32Wnd::getWindowText() const
		{
			if(!getHWND())
				return L"";
			wchar_t buffer[4096];
			buffer[4095]=0;
			GetWindowText(getHWND(),buffer,4095);
			return buffer;
		}
		void				W32Wnd::setWindowText(const wcs& text) const
		{
			if(!getHWND())
				return;
			SetWindowText(getHWND(),text.c_str());
		}
		const wcs		W32Wnd::getClassName() const
		{
			if(!getHWND())
				return L"";
			wchar_t buffer[4096];
			buffer[4095]=0;
			GetClassName(getHWND(),buffer,4095);
			return buffer;
		}
		void				W32Wnd::bringToFront()
		{
			if(!getHWND())
				return;
			SetWindowPos(getHWND(),HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
		void				W32Wnd::alignCenter()
		{
			if(!getHWND())
				return;
			HWND parent=GetParent(getHWND());
			if(!parent)
				parent=GetDesktopWindow();

			bool window=getParentTilWindow(getHWND())?false:true;

			RECT rt,rt2;
			GetWindowRect(getHWND(),&rt);
			GetWindowRect(parent,&rt2);

			int x,y;
			x=((rt2.right-rt2.left)-(rt.right-rt.left))/2;
			y=((rt2.bottom-rt2.top)-(rt.bottom-rt.top))/2;
			if(window)
			{
				x+=rt2.left;
				y+=rt2.top;
			}
			SetWindowPos(getHWND(),NULL,x,y,0,0,SWP_NOZORDER|SWP_NOSIZE);
		}
		void				W32Wnd::forceUpdateWindow()
		{
			if(!getHWND())
				return;
			InvalidateRect(getHWND(),NULL,FALSE);
			UpdateWindow(getHWND());
		}
		LRESULT				W32Wnd::sendMessage(UINT message,WPARAM wParam,LPARAM lParam)
		{
			if(!getHWND())
				return 0;
			return SendMessage(getHWND(),message,wParam,lParam);
		}
		void						W32Wnd::setMsgLoopSleep(unsigned long sleep)
		{
			_msg_loop_sleep=sleep;
		}
		unsigned long				W32Wnd::getMsgLoopSleep() const
		{
			return _msg_loop_sleep;
		}
	};
};



