
#include "SMgr.h"
#include "SNode.h"
#include "SCameraNode.h"
#include "SLightNode.h"

namespace z3D
{
	namespace SG
	{
		void						SMgr::render(SRenderInfo* ri)
		{
			render_normal(ri);

			//DRAWCONTROL dc=current_render_info()->dc;
			//if(dc == DRAWCONTROL_NORMAL)
			//{
			//	render_normal();
			//}
			//else if(dc == DRAWCONTROL_GENERATE_SHADOWMAP)
			//{
			//	render_generate_shadowmap();
			//}
			//else if(dc == DRAWCONTROL_DIR_SHADOWMAPPING_FILL_SHADOW)
			//{
			//	render_dir_shadowmapping_fill_shadow();
			//}
			//else if(dc == DRAWCONTROL_POINT_SHADOWMAPPING_FILL_SHADOW)
			//{
			//	render_dir_shadowmapping_fill_shadow();
			//}
			//else if(dc == DRAWCONTROL_SPOT_SHADOWMAPPING_FILL_SHADOW)
			//{
			//	render_dir_shadowmapping_fill_shadow();
			//}
		}
		void						SMgr::render_normal(SRenderInfo* ri)
		{
			Gfx* gfx = Gfx::current();

			rsView rs;
			rs.init(gfx);

			gfx->setViewState(rs);
			gfx->setViewProjTransform(ri->getCamera()->view(), ri->getCamera()->proj());

			//// setting solid states
			//gfx->setWorldMatrix(Mat4::identity);
			//gfx->setViewMatrix(Mat4::identity);
			//gfx->setProjectionMatrix(Mat4::identity);

			//gfx->RS_setShade(Gfx::SHADEMODE_GOURAUD);
			//gfx->RS_setAlpha(Gfx::CMPFUNC_GREATER,0.5f);
			//gfx->RS_setBlendRGB(Gfx::BLENDFACTOR_SRC_ALPHA,Gfx::BLENDFACTOR_ONE_MINUS_SRC_ALPHA);
			//gfx->RS_setBlendAlpha(Gfx::BLENDFACTOR_ONE,Gfx::BLENDFACTOR_ZERO);
			//gfx->RS_setBlendOpRGB(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setBlendOpAlpha(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setDepth(Gfx::CMPFUNC_LESS);
			//Gfx::CULLMODE cullmode=Gfx::CULLMODE_CW;
			//if(gfx->boundRenderTarget())
			//	cullmode = Gfx::reverseCullMode(cullmode);
			//if(SMgr::current()->current_render_info()->inside_reflection)
			//	cullmode = Gfx::reverseCullMode(cullmode);
			//// more reverse if any, e.g. reflection
			//gfx->RS_setCullMode(cullmode);
			//gfx->RS_setPolygonMode(Gfx::POLYGONMODE_FILL);
			//gfx->RS_setStencil(Gfx::CMPFUNC_ALWAYS,0,~0);
			//gfx->RS_setStencilOp(Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP);
			//gfx->RS_setLightAmbient(Vec4(0,0,0,1));
			//gfx->RS_setClipPlanes(current_camera()->additionalClipPlane());

			//gfx->RS_enableWriteColor(true,true,true,true);
			//gfx->RS_enableWriteDepth(true);
			//gfx->RS_enableWriteStencil(false);

			//gfx->RS_enableDepthTest(true);
			//gfx->RS_enableFog(false);
			//gfx->RS_enableScissorTest(false);
			//gfx->RS_enableAlphaTest(true);
			//gfx->RS_enableBlend(false);
			//gfx->RS_enableStencil(false);
			//gfx->RS_enableLighting(false);
			//gfx->RS_enableSpecular(true);

			//gfx->RS_setTSS_color_op(0,Gfx::TEXTUREOP_DISABLE);
			//gfx->RS_setTSS_alpha_op(0,Gfx::TEXTUREOP_DISABLE);

			//// must be set, otherwise clip planes are incorrectly setup
			//gfx->setViewMatrix(current_camera()->view());
			//gfx->setProjectionMatrix(current_camera()->proj());

			// render solid

			const SRenderInfo::SceneInput& input = ri->getSceneInput();

			switch(ri->getDrawPass())
			{
			case DRAWPASS_DEPTH_ONLY:
			case DRAWPASS_SHADE:
			case DRAWPASS_SHADE_EQUAL_DEPTH:
			case DRAWPASS_DEPTH_ONLY_SOLID_RECV_SHADOW_VSM:
			case DRAWPASS_GEN_SHADOW_MAP_VSM:
				{
					ri->sortSolidItems();

					if(input.array_solid_item.size())
					{
						ri->setRenderingDrawControl(DRAWCONTROL_SOLID);

						const SRenderItem* end = VECTOR_END_PTR(input.array_solid_item);
						for(const SRenderItem* iter = VECTOR_BEGIN_PTR(input.array_solid_item); iter != end; ++iter)
						{
							const SRenderItem& info = *iter;
							ri->setRenderingNode(info.node);
							info.node->render(info.parameter, ri);
						}
					}
				}
				break;
			}

			switch(ri->getDrawPass())
			{
			case DRAWPASS_SHADE:
			case DRAWPASS_SHADE_EQUAL_DEPTH:
				{
					ri->sortTransparentItems(true, true);

					if(input.array_transparent_item.size())
					{
						ri->setRenderingDrawControl(DRAWCONTROL_TRANSPARENT);

						const SRenderItem* end = VECTOR_END_PTR(input.array_transparent_item);
						for(const SRenderItem* iter = VECTOR_BEGIN_PTR(input.array_transparent_item); iter != end; ++iter)
						{
							const SRenderItem& info = *iter;
							ri->setRenderingNode(info.node);
							info.node->render(info.parameter, ri);
						}
					}
				}
				break;
			}

			//// shadowmapping time
			//if(doUseShadow())
			//{
			//	if(_ri_current->dir_light_node.size())
			//	{
			//		SLightNode*const* end=VECTOR_END_PTR(_ri_current->dir_light_node);
			//		for(SLightNode*const* iter=&*_ri_current->dir_light_node.begin();iter!=end;++iter)
			//		{
			//			SLightNode* l=*iter;
			//			if(!l->getLightCastShadow())
			//				continue;

			//			SPtr<SCameraNode> cam=_stock_camera.alloc();
			//			SPtr<Tex2D> rt_float=_stock_float.alloc();
			//			SPtr<Tex2D> rt_depth=_stock_depth_for_rt.alloc();

			//			if(!rt_float || !rt_depth)
			//			{
			//				if(rt_float)
			//					_stock_float.dealloc(rt_float);
			//				if(rt_depth)
			//					_stock_depth_for_rt.dealloc(rt_depth);
			//				continue;
			//			}

			//			gfx->push_RT_n_DS();

			//			cam->setAutoAspectRatio(false);
			//			cam->setAspectRatio(1);
			//			cam->setAutoOrtho(false);
			//			cam->setWorldTransform(l->world_trans());
			//			cam->setOrtho(-l->getBeam(),l->getBeam(),l->getBeam(),-l->getBeam(),0,l->getDist());

			//			gfx->bindRenderTarget(rt_float);
			//			gfx->bindDepthStencil(rt_depth);

			//			gfx->push_Viewport2DClipRect();
			//			gfx->setViewport(true);
			//			gfx->set2DClipRect(false);

			//			clear_shadowmap();

			//			RenderInfo* ri=RenderInfo::alloc();
			//			ri->camera = cam.get();
			//			ri->inside_reflection = _ri_current->inside_reflection;

			//			drawAll(DRAWCONTROL_GENERATE_SHADOWMAP,ri);

			//			gfx->pop_Viewport2DClipRect();

			//			gfx->pop_RT_n_DS();

			//			ri->dealloc();

			//			_ri_current->dir_shadow_map = rt_float;
			//			_ri_current->dir_shadowmapping_view = cam->view();
			//			_ri_current->dir_shadowmapping_viewproj = cam->view_proj();

			//			_ri_current->dc = DRAWCONTROL_DIR_SHADOWMAPPING_FILL_SHADOW;

			//			render();

			//			_ri_current->dir_shadow_map.reset();

			//			_stock_depth_for_rt.dealloc(rt_depth);
			//			_stock_float.dealloc(rt_float);
			//			_stock_camera.dealloc(cam);

			//			_ri_current->dc = DRAWCONTROL_NORMAL;
			//		}
			//	}
			//}
			//gfx->RS_setDepth(Gfx::CMPFUNC_LESS);
			//gfx->RS_enableWriteDepth(false);
			//gfx->RS_enableDepthTest(true);
			//gfx->RS_setAlpha(Gfx::CMPFUNC_GREATER,0.0f);
			//gfx->RS_enableAlphaTest(false);
			//gfx->RS_enableBlend(true);
			//gfx->RS_enableLighting(false);
			//if(_ri_current->transparent_node.size())
			//{
			//	multimap<int,RenderNodeInfo>::iterator end=_ri_current->transparent_node.end();
			//	for(multimap<int,RenderNodeInfo>::iterator iter=_ri_current->transparent_node.begin();iter!=end;++iter)
			//	{
			//		const RenderNodeInfo& info=iter->second;
			//		_ri_current->setRenderingNode(info.node);
			//		info.node->render(info.parameter);
			//	}
			//}
			//if(!_ri_current->inside_reflection)
			//{
			//	if(_ri_current->reflection_node.size())
			//	{
			//		SPtr<SNode> last_node;
			//		SPtr<Tex2D> rt_rgb;

			//		const RenderNodeInfo* end=VECTOR_END_PTR(_ri_current->reflection_node);
			//		for(const RenderNodeInfo* iter=VECTOR_BEGIN_PTR(_ri_current->reflection_node);iter!=end;++iter)
			//		{
			//			if(iter->node != last_node)
			//			{
			//				//////// clean up ///////
			//				_ri_current->reflection_map.reset();
			//				if(rt_rgb)
			//				{
			//					_stock_rgb.dealloc(rt_rgb);
			//					rt_rgb.reset();
			//				}
			//				/////////////////////////

			//				SPtr<SCameraNode> cam=_stock_camera.alloc();
			//				rt_rgb=_stock_rgb.alloc();
			//				SPtr<Tex2D> rt_depth=_stock_depth_for_rt.alloc();

			//				if(!rt_rgb || !rt_depth)
			//				{
			//					if(rt_rgb)
			//						_stock_rgb.dealloc(rt_rgb);
			//					if(rt_depth)
			//						_stock_depth_for_rt.dealloc(rt_depth);
			//					continue;
			//				}

			//				Plane p=Plane(Vec3::zero,Vec3::yAxis).transformedBy(iter->node->world_trans());
			//				Mat4 m=Mat4::mirror(p);

			//				cam->copyCameraSettings(_ri_current->camera);
			//				cam->setAutoAspectRatio(false);
			//				cam->setAspectRatio(_ri_current->camera->aspect_ratio());
			//				cam->setWorldTransform(_ri_current->camera->world_trans()*m);
			//				cam->additionalClipPlane().push_back(p);

			//				gfx->push_RT_n_DS();

			//				gfx->bindRenderTarget(rt_rgb);
			//				gfx->bindDepthStencil(rt_depth);

			//				gfx->push_Viewport2DClipRect();
			//				gfx->setViewport(true);
			//				gfx->set2DClipRect(false);

			//				RenderInfo* ri=RenderInfo::alloc();
			//				ri->camera = cam.get();
			//				ri->inside_reflection = true;

			//				gfx->clear(true,true,false,Vec4::zero,gfx->getZFarMapping(),0);

			//				drawAll(DRAWCONTROL_NORMAL,ri);

			//				gfx->pop_Viewport2DClipRect();

			//				gfx->pop_RT_n_DS();

			//				ri->dealloc();

			//				_ri_current->reflection_map = rt_rgb;
			//				_ri_current->reflection_plane = p;
			//				_ri_current->reflection_matrix = m;

			//				_stock_depth_for_rt.dealloc(rt_depth);
			//				_stock_camera.dealloc(cam);

			//				last_node = iter->node;
			//			}

			//			_ri_current->dc = DRAWCONTROL_FILL_REFLECTION;

			//			render_fill_reflection(*iter);

			//			_ri_current->dc = DRAWCONTROL_NORMAL;
			//		}

			//		//////// clean up ///////
			//		_ri_current->reflection_map.reset();
			//		if(rt_rgb)
			//		{
			//			_stock_rgb.dealloc(rt_rgb);
			//			rt_rgb.reset();
			//		}
			//		/////////////////////////
			//	}
			//}
		}
		void						SMgr::render_generate_shadowmap()
		{
			//Gfx* gfx=Gfx::current();

			//gfx->setWorldMatrix(Mat4::identity);
			//gfx->setViewMatrix(Mat4::identity);
			//gfx->setProjectionMatrix(Mat4::identity);

			//gfx->RS_setShade(Gfx::SHADEMODE_GOURAUD);
			//gfx->RS_setAlpha(Gfx::CMPFUNC_GREATER,0.5f);
			//gfx->RS_setBlendRGB(Gfx::BLENDFACTOR_SRC_ALPHA,Gfx::BLENDFACTOR_ONE_MINUS_SRC_ALPHA);
			//gfx->RS_setBlendAlpha(Gfx::BLENDFACTOR_ONE,Gfx::BLENDFACTOR_ZERO);
			//gfx->RS_setBlendOpRGB(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setBlendOpAlpha(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setDepth(Gfx::CMPFUNC_LESS);
			//Gfx::CULLMODE cullmode=Gfx::CULLMODE_CW;
			//if(gfx->boundRenderTarget())
			//	cullmode = Gfx::reverseCullMode(cullmode);
			//if(_ri_current->inside_reflection)
			//	cullmode = Gfx::reverseCullMode(cullmode);
			//// more reverse if any, e.g. reflection
			//gfx->RS_setCullMode(cullmode);
			//gfx->RS_setPolygonMode(Gfx::POLYGONMODE_FILL);
			//gfx->RS_setStencil(Gfx::CMPFUNC_ALWAYS,0,~0);
			//gfx->RS_setStencilOp(Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP);
			//gfx->RS_setLightAmbient(Vec4(0,0,0,1));
			//gfx->RS_setClipPlanes(current_camera()->additionalClipPlane());

			//gfx->RS_enableWriteColor(true,true,true,true);
			//gfx->RS_enableWriteDepth(true);
			//gfx->RS_enableWriteStencil(false);

			//gfx->RS_enableDepthTest(true);
			//gfx->RS_enableFog(false);
			//gfx->RS_enableScissorTest(false);
			//gfx->RS_enableAlphaTest(true);
			//gfx->RS_enableBlend(false);
			//gfx->RS_enableStencil(false);
			//gfx->RS_enableLighting(false);
			//gfx->RS_enableSpecular(true);

			//gfx->RS_setTSS_color_op(0,Gfx::TEXTUREOP_DISABLE);
			//gfx->RS_setTSS_alpha_op(0,Gfx::TEXTUREOP_DISABLE);

			//// must be set, otherwise clip planes are incorrectly setup
			//gfx->setViewMatrix(current_camera()->view());
			//gfx->setProjectionMatrix(current_camera()->proj());

			//if(_ri_current->solid_unlit_node.size())
			//{
			//	const RenderNodeInfo* end=VECTOR_END_PTR(_ri_current->solid_unlit_node);
			//	for(const RenderNodeInfo* iter=&*_ri_current->solid_unlit_node.begin();iter!=end;++iter)
			//	{
			//		const RenderNodeInfo& info=*iter;
			//		_ri_current->setRenderingNode(info.node);
			//		info.node->render(info.parameter);
			//	}
			//}
		}
		void						SMgr::render_shadowmapping_fill_shadow()
		{
			//Gfx* gfx=Gfx::current();

			//gfx->setWorldMatrix(Mat4::identity);
			//gfx->setViewMatrix(Mat4::identity);
			//gfx->setProjectionMatrix(Mat4::identity);

			//gfx->RS_setShade(Gfx::SHADEMODE_GOURAUD);
			//gfx->RS_setAlpha(Gfx::CMPFUNC_GREATER,0.5f);
			//gfx->RS_setBlendRGB(Gfx::BLENDFACTOR_SRC_ALPHA,Gfx::BLENDFACTOR_ONE_MINUS_SRC_ALPHA);
			//gfx->RS_setBlendAlpha(Gfx::BLENDFACTOR_ONE,Gfx::BLENDFACTOR_ZERO);
			//gfx->RS_setBlendOpRGB(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setBlendOpAlpha(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setDepth(Gfx::CMPFUNC_EQUAL);
			//Gfx::CULLMODE cullmode=Gfx::CULLMODE_CW;
			//if(gfx->boundRenderTarget())
			//	cullmode = Gfx::reverseCullMode(cullmode);
			//if(_ri_current->inside_reflection)
			//	cullmode = Gfx::reverseCullMode(cullmode);
			//// more reverse if any, e.g. reflection
			//gfx->RS_setCullMode(cullmode);
			//gfx->RS_setPolygonMode(Gfx::POLYGONMODE_FILL);
			//gfx->RS_setStencil(Gfx::CMPFUNC_ALWAYS,0,~0);
			//gfx->RS_setStencilOp(Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP);
			//gfx->RS_setLightAmbient(Vec4(0,0,0,1));
			//gfx->RS_setClipPlanes(current_camera()->additionalClipPlane());

			//gfx->RS_enableWriteColor(true,true,true,true);
			//gfx->RS_enableWriteDepth(false);
			//gfx->RS_enableWriteStencil(false);

			//gfx->RS_enableDepthTest(true);
			//gfx->RS_enableFog(false);
			//gfx->RS_enableScissorTest(false);
			//gfx->RS_enableAlphaTest(false);
			//gfx->RS_enableBlend(true);
			//gfx->RS_enableStencil(false);
			//gfx->RS_enableLighting(false);
			//gfx->RS_enableSpecular(true);

			//gfx->RS_setTSS_color_op(0,Gfx::TEXTUREOP_DISABLE);
			//gfx->RS_setTSS_alpha_op(0,Gfx::TEXTUREOP_DISABLE);

			//// must be set, otherwise clip planes are incorrectly setup
			//gfx->setViewMatrix(current_camera()->view());
			//gfx->setProjectionMatrix(current_camera()->proj());

			//if(_ri_current->solid_unlit_node.size())
			//{
			//	const RenderNodeInfo* end=VECTOR_END_PTR(_ri_current->solid_unlit_node);
			//	for(const RenderNodeInfo* iter=&*_ri_current->solid_unlit_node.begin();iter!=end;++iter)
			//	{
			//		const RenderNodeInfo& info=*iter;
			//		if(!info.node->recvShadow())
			//			continue;
			//		_ri_current->setRenderingNode(info.node);
			//		info.node->render(info.parameter);
			//	}
			//}
			//if(_ri_current->solid_lit_node.size())
			//{
			//	const RenderNodeInfo* end=VECTOR_END_PTR(_ri_current->solid_lit_node);
			//	for(const RenderNodeInfo* iter=&*_ri_current->solid_lit_node.begin();iter!=end;++iter)
			//	{
			//		const RenderNodeInfo& info=*iter;
			//		if(!info.node->recvShadow())
			//			continue;
			//		_ri_current->setRenderingNode(info.node);
			//		info.node->render(info.parameter);
			//	}
			//}
		}
		void						SMgr::render_dir_shadowmapping_fill_shadow()
		{
			render_shadowmapping_fill_shadow();
		}
		void						SMgr::render_point_shadowmapping_fill_shadow()
		{
			render_shadowmapping_fill_shadow();
		}
		void						SMgr::render_spot_shadowmapping_fill_shadow()
		{
			render_shadowmapping_fill_shadow();
		}
		void						SMgr::clear_shadowmap()
		{
			//Gfx* gfx=Gfx::current();

			//gfx->setWorldMatrix(Mat4::identity);
			//gfx->setViewMatrix(Mat4::identity);
			//gfx->setProjectionMatrix(Mat4::identity);

			//gfx->RS_setShade(Gfx::SHADEMODE_GOURAUD);
			//gfx->RS_setAlpha(Gfx::CMPFUNC_GREATER,0.5f);
			//gfx->RS_setBlendRGB(Gfx::BLENDFACTOR_SRC_ALPHA,Gfx::BLENDFACTOR_ONE_MINUS_SRC_ALPHA);
			//gfx->RS_setBlendAlpha(Gfx::BLENDFACTOR_ONE,Gfx::BLENDFACTOR_ZERO);
			//gfx->RS_setBlendOpRGB(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setBlendOpAlpha(Gfx::BLENDOPERATION_ADD);
			//gfx->RS_setDepth(Gfx::CMPFUNC_LESS);
			//Gfx::CULLMODE cullmode=Gfx::CULLMODE_CW;
			//if(gfx->boundRenderTarget())
			//	cullmode = Gfx::reverseCullMode(cullmode);
			//// more reverse if any, e.g. reflection
			//gfx->RS_setCullMode(cullmode);
			//gfx->RS_setPolygonMode(Gfx::POLYGONMODE_FILL);
			//gfx->RS_setStencil(Gfx::CMPFUNC_ALWAYS,0,~0);
			//gfx->RS_setStencilOp(Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP);
			//gfx->RS_setLightAmbient(Vec4(0,0,0,1));
			//gfx->RS_setClipPlanes(_empty_array_clipplane);

			//gfx->RS_enableWriteColor(true,true,true,true);
			//gfx->RS_enableWriteDepth(true);
			//gfx->RS_enableWriteStencil(false);

			//gfx->RS_enableDepthTest(false);
			//gfx->RS_enableFog(false);
			//gfx->RS_enableScissorTest(false);
			//gfx->RS_enableAlphaTest(false);
			//gfx->RS_enableBlend(false);
			//gfx->RS_enableStencil(false);
			//gfx->RS_enableLighting(false);
			//gfx->RS_enableSpecular(true);

			//gfx->RS_setTSS_color_op(0,Gfx::TEXTUREOP_DISABLE);
			//gfx->RS_setTSS_alpha_op(0,Gfx::TEXTUREOP_DISABLE);

			//// must be set, otherwise clip planes are incorrectly setup
			//gfx->setViewMatrix(current_camera()->view());
			//gfx->setProjectionMatrix(current_camera()->proj());

			//if(!_shader_clear_shadowmap)
			//	_shader_clear_shadowmap=Shader::load(L"shaders/clear_shadowmap_vs.txt",L"shaders/clear_shadowmap_ps.txt");
			//if(!_shader_clear_shadowmap)
			//	return;
			//_shader_clear_shadowmap->bind();

			//Mat4 proj=gfx->computeDeviceProjectionMatrix(Mat4::identity);

			//gfx->imBegin(Gfx::PRIMTYPE_TRIANGLE_STRIP,false,false,0);
			//gfx->imVertex(Vec3(-1,1,1)*proj);
			//gfx->imVertex(Vec3(-1,-1,1)*proj);
			//gfx->imVertex(Vec3(1,1,1)*proj);
			//gfx->imVertex(Vec3(1,-1,1)*proj);
			//gfx->imEnd();

			//Shader::unbind();
		}
		//void						SMgr::render_fill_reflection(const SRenderNodeInfo& info)
		//{
		//	//Gfx* gfx=Gfx::current();

