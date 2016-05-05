//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// including this header in "z3D/z3DCore/Header.h" may introduce many compiler errors in other z3D engine projects
// therefore, include this header manually in your program if you want to use the following class(es)

#pragma once

#include "predecl.h"

#include "W32OleContainer.h"
#include "flash.tlh"

#pragma warning(disable:4275)

namespace z3D
{
	namespace Core
	{
		class W32Flash:
			virtual public W32OleContainer<ShockwaveFlashObjects::IShockwaveFlash>,
			virtual public ShockwaveFlashObjects::_IShockwaveFlashEvents,
			virtual public IServiceProvider
		{
		public:
			void						before_shown();
			void						after_shown();
		public:
			HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void ** ppvObject);
			ULONG STDMETHODCALLTYPE		AddRef();
			ULONG STDMETHODCALLTYPE		Release();
		public:						// IDispatch
			HRESULT STDMETHODCALLTYPE	GetTypeInfoCount(UINT __RPC_FAR *pctinfo);
			HRESULT STDMETHODCALLTYPE	GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
			HRESULT STDMETHODCALLTYPE	GetIDsOfNames(REFIID riid,LPOLESTR __RPC_FAR *rgszNames,UINT cNames,LCID lcid,DISPID __RPC_FAR *rgDispId);
			HRESULT STDMETHODCALLTYPE	Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS __RPC_FAR *pDispParams,VARIANT __RPC_FAR *pVarResult,EXCEPINFO __RPC_FAR *pExcepInfo,UINT __RPC_FAR *puArgErr);
		public:						// IServiceProvider
			HRESULT __stdcall			raw_RemoteQueryService(GUID * guidService,GUID * riid,IUnknown** ppvObject);
			HRESULT __stdcall			QueryService(REFGUID guidService,REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject);
		public:						// ShockwaveFlashObjects::_IShockwaveFlashEvents
			HRESULT STDMETHODCALLTYPE	OnReadyStateChange(long newState);
			HRESULT STDMETHODCALLTYPE	OnProgress(long percentDone);
			HRESULT STDMETHODCALLTYPE	FSCommand(_bstr_t command,_bstr_t args);
		};
	};
};

#pragma warning(default:4275)
