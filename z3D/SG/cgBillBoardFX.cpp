
#include "cgBillBoardFX.h"
#include "RenderInfo.h"

namespace z3D
{
	namespace SG
	{
		cgBillBoardFX::Image::Image() {}
		cgBillBoardFX::Image::~Image() {}

		cgBillBoardFX::Item::Item()
		{
			anim = 0.0f;
			align = ALIGN_SCREEN;
			blend = BLEND_ADDITIVE;
		}
		cgBillBoardFX::Item::~Item() {}

		cgBillBoardFX::cgBillBoardFX(const AABB& local_bound) : _local_bound(local_bound)
		{
		}
		cgBillBoardFX::~cgBillBoardFX()
		{
		}
		void						cgBillBoardFX::addBillBoard(const SPtr<Item>& item)
		{
			_array_item.push_back(item);
		}
		SPtr<cgBillBoardFX::Image>	cgBillBoardFX::createImage(const wcs& filepath, size_t rows, size_t cols, size_t frames)
		{
			Z_ASSERT( frames <= Image::MAX_FRAMES );

			SPtr<Image> image = new Image();
			image->_tex = Tex2D::load(filepath, false, GfxEnum::PIXELFORMAT_A8R8G8B8, false);
			image->_frames = frames;

			for(size_t i = 0; i < frames; ++i)
			{
				Image::FRAME& f = image->_array_frame[i];
				size_t r = i / cols;
				size_t c = i % cols;

				f.lt[0] = (REAL)c / cols;
				f.lt[1] = 1.0f - (REAL)r / rows;

				f.rb[0] = (REAL)(c + 1) / cols;
				f.rb[1] = 1.0f - (REAL)(r + 1) / rows;
			}

			return image;
		}
		bool						cgBillBoardFX::loadShaders()
		{
			_vs_screen = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, 
"\
\n\
float4x4 wvp;\n\
float3 cam_right;\n\
float3 cam_up;\n\
\n\
void main(\n\
	float4 in_pos : POSITION,\n\
	out float4 out_pos : POSITION,\n\
	float3 in_nor : NORMAL,\n\
	float4 in_color : COLOR,\n\
	out float4 out_color : COLOR,\n\
	float2 in_tc_0 : TEXCOORD0,\n\
	out float2 out_tc_0 : TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos + float4(in_nor.x * cam_right + in_nor.y * cam_up, 0), wvp);\n\
	out_color = in_color;\n\
	out_tc_0 = in_tc_0;\n\
}\n\
\
");
			_vs_world = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, 
"\
\n\
float4x4 wvp;\n\
float3 cam_pos;\n\
float3 cam_right;\n\
float3 cam_up;\n\
\n\
void main(\n\
	float4 in_pos : POSITION,\n\
	out float4 out_pos : POSITION,\n\
	float3 in_nor : NORMAL,\n\
	float4 in_color : COLOR,\n\
	out float4 out_color : COLOR,\n\
	float2 in_tc_0 : TEXCOORD0,\n\
	out float2 out_tc_0 : TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	float3 right = cam_right;\n\
	float3 up = cam_up;\n\
	float3 vp = cam_pos - in_pos.xyz;\n\
	if(dot(vp, vp) > 0.00001)\n\
	{\n\
		vp = normalize(vp);\n\
		if(abs(dot(cam_up, up)) > 1 - 0.00001)\n\
			right = normalize(cross(cam_up, vp));\n\
		up = normalize(cross(vp, right));\n\
	}\n\
\n\
	out_pos = mul(in_pos + float4(in_nor.x * right + in_nor.y * up, 0), wvp);\n\
	out_color = in_color;\n\
	out_tc_0 = in_tc_0;\n\
}\n\
\
");

			_ps = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, 
"\
\n\
sampler2D sampler_0: register(s0);\n\
\n\
void main(\n\
	float4 in_color : COLOR,\n\
	float2 in_tc_0 : TEXCOORD0,\n\
	out float4 out_color : COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = tex2D(sampler_0, in_tc_0) * in_color;\n\
}\n\
\
");

			_ps_max = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, 
