
#include "TexCube.h"

//#include "../../FreeImage/Header.h"

typedef struct tagDDPIXELFORMAT {
	DWORD dwSize;	// size of this structure (must be 32)
	DWORD dwFlags;	// see DDPF_*
	DWORD dwFourCC;
	DWORD dwRGBBitCount;	// Total number of bits for RGB formats
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwRGBAlphaBitMask;
} DDPIXELFORMAT;

// DIRECTDRAW PIXELFORMAT FLAGS
enum {
	DDPF_ALPHAPIXELS = 0x00000001l,	// surface has alpha channel
	DDPF_ALPHA		 = 0x00000002l,	// alpha only
	DDPF_FOURCC		 = 0x00000004l,	// FOURCC available
	DDPF_RGB		 = 0x00000040l	// RGB(A) bitmap
};

typedef struct tagDDCAPS2 {
	DWORD dwCaps1;	// Zero or more of the DDSCAPS_* members
	DWORD dwCaps2;	// Zero or more of the DDSCAPS2_* members
	DWORD dwReserved[2];
} DDCAPS2;

// DIRECTDRAWSURFACE CAPABILITY FLAGS
enum {
	DDSCAPS_ALPHA	= 0x00000002l, // alpha only surface
	DDSCAPS_COMPLEX	= 0x00000008l, // complex surface structure
	DDSCAPS_TEXTURE	= 0x00001000l, // used as texture (should always be set)
	DDSCAPS_MIPMAP	= 0x00400000l  // Mipmap present
};

enum {
	DDSCAPS2_CUBEMAP			= 0x00000200L,
	DDSCAPS2_CUBEMAP_POSITIVEX	= 0x00000400L,
	DDSCAPS2_CUBEMAP_NEGATIVEX	= 0x00000800L,
	DDSCAPS2_CUBEMAP_POSITIVEY	= 0x00001000L,
	DDSCAPS2_CUBEMAP_NEGATIVEY	= 0x00002000L,
	DDSCAPS2_CUBEMAP_POSITIVEZ	= 0x00004000L,
	DDSCAPS2_CUBEMAP_NEGATIVEZ	= 0x00008000L,
	DDSCAPS2_VOLUME				= 0x00200000L
};

typedef struct tagDDSURFACEDESC2 {
	DWORD dwSize;	// size of this structure (must be 124)
	DWORD dwFlags;	// combination of the DDSD_* flags
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwPitchOrLinearSize;
	DWORD dwDepth;	// Depth of a volume texture
	DWORD dwMipMapCount;
	DWORD dwReserved1[11];
	DDPIXELFORMAT ddpfPixelFormat;
	DDCAPS2 ddsCaps;
	DWORD dwReserved2;
} DDSURFACEDESC2;

enum {
	DDSD_CAPS			= 0x00000001l,
	DDSD_HEIGHT			= 0x00000002l,
	DDSD_WIDTH			= 0x00000004l,
	DDSD_PITCH			= 0x00000008l,
	DDSD_ALPHABITDEPTH  = 0x00000080l,
	DDSD_PIXELFORMAT	= 0x00001000l,
	DDSD_MIPMAPCOUNT	= 0x00020000l,
	DDSD_LINEARSIZE		= 0x00080000l,
	DDSD_DEPTH			= 0x00800000l
};

typedef struct tagDDSHEADER {
	DWORD dwMagic;			// FOURCC: "DDS "
	DDSURFACEDESC2 surfaceDesc;
} DDSHEADER;



namespace z3D
{
	namespace Video
	{
		//bool				TexCube::loadUncompressedDDSCube(const void* src, size_t src_len, size_t& length, size_t& bpp, size_t& mips, vector<void*>& rh_pos_x, vector<void*>& rh_neg_x, vector<void*>& rh_pos_y, vector<void*>& rh_neg_y, vector<void*>& rh_pos_z, vector<void*>& rh_neg_z, bool& dword_aligned)
		//{
		//	DDSHEADER* header;
		//	if(src_len<sizeof(DDSHEADER))
		//		return false;

		//	size_t dx,dy;

