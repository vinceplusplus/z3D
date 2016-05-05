
#include "MeshRenderer.h"
#include "MeshInst.h"
#include "Mesh.h"
#include "SMgr.h"
#include "SNode.h"
#include "SCameraNode.h"
#include "SLightNode.h"

namespace z3D
{
	namespace SG
	{
		DefMeshRenderer::~DefMeshRenderer()
		{}
		const SPtr<DefMeshRenderer>&		DefMeshRenderer::instance()
		{
			if(!_instance)
				_instance = new DefMeshRenderer();
			return _instance;
		}
		void								DefMeshRenderer::draw(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param)
		{
			Gfx* gfx = Gfx::current();

			const SubMesh& sub = mesh->getSubMesh(submesh);
			Mtl& mtl = (Mtl&)mesh->data.mtls.get()[sub.mtl_index % mesh->data.mtls.get().size()];

			// 4)
			//gfx->useFixedPipeline();

			rsShade rs(false);

			param.preSetupShadeState(rs);

			if(mtl.two_sided)
				rs.disableCull();

			if(mtl.blend.enabled)
				rs.enableBlend(mtl.blend.sbf, mtl.blend.dbf, mtl.blend.op);

			ShaderGenConf cfg;

			cfg.begin(param.getWorldTransform());

			Vec3 control_color = (Vec3)param.getControlColor();

			cfg.setMaterial(mtl.ambient.modulate(control_color), mtl.diffuse.modulate(control_color), mtl.computeSpecular().modulate(control_color), mtl.computeShininess(), mtl.opacity * param.getControlColor().w);

			bool selfill_mapping = (mtl.map_selfill.tex && mtl.map_selfill.tex->tex_target() == GfxEnum::TEXTARGET_2D && mesh->has_mchan(mtl.map_selfill.channel));

			bool lighting = !selfill_mapping && mtl.lighting;

			bool per_pixel_lighting = false;

			bool normal_mapping = false;

			bool do_shadow = param.recvShadow() && param.getLightsVisibility();

			if(!gfx->supportsShaderModel3())
				lighting = false;

			if(lighting)
				per_pixel_lighting = true;

			if(per_pixel_lighting && (mtl.map_normal.tex && mtl.map_normal.tex->tex_target() == GfxEnum::TEXTARGET_2D && mesh->has_mchan(mtl.map_normal.channel)))
				normal_mapping = true;

			if(per_pixel_lighting)
			{
				cfg.enablePerPixelLighting();
				if(mtl.two_sided)
					cfg.enableTwoSidedLighting();
			}

			if(normal_mapping)
				cfg.enableNormalMapping(mtl.map_normal.computeTransform());

			if(!lighting)
			{
				cfg.setLightAmbient(Vec3(1, 1, 1));
			}
			else
			{
				Vec3 ambient;
				RenderParamQuery::LIGHTINFO array_lightinfo[8];
				size_t light_count;

				param.getLightingInfo(ambient, _countof(array_lightinfo), array_lightinfo, light_count);

				cfg.setLightAmbient(ambient);

				for(size_t i = 0; i < light_count; ++i)
				{
					const RenderParamQuery::LIGHTINFO& l = array_lightinfo[i];
					ptrdiff_t visibility_channel = do_shadow ? l.visibility_channel : -1;
					switch(l.type)
					{
					case GfxEnum::LIGHTTYPE_DIRECTIONAL:
						cfg.addDirLight(l.color, l.dir, visibility_channel);
						break;
					case GfxEnum::LIGHTTYPE_POINT:
						cfg.addPointLight(l.color, l.pos, l.range, visibility_channel);
						break;
					case GfxEnum::LIGHTTYPE_SPOT:
						cfg.addSpotLight(l.color, l.pos, l.dir, l.inner_cone, l.outer_cone, l.range, visibility_channel);
						break;
					}
				}
			}

			if(selfill_mapping)
			{
				setSamplerParam(rs, 1, mtl.map_selfill);

				cfg.useTexCoord(1, mtl.map_selfill.computeTransform());
				cfg.useTexSampler(1, GfxEnum::TEXTARGET_2D);
				cfg.fetchTex(1, 1);

				cfg.addPSCode("	in_diff.xyz *= tf_1_1.xyz;\n");
			}

			bool diffuse_mapping = (mtl.map_diffuse.tex && mtl.map_diffuse.tex->tex_target() == GfxEnum::TEXTARGET_2D && mesh->has_mchan(mtl.map_diffuse.channel));
			if(diffuse_mapping)
			{
				if(mtl.alpha_test.enabled == 1 || (mtl.alpha_test.enabled == 2 && (!mtl.map_diffuse.tex->knownOpaque() || mtl.opacity != 1.0f)))
					rs.enableAlphaTest(mtl.alpha_test.func, mtl.alpha_test.ref);

				setSamplerParam(rs, 0, mtl.map_diffuse);

				cfg.useTexCoord(0, mtl.map_diffuse.computeTransform());
				cfg.useTexSampler(0, GfxEnum::TEXTARGET_2D);
				cfg.fetchTex(0, 0);

				if(mtl.sep_opacity_mapping && mesh->has_mchan(mtl.map_opacity.channel))
				{
					cfg.useTexCoord(4, mtl.map_opacity.computeTransform());
					cfg.useTexSampler(0, GfxEnum::TEXTARGET_2D);
					cfg.fetchTex(0, 4);

					cfg.addPSCode("	in_diff.xyz *= tf_0_0.xyz;\n");
					cfg.addPSCode("	in_diff.w *= tf_0_4;\n");
				}
				else
				{
					cfg.addPSCode("	in_diff *= tf_0_0;\n");
				}
			}

			bool env_mapping = !!(mtl.map_env.tex && (mtl.map_env.tex->tex_target() == GfxEnum::TEXTARGET_2D || mtl.map_env.tex->tex_target() == GfxEnum::TEXTARGET_CUBE));
			if(env_mapping)
			{
				setSamplerParam(rs, 3, mtl.map_env);

				if(mtl.map_env.tex->tex_target() == GfxEnum::TEXTARGET_2D)
				{
					cfg.useTexCoord(3, ShaderGenConf::TCGEN_SPHERE_MAPPING);
					cfg.useTexSampler(3, GfxEnum::TEXTARGET_2D);
				}
				else if(mtl.map_env.tex->tex_target() == GfxEnum::TEXTARGET_CUBE)
				{
					cfg.useTexCoord(3, ShaderGenConf::TCGEN_CUBE_MAPPING);
					cfg.useTexSampler(3, GfxEnum::TEXTARGET_CUBE);
				}
				cfg.fetchTex(3, 3);

				cfg.addPSCode("	in_spec += tf_3_3.xyz;\n");
			}

			bool specular_mapping = (mtl.map_specular.tex && mtl.map_specular.tex->tex_target() == GfxEnum::TEXTARGET_2D && mesh->has_mchan(mtl.map_specular.channel));
			if(specular_mapping)
			{
				setSamplerParam(rs, 2, mtl.map_specular);

				cfg.useTexCoord(2, mtl.map_specular.computeTransform());
				cfg.useTexSampler(2, GfxEnum::TEXTARGET_2D);
				cfg.fetchTex(2, 2);

				cfg.addPSCode("	in_spec *= tf_2_2.xyz;\n");
			}

			if(normal_mapping)
			{
				setSamplerParam(rs, 8, mtl.map_normal);
			}

			if(do_shadow)
			{
				rs.setSampler(9, GfxEnum::FILTERMODE_NEAREST, GfxEnum::FILTERMODE_NEAREST, GfxEnum::FILTERMODE_NEAREST, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);
			}

			cfg.addPSCode("	out_color = in_diff + float4(in_spec, 0);\n");
			bool bind_norm_vbo = true;							// very complicated to determine

			ShaderGen* gen = gfx->getShaderGen();
			SPtr<Shader> vs, ps;
			gfx->useProgrammablePipeline(vs = gen->genVS(cfg), ps = gen->genPS(cfg));

			ShaderUniforms2 unis(vs, ps);
			gen->setShaderUniforms(cfg, unis, true, true);

			//// need to adjust some code in shader gen to test, ie adding semantic
			//class ExampleSemanticizedConstantSetter
			//{
			//private:
			//	RenderParamQuery&			_query;
			//public:
			//	ExampleSemanticizedConstantSetter(RenderParamQuery& query)
			//		: _query(query)
			//	{}
			//public:
			//	bool						set(ShaderUniforms2& unis, GfxEnum::SHADERTYPE shadertype, const mbs& name, const Shader::SEMANTICIZED_CONSTANT_DESC& cd)
			//	{
			//		switch(cd.semantic)
			//		{
			//		case GfxEnum::SCS_WORLDVIEWPROJECTION:
			//			{
			//				unis.set(shadertype, cd, _query.getWorldTransform() * Gfx::current()->getAdjustedViewProjectionTransform());
			//			}
			//			break;
			//		default:
			//			{
			//				return false;
			//			}
			//			break;
			//		}
			//		return true;
			//	}
			//};
			//unis.setSemanticizedConstants(ExampleSemanticizedConstantSetter(param));

			gfx->setShaderUniforms(unis);

			param.postSetupShadeState(rs);

			gfx->setShadeState(rs);

			if(diffuse_mapping)
				gfx->bindTexture(0, mtl.map_diffuse.tex);

			if(selfill_mapping)
				gfx->bindTexture(1, mtl.map_selfill.tex);

			if(specular_mapping)
				gfx->bindTexture(2, mtl.map_specular.tex);

			if(env_mapping)
				gfx->bindTexture(3, mtl.map_env.tex);

			if(normal_mapping)
				gfx->bindTexture(8, mtl.map_normal.tex);

			if(do_shadow)
				gfx->bindTexture(9, param.getLightsVisibility());

			gfx->bindVBO(mesh->get_vbo_vert(), sizeof(Vec3), sub.face_start * 3 * sizeof(Vec3));
			gfx->bindVertexPosition(0, 3);
			if(bind_norm_vbo)
			{
				gfx->bindVBO(mesh->get_vbo_norm(), sizeof(Vec3), sub.face_start * 3 * sizeof(Vec3));
				gfx->bindVertexNormal(0);
			}
			if(diffuse_mapping)
			{
				gfx->bindVBO(mesh->get_vbo_mvert(mtl.map_diffuse.channel), sizeof(Vec2), sub.face_start * 3 * sizeof(Vec2));
				gfx->bindVertexTexCoord(0, 0, 2);
			}
			if(selfill_mapping)
			{
				gfx->bindVBO(mesh->get_vbo_mvert(mtl.map_selfill.channel), sizeof(Vec2), sub.face_start * 3 * sizeof(Vec2));
				gfx->bindVertexTexCoord(0, 1, 2);
			}
			if(specular_mapping)
			{
				gfx->bindVBO(mesh->get_vbo_mvert(mtl.map_specular.channel), sizeof(Vec2), sub.face_start * 3 * sizeof(Vec2));
				gfx->bindVertexTexCoord(0, 2, 2);
			}
			if(normal_mapping)
			{
				gfx->bindVBO(mesh->get_vbo_tangent(mtl.map_normal.channel), sizeof(Vec4), sub.face_start * 3 * sizeof(Vec4));
				gfx->bindVertexTexCoord(0, 10, 4);

				gfx->bindVBO(mesh->get_vbo_mvert(mtl.map_normal.channel), sizeof(Vec2), sub.face_start * 3 * sizeof(Vec2));
				gfx->bindVertexTexCoord(0, 11, 2);
			}

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, 0, sub.face_count);

