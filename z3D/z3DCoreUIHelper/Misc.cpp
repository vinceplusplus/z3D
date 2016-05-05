
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "Misc.h"
#include "resource.h"
#include <shlobj.h>

#pragma comment(lib,"shell32.lib")

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#include <commdlg.h>

#include <richedit.h>

namespace z3D
{
	namespace Core
	{
		static LRESULT CALLBACK FolderChooserDummyWindowProc(          HWND hwnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam
		)
		{
			switch(uMsg)
			{
			case WM_PAINT:
				{
					if(!GetWindowLongPtr(hwnd, GWLP_USERDATA))
					{
						HWND hwnd_shbrowseforfolder = FindWindowEx(GetParent(hwnd), NULL, L"SHBrowseForFolder ShellNameSpace Control", NULL);
						if(hwnd_shbrowseforfolder)
						{
							HWND hwnd_treeview = FindWindowEx(hwnd_shbrowseforfolder, NULL, L"SysTreeView32", NULL);
							if(hwnd_treeview)
								SetFocus(hwnd_treeview);
						}
						SetWindowLongPtr(hwnd, GWLP_USERDATA, 1);
					}

					// validate to avoid constant receiving WM_PAINT messages
					ValidateRect(hwnd, NULL);
				}
				break;
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
			return 0;
		}

		class CHOOSEFOLDER_DATA
		{
		public:
			const wchar_t*		title;
			LONG_PTR			old_proc;
		};

		static int CALLBACK chooseFolder_callback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
		{ 
			switch(uMsg) 
			{ 
			case BFFM_INITIALIZED:
				{
					HWND hwnd_edit = FindWindowEx(hwnd, NULL, L"EDIT", NULL); 
					if(hwnd_edit && lpData)
					{
						SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)((CHOOSEFOLDER_DATA*)lpData)->title);
						SendMessage(hwnd, BFFM_SETEXPANDED, TRUE, (LPARAM)(((CHOOSEFOLDER_DATA*)lpData)->title));
						SetWindowText(hwnd_edit, ((CHOOSEFOLDER_DATA*)lpData)->title);
					}

					if(true)
					{
						WNDCLASSEX wc;
						wc.cbSize = sizeof(wc);
						wc.style = 0;
						wc.lpfnWndProc = FolderChooserDummyWindowProc;
						wc.cbClsExtra = 0;
						wc.cbWndExtra = 0;
						wc.hInstance = (HINSTANCE)GetModuleHandle(NULL);
						wc.hIcon = NULL;
						wc.hCursor = NULL;
						wc.hbrBackground = NULL;
						wc.lpszMenuName = NULL;
						wc.lpszClassName = L"FolderChooserDummy";
						wc.hIconSm = NULL;

						RegisterClassEx(&wc);

						CreateWindowW(L"FolderChooserDummy", L"", WS_VISIBLE | WS_CHILD, 0, 0, 1, 1, hwnd, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
					}
				}
				break;
			case BFFM_SELCHANGED:
				{
					HWND hwnd_edit = FindWindowEx(hwnd, NULL, L"EDIT", NULL);
					if(hwnd_edit)
					{
						wchar_t path[MAX_PATH];
						if(SHGetPathFromIDList((LPCITEMIDLIST)lParam, path))
							SetWindowText(hwnd_edit, path);
					}
				}
				break;
			case BFFM_IUNKNOWN:
				{
				}
				break;
			case BFFM_VALIDATEFAILED:
				{
				}
				break;
			}
			return 0;
		}
		__Z3DCOREUIHELPER_API wstring chooseFolder(const wstring& title, const wstring& start_dir, const wstring& root_dir, intptr_t hwnd_owner)
		{
			Z_STATIC_ASSERT( sizeof(intptr_t) == sizeof(HWND), WRONG_ASSUMPTION );

			CoInitialize(NULL);
			BROWSEINFO bi;
			memset(&bi, 0, sizeof(bi));
			wchar_t path[MAX_PATH];
			bi.hwndOwner = (HWND)hwnd_owner;
			bi.pszDisplayName = path; 
			if(root_dir.length())
			{
				IShellFolder* sf = NULL;
				SHGetDesktopFolder(&sf);
				if(sf)
				{
					sf->ParseDisplayName(NULL, NULL, (LPOLESTR)root_dir.c_str(), NULL, (LPITEMIDLIST*)&bi.pidlRoot, NULL);
					sf->Release();
				}
			}
			bi.lpszTitle = title.c_str();

			bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NOTRANSLATETARGETS | BIF_DONTGOBELOWDOMAIN;

			CHOOSEFOLDER_DATA choosefolder_data = {start_dir.c_str(), NULL};

			bi.lParam = (LPARAM)&choosefolder_data;
			bi.lpfn = chooseFolder_callback;
			LPITEMIDLIST pid;
			if(!(pid = SHBrowseForFolder(&bi)))
			{
				CoUninitialize();
				return L"";
			}
			memset(path, 0, sizeof(path));
			SHGetPathFromIDList(pid, path);
			IMalloc* imalloc = NULL;
			if(SHGetMalloc(&imalloc) == NOERROR)
			{
				imalloc->Free(pid);
				if(bi.pidlRoot)
					imalloc->Free((LPITEMIDLIST)bi.pidlRoot);
				imalloc->Release();
			}
			CoUninitialize();
			return path;
		} 
		__Z3DCOREUIHELPER_API wstring	chooseOpenFile(const wstring& title, const wstring& filter_display, const wstring& filter_pattern, const wstring& initial_dir, intptr_t hwnd_owner)
		{
			Z_STATIC_ASSERT( sizeof(intptr_t) == sizeof(HWND), WRONG_ASSUMPTION );

			wchar_t filename[4096];
			memset(filename, 0, sizeof(filename));

			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = (HWND)hwnd_owner;
			wchar_t filter[4096];
			memset(filter, 0, sizeof(filter));
			memcpy(filter, filter_display.c_str(), filter_display.length() * sizeof(filter[0]));
			memcpy(filter + filter_display.length() + 1, filter_pattern.c_str(), filter_pattern.length() * sizeof(filter_pattern[0]));
			const wchar_t default_pattern[] = L"All Files(*.*)\0*.*\0\0";
			memcpy(filter + filter_display.length() + filter_pattern.length() + 2, default_pattern, sizeof(default_pattern));
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = filename;
			ofn.nMaxFile = _countof(filename);
			ofn.lpstrTitle = title.c_str();
			if(initial_dir.length())
				ofn.lpstrInitialDir = initial_dir.c_str();

			ofn.Flags = OFN_NOCHANGEDIR;											// very important because the program is written to take relative file path (ie, depend on the working directory), if without this flag, the working directory may change, it may lead to larger amount of meaningless debug time, but of course always use absolute path to eliminate uncertainty

			if(!GetOpenFileName(&ofn))
				return L"";
			return ofn.lpstrFile;
		}
		__Z3DCOREUIHELPER_API wstring	chooseSaveFile(const wstring& title, const wstring& filter_display, const wstring& filter_pattern, const wstring& initial_dir, intptr_t hwnd_owner)
		{
			Z_STATIC_ASSERT( sizeof(intptr_t) == sizeof(HWND), WRONG_ASSUMPTION );

			wchar_t filename[4096];
			memset(filename, 0, sizeof(filename));

			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = (HWND)hwnd_owner;
			wchar_t filter[4096];
			memset(filter, 0, sizeof(filter));
			if(filter_display.c_str() && filter_pattern.c_str())
			{
				memcpy(filter, filter_display.c_str(), filter_display.length() * sizeof(filter[0]));
				memcpy(filter + filter_display.length() + 1, filter_pattern.c_str(), filter_pattern.length() * sizeof(filter_pattern[0]));
			}
			const wchar_t default_pattern[] = L"All Files(*.*)\0*.*\0\0";
			memcpy(filter + filter_display.length() + filter_pattern.length() + 2, default_pattern,sizeof(default_pattern));
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = filename;
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrTitle = title.c_str();
			if(initial_dir.length())
				ofn.lpstrInitialDir = initial_dir.c_str();
			ofn.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;						// very important because the program is written to take relative file path (ie, depend on the working directory), if without this flag, the working directory may change, it may lead to larger amount of meaningless debug time, but of course always use absolute path to eliminate uncertainty

			if(!GetSaveFileName(&ofn))
				return L"";

			if(ofn.nFilterIndex == 1)
			{
				size_t dot = filter_pattern.find_last_of('.');
				if(dot != wstring::npos)
				{
					wchar_t* dotstr = wcsrchr(filename, '.');
					ptrdiff_t dot2 = (dotstr == NULL) ? -1 : (dotstr - filename);
					if(dot2 < 0 || _wcsicmp(filename + dot2 + 1, filter_pattern.c_str() + dot + 1) != 0)
						if(filter_pattern.find('*', dot + 1) == wstring::npos && filter_pattern.find('?', dot + 1) == wstring::npos)
							wcscat_s(filename, filter_pattern.c_str() + dot);
				}
			}

			return ofn.lpstrFile;
		}
		static INT_PTR CALLBACK promptbox_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch(uMsg)
			{
			case WM_INITDIALOG:
#pragma warning(disable:4244)
				SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG_PTR)lParam);
#pragma warning(disable:4312)
				SetWindowText(hwndDlg, (const wchar_t*)lParam);
				wcscpy((wchar_t*)lParam, L"");
#pragma warning(default:4312)
				SetFocus(GetDlgItem(hwndDlg, IDC_PROMPT));
				SendMessage(GetDlgItem(hwndDlg, IDC_PROMPT), EM_LIMITTEXT, 4095, 0);
				SetDlgItemText(hwndDlg, IDC_PROMPT, ((wchar_t*)lParam) + 4096);
				return FALSE;
			case WM_COMMAND:
				if(LOWORD(wParam) == IDOK)
				{
#pragma warning(disable:4312)
					GetWindowText(GetDlgItem(hwndDlg, IDC_PROMPT), (wchar_t*)GetWindowLongPtr(hwndDlg, GWL_USERDATA), 4096);
#pragma warning(default:4312)
					EndDialog(hwndDlg, 1);
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
		__Z3DCOREUIHELPER_API wstring	promptBox(const wstring& title, const wstring& default_string, intptr_t hwnd_owner)
		{
			Z_STATIC_ASSERT( sizeof(intptr_t) == sizeof(HWND), WRONG_ASSUMPTION );

			wchar_t buffer[2][4096] = {L"", L""};
			if(title.length() < 4095)
				wcscpy(buffer[0], title.c_str());
			else
				wcscpy(buffer[0], L"Prompt Box");
			if(default_string.length() < 4095)
				wcscpy(buffer[1], default_string.c_str());
			if(DialogBoxParam((HINSTANCE)GetModuleHandle(L"z3DCoreUIHelper"), MAKEINTRESOURCE(IDD_PROMPTBOX), (HWND)hwnd_owner, promptbox_proc, (LPARAM)buffer))
				return buffer[0];
			return L"";
		}

		class MULTISELECT
		{
		public:
			wstring				title;
			wstring				instruction;
			wstring				item_text;
			wstring				description_text;
			vector<wstring>		items;
			vector<wstring>		descriptions;
			vector<size_t>		selected_indices;
		};

		static INT_PTR CALLBACK multi_select_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch(uMsg)
			{
			case WM_INITDIALOG:
				{
#pragma warning(suppress:4244)
					SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG_PTR)lParam);
					MULTISELECT& ms = *(MULTISELECT*)lParam;
#pragma warning(once:4312)
					SetWindowText(hwndDlg, ms.title.c_str());
					SetDlgItemText(hwndDlg, IDC_MULTI_SELECT_DESCRIPTION, ms.instruction.c_str());

					ListView_SetExtendedListViewStyle(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST), ListView_GetExtendedListViewStyle(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST)) | LVS_EX_FULLROWSELECT);
					if(true)
					{
						LVCOLUMN c;
						c.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
						c.cx = 650;
						c.pszText = (LPWSTR)ms.item_text.c_str();
						c.cchTextMax = (int)wcslen(c.pszText);
						c.iSubItem = 0;

						ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST), 0, &c);

						c.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
						c.cx = 200;
						c.pszText = (LPWSTR)ms.description_text.c_str();
						c.cchTextMax = (int)wcslen(c.pszText);
						c.iSubItem = 1;

						ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST), 1, &c);
					}
					for(size_t i = 0; i < ms.items.size(); ++i)
					{
						LVITEM item;
						item.mask = LVIF_TEXT;
						item.iItem = (int)i;
						item.iSubItem = 0;
						item.pszText = (LPWSTR)ms.items[i].c_str();
						item.cchTextMax = (int)wcslen(item.pszText);

						ListView_InsertItem(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST), &item);

						if(ms.descriptions.size() != ms.items.size())
							continue;
						item.mask = LVIF_TEXT;
						item.iItem = (int)i;
						item.iSubItem = 1;
						item.pszText = (LPWSTR)ms.descriptions[i].c_str();
						item.cchTextMax = (int)wcslen(item.pszText);

						ListView_SetItem(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST), &item);
					}
					if(ms.selected_indices.size())
					{
						for(size_t i = 0; i < ms.selected_indices.size(); ++i)
						{
							ListView_SetItemState(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST), (int)ms.selected_indices[i], LVIS_SELECTED, LVIS_SELECTED);
						}
					}
				}
				return FALSE;
			case WM_COMMAND:
				if(LOWORD(wParam) == IDOK)
				{
#pragma warning(suppress:4312)
					MULTISELECT& ms = *(MULTISELECT*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
					ms.selected_indices.clear();
					for(size_t i = 0; i < ms.items.size(); ++i)
					{
						if(ListView_GetItemState(GetDlgItem(hwndDlg, IDC_MULTI_SELECT_LIST), (int)i, LVIS_SELECTED))
							ms.selected_indices.push_back(i);
					}
					EndDialog(hwndDlg, 1);
				}
				else if(LOWORD(wParam) == IDCANCEL)
				{
					EndDialog(hwndDlg, 0);
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
		__Z3DCOREUIHELPER_API vector<size_t>	selectMulti(const wstring& title, const wstring& instruction, const wstring& item_text, const wstring& description_text, const vector<wstring>& items, const vector<wstring>& descriptions, const vector<size_t>& selected_indices, intptr_t hwnd_owner)
		{
			Z_STATIC_ASSERT( sizeof(intptr_t) == sizeof(HWND), WRONG_ASSUMPTION );

			MULTISELECT ms;
			ms.title = title;
			ms.instruction = instruction;
			ms.item_text = item_text;
			ms.description_text = description_text;
			ms.items = items;
			ms.descriptions = descriptions;
			ms.selected_indices = selected_indices;

			if(!DialogBoxParam((HINSTANCE)GetModuleHandle(L"z3DCoreUIHelper"), MAKEINTRESOURCE(IDD_MULTI_SELECT), (HWND)hwnd_owner, multi_select_proc, (LPARAM)&ms))
				return vector<size_t>();
			return ms.selected_indices;
		}
		static HWND		g_hwnd_log_window = NULL;
		static HMODULE	g_riched = NULL;
		static INT_PTR	CALLBACK log_window_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch(uMsg)
			{
			case WM_INITDIALOG:
				SendDlgItemMessage(hwndDlg, IDC_EDIT_LOG, EM_SETBKGNDCOLOR, FALSE, 0xc0c0c0);
				return FALSE;

			case WM_CLOSE:
				DestroyWindow(hwndDlg);
				break;

			case WM_DESTROY:
				g_hwnd_log_window = NULL;
				DestroyWindow(GetDlgItem(hwndDlg, IDC_EDIT_LOG));
				FreeLibrary(g_riched);
				break;

			default:
				return FALSE;
			}
			return TRUE;
		}
		__Z3DCOREUIHELPER_API void					showLogWindow(intptr_t hwnd_owner)
		{
			if(g_hwnd_log_window)
				return;

			g_riched = LoadLibrary(L"riched20.dll");

			g_hwnd_log_window = CreateDialog((HINSTANCE)GetModuleHandle(L"z3DCoreUIHelper"), MAKEINTRESOURCE(IDD_LOG_WINDOW), (HWND)hwnd_owner, log_window_proc);
		}
		__Z3DCOREUIHELPER_API void					addLogToLogWindow(const wchar_t* log)
		{
			if(!g_hwnd_log_window)
				return;

			int len;
			len = GetWindowTextLength(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG));
			SendMessage(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_SETSEL, len, len);

			SendMessageW(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_REPLACESEL, FALSE, (LPARAM)log);

			len = GetWindowTextLength(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG));
			SendMessage(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_SETSEL, len, len);

			SendMessageW(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_REPLACESEL, FALSE, (LPARAM)L"\n");

			len = GetWindowTextLength(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG));
			SendMessage(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_SETSEL, len, len);

			while(SendMessage(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_GETLINECOUNT, 0, 0) > 500 + 1)
			{
				SendMessage(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_SETSEL, 0, SendMessage(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_LINELENGTH, 0, 0) + 1);
				SendMessage(GetDlgItem(g_hwnd_log_window, IDC_EDIT_LOG), EM_REPLACESEL, 0, (LPARAM)L"");
			}
		}
	}
}

