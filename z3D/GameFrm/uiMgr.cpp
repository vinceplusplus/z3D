
#include "uiMgr.h"

#include "../Core/temp_stock_inl.h"
#include "../Core/functor_inl.h"

#include "uiBase.h"

#include "Input.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace GameFrm
	{
		uiMgr::BGCacheCfg::BGCacheCfg(int32_t dx_, int32_t dy_)
			: dx(dx_), dy(dy_)
		{}
		bool						uiMgr::BGCacheCfg::operator<(const BGCacheCfg& rhs)
		{
			return memcmp(this, &rhs, sizeof(*this)) < 0;
		}

		uiUpdateLink::uiUpdateLink()
		{
			_list = NULL;
		}
		uiUpdateLink::~uiUpdateLink()
		{
			if(_list)
				_list->erase(_iter);
		}
		uiMgr::uiMgr()
		{
			Z_STATIC_ASSERT( sizeof(HWND) == sizeof(_hwnd), WRONG_ASSUMPTION );

			//_diwrap=diwrap;

			_hwnd = NULL;

			_root = new uiBase();

			//_ui_capture_input=NULL;
			//_ui_last_hover=NULL;
			//_ui_draw_overhang=NULL;

			//_root->setExt(Vec2((REAL)_ogl->getWidth(),(REAL)_ogl->getHeight()));
			_root->setExt(65536, 65536);

			if(Gfx::current())
			{
				_res_dx = (int32_t)Gfx::current()->getFramebufferDX();
				_res_dy = (int32_t)Gfx::current()->getFramebufferDY();
			}

			_sync_desktop_mouse = true;

			makeCurrent();

			_stock_bgcache_storage.setSourceFunc(functor1<SPtr<Tex2D>, const BGCacheCfg&>(this, &uiMgr::__source_bgcache_storage));

			//_img_visfade1 = NULL;
			//_img_visfade2 = NULL;

			_visfade_speed = 1.0f * 4;

			//_current = this;
		}
		uiMgr::~uiMgr()
		{
			// start to use stl algorithms

			class detach
			{
			public:
				void		operator() (const WPtr<uiUpdateLink>& link)
				{
					link.get_unsafe()->_list = NULL;
				}
			};
			for_each(_update_items.begin(), _update_items.end(), detach());
			_update_items.clear();
		}
		//uiMgr*			uiMgr::current()
		//{
		//	return _current;
		//}
		//void				uiMgr::removeUIRef(uiBase* ui)
		//{
		//	if(_ui_capture_input==ui)
		//		_ui_capture_input=NULL;
		//	if(_ui_last_hover==ui)
		//		_ui_last_hover=NULL;
		//	if(_ui_draw_overhang==ui)
		//		_ui_draw_overhang=NULL;
		//}
		void						uiMgr::setHWND(intptr_t hwnd)
		{
			_hwnd = hwnd;
		}
		intptr_t					uiMgr::getHWND() const
		{
			Z_ASSERT( _hwnd );
			return _hwnd;
		}
		const SPtr<uiBase>&			uiMgr::getRoot() const
		{
			return _root;
		}
		//DIWrap*			uiMgr::getDI()
		//{
		//	return _diwrap;
		//}
		SPtr<uiBase>				uiMgr::getLastHoverUI() const
		{
			return _ui_last_hover.lock();
		}
		SPtr<uiBase>				uiMgr::uiAtPoint(const Vec2& pos) const
		{
			return uiAtPoint(pos - _root->getPos(), _root);
		}
		SPtr<uiBase>				uiMgr::uiAtPoint(const Vec2& pos, const SPtr<uiBase>& ui) const
		{
			if(!ui->isVisible())
				return NULL;

			if(!ui->hittest(pos))
				return NULL;

			SPtr<uiBase> ui_hit;
			SPtr<uiBase> child;
			for(ptrdiff_t i = ui->getChildCount() - 1; i >= 0 ; --i)
			{
				child = ui->getChild(i);
				if((ui_hit = uiAtPoint(pos - child->getPos(), child)))
					return ui_hit;
			}
			return ui;
		}
		void						uiMgr::update(const SPtr<Input>& input, REAL dt)
		{
			Z_ASSERT( _hwnd );

			int32_t dx, dy, dz;
			dx = input->getMouseDeltaX();
			dy = input->getMouseDeltaY();
			dz = input->getMouseDeltaZ();

			int32_t client_dx, client_dy;
			if(true)
			{
				RECT rt;
				GetClientRect((HWND)_hwnd, &rt);
				client_dx = rt.right;
				client_dy = rt.bottom;
			}

			//Ogl* ogl=Ogl::current();

			//if(update_input_state)
			//{
			//	_diwrap->processMouse();
			//	_diwrap->processKB();
			//}

			//int dx,dy,dz;
			//_diwrap->getMouseDelta(dx,dy,dz);
			if(dx || dy)
			{
				_pointer_pos.x += (REAL)dx * ((REAL)_res_dx / client_dx);
				_pointer_pos.y += (REAL)dy * ((REAL)_res_dy / client_dy);

				_pointer_pos.x = min(max(_pointer_pos.x, 0.0f), (REAL)client_dx - 1);
				_pointer_pos.y = min(max(_pointer_pos.y, 0.0f), (REAL)client_dy - 1);
			}
			if(_sync_desktop_mouse)
			{
				_pointer_pos.x = (REAL)input->getMouseX();
				_pointer_pos.y = (REAL)input->getMouseY();

				//POINT pt;
				//GetCursorPos(&pt);
				//ScreenToClient(ogl->getWindow(),&pt);

				//pt.x=min(max(pt.x,0),ogl->getWidth()-1);
				//pt.y=min(max(pt.y,0),ogl->getHeight()-1);

				//pt.x = pt.x * _res_dx / ogl->getWidth();
				//pt.y = pt.y * _res_dy / ogl->getHeight();

				//_pointer_pos.x = (REAL)pt.x;
				//_pointer_pos.y = (REAL)pt.y;

				/*
				_pointer_pos.x=min(max(_pointer_pos.x,0.0f),(REAL)_ogl->getWidth()-1);
				_pointer_pos.y=min(max(_pointer_pos.y,0.0f),(REAL)_ogl->getHeight()-1);*/
			}

			_ui_last_hover.reset();

			SPtr<uiBase> target_ui;

			if(_ui_capture_input.valid() && !_ui_capture_input.get_unsafe()->isEnabled())
				_ui_capture_input.reset();

			if(_ui_capture_input.valid())
			{
				target_ui = _ui_capture_input.lock();
				_ui_last_hover = (uiAtPoint(_pointer_pos) == target_ui ? target_ui : NULL);
			}
			else
				_ui_last_hover = target_ui = uiAtPoint(_pointer_pos);
			if(target_ui)
			{
				if(dx || dy)
				{
					if(target_ui->isEnabled())
						target_ui->onMouseMoved(_pointer_pos - target_ui->getAbsPos());
				}
				if(dz)
				{
					if(target_ui->isEnabled())
						target_ui->onMouseScroll(dz);
				}
				for(size_t i = 0; i < 3; ++i)
				{
					if(input->isButtonPressed(i))
					{
						if(i == 0)
						{
							SPtr<uiBase> ui = target_ui;
							while(ui)
							{
								if(ui->getClickToBringFront())
									ui->bringToFront();
								ui = ui->getParent();
							}
						}
						if(target_ui->isEnabled())
							target_ui->onMousePressed(_pointer_pos - target_ui->getAbsPos(), (uint32_t)i);
					}
					else if(input->isButtonReleased(i))
					{
						if(target_ui->isEnabled())
							target_ui->onMouseReleased(_pointer_pos - target_ui->getAbsPos(), (uint32_t)i);
					}
				}
			}
			if(true)				// seems redundant, but bullet proof
			{
				_temp_update_items.clear();
				class copy_update_items
				{
				private:
					vector<WPtr<uiUpdateLink>>&		_update_items;
				public:
					copy_update_items(vector<WPtr<uiUpdateLink>>& update_items): _update_items(update_items) {}
				public:
					void							operator() (const WPtr<uiUpdateLink>& item)
					{
						_update_items.push_back(item);
					}
				};
				for_each(_update_items.begin(), _update_items.end(), copy_update_items(_temp_update_items));
				class update_items
				{
				private:
					REAL							_dt;
				public:
					update_items(REAL dt)
						: _dt(dt)
					{}
				public:
					void							operator() (const WPtr<uiUpdateLink>& item)
					{
						if(item.valid())
							if(item.get_unsafe()->_ui.valid())
								item.get_unsafe()->_ui.get_unsafe()->onUpdate(_dt);
					}
				};
				for_each(_temp_update_items.begin(), _temp_update_items.end(), update_items(dt));
				_temp_update_items.clear();
			}
		}
		void						uiMgr::setCapture(const SPtr<uiBase>& ui)
		{
			_ui_capture_input = ui;

			if(_ui_draw_overhang.valid() && _ui_draw_overhang.lock() != ui)
				_ui_draw_overhang.reset();
		}
		void						uiMgr::releaseCapture(const SPtr<uiBase>& ui)
		{
			if(_ui_capture_input.valid() && _ui_capture_input.lock() == ui)
				_ui_capture_input.reset();
		}
		SPtr<uiBase>				uiMgr::getCapture() const
		{
			return _ui_capture_input.lock();
		}
		void						uiMgr::setDrawOverhang(const SPtr<uiBase>& ui)
		{
			_ui_draw_overhang = ui;
		}
		void						uiMgr::releaseDrawOverhang(const SPtr<uiBase>& ui)
		{
			if(_ui_draw_overhang.valid() && _ui_draw_overhang.lock() == ui)
				_ui_draw_overhang.reset();
		}
		SPtr<uiBase>				uiMgr::getDrawOverhang() const
		{
			return _ui_draw_overhang.lock();
		}
		const Vec2&					uiMgr::getPointerPos() const
		{
			return _pointer_pos;
		}
		void						uiMgr::setSyncDesktopMouse(bool sync)
		{
			_sync_desktop_mouse = sync;
		}
		bool						uiMgr::getSyncDesktopMouse() const
		{
			return !!_sync_desktop_mouse;
		}
		void						uiMgr::setTransforms(const Mat4& world)
		{
			Gfx* gfx = Gfx::current();
			if(!gfx)
				return;

			gfx->setWorldViewProjTransform(world, Mat4::identity, Mat4::ortho(0, (REAL)_res_dy, (REAL)_res_dx, 0, 0, 1, gfx->getZNearMapping(), gfx->getZFarMapping()));
		}
		void						uiMgr::drawAll(REAL dt)
		{
			//_frame_interval.update();

			//set2DRS();

			draw(_root, Vec2(0, 0), dt);

			if(_ui_draw_overhang.valid())
			{
				setTransforms(Mat4::identity);

				_ui_draw_overhang.get_unsafe()->onDrawOverhang();
			}
		}
		void			uiMgr::draw(const SPtr<uiBase>& ui, const Vec2& parent_abspos, REAL dt)
		{
			if(!ui->isVisibilityFadeEnabled() && !ui->isVisible())
				return;
			if(ui->isVisibilityFadeEnabled() && !ui->isVisible() && ui->getVisibility() == 0.0f)
				return;
			if(ui->getTransparency() == 0.0f)
				return;

			//Ogl* ogl=Ogl::current();

			Vec2 ui_worldpos = ui->getPos() + parent_abspos;
			//ogl->setWorldMatrix(Mat4::translation((Vec3)ui_worldpos));

			bool do_transparency = false;
			SPtr<Tex2D> bgcache;
			int32_t cache_x, cache_y, cache_dx, cache_dy;
			if((ui->isVisibilityFadeEnabled() && ui->getVisibility() != 1.0f) || ui->getTransparency() != 1.0f)
			{
				Gfx* gfx = Gfx::current();
				cache_x = (int32_t)ui_worldpos.x;
				cache_y = (int32_t)ui_worldpos.y;
				cache_dx = ui->getDX();
				cache_dy = ui->getDY();
				const rsView& view_state = gfx->getViewState();
				intersect_area(cache_x, cache_y, cache_dx, cache_dy, (int32_t)0, (int32_t)0, (int32_t)gfx->getFramebufferDX(), (int32_t)gfx->getFramebufferDY());
				bgcache = Tex2D::create(cache_dx, cache_dy, 1, GfxEnum::PIXELFORMAT_A8R8G8B8, true);
				if(bgcache)
				{
					gfx->copyToTexture(bgcache, cache_x, cache_y, cache_dx, cache_dy, 0, 0, cache_dx, cache_dy);

					do_transparency = true;
				}
			}

			bool clip = ui->getClip();
			rsView old_view;
			if(clip)
			{
				pushViewState_intersectClipRect((int32_t)ui_worldpos.x, (int32_t)ui_worldpos.y, ui->getDX(), ui->getDY());
			}

			setTransforms(Mat4::translation((Vec3)ui_worldpos));

			/*
			if(ui->getVisibilityFade())
				getOgl()->backupScreenColor(_img_visfade1,(int)ui_worldpos.x,(int)ui_worldpos.y,ui->getDX(),ui->getDY(),0,0);*/

			ui->onDraw();
			for(size_t i= 0; i < ui->getChildCount(); ++i)
				draw(ui->getChild(i), ui_worldpos, dt);

			if(clip)
			{
				popViewState();
			}

			if(do_transparency)
			{
				setTransforms(Mat4::translation((Vec3)ui_worldpos));

				Gfx* gfx = Gfx::current();

				SPtr<Shader> vs, ps;
				gfx->getImg2DShaders(vs, ps);
				gfx->useProgrammablePipeline(vs, ps);

				ShaderUniforms2 unis(vs, ps);
				unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedWorldViewProjectionTransform());

				gfx->setShaderUniforms(unis);

				rsShade rs(false);
				rs.enableBlend();
				rs.setSampler(0);

				gfx->setShadeState(rs);

				gfx->bindTexture(0, bgcache);

				Jotter jotter;
				jotter.begin(2, false, true, 1, 2);
				jotter.color(1.0f, 1.0f, 1.0f, 1.0f - (ui->getVisibility() * ui->getTransparency()));

				jotter.tc(0, 0.0f, 0.0f);
				jotter.vertex(0.0f, 0.0f);

				jotter.tc(0, 0.0f, 1.0f);
				jotter.vertex(0.0f, (REAL)ui->getDY());

				jotter.tc(0, 1.0f, 0.0f);
				jotter.vertex((REAL)ui->getDX(), 0.0f);

				jotter.tc(0, 1.0f, 0.0f);
				jotter.vertex((REAL)ui->getDX(), 0.0f);

				jotter.tc(0, 0.0f, 1.0f);
				jotter.vertex(0.0f, (REAL)ui->getDY());

				jotter.tc(0, 1.0f, 1.0f);
				jotter.vertex((REAL)ui->getDX(), (REAL)ui->getDY());

				gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, jotter);

				bgcache.reset();

				if(ui->isVisible())
					ui->setVisibility(min(ui->getVisibility() + 1.0f * dt * getVisibilityFadeSpeed(), 1.0f));
				else
					ui->setVisibility(max(ui->getVisibility() - 1.0f * dt * getVisibilityFadeSpeed(), 0.0f));
				//uiBase::draw_rect_outline((int32_t)ui_worldpos.x, (int32_t)ui_worldpos.y, ui->getDX(), ui->getDY(), Vec4(1, 1, 1, 1));
			}

			/*
			if(ui->getVisibilityFade())
			{
				getOgl()->backupScreenColor(_img_visfade2,(int)ui_worldpos.x,(int)ui_worldpos.y,ui->getDX(),ui->getDY(),0,0);
				_ogl->setWorldMatrix(Mat4::translation((Vec3)ui_worldpos));
				_img_visfade1->draw(getOgl(),0,0,ui->getDX(),ui->getDY(),0,0);
				glColor4f(1.0f,1.0f,1.0f,ui->getVisibility()*ui->getTransparency());
				_img_visfade2->draw(getOgl(),0,0,ui->getDX(),ui->getDY(),0,0);
				glColor4f(1.0f,1.0f,1.0f,1.0f);

				if(ui->isVisible())
					ui->setVisibility(min(ui->getVisibility()+1.0f*_frame_interval.elapsedTime()*getVisibilityFadeSpeed(),1.0f));
				else
					ui->setVisibility(max(ui->getVisibility()-1.0f*_frame_interval.elapsedTime()*getVisibilityFadeSpeed(),0.0f));
			}*/
		}
		void			uiMgr::setVisibilityFadeSpeed(REAL speed)
		{
			_visfade_speed = speed;
		}
		REAL			uiMgr::getVisibilityFadeSpeed() const
		{
			return _visfade_speed;
		}
		void			uiMgr::setRes(int32_t dx, int32_t dy)
		{
			_res_dx = dx;
			_res_dy = dy;
		}
		int32_t			uiMgr::getResDX() const {return _res_dx;}
		int32_t			uiMgr::getResDY() const {return _res_dy;}

		void						uiMgr::registerUpdate(const SPtr<uiBase>& ui)
		{
			SPtr<uiUpdateLink> item = new uiUpdateLink();
			item->_ui = ui;
			item->_list = &_update_items;
			item->_iter = _update_items.insert(_update_items.end(), item);

			ui->_update_link = item;
		}
		void						uiMgr::unregisterUpdate(const SPtr<uiBase>& ui)
		{
			ui->_update_link.reset();
		}
		void						uiMgr::pushViewState_intersectClipRect(int32_t x, int32_t y, int32_t dx, int32_t dy)
		{
			Gfx* gfx = Gfx::current();

			Z_ASSERT( gfx );

			rsView old_view = gfx->getViewState();
			rsView rs = old_view;
			rs.intersectClipRect(x, y, dx, dy);
			gfx->setViewState(rs);

			_stack_viewstate.push(old_view);
		}
		void						uiMgr::popViewState()
		{
			Gfx* gfx = Gfx::current();

			Z_ASSERT( gfx );

			Z_ASSERT( _stack_viewstate.size() );
			rsView old_view = _stack_viewstate.top();
			_stack_viewstate.pop();
			gfx->setViewState(old_view);
		}
		SPtr<Tex2D>					uiMgr::__source_bgcache_storage(const BGCacheCfg& cfg)
		{
			return Tex2D::create(cfg.dx, cfg.dy, 1, GfxEnum::PIXELFORMAT_A8R8G8B8, true);
		}

		//REAL			uiMgr::getPixelScaleH() const {return (REAL)Ogl::current()->getWidth()/getResDX();}
		//REAL			uiMgr::getPixelScaleV() const {return (REAL)Ogl::current()->getHeight()/getResDY();}
		//void			uiMgr::set2DRS()
		//{
		//	Ogl::current()->set2DRS(_res_dx,_res_dy);
		//}
		//void			uiMgr::reg_update(uiBase* ui)
		//{
		//	_map_ui_update[ui]=true;
		//}
		//void			uiMgr::unreg_update(uiBase* ui)
		//{
		//	_map_ui_update.erase(ui);
		//}
		//void			uiMgr::setScissorLT_push(int x,int y,int dx,int dy)
		//{
		//	Ogl::current()->setScissorLT_push(NINT(x*getPixelScaleH()),NINT(y*getPixelScaleV()),NINT(dx*getPixelScaleH()),NINT(dy*getPixelScaleV()));
		//}
		//void			uiMgr::setScissorLT_pop()
		//{
		//	Ogl::current()->setScissorLT_pop();
		//}
		//bool			uiMgr::should_process_keyboard()
		//{
		//	HWND hwnd=GetFocus();
		//	if(!hwnd)
		//		return false;
		//	if(!Ogl::current())
		//		return false;
		//	if(!Ogl::current()->getWindow())
		//		return false;
		//	if(hwnd==Ogl::current()->getWindow())
		//		return true;
		//	if(hwnd==GetParent(Ogl::current()->getWindow()))
		//		return true;
		//	return false;
		//}
	};
};

