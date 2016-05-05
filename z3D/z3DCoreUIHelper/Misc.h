
#pragma once

#include "predecl.h"

#include "../Core/predecl.h"

namespace z3D
{
	namespace Core
	{
		__Z3DCOREUIHELPER_API wstring				chooseFolder(const wstring& title, const wstring& start_dir = L"", const wstring& root_dir = L"", intptr_t hwnd_owner = NULL);
		__Z3DCOREUIHELPER_API wstring				chooseOpenFile(const wstring& title, const wstring& filter_display, const wstring& filter_pattern, const wstring& initial_dir, intptr_t hwnd_owner = NULL);
		__Z3DCOREUIHELPER_API wstring				chooseSaveFile(const wstring& title, const wstring& filter_display, const wstring& filter_pattern, const wstring& initial_dir, intptr_t hwnd_owner = NULL);
		__Z3DCOREUIHELPER_API wstring				promptBox(const wstring& title, const wstring& default_string, intptr_t hwnd_owner = NULL);
		__Z3DCOREUIHELPER_API vector<size_t>		selectMulti(const wstring& title, const wstring& description, const wstring& item_text, const wstring& description_text, const vector<wstring>& items, const vector<wstring>& items_descriptions, const vector<size_t>& selected_indices, intptr_t hwnd_owner = NULL);

		__Z3DCOREUIHELPER_API void*					create_progress_dialog(const wstring& title, intptr_t hwnd_owner = NULL);
		__Z3DCOREUIHELPER_API void					update_progress_dialog(void* dlg, REAL percentage);
		__Z3DCOREUIHELPER_API void					delete_progress_dialog(void* dlg);

		__Z3DCOREUIHELPER_API void					showLogWindow(intptr_t hwnd_owner = NULL);
		__Z3DCOREUIHELPER_API void					addLogToLogWindow(const wchar_t* log);
	};
};