"\
\n\
sampler2D sampler_0: register(s0);\n\
\n\
void main(\n\
	float4 in_color : COLOR,\n\
	float2 in_tc_0 : TEXCOORD0,\n\
	out float4 out_color : COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = tex2D(sampler_0, in_tc_0) * in_color;\n\
	out_color.xyz *= out_color.w;\n\
}\n\
\
");
			return _vs_screen && _vs_world && _ps && _ps_max;
		}
		const AABB&					cgBillBoardFX::local_bound()
		{
			return _local_bound;
		}
		bool						cgBillBoardFX::renderable() const
		{
			return true;
		}
		void						cgBillBoardFX::preRender(SRenderInfo* ri)
		{
			if(!_array_item.size())
				return;

			ri->regTransparentItem(this, 0, -32766);
		}
		void						cgBillBoardFX::render(intptr_t parameter, SRenderInfo* ri)
		{
			Gfx* gfx = Gfx::current();

			SRenderParamQuery param(ri);

			rsShade rs(false);

			rs.reset(false);
			param.preSetupShadeState(rs);

			rs.setSampler(0);
			rs.enableBlend(GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR_ONE, GfxEnum::BLENDOPERATION_ADD);
			rs.enableDepthTest(GfxEnum::CMPFUNC_LESS);

			rs.enableAlphaTest(GfxEnum::CMPFUNC_GREATER, 0.0f);

			Jotter jotter;

			Z_ASSERT( _vs_screen && _vs_world && _ps );

			ShaderUniforms2 unis(NULL, NULL);

			Mat4 inv_view = gfx->getViewTransform().inverse();

			Vec3 cam_pos(inv_view[3]);
			Vec3 cam_right(inv_view[0]);
			Vec3 cam_up(inv_view[1]);

			Item::ALIGN last_align = Item::ALIGN_SCREEN;
			Item::BLEND last_blend = Item::BLEND_ADDITIVE;
			SPtr<Image> last_image;

			for(size_t i = 0; i < _array_item.size(); ++i)
			{
				const WPtr<Item>& wptr = _array_item[i];
				if(!wptr.valid())
				{
					_array_item.erase(_array_item.begin() + i);
					--i;
					continue;
				}

				const SPtr<Item>& item = wptr.lock();

				if(i == 0 || item->align != last_align || item->blend != last_blend || item->image != last_image)
				{
					if(i && jotter.vertices())
						gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, jotter);

					switch(item->align)
					{
					case Item::ALIGN_SCREEN:
						gfx->useProgrammablePipeline(_vs_screen, (item->blend == Item::BLEND_MAX) ? _ps_max : _ps);
						unis.reset(_vs_screen, (item->blend == Item::BLEND_MAX) ? _ps_max : _ps);
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_right", cam_right);
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_up", cam_up);
						break;
					case Item::ALIGN_WORLD:
						gfx->useProgrammablePipeline(_vs_world, (item->blend == Item::BLEND_MAX) ? _ps_max : _ps);
						unis.reset(_vs_world, (item->blend == Item::BLEND_MAX) ? _ps_max : _ps);
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_pos", cam_pos);
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_right", cam_right);
						unis.set(GfxEnum::SHADERTYPE_VERTEX, "cam_up", cam_up);
						break;
					default:
						Z_ASSERT( false );
						break;
					}

					//unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getViewTransform() * gfx->adjustProjectionTransform(gfx->getUnadjustedProjectionTransform()));
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedViewProjectionTransform());
					switch(item->blend)
					{
					case Item::BLEND_ADDITIVE:
						rs.enableBlend(GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR_ONE, GfxEnum::BLENDOPERATION_ADD);
						break;
					case Item::BLEND_DECAL:
						rs.enableBlend(GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GfxEnum::BLENDOPERATION_ADD);
						break;
					case Item::BLEND_MAX:
						rs.enableBlend(GfxEnum::BLENDFACTOR_ONE, GfxEnum::BLENDFACTOR_ONE, GfxEnum::BLENDOPERATION_MAX);
						break;
					default:
						Z_ASSERT( false );
						break;
					}

					param.postSetupShadeState(rs);

					gfx->setShadeState(rs);

					gfx->setShaderUniforms(unis);

					gfx->bindTexture(0, item->image->_tex);

					last_align = item->align;
					last_blend = item->blend;
					last_image = item->image;

					jotter.begin(3, true, true, 1, 2);
				}

				const Image::FRAME& f = last_image->_array_frame[min(max((size_t)0, (size_t)(item->anim * last_image->_frames)), last_image->_frames - 1)];

				jotter.color(item->color);

				const Vec2& ext = item->extent;
				const Vec3& pos = item->pos;

				jotter.normal(-ext.x, ext.y, 0);
				jotter.tc(0, f.lt[0], f.lt[1]);
				jotter.vertex(pos);

				jotter.normal(-ext.x, -ext.y, 0);
				jotter.tc(0, f.lt[0], f.rb[1]);
				jotter.vertex(pos);

				jotter.normal(ext.x, ext.y, 0);
				jotter.tc(0, f.rb[0], f.lt[1]);
				jotter.vertex(pos);

				///////////////////////////////////////////////////////////

				jotter.normal(ext.x, ext.y, 0);
				jotter.tc(0, f.rb[0], f.lt[1]);
				jotter.vertex(pos);

				jotter.normal(-ext.x, -ext.y, 0);
				jotter.tc(0, f.lt[0], f.rb[1]);
				jotter.vertex(pos);

				jotter.normal(ext.x, -ext.y, 0);
				jotter.tc(0, f.rb[0], f.rb[1]);
				jotter.vertex(pos);
			}

			if(jotter.vertices())
				gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, jotter);
		}
	};
};


