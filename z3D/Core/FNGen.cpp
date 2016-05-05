
#include "FNGen.h"
#include "Misc.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		wcs		FNGen::gen(const wcs& prefix, int digits, const wcs suffix)
		{
			int i;

			wcs search_filename=prefix;
			for(i=0;i<digits;++i)
				search_filename+='?';
			search_filename+=suffix;

			int new_no=1;

			do
			{
				WIN32_FIND_DATA data;
				HANDLE handle;
				handle=FindFirstFile(search_filename.c_str(),&data);
				if(handle==INVALID_HANDLE_VALUE)
					break;

				wcs last_filename=data.cFileName;
				while(FindNextFile(handle,&data))
					if(last_filename<wcs(data.cFileName))
						last_filename=data.cFileName;

				FindClose(handle);

				new_no=_wtoi(last_filename.substr(prefix.length(),digits).c_str())+1;
			}while(0);

			return prefix+FORMATW((wcs(L"%0")+FORMATW(L"%d",digits)+L"d").c_str(),new_no)+suffix;
		}
	};
};


