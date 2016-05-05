
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "Misc.h"

#include "mutex_fast.h"
#include "scoped_lock.h"

#include "Constants.h"

#include "../../libmd5/md5.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		//void		intersect_area(int x0,int y0,int dx0,int dy0,int x1,int y1,int dx1,int dy1,int& x,int& y,int& dx,int& dy)
		//{
		//	x=max(x0,x1);
		//	y=max(y0,y1);
		//	dx=max(min(x0+dx0,x1+dx1)-x,0);
		//	dy=max(min(y0+dy0,y1+dy1)-y,0);
		//}
		//void		intersect_area(int& x0,int& y0,int& dx0,int& dy0,int x1,int y1,int dx1,int dy1)
		//{
		//	int x,y,dx,dy;
		//	intersect_area(x0,y0,dx0,dy0,x1,y1,dx1,dy1,x,y,dx,dy);
		//	x0=x;
		//	y0=y;
		//	dx0=dx;
		//	dy0=dy;
		//}
		vector<wcs> find_files(const wcs& pattern)
		{
			vector<wcs> array_files;

			WIN32_FIND_DATA data;
			HANDLE handle;
			handle=FindFirstFile(pattern.c_str(),&data);
			if(handle==INVALID_HANDLE_VALUE)
				return array_files;

			do
			{
				if(!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					array_files.push_back(data.cFileName);
			}while(FindNextFile(handle,&data));

			FindClose(handle);

			return array_files;
		}
		vector<wcs> find_files(const wcs& dir, const wcs& pattern)
		{
			vector<wcs> array_files;

			WIN32_FIND_DATA data;
			HANDLE handle;
			handle=FindFirstFile((dir+L"/"+pattern).c_str(),&data);
			if(handle==INVALID_HANDLE_VALUE)
				return array_files;

			do
			{
				if(!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					array_files.push_back(dir + L"/" + data.cFileName);
			}while(FindNextFile(handle, &data));

			FindClose(handle);

			return array_files;
		}
		vector<wcs> find_directories(const wcs& pattern)
		{
			vector<wcs> array_files;

			WIN32_FIND_DATA data;
			HANDLE handle;
			handle=FindFirstFile(pattern.c_str(),&data);
			if(handle==INVALID_HANDLE_VALUE)
				return array_files;

			do
			{
				if(wcscmp(data.cFileName,L".")!=0 && wcscmp(data.cFileName,L"..")!=0 && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					array_files.push_back(data.cFileName);
			}while(FindNextFile(handle,&data));

			FindClose(handle);

			return array_files;
		}
		vector<wcs> find_directories(const wcs& dir,const wcs& pattern)
		{
			vector<wcs> array_files;

			WIN32_FIND_DATA data;
			HANDLE handle;
			handle=FindFirstFile((dir+L"/"+pattern).c_str(),&data);
			if(handle==INVALID_HANDLE_VALUE)
				return array_files;

			do
			{
				if(wcscmp(data.cFileName,L".")!=0 && wcscmp(data.cFileName,L"..")!=0 && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					array_files.push_back(dir+L"/"+data.cFileName);
			}while(FindNextFile(handle,&data));

			FindClose(handle);

			return array_files;
		}

		vector<wcs> find_files_recursive(const wcs& dir, const wcs& pattern)
		{
			vector<wcs> array_files = find_files(dir, pattern);
			vector<wcs> array_directories = find_directories(dir + L"/*.*");
			for(size_t i = 0; i < array_directories.size(); ++i)
			{
				vector<wcs> files = find_files_recursive(dir + L"/" + array_directories[i], pattern);
				array_files.insert(array_files.end(), files.begin(), files.end());
			}
			return array_files;
		}

		mbs			getTextUTF8(const void* buffer, size_t size)
		{
			if(size >= 2)
				if(*(unsigned short*)buffer == UNICODE_BYTE_ORDER_MARK)
					return WCSTOMBS(wcs(((const wchar_t*)buffer) + 1, (size_t)((size - 2) >> 1)), CP_UTF8);
			return WCSTOMBS(MBSTOWCS(mbs((char*)buffer, (size_t)size)), CP_UTF8);
		}
		wcs			getTextUCS2(const void* buffer, size_t size)
		{
			if(size >= 2)
				if(*(unsigned short*)buffer == UNICODE_BYTE_ORDER_MARK)
					return wcs(((const wchar_t*)buffer) + 1, (size_t)((size - 2) >> 1));
			return MBSTOWCS(mbs((char*)buffer, (size_t)size));
		}
		wcs		get_env_var(const wcs& name)
		{
			wchar_t buffer[4096];
			buffer[GetEnvironmentVariable(name.c_str(),buffer,4095)]=0;
			return buffer;
		}
		wcs	get_sys_err_string(unsigned long err)
		{
			LPVOID lpMsgBuf;
			if (FormatMessageW( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				err,
				//MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), // Default language
				0x0409,
				(LPWSTR) &lpMsgBuf,
				0,
				NULL ))
			{
				wcs s = (wchar_t*)lpMsgBuf;
				LocalFree(lpMsgBuf);

				return s;
			}
			return L"";
		}
		wcs	esc_http_str(const wcs& str, uint32_t codepage)
		{
			wcs s;
			int i,j;
			for(i=0;i<(int)str.size();++i)
			{
				wchar_t ch=str[i];
				if(
					(ch>='A'&&ch<='Z')||
					(ch>='a'&&ch<='z')||
					(ch>='0'&&ch<='9')
					)
					s+=str[i];
				else
				{
					mbs c=WCSTOMBS(wcs(L"")+str[i],codepage);
					for(j=0;j<(int)c.size();++j)
						s+=FORMATW(L"%%%02X",(unsigned char)c[j]);
				}
			}
			return s;
		}

		wcs	esc_http_str_utf8(const wcs& str)
		{
			return esc_http_str(str,CP_UTF8);
		}

		intptr_t		create_w32_dc()
		{
			return (intptr_t)CreateCompatibleDC(NULL);
		}

		intptr_t	create_w32_bitmap(int32_t dx, int32_t dy, int32_t bpp, void*& bits)
		{
			HDC deskdc=GetDC(GetDesktopWindow());
			if(!deskdc)
				return NULL;

			BITMAPINFO bi;

			bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
			bi.bmiHeader.biWidth=dx;
			bi.bmiHeader.biHeight=dy;
			bi.bmiHeader.biPlanes=1;
			bi.bmiHeader.biBitCount=bpp;
			bi.bmiHeader.biCompression=BI_RGB;
			bi.bmiHeader.biSizeImage=0;
			bi.bmiHeader.biXPelsPerMeter=3000;
			bi.bmiHeader.biYPelsPerMeter=3000;
			bi.bmiHeader.biClrUsed=0;
			bi.bmiHeader.biClrImportant=0;

			HBITMAP hbitmap=CreateDIBSection(deskdc,&bi,DIB_RGB_COLORS,&bits,NULL,0);
			ReleaseDC(GetDesktopWindow(),deskdc);

			return (intptr_t)hbitmap;
		}

		bool		create_w32_offscreen(int32_t dx, int32_t dy, int32_t bpp, intptr_t& hdc, intptr_t& hbitmap, void*& bits)
		{
			hdc=NULL;
			hbitmap=NULL;
			bits=NULL;

			bool succ=false;
			do
			{
				hdc=create_w32_dc();
				if(!hdc)
					break;

				hbitmap=create_w32_bitmap(dx,dy,bpp,bits);
				if(!hbitmap)
					break;

				SelectObject((HDC)hdc,(HBITMAP)hbitmap);

				succ=true;
			}while(false);

			if(!succ)
			{
				if(hdc)
					DeleteDC((HDC)hdc);
				if(hbitmap)
					DeleteObject((HBITMAP)hbitmap);
			}

			return succ;
		}
		void		solve_alphablend(const uint32_t* src_b, const uint32_t* src_w, uint32_t pixels, uint32_t tolerance, uint32_t* dest, uint32_t transparent_color)
		{
			typedef struct
			{
				unsigned long b:8;
				unsigned long g:8;
				unsigned long r:8;
				unsigned long a:8;
			} Z3DBGRA;

			Z3DBGRA* s0=(Z3DBGRA*)src_b;
			Z3DBGRA* s1=(Z3DBGRA*)src_w;

			while(pixels)
			{
				Z3DBGRA& c0=*s0;
				Z3DBGRA& c1=*s1;
				Z3DBGRA& d=*(Z3DBGRA*)dest;

				long a=0;
				a+=255+c0.b-c1.b;
				a+=255+c0.g-c1.g;
				a+=255+c0.r-c1.r;
				a/=3;
				a=min(max(a,0L),255L);

				if((unsigned long)a>tolerance)
				{
					d.b=c0.b*255/a;
					d.g=c0.g*255/a;
					d.r=c0.r*255/a;
					d.a=a;
				}
				else
					d=*(Z3DBGRA*)&transparent_color;

				++s0;
				++s1;
				++dest;
				--pixels;
			}
		}
		mbs					gen_md5(const void* array_byte, size_t bytes)
		{
			mbs md5;

			md5_state_t state;
			md5_byte_t digest[16];

			md5_init(&state);
			md5_append(&state, (const md5_byte_t*)array_byte, (int)bytes);
			md5_finish(&state, digest);

			for(int i = 0; i < 16; ++i)
				md5 += FORMATA("%02x", (int)digest[i]);
			return md5;
		}
		bool				is_directory(const wcs& path)
		{
			wcs path2=path;
			while(path2.length() && (path2[path2.length()-1]=='\\' || path2[path2.length()-1]=='/'))
				path2.erase(path2.length()-1);
			WIN32_FIND_DATA data;
			HANDLE handle=FindFirstFile(path2.c_str(),&data);
			if(handle == INVALID_HANDLE_VALUE)
				return false;
			FindClose(handle);
			return !!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}
		void		create_directory_hierarchy(const wcs& path)
		{
			wcs dir=REPLACE(path,L"/",L"\\");
			if(is_directory(path))
				return;
			int pos=-1;
			while((pos=(int)dir.find('\\',pos+1))>=0)
				CreateDirectory(dir.substr(0,pos).c_str(),NULL);
			CreateDirectory(dir.c_str(),NULL);
		}
		wcs				EXE_DIR()
		{
			wchar_t exe_dir[4096];
			GetModuleFileName(NULL,exe_dir,sizeof(exe_dir)/sizeof(wchar_t));
			wchar_t* back_slash=wcsrchr(exe_dir,'\\');
			if(back_slash)
				*back_slash=0;
			return exe_dir;
		}
	}
}

