
#include "Img2D.h"

#include "rsShade.h"
#include "Shader.h"
#include "ShaderUniforms.h"
#include "ShaderUniforms2.h"

#include "Jotter.h"

#include "../../FreeImage/Header.h"

namespace z3D
{
	namespace Video
	{
		Img2D::Img2D(const wcs& filepath)
			: ResBase(filepath)
		{
			_dx = 0;
			_dy = 0;
			_horz_slots = 0;
			_vert_slots = 0;
			_texture_size = 0;
			_packed_cfg = NULL;
			_packed = NULL;
			_offset_x = 0;
			_offset_y = 0;
			_clip_x = 0;
			_clip_y = 0;
			_clip_dx = 0;
			_clip_dy = 0;
		}
		Img2D::~Img2D()
		{
			if(_packed_cfg)
				delete _packed_cfg;
			hash_map<wcs, SPtr<Img2D> >::iterator iter;
			for(iter = _packed_img_lookup.begin(); iter != _packed_img_lookup.end(); )
				if(iter->second.get() == this)
					iter = _packed_img_lookup.erase(iter);
				else
					++iter;
		}
		void						Img2D::setForce16Bit(bool force)
		{
			FORCE_16BIT = force;
		}
		bool						Img2D::getForce16Bit()
		{
			return FORCE_16BIT;
		}
		SPtr<Img2D>					Img2D::load(const wcs& filepath, int texture_size)
		{
			if(!filepath.length())
				return NULL;
			hash_map<wcs, SPtr<Img2D> >::iterator iter = _packed_img_lookup.find(filepath);
			if(iter != _packed_img_lookup.end())
			{
				SPtr<Img2D> packed = iter->second;
				int index = packed->_packed_img_index[filepath];

				wcs prefix = FORMATW(L"image%d_",index);
				Img2D* img = new Img2D(filepath);
				img->_dx = packed->_packed_cfg->getInt32(prefix + L"width");
				img->_dy = packed->_packed_cfg->getInt32(prefix + L"height");
				img->_offset_x = packed->_packed_cfg->getInt32(prefix + L"offset_left");
				img->_offset_y = packed->_packed_cfg->getInt32(prefix + L"offset_top");
				img->_clip_x = packed->_packed_cfg->getInt32(prefix + L"clip_left");
				img->_clip_y = packed->_packed_cfg->getInt32(prefix + L"clip_top");
				img->_clip_dx = packed->_packed_cfg->getInt32(prefix + L"clip_width");
				img->_clip_dy = packed->_packed_cfg->getInt32(prefix + L"clip_height");
				img->_packed = packed;

				_res_mgr.add(img);

				return SPtr<Img2D>(img);
			}

			Img2D* img = _res_mgr.find<Img2D>(filepath);
			if(img)
				return SPtr<Img2D>(img);

			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			if(!sm.size())
				return NULL;

			fipMemoryIO memio((BYTE*)sm.get(), (DWORD)sm.size());

			fipImage fip;
			BOOL ret = fip.loadFromMemory(memio);
			if(!ret)
				return NULL;

			if(fip.getBitsPerPixel() != 32)
				fip.convertTo32Bits();

			SPtr<Img2D> img2 = load_fip(&fip, texture_size);
			if(!img2)
				return NULL;

			img2->set_filepath(filepath);
			_res_mgr.add(img2.get());

			return img2;
		}
		SPtr<Img2D>					Img2D::load_fip(void* fipimage, int texture_size)
		{
			fipImage& fip = *(fipImage*)fipimage;

			Z_ASSERT( fip.getBitsPerPixel() == 32 );

			SPtr<Img2D> img = Img2D::create(fip.getWidth(), fip.getHeight(), texture_size);

			int i, j;
			for(j = 0; j < img->_vert_slots; ++j)
				for(i = 0; i < img->_horz_slots; ++i)
				{
					size_t dx, dy;
					img->texture(j, i)->getMipLevelDim(0, dx, dy);
					fipImage sub;
					fip.copySubImage(sub, i * texture_size, j * texture_size, min((i + 1) * texture_size, (int)fip.getWidth()), min((j + 1) * texture_size, (int)fip.getHeight()));
					if(sub.getWidth() == dx && sub.getHeight() == dy)
					{
						void* bits;
						size_t pitch;
						img->texture(j, i)->lockMipLevel(0, bits, pitch, LockParams());
						GfxHelper::copy2D(sub.accessPixels(), bits, dx * sub.getBitsPerPixel(), dy, (size_t)sub.getScanWidth(), pitch);
						img->texture(j, i)->unlockMipLevel(0);
					}
					else
					{
						fipImage sub2;
						sub2.setSize(FIT_BITMAP, (WORD)dx, (WORD)dy, 32);
						sub2.pasteSubImage(sub, 0, 0);

						void* bits;
						size_t pitch;
						img->texture(j, i)->lockMipLevel(0, bits, pitch, LockParams());
						GfxHelper::copy2D(sub2.accessPixels(), bits, dx * sub2.getBitsPerPixel(), dy, (size_t)sub2.getScanWidth(), pitch);
						img->texture(j, i)->unlockMipLevel(0);
					}
				}

			return img;
		}
		SPtr<Img2D>					Img2D::create(int dx, int dy, int texture_size)
		{
			Z_ASSERT( Gfx::current() );
			int horz_slots = (dx + texture_size - 1) / texture_size;
			int vert_slots = (dy + texture_size - 1) / texture_size;
			int i, j;
			Img2D* img = new Img2D(L"");
			img->_dx = dx;
			img->_dy = dy;
			img->_texture_size = texture_size;
			img->_horz_slots = horz_slots;
			img->_vert_slots = vert_slots;
			for(j = 0; j < vert_slots; ++j)
				for(i = 0; i < horz_slots; ++i)
				{
					int part_dx, part_dy;
					part_dx = min(texture_size, dx - i * texture_size);
					part_dy = min(texture_size, dy - j * texture_size);

					int tex_dx, tex_dy;
					tex_dx = NEXT_POW2(part_dx);
					tex_dy = NEXT_POW2(part_dy);

					SPtr<Tex2D> tex = Gfx::current()->createTex2D(Tex2DCreateParams(tex_dx, tex_dy, 1, GfxEnum::PIXELFORMAT_A8R8G8B8, GfxEnum::POOL_MANAGED));
					img->_tex.push_back(tex);
				}

			return img;
		}
		int							Img2D::dx() const {return _dx;}
		int							Img2D::dy() const {return _dy;}
		int							Img2D::horz_slots() const {return _horz_slots;}
		int							Img2D::vert_slots() const {return _vert_slots;}
		int							Img2D::texture_size() const {return _texture_size;}
		const SPtr<Tex2D>&			Img2D::texture(int r, int c) const {int index = r * _horz_slots + c; if(index < 0 || index >= (int)_tex.size()) return _null_tex2d; return _tex[index];}
		void						Img2D::draw(int dest_x,int dest_y, const Vec4& color, rsShade* shade_state) const
		{
			draw(0, 0, dx(), dy(), dest_x, dest_y, color, shade_state);
		}
		void						Img2D::draw(int src_x,int src_y,int dx,int dy,int dest_x,int dest_y, const Vec4& color, rsShade* shade_state) const
		{
			if(_packed)
			{
				if(_clip_x > src_x)
				{
					int t = _clip_x - src_x;
					dx -= t;
					dest_x += t;
					src_x = _clip_x;
				}
				if(src_x + dx > _clip_x + _clip_dx)
					dx -= src_x + dx - (_clip_x + _clip_dx);

				if(_clip_y > src_y)
				{
					int t = _clip_y - src_y;
					dy -= t;
					dest_y += t;
					src_y = _clip_y;
				}
				if(src_y + dy > _clip_y + _clip_dy)
					dy -= src_y + dy - (_clip_y + _clip_dy);

				_packed->draw(_offset_x + src_x - _clip_x, _offset_y + src_y - _clip_y, dx, dy, dest_x, dest_y, color, shade_state);
				return;
			}
			Gfx* gfx=Gfx::current();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getImg2DShaders(vs, ps);
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

			int i, j;
			int start_vert, start_horz, end_vert, end_horz;
			start_vert = max(src_y / _texture_size, 0);
			start_horz = max(src_x / _texture_size, 0);
			end_vert = min((src_y + dy - 1) / _texture_size, _vert_slots - 1);
			end_horz = min((src_x + dx - 1) / _texture_size, _horz_slots - 1);
			for(j = start_vert; j <= end_vert; ++j)
				for(i = start_horz; i <= end_horz; ++i)
				{
					int part_dx, part_dy;
					part_dx = min(_texture_size, _dx - i * _texture_size);
					part_dy = min(_texture_size, _dy - j * _texture_size);

					int tex_dx, tex_dy;
					tex_dx = NEXT_POW2(part_dx);
					tex_dy = NEXT_POW2(part_dy);

					gfx->bindTexture(0, texture(j, i));

					int u,v;
					u = i * _texture_size;
					v = j * _texture_size;
					int x1, y1, dx1, dy1;
					intersect_area(src_x, src_y, dx, dy, u, v, part_dx, part_dy, x1, y1, dx1, dy1);

					Jotter jotter;

					jotter.begin(2, false, true, 1, 2);
					jotter.color(color);

					jotter.tc(0, ((x1 - u)) / (REAL)tex_dx, 1.0f - ((y1 - v)) / (REAL)tex_dy);
					jotter.vertex((REAL)(dest_x + (x1 - src_x)), (REAL)(dest_y + (y1 - src_y)));

					jotter.tc(0, ((x1 - u)) / (REAL)tex_dx, 1.0f - ((y1 - v) + dy1) / (REAL)tex_dy);
					jotter.vertex((REAL)(dest_x + (x1 - src_x)), (REAL)(dest_y + (y1 - src_y) + dy1));

					jotter.tc(0, ((x1 - u) + dx1) / (REAL)tex_dx, 1.0f - ((y1 - v)) / (REAL)tex_dy);
					jotter.vertex((REAL)(dest_x + (x1 - src_x) + dx1), (REAL)(dest_y + (y1 - src_y)));

					jotter.tc(0, ((x1 - u) + dx1) / (REAL)tex_dx, 1.0f - ((y1 - v) + dy1) / (REAL)tex_dy);
					jotter.vertex((REAL)(dest_x + (x1 - src_x) + dx1), (REAL)(dest_y + (y1 - src_y) + dy1));

					gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);
				}
		}
		void						Img2D::drawStretch(int src_x,int src_y,int src_dx,int src_dy,int dest_x,int dest_y,int dest_dx,int dest_dy, const Vec4& color, rsShade* shade_state) const
		{
			if(_packed)
			{
				REAL dx_rate = (REAL)dest_dx / src_dx;
				REAL dy_rate = (REAL)dest_dy / src_dy;

				if(_clip_x > src_x)
				{
					int t = _clip_x - src_x;
					src_dx -= t;
					dest_x += (int)(t * dx_rate);
					src_x = _clip_x;
				}
				if(src_x + src_dx > _clip_x + _clip_dx)
					src_dx -= src_x + src_dx - (_clip_x + _clip_dx);

				if(_clip_y > src_y)
				{
					int t = _clip_y - src_y;
					src_dy -= t;
					dest_y += (int)(t * dy_rate);
					src_y = _clip_y;
				}
				if(src_y + src_dy > _clip_y + _clip_dy)
					src_dy -= src_y + src_dy - (_clip_y + _clip_dy);

				dest_dx = (int)(src_dx * dx_rate);
				dest_dy = (int)(src_dy * dy_rate);

				_packed->drawStretch(_offset_x + src_x - _clip_x, _offset_y + src_y - _clip_y, src_dx, src_dy, dest_x, dest_y, dest_dx, dest_dy, color, shade_state);

				return;
			}
			REAL dx_rate = (REAL)dest_dx / src_dx;
			REAL dy_rate = (REAL)dest_dy / src_dy;

			Gfx* gfx = Gfx::current();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getImg2DShaders(vs, ps);
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

			int i, j;
			int start_vert, start_horz, end_vert, end_horz;
			start_vert = max(src_y / _texture_size, 0);
			start_horz = max(src_x / _texture_size, 0);
			end_vert = min((src_y + src_dy - 1) / _texture_size, _vert_slots - 1);
			end_horz = min((src_x + src_dx - 1) / _texture_size, _horz_slots - 1);
			for(j = start_vert; j <= end_vert; ++j)
				for(i = start_horz; i <= end_horz; ++i)
				{
					int part_dx, part_dy;
					part_dx = min(_texture_size, _dx - i * _texture_size);
					part_dy = min(_texture_size, _dy - j * _texture_size);

					int tex_dx, tex_dy;
					tex_dx = NEXT_POW2(part_dx);
					tex_dy = NEXT_POW2(part_dy);

					gfx->bindTexture(0, texture(j, i));

					int u,v;
					u = i * _texture_size;
					v = j * _texture_size;
					int x1, y1, dx1, dy1;
					intersect_area(src_x, src_y, src_dx, src_dy, u, v, part_dx, part_dy, x1, y1, dx1, dy1);

					Jotter jotter;
					jotter.begin(2, false, true, 1, 2);
					jotter.color(color);

					jotter.tc(0,((x1-u))/(REAL)tex_dx,1.0f-((y1-v))/(REAL)tex_dy);
					jotter.vertex((REAL)(dest_x+(x1-src_x)*dx_rate),(REAL)(dest_y+(y1-src_y)*dy_rate));

					jotter.tc(0,((x1-u))/(REAL)tex_dx,1.0f-((y1-v)+dy1)/(REAL)tex_dy);
					jotter.vertex((REAL)(dest_x+(x1-src_x)*dx_rate),(REAL)(dest_y+((y1-src_y)+dy1)*dy_rate));

					jotter.tc(0,((x1-u)+dx1)/(REAL)tex_dx,1.0f-((y1-v))/(REAL)tex_dy);
					jotter.vertex((REAL)(dest_x+((x1-src_x)+dx1)*dx_rate),(REAL)(dest_y+(y1-src_y)*dy_rate));

					jotter.tc(0,((x1-u)+dx1)/(REAL)tex_dx,1.0f-((y1-v)+dy1)/(REAL)tex_dy);
					jotter.vertex((REAL)(dest_x+((x1-src_x)+dx1)*dx_rate),(REAL)(dest_y+((y1-src_y)+dy1)*dy_rate));

					gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);
				}
		}
		void						Img2D::author_packing(const Config& cfg)
		{
			int total_images = cfg.getInt32(L"total_images");
			int i;
			for(i = 0; i < total_images; ++i)
			{
				wcs filepath = cfg.getWString(FORMATW(L"image%d", i));
				_packed_img_lookup[filepath] = this;
				_packed_img_index[filepath] = i;
			}
			if(!_packed_cfg)
				_packed_cfg = new Config(L"");
			*_packed_cfg = cfg;
		}
		int							Img2D::offset_x() const {return _offset_x;}
		int							Img2D::offset_y() const {return _offset_y;}
		int							Img2D::clip_x() const {return _clip_x;}
		int							Img2D::clip_y() const {return _clip_y;}
		int							Img2D::clip_dx() const {return _clip_dx;}
		int							Img2D::clip_dy() const {return _clip_dy;}
		void						Img2D::getRespectingShadeState(rsShade& rs, bool reset_shade_state) const
		{
			if(reset_shade_state)
				rs.reset(true);

			rs.enableBlend(GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GfxEnum::BLENDOPERATION_ADD);
			rs.setSampler(0, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);
		}
	};
};

