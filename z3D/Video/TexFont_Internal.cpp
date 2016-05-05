
#include "TexFont_Internal.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_BITMAP_H

namespace z3D
{
	namespace Video
	{
		FTLibrary::FTLibrary(void* library):_library(library) {}
		FTLibrary::~FTLibrary() {if(_instance==this) _instance=NULL; FT_Done_FreeType((FT_Library)_library);}
		SPtr<FTLibrary>	FTLibrary::load()
		{
			if(!_instance)
			{
				FT_Library lib=NULL;
				FT_Init_FreeType(&lib);
				if(!lib)
					return NULL;
				_instance=new FTLibrary(lib);
			}
			return _instance;
		}
		void*					FTLibrary::library() const {return _library;}

		FontCache::FontCache(const wcs& filepath, const SMem& sm): ResBase(filepath), _filemem(sm) {}
		FontCache::~FontCache() {}
		SPtr<FontCache>		FontCache::load(const wcs& filepath)
		{
			SPtr<FontCache> cache = _res_mgr.find<FontCache>(filepath);
			if(cache)
				return cache;
			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			SPtr<FTLibrary> lib = FTLibrary::load();
			if(!lib)
				return NULL;

			FT_Face face = NULL;
			FT_New_Memory_Face((FT_Library)lib->library(), (const FT_Byte*)sm.get(), (FT_Long)sm.size(), 0, &face);
			if(!face)
				return NULL;

			unsigned long ch;
			unsigned int g_index = 0;
			int num_faces = face->num_faces;
			FT_Done_Face(face);

			cache = new FontCache(filepath, sm);
			cache->_lib = lib;
			for(int i = 0; i < num_faces; ++i)
			{
				face = NULL;
				FT_New_Memory_Face((FT_Library)lib->library(), (const FT_Byte*)sm.get(), (FT_Long)sm.size(), i, &face);
				if(!face)
					continue;
				FTFaceInfo* f;
				cache->_array_face.push_back(f = new FTFaceInfo());
				f->index = i;
				f->name = face->family_name;
				for(ch = FT_Get_First_Char(face, &g_index); ch != 0; ch = FT_Get_Next_Char(face, ch, &g_index))
				{
					f->map_char_glyph.resize(max((unsigned long)f->map_char_glyph.size(), ch + 1));
					f->map_char_glyph[ch] = g_index;
					g_index = 0;
				}
				FT_Done_Face(face);
			}

			_res_mgr.add(cache.get());
			return cache;
		}
		const SMem&							FontCache::filemem() const
		{
			return _filemem;
		}

		const vector< SPtr<FTFaceInfo> >&	FontCache::array_face() const {return _array_face;}

		FaceInst::~FaceInst()
		{
			FT_Done_Face((FT_Face)face);
		}
	};
};

