
#include "Misc.h"
#include "resource.h"
#include <shlobj.h>

#pragma comment(lib,"shell32.lib")

#include "../Core/Misc.h"

namespace z3D
{
	namespace Core
	{
		typedef struct
		{
			wstring		title;
			HANDLE		evtCreated;
			HANDLE		evtDeleteRequest;
			HANDLE		hThread;
			HWND		hWnd;
			HWND		hWndOwner;
		} PROGDLG;

		INT_PTR CALLBACK progress_dialog_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
			case WM_INITDIALOG:
				SendMessage(GetDlgItem(hwndDlg, IDC_DLGPROG_PROGRESS), PBM_SETRANGE, 0, MAKELPARAM(0, 100));
				return TRUE;
			}
			return FALSE;
		}

		DWORD WINAPI progress_dialog_thread(LPVOID lpParameter)
		{
			PROGDLG& progdlg = *(PROGDLG*)lpParameter;

			progdlg.hWnd = CreateDialog((HINSTANCE)GetModuleHandle(L"z3DCoreUIHelper"), MAKEINTRESOURCE(IDD_DLGPROG), progdlg.hWndOwner, progress_dialog_proc);
			SetWindowText(progdlg.hWnd, progdlg.title.c_str());
			ShowWindow(progdlg.hWnd, SW_SHOW);
			SetEvent(progdlg.evtCreated);

			while(true)
			{
				MSG msg;
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				if(WaitForSingleObject(progdlg.evtDeleteRequest, 0) == WAIT_OBJECT_0)
					break;
				Sleep(10);
			}
			DestroyWindow(progdlg.hWnd);
			return 0;
		}

		__Z3DCOREUIHELPER_API void*		create_progress_dialog(const wstring& title, intptr_t hwnd_owner)
		{
			Z_STATIC_ASSERT( sizeof(intptr_t) == sizeof(HWND), WRONG_ASSUMPTION );

			HANDLE evtCreated = NULL;
			HANDLE evtDeleteRequest = NULL;
			HANDLE hThread = NULL;

			if(!(evtCreated = CreateEvent(NULL, FALSE, FALSE, NULL)))
				return NULL;
			if(!(evtDeleteRequest = CreateEvent(NULL, FALSE, FALSE, NULL)))
			{
				CloseHandle(evtCreated);
				return NULL;
			}
			DWORD id;
			PROGDLG* progdlg = new PROGDLG;
			if(!(hThread = CreateThread(NULL, 0, progress_dialog_thread, progdlg, CREATE_SUSPENDED, &id)))
			{
				CloseHandle(evtCreated);
				CloseHandle(evtDeleteRequest);
				delete progdlg;
				return NULL;
			}

			progdlg->title = title;
			progdlg->evtCreated = evtCreated;
			progdlg->evtDeleteRequest = evtDeleteRequest;
			progdlg->hThread = hThread;
			progdlg->hWndOwner = (HWND)hwnd_owner;

			ResumeThread(hThread);

			while(WaitForSingleObject(evtCreated, 0) != WAIT_OBJECT_0)
				Sleep(10);

			return progdlg;
		}

		__Z3DCOREUIHELPER_API void		update_progress_dialog(void* dlg, REAL percentage)
		{
			if(!dlg)
				return;

			PROGDLG& progdlg = *(PROGDLG*)dlg;

			HWND hwnd = GetDlgItem(progdlg.hWnd, IDC_DLGPROG_PROGRESS);
			SendMessage(hwnd, PBM_SETPOS, (WPARAM)percentage, 0);
			wchar_t buffer[1024];
			swprintf(buffer, _countof(buffer), L"%.1f", percentage);
			wcscat_s(buffer, L"% ");
			wcscat_s(buffer, progdlg.title.c_str());
			SetWindowText(progdlg.hWnd, buffer);
		}

		__Z3DCOREUIHELPER_API void		delete_progress_dialog(void* dlg)
		{
			if(!dlg)
				return;
			PROGDLG& progdlg = *(PROGDLG*)dlg;

			SetEvent(progdlg.evtDeleteRequest);
			while(WaitForSingleObject(progdlg.hThread, 0) != WAIT_OBJECT_0)
				Sleep(10);

			CloseHandle(progdlg.evtCreated);
			CloseHandle(progdlg.evtDeleteRequest);
			CloseHandle(progdlg.hThread);

			delete (PROGDLG*)dlg;
		}
	}
}

