
#include "TexFont.h"

#include "../Core/allocators_inl.h"

#include "Tex2D.h"

#include "Jotter.h"

#include "rsShade.h"

#include "Shader.h"

#include "ShaderUniforms.h"
#include "ShaderUniforms2.h"

#include "TexFont_Internal.h"

#include "../../FreeImage/Header.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_BITMAP_H

namespace z3D
{
	namespace Video
	{
		DrawTextOperation::DrawTextOperation(TexFont* font)
		{
			_font = font;

			p.color = Vec4(1, 1, 1, 1);
			p.shadow_color = Vec4(0, 0, 0, 1);
			p.shadow_offset = Vec2(1, 1);

			p.flags._FLAGS = 0;
		}
		DrawTextOperation::DrawTextOperation(const DrawTextOperation& other)
		{
			memcpy(this, &other, sizeof(*this));
		}
		DrawTextOperation&		DrawTextOperation::operator=(const DrawTextOperation& right)
		{
			memcpy(this, &right, sizeof(*this));
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::ext(const Vec2& extent)
		{
			p.ext = extent;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::ext(int32_t dx, int32_t dy)
		{
			p.ext = Vec2((REAL)dx, (REAL)dy);
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::color(const Vec4& c)
		{
			p.color = c;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::color(REAL r, REAL g, REAL b, REAL a)
		{
			p.color = Vec4(r, g, b, a);
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::shadow()
		{
			p.flags.shadow = true;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::shadow_color(const Vec4& c)
		{
			p.shadow_color = c;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::shadow_color(REAL r, REAL g, REAL b, REAL a)
		{
			p.shadow_color = Vec4(r, g, b, a);
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::shadow_offset(const Vec2& o)
		{
			p.shadow_offset = o;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::shadow_offset(int32_t offset_x, int32_t offset_y)
		{
			p.shadow_offset = Vec2((REAL)offset_x, (REAL)offset_y);
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::line_wrap()
		{
			p.flags.line_wrap = true;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::left()
		{
			p.flags.halign = 0;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::center()
		{
			p.flags.halign = 1;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::right()
		{
			p.flags.halign = 2;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::top()
		{
			p.flags.valign = 0;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::middle()
		{
			p.flags.valign = 1;
			return *this;
		}
		DrawTextOperation&		DrawTextOperation::bottom()
		{
			p.flags.valign = 2;
			return *this;
		}
		void					DrawTextOperation::end(const wcs& text, const Vec2& pos, rsShade* shade_state)
		{
			_font->__draw(text, pos, shade_state, *this);
		}
		void					DrawTextOperation::end(const wcs& text, int32_t x, int32_t y, rsShade* shade_state)
		{
			_font->__draw(text, Vec2((REAL)x, (REAL)y), shade_state, *this);
		}
		void					DrawTextOperation::end(const wcs& text, const Vec3& pos, rsShade* shade_state)
		{
			_font->__draw(text, pos, shade_state, *this);
		}

		static void			ftbitmap_to_fip(FT_Bitmap* bitmap,fipImage& fip)
		{
			fip.setSize(FIT_BITMAP,bitmap->width,bitmap->rows,8);

			unsigned char* dst=fip.accessPixels()+(bitmap->pitch>0?(fip.getScanWidth()*(fip.getHeight()-1)):0);
			int dst_inc=(int)fip.getScanWidth();
			dst_inc=(bitmap->pitch>0?-dst_inc:dst_inc);

			unsigned char* src=bitmap->buffer;
			unsigned char* src_end=src+bitmap->rows*bitmap->pitch;

			int pitch=bitmap->pitch;

			if(bitmap->pixel_mode == FT_PIXEL_MODE_MONO)
			{
				int width=bitmap->width;

				while(src!=src_end)
				{
					unsigned char* p=src;

					unsigned char* p2=dst;
					unsigned char* p2_end=p2+width;

					int bits=0;
					unsigned char b=*p;
					while(p2!=p2_end)
					{
						*p2=(b&0x80)?0xff:0;
						++p2;
						b<<=1;
						++bits;
						if(bits==8)
						{
							b=*(++p);
							bits=0;
						}
					}

					src+=pitch;
					dst+=dst_inc;
				}
			}
			else if(bitmap->pixel_mode == FT_PIXEL_MODE_GRAY)
			{
				int width=bitmap->width;

				while(src!=src_end)
				{
					memcpy(dst,src,width);
					src+=pitch;
					dst+=dst_inc;
				}
			}
			/* more pixel mode formats to handle to guarantee no failure */
		}

		TexFont::TexFont()
		{
			_cache_limit = 10;
			_next_id = 0;

			_cur_face = NULL;
		}
		TexFont::~TexFont()
		{
			_char_cache.clear();
			_array_face.clear();
		}
		void							TexFont::rebuildLoadInfo()
		{
			_map_name_load_info.clear();
			for(int i=0;i<(int)_array_font_cache.size();++i)
			{
				const SPtr<FontCache>& cache=_array_font_cache[i];
				for(int j=0;j<(int)cache->array_face().size();++j)
				{
					const SPtr<FTFaceInfo>& face=cache->array_face()[j];
					FontFaceLoadInfo* info=new FontFaceLoadInfo;
					info->name=face->name;
					info->cache=cache;
					info->index=j;

					_map_name_load_info[TOLOWER(face->name)]=info;
				}
			}
		}
		bool							TexFont::addFontCache(const wcs& filepath)
		{
			SPtr<FontCache> cache=FontCache::load(filepath);
			if(!cache)
				return false;
			for(int i=0;i<(int)_array_font_cache.size();++i)
				if(_array_font_cache[i]==cache)
					return true;
			_array_font_cache.push_back(cache);
			rebuildLoadInfo();
			return true;
		}
		bool							TexFont::removeFontCache(const wcs& filepath)
		{
			for(int i=0;i<(int)_array_font_cache.size();++i)
				if(_array_font_cache[i]->filepath()==filepath)
				{
					_array_font_cache.erase(_array_font_cache.begin()+i);
					rebuildLoadInfo();
					return true;
				}
			return false;
		}
		SPtr<TexFont>				TexFont::load(const mbs& family_name, size_t height)
		{
			SPtr<TexFont> f = new TexFont();
			f->_height = height;

			if(!f->insertFace(family_name, 0, height))
				return NULL;
			//f->_sampler=TexSampler::create(NULL);
			//f->_sampler->setMinFilter(Gfx::FILTERMODE_LINEAR);
			//f->_sampler->setMagFilter(Gfx::FILTERMODE_LINEAR);
			//f->_sampler->setMipFilter(Gfx::FILTERMODE_NONE);
			//f->_sampler->setUWrapMode(Gfx::WRAPMODE_CLAMP);
			//f->_sampler->setVWrapMode(Gfx::WRAPMODE_CLAMP);
			//f->_sampler->setWWrapMode(Gfx::WRAPMODE_CLAMP);
			//f->_sampler_decal=TexSampler::create(NULL);
			//f->_sampler_decal->setMinFilter(Gfx::FILTERMODE_LINEAR);
			//f->_sampler_decal->setMagFilter(Gfx::FILTERMODE_LINEAR);
			//f->_sampler_decal->setMipFilter(Gfx::FILTERMODE_NONE);
			//f->_sampler_decal->setUWrapMode(Gfx::WRAPMODE_REPEAT);
			//f->_sampler_decal->setVWrapMode(Gfx::WRAPMODE_REPEAT);
			//f->_sampler_decal->setWWrapMode(Gfx::WRAPMODE_REPEAT);
			return f;
		}
		bool								TexFont::insertFace(const mbs& family_name, size_t index)
		{
			return insertFace(family_name, index, _height);
		}
		bool								TexFont::insertFace(const mbs& family_name, size_t index, size_t height)
		{
			index = min(max(0, index), _array_face.size());
			mbs s = TOLOWER(family_name);
			if(!s.length() && _map_name_load_info.size())
				s = _map_name_load_info.begin()->first;

			if(!_map_name_load_info.count(s))
				return false;

			SPtr<FTLibrary> lib = FTLibrary::load();

			const FontFaceLoadInfo& info = *_map_name_load_info[s].get();

			FaceInst* inst = new FaceInst;
			inst->face = NULL;
			FT_New_Memory_Face((FT_Library)lib->library(), (const FT_Byte*)info.cache->filemem().get(), (FT_Long)info.cache->filemem().size(), info.index, (FT_Face*)&inst->face);
			if(!inst->face)
				return false;
			if(FT_Set_Pixel_Sizes((FT_Face)inst->face, 0, (FT_UInt)height))
			{
				FT_Done_Face((FT_Face)inst->face);
				return false;
			}
			//FT_Load_Char((FT_Face)inst->face,'a',FT_LOAD_RENDER);
			inst->lib = lib;
			inst->font_cache = info.cache;
			inst->info = info.cache->array_face()[info.index];

			_array_face.insert(_array_face.begin() + index, inst);

			return true;
		}
		void								TexFont::setDecal(const SPtr<TexBase>& decal)
		{
			//_sampler_decal->setTexture(decal);
			_tex_decal = decal;
		}
		void								TexFont::setDecalUVTransform(const Mat3& mat)
		{
			_mat_decal=mat;
		}
		bool								TexFont::useDecal() const
		{
			//return _sampler_decl->getTexture();
			return !!_tex_decal;
		}
		int									TexFont::supports(wchar_t ch) const				// -1 not found, otherwise 0-based index to array of face instances
		{
			for(int i=0;i<(int)_array_face.size();++i)
			{
				const FaceInst& inst=*_array_face[i].get();
				if(inst.info->map_char_glyph.size()<=ch)
					continue;
				if(inst.info->map_char_glyph[ch])
					return i;
			}
			return -1;
		}
		void								TexFont::cacheChar(wchar_t ch)
		{
			MAP_CHAR_CACHE::iterator iter = _char_cache.find(ch);
			if(iter != _char_cache.end())
			{
				size_t& id = iter->second->id;
				_map_id_char.erase(id);
				id = (++_next_id);
				_map_id_char[id] = ch;
				return;
			}
			if(!__render(ch))
			{
				cacheAdvance(ch,true);
				return;
			}

			FT_Face face=(FT_Face)_cur_face;
			const FT_Bitmap& bmp=face->glyph->bitmap;
			int dx=bmp.width;
			int dy=bmp.rows;dy=dy<0?-dy:dy;
			int tex_dx,tex_dy;
			if(true)
			{
				Gfx* gfx=Gfx::current();
				if(gfx->supportsNonPower2Tex())
				{
					tex_dx=dx;
					tex_dy=dy;
				}
				else
				{
					tex_dx=NEXT_POW2(dx);
					tex_dy=NEXT_POW2(dy);
				}
				tex_dx=min((int)min(Tex2D::getMaxSize(),Tex2D::getMaxRes()),tex_dx);
				tex_dy=min((int)min(Tex2D::getMaxSize(),Tex2D::getMaxRes()),tex_dy);
			}
			SPtr<Tex2D> tex=Tex2D::create(tex_dx, tex_dy, 1, GfxEnum::PIXELFORMAT_A8, false);
			if(!tex)
				tex=Tex2D::create(tex_dx, tex_dy, 1, GfxEnum::PIXELFORMAT_A8R8G8B8, false);
			if(!tex)
				return;

			fipImage fip;
			fip.setSize(FIT_BITMAP, dx, dy, 8);
			ftbitmap_to_fip((FT_Bitmap*)&bmp, fip);
			if(tex->getPixelFormat() != GfxEnum::PIXELFORMAT_A8)
			{
				fipImage fip2;
				fip2.setSize(FIT_BITMAP, dx, dy, 32);
				memset(fip2.accessPixels(), 0xff, fip2.getScanWidth() * fip.getHeight());
				fip2.setChannel(fip, FICC_ALPHA);
				fip=fip2;
			}
			
			if(tex_dx != dx || tex_dy != dy)
			{
				fipImage fip2;
				fip2.setSize(FIT_BITMAP,tex_dx,tex_dy,fip.getBitsPerPixel());
				fip2.pasteSubImage(fip,0,tex_dy-dy);
				fip=fip2;
			}
			tex->updateMipLevel(0,&fip);
			//fip.saveU(FORMATW(L"%c.png",ch).c_str());

			SPtr<CharCache> c = new CharCache;
			c->tex = tex;
			c->dx = dx;
			c->dy = dy;
			c->left = face->glyph->bitmap_left;
			c->top = face->glyph->bitmap_top;
			c->uv = Vec2((REAL)dx / tex_dx, (REAL)dy / tex_dy);
			c->id = (++_next_id);

			_char_cache[ch] = c;
			_map_id_char[c->id] = ch;

			cacheAdvance(ch, false);
		}
		void								TexFont::cacheAdvance(wchar_t ch)
		{
			cacheAdvance(ch, true);
		}
		bool								TexFont::__render(wchar_t ch)
		{
			int index = supports(ch);
			if(index == -1)
				index = 0;

			const FaceInst& inst = *_array_face[index].get();

			FT_Face face = (FT_Face)inst.face;
			_cur_face = face;

			int glyph = 0;
			if(inst.info->map_char_glyph.size() > ch)
				glyph = inst.info->map_char_glyph[ch];

			FT_Load_Glyph(face,glyph, FT_LOAD_RENDER | FT_LOAD_IGNORE_TRANSFORM | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
			if(!face->glyph->bitmap.buffer || !face->glyph->bitmap.width || !face->glyph->bitmap.rows)
				return false;

			return true;
		}
		void								TexFont::cacheAdvance(wchar_t ch,bool render)
		{
			if(_char_xadv.count(ch))
				return;

			FT_Face& f = (FT_Face&)_cur_face;							// needs to be reference, because _cur_face maybe change inside functions

			if(render)
				if(!__render(ch))
				{
					// for space, other control characters, other characters that don't render
					_char_xadv[ch] = (unsigned short)(f->glyph->advance.x >> 6);
					//_char_xadv[ch]=(unsigned short)(f->size->metrics.max_advance>>7);
					return;
				}

			//_char_xadv[ch]=(unsigned short)(f->glyph->advance.x>>6);
			_char_xadv[ch] = (unsigned short)((f->glyph->metrics.width + f->glyph->metrics.horiBearingX) >> 6);
		}
		//unsigned long						TexFont::getLineSpacing() const
		//{
		//	return (((FT_Face)_array_face[0]->face)->size->metrics.height)>>6;
		//}
		void								TexFont::draw(const wcs& text, const Vec3& pos, const Vec4& color, rsShade* shade_state)
		{
			if(!text.length())
				return;
			Gfx* gfx = Gfx::current();

			bool decal = useDecal();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getTexFontShaders(decal, vs, ps);
				gfx->useProgrammablePipeline(vs, ps);
				// presume the world transform
			}

			if(shade_state)
				gfx->setShadeState(*shade_state);
			else
			{
				rsShade rs(false);
				getRespectingShadeState(rs, false);
				gfx->setShadeState(rs);
			}

			ShaderUniforms2 unis(vs, ps);
			unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedWorldViewProjectionTransform());

			gfx->setShaderUniforms(unis);

			Vec2 p = Vec2(pos.x, pos.y);
			p.y += getHeight() + ((((FT_Face)_array_face[0]->face)->size->metrics.descender) >> 6);

			if(!decal)
			{
				const wchar_t* end = &*text.begin() + text.length();
				for(const wchar_t* iter = &*text.begin(); iter != end; ++iter)
				{
					wchar_t ch = *iter;
					cacheChar(ch);
					MAP_CHAR_CACHE::const_iterator it = _char_cache.find(ch);
					if(it == _char_cache.end())
					{
						p.x += _char_xadv[ch];
						continue;
					}
					const CharCache& c = *it->second;
					Vec2 p2 = p;
					p2.x += (REAL)c.left;
					p2.y += (REAL)(c.dy - c.top);

					REAL u = c.uv.x, v = c.uv.y;

					gfx->bindTexture(0, c.tex);

					Jotter jotter;
					jotter.begin(3, false, true, 2, 2);
					jotter.color(color);
					jotter.tc(1, 0, 0);
					//jotter.color(0, 0, 0, 1);

					jotter.tc(0, 0, v);
					jotter.vertex(p2.x, p2.y - c.dy, pos.z);

					jotter.tc(0, 0, 0);
					jotter.vertex(p2.x, p2.y, pos.z);

					jotter.tc(0, u, v);
					jotter.vertex(p2.x + c.dx, p2.y - c.dy, pos.z);

					jotter.tc(0, u, 0);
					jotter.vertex(p2.x + c.dx, p2.y, pos.z);

					gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);

					p.x += (REAL)_char_xadv[ch];
				}
			}
			else
			{
				gfx->bindTexture(1, _tex_decal);

				Vec3 uv;
				const wchar_t* end = &*text.begin() + text.length();
				for(const wchar_t* iter = &*text.begin(); iter != end; ++iter)
				{
					wchar_t ch = *iter;
					cacheChar(ch);
					MAP_CHAR_CACHE::const_iterator it = _char_cache.find(ch);
					if(it == _char_cache.end())
					{
						p.x += _char_xadv[ch];
						continue;
					}
					const CharCache& c = *it->second;
					Vec2 p2 = p;
					p2.x += (REAL)c.left;
					p2.y += (REAL)(c.dy - c.top);

					REAL u = c.uv.x, v = c.uv.y;

					gfx->bindTexture(0, c.tex);

					Jotter jotter;
					jotter.begin(3, false, true, 2, 2);
					jotter.color(color);

					jotter.tc(0, 0, v);
					uv = Vec3(p2.x, p2.y - c.dy, 1)*_mat_decal;
					uv /= uv.z;
					jotter.tc(1, uv.x, uv.y);
					jotter.vertex(p2.x, p2.y - c.dy, pos.z);

					jotter.tc(0, 0, 0);
					uv = Vec3(p2.x, p2.y, 1)*_mat_decal;
					uv /= uv.z;
					jotter.tc(1, uv.x, uv.y);
					jotter.vertex(p2.x, p2.y, pos.z);

					jotter.tc(0, u, v);
					uv = Vec3(p2.x + c.dx, p2.y - c.dy,1)*_mat_decal;
					uv /= uv.z;
					jotter.tc(1, uv.x, uv.y);
					jotter.vertex(p2.x + c.dx, p2.y - c.dy, pos.z);

					jotter.tc(0, u, 0);
					uv = Vec3(p2.x + c.dx, p2.y, 1) * _mat_decal;
					uv /= uv.z;
					jotter.tc(1,uv.x, uv.y);
					jotter.vertex(p2.x + c.dx, p2.y, pos.z);

					gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);

					p.x += (REAL)_char_xadv[ch];
				}
			}
		}
		//void								TexFont::begin()
		//{
		//	Gfx* gfx=Gfx::current();

		//	gfx->RS_setTSS_color_op(0,Gfx::TEXTUREOP_SELECT_ARG1);
		//	gfx->RS_setTSS_color_arg1(0,Gfx::TEXTUREARG_PREVIOUS);
		//	gfx->RS_setTSS_alpha_op(0,Gfx::TEXTUREOP_MODULATE);
		//	gfx->RS_setTSS_alpha_arg1(0,Gfx::TEXTUREARG_PREVIOUS);
		//	gfx->RS_setTSS_alpha_arg2(0,Gfx::TEXTUREARG_TEXTURE);

		//	gfx->RS_setTSS_color_op(1,Gfx::TEXTUREOP_DISABLE);
		//}
		//void								TexFont::end()
		//{
		//	Gfx* gfx=Gfx::current();

		//	gfx->RS_setTSS_color_op(0,Gfx::TEXTUREOP_DISABLE);
		//}
		size_t								TexFont::getHeight() const
		{
			return _height;
		}
		int									TexFont::stringWidth(const wcs& s)
		{
			int dx = 0;
			for(int i = 0; i < (int)s.length(); ++i)
			{
				wchar_t ch = s[i];
				cacheAdvance(ch, true);
				dx += _char_xadv[ch];
			}
			return dx;
		}
		vector<wcs>							TexFont::line_wrap(const wcs& str, int line_width)
		{
			vector<wcs> s;

			int remaining = -1;
			wcs s2 = str;
			wcs* cur = NULL;
			while(s2.length())
			{
				wchar_t ch = s2[0];
				if(ch == '\x0a')
				{
					s.push_back(L"");
					if(s.size() == 1)
						s.push_back(L"");
					remaining = line_width;
					s2.erase(s2.begin());
					cur = &s[s.size() - 1];
					continue;
				}
				cacheAdvance(ch);
				int dx = _char_xadv[ch];
				if(dx > remaining)
				{
					s.push_back(L"");
					remaining = line_width;
					cur = &s[s.size() - 1];
				}
				remaining -= dx;
				*cur += ch;
				s2.erase(s2.begin());
			}
			return s;
		}
		int									TexFont::getCharAdvance(wchar_t ch)
		{
			cacheAdvance(ch,true);
			return _char_xadv[ch];
		}
		//void								TexFont::draw(const wstring& text, const Vec3& pos, const Vec2& ext, const Vec4& color, bool perform_line_wrap, int textalign, rsShade* shade_state)
		//{
		//	// need optimize, too slow every time allocating memory
		//	vector<wstring> s;
		//	if(perform_line_wrap)
		//		s = line_wrap(text, NINT(ext.x));
		//	else
		//		s.push_back(text);

		//	int dx;
		//	int dy = (int)(s.size() * getHeight());
		//	int y = NINT(pos.y);
		//	if(textalign & TexFont::TEXTALIGN_MIDDLE)
		//		y = y + ((NINT(ext.y) - dy) >> 1);
		//	else if(textalign & TexFont::TEXTALIGN_BOTTOM)
		//		y = y + NINT(ext.y) - dy;
		//	for(int i = 0; i < (int)s.size(); ++i)
		//	{
		//		const wstring& s2 = s[i];
		//		dx = stringWidth(s2);

		//		int x = NINT(pos.x);
		//		if(textalign & TexFont::TEXTALIGN_CENTER)
		//			x = x + ((NINT(ext.x) - dx) >> 1);
		//		else if(textalign & TexFont::TEXTALIGN_RIGHT)
		//			x = x + NINT(ext.x) - dx;

		//		draw(s2, Vec3((REAL)x, (REAL)(y + getHeight() * i), pos.z), color, shade_state);
		//	}
		//}
		void								TexFont::__draw(const wcs& text, const Vec3& pos, rsShade* shade_state, DrawTextOperation& dto)
		{
			vector<wcs> s;
			if(dto.p.flags.line_wrap)
				s = line_wrap(text, NINT(dto.p.ext.x));

			int32_t dx;
			int32_t dy = (int32_t)((dto.p.flags.line_wrap ? s.size() : 1) * getHeight());
			int32_t y = NINT(pos.y);
			if(dto.p.flags.valign == 1)
				y = y + ((NINT(dto.p.ext.y) - dy) >> 1);
			else if(dto.p.flags.valign == 2)
				y = y + NINT(dto.p.ext.y) - dy;
			size_t c = (dto.p.flags.line_wrap ? s.size() : 1);
			for(size_t i = 0; i < c; ++i)
			{
				const wcs& s2 = (dto.p.flags.line_wrap ? s[i] : text);
				dx = stringWidth(s2);

				int32_t x = NINT(pos.x);
				if(dto.p.flags.halign == 1)
					x = x + ((NINT(dto.p.ext.x) - dx) >> 1);
				else if(dto.p.flags.halign == 2)
					x = x + NINT(dto.p.ext.x) - dx;

				__draw_single_line(s2, Vec3((REAL)x, (REAL)(y + getHeight() * i), pos.z), shade_state, dto);
			}
		}
		void								TexFont::__draw_single_line(const wcs& text, const Vec3& pos, rsShade* shade_state, DrawTextOperation& dto)
		{
			if(dto.p.flags.shadow)
				draw(text, pos + dto.p.shadow_offset, dto.p.shadow_color, shade_state);
			draw(text, pos, dto.p.color, shade_state);
		}
		void								TexFont::getRespectingShadeState(rsShade& rs, bool reset_shade_state) const
		{
			if(reset_shade_state)
				rs.reset(false);

			rs.enableBlend(GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GfxEnum::BLENDOPERATION_ADD);

			rs.setSampler(0, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);
			if(useDecal())
				rs.setSampler(1, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE_REPEAT, GfxEnum::WRAPMODE_REPEAT, GfxEnum::WRAPMODE_REPEAT);
		}
		DrawTextOperation					TexFont::begin()
		{
			return DrawTextOperation(this);
		}
	};
};

