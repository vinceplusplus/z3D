
#pragma once

#include "rsShade.h"
#include "TexFont.h"

namespace z3D
{
	namespace Video
	{
		rsShade::rsShade(bool fixed_pipeline)
		{
			reset(fixed_pipeline);
		}
		rsShade::rsShade(const rsShade& rhs)
		{
			__assign(rhs);
		}
		rsShade&					rsShade::operator=(const rsShade& rhs)
		{
			__assign(rhs);
			return *this;
		}
		void						rsShade::__assign(const rsShade& rhs)
		{
			__UNNAMED00 = rhs.__UNNAMED00;
			__UNNAMED01 = rhs.__UNNAMED01;
			write = rhs.write;
			enable = rhs.enable;
			if(enable.alpha_test)
				alpha_ref = rhs.alpha_ref;
			if(enable.stencil)
			{
				stencil = rhs.stencil;
				stencil_op = rhs.stencil_op;
			}
			if(enable.scissor_test)
				scissor = rhs.scissor;
			if((clip_plane_count = rhs.clip_plane_count) != 0)
				memcpy(clip_planes, rhs.clip_planes, sizeof(clip_planes[0]) * clip_plane_count);
			sampler_use_bitset = rhs.sampler_use_bitset;
			memcpy(ss, rhs.ss, sizeof(ss));

			if(!fixed_pipeline)
				return;

			memcpy(fixed.default_color, rhs.fixed.default_color, sizeof(fixed.default_color));
			fixed.enable = rhs.fixed.enable;
			if(fixed.enable.lighting)
			{
				fixed.lighting.material = rhs.fixed.lighting.material;
				memcpy(fixed.lighting.light_ambient, rhs.fixed.lighting.light_ambient, sizeof(fixed.lighting.light_ambient));
				if((fixed.lighting.light_count = rhs.fixed.lighting.light_count) != 0)
					memcpy(fixed.lighting.lights, rhs.fixed.lighting.lights, sizeof(fixed.lighting.lights[0]) * fixed.lighting.light_count);
			}
			if(fixed.enable.fog)
				fixed.fog = rhs.fixed.fog;
			if((fixed.texture_stage_count = rhs.fixed.texture_stage_count) != 0)
			{
				memcpy(fixed.tss, rhs.fixed.tss, sizeof(fixed.tss[0]) * fixed.texture_stage_count);
				memcpy(fixed.tct, rhs.fixed.tct, sizeof(fixed.tct[0]) * fixed.texture_stage_count);
			}
		}
		void						rsShade::enableWriteColor								(bool red, bool green, bool blue, bool alpha)
		{
			write.R = red;
			write.G = green;
			write.B = blue;
			write.A = alpha;
		}
		void						rsShade::enableWriteDepth								(bool write)
		{
			this->write.depth = write;
		}
		void						rsShade::enableWriteStencil								(unsigned int mask)
		{
			write.stencil = mask;
		}

