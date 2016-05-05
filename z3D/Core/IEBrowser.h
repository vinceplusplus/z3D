
#pragma once

#include "predecl.h"

#include <objbase.h>
#include <ole2.h>
#include <exdisp.h>
#include <exdispid.h>
#include <mshtml.h>
#include <mshtmhst.h>

#pragma comment(lib,"ole32.lib")

namespace z3D
{
	namespace Core
	{
		class IEBrowser;

		class IEClientSite: public IOleClientSite
		{
		public:
			IEBrowser*						ie;
		public:
			IEClientSite() {ie=NULL;}
		public:
			HRESULT STDMETHODCALLTYPE		QueryInterface(REFIID iid,void** ppvObject);
			ULONG STDMETHODCALLTYPE			AddRef(void);
			ULONG STDMETHODCALLTYPE			Release(void);
		public:
			HRESULT STDMETHODCALLTYPE		SaveObject();
			HRESULT STDMETHODCALLTYPE		GetMoniker(DWORD dwAssign,DWORD dwWhichMoniker,IMoniker ** ppmk);
			HRESULT STDMETHODCALLTYPE		GetContainer(LPOLECONTAINER FAR* ppContainer);
			HRESULT STDMETHODCALLTYPE		ShowObject(void);
			HRESULT STDMETHODCALLTYPE		OnShowWindow(BOOL fShow);
			HRESULT STDMETHODCALLTYPE		RequestNewObjectLayout(void);
		};

		class IEInPlaceSite: public IOleInPlaceSite
		{
		public:
			IEBrowser*						ie;
		public:
			IEInPlaceSite() {ie=NULL;}
		public:
			HRESULT STDMETHODCALLTYPE		QueryInterface(REFIID iid,void** ppvObject);
			ULONG STDMETHODCALLTYPE			AddRef(void);
			ULONG STDMETHODCALLTYPE			Release(void);
		public:
			HRESULT STDMETHODCALLTYPE		GetWindow(HWND *phwnd);
			HRESULT STDMETHODCALLTYPE		ContextSensitiveHelp(BOOL fEnterMode);
			HRESULT STDMETHODCALLTYPE		CanInPlaceActivate(void);
			HRESULT STDMETHODCALLTYPE		OnInPlaceActivate(void);
			HRESULT STDMETHODCALLTYPE		OnUIActivate(void);
			HRESULT STDMETHODCALLTYPE		GetWindowContext(IOleInPlaceFrame **ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo);
			HRESULT STDMETHODCALLTYPE		Scroll(SIZE scrollExtant);
			HRESULT STDMETHODCALLTYPE		OnUIDeactivate(BOOL fUndoable);
			HRESULT STDMETHODCALLTYPE		OnInPlaceDeactivate(void);
			HRESULT STDMETHODCALLTYPE		DiscardUndoState(void);
			HRESULT STDMETHODCALLTYPE		DeactivateAndUndo(void);
			HRESULT STDMETHODCALLTYPE		OnPosRectChange(LPCRECT lprcPosRect);
		};

		class IEInPlaceFrame: public IOleInPlaceFrame
		{
		public:
			IEBrowser*						ie;
		public:
			IEInPlaceFrame() {ie=NULL;}
		public:
			HRESULT STDMETHODCALLTYPE		QueryInterface(REFIID iid,void** ppvObject);
			ULONG STDMETHODCALLTYPE			AddRef(void);
			ULONG STDMETHODCALLTYPE			Release(void);
		public:
			HRESULT STDMETHODCALLTYPE		GetWindow(HWND *phwnd);
			HRESULT STDMETHODCALLTYPE		ContextSensitiveHelp(BOOL fEnterMode);
			HRESULT STDMETHODCALLTYPE		GetBorder(LPRECT lprectBorder);
			HRESULT STDMETHODCALLTYPE		RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
			HRESULT STDMETHODCALLTYPE		SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
			HRESULT STDMETHODCALLTYPE		SetActiveObject(IOleInPlaceActiveObject *pActiveObject,LPCOLESTR pszObjName);
			HRESULT STDMETHODCALLTYPE		InsertMenus(HMENU hmenuShared,LPOLEMENUGROUPWIDTHS lpMenuWidths);
			HRESULT STDMETHODCALLTYPE		SetMenu(HMENU hmenuShared,HOLEMENU holemenu,HWND hwndActiveObject);
			HRESULT STDMETHODCALLTYPE		RemoveMenus(HMENU hmenuShared);
			HRESULT STDMETHODCALLTYPE		SetStatusText(LPCOLESTR pszStatusText);
			HRESULT STDMETHODCALLTYPE		EnableModeless(BOOL fEnable);
			HRESULT STDMETHODCALLTYPE		TranslateAccelerator(LPMSG lpmsg,WORD wID);
		};

