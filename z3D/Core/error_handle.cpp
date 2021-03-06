
#define WIN32_LEAN_AND_MEAN

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#undef min
#undef max

#include "Critical/Header.h"

void __z_assert_failed(const wchar_t* expr, const wchar_t* file, unsigned int line)
{
	z3D::Core::syslog_basic.flush();

	// bad...
#ifndef NDEBUG
#	if _MSC_VER == 1400 || _MSC_VER == 1500
		_wassert(expr, file, line);
#	else
#		error unsupported platform
#	endif
#endif
}

#include "../../HiddenRes/Resource.h"

INT_PTR CALLBACK verify_failed_proc(
	HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hwndDlg, IDC_TXT_FAIL_MESSAGE, (LPCWSTR)lParam);
		return FALSE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hwndDlg, 0);
			return TRUE;
		case IDC_BTN_DEBUG:
			EndDialog(hwndDlg, 1);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

#pragma intrinsic(__debugbreak)

void __z_verify_failed(const wchar_t* expr, const wchar_t* file, unsigned int line)
{
	z3D::Core::syslog_basic.flush();

	wchar_t buffer[8192];
	swprintf_s(buffer, _countof(buffer), L"Verification Failed !!!\r\n\r\nFile: %s\r\nLine: %d\r\n\r\nExpr: %s\r\n\r\n", file, line, expr);

	if(true)
	{
		uint8_t dlgtemp[] = {0xcc, 0x08, 0xc8, 0x90, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x01, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x5f, 0x00, 0x56, 0x00, 0x45, 0x00, 0x52, 0x00, 0x49, 0x00, 0x46, 0x00, 0x59, 0x00, 0x28, 0x00, 0x29, 0x00, 0x20, 0x00, 0x66, 0x00, 0x61, 0x00, 0x69, 0x00, 0x6c, 0x00, 0x65, 0x00, 0x64, 0x00, 0x00, 0x00, 0x08, 0x00, 0x4d, 0x00, 0x73, 0x00, 0x20, 0x00, 0x53, 0x00, 0x68, 0x00, 0x65, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x20, 0x00, 0x44, 0x00, 0x6c, 0x00, 0x67, 0x00, 0x20, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x82, 0x00, 0x32, 0x00, 0x0e, 0x00, 0x01, 0x00, 0xff, 0xff, 0x80, 0x00, 0x45, 0x00, 0x78, 0x00, 0x69, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x82, 0x00, 0x32, 0x00, 0x0e, 0x00, 0xec, 0x03, 0xff, 0xff, 0x80, 0x00, 0x44, 0x00, 0x65, 0x00, 0x62, 0x00, 0x75, 0x00, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x08, 0x81, 0x50, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x2a, 0x01, 0x78, 0x00, 0xed, 0x03, 0xff, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00};
		INT_PTR ret = DialogBoxIndirectParam(GetModuleHandle(0), (DLGTEMPLATE*)dlgtemp, NULL, verify_failed_proc, (LPARAM)buffer);

		if(ret == 1)
		{
			__debugbreak();
		}
	}
	else
	{
		MessageBoxW(NULL, buffer, L"FATAL ERROR", S_OK);
	}

	ExitProcess(0);
}

