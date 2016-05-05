
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Video
	{
		class FTLibrary: public RefBase
		{
		protected:
			static FTLibrary*		_instance;
		protected:
			void*					_library;
		protected:
			FTLibrary(void* library);
			~FTLibrary();
		public:
			static SPtr<FTLibrary>	load();
		public:
			void*					library() const;
		};

		class FTFaceInfo: public RefBase
		{
		public:
			int								index;
			mbs								name;
			vector<int>						map_char_glyph;
		};

		class FontCache: public ResBase
		{
		protected:
			static ResMgr						_res_mgr;
		protected:
			SPtr<FTLibrary>						_lib;
			SMem								_filemem;
			vector< SPtr<FTFaceInfo> >			_array_face;
		protected:
			FontCache(const wcs& filepath, const SMem& sm);
			~FontCache();
		public:
			static SPtr<FontCache>				load(const wcs& filepath);
		public:
			const SMem&							filemem() const;
		public:
			const vector< SPtr<FTFaceInfo> >&	array_face() const;
		};

		class FontFaceLoadInfo: public RefBase
		{
		public:
			mbs									name;
			SPtr<FontCache>						cache;
			int									index;
		};

		class FaceInst: public RefBase
		{
		public:
			SPtr<FTLibrary>						lib;
		public:
			SPtr<FontCache>						font_cache;
			SPtr<FTFaceInfo>					info;
			void*								face;
		public:
			~FaceInst();
		};

		class CharCache: public RefBase
		{
		public:
			SPtr<Tex2D>							tex;
			Vec2								uv;
		public:
			short								left;
			short								top;
			unsigned short						dx;
			unsigned short						dy;
		public:
			size_t								id;
		};
	};
};

