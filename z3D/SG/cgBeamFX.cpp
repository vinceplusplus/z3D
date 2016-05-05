
#include "cgBeamFX.h"
#include "RenderInfo.h"

namespace z3D
{
	namespace SG
	{
		cgBeamFX::cgBeamFX(const AABB& local_bound) : _local_bound(local_bound)
		{
		}
		cgBeamFX::~cgBeamFX() {}
		void					cgBeamFX::setBeamTextures(const SPtr<Tex2D>& beam, const SPtr<Tex2D>& beam_core)
		{
			_tex_beam = beam;
			_tex_beam_core = beam_core;
		}
		bool					cgBeamFX::loadShaders()
		{
			_vs = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, 
"\
\n\
float4x4 wvp;\n\
\n\
void main(\n\
	float4 in_pos : POSITION,\n\
	out float4 out_pos : POSITION,\n\
	float4 in_color : COLOR,\n\
	out float4 out_color : COLOR,\n\
	float2 in_tc_0 : TEXCOORD0,\n\
	out float2 out_tc_0 : TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_color = in_color;\n\
	out_tc_0 = in_tc_0;\n\
}\n\
\
");

			_ps = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, 
"\
\n\
sampler2D sampler_0: register(s0);\n\
sampler2D sampler_1: register(s1);\n\
\n\
void main(\n\
	float4 in_color : COLOR,\n\
	float2 in_tc_0 : TEXCOORD0,\n\
	out float4 out_color : COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = float4(tex2D(sampler_0, in_tc_0).xyz * in_color.xyz + tex2D(sampler_1, in_tc_0).xyz, in_color.w);\n\
}\n\
\n\
\n\
\
");
				return (_vs && _ps);
		}
		SPtr<cgBeamFX::Item>	cgBeamFX::createBeam(const Vec3& from, const Vec3& to, const Vec4& color, REAL radius)
		{
			SPtr<Item> i = new Item();
			i->from = from;
			i->to = to;
			i->color = color;
			i->radius = radius;

			_array_item.push_back(i);

			return i;
		}
		const AABB&				cgBeamFX::local_bound()
		{
			return _local_bound;
		}
		bool					cgBeamFX::renderable() const
		{
			return true;
		}
		void					cgBeamFX::preRender(SRenderInfo* ri)
		{
			if(!_array_item.size())
				return;

			ri->regTransparentItem(this, 0, -32765);
		}
		void					cgBeamFX::render(intptr_t parameter, SRenderInfo* ri)
		{
			Gfx* gfx = Gfx::current();

			SRenderParamQuery param(ri);

			rsShade rs(false);

			rs.reset(false);
			param.preSetupShadeState(rs);

			rs.setSampler(0);
			rs.setSampler(1);
			rs.enableBlend(GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR_ONE, GfxEnum::BLENDOPERATION_ADD);
			rs.enableDepthTest(GfxEnum::CMPFUNC_LESS);

			rs.enableAlphaTest(GfxEnum::CMPFUNC_GREATER, 0.0f);

			Jotter jotter;

			Z_ASSERT( _vs && _ps );

			gfx->useProgrammablePipeline(_vs, _ps);

			param.postSetupShadeState(rs);

			gfx->setShadeState(rs);

			gfx->bindTexture(0, _tex_beam);
			gfx->bindTexture(1, _tex_beam_core);

			ShaderUniforms2 unis(_vs, _ps);

			//unis.clear();
			//unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getViewTransform() * gfx->adjustProjectionTransform(gfx->getUnadjustedProjectionTransform()));
			unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedViewProjectionTransform());

			gfx->setShaderUniforms(unis);

			jotter.begin(3, false, true, 1);

			Vec3 cam_pos(gfx->getViewTransform().inverse()[3]);

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

				Vec3 from = item->from;
				Vec3 to = item->to;

				Vec3 dir = (to - from).normal_safe();
				Vec3 cam_vec = (from - cam_pos).normal_safe();
				if(cam_vec.lengthsq() < 0.01f)
					cam_vec = (to - cam_pos).normal_safe();

				Vec3 right = (cam_vec ^ dir).normal_safe();
				if(right.lengthsq() < 0.01f)		// parallel case, ignore
					continue;

				right *= item->radius;

				jotter.color(item->color);

				jotter.tc(0, 0.0f, 1.0f);
				jotter.vertex(to - right);
				jotter.tc(0, 0.0f, 0.0f);
				jotter.vertex(from - right);
				jotter.tc(0, 1.0f, 1.0f);
				jotter.vertex(to + right);

				jotter.tc(0, 1.0f, 1.0f);
				jotter.vertex(to + right);
				jotter.tc(0, 0.0f, 0.0f);
				jotter.vertex(from - right);
				jotter.tc(0, 1.0f, 0.0f);
				jotter.vertex(from + right);
			}

			if(jotter.vertices())
				gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, jotter);
		}
	};
};


