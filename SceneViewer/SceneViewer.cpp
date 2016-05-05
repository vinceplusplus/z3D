
#include "stdafx.h"

#include "../z3D/Core/functor_inl.h"

#include "../z3D/Core/gaussian_inl.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

const size_t BLUR_RADIUS = 24;

namespace z3D
{
	namespace SG
	{
		class cgPostRenderFX
		{
		public:
			static void			process(const SPtr<Gfx>& gfx, const rsShade& rs, const SPtr<Shader>& vs, const SPtr<Shader>& ps, const ShaderUniforms2& unis, const SPtr<Tex2D>& tex0)
			{
				gfx->useProgrammablePipeline(vs, ps);

				gfx->setShaderUniforms(unis);

				gfx->setShadeState(rs);

				gfx->bindTexture(0, tex0);

				Jotter jotter;
				jotter.begin(2, false, true, 1, 2);

				jotter.color(Vec4(1, 1, 1, 1));

				jotter.tc(0, 0, 0);
				jotter.vertex(-1, 1);
				jotter.tc(0, 0, 1);
				jotter.vertex(-1, -1);
				jotter.tc(0, 1, 0);
				jotter.vertex(1, 1);

				jotter.tc(0, 1, 0);
				jotter.vertex(1, 1);
				jotter.tc(0, 0, 1);
				jotter.vertex(-1, -1);
				jotter.tc(0, 1, 1);
				jotter.vertex(1, -1);

				gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, jotter);
			}
		};
	};
};

class SceneViewer:public GameApp, public SNodeUserTextParser
{
private:
	SPtr<TexFont>		_sys_font;
private:
	QStats<25>			_tri_stats;
	FPSCounter			_fps_counter;
private:
	SPtr<SCameraNode>	_camera_node;
	SPtr<SNode>			_camera_tm_node;
	SPtr<SStructNode>	_map_node;
private:
	REAL				_cam_move_speed;
private:
	wcs					_last_load_path;
private:
	SPtr<phyMgr>		_phy_mgr;
private:
	SPtr<Oal>			_oal;
	//SPtr<SndPlayer>		_player;
	SPtr<wavSndPlayerSet>	_player;
private:
	bool				_animate;
	bool				_draw_bounding_boxes;
	bool				_draw_obbs;

	bool				_draw_grid;

	bool				_draw_wireframe;
private:
	wcs					_picked_node_name;
	Profiler			_profiler;

	SPtr<Tex2D>			_4floats_rt;
	SPtr<Tex2D>			_4floats_rt2;
	SPtr<Tex2D>			_4floats_rt3;
	SPtr<Tex2D>			_4floats_rt4;
	SPtr<Tex2D>			_4floats_rt5;

	SPtr<Shader>		_bright_vs;
	SPtr<Shader>		_bright_ps;

	SPtr<Shader>		_blur_vs;
	SPtr<Shader>		_blur_ps;