		//	//// setting solid states
		//	//gfx->setWorldMatrix(Mat4::identity);
		//	//gfx->setViewMatrix(Mat4::identity);
		//	//gfx->setProjectionMatrix(Mat4::identity);

		//	//gfx->RS_setShade(Gfx::SHADEMODE_GOURAUD);
		//	//gfx->RS_setAlpha(Gfx::CMPFUNC_GREATER,0.5f);
		//	//gfx->RS_setBlendRGB(Gfx::BLENDFACTOR_SRC_ALPHA,Gfx::BLENDFACTOR_ONE_MINUS_SRC_ALPHA);
		//	//gfx->RS_setBlendAlpha(Gfx::BLENDFACTOR_ONE,Gfx::BLENDFACTOR_ZERO);
		//	//gfx->RS_setBlendOpRGB(Gfx::BLENDOPERATION_ADD);
		//	//gfx->RS_setBlendOpAlpha(Gfx::BLENDOPERATION_ADD);
		//	//gfx->RS_setDepth(Gfx::CMPFUNC_LESS);
		//	//Gfx::CULLMODE cullmode=Gfx::CULLMODE_CW;
		//	//if(gfx->boundRenderTarget())
		//	//	cullmode = Gfx::reverseCullMode(cullmode);
		//	//// more reverse if any, e.g. reflection
		//	//gfx->RS_setCullMode(cullmode);
		//	//gfx->RS_setPolygonMode(Gfx::POLYGONMODE_FILL);
		//	//gfx->RS_setStencil(Gfx::CMPFUNC_ALWAYS,0,~0);
		//	//gfx->RS_setStencilOp(Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP,Gfx::STENCILOP_KEEP);
		//	//gfx->RS_setLightAmbient(Vec4(0,0,0,1));
		//	//gfx->RS_setClipPlanes(current_camera()->additionalClipPlane());

