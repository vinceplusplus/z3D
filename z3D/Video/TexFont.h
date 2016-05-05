
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Video
	{
		class FTLibrary;
		class FTFaceInfo;
		class FontCache;
		class FontFaceLoadInfo;
		class FaceInst;
		class CharCache;

		class DrawTextOperation
		{
		private:
			TexFont*				_font;
		public:
			struct P
			{
				Vec4					color;
				Vec4					shadow_color;
				Vec2					shadow_offset;
				Vec2					ext;
				struct FLAGS
				{
					union
					{
						size_t					_FLAGS;
						struct
						{
							uint8_t					shadow : 1;
							uint8_t					line_wrap : 1;
							uint8_t					halign : 2;					// 0 - left, 1 - center, 2 - right
							uint8_t					valign : 2;					// 0 - top, 1 - middle, 2 - bottom
						};
					};
				}						flags;
			}						p;
		private:
			DrawTextOperation(TexFont* font);
		public:
			DrawTextOperation(const DrawTextOperation& other);
		public:
			DrawTextOperation&		operator=(const DrawTextOperation& right);
		public:
			DrawTextOperation&		ext(const Vec2& extent);
			DrawTextOperation&		ext(int32_t dx, int32_t dy);
		public:
			DrawTextOperation&		color(const Vec4& c);
			DrawTextOperation&		color(REAL r, REAL g, REAL b, REAL a = 1.0f);
		public:
			DrawTextOperation&		shadow();
		public:
			DrawTextOperation&		shadow_color(const Vec4& c);
			DrawTextOperation&		shadow_color(REAL r, REAL g, REAL b, REAL a = 1.0f);
		public:
			DrawTextOperation&		shadow_offset(const Vec2& o);
			DrawTextOperation&		shadow_offset(int32_t offset_x, int32_t offset_y);
		public:
			DrawTextOperation&		line_wrap();
		public:
			DrawTextOperation&		left();
			DrawTextOperation&		center();
			DrawTextOperation&		right();
		public:
			DrawTextOperation&		top();
			DrawTextOperation&		middle();
			DrawTextOperation&		bottom();
		public:
			void					end(const wcs& text, const Vec2& pos, rsShade* shade_state = NULL);
			void					end(const wcs& text, int32_t x, int32_t y, rsShade* shade_state = NULL);
			void					end(const wcs& text, const Vec3& pos, rsShade* shade_state = NULL);
		public:
			friend class TexFont;
		};

		class TexFont: public RefBase
		{
		protected:
			static vector< SPtr<FontCache> >			_array_font_cache;
			static map<mbs, SPtr<FontFaceLoadInfo> >	_map_name_load_info;
		protected:
			vector< SPtr<FaceInst> >					_array_face;
		protected:
			map<wchar_t,unsigned short>					_char_xadv;
		protected:
			typedef map<wchar_t, SPtr<CharCache>, less<wchar_t>, in_place_allocator<pair<const wchar_t, SPtr<CharCache>>>>
														MAP_CHAR_CACHE;														// TODO: improve so that the performance could even be better
			typedef map<size_t, wchar_t, less<size_t>, in_place_allocator<pair<const size_t, wchar_t>>>
														MAP_ID_CHAR;														// TODO: improve so that the performance could even be better
			MAP_CHAR_CACHE								_char_cache;
			MAP_ID_CHAR									_map_id_char;
			size_t										_next_id;
		protected:
			size_t										_cache_limit;
		private:
			void*										_cur_face;
		protected:
			SPtr<TexBase>								_tex_decal;
			Mat3										_mat_decal;
		private:
			size_t										_height;
		protected:
			TexFont();
			~TexFont();
		private:
			static void							rebuildLoadInfo();
		public:
			static bool							addFontCache(const wcs& filepath);
			static bool							removeFontCache(const wcs& filepath);
		public:
			static SPtr<TexFont>				load(const mbs& family_name, size_t height);
		public:
			bool								insertFace(const mbs& family_name, size_t index);
		private:
			bool								insertFace(const mbs& family_name, size_t index, size_t height);
		public:
			void								setDecal(const SPtr<TexBase>& decal);
			void								setDecalUVTransform(const Mat3& mat);
			bool								useDecal() const;
		public:
			int									supports(wchar_t ch) const;				// -1 not found, otherwise 0-based index to array of face instances
		public:
			void								cacheChar(wchar_t ch);
		public:
			void								cacheAdvance(wchar_t ch);
		private:
			bool								__render(wchar_t ch);
			void								cacheAdvance(wchar_t ch, bool render);
		public:
			//unsigned long						getLineSpacing() const;
		public:
			void								draw(const wcs& text, const Vec3& pos, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* shade_state = NULL);
		public:
			size_t								getHeight() const;
			int									stringWidth(const wcs& s);
			vector<wcs>							line_wrap(const wcs& str, int line_width);
			int									getCharAdvance(wchar_t ch);
		public:
			void								getRespectingShadeState(rsShade& rs, bool reset_shade_state) const;
		public:
			DrawTextOperation					begin();
		private:
			void								__draw(const wcs& text, const Vec3& pos, rsShade* shade_state, DrawTextOperation& dto);
			void								__draw_single_line(const wcs& text, const Vec3& pos, rsShade* shade_state, DrawTextOperation& dto);
		public:
			friend class DrawTextOperation;
		};
	};
};

