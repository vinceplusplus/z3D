
#include "SMgr.h"

#include "../Core/stock_inl.h"
#include "../Core/functor_inl.h"

#include "SNode.h"
#include "SCameraNode.h"
#include "SLightNode.h"

namespace z3D
{
	namespace SG
	{
		SMgr::SMgr()
		{
			_root = new SNode();

			makeCurrent();

			_enable_shader = true;
			_enable_shadow = true;

			_stock_screen_float.setSourceFunc(functor0<SPtr<Tex2D>>(this, &SMgr::__stock_source_screen_float));
			_stock_screen_depth.setSourceFunc(functor0<SPtr<Tex2D>>(this, &SMgr::__stock_source_screen_depth));
			_stock_screen_rgba.setSourceFunc(functor0<SPtr<Tex2D>>(this, &SMgr::__stock_source_screen_rgba));
			_stock_2floats.setSourceFunc(functor0<SPtr<Tex2D>>(this, &SMgr::__stock_source_2floats));
			_stock_depth.setSourceFunc(functor0<SPtr<Tex2D>>(this, &SMgr::__stock_source_depth));
			_stock_rgba.setSourceFunc(functor0<SPtr<Tex2D>>(this, &SMgr::__stock_source_rgba));
			_stock_screen_rb_depth.setSourceFunc(functor0<SPtr<Renderbuffer>>(this, &SMgr::__stock_source_screen_rb_depth));
			_stock_rb_depth.setSourceFunc(functor0<SPtr<Renderbuffer>>(this, &SMgr::__stock_source_rb_depth));
			_stock_camera.setSourceFunc(functor0<SPtr<SCameraNode>>(this, &SMgr::__stock_source_camera));
			_stock_camera.setRenewFunc(functor1<void, const SPtr<SCameraNode>&>(this, &SMgr::__stock_renew_camera));

			_non_screen_size_rt_dx = 512;
			_non_screen_size_rt_dy = 512;

			_default_max_recursions = 1;
		}
		SMgr::~SMgr()
		{
		}
		void						SMgr::enableShader(bool enable)
		{
			_enable_shader = enable;
		}
		void						SMgr::enableShadow(bool enable)
		{
			_enable_shadow = enable;
		}
		bool						SMgr::enabledShader() const
		{
			return _enable_shader;
		}
		bool						SMgr::enabledShadow() const
		{
			return _enable_shadow;
		}
		void						SMgr::setActiveCamera(const SPtr<SCameraNode>& node)
		{
			_active_camera=node;
		}
		const SPtr<SCameraNode>&	SMgr::getActiveCamera() const
		{
			return _active_camera;
		}
		SPtr<SNode>&				SMgr::root()
		{
			return _root;
		}
		void						SMgr::drawAll(DRAWPASS pass, SRenderInfo* ri)
		{
			draw(root(), pass, ri);
		}
		void						SMgr::draw(const SPtr<SNode>& the_root, DRAWPASS pass, SRenderInfo* ri)
		{
			bool free_ri = false;
			if(!ri)
			{
				if(!_active_camera)
					return;

				ri = SRenderInfo::alloc();
				ri->setCamera(_active_camera);

				ri->setMaxRecursions(getDefaultMaxRecursions());
				ri->setPostSetupShadeStateAdjustCallback(_post_setup_shade_state_adjust_cb);

				free_ri = true;
			}

			ri->setSMgr(this);

			if(pass == DRAWPASS_DEFAULT)
			{
				if(!doUseShadow())
				{
					if(true)
						draw(the_root, DRAWPASS_SHADE, ri);
					else	// use it only if the scene complexity is high
					{
						draw(the_root, DRAWPASS_DEPTH_ONLY, ri);
						draw(the_root, DRAWPASS_SHADE_EQUAL_DEPTH, ri);
					}
				}
				else
				{
					SRenderInfo* ri_scan = SRenderInfo::alloc();
					ri_scan->setCamera(ri->getCamera());

					draw(the_root, DRAWPASS_SCAN_SOLID_RECV_SHADOW_AND_SHADOW_LIGHTS, ri_scan);

					SLightNode* shadow_lights[4];
					size_t shadow_light_count = 0;

					Z_STATIC_ASSERT( _countof(shadow_lights) <= 4, WRONG_ASSUMPTION );

					size_t max_shadow_light_count = min(_countof(shadow_lights), (size_t)4);

					if(true)
					{
						SLightNode* lights[64];
						size_t light_count = 0;

						const SRenderInfo::SceneInput& input = ri_scan->getSceneInput();
						for(size_t i = 0; i < input.array_light_item.size(); ++i)
						{
							SLightNode* l = input.array_light_item[i].node;
							if(!l->getLightCastShadow())
								continue;
							switch(l->light_type())
							{
							case SLightNode::LIGHTTYPE_DIRECTIONAL:
							case SLightNode::LIGHTTYPE_SPOT:
								{
									lights[light_count] = l;
									++light_count;
								}
								break;
							}
							if(light_count == _countof(lights))
								break;
						}

						shadow_light_count = max_shadow_light_count;
						SLightsChooser::choose(lights, lights + light_count, ri->getCamera()->world_pos(), shadow_lights, shadow_light_count);
					}

					if(!shadow_light_count || !ri_scan->getSceneInput().array_solid_item.size())
					{
						draw(the_root, DRAWPASS_SHADE, ri);
					}
					else
					{
						sort(shadow_lights + 0, shadow_lights + shadow_light_count);

						Gfx* gfx = Gfx::current();

						gfx->push_RT_n_DS();

						do
						{
							SPtr<Tex2D> depth = _stock_screen_float.alloc();
							SPtr<Renderbuffer> depthstencil = _stock_screen_rb_depth.alloc();

							if(!depth || !depthstencil)
								break;

							gfx->unbindDepthStencil();
							gfx->unbindAllRenderTargets();
							gfx->bindRenderTarget(0, depth);
							gfx->bindDepthStencil(depthstencil);

							gfx->clear(true, true, false, Vec4(1, 1, 1, 1), gfx->getZFarMapping(), 0);

							if(true)
							{
								SRenderInfo* ri2 = SRenderInfo::alloc();
								ri2->setCamera(ri->getCamera());
								draw(the_root, DRAWPASS_DEPTH_ONLY_SOLID_RECV_SHADOW_VSM, ri2);
								ri2->dealloc();
							}

							SPtr<Tex2D> shadowmap_blur = _stock_2floats.alloc();
							if(!shadowmap_blur)
								break;

							SPtr<SCameraNode> shadowmaps_cam[4];
							SPtr<Tex2D> shadowmaps[4];
							SPtr<Renderbuffer> shadowmap_depthstencil = _stock_rb_depth.alloc();

							Z_STATIC_ASSERT( _countof(shadowmaps) == _countof(shadow_lights), ASSUMPTION_WRONG );
							Z_STATIC_ASSERT( _countof(shadowmaps) == _countof(shadowmaps_cam), ASSUMPTION_WRONG );

							if(true)
							{
								bool succ = true;
								for(size_t i = 0; i < shadow_light_count; ++i)
								{
									SPtr<SCameraNode> shadowmap_cam = shadowmaps_cam[i] = _stock_camera.alloc();

									shadowmaps[i] = _stock_2floats.alloc();
									if(!shadowmaps[i])
									{
										succ = false;
										break;
									}

									SLightNode* l = shadow_lights[i];
									switch(l->light_type())
									{
									case SLightNode::LIGHTTYPE_DIRECTIONAL:
										shadowmap_cam->setOrtho(-l->getBeam(), l->getBeam(), l->getBeam(), -l->getBeam(), 0.1f, l->getRadius());
										break;
									case SLightNode::LIGHTTYPE_SPOT:
										shadowmap_cam->setAspectRatio(1.0f);
										shadowmap_cam->setPerspective(-l->getOuterAngle(), 0.1f, l->getRadius());
										break;
									}
									shadowmap_cam->setWorldTransform(l->world_trans());

									gfx->unbindDepthStencil();
									gfx->unbindAllRenderTargets();
									gfx->bindRenderTarget(0, shadowmaps[i]);
									gfx->bindDepthStencil(shadowmap_depthstencil);

									gfx->clear(true, true, false, Vec4(1, 1, 1, 1), gfx->getZFarMapping(), 0);

									if(true)
									{
										SRenderInfo* ri2 = SRenderInfo::alloc();
										ri2->setCamera(shadowmaps_cam[i]);
										draw(the_root, DRAWPASS_GEN_SHADOW_MAP_VSM, ri2);
										ri2->dealloc();
									}

									for(size_t j = 0; j < 1; ++j)
										blur5x5(shadowmaps[i], shadowmap_blur);
								}
								if(!succ)
									break;
							}

							SPtr<Tex2D> light_vis = _stock_screen_rgba.alloc();

							if(!light_vis)
								break;

							if(true)
							{
								gfx->unbindDepthStencil();
								gfx->unbindAllRenderTargets();
								gfx->bindRenderTarget(0, light_vis);

								rsView rs;
								rs.init(gfx);

								gfx->setViewState(rs);
								gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

								SPtr<Shader> vs, ps;
								gfx->getVSMMarkShadow(shadow_light_count, vs, ps);
								gfx->useProgrammablePipeline(vs, ps);

								gfx->bindTexture(0, depth);
								for(size_t i = 0; i < shadow_light_count; ++i)
									gfx->bindTexture(1 + i, shadowmaps[i]);

								rsShade rs2(false);
								rs2.setSampler(0, GfxEnum::FILTERMODE_NEAREST, GfxEnum::FILTERMODE_NEAREST, GfxEnum::FILTERMODE_NEAREST, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);
								for(size_t i = 0; i < shadow_light_count; ++i)
									rs2.setSampler(1 + i, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);
								gfx->setShadeState(rs2);

								ShaderUniforms2 unis(vs, ps);
								unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedViewProjectionTransform());
								unis.set(GfxEnum::SHADERTYPE_PIXEL, "world_xform", ri->getCamera()->view_proj().inverse());
								for(size_t i = 0; i < shadow_light_count; ++i)
								{
									unis.set(GfxEnum::SHADERTYPE_PIXEL, FORMATA("sm_uv_xform_%Id", i).c_str(), shadowmaps_cam[i]->view_proj() * Mat4::scaling(0.5f, 0.5f, 1) * Mat4::translation(0.5f, 0.5f, 0) * Mat4::scaling(1, -1, 1) * Mat4::translation(0, 1, 0));
									unis.set(GfxEnum::SHADERTYPE_PIXEL, FORMATA("light_space_depth_xform_%Id", i).c_str(), *(Vec4*)((shadowmaps_cam[i]->view() * Mat4::scaling(1, 1, -1 / shadowmaps_cam[i]->zfar())).transpose()[2]));
								}
								gfx->setShaderUniforms(unis);

								Jotter jotter;
								jotter.begin(2, false, false, 1);

								jotter.tc(0, -1, 1);
								jotter.vertex(-1, 1);

								jotter.tc(0, -1, -1);
								jotter.vertex(-1, -1);

								jotter.tc(0, 1, 1);
								jotter.vertex(1, 1);

								jotter.tc(0, 1, -1);
								jotter.vertex(1, -1);

								gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);

								ri->setLightsVisibility(light_vis, shadow_lights, shadow_light_count);
							}
						}while(false);

						gfx->pop_RT_n_DS();

						draw(the_root, DRAWPASS_SHADE, ri);
					}

					ri_scan->dealloc();
				}
			}
			else
			{
				ri->setDrawPass(pass);

				preRender(the_root, ri->getCamera()->frustum(), ri);

				switch(ri->getDrawPass())
				{
				case DRAWPASS_SHADE:
				case DRAWPASS_SHADE_EQUAL_DEPTH:
				case DRAWPASS_SCAN_SOLID_RECV_SHADOW_AND_SHADOW_LIGHTS:
					ri->computeSolidBound();
					preRenderLighting(the_root, ri->getSolidBound(), ri);
					break;
				}

				switch(ri->getDrawPass())
				{
				case DRAWPASS_SCAN_SOLID_RECV_SHADOW_AND_SHADOW_LIGHTS:
					break;
				default:
					render(ri);
					break;
				}
			}