		//	//gfx->RS_enableWriteColor(true,true,true,true);
		//	//gfx->RS_enableWriteDepth(true);
		//	//gfx->RS_enableWriteStencil(false);

		//	//gfx->RS_enableDepthTest(true);
		//	//gfx->RS_enableFog(false);
		//	//gfx->RS_enableScissorTest(false);
		//	//gfx->RS_enableAlphaTest(true);
		//	//gfx->RS_enableBlend(false);
		//	//gfx->RS_enableStencil(false);
		//	//gfx->RS_enableLighting(false);
		//	//gfx->RS_enableSpecular(true);

		//	//gfx->RS_setTSS_color_op(0,Gfx::TEXTUREOP_DISABLE);
		//	//gfx->RS_setTSS_alpha_op(0,Gfx::TEXTUREOP_DISABLE);

		//	//// must be set, otherwise clip planes are incorrectly setup
		//	//gfx->setViewMatrix(current_camera()->view());
		//	//gfx->setProjectionMatrix(current_camera()->proj());

		//	//gfx->RS_enableWriteDepth(false);
		//	//gfx->RS_enableDepthTest(true);
		//	//gfx->RS_enableAlphaTest(false);
		//	//gfx->RS_enableBlend(true);

		//	//_ri_current->setRenderingNode(info.node);
		//	//info.node->render(info.parameter);
		//}

