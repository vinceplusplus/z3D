
#include "IEBrowser.h"

#include "../../FreeImagePlus/Header.h"

namespace z3D
{
	namespace Core
	{
		HRESULT STDMETHODCALLTYPE		IEClientSite::QueryInterface(REFIID iid,void** ppvObject)
		{
			if(memcmp(&iid,&IID_IUnknown,sizeof(GUID))==0 || memcmp(&iid,&IID_IOleClientSite,sizeof(GUID))==0)
				*ppvObject=(void*)&ie->client_site;
			else if(memcmp(&iid,&IID_IOleInPlaceSite,sizeof(GUID))==0)
				*ppvObject=(void*)&ie->inplace_site;
			else if(memcmp(&iid,&IID_IDocHostUIHandler,sizeof(GUID))==0)
				*ppvObject=(void*)&ie->dochost_uihandler;
			else if(memcmp(&iid,&DIID_DWebBrowserEvents2,sizeof(GUID))==0)
				*ppvObject=(void*)&ie->event_handler;
			else
			{
				*ppvObject=NULL;
				return E_NOINTERFACE;
			}
			return S_OK;
		}

		ULONG STDMETHODCALLTYPE			IEClientSite::AddRef(void)
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE			IEClientSite::Release(void)
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE		IEClientSite::SaveObject()
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEClientSite::GetMoniker(DWORD dwAssign,DWORD dwWhichMoniker,IMoniker ** ppmk)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEClientSite::GetContainer(LPOLECONTAINER FAR* ppContainer)
		{
			*ppContainer=NULL;
			return E_NOINTERFACE;
		}