		void						rsShade::enableCull										(GfxEnum::CULLMODE mode)
		{
			enable.cull = true;
			cull_mode = mode;
		}
		void						rsShade::enableDepthTest								(GfxEnum::CMPFUNC func)
		{
			enable.depth_test = true;
			depth_func = func;
		}
		void						rsShade::enableAlphaTest								(GfxEnum::CMPFUNC func, REAL ref)
		{
			enable.alpha_test = true;
			alpha_func = func;
			alpha_ref = ref;
		}
		void						rsShade::enableBlend									(GfxEnum::BLENDFACTOR sbf, GfxEnum::BLENDFACTOR dbf, GfxEnum::BLENDOPERATION bo)
		{
			enable.blend = true;
			src_blend_factor = sbf;
			dst_blend_factor = dbf;
			blend_operation = bo;
		}
		void						rsShade::enableStencilTest								(GfxEnum::CMPFUNC front_func, int ref, int mask, GfxEnum::STENCILOP front_fail, GfxEnum::STENCILOP front_zfail, GfxEnum::STENCILOP front_pass)
		{
			enable.stencil = true;
			stencil.front_func = front_func;
			stencil.ref = ref;
			stencil.mask = mask;
			stencil_op.front_fail = front_fail;
			stencil_op.front_zfail = front_zfail;
			stencil_op.front_pass = front_pass;
		}
		void						rsShade::enableTwoSideStencilTest						(GfxEnum::CMPFUNC back_func, GfxEnum::STENCILOP back_fail, GfxEnum::STENCILOP back_zfail, GfxEnum::STENCILOP back_pass)
		{
			stencil.back_func = back_func;
			stencil_op.back_fail = back_fail;
			stencil_op.back_zfail = back_zfail;
			stencil_op.back_pass = back_pass;
		}
		void						rsShade::enableScissorTest								(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			enable.scissor_test = true;
			scissor.x = x;
			scissor.y = y;
			scissor.dx = dx;
			scissor.dy = dy;
		}
		void						rsShade::enableFixedFog									(GfxEnum::FOGMODE mode, REAL density, REAL start, REAL end, const Vec4& color)
		{
			Z_ASSERT( fixed_pipeline );

			fixed.enable.fog = true;
			fixed.fog.mode = mode;
			fixed.fog.density = density;
			fixed.fog.start = start;
			fixed.fog.end = end;
			memcpy(fixed.fog.color, &color, sizeof(color));
		}
		void						rsShade::enableFixedLighting							(const Vec4& light_ambient, const Vec4& mtl_ambient, const Vec4& mtl_diffuse, const Vec4& mtl_specular, const Vec4& mtl_emissive, REAL mtl_shininess)
		{
			Z_ASSERT( fixed_pipeline );

			fixed.enable.lighting = true;
			memcpy(fixed.lighting.material.ambient, &mtl_ambient, sizeof(mtl_ambient));
			memcpy(fixed.lighting.material.diffuse, &mtl_diffuse, sizeof(mtl_diffuse));
			memcpy(fixed.lighting.material.specular, &mtl_specular, sizeof(mtl_specular));
			memcpy(fixed.lighting.material.emissive, &mtl_emissive, sizeof(mtl_emissive));
			fixed.lighting.material.shininess = mtl_shininess;
			memcpy(fixed.lighting.light_ambient, &light_ambient, sizeof(light_ambient));
			fixed.lighting.light_count = 0;
		}
		void						rsShade::enableFixedNormalize							()
		{
			Z_ASSERT( fixed_pipeline );

			fixed.enable.normalize = true;
		}
		void						rsShade::enableFixedSpecular							()
		{
			Z_ASSERT( fixed_pipeline );

			fixed.enable.specular = true;
		}

		void						rsShade::setShadeMode									(GfxEnum::SHADEMODE mode)
		{
			shade_mode = mode;
		}
		void						rsShade::setPolygonMode									(GfxEnum::POLYGONMODE mode)
		{
			polygon_mode = mode;
		}
		void						rsShade::setFixedDefaultColor							(const Vec4& color)
		{
			memcpy(fixed.default_color, &color, sizeof(color));
		}