		void						SMgr::blur5x5(const SPtr<Tex2D>& tex, const SPtr<Tex2D>& tmp)
		{
			Gfx* gfx = Gfx::current();

			REAL texel_dx = 1.0f;
			REAL texel_dy = 1.0f;
			if(true)
			{
				size_t dx, dy, dx2, dy2;

				tex->getMipLevelDim(0, dx, dy);
				tmp->getMipLevelDim(0, dx2, dy2);

				Z_ASSERT( dx == dx2 && dy == dy2 );

				texel_dx = 1.0f / dx;
				texel_dy = 1.0f / dy;
			}

			rsView rsview;

			SPtr<Shader> vs, ps;
			gfx->getGaussianBlur5(vs, ps);

			rsShade rs(false);
			rs.setSampler(0, GfxEnum::FILTERMODE_NEAREST, GfxEnum::FILTERMODE_NEAREST, GfxEnum::FILTERMODE_NEAREST, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);

			ShaderUniforms2 unis(vs, ps);

			Jotter jotter;

			gfx->unbindDepthStencil();
			gfx->unbindAllRenderTargets();
			gfx->bindRenderTarget(0, tmp);

			rsview.init(gfx);
			gfx->setViewState(rsview);
			gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

			gfx->useProgrammablePipeline(vs, ps);

			gfx->bindTexture(0, tex);

			gfx->setShadeState(rs);

			unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedViewProjectionTransform());
			gfx->setShaderUniforms(unis);

