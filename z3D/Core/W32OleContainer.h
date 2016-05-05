//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// including this header in "z3D/z3DCore/Header.h" may introduce many compiler errors in other z3D engine projects
// therefore, include this header manually in your program if you want to use the following class(es)

#pragma once

#include <objbase.h>
#include <ole2.h>
#include <exdisp.h>
#include <exdispid.h>

#include "predecl.h"

#pragma comment(lib,"ole32.lib")

namespace z3D
{
	namespace Core
	{
		template<class T> class W32OleContainer;

		template<class T>
		class W32WindowlessHandler
		{
		public:
			virtual void			invalidate_rect(W32OleContainer<T>* container,const RECT& rt) {}
		};

		template<class T>
		class W32OleContainer:
			virtual public IOleClientSite,
		//	virtual public IOleInPlaceSite,
			virtual public IOleInPlaceSiteWindowless,
			virtual public IOleInPlaceFrame,
			virtual public IStorage
		{
		public:
			GUID					_clsid;
			T*						_control;
			IOleObject*				_object;
		public:
			HWND					_parent;
			int						_x;
			int						_y;
			int						_dx;
			int						_dy;
		public:
			bool					_windowless;
		protected:
			W32WindowlessHandler<T>*	_windowless_handler;
		public:
			W32OleContainer();
			virtual ~W32OleContainer();
		public:
			T*						get_control();
			IOleObject*				get_object();
		public:
			void					set_windowless_handler(W32WindowlessHandler<T>* handler);
		public:
			virtual bool			create(GUID clsid,HWND parent,int x,int y,int dx,int dy,bool windowless=true);
		public:
			virtual void			before_shown();
			virtual void			after_shown();
		public:						// IUnknown
			HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void ** ppvObject);
			ULONG STDMETHODCALLTYPE		AddRef();
			ULONG STDMETHODCALLTYPE		Release();
		public:						// IOleClientSite
			HRESULT STDMETHODCALLTYPE	SaveObject();
			HRESULT STDMETHODCALLTYPE	GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk);
			HRESULT STDMETHODCALLTYPE	GetContainer(LPOLECONTAINER FAR* ppContainer);
			HRESULT STDMETHODCALLTYPE	ShowObject();
			HRESULT STDMETHODCALLTYPE	OnShowWindow(BOOL fShow);
			HRESULT STDMETHODCALLTYPE	RequestNewObjectLayout();
		public:						// IOleWindow
			HRESULT STDMETHODCALLTYPE	GetWindow(HWND FAR* lphwnd);
			HRESULT STDMETHODCALLTYPE	ContextSensitiveHelp(BOOL fEnterMode);
		public:						// IOleInPlaceSite
			HRESULT STDMETHODCALLTYPE	CanInPlaceActivate();
			HRESULT STDMETHODCALLTYPE	OnInPlaceActivate();
			HRESULT STDMETHODCALLTYPE	OnUIActivate();
			HRESULT STDMETHODCALLTYPE	GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo);
			HRESULT STDMETHODCALLTYPE	Scroll(SIZE scrollExtent);
			HRESULT STDMETHODCALLTYPE	OnUIDeactivate(BOOL fUndoable);
			HRESULT STDMETHODCALLTYPE	OnInPlaceDeactivate();
			HRESULT STDMETHODCALLTYPE	DiscardUndoState();
			HRESULT STDMETHODCALLTYPE	DeactivateAndUndo();
			HRESULT STDMETHODCALLTYPE	OnPosRectChange(LPCRECT lprcPosRect);
		public:						// IOleInPlaceSiteEx
			HRESULT STDMETHODCALLTYPE	OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags);
			HRESULT STDMETHODCALLTYPE	OnInPlaceDeactivateEx(BOOL fNoRedraw);
			HRESULT STDMETHODCALLTYPE	RequestUIActivate(void);
		public:						// IOleInPlaceSiteWindowless
			HRESULT STDMETHODCALLTYPE	CanWindowlessActivate(void);
			HRESULT STDMETHODCALLTYPE	GetCapture(void);
			HRESULT STDMETHODCALLTYPE	SetCapture(BOOL fCapture);
			HRESULT STDMETHODCALLTYPE	GetFocus(void);
			HRESULT STDMETHODCALLTYPE	SetFocus(BOOL fFocus);
			HRESULT STDMETHODCALLTYPE	GetDC(LPCRECT pRect,DWORD grfFlags,HDC __RPC_FAR *phDC);
			HRESULT STDMETHODCALLTYPE	ReleaseDC(HDC hDC);
			HRESULT STDMETHODCALLTYPE	InvalidateRect(LPCRECT pRect,BOOL fErase);
			HRESULT STDMETHODCALLTYPE	InvalidateRgn(HRGN hRGN,BOOL fErase);
			HRESULT STDMETHODCALLTYPE	ScrollRect(INT dx,INT dy,LPCRECT pRectScroll,LPCRECT pRectClip);
			HRESULT STDMETHODCALLTYPE	AdjustRect(LPRECT prc);
			HRESULT STDMETHODCALLTYPE	OnDefWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam,LRESULT __RPC_FAR *plResult);
		public:						// IOleInPlaceUIWindow
			HRESULT STDMETHODCALLTYPE	GetBorder(LPRECT lprectBorder);
			HRESULT STDMETHODCALLTYPE	RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
			HRESULT STDMETHODCALLTYPE	SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
			HRESULT STDMETHODCALLTYPE	SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);
		public:						// IOleInPlaceFrame
			HRESULT STDMETHODCALLTYPE	InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
			HRESULT STDMETHODCALLTYPE	SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
			HRESULT STDMETHODCALLTYPE	RemoveMenus(HMENU hmenuShared);
			HRESULT STDMETHODCALLTYPE	SetStatusText(LPCOLESTR pszStatusText);
			HRESULT STDMETHODCALLTYPE	EnableModeless(BOOL fEnable);
			HRESULT STDMETHODCALLTYPE	TranslateAccelerator(LPMSG lpmsg, WORD wID);
		public:						// IStorage
			HRESULT STDMETHODCALLTYPE	CreateStream(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm);
			HRESULT STDMETHODCALLTYPE	OpenStream(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm);
			HRESULT STDMETHODCALLTYPE	CreateStorage(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg);
			HRESULT STDMETHODCALLTYPE	OpenStorage(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg);
			HRESULT STDMETHODCALLTYPE	CopyTo(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest);
			HRESULT STDMETHODCALLTYPE	MoveElementTo(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags);
			HRESULT STDMETHODCALLTYPE	Commit(DWORD grfCommitFlags);
			HRESULT STDMETHODCALLTYPE	Revert();
			HRESULT STDMETHODCALLTYPE	EnumElements(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum);
			HRESULT STDMETHODCALLTYPE	DestroyElement(const OLECHAR *pwcsName);
			HRESULT STDMETHODCALLTYPE	RenameElement(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName);
			HRESULT STDMETHODCALLTYPE	SetElementTimes(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime);
			HRESULT STDMETHODCALLTYPE	SetClass(REFCLSID clsid);
			HRESULT STDMETHODCALLTYPE	SetStateBits(DWORD grfStateBits, DWORD grfMask);
			HRESULT STDMETHODCALLTYPE	Stat(STATSTG * pstatstg, DWORD grfStatFlag);
		public:						// IDispatch
			HRESULT STDMETHODCALLTYPE	GetTypeInfoCount(UINT __RPC_FAR *pctinfo);
			HRESULT STDMETHODCALLTYPE	GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
			HRESULT STDMETHODCALLTYPE	GetIDsOfNames(REFIID riid,LPOLESTR __RPC_FAR *rgszNames,UINT cNames,LCID lcid,DISPID __RPC_FAR *rgDispId);
			HRESULT STDMETHODCALLTYPE	Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS __RPC_FAR *pDispParams,VARIANT __RPC_FAR *pVarResult,EXCEPINFO __RPC_FAR *pExcepInfo,UINT __RPC_FAR *puArgErr);
		};


		template<class T> W32OleContainer<T>::W32OleContainer()
		{
			OleInitialize(NULL);

			_control = NULL;
			_object = NULL;

			_parent = NULL;

			_x=0;
			_y=0;
			_dx=0;
			_dy=0;

			_windowless_handler = NULL;
		}

		template<class T> W32OleContainer<T>::~W32OleContainer()
		{
			if(_control)
				_control->Release();
			if(_object)
			{
				_object->Close(OLECLOSE_NOSAVE);
				_object->Release();
			}
			OleUninitialize();
		}

		template<class T> T*			W32OleContainer<T>::get_control()
		{
			return _control;
		}

		template<class T> IOleObject*	W32OleContainer<T>::get_object()
		{
			return _object;
		}

		template<class T> void			W32OleContainer<T>::set_windowless_handler(W32WindowlessHandler<T>* handler)
		{
			_windowless_handler=handler;
		}

		template<class T> bool			W32OleContainer<T>::create(GUID clsid,HWND parent,int x,int y,int dx,int dy,bool windowless)
		{
			if(_object)		// existing, cannot be created again
				return false;
			_parent=parent;
			_x=x;
			_y=y;
			_dx=dx;
			_dy=dy;
			_windowless=windowless;
			if(FAILED(OleCreate(clsid,IID_IOleObject,OLERENDER_DRAW,NULL,this,this,(void**)&_object)))
				return false;
			if(FAILED(OleSetContainedObject(_object,TRUE)))
				return false;
			if(FAILED(_object->QueryInterface(__uuidof(T),(void**)&_control)))
				return false;

			before_shown();

			RECT rt;
			rt.left=x;
			rt.top=y;
			rt.right=rt.left+dx;
			rt.bottom=rt.top+dx;
			if(FAILED(_object->DoVerb(OLEIVERB_SHOW,NULL,this,0,_parent,&rt)))
				return false;

			after_shown();

			return true;
		}

		template<class T> void			W32OleContainer<T>::before_shown()
		{
		}

		template<class T> void			W32OleContainer<T>::after_shown()
		{
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::QueryInterface(REFIID riid, void ** ppvObject)
		{
			/*
			if(IsEqualGUID(riid,IID_IUnknown))
				*ppvObject=(void*)dynamic_cast<IUnknown*>(this);
			else */if(IsEqualGUID(riid,IID_IOleClientSite))
				*ppvObject=(void*)dynamic_cast<IOleClientSite*>(this);
				/*
			else if(IsEqualGUID(riid,IID_IOleWindow))
				*ppvObject=(void*)dynamic_cast<IOleWindow*>(this);*/
			else if(IsEqualGUID(riid,IID_IOleInPlaceUIWindow))
				*ppvObject=(void*)dynamic_cast<IOleInPlaceUIWindow*>(this);
			else if(IsEqualGUID(riid,IID_IOleInPlaceSite))
				*ppvObject=(void*)dynamic_cast<IOleInPlaceSite*>(this);
			else if(IsEqualGUID(riid,IID_IOleInPlaceSiteEx))
				*ppvObject=(void*)dynamic_cast<IOleInPlaceSiteEx*>(this);
			else if(_windowless && IsEqualGUID(riid,IID_IOleInPlaceSiteWindowless))
				*ppvObject=(void*)dynamic_cast<IOleInPlaceSiteWindowless*>(this);
			else if(IsEqualGUID(riid,IID_IOleInPlaceFrame))
				*ppvObject=(void*)dynamic_cast<IOleInPlaceFrame*>(this);
			else if(IsEqualGUID(riid,IID_IStorage))
				*ppvObject=(void*)dynamic_cast<IStorage*>(this);
			/*
			else if(IsEqualGUID(riid,IID_IDispatch))
				*ppvObject=(void*)dynamic_cast<IDispatch*>(this);*/
			else
			{
				*ppvObject=NULL;
				return E_NOINTERFACE;
			}
			if(!*ppvObject)
				return E_NOINTERFACE;
			return S_OK;
		}

		template<class T> ULONG STDMETHODCALLTYPE		W32OleContainer<T>::AddRef()
		{
			return 1;
		}

		template<class T> ULONG STDMETHODCALLTYPE		W32OleContainer<T>::Release()
		{
			return 1;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SaveObject()
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetContainer(LPOLECONTAINER FAR* ppContainer)
		{
			*ppContainer = NULL;
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::ShowObject()
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnShowWindow(BOOL fShow)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::RequestNewObjectLayout()
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetWindow(HWND FAR* lphwnd)
		{
			*lphwnd=_parent;
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::ContextSensitiveHelp(BOOL fEnterMode)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::CanInPlaceActivate()
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnInPlaceActivate()
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnUIActivate()
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo)
		{
			*lplpFrame=this;
			*lplpDoc=NULL;

			lpFrameInfo->fMDIApp = FALSE;
			lpFrameInfo->hwndFrame = _parent;
			lpFrameInfo->haccel = NULL;
			lpFrameInfo->cAccelEntries = 0;

			RECT rt={_x,_y,_x+_dx,_y+_dy};

			*lprcPosRect=rt;
			*lprcClipRect=rt;

			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::Scroll(SIZE scrollExtent)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnUIDeactivate(BOOL fUndoable)
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnInPlaceDeactivate()
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::DiscardUndoState()
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::DeactivateAndUndo()
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnPosRectChange(LPCRECT lprcPosRect)
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags)
		{
			if(pfNoRedraw)
				*pfNoRedraw = FALSE;
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnInPlaceDeactivateEx(BOOL fNoRedraw)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::RequestUIActivate(void)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::CanWindowlessActivate(void)
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetCapture(void)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetCapture(BOOL fCapture)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetFocus(void)
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetFocus(BOOL fFocus)
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetDC(LPCRECT pRect,DWORD grfFlags,HDC __RPC_FAR *phDC)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::ReleaseDC(HDC hDC)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::InvalidateRect(LPCRECT pRect,BOOL fErase)
		{
			/*
			if(_windowless && _windowless_invalidate_parent)
			{
				RECT rt;
				if(pRect)
					rt=*pRect;
				else
				{
					rt.left=0;
					rt.top=0;
					rt.right=_dx;
					rt.bottom=_dy;
				}
				OffsetRect(&rt,_x,_y);
				::InvalidateRect(_parent,&rt,FALSE);
				::UpdateWindow(_parent);
			}*/
			RECT rt;
			if(pRect)
				rt=*pRect;
			else
			{
				rt.left=0;
				rt.top=0;
				rt.right=_dx;
				rt.bottom=_dy;
			}
			if(_windowless_handler)
				_windowless_handler->invalidate_rect(this,rt);

			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::InvalidateRgn(HRGN hRGN,BOOL fErase)
		{
			return S_OK;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::ScrollRect(INT dx,INT dy,LPCRECT pRectScroll,LPCRECT pRectClip)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::AdjustRect(LPRECT prc)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OnDefWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam,LRESULT __RPC_FAR *plResult)
		{
			return S_FALSE;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetBorder(LPRECT lprectBorder)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
		{
			return(S_OK);
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
		{
			return(S_OK);
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::RemoveMenus(HMENU hmenuShared)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetStatusText(LPCOLESTR pszStatusText)
		{
			return(S_OK);
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::EnableModeless(BOOL fEnable)
		{
			return(S_OK);
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::TranslateAccelerator(LPMSG lpmsg, WORD wID)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::CreateStream(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OpenStream(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::CreateStorage(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::OpenStorage(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::CopyTo(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::MoveElementTo(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::Commit(DWORD grfCommitFlags)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::Revert()
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::EnumElements(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::DestroyElement(const OLECHAR *pwcsName)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::RenameElement(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetElementTimes(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetClass(REFCLSID clsid)
		{
			return(S_OK);
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::SetStateBits(DWORD grfStateBits, DWORD grfMask)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::Stat(STATSTG * pstatstg, DWORD grfStatFlag)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetTypeInfoCount(UINT __RPC_FAR *pctinfo)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::GetIDsOfNames(REFIID riid,LPOLESTR __RPC_FAR *rgszNames,UINT cNames,LCID lcid,DISPID __RPC_FAR *rgDispId)
		{
			return E_NOTIMPL;
		}

		template<class T> HRESULT STDMETHODCALLTYPE	W32OleContainer<T>::Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS __RPC_FAR *pDispParams,VARIANT __RPC_FAR *pVarResult,EXCEPINFO __RPC_FAR *pExcepInfo,UINT __RPC_FAR *puArgErr)
		{
			return(S_OK);
		}
	};
};