	SPtr<Shader>		_hsl_adjust_vs;
	SPtr<Shader>		_hsl_adjust_ps;
public:
	SceneViewer()
	{
		_cam_move_speed = 100;

		_phy_mgr = new phyMgr();
		_phy_mgr->setGravity(Vec3(0, -10, 0));

		_animate = true;

		_draw_bounding_boxes = false;
		_draw_obbs = false;

		_draw_grid = false;

		_draw_wireframe = false;
	}
	~SceneViewer()
	{
	}
	void				initFonts()
	{
		wchar_t buffer[4096];
		GetWindowsDirectory(buffer, _countof(buffer));
		wcs s = buffer;
		s += L"\\fonts\\arial.ttf";
		//s += L"\\fonts\\mingliu.ttc";
		TexFont::addFontCache(s);

		_sys_font = TexFont::load("arial", 12);
		//_sys_font = TexFont::load("pmingliu", 16);

		TexFont::removeFontCache(s);
	}
public:
	bool				post_create()
	{
		Tex2D::setForce16Bit(false);
		Tex2D::setMaxRes(2048);
		Tex2DGIF::setForce16Bit(false);
		Tex2DGIF::setMaxRes(2048);
		Tex2DAVI::setForce16Bit(false);
		Tex2DAVI::setMaxRes(2048);
		TexCube::setForce16Bit(false);
		TexCube::setMaxRes(2048);

		//_smgr->enableShader(false);
		//_smgr->enableShadow(false);

		initFonts();

		load();

		/*
		vector<string> d=Oal::enumDevices();
		//int k=sizeof(k);
		_oal=Oal::create(d[0]);
		_player=new wavSndPlayerSet(4);
		for(int i=0;i<1;++i)
			_player->play(_player->load(L"chimes_mono16.wav"));
			*/

		_4floats_rt = _gfx->createTex2D(_gfx->getFramebufferDX(), _gfx->getFramebufferDY(), 1, GfxEnum::PIXELFORMAT_A16B16G16R16F, true);
		_4floats_rt2 = _gfx->createTex2D(_gfx->getFramebufferDX() / 2, _gfx->getFramebufferDY() / 2, 1, GfxEnum::PIXELFORMAT_A16B16G16R16F, true);
		_4floats_rt3 = _gfx->createTex2D(_gfx->getFramebufferDX() / 4, _gfx->getFramebufferDY() / 4, 1, GfxEnum::PIXELFORMAT_A16B16G16R16F, true);
		_4floats_rt4 = _gfx->createTex2D(_gfx->getFramebufferDX() / 8, _gfx->getFramebufferDY() / 8, 1, GfxEnum::PIXELFORMAT_A16B16G16R16F, true);
		_4floats_rt5 = _gfx->createTex2D(_gfx->getFramebufferDX() / 4, _gfx->getFramebufferDY() / 4, 1, GfxEnum::PIXELFORMAT_A16B16G16R16F, true);

		_bright_vs = _gfx->createShaderFromFile(GfxEnum::SHADERTYPE_VERTEX, L"bright_vs.txt");
		_bright_ps = _gfx->createShaderFromFile(GfxEnum::SHADERTYPE_PIXEL, L"bright_ps.txt");

		_hsl_adjust_vs = _gfx->createShaderFromFile(GfxEnum::SHADERTYPE_VERTEX, L"hsl_adjust_vs.txt");
		_hsl_adjust_ps = _gfx->createShaderFromFile(GfxEnum::SHADERTYPE_PIXEL, L"hsl_adjust_ps.txt");

		if(true)
		{
			mbs vs = FileSystem::loadUTF8FromAll(L"blur_vs.txt");
			mbs ps = FileSystem::loadUTF8FromAll(L"blur_ps.txt");

			ps = REPLACE(ps, "/*** FETCH_COUNT ***/", FORMATA("%d", 1 + BLUR_RADIUS * 2));

			_blur_vs = _gfx->createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, vs);
			_blur_ps = _gfx->createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, ps);
		}

		return true;
	}
public:
	void				load(const wcs& dir)
	{
		release();

		if(!dir.length())
			return;

		_map_node = SStructNode::load(dir + L"/scene.xml");
		if(!_map_node)
		{
			MessageBox(NULL, L"cannot load the struct file", L"error", 0);
			return;
		}
		_map_node->loadTMCtrl(dir + L"/scene_tm.xml");
		_map_node->loadSkinCtrl(dir + L"/scene_skin.xml");
		//_map_node->loadUserText(dir + L"/scene_user.txt");
		_map_node->loadUserText(dir + L"/scene_user.txt", *this);

		_map_node->loadTextures();
		_smgr->root()->addChild(_map_node,false);
		if(_map_node->getLoadedNode(L"Camera01") && _map_node->getLoadedNode(L"Camera01")->type_info()->kind_of(SCameraNode::type_info_static()))
			_camera_node = _map_node->getLoadedNode(L"Camera01").cast<SCameraNode>();
		else if(_map_node->getLoadedNode(L"Camera01Shape") && _map_node->getLoadedNode(L"Camera01Shape")->type_info()->kind_of(SCameraNode::type_info_static()) && _map_node->getLoadedNode(L"Camera01"))
		{
			_camera_node = _map_node->getLoadedNode(L"Camera01Shape").cast<SCameraNode>();
			_camera_tm_node = _map_node->getLoadedNode(L"Camera01");
		}
		if(!_camera_node)
		{
			MessageBox(NULL, L"I need a camera called \"Camera01\"", L"error", 0);
			return;
		}
		_smgr->setActiveCamera(_camera_node);

		_map_node->setBillboardCameraNode(_camera_node);

		//_map_node->forceTreeLocalBound(true, AABB(Vec3(-1000, -1000, -1000), Vec3(1000, 1000, 1000)));

		//_map_node->rebuildTree(4, 10);
		//syslog << wstring(L"\r\n") + _map_node->print_hierarchy();

		_last_load_path = dir;
	}
	void				release()
	{
		_smgr->setActiveCamera(NULL);
		if(_map_node)
			_map_node->detachFromParent(false);
		_map_node.reset();
		_camera_node.reset();
		_camera_tm_node.reset();

		_phy_mgr->removeAllBodies();
	}