		class IEDocHostUIHandler: public IDocHostUIHandler
		{
		public:
			IEBrowser*						ie;
		public:
			IEDocHostUIHandler() {ie=NULL;}
		public:
			HRESULT STDMETHODCALLTYPE		QueryInterface(REFIID iid,void** ppvObject);
			ULONG STDMETHODCALLTYPE			AddRef(void);
			ULONG STDMETHODCALLTYPE			Release(void);
		public:
			HRESULT STDMETHODCALLTYPE		ShowContextMenu(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved);
			HRESULT STDMETHODCALLTYPE		GetHostInfo(DOCHOSTUIINFO *pInfo);
			HRESULT STDMETHODCALLTYPE		ShowUI(DWORD dwID,IOleInPlaceActiveObject *pActiveObject,IOleCommandTarget *pCommandTarget,IOleInPlaceFrame *pFrame,IOleInPlaceUIWindow *pDoc);
			HRESULT STDMETHODCALLTYPE		HideUI(void);
			HRESULT STDMETHODCALLTYPE		UpdateUI(void);
			HRESULT STDMETHODCALLTYPE		EnableModeless(BOOL fEnable);
			HRESULT STDMETHODCALLTYPE		OnDocWindowActivate(BOOL fActivate);
			HRESULT STDMETHODCALLTYPE		OnFrameWindowActivate(BOOL fActivate);
			HRESULT STDMETHODCALLTYPE		ResizeBorder(LPCRECT prcBorder,IOleInPlaceUIWindow *pUIWindow,BOOL fRameWindow);
			HRESULT STDMETHODCALLTYPE		TranslateAccelerator(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID);
			HRESULT STDMETHODCALLTYPE		GetOptionKeyPath(LPOLESTR *pchKey,DWORD dw);
			HRESULT STDMETHODCALLTYPE		GetDropTarget(IDropTarget *pDropTarget,IDropTarget **ppDropTarget);
			HRESULT STDMETHODCALLTYPE		GetExternal(IDispatch **ppDispatch);
			HRESULT STDMETHODCALLTYPE		TranslateUrl(DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut);
			HRESULT STDMETHODCALLTYPE		FilterDataObject(IDataObject *pDO,IDataObject **ppDORet);
		};

		class IEStorage: public IStorage
		{
		public:
			IEBrowser*						ie;
		public:
			IEStorage() {ie=NULL;}
		public:
			HRESULT STDMETHODCALLTYPE		QueryInterface(REFIID iid,void** ppvObject);
			ULONG STDMETHODCALLTYPE			AddRef(void);
			ULONG STDMETHODCALLTYPE			Release(void);
		public:
			HRESULT STDMETHODCALLTYPE		CreateStream(const OLECHAR *pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStream **ppstm);
			HRESULT STDMETHODCALLTYPE		OpenStream(const OLECHAR *pwcsName,void *reserved1,DWORD grfMode,DWORD reserved2,IStream **ppstm);
			HRESULT STDMETHODCALLTYPE		CreateStorage(const OLECHAR *pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStorage **ppstg);
			HRESULT STDMETHODCALLTYPE		OpenStorage(const OLECHAR *pwcsName,IStorage *pstgPriority,DWORD grfMode,SNB snbExclude,DWORD reserved,IStorage **ppstg);
			HRESULT STDMETHODCALLTYPE		CopyTo(DWORD ciidExclude,const IID *rgiidExclude,SNB snbExclude,IStorage *pstgDest);
			HRESULT STDMETHODCALLTYPE		MoveElementTo(const OLECHAR *pwcsName,IStorage *pstgDest,const OLECHAR *pwcsNewName,DWORD grfFlags);
			HRESULT STDMETHODCALLTYPE		Commit(DWORD grfCommitFlags);
			HRESULT STDMETHODCALLTYPE		Revert(void);
			HRESULT STDMETHODCALLTYPE		EnumElements(DWORD reserved1,void *reserved2,DWORD reserved3,IEnumSTATSTG **ppenum);
			HRESULT STDMETHODCALLTYPE		DestroyElement(const OLECHAR *pwcsName);
			HRESULT STDMETHODCALLTYPE		RenameElement(const OLECHAR *pwcsOldName,const OLECHAR *pwcsNewName);
			HRESULT STDMETHODCALLTYPE		SetElementTimes(const OLECHAR *pwcsName,const FILETIME *pctime,const FILETIME *patime,const FILETIME *pmtime);
			HRESULT STDMETHODCALLTYPE		SetClass(REFCLSID clsid);
			HRESULT STDMETHODCALLTYPE		SetStateBits(DWORD grfStateBits,DWORD grfMask);
			HRESULT STDMETHODCALLTYPE		Stat(STATSTG *pstatstg,DWORD grfStatFlag);
		};

