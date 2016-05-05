
#pragma once

#include "predecl.h"

#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		Z3D_API mbs					FORMATA(const char* format, ...);
		Z3D_API wcs					FORMATW(const wchar_t* format, ...);
		Z3D_API wcs					MBSTOWCS(const mbs& s);
		Z3D_API mbs					WCSTOMBS(const wcs& s);
		Z3D_API wcs					MBSTOWCS(const mbs& s, uint32_t codepage);
		Z3D_API mbs					WCSTOMBS(const wcs& s, uint32_t codepage);
		Z3D_API wcs					UTF8TOWCS(const mbs& s);
		Z3D_API mbs					WCSTOUTF8(const wcs& s);
		Z3D_API wcs					ESCSTR(const wcs& s);
		Z3D_API mbs					REPLACE(const mbs& source, const mbs& what, const mbs& with);
		Z3D_API wcs					REPLACE(const wcs& source, const wcs& what, const wcs& with);
		Z3D_API mbs					TOLOWER(const mbs& s);
		Z3D_API wcs					TOLOWER(const wcs& s);

		template<class SIGNED_TYPE>
		void				intersect_area(SIGNED_TYPE x0, SIGNED_TYPE y0, SIGNED_TYPE dx0, SIGNED_TYPE dy0, SIGNED_TYPE x1, SIGNED_TYPE y1, SIGNED_TYPE dx1, SIGNED_TYPE dy1, SIGNED_TYPE& x, SIGNED_TYPE& y, SIGNED_TYPE& dx, SIGNED_TYPE& dy);

		template<class SIGNED_TYPE>
		void				intersect_area(SIGNED_TYPE& x0, SIGNED_TYPE& y0, SIGNED_TYPE& dx0, SIGNED_TYPE& dy0, SIGNED_TYPE x1, SIGNED_TYPE y1, SIGNED_TYPE dx1, SIGNED_TYPE dy1);

		Z3D_API vector<wcs>			find_files(const wcs& pattern);
		Z3D_API vector<wcs>			find_files(const wcs& dir, const wcs& pattern);
		Z3D_API vector<wcs>			find_directories(const wcs& pattern);
		Z3D_API vector<wcs>			find_directories(const wcs& dir, const wcs& pattern);
		Z3D_API vector<wcs>			find_files_recursive(const wcs& dir, const wcs& pattern);
		Z3D_API mbs					getTextUTF8(const void* buffer, size_t size);			// ensure getting the utf8 text whether if the buffer is a unicode or system default mb encoding text file content
		Z3D_API wcs					getTextUCS2(const void* buffer, size_t size);			// ensure getting the unicode text whether if the buffer is a unicode or system default mb encoding text file content
		Z3D_API wcs					get_env_var(const wcs& name);
		Z3D_API wcs					get_sys_err_string(unsigned long err);		// use GetLastError()

		Z3D_API wcs					esc_http_str(const wcs& str, uint32_t codepage);
		Z3D_API wcs					esc_http_str_utf8(const	wcs& str);

		Z3D_API intptr_t			create_w32_dc();
		Z3D_API intptr_t			create_w32_bitmap(int32_t dx, int32_t dy, int32_t bpp, void*& bits);
		Z3D_API bool				create_w32_offscreen(int32_t dx, int32_t dy, int32_t bpp, intptr_t& hdc, intptr_t& hbitmap, void*& bits);

		// src_b - image with prepainted black background, src_w - image with prepainted white background, tolerance - to how large the alpha will be treated completely transparent
		// colors are in format of 0xAARRGGBB, or GL_BGRA_EXT
		Z3D_API void				solve_alphablend(const uint32_t* src_b, const uint32_t* src_w, uint32_t pixels, uint32_t tolerance, uint32_t* dest, uint32_t transparent_color);

		Z3D_API mbs					gen_md5(const void* array_byte, size_t bytes);
		Z3D_API uint32_t			gen_crc(const void* array_byte, size_t bytes);
		Z3D_API bool				is_directory(const wcs& path);
		Z3D_API void				create_directory_hierarchy(const wcs& path);
		Z3D_API wcs					EXE_DIR();
	};
};

namespace z3D
{
	namespace Core
	{
		template<class SIGNED_TYPE>
		void		intersect_area(SIGNED_TYPE x0, SIGNED_TYPE y0, SIGNED_TYPE dx0, SIGNED_TYPE dy0, SIGNED_TYPE x1, SIGNED_TYPE y1, SIGNED_TYPE dx1, SIGNED_TYPE dy1, SIGNED_TYPE& x, SIGNED_TYPE& y, SIGNED_TYPE& dx, SIGNED_TYPE& dy)
		{
			Z_STATIC_ASSERT( static_cast<SIGNED_TYPE>(-1) < 0, NOT_SIGNED_TYPE );

			x = max(x0, x1);
			y = max(y0, y1);
			dx = max(min(x0 + dx0, x1 + dx1) - x, 0);
			dy = max(min(y0 + dy0, y1 + dy1) - y, 0);
		}
		template<class SIGNED_TYPE>
		void		intersect_area(SIGNED_TYPE& x0, SIGNED_TYPE& y0, SIGNED_TYPE& dx0, SIGNED_TYPE& dy0, SIGNED_TYPE x1, SIGNED_TYPE y1, SIGNED_TYPE dx1, SIGNED_TYPE dy1)
		{
			Z_STATIC_ASSERT( static_cast<SIGNED_TYPE>(-1) < 0, NOT_SIGNED_TYPE );

			SIGNED_TYPE x, y, dx, dy;
			intersect_area(x0,y0,dx0,dy0,x1,y1,dx1,dy1,x,y,dx,dy);
			x0 = x;
			y0 = y;
			dx0 = dx;
			dy0 = dy;
		}
	};
};

