
#include "W32Flash.h"

namespace z3D
{
	namespace Core
	{
		void						W32Flash::before_shown()
		{
			IConnectionPointContainer* cpc=NULL;
			IConnectionPoint* cp=NULL;
			_control->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc);
			if(cpc)
				cpc->FindConnectionPoint(ShockwaveFlashObjects::DIID__IShockwaveFlashEvents, &cp);
			if(cp)
				cp->Advise(dynamic_cast<ShockwaveFlashObjects::_IShockwaveFlashEvents*>(this), NULL);
			if(cp) cp->Release();
			if(cpc) cpc->Release();

			_control->PutWMode(L"Transparent");
			_control->PutScale(L"showAll");
			_control->PutBackgroundColor(0x00000000);
			_control->PutEmbedMovie(FALSE);
		}

		void						W32Flash::after_shown()
		{
			_control->PutEmbedMovie(TRUE);
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::QueryInterface(REFIID riid, void ** ppvObject)
		{
			if(SUCCEEDED(W32OleContainer<ShockwaveFlashObjects::IShockwaveFlash>::QueryInterface(riid,ppvObject)))
				return S_OK;
			if (IsEqualGUID(riid, ShockwaveFlashObjects::DIID__IShockwaveFlashEvents))
				*ppvObject=(void*)dynamic_cast<ShockwaveFlashObjects::_IShockwaveFlashEvents*>(this);
			else if(IsEqualGUID(riid, IID_IServiceProvider))
				*ppvObject=(void*)dynamic_cast<IServiceProvider*>(this);
			else
			{
				*ppvObject=NULL;
				return E_NOINTERFACE;
			}
			return S_OK;
		}

		ULONG STDMETHODCALLTYPE		W32Flash::AddRef()
		{
			return 1;
		}

		ULONG STDMETHODCALLTYPE		W32Flash::Release()
		{
			return 1;
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::GetTypeInfoCount(UINT __RPC_FAR *pctinfo)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::GetIDsOfNames(REFIID riid,LPOLESTR __RPC_FAR *rgszNames,UINT cNames,LCID lcid,DISPID __RPC_FAR *rgDispId)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS __RPC_FAR *pDispParams,VARIANT __RPC_FAR *pVarResult,EXCEPINFO __RPC_FAR *pExcepInfo,UINT __RPC_FAR *puArgErr)
		{
			return(S_OK);
		}

		HRESULT __stdcall			W32Flash::raw_RemoteQueryService(GUID * guidService,GUID * riid,IUnknown** ppvObject)
		{
			return E_NOINTERFACE;
		}

		HRESULT __stdcall			W32Flash::QueryService(REFGUID guidService,REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject)
		{
			return raw_RemoteQueryService((GUID*)&guidService,(GUID*)&riid,(IUnknown**)ppvObject);
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::OnReadyStateChange(long newState)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::OnProgress(long percentDone)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE	W32Flash::FSCommand(_bstr_t command,_bstr_t args)
		{
			return S_OK;
		}

	};
};