		//	header=(DDSHEADER*)src;
		//	if(memcmp(&header->dwMagic,"DDS ",4))
		//		return false;
		//	if(header->surfaceDesc.dwSize != sizeof(header->surfaceDesc))
		//		return false;
		//	if(header->surfaceDesc.ddpfPixelFormat.dwSize != sizeof(header->surfaceDesc.ddpfPixelFormat))
		//		return false;
		//	if(!(header->surfaceDesc.ddpfPixelFormat.dwFlags & DDPF_RGB))
		//		return false;
		//	if(header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount != 32 && header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount != 24)
		//		return false;
		//	if(header->surfaceDesc.ddpfPixelFormat.dwBBitMask != 0xff || header->surfaceDesc.ddpfPixelFormat.dwGBitMask != 0xff00 || header->surfaceDesc.ddpfPixelFormat.dwRBitMask != 0xff0000 || (header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount == 32 && header->surfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask != 0xff000000))
		//	{
		//		Z_ASSERT( false );
		//		return false;
		//	}
		//	if(!(header->surfaceDesc.ddsCaps.dwCaps1 & DDSCAPS_TEXTURE) || !(header->surfaceDesc.ddsCaps.dwCaps1 & DDSCAPS_COMPLEX))
		//		return false;
		//	if(!(header->surfaceDesc.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP) || 
		//		!(header->surfaceDesc.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEX) ||
		//		!(header->surfaceDesc.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) ||
		//		!(header->surfaceDesc.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEY) ||
		//		!(header->surfaceDesc.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) ||
		//		!(header->surfaceDesc.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) ||
		//		!(header->surfaceDesc.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ)
		//		)
		//		return false;
		//	if(
		//		!(header->surfaceDesc.dwFlags & DDSD_CAPS) ||
		//		!(header->surfaceDesc.dwFlags & DDSD_HEIGHT) ||
		//		!(header->surfaceDesc.dwFlags & DDSD_WIDTH) ||
		//		//!(header->surfaceDesc.dwFlags & DDSD_PITCH) ||
		//		!(header->surfaceDesc.dwFlags & DDSD_PIXELFORMAT)
		//		)
		//		return false;

		//	if((header->surfaceDesc.dwFlags & DDSD_PITCH) && header->surfaceDesc.dwPitchOrLinearSize != ((header->surfaceDesc.dwWidth * ((header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount + 7) >> 3) + 3) & ~3))
		//		return false;

		//	if(!z3D::Core::IS_POW2(header->surfaceDesc.dwWidth) || !z3D::Core::IS_POW2(header->surfaceDesc.dwHeight))
		//		return false;

		//	if(header->surfaceDesc.dwWidth != header->surfaceDesc.dwHeight)
		//		return false;

		//	/*
		//	if(header->surfaceDesc.dwPitchOrLinearSize != ((header->surfaceDesc.dwWidth * (header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount>>3) + 3) & ~3))
		//		return false;
		//		*/
		//	dword_aligned = !!(header->surfaceDesc.dwFlags & DDSD_PITCH);

		//	mips = 1;
		//	if((header->surfaceDesc.ddsCaps.dwCaps1 & DDSCAPS_MIPMAP) && (header->surfaceDesc.dwFlags & DDSD_MIPMAPCOUNT))
		//		mips = header->surfaceDesc.dwMipMapCount;

		//	bpp = (int)header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount;

		//	size_t sum = 0;
		//	if(true)
		//	{
		//		size_t dx2 = header->surfaceDesc.dwWidth, dy2 = header->surfaceDesc.dwHeight;
		//		for(size_t i = 0; i < mips; ++i)
		//		{
		//			if(dword_aligned)
		//				sum+=(((dx2 * ((bpp + 7)>> 3)) + 3) & ~3) * dy2;
		//			else
		//				sum+=dx2 * ((bpp + 7) >> 3) * dy2;
		//			dx2 = max(1, dx2 >> 1);
		//			dy2 = max(1, dy2 >> 1);
		//		}
		//	}
		//	if(src_len != (int)(sizeof(DDSHEADER) + sum * 6))
		//		return false;

		//	char* data_ptr = (char*)src + sizeof(DDSHEADER);

		//	const DDSURFACEDESC2& desc = header->surfaceDesc;

		//	dx = desc.dwWidth;
		//	dy = desc.dwHeight;

		//	length = dx;

		//	for(int j = 0; j < 6; ++j)
		//	{
		//		vector<void*>* v = NULL;
		//		switch(j)
		//		{
		//		case 0:	v = &rh_pos_x; break;
		//		case 1:	v = &rh_neg_x; break;
		//		case 2:	v = &rh_pos_y; break;
		//		case 3:	v = &rh_neg_y; break;
		//		case 4:	v = &rh_neg_z; break;					// swap the two axes to make it right hand
		//		case 5:	v = &rh_pos_z; break;					// swap the two axes to make it right hand
		//		}
		//		vector<void*>& mip=*v;

		//		size_t dx2=dx;
		//		size_t dy2=dy;

		//		for(size_t i=0;i<mips;++i)
		//		{
		//			mip.push_back(data_ptr);

		//			if(dword_aligned)
		//				data_ptr += (((dx2 * ((bpp + 7) >> 3)) + 3) & ~3) * dy2;
		//			else
		//				data_ptr += dx2 * ((bpp + 7) >> 3) * dy2;

		//			dx2 = max(1, dx2 >> 1);
		//			dy2 = max(1, dy2 >> 1);
		//		}
		//	}

		//	return true;
		//}
		//void				TexCube::fill(void* fipimage, const void* unaligned)
		//{
		//	fipImage& img = *(fipImage*)fipimage;
		//	size_t h = img.getHeight();
		//	size_t w = img.getWidth();
		//	size_t line = w * ((img.getBitsPerPixel() + 7) >> 3);
		//	for(size_t i = 0; i < h; ++i)
		//		memcpy(img.accessPixels() + i * img.getScanWidth(), ((unsigned char*)unaligned) + line * i, line);
		//}
	};
};

