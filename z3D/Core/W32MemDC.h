
#pragma once

#include "predecl.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		// not designed for derivation
		class W32MemDC
		{
		private:
			HDC				_dc;
			HBITMAP			_bitmap;
			void*			_bits;
		private:
			int				_dx;
			int				_dy;
			int				_bpp;
		public:
			W32MemDC();
			~W32MemDC();
		public:
			bool			create(int dx,int dy,int bpp);				// will call destroy before create/recreate
			void			destroy();
		public:
			HDC				getDC() const;
			HBITMAP			getBitmap() const;
			void*			getBits() const;
		public:
			int				getDX() const;
			int				getDY() const;
			int				getBPP() const;
		private:
			W32MemDC(const W32MemDC&);
			W32MemDC&		operator=(const W32MemDC&);
		};
	};
};