		HRESULT STDMETHODCALLTYPE		IEClientSite::ShowObject(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEClientSite::OnShowWindow(BOOL fShow)
		{
			return(NOERROR);
		}

		HRESULT STDMETHODCALLTYPE		IEClientSite::RequestNewObjectLayout(void)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::QueryInterface(REFIID iid,void** ppvObject)
		{
			return ie->client_site.QueryInterface(iid,ppvObject);
		}

		ULONG STDMETHODCALLTYPE			IEInPlaceSite::AddRef(void)
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE			IEInPlaceSite::Release(void)
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::GetWindow(HWND *phwnd)
		{
			*phwnd=ie->parent;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::ContextSensitiveHelp(BOOL fEnterMode)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::CanInPlaceActivate(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::OnInPlaceActivate(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::OnUIActivate(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::GetWindowContext(IOleInPlaceFrame **ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo)
		{
			*ppFrame=&ie->inplace_frame;
			*ppDoc=NULL;
			lpFrameInfo->fMDIApp = FALSE;
			lpFrameInfo->hwndFrame = ie->parent;
			lpFrameInfo->haccel = NULL;
			lpFrameInfo->cAccelEntries = 0;

			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::Scroll(SIZE scrollExtant)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::OnUIDeactivate(BOOL fUndoable)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::OnInPlaceDeactivate(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::DiscardUndoState(void)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::DeactivateAndUndo(void)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceSite::OnPosRectChange(LPCRECT lprcPosRect)
		{
			IOleInPlaceObject* inplace_object=NULL;

			if(SUCCEEDED(ie->browser_object->QueryInterface(IID_IOleInPlaceObject,(void**)&inplace_object)))
				inplace_object->SetObjectRects(lprcPosRect,lprcPosRect);

			if(inplace_object)
				inplace_object->Release();
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::QueryInterface(REFIID iid,void** ppvObject)
		{
			return E_NOTIMPL;
		}

		ULONG STDMETHODCALLTYPE			IEInPlaceFrame::AddRef(void)
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE			IEInPlaceFrame::Release(void)
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::GetWindow(HWND *phwnd)
		{
			*phwnd=ie->parent;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::ContextSensitiveHelp(BOOL fEnterMode)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::GetBorder(LPRECT lprectBorder)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::SetActiveObject(IOleInPlaceActiveObject *pActiveObject,LPCOLESTR pszObjName)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::InsertMenus(HMENU hmenuShared,LPOLEMENUGROUPWIDTHS lpMenuWidths)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::SetMenu(HMENU hmenuShared,HOLEMENU holemenu,HWND hwndActiveObject)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::RemoveMenus(HMENU hmenuShared)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::SetStatusText(LPCOLESTR pszStatusText)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::EnableModeless(BOOL fEnable)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEInPlaceFrame::TranslateAccelerator(LPMSG lpmsg,WORD wID)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::QueryInterface(REFIID iid,void** ppvObject)
		{
			return ie->client_site.QueryInterface(iid,ppvObject);
		}

		ULONG STDMETHODCALLTYPE			IEDocHostUIHandler::AddRef(void)
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE			IEDocHostUIHandler::Release(void)
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::ShowContextMenu(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO *pInfo)
		{
			pInfo->cbSize=sizeof(DOCHOSTUIINFO);
			//pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER;

			pInfo->dwFlags = 0;
			pInfo->dwFlags |= ie->option.no3dborder ? DOCHOSTUIFLAG_NO3DBORDER :0;
			pInfo->dwFlags |= ie->option.no3douterborder ? DOCHOSTUIFLAG_NO3DOUTERBORDER :0;
			pInfo->dwFlags |= ie->option.disable_selection ? DOCHOSTUIFLAG_DIALOG :0;
			pInfo->dwFlags |= ie->option.disable_help ? DOCHOSTUIFLAG_DISABLE_HELP_MENU :0;
			pInfo->dwFlags |= ie->option.disable_scroll ? DOCHOSTUIFLAG_SCROLL_NO :0;
			pInfo->dwFlags |= ie->option.disable_script ? DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE :0;
			pInfo->dwFlags |= ie->option.open_new_window ? DOCHOSTUIFLAG_OPENNEWWIN :0;
			pInfo->dwFlags |= ie->option.enable_flat_scrollbar ? DOCHOSTUIFLAG_FLAT_SCROLLBAR :0;
			pInfo->dwFlags |= ie->option.enable_autocomplete ? DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE :0;
			pInfo->dwFlags |= ie->option.enable_inplace_navigation ? DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION :0;
			pInfo->dwFlags |= ie->option.enable_ime_reconversion ? DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION :0;

			pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::ShowUI(DWORD dwID,IOleInPlaceActiveObject *pActiveObject,IOleCommandTarget *pCommandTarget,IOleInPlaceFrame *pFrame,IOleInPlaceUIWindow *pDoc)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::HideUI(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::UpdateUI(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::EnableModeless(BOOL fEnable)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::OnFrameWindowActivate(BOOL fActivate)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::ResizeBorder(LPCRECT prcBorder,IOleInPlaceUIWindow *pUIWindow,BOOL fRameWindow)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID)
		{
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::GetOptionKeyPath(LPOLESTR *pchKey,DWORD dw)
		{
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::GetDropTarget(IDropTarget *pDropTarget,IDropTarget **ppDropTarget)
		{
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::GetExternal(IDispatch **ppDispatch)
		{
			*ppDispatch=NULL;
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::TranslateUrl(DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut)
		{
			*ppchURLOut=NULL;
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE		IEDocHostUIHandler::FilterDataObject(IDataObject *pDO,IDataObject **ppDORet)
		{
			*ppDORet=NULL;
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::QueryInterface(REFIID iid,void** ppvObject)
		{
			return E_NOTIMPL;
		}

		ULONG STDMETHODCALLTYPE			IEStorage::AddRef(void)
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE			IEStorage::Release(void)
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::CreateStream(const OLECHAR *pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStream **ppstm)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::OpenStream(const OLECHAR *pwcsName,void *reserved1,DWORD grfMode,DWORD reserved2,IStream **ppstm)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::CreateStorage(const OLECHAR *pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStorage **ppstg)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::OpenStorage(const OLECHAR *pwcsName,IStorage *pstgPriority,DWORD grfMode,SNB snbExclude,DWORD reserved,IStorage **ppstg)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::CopyTo(DWORD ciidExclude,const IID *rgiidExclude,SNB snbExclude,IStorage *pstgDest)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::MoveElementTo(const OLECHAR *pwcsName,IStorage *pstgDest,const OLECHAR *pwcsNewName,DWORD grfFlags)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::Commit(DWORD grfCommitFlags)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::Revert(void)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::EnumElements(DWORD reserved1,void *reserved2,DWORD reserved3,IEnumSTATSTG **ppenum)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::DestroyElement(const OLECHAR *pwcsName)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::RenameElement(const OLECHAR *pwcsOldName,const OLECHAR *pwcsNewName)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::SetElementTimes(const OLECHAR *pwcsName,const FILETIME *pctime,const FILETIME *patime,const FILETIME *pmtime)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::SetClass(REFCLSID clsid)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::SetStateBits(DWORD grfStateBits,DWORD grfMask)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEStorage::Stat(STATSTG *pstatstg,DWORD grfStatFlag)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEWebBrowserEvents2::QueryInterface(REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject)
		{
			if(	memcmp(&riid,&IID_IUnknown,sizeof(GUID))==0||
				memcmp(&riid,&IID_IDispatch,sizeof(GUID))==0||
				memcmp(&riid,&IID_IDocHostUIHandler,sizeof(GUID))==0)
			{
				*ppvObject=(void*)&ie->dochost_uihandler;
				return S_OK;
			}
			return E_NOINTERFACE;
		}

		ULONG STDMETHODCALLTYPE			IEWebBrowserEvents2::AddRef( void)
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE			IEWebBrowserEvents2::Release( void)
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE		IEWebBrowserEvents2::GetTypeInfoCount(UINT *pctinfo)
		{
			*pctinfo=0;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE		IEWebBrowserEvents2::GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE		IEWebBrowserEvents2::GetIDsOfNames(REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId)
		{
			return E_NOTIMPL;
		}

		class DP
		{
		private:
			DISPPARAMS* p;
		public:
			DP(DISPPARAMS* p) {this->p=p;}
		public:
			VARIANT&		operator[](int index){return p->rgvarg[p->cArgs-1-index];}
			bool			checkArgs(int args) const {return p->cArgs==args;}
			bool			checkType(int index,VARTYPE type) {return (*this)[index].vt==type;}
			bool			checkType2(int index,VARTYPE type) {return (*this)[index].pvarVal->vt==type;}
		};

		HRESULT STDMETHODCALLTYPE		IEWebBrowserEvents2::Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr)
		{
			DP p(pDispParams);

			switch(dispIdMember)
			{
			case DISPID_BEFORENAVIGATE2:
				{
					if(!p.checkArgs(7)||
						!p.checkType(0,VT_DISPATCH)||!p.checkType(1,VT_VARIANT|VT_BYREF)||!p.checkType(2,VT_VARIANT|VT_BYREF)||
						!p.checkType(3,VT_VARIANT|VT_BYREF)||!p.checkType(4,VT_VARIANT|VT_BYREF)||
						!p.checkType(5,VT_VARIANT|VT_BYREF)||!p.checkType(6,VT_BOOL|VT_BYREF)||
						!p.checkType2(1,VT_BSTR)||!p.checkType2(4,VT_VARIANT|VT_BYREF))
						break;
					bool cancel=*p[6].pboolVal?true:false;
					ie->beforeNavigate2(p[0].pdispVal,
						p[1].pvarVal->bstrVal?p[1].pvarVal->bstrVal:L"",
						p[3].pvarVal->bstrVal?p[3].pvarVal->bstrVal:L"",
						*p[4].pvarVal,
						p[5].pvarVal->bstrVal?p[5].pvarVal->bstrVal:L"",
						cancel);
					*p[6].pboolVal=cancel;
				}
				break;
			case DISPID_CLIENTTOHOSTWINDOW:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_I4|VT_BYREF)||!p.checkType(1,VT_I4|VT_BYREF))
						break;
					ie->clientToHostWindow(*p[0].plVal,*p[1].plVal);
				}
				break;
			case DISPID_COMMANDSTATECHANGE:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_I4)||!p.checkType(1,VT_BOOL))
						break;
					ie->commandStateChange(p[0].lVal,p[1].boolVal?true:false);
				}
				break;
			case DISPID_DOCUMENTCOMPLETE:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_DISPATCH)||!p.checkType(1,VT_VARIANT|VT_BYREF)||!p.checkType2(1,VT_BSTR))
						break;
					ie->documentComplete(
						p[0].pdispVal,
						p[1].pvarVal->bstrVal?p[1].pvarVal->bstrVal:L"");
				}
				break;
			case DISPID_DOWNLOADBEGIN:
				{
					if(!p.checkArgs(0))
						break;
					ie->downloadBegin();
				}
				break;
			case DISPID_DOWNLOADCOMPLETE:
				{
					if(!p.checkArgs(0))
						break;
					ie->downloadComplete();
				}
				break;
			case DISPID_FILEDOWNLOAD:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_BOOL)||!p.checkType(1,VT_BYREF|VT_BOOL))
						break;
					bool cancel=*p[1].pboolVal?true:false;
					ie->fileDownload(cancel);
					*p[1].pboolVal=cancel;
				}
				break;
			case DISPID_NAVIGATECOMPLETE2:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_DISPATCH)||!p.checkType(1,VT_BYREF|VT_VARIANT)||!p.checkType2(1,VT_BSTR))
						break;
					ie->navigateComplete2(p[0].pdispVal,p[1].pvarVal->bstrVal?p[1].pvarVal->bstrVal:L"");
				}
				break;
			case DISPID_NAVIGATEERROR:
				{
					if(	!p.checkArgs(5)||!p.checkType(0,VT_DISPATCH)||!p.checkType(1,VT_BYREF|VT_VARIANT)||!p.checkType2(1,VT_BSTR)||
						!p.checkType(2,VT_BYREF|VT_VARIANT)||!p.checkType2(2,VT_BSTR)||
						!p.checkType(3,VT_BYREF|VT_VARIANT)||!p.checkType2(3,VT_I4)||!p.checkType(4,VT_BOOL|VT_BYREF))
						break;
					bool cancel=*p[4].pboolVal?true:false;
					ie->navigateError(p[0].pdispVal,p[1].pvarVal->bstrVal?p[1].pvarVal->bstrVal:L"",p[2].pvarVal->bstrVal?p[2].pvarVal->bstrVal:L"",p[3].pvarVal->lVal,cancel);
					*p[4].pboolVal=cancel;
				}
				break;
			case DISPID_NEWWINDOW2:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_DISPATCH|VT_BYREF)||!p.checkType(1,VT_BYREF|VT_BOOL))
						break;
					bool cancel=*p[1].pboolVal?true:false;
					ie->newWindow2(*p[0].ppdispVal,cancel);
					*p[1].pboolVal=cancel;
				}
				break;
			case DISPID_ONFULLSCREEN:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->onFullscreen(p[0].boolVal?true:false);
				}
				break;
			case DISPID_ONMENUBAR:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->onMenuBar(p[0].boolVal?true:false);
				}
				break;
			case DISPID_ONQUIT:
				{
					if(!p.checkArgs(0))
						break;
					ie->onQuit();
				}
				break;
			case DISPID_ONSTATUSBAR:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->onStatusBar(p[0].boolVal?true:false);
				}
				break;
			case DISPID_ONTHEATERMODE:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->onTheaterMode(p[0].boolVal?true:false);
				}
				break;
			case DISPID_ONTOOLBAR:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->onToolBar(p[0].boolVal?true:false);
				}
				break;
			case DISPID_ONVISIBLE:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->onVisible(p[0].boolVal?true:false);
				}
				break;
			case DISPID_PRINTTEMPLATEINSTANTIATION:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_DISPATCH))
						break;
					ie->printTemplateInitantiation(p[0].pdispVal);
				}
				break;
			case DISPID_PRINTTEMPLATETEARDOWN:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_DISPATCH))
						break;
					ie->printTemplateTearDown(p[0].pdispVal);
				}
				break;
			case DISPID_PRIVACYIMPACTEDSTATECHANGE:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->privacyImpactedStateChanged(p[0].boolVal?true:false);
				}
				break;
			case DISPID_PROGRESSCHANGE:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_I4)||!p.checkType(1,VT_I4))
						break;
					ie->progressChange(p[0].lVal,p[1].lVal);
				}
				break;
			case DISPID_PROPERTYCHANGE:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BSTR))
						break;
					ie->propertyChange(p[0].bstrVal);
				}
				break;
			case DISPID_SETSECURELOCKICON:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_I4))
						break;
					ie->setSecureLockIcon(p[0].lVal);
				}
				break;
			case DISPID_STATUSTEXTCHANGE:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BSTR))
						break;
					ie->statusTextChange(p[0].bstrVal);
				}
				break;
			case DISPID_TITLECHANGE:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BSTR))
						break;
					ie->titleChange(p[0].bstrVal);
				}
				break;
			case DISPID_WINDOWCLOSING:
				{
					if(!p.checkArgs(2)||!p.checkType(0,VT_BOOL)||!p.checkType(1,VT_BOOL|VT_BYREF))
						break;
					bool cancel=*p[1].pboolVal?true:false;
					ie->windowClosing(p[0].boolVal?true:false,cancel);
					*p[1].pboolVal=cancel;
				}
				break;
			case DISPID_WINDOWSETHEIGHT:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_I4))
						break;
					ie->windowSetHeight(p[0].lVal);
				}
				break;
			case DISPID_WINDOWSETLEFT:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_I4))
						break;
					ie->windowSetLeft(p[0].lVal);
				}
				break;
			case DISPID_WINDOWSETRESIZABLE:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_BOOL))
						break;
					ie->windowSetResizable(p[0].boolVal?true:false);
				}
				break;
			case DISPID_WINDOWSETTOP:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_I4))
						break;
					ie->windowSetTop(p[0].lVal);
				}
				break;
			case DISPID_WINDOWSETWIDTH:
				{
					if(!p.checkArgs(1)||!p.checkType(0,VT_I4))
						break;
					ie->windowSetWidth(p[0].lVal);
				}
				break;
			default:
				return DISP_E_MEMBERNOTFOUND;
			}
			return S_OK;
		}


		IEBrowser::IEBrowser()
		{
			parent=NULL;

			browser_object=NULL;

			browser=NULL;

			client_site.ie = this;
			inplace_site.ie = this;
			inplace_frame.ie = this;
			dochost_uihandler.ie = this;
			storage.ie = this;
			event_handler.ie = this;

			setOptions();

			memdc = NULL;
			membitmap = NULL;

			memset(&display_rect,0,sizeof(display_rect));
		}

		IEBrowser::~IEBrowser()
		{
			destroy();
			destroyCaptureTemp();
		}


		void				IEBrowser::setOptions(bool no3dborder,bool no3douterborder,bool disable_selection,bool disable_help,bool disable_scroll,bool disable_script,bool open_new_window,bool enable_flat_scrollbar,bool enable_autocomplete, bool enable_inplace_navigation,bool enable_ime_reconversion)
		{
			option.no3dborder = no3dborder;
			option.no3douterborder = no3douterborder;
			option.disable_selection = disable_selection;
			option.disable_help = disable_help;
			option.disable_scroll = disable_scroll;
			option.disable_script = disable_script;
			option.open_new_window = open_new_window;
			option.enable_flat_scrollbar = enable_flat_scrollbar;
			option.enable_autocomplete = enable_autocomplete;
			option.enable_inplace_navigation = enable_inplace_navigation;
			option.enable_ime_reconversion = enable_ime_reconversion;
		}

		bool				IEBrowser::create(HWND parent,int x,int y,int width,int height)
		{
			OleInitialize(NULL);
			destroy();
			this->parent=parent;
			if(FAILED(OleCreate(CLSID_WebBrowser,IID_IOleObject,OLERENDER_DRAW,NULL,&client_site,&storage,(void**)&browser_object)))
				return false;
			browser_object->SetHostNames(L"z3D Application",NULL);
			OleSetContainedObject(browser_object,TRUE);
			RECT rt={x,y,x+width,y+height};
			browser_object->DoVerb(OLEIVERB_SHOW,NULL,&client_site,-1,parent,&rt);
			browser_object->QueryInterface(IID_IWebBrowser2,(void**)&browser);
			browser->put_Left(x);
			browser->put_Top(y);
			browser->put_Width(width);
			browser->put_Height(height);

			/********************
			Add Event Sink
			********************/

			IConnectionPointContainer* cpc=NULL;
			IConnectionPoint* pc=NULL;

			if(SUCCEEDED(browser_object->QueryInterface(IID_IConnectionPointContainer,(void**)&cpc)))
			{
				if(SUCCEEDED(cpc->FindConnectionPoint(DIID_DWebBrowserEvents2,&pc)))
				{
					unsigned long cookie = 1;
					pc->Advise(&event_handler,&cookie);

					pc->Release();
				}
				cpc->Release();
			}

			return true;
		}

		void				IEBrowser::destroy()
		{
			if(!browser_object)
				return;
			if(browser)
			{
				browser->Release();
				browser=NULL;
			}
			browser_object->Close(OLECLOSE_NOSAVE);
			browser_object->Release();
			browser_object=NULL;

			OleUninitialize();
		}

		void				IEBrowser::navigate(const wstring& url)
		{
			if(!browser_object)
				return;

			VARIANT var_url;

			VariantInit(&var_url);
			var_url.vt=VT_BSTR;
			var_url.bstrVal=SysAllocString(url.c_str());
			browser->Navigate2(&var_url,NULL,NULL,NULL,NULL);
			VariantClear(&var_url);
		}

		bool				IEBrowser::capture(int& width,int& height)
		{
			long width2,height2;
			browser->get_Width(&width2);
			browser->get_Height(&height2);

			width=(int)width2;
			height=(int)height2;

			if(!width||!height)
				return false;

			bool create_mem=false;
			if(!memdc)
				create_mem=true;
			if(memdc)
			{
				BITMAP bitmap;
				GetObject(membitmap,sizeof(BITMAP),&bitmap);

				if(width!=bitmap.bmWidth||height!=bitmap.bmHeight)
					create_mem=true;
			}
			if(create_mem)
			{
				destroyCaptureTemp();

				HDC deskdc=GetDC(GetDesktopWindow());

				memdc=CreateCompatibleDC(deskdc);

				BITMAPINFO bi;

				bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
				bi.bmiHeader.biWidth=width;
				bi.bmiHeader.biHeight=height;
				bi.bmiHeader.biPlanes=1;
				bi.bmiHeader.biBitCount=24;
				bi.bmiHeader.biCompression=BI_RGB;
				bi.bmiHeader.biSizeImage=0;
				bi.bmiHeader.biXPelsPerMeter=3000;
				bi.bmiHeader.biYPelsPerMeter=3000;
				bi.bmiHeader.biClrUsed=0;
				bi.bmiHeader.biClrImportant=0;

				bits=NULL;

				membitmap=CreateDIBSection(deskdc,&bi,DIB_RGB_COLORS,&bits,NULL,0);

				ReleaseDC(GetDesktopWindow(),deskdc);

				SelectObject(memdc,membitmap);
			}
			RECT rt={0,0,width,height};
			OleDraw(browser,DVASPECT_CONTENT,memdc,&rt);

			return true;
		}

		bool				IEBrowser::capture(void* _fipImage)
		{
			fipImage& img=*(fipImage*)_fipImage;

			int width,height;
			if(!capture(width,height))
				return false;

			img.setSize(FIT_BITMAP,width,height,24);
			memcpy(img.accessPixels(),bits,((width*3)+3)/4*4*height);

			return true;
		}

		bool				IEBrowser::capture_raw24bit(vector<unsigned char>& array_byte,int& width,int height)
		{
			if(!capture(width,height))
				return false;

			array_byte.resize(((width*3)+3)/4*4*height);
			memcpy(&array_byte[0],bits,((width*3)+3)/4*4*height);

			return true;
		}

		void				IEBrowser::destroyCaptureTemp()
		{
			DeleteDC(memdc);
			DeleteObject(membitmap);

			memdc=NULL;
			membitmap=NULL;
		}

		void				IEBrowser::setPos(int x,int y)
		{
			browser->put_Left(x);
			browser->put_Top(y);
		}

		int					IEBrowser::getX() const
		{
			long x;
			browser->get_Left(&x);
			return (int)x;
		}

		int					IEBrowser::getY() const
		{
			long y;
			browser->get_Top(&y);
			return (int)y;
		}

		void				IEBrowser::setExt(int dx,int dy)
		{
			browser->put_Width(dx);
			browser->put_Height(dy);
		}

		int					IEBrowser::getDX() const
		{
			long dx;
			browser->get_Width(&dx);
			return (int)dx;
		}

		int					IEBrowser::getDY() const
		{
			long dy;
			browser->get_Height(&dy);
			return (int)dy;
		}

		void				IEBrowser::saveDisplayRect()
		{
			display_rect.left=getX();
			display_rect.top=getY();
			display_rect.right=getX()+getDX();
			display_rect.bottom=getY()+getDY();
		}

		void				IEBrowser::loadDisplayRect()
		{
			browser->put_Left(display_rect.left);
			browser->put_Top(display_rect.top);
			browser->put_Width(display_rect.right-display_rect.left);
			browser->put_Height(display_rect.bottom-display_rect.top);
		}

		void				IEBrowser::setInvisible()
		{
			browser->put_Width(0);
			browser->put_Height(0);
		}

		void				IEBrowser::beforeNavigate2(IDispatch* disp,const wstring& url,const wstring& target_frame,VARIANT& post_data,const wstring& headers,bool& cancel)
		{
		}

		void				IEBrowser::clientToHostWindow(long& cx,long& cy)
		{
		}

		void				IEBrowser::commandStateChange(long command,bool enabled)
		{
		}

		void				IEBrowser::documentComplete(IDispatch* disp,const wstring& url)
		{
		}

		void				IEBrowser::downloadBegin()
		{
		}

		void				IEBrowser::downloadComplete()
		{
		}

		void				IEBrowser::fileDownload(bool& cancel)
		{
		}

		void				IEBrowser::navigateComplete2(IDispatch* disp,const wstring& url)
		{
		}

		void				IEBrowser::navigateError(IDispatch* disp,const wstring& url,const wstring& target_frame,long status_code,bool& cancel)
		{
		}

		void				IEBrowser::newWindow2(IDispatch*& disp,bool& cancel)
		{
		}

		void				IEBrowser::onFullscreen(bool fullscreen)
		{
		}

		void				IEBrowser::onMenuBar(bool shown)
		{
		}

		void				IEBrowser::onQuit()
		{
		}

		void				IEBrowser::onStatusBar(bool shown)
		{
		}

		void				IEBrowser::onTheaterMode(bool theater_mode)
		{
		}

		void				IEBrowser::onToolBar(bool shown)
		{
		}

		void				IEBrowser::onVisible(bool visible)
		{
		}

		void				IEBrowser::printTemplateInitantiation(IDispatch* disp)
		{
		}

		void				IEBrowser::printTemplateTearDown(IDispatch* disp)
		{
		}

		void				IEBrowser::privacyImpactedStateChanged(bool privacyimpacted)
		{
		}

		void				IEBrowser::progressChange(long progress,long progress_max)
		{
		}

		void				IEBrowser::propertyChange(const wstring& property_name)
		{
		}

		void				IEBrowser::setSecureLockIcon(long secure_lock_icon)
		{
		}

		void				IEBrowser::statusTextChange(const wstring& text)
		{
		}

		void				IEBrowser::titleChange(const wstring& text)
		{
		}

		void				IEBrowser::windowClosing(bool is_child_window,bool& cancel)
		{
		}

		void				IEBrowser::windowSetHeight(long height)
		{
		}

		void				IEBrowser::windowSetLeft(long left)
		{
		}

		void				IEBrowser::windowSetResizable(bool resizable)
		{
		}

		void				IEBrowser::windowSetTop(long top)
		{
		}

		void				IEBrowser::windowSetWidth(long width)
		{
		}
	};
};