			if(free_ri)
				ri->dealloc();
		}
		void						SMgr::preRender(const SPtr<SNode>& node, const Frustum& frustum, SRenderInfo* ri)
		{
			Z_ASSERT(node);
			if(!node->visible())
				return;
			if(!node->treeHasRenderables())
				return;

			if(node->getChildCount())
			{
				Frustum::FRUSTUMCULL fc = frustum.contains(node->tree_world_bound());
				if(!fc)
					return;

				if(node->renderable() && (fc == Frustum::FC_INSIDE || frustum.contains(node->world_bound())))
					node->preRender(ri);

				if(fc == Frustum::FC_INSIDE)
				{
					preRender(node, ri);
					return;
				}

				class fn
				{
				private:
					SMgr&				_t;
					const Frustum&		_frustum;
					SRenderInfo*&		_ri;
				public:
					fn(SMgr& t, const Frustum& frustum, SRenderInfo*& ri) : _t(t), _frustum(frustum), _ri(ri) {}
					void				operator() (const SPtr<SNode>& n)
					{
						_t.preRender(n, _frustum, _ri);
					}
				};

				for_each<vector<SPtr<SNode>>::const_iterator, fn&>(node->getChildren().begin(), node->getChildren().end(), fn(*this, frustum, ri));
			}
			else
			{
				if(node->renderable() && frustum.contains(node->world_bound()))
					node->preRender(ri);
				return;
			}
		}
		void						SMgr::preRender(const SPtr<SNode>& node, SRenderInfo* ri)
		{
			Z_ASSERT(node);
			if(!node->visible())
				return;
			if(!node->treeHasRenderables())
				return;

			if(node->renderable())
				node->preRender(ri);

			if(node->getChildCount())
			{
				class fn
				{
				private:
					SMgr&				_t;
					SRenderInfo*&		_ri;
				public:
					fn(SMgr& t, SRenderInfo*& ri) : _t(t), _ri(ri) {}
					void				operator() (const SPtr<SNode>& n)
					{
						_t.preRender(n, _ri);
					}
				};

				for_each<vector<SPtr<SNode>>::const_iterator, fn&>(node->getChildren().begin(), node->getChildren().end(), fn(*this, ri));
			}
		}
		void						SMgr::preRenderLighting(const SPtr<SNode>& node, const AABB& bound, SRenderInfo* ri)
		{
			Z_ASSERT(node);
			if(!node->visible())
				return;
			if(!node->treeHasRenderables())
				return;

			if(node->getChildCount())
			{
				if(!Test::AABBAABB(bound, node->tree_world_bound()))
					return;
				if(node->renderable() && Test::AABBAABB(bound, node->world_bound()))
					node->preRenderLighting(ri);

				class fn
				{
				private:
					SMgr&				_t;
					const AABB&			_bound;
					SRenderInfo*&		_ri;
				public:
					fn(SMgr& t, const AABB& bound, SRenderInfo*& ri) : _t(t), _bound(bound), _ri(ri) {}
					void				operator() (const SPtr<SNode>& n)
					{
						_t.preRenderLighting(n, _bound, _ri);
					}
				};

				for_each<vector<SPtr<SNode>>::const_iterator, fn&>(node->getChildren().begin(), node->getChildren().end(), fn(*this, bound, ri));
			}
			else
			{
				if(node->renderable() && Test::AABBAABB(bound, node->world_bound()))
					node->preRenderLighting(ri);
			}
		}
		bool						SMgr::doUseShader() const
		{
			return _enable_shader && Gfx::current()->supportsShaderModel2();
		}
		bool						SMgr::doUseShadow() const
		{
			return _enable_shadow && _enable_shader && Gfx::current()->supportsShaderModel3();
		}
		void						SMgr::setNonScreenSizeRTSize(uint32_t dx, uint32_t dy)
		{
			if(_non_screen_size_rt_dx == dx && _non_screen_size_rt_dy == dy)
				return;

			Z_ASSERT( dx && IS_POW2(dx) && dy && IS_POW2(dy) );

			_non_screen_size_rt_dx = dx;
			_non_screen_size_rt_dy = dy;

			_stock_2floats.clear();
			_stock_depth.clear();
		}
		void						SMgr::getNonScreenSizeRTSize(uint32_t& dx, uint32_t& dy) const
		{
			dx = _non_screen_size_rt_dx;
			dy = _non_screen_size_rt_dy;
		}
		void						SMgr::setDefaultMaxRecursions(size_t max_recursions)
		{
			_default_max_recursions = max_recursions;
		}
		size_t						SMgr::getDefaultMaxRecursions() const
		{
			return _default_max_recursions;
		}
		void						SMgr::setPostSetupShadeStateAdjustCallback(const functor2<void, const SRenderInfo*, rsShade&>& cb)
		{
			_post_setup_shade_state_adjust_cb = cb;
		}
		const functor2<void, const SRenderInfo*, rsShade&>&
									SMgr::getPostSetupShadeStateAdjustCallback() const
		{
			return _post_setup_shade_state_adjust_cb;
		}
		SPtr<Tex2D>					SMgr::__stock_source_screen_float()
		{
			return Tex2D::create(Gfx::current()->getWindowDX(), Gfx::current()->getWindowDY(), 1, GfxEnum::PIXELFORMAT_R32F, true);
		}
		SPtr<Tex2D>					SMgr::__stock_source_screen_depth()
		{
			return Tex2D::create(Gfx::current()->getWindowDX(), Gfx::current()->getWindowDY(), 1, GfxEnum::PIXELFORMAT_D24S8, false);
		}
		SPtr<Tex2D>					SMgr::__stock_source_screen_rgba()
		{
			return Tex2D::create(Gfx::current()->getWindowDX(), Gfx::current()->getWindowDY(), 1, GfxEnum::PIXELFORMAT_A8R8G8B8, true);
		}
		SPtr<Tex2D>					SMgr::__stock_source_2floats()
		{
			return Tex2D::create(_non_screen_size_rt_dx, _non_screen_size_rt_dy, 1, GfxEnum::PIXELFORMAT_G16R16F, true);
		}
		SPtr<Tex2D>					SMgr::__stock_source_depth()
		{
			return Tex2D::create(_non_screen_size_rt_dx, _non_screen_size_rt_dy, 1, GfxEnum::PIXELFORMAT_D24S8, false);
		}
		SPtr<Tex2D>					SMgr::__stock_source_rgba()
		{
			return Tex2D::create(_non_screen_size_rt_dx, _non_screen_size_rt_dy, 1, GfxEnum::PIXELFORMAT_A8R8G8B8, true);
		}
		SPtr<Renderbuffer>			SMgr::__stock_source_screen_rb_depth()
		{
			return Renderbuffer::createDepthStencil(Gfx::current()->getWindowDX(), Gfx::current()->getWindowDY(), GfxEnum::PIXELFORMAT_D24S8, false);
		}
		SPtr<Renderbuffer>			SMgr::__stock_source_rb_depth()
		{
			return Renderbuffer::createDepthStencil(_non_screen_size_rt_dx, _non_screen_size_rt_dy, GfxEnum::PIXELFORMAT_D24S8, false);
		}
		SPtr<SCameraNode>			SMgr::__stock_source_camera()
		{
			return new SCameraNode();
		}
		void						SMgr::__stock_renew_camera(const SPtr<SCameraNode>& camera_node)
		{
			camera_node->additionalClipPlanes().clear();
		}
		SPtr<Tex2D>					SMgr::getStockScreenFloat()
		{
			return _stock_screen_float.alloc();
		}
		SPtr<Tex2D>					SMgr::getStockScreenDepth()
		{
			return _stock_screen_depth.alloc();
		}
		SPtr<Tex2D>					SMgr::getStockScreenRGBA()
		{
			return _stock_screen_rgba.alloc();
		}
		SPtr<Tex2D>					SMgr::getStockNonScreen2Floats()
		{
			return _stock_2floats.alloc();
		}
		SPtr<Tex2D>					SMgr::getStockNonScreenDepth()
		{
			return _stock_depth.alloc();
		}
		SPtr<Tex2D>					SMgr::getStockNonScreenRGBA()
		{
			return _stock_rgba.alloc();
		}
		SPtr<Renderbuffer>			SMgr::getStockScreenRBDepth()
		{
			return _stock_screen_rb_depth.alloc();
		}
		SPtr<Renderbuffer>			SMgr::getStockNonScreenRBDepth()
		{
			return _stock_rb_depth.alloc();
		}
		SPtr<SCameraNode>			SMgr::getStockCamera()
		{
			return _stock_camera.alloc();
		}
	};
};