		bool						rsShade::addClipPlane									(const Plane& p)
		{
			if(clip_plane_count >= MAX_CLIP_PLANES)
				return false;

			memcpy(clip_planes + clip_plane_count, &p, sizeof(p));
			++clip_plane_count;

			return true;
		}
		bool						rsShade::setSampler										(size_t index, GfxEnum::FILTERMODE min_filter, GfxEnum::FILTERMODE mag_filter, GfxEnum::FILTERMODE mip_filter, GfxEnum::WRAPMODE u_wrap, GfxEnum::WRAPMODE v_wrap, GfxEnum::WRAPMODE w_wrap)
		{
			if(index >= MAX_SAMPLERS)
				return false;

			rsShade::SS& s = ss[index];

			s.min_filter = min_filter;
			s.mag_filter = mag_filter;
			s.mip_filter = mip_filter;
			s.u_wrap = u_wrap;
			s.v_wrap = v_wrap;
			s.w_wrap = w_wrap;

			sampler_use_bitset |= 1 << index;

			return true;
		}
		bool						rsShade::addFixedLight									(GfxEnum::LIGHTTYPE type, const Vec4& ambient, const Vec4& diffuse, const Vec4& specular, const Vec3& position, const Vec3& direction, REAL outer_cone, REAL inner_cone, REAL falloff, const Vec3& attenuation)
		{
			Z_ASSERT( fixed_pipeline );

			if(fixed.lighting.light_count >= MAX_LIGHTS)
				return false;

			rsShade::FIXED::LIGHTING::LIGHT& l = fixed.lighting.lights[fixed.lighting.light_count];

			l.type = type;
			memcpy(l.ambient, &ambient, sizeof(ambient));
			memcpy(l.diffuse, &diffuse, sizeof(diffuse));
			memcpy(l.specular, &specular, sizeof(specular));
			memcpy(l.position, &position, sizeof(position));
			memcpy(l.direction, &direction, sizeof(direction));
			l.outer_cone = outer_cone;
			l.inner_cone = inner_cone;
			l.falloff = falloff;
			memcpy(l.attenuation, &attenuation, sizeof(attenuation));

			++fixed.lighting.light_count;

			return true;
		}
		bool						rsShade::addFixedTextureStage							(GfxEnum::TEXTUREOP color_op, GfxEnum::TEXTUREARG color_arg1, GfxEnum::TEXTUREARG color_arg2, GfxEnum::TEXTUREOP alpha_op, GfxEnum::TEXTUREARG alpha_arg1, GfxEnum::TEXTUREARG alpha_arg2, const Mat4& texcoord_transform)
		{
			Z_ASSERT( fixed_pipeline );

			if(fixed.texture_stage_count >= MAX_STAGES)
				return false;

			rsShade::FIXED::TSS& t = fixed.tss[fixed.texture_stage_count];

			memcpy(fixed.tct[fixed.texture_stage_count], &texcoord_transform, sizeof(texcoord_transform));

			t.color_op = color_op;
			t.color_arg1 = color_arg1;
			t.color_arg2 = color_arg2;
			t.alpha_op = alpha_op;
			t.alpha_arg1 = alpha_arg1;
			t.alpha_arg2 = alpha_arg2;

			++fixed.texture_stage_count;

			return true;
		}

		void						rsShade::disableWriteColor								()
		{
			write.R = false;
			write.G = false;
			write.B = false;
			write.A = false;
		}

		void						rsShade::disableWriteDepth								()
		{
			write.depth = false;
		}

		void						rsShade::disableCull									()
		{
			enable.cull = false;
		}

		void						rsShade::disableDepthTest								()
		{
			enable.depth_test = false;
		}

		void						rsShade::disableAlphaTest								()
		{
			enable.alpha_test = false;
		}
		void						rsShade::disableBlend									()
		{
			enable.blend = false;
		}

		void						rsShade::reverseCull									()
		{
			if(!enable.cull)
				return;
			if(cull_mode == GfxEnum::CULLMODE_CW)
				cull_mode = GfxEnum::CULLMODE_CCW;
			else
				cull_mode = GfxEnum::CULLMODE_CW;
		}



		void						rsShade::reset											(bool fixed_pipeline)
		{
			__UNNAMED00 = 0;
			__UNNAMED01 = 0;
			clip_plane_count = 0;
			sampler_use_bitset = 0;
			write.__UNNAMED04 = 0;
			write.stencil = 0;
			__UNNAMED05 = 0;

			if(fixed_pipeline)
			{
				this->fixed_pipeline = true;

				fixed.default_color[0] = 1.0f; fixed.default_color[1] = 1.0f; fixed.default_color[2] = 1.0f; fixed.default_color[3] = 1.0f;
				fixed.texture_stage_count = 0;
				fixed.__UNNAMED06 = 0;
			}

			shade_mode = GfxEnum::SHADEMODE_GOURAUD;
			polygon_mode = GfxEnum::POLYGONMODE_FILL;
			write.R = true;
			write.G = true;
			write.B = true;
			write.A = true;

			// test scissor
			//enableScissorTest(50, 50, 100, 100);
		}
	};
};

