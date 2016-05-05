
#include "W32MemDC.h"
#include "Misc.h"

namespace z3D
{
	namespace Core
	{
		W32MemDC::W32MemDC()
		{
			_dc=NULL;
			_bitmap=NULL;
			_bits=NULL;

			_dx=0;
			_dy=0;
			_bpp=0;
		}
		W32MemDC::~W32MemDC()
		{
			destroy();
		}
		bool			W32MemDC::create(int dx,int dy,int bpp)
		{
			destroy();

			if(!create_w32_offscreen(dx,dy,bpp,(intptr_t&)_dc,(intptr_t&)_bitmap,_bits))
				return false;

			_dx=dx;
			_dy=dy;
			_bpp=bpp;

			return true;
		}
		void			W32MemDC::destroy()
		{
			if(_dc)
				DeleteDC(_dc);
			if(_bitmap)
				DeleteObject(_bitmap);

			_dc=NULL;
			_bitmap=NULL;
			_bits=NULL;

			_dx=0;
			_dy=0;
			_bpp=0;
		}
		HDC			W32MemDC::getDC() const {return _dc;}
		HBITMAP		W32MemDC::getBitmap() const {return _bitmap;}
		void*		W32MemDC::getBits() const {return _bits;}
		int			W32MemDC::getDX() const {return _dx;}
		int			W32MemDC::getDY() const {return _dy;}
		int			W32MemDC::getBPP() const {return _bpp;}
		W32MemDC::W32MemDC(const W32MemDC&) {}
		W32MemDC&		W32MemDC::operator=(const W32MemDC&) {return *this;}
	};
};