			// debug, draw normals
			if(false)
			{
				rs.reset(false);
				param.preSetupShadeState(rs);

				cfg.begin(param.getWorldTransform());

				cfg.setMaterial(Vec3(1, 1, 1), Vec3::zero, Vec3::zero, 0, 1.0f);
				cfg.setLightAmbient(Vec3(1, 1, 1));

				cfg.addPSCode("	out_color = in_diff + float4(in_spec, 0);\n");

				ShaderGen* gen = gfx->getShaderGen();
				SPtr<Shader> vs, ps;
				gfx->useProgrammablePipeline(vs = gen->genVS(cfg), ps = gen->genPS(cfg));

				ShaderUniforms2 unis(vs, ps);
				gen->setShaderUniforms(cfg, unis, true, true);

				gfx->setShaderUniforms(unis);

				param.postSetupShadeState(rs);

				gfx->setShadeState(rs);

				Jotter jotter;
				jotter.begin(3, false, false, 0);

				const vnFace* faces = mesh->get_faces();
				const Vec3* verts = mesh->get_verts();
				const Vec3* norms = mesh->get_norms();

				size_t face_end = sub.face_start + sub.face_count;
				for(size_t i = sub.face_start; i < face_end; ++i)
				{
					const vnFace& f = faces[i];
					for(size_t j = 0; j < 3; ++j)
					{
						const Vec3& v = verts[f.v[j]];
						jotter.vertex(v);
						jotter.vertex(v + norms[f.n[j]] * 10);
					}
				}

				gfx->drawPrimitives(GfxEnum::PRIMTYPE_LINES, jotter);
			}
		}
		void								DefMeshRenderer::draw_depth_only(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param)
		{
			Gfx* gfx = Gfx::current();

			const SubMesh& sub = mesh->getSubMesh(submesh);
			Mtl& mtl = (Mtl&)mesh->data.mtls.get()[sub.mtl_index % mesh->data.mtls.get().size()];

			// 4)
			//gfx->useFixedPipeline();

			rsShade rs(false);

			param.preSetupShadeState(rs);

			if(mtl.two_sided)
				rs.disableCull();

			ShaderGenConf cfg;

			cfg.begin(param.getWorldTransform());

			cfg.setMaterial(mtl.ambient, mtl.diffuse, mtl.computeSpecular(), mtl.computeShininess(), mtl.opacity);

			cfg.setLightAmbient(Vec3(1, 1, 1));

			bool diffuse_mapping = (mtl.map_diffuse.tex && mtl.map_diffuse.tex->tex_target() == GfxEnum::TEXTARGET_2D && mesh->has_mchan(mtl.map_diffuse.channel));
			if(diffuse_mapping)
			{
				diffuse_mapping = false;
				if(mtl.alpha_test.enabled == 2)
				{
					if(!mtl.map_diffuse.tex->knownOpaque() || mtl.opacity != 1.0f)
						diffuse_mapping = true;
				}
				else
					diffuse_mapping = !!mtl.alpha_test.enabled;
			}
			if(diffuse_mapping)
			{
				rs.enableAlphaTest(mtl.alpha_test.func, mtl.alpha_test.ref);

				setSamplerParam(rs, 0, mtl.map_diffuse);

				cfg.useTexCoord(0, mtl.map_diffuse.computeTransform());
				cfg.useTexSampler(0, GfxEnum::TEXTARGET_2D);
				cfg.fetchTex(0, 0);

				cfg.addPSCode("	out_color = tf_0_0;\n");
			}
			else
				cfg.addPSCode("	out_color = float4(1, 1, 1, 1);\n");

			ShaderGen* gen = gfx->getShaderGen();
			SPtr<Shader> vs, ps;
			gfx->useProgrammablePipeline(vs = gen->genVS(cfg), ps = gen->genPS(cfg));

			ShaderUniforms2 unis(vs, ps);
			gen->setShaderUniforms(cfg, unis, true, true);

			gfx->setShaderUniforms(unis);

			param.postSetupShadeState(rs);

			gfx->setShadeState(rs);

			if(diffuse_mapping)
				gfx->bindTexture(0, mtl.map_diffuse.tex);

			gfx->bindVBO(mesh->get_vbo_vert(), sizeof(Vec3), sub.face_start * 3 * sizeof(Vec3));
			gfx->bindVertexPosition(0, 3);

			if(diffuse_mapping)
			{
				gfx->bindVBO(mesh->get_vbo_mvert(mtl.map_diffuse.channel), sizeof(Vec2), sub.face_start * 3 * sizeof(Vec2));
				gfx->bindVertexTexCoord(0, 0, 2);
			}

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, 0, sub.face_count);
		}
		void								DefMeshRenderer::draw_depth_only_vsm(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param)
		{
			Gfx* gfx = Gfx::current();

			const SubMesh& sub = mesh->getSubMesh(submesh);
			Mtl& mtl = (Mtl&)mesh->data.mtls.get()[sub.mtl_index % mesh->data.mtls.get().size()];

			rsShade rs(false);

			param.preSetupShadeState(rs);

			if(mtl.two_sided)
				rs.disableCull();

			SPtr<Shader> vs, ps;

			bool diffuse_mapping = (mtl.map_diffuse.tex && mtl.map_diffuse.tex->tex_target() == GfxEnum::TEXTARGET_2D && mesh->has_mchan(mtl.map_diffuse.channel));
			if(diffuse_mapping)
			{
				diffuse_mapping = false;
				if(!mtl.map_diffuse.tex->knownOpaque() || mtl.opacity != 1.0f)
					diffuse_mapping = true;
			}
			if(diffuse_mapping)
				setSamplerParam(rs, 0, mtl.map_diffuse);

			gfx->getVSMScreenDepth(diffuse_mapping, vs, ps);

			gfx->useProgrammablePipeline(vs, ps);

			ShaderUniforms2 unis(vs, ps);

			unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", param.getWorldTransform() * gfx->getAdjustedViewProjectionTransform());
			if(diffuse_mapping)
			{
				unis.set(GfxEnum::SHADERTYPE_VERTEX, "xform_tc", mtl.map_diffuse.computeTransform());
				unis.set(GfxEnum::SHADERTYPE_PIXEL, "alpha_ref", mtl.alpha_test.ref);
			}

			gfx->setShaderUniforms(unis);

			param.postSetupShadeState(rs);

			gfx->setShadeState(rs);

			if(diffuse_mapping)
				gfx->bindTexture(0, mtl.map_diffuse.tex);

			gfx->bindVBO(mesh->get_vbo_vert(), sizeof(Vec3), sub.face_start * 3 * sizeof(Vec3));
			gfx->bindVertexPosition(0, 3);

			if(diffuse_mapping)
			{
				gfx->bindVBO(mesh->get_vbo_mvert(mtl.map_diffuse.channel), sizeof(Vec2), sub.face_start * 3 * sizeof(Vec2));
				gfx->bindVertexTexCoord(0, 0, 2);
			}

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, 0, sub.face_count);
		}
		void								DefMeshRenderer::draw_gen_shadow_map_vsm(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param)
		{
			Gfx* gfx = Gfx::current();

			const SubMesh& sub = mesh->getSubMesh(submesh);
			Mtl& mtl = (Mtl&)mesh->data.mtls.get()[sub.mtl_index % mesh->data.mtls.get().size()];

			rsShade rs(false);

			param.preSetupShadeState(rs);

			if(mtl.two_sided)
				rs.disableCull();

			SPtr<Shader> vs, ps;

			bool diffuse_mapping = (mtl.map_diffuse.tex && mtl.map_diffuse.tex->tex_target() == GfxEnum::TEXTARGET_2D && mesh->has_mchan(mtl.map_diffuse.channel));
			if(diffuse_mapping)
			{
				diffuse_mapping = false;
				if(!mtl.map_diffuse.tex->knownOpaque() || mtl.opacity != 1.0f)
					diffuse_mapping = true;
			}
			if(diffuse_mapping)
				setSamplerParam(rs, 0, mtl.map_diffuse);

			gfx->getVSMGenShadowMap(diffuse_mapping, vs, ps);

			gfx->useProgrammablePipeline(vs, ps);

			ShaderUniforms2 unis(vs, ps);

			unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", param.getWorldTransform() * gfx->getAdjustedViewProjectionTransform());
			const Mat4& m = gfx->getUnadjustedProjectionTransform();
			const REAL F = gfx->getZFarMapping();
			REAL zfar = -(-m.a43 + F * m.a44) / (-m.a33 + F * m.a34);
			unis.set(GfxEnum::SHADERTYPE_VERTEX, "depth_space", *(Vec4*)((param.getWorldTransform() * gfx->getViewTransform() * Mat4::scaling(1, 1, -1 / -zfar)).transpose()[2]));
			if(diffuse_mapping)
			{
				unis.set(GfxEnum::SHADERTYPE_VERTEX, "xform_tc", mtl.map_diffuse.computeTransform());
				unis.set(GfxEnum::SHADERTYPE_PIXEL, "alpha_ref", mtl.alpha_test.ref);
			}

			gfx->setShaderUniforms(unis);

			param.postSetupShadeState(rs);

			gfx->setShadeState(rs);

			if(diffuse_mapping)
				gfx->bindTexture(0, mtl.map_diffuse.tex);

			gfx->bindVBO(mesh->get_vbo_vert(), sizeof(Vec3), sub.face_start * 3 * sizeof(Vec3));
			gfx->bindVertexPosition(0, 3);

			if(diffuse_mapping)
			{
				gfx->bindVBO(mesh->get_vbo_mvert(mtl.map_diffuse.channel), sizeof(Vec2), sub.face_start * 3 * sizeof(Vec2));
				gfx->bindVertexTexCoord(0, 0, 2);
			}

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, 0, sub.face_count);
		}
		void								DefMeshRenderer::setSamplerParam(rsShade& rs, size_t sampler, const MtlMap& mtlmap)
		{
			rs.setSampler(sampler, mtlmap.filter_min, mtlmap.filter_mag, mtlmap.filter_mip, mtlmap.wrap_u, mtlmap.wrap_v, mtlmap.wrap_w);
		}
	};
};