public:
	void				load()
	{
		wchar_t cur_dir[MAX_PATH], sys_vol[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, cur_dir);
		GetWindowsDirectory(sys_vol, MAX_PATH);
		if(wcschr(sys_vol, '\\'))
			*wcschr(sys_vol, '\\') = 0;
		if(wstring(cur_dir) == wstring(sys_vol) + L"\\")
		{
			GetModuleFileName(GetModuleHandle(NULL), cur_dir, MAX_PATH);
			if(wcsrchr(cur_dir, '\\'))
				*wcsrchr(cur_dir, '\\') = 0;
			SetCurrentDirectory(cur_dir);
		}

		load(CONVERT(chooseFolder(L"choose the folder that has the struct file", CONVERT(_last_load_path), cur_dir, getGameWindow())));
	}
public:
	void			overrideToWireFrame(const SRenderInfo* ri, rsShade& rs)
	{
		rs.setPolygonMode(GfxEnum::POLYGONMODE_LINE);
	}
public:
	void			update(REAL dt)
	{
		_phy_mgr->step(dt);

		if(_animate)
		{
			if(_map_node)
			{
				//_map_node->advance_all(dt);
				_profiler.start(L"update_anim");
				SCtrlOps::update(CtrlUpdater(dt, false), _map_node);
				_profiler.stop(L"update_anim");
			}
		}

		_input->update();

		if((_input->isKeyDown(VK_MENU) || _input->isKeyDown(VK_LMENU) || _input->isKeyDown(VK_RMENU)) && _input->isKeyPressed(VK_RETURN))
		{
			switchFullscreen(!fullscreen());
		}
		if(_input->isKeyDown(VK_ADD))
		{
			_cam_move_speed *= 1.1f;
		}
		if(_input->isKeyDown(VK_SUBTRACT))
		{
			_cam_move_speed /= 1.1f;
		}
		if(_input->isKeyPressed('B'))
		{
			if(!_draw_bounding_boxes)
			{
				_draw_bounding_boxes = true;
				_draw_obbs = false;
			}
			else
			{
				if(!_draw_obbs)
					_draw_obbs = true;
				else
					_draw_bounding_boxes = false;
			}
		}
		if(_input->isKeyPressed('G'))
		{
			_draw_grid = !_draw_grid;
		}
		if(_input->isKeyPressed('F'))
		{
			_draw_wireframe = !_draw_wireframe;
		}
		if(_input->isKeyPressed(VK_F3))
		{
			load();
			cancelPendingTimeSoFar();
		}
		if(_input->isKeyPressed(VK_ESCAPE))
		{
			_animate = !_animate;
		}
		//if(_input->isKeyPressed('Z'))
		//{
		//	if(_camera_node && _map_node)
		//		_camera_node->frame(_map_node->tree_world_bound());
		//}
		if(_camera_node)
		{
			SPtr<SNode> n = _camera_node;
			if(_camera_tm_node)
				n = _camera_tm_node;

			if(_input->isKeyDown('W'))
				n->translateWorld(Vec3::zNegAxis * n->world_rot().asMat3() * _cam_move_speed * dt);
			if(_input->isKeyDown('S'))
				n->translateWorld(Vec3::zAxis * n->world_rot().asMat3() * _cam_move_speed * dt);
			if(_input->isKeyDown('A'))
				n->translateWorld(Vec3::xNegAxis * n->world_rot().asMat3() * _cam_move_speed * dt);
			if(_input->isKeyDown('D'))
				n->translateWorld(Vec3::xAxis * n->world_rot().asMat3() * _cam_move_speed * dt);

			//if(_input->isKeyDown('W'))
			//	_camera_node->pan(Vec3::zNegAxis * _cam_move_speed * dt);
			//if(_input->isKeyDown('S'))
			//	_camera_node->pan(Vec3::zAxis * _cam_move_speed * dt);
			//if(_input->isKeyDown('A'))
			//	_camera_node->pan(Vec3::xNegAxis * _cam_move_speed * dt);
			//if(_input->isKeyDown('D'))
			//	_camera_node->pan(Vec3::xAxis * _cam_move_speed * dt);
			if(_input->getMouseDeltaX() || _input->getMouseDeltaY())
			{
				REAL yaw, pitch;
				n->world_rot().asMat3().resolveOriRH(yaw, pitch);
				n->setWorldRot(Mat3::rotationX(min(max(-89.9f, pitch - _input->getMouseDeltaY() * 0.1f), 89.9f)) * Mat3::rotationY(yaw - _input->getMouseDeltaX() * 0.1f));
				//_camera_node->rotateYaw(-_input->getMouseDeltaX() * 0.1f);
				//_camera_node->setPitch(min(max(_camera_node->pitch() - _input->getMouseDeltaY() * 0.1f, -89.9f), 89.9f));
			}
		}
		if(_camera_node)
		{
			_camera_node->setAspectRatio((REAL)_gfx->getFramebufferDX() / _gfx->getFramebufferDY());

			//if(_camera_node->isOrtho())
			//	_camera_node->setOrtho((REAL)-dx() / 2, (REAL)dx() / 2, (REAL)dy() / 2, (REAL)-dy() / 2, _camera_node->znear(), _camera_node->zfar());
		}

		if(_camera_node)
		{
			_picked_node_name = L"";

			Ray r = _camera_node->castRay(Vec2(0, 0));

			wcs name;

			REAL first_time = INFINITY;
			for(SStructNode::MAP_NAME_LOADED_NODE::const_iterator iter = _map_node->getLoadedNodes().begin(); iter != _map_node->getLoadedNodes().end(); ++iter)
			{
				if(iter->second->type_info()->kind_of(SCameraNode::type_info_static()))
					continue;
				if(iter->first == L"Camera01")
					continue;

				REAL t;
				if(Test::RayOBB(r, OBB(iter->second->local_bound()).transformedBy(iter->second->world_trans()), t))
				{
					if(t < first_time)
					{
						first_time = t;

						name = iter->first;
					}
				}
			}
			if(first_time != INFINITY)
			{
				_picked_node_name = name;
			}
		}
	}
	void			draw(REAL dt)
	{
		_profiler.start(L"render");
		if(_animate)
		{
			if(_map_node)
			{
				//_map_node->update_all();
			}
		}

		_gfx->resetAccumPolycount();

		_gfx->beginScene();

		if(false)
		{
			_gfx->bindRenderTarget(0, _4floats_rt);
			_gfx->bindDefaultDepthStencil();
		}

		_gfx->clear(true, true, false, Vec4(0.25f, 0.25f, 0.25f, 1), _gfx->getZFarMapping(), 0);

		_smgr->setPostSetupShadeStateAdjustCallback(functor2<void, const SRenderInfo*, rsShade&>());

		if(_draw_wireframe)
			_smgr->setPostSetupShadeStateAdjustCallback(functor2<void, const SRenderInfo*, rsShade&>(this, &SceneViewer::overrideToWireFrame));

		_smgr->drawAll();

		if(false)
		{
			// copy to main framebuffer first
			_gfx->bindDefaultRenderTarget();

			_gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

			rsShade rs(false);
			rs.setSampler(0, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);

			if(true)
			{
				SPtr<Shader> vs, ps;
				//_gfx->getImg2DShaders(vs, ps);
				vs = _hsl_adjust_vs;
				ps = _hsl_adjust_ps;

				ShaderUniforms2 unis(vs, ps);
				unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", _gfx->getAdjustedViewProjectionTransform());
				unis.set(GfxEnum::SHADERTYPE_PIXEL, "xform", Mat4::scaling(1, 1.75f, 1.0f));

				cgPostRenderFX::process(_gfx, rs, vs, ps, unis, _4floats_rt);
			}

			// bright pass and down sample
			_gfx->bindRenderTarget(0, _4floats_rt2);

			_gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

			if(true)
			{
				ShaderUniforms2 unis(_bright_vs, _bright_ps);
				unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", _gfx->getAdjustedViewProjectionTransform());

				cgPostRenderFX::process(_gfx, rs, _bright_vs, _bright_ps, unis, _4floats_rt);
			}

			// down sample again
			_gfx->bindRenderTarget(0, _4floats_rt3);

			_gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

			if(true)
			{
				SPtr<Shader> vs, ps;
				_gfx->getImg2DShaders(vs, ps);

				ShaderUniforms2 unis(vs, ps);
				unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", _gfx->getAdjustedViewProjectionTransform());

				cgPostRenderFX::process(_gfx, rs, vs, ps, unis, _4floats_rt2);
			}

			// bloom
			if(true)
			{
				ShaderUniforms2 unis1(_blur_vs, _blur_ps);
				unis1.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", _gfx->getAdjustedViewProjectionTransform());
				ShaderUniforms2 unis2(_blur_vs, _blur_ps);
				unis2.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", _gfx->getAdjustedViewProjectionTransform());

				double kernel[BLUR_RADIUS * 2 + 1];
				gaussian::dist_kernel(gaussian::dist_sigma(BLUR_RADIUS * 2 + 1), kernel);
				for(size_t i = 0; i < BLUR_RADIUS * 2 + 1; ++i)
				{
					REAL ox = (ptrdiff_t)(i - BLUR_RADIUS) * (1.0f / _4floats_rt5->getDX());
					REAL oy = (ptrdiff_t)(i - BLUR_RADIUS) * (1.0f / _4floats_rt5->getDY());
					//REAL ox = (ptrdiff_t)(i - BLUR_RADIUS) * ((i - BLUR_RADIUS) ? ((ptrdiff_t)(i - BLUR_RADIUS)) * 2.0f - COPYSIGN(0.5f, (REAL)i - BLUR_RADIUS): 0) * (1.0f / _4floats_rt5->getDX());
					//REAL oy = (ptrdiff_t)(i - BLUR_RADIUS) * ((i - BLUR_RADIUS) ? ((ptrdiff_t)(i - BLUR_RADIUS)) * 2.0f - COPYSIGN(0.5f, (REAL)i - BLUR_RADIUS): 0) * (1.0f / _4floats_rt5->getDY());

					unis1.set(GfxEnum::SHADERTYPE_PIXEL, "fetches", i, Vec3(ox, 0, (REAL)kernel[i]));
					unis2.set(GfxEnum::SHADERTYPE_PIXEL, "fetches", i, Vec3(0, oy, (REAL)kernel[i]));
				}

				// horizontal blur
				_gfx->bindRenderTarget(0, _4floats_rt5);

				_gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

				cgPostRenderFX::process(_gfx, rs, _blur_vs, _blur_ps, unis1, _4floats_rt3);

				// vertical blur
				_gfx->bindRenderTarget(0, _4floats_rt3);

				_gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

				cgPostRenderFX::process(_gfx, rs, _blur_vs, _blur_ps, unis2, _4floats_rt5);
			}

			// blend the bloom to the main framebuffer
			_gfx->bindDefaultRenderTarget();

			_gfx->setViewProjTransform(Mat4::identity, Mat4::identity);

			// need to change blend mode
			rs.enableBlend(GfxEnum::BLENDFACTOR_ONE, GfxEnum::BLENDFACTOR_ONE);

			if(true)
			{
				SPtr<Shader> vs, ps;
				_gfx->getImg2DShaders(vs, ps);

				ShaderUniforms2 unis(vs, ps);
				unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", _gfx->getAdjustedViewProjectionTransform());

				cgPostRenderFX::process(_gfx, rs, vs, ps, unis, _4floats_rt3);
			}

		}

		if(_camera_node)
		{
			if(_map_node)
			{
				if(_draw_bounding_boxes)
				{
					rsView viewstate;
					viewstate.init(_gfx);
					_gfx->setViewState(viewstate);
					//_gfx->unbindDepthStencil();
					//_gfx->unbindAllRenderTargets();
					//_gfx->bindDefaultRenderTarget();
					//_gfx->bindDefaultDepthStencil();
					_gfx->setViewProjTransform(_camera_node->view(), _camera_node->proj());

					//DrawHelper::drawAABB(_map_node->tree_world_bound());

					if(!_draw_obbs)
						DrawHelper::drawAABBsOutline(_map_node);
					else
						DrawHelper::drawOBBsOutline(_map_node);
				}
				if(_draw_grid)
				{
					DrawHelper::drawGrid(10, 7);
				}
			}
		}

		if(true)
		{
			rsView viewstate;
			viewstate.init(_gfx);
			_gfx->setViewState(viewstate);
		}
		_gfx->setViewProjTransform2D();

		if(_camera_node)
		{
			Vec2 pos(_gfx->getFramebufferDX() / 32.0f + 16.0f, _gfx->getFramebufferDY() - _gfx->getFramebufferDY() / 32.0f - 16.0f);

			DrawHelper::drawOrthoAxisTripod(_camera_node->world_rot().asMat3(), pos, true, _sys_font);
		}

		_sys_font->draw(FORMATW(L"fps: %d, update_anim: %f, render: %f", _fps_counter.getFPS(), _profiler.getAverageTime(L"update_anim"), _profiler.getAverageTime(L"render")), Vec2(0, 0));
		_sys_font->draw(FORMATW(L"prim count: %d", _gfx->getAccumPolycount()), Vec2(0, 20));

		if(_camera_node)
			_sys_font->draw(FORMATW(L"cam, pos (%f, %f, %f), yaw(%f), pitch(%f)", _camera_node->world_pos().x, _camera_node->world_pos().y, _camera_node->world_pos().z, _camera_node->yaw(), _camera_node->pitch()), Vec2(0, 40));

		_sys_font->draw(L"ESC - pause/resume the animation", Vec2(0, 80));
		_sys_font->draw(L"B - toggle bounding boxes", Vec2(0, 100));
		_sys_font->draw(L"G - toggle grid", Vec2(0, 120));
		_sys_font->draw(L"F - toggle wireframe", Vec2(0, 140));
		_sys_font->draw(_picked_node_name, Vec2(0, 160));

		_gfx->endScene();

		if(!fullscreen())
			_gfx->flush();

		_gfx->present();

		if(!fullscreen())
			_gfx->flush();

		_profiler.stop(L"render");
	}
	void		parseUserText(const wcs& node_name, const SPtr<SNode>& node, const wcs& user_text)
	{
		Config* cfg = Config::fromWCS(user_text);
		if(cfg->exists(L"collision_shape"))
		{
			Z_ASSERT( !node->getPhyBody() );

			wcs shape_name = cfg->getWString(L"collision_shape");
			REAL mass = cfg->getFloat(L"mass");
			int32_t compound = cfg->getInt32(L"compound");

			if(shape_name == L"box")
			{
				if(compound)
				{
					SPtr<phyShape> shape = _phy_mgr->createCompoundWrappedBoxShape(node->local_bound().extent(), node->local_bound().center());
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::identity, mass, false);
					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
				else
				{
					SPtr<phyShape> shape = _phy_mgr->createBoxShape(node->local_bound().extent());
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(Vec3(node->local_bound().center())), mass, false);
					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
			else if(shape_name == L"sphere")
			{
				Vec3 ext = node->local_bound().extent();
				SPtr<phyShape> shape = _phy_mgr->createSphereShape((ext[0] + ext[1] + ext[2]) / 3);
				SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(Vec3(node->local_bound().center())), mass, false);
				_phy_mgr->addBody(body);

				node->setPhyBody(body);
			}
			else if(shape_name == L"mesh")
			{
				if(node->type_info()->kind_of(SMeshNode::type_info_static()))
				{
					SPtr<phyShape> shape = _phy_mgr->createMeshShape(node.cast<SMeshNode>()->mesh());
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::identity, mass, true);
					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
			else if(shape_name == L"convex_hull")
			{
				if(node->type_info()->kind_of(SMeshNode::type_info_static()))
				{
					Vec3 offset;
					REAL computed_mass;
					Mat3 inertia_tensor;
					SPtr<phyShape> shape = _phy_mgr->createConvexHullShape(node.cast<SMeshNode>()->mesh(), offset, computed_mass, inertia_tensor);
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(offset), mass, mass / computed_mass * Vec3(inertia_tensor[0][0], inertia_tensor[1][1], inertia_tensor[2][2]));

					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
			else if(shape_name == L"convex_decomp")
			{
				if(node->type_info()->kind_of(SMeshNode::type_info_static()))
				{
					Vec3 offset;
					REAL computed_mass;
					Mat3 inertia_tensor;
					SPtr<phyShape> shape = _phy_mgr->createDecompConvexHullShape(node.cast<SMeshNode>()->mesh(), offset, computed_mass, inertia_tensor);
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(offset), mass, mass / computed_mass * Vec3(inertia_tensor[0][0], inertia_tensor[1][1], inertia_tensor[2][2]));

					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
		}
		delete cfg;
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	SPtr<SceneViewer> app = new SceneViewer();
	if(!app->create(L"SceneViewer", false, 800, 600, 24, false))
	{
		app.reset();
		return 0;
	}

	app->setUpdateFPS(0);
	app->setCapDrawFPS(true);
	app->setSleep(true, 0);
	app->gameLoop();

	app.reset();

	AVIRenderer::__shutdown();

	return 0;
}