			jotter.begin(2, false, false, 3);

			jotter.tc(0, 0, 0);
			jotter.tc(1, 0 + texel_dx, 0);
			jotter.tc(2, 0 + 2 * texel_dx, 0);
			jotter.vertex(-1, 1);

			jotter.tc(0, 0, 1);
			jotter.tc(1, 0 + texel_dx, 1);
			jotter.tc(2, 0 + 2 * texel_dx, 1);
			jotter.vertex(-1, -1);

			jotter.tc(0, 1, 0);
			jotter.tc(1, 1 + texel_dx, 0);
			jotter.tc(2, 1 + 2 * texel_dx, 0);
			jotter.vertex(1, 1);

			jotter.tc(0, 1, 1);
			jotter.tc(1, 1 + texel_dx, 1);
			jotter.tc(2, 1 + 2 * texel_dx, 1);
			jotter.vertex(1, -1);

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);

			//////////////////////////////////
			gfx->unbindDepthStencil();
			gfx->unbindAllRenderTargets();
			gfx->bindRenderTarget(0, tex);

			rsview.init(gfx);
			gfx->setViewState(rsview);
			gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

			gfx->useProgrammablePipeline(vs, ps);

			gfx->bindTexture(0, tmp);

			gfx->setShadeState(rs);

			unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedViewProjectionTransform());
			gfx->setShaderUniforms(unis);

			jotter.begin(2, false, false, 3);

			jotter.tc(0, 0, 0);
			jotter.tc(1, 0, 0 + texel_dy);
			jotter.tc(2, 0, 0 + 2 * texel_dy);
			jotter.vertex(-1, 1);

			jotter.tc(0, 0, 1);
			jotter.tc(1, 0, 1 + texel_dy);
			jotter.tc(2, 0, 1 + 2 * texel_dy);
			jotter.vertex(-1, -1);

			jotter.tc(0, 1, 0);
			jotter.tc(1, 1, 0 + texel_dy);
			jotter.tc(2, 1, 0 + 2 * texel_dy);
			jotter.vertex(1, 1);

			jotter.tc(0, 1, 1);
			jotter.tc(1, 1, 1 + texel_dy);
			jotter.tc(2, 1, 1 + 2 * texel_dy);
			jotter.vertex(1, -1);

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);
		}
	};
};