		class IEWebBrowserEvents2: public DWebBrowserEvents2
		{
		public:
			IEBrowser*			ie;
		public:
			HRESULT STDMETHODCALLTYPE		QueryInterface(REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject);
			ULONG STDMETHODCALLTYPE			AddRef( void);
			ULONG STDMETHODCALLTYPE			Release( void);
		public:
			HRESULT STDMETHODCALLTYPE		GetTypeInfoCount(UINT *pctinfo);
			HRESULT STDMETHODCALLTYPE		GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
			HRESULT STDMETHODCALLTYPE		GetIDsOfNames(REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
			HRESULT STDMETHODCALLTYPE		Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
		};

		class IEBrowser
		{
		public:
			HWND				parent;
			IOleObject*			browser_object;
			IEClientSite		client_site;
			IEInPlaceSite		inplace_site;
			IEInPlaceFrame		inplace_frame;
			IEDocHostUIHandler	dochost_uihandler;
			IEStorage			storage;
			IEWebBrowserEvents2	event_handler;
			IWebBrowser2*		browser;
		public:
			struct
			{
				bool			no3dborder:1;
				bool			no3douterborder:1;
				bool			disable_selection:1;
				bool			disable_help:1;
				bool			disable_scroll:1;
				bool			disable_script:1;
				bool			open_new_window:1;
				bool			enable_flat_scrollbar:1;
				bool			enable_autocomplete:1;
				bool			enable_inplace_navigation:1;
				bool			enable_ime_reconversion:1;
			}					option;
		public:
			HDC					memdc;
			HBITMAP				membitmap;
			void*				bits;
		public:
			RECT				display_rect;
		public:
			IEBrowser();
			virtual ~IEBrowser();
		public:
			void				setOptions(bool no3dborder=true,bool no3douterborder=true,bool disable_selection=false,bool disable_help=false,bool disable_scroll=false,bool disable_script=false,bool open_new_window=false,bool enable_flat_scrollbar=false,bool enable_autocomplete=false, bool enable_inplace_navigation=false,bool enable_ime_reconversion=false);
		public:
			bool				create(HWND parent,int x,int y,int width,int height);
			void				destroy();
		public:
			void				navigate(const wstring& url);
		private:
			bool				capture(int& width,int& height);
		public:
			bool				capture(void* _fipImage);
			bool				capture_raw24bit(vector<unsigned char>& array_byte,int& width,int height);
		public:
			void				destroyCaptureTemp();
		public:
			void				setPos(int x,int y);
			int					getX() const;
			int					getY() const;
			void				setExt(int dx,int dy);
			int					getDX() const;
			int					getDY() const;
		public:
			void				saveDisplayRect();
			void				loadDisplayRect();
		public:
			void				setInvisible();					// set width and height to 0
		protected:
			// event handlers, details reference to DWebBrowserEvents2
			// note that because Microsoft may change the arguments of event handlers,
			// the implementation will verify whether each argument's type is correct then proceed,
			// otherwise the event handlers may not be got called
			virtual void		beforeNavigate2(IDispatch* disp,const wstring& url,const wstring& target_frame,VARIANT& post_data,const wstring& headers,bool& cancel);
			virtual void		clientToHostWindow(long& cx,long& cy);
			virtual void		commandStateChange(long command,bool enabled);
			virtual void		documentComplete(IDispatch* disp,const wstring& url);
			virtual void		downloadBegin();
			virtual void		downloadComplete();
			virtual void		fileDownload(bool& cancel);
			virtual void		navigateComplete2(IDispatch* disp,const wstring& url);
			virtual void		navigateError(IDispatch* disp,const wstring& url,const wstring& target_frame,long status_code,bool& cancel);
			virtual void		newWindow2(IDispatch*& disp,bool& cancel);
			virtual void		onFullscreen(bool fullscreen);
			virtual void		onMenuBar(bool shown);
			virtual void		onQuit();
			virtual void		onStatusBar(bool shown);
			virtual void		onTheaterMode(bool theater_mode);
			virtual void		onToolBar(bool shown);
			virtual void		onVisible(bool visible);
			virtual void		printTemplateInitantiation(IDispatch* disp);
			virtual void		printTemplateTearDown(IDispatch* disp);
			virtual void		privacyImpactedStateChanged(bool privacyimpacted);
			virtual void		progressChange(long progress,long progress_max);
			virtual void		propertyChange(const wstring& property_name);
			virtual void		setSecureLockIcon(long secure_lock_icon);
			virtual void		statusTextChange(const wstring& text);
			virtual void		titleChange(const wstring& text);
			virtual void		windowClosing(bool is_child_window,bool& cancel);
			virtual void		windowSetHeight(long height);
			virtual void		windowSetLeft(long left);
			virtual void		windowSetResizable(bool resizable);
			virtual void		windowSetTop(long top);
			virtual void		windowSetWidth(long width);
		public:
			friend class		IEWebBrowserEvents2;
		};
	};
};


