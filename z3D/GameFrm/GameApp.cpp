
#define _WIN32_WINNT	0x0501

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "GameApp.h"
#include "GameWnd.h"
#include "GameView.h"
#include "Input.h"

#include "W32Wnd.h"

#undef min
#undef max

namespace z3D
{
	namespace GameFrm
	{
		GameApp::GameApp()
		{
			Z_STATIC_ASSERT( sizeof(HCURSOR) == sizeof(_hcur), WRONG_ASSUMPTION );
			Z_STATIC_ASSERT( sizeof(HWND) == sizeof(intptr_t), WRONG_ASSUMPTION );

			_sleep = true;
			_sleep_time = 1;

			_fullscreen = false;
			_colorbits = 0;

			memset(_cursor_buffer, 0xff, sizeof(_cursor_buffer));
			memset(_cursor_buffer2, 0x0, sizeof(_cursor_buffer2));
			_hcur = (intptr_t)CreateCursor(NULL, 0, 0, GetSystemMetrics(SM_CXCURSOR), GetSystemMetrics(SM_CYCURSOR), _cursor_buffer, _cursor_buffer2);

			_cap_draw_fps = true;

			_lockstep_update = true;
			setUpdateFPS(60);
		}
		GameApp::~GameApp()
		{
			_input.reset();
			_game_view.reset();
			_game_wnd.reset();

			if(_hcur)
				DestroyCursor((HCURSOR)_hcur);

			ChangeDisplaySettings(NULL, 0);
		}
		bool					GameApp::changeFullscreen(int32_t dx, int32_t dy, int32_t color_bits)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = dx;
			dmScreenSettings.dmPelsHeight = dy;
			dmScreenSettings.dmBitsPerPel = color_bits;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH|DM_PELSHEIGHT;
			if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				if(color_bits == 24)
				{
					dmScreenSettings.dmBitsPerPel = 32;
					if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
					{
						return false;
					}
				}
				else
					return false;
			}
			return true;
		}
		SPtr<GameWnd>			GameApp::createGameWnd(const wcs& title, bool fullscreen, int32_t dx, int32_t dy, int32_t colorbits)
		{
			SPtr<GameWnd> game_wnd = new GameWnd();

			bool succ = false;
			do
			{
				DWORD style = 0;
				DWORD ex_style = 0;
				if(!fullscreen)
				{
					ChangeDisplaySettings(NULL, 0);

					ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
					style = WS_OVERLAPPEDWINDOW | WS_OVERLAPPEDWINDOW & (~(WS_SIZEBOX | WS_MAXIMIZEBOX));
				}
				else
				{
					if(!changeFullscreen(dx, dy, colorbits))
						break;

					ex_style = WS_EX_APPWINDOW;
					style = WS_POPUP;
				}
				if(true)
				{
					RECT rt = {0, 0, dx, dy};
					AdjustWindowRectEx(&rt, style, FALSE, ex_style);
					int dx2 = rt.right - rt.left;
					int dy2 = rt.bottom - rt.top;
					if(!game_wnd->getWnd()->createCustom(L"GAMEWND", NULL, LoadCursor(NULL, IDC_ARROW), NULL, title, style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, ex_style, 0, 0, dx2, dy2, NULL, NULL, NULL))
						break;
					game_wnd->getWnd()->alignCenter();
					game_wnd->getWnd()->setVisible(true);
				}
				succ = true;
			}while(false);

			if(!succ)
			{
				if(game_wnd)
					game_wnd.reset();
				return NULL;
			}
			return game_wnd;
		}
		SPtr<GameView>		GameApp::createGameView(intptr_t parent, int32_t dx, int32_t dy)
		{
			SPtr<GameView> game_view = new GameView();
			if(!game_view->getWnd()->createCustom(L"GAMEVIEW", NULL, (HCURSOR)_hcur, NULL, L"", WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 0, dx, dy, (HWND)parent, NULL, NULL))
			{
				game_view.reset();
				return NULL;
			}
			return game_view;
		}
		bool				GameApp::create(const wcs& title, bool fullscreen, int32_t dx, int32_t dy, int32_t colorbits, bool vsync)
		{
			if(_game_wnd)
				return false;

			SPtr<GameWnd> game_wnd = createGameWnd(title, fullscreen, dx, dy, colorbits);
			if(!game_wnd)
				return false;
			SPtr<GameView> game_view = createGameView((intptr_t)game_wnd->getWnd()->getHWND(), dx, dy);
			if(!game_view)
				return false;

			SPtr<Input> input = new Input((intptr_t)game_wnd->getWnd()->getHWND());

			game_wnd->setInput(input);

			SPtr<Gfx> gfx = Gfx::createD3D((intptr_t)game_view->getWnd()->getHWND(), vsync);
			if(!gfx)
			{
				input.reset();
				game_view.reset();
				game_wnd.reset();

				return false;
			}

			_game_wnd = game_wnd;
			_game_view = game_view;

			_input = input;
			_gfx = gfx;

			_fullscreen = fullscreen;

			_title = title;
			_colorbits = colorbits;

			_smgr = new SMgr();

			if(!post_create())
				return false;

			return true;
		}
		void				GameApp::gameLoop()
		{
			onGameLoopStart();

			_fpsctrl_update.start();

			MSG msg;
			while(true)
			{
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if(msg.message == WM_QUIT)
					{
						if(onGameLoopEnd())
							return;
					}
					if(msg.message == WM_MOUSEWHEEL)
						_input->addPendingZDelta((short)HIWORD(msg.wParam));
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				flowControl();
			}
		}
		void				GameApp::flowControl()
		{
			SPtrLocker::clear();

			size_t update_frames = 0;
			double advancement = 0;

			if(_lockstep_update)
			{
				_fpsctrl_update.update();

				while(_fpsctrl_update.nextFrame())
				{
					update((REAL)_fpsctrl_update.frame_length());
					++update_frames;
				}

				advancement = _fpsctrl_update.frame_length() * update_frames;
			}
			else
			{
				_non_lockstep_timer.update();
				update(_non_lockstep_timer.elapsedTime());

				update_frames = 1;
				advancement = _non_lockstep_timer.elapsedTimeDOUBLE();
			}
			if(_cap_draw_fps)
			{
				if(update_frames)
				{
					if(_game_view->dx() != _game_wnd->getWnd()->getWidth() || _game_view->dy() != _game_wnd->getWnd()->getHeight())
						_game_view->getWnd()->setSize(_game_wnd->getWnd()->getWidth(), _game_wnd->getWnd()->getHeight());

					draw((REAL)advancement);
					//_gfx->present();
				}
			}
			else
			{
				if(update_frames)
					draw((REAL)advancement);
				else
					draw(max(0.001f, _fpsctrl_update.getTimer().elapsedTime()));
				//_gfx->present();
			}

			if(_sleep)
				Sleep(_sleep_time);
		}
		void				GameApp::setUpdateFPS(REAL fps)
		{
			if(fps != 0.0f)
			{
				_fpsctrl_update.setFPS(fps);
				_lockstep_update = true;
			}
			else
			{
				if(_lockstep_update)
				{
					_lockstep_update = false;
					_non_lockstep_timer.update();
				}
			}
		}
		void				GameApp::setCapDrawFPS(bool cap)
		{
			_cap_draw_fps = cap;
		}
		void				GameApp::update(REAL dt) {}
		void				GameApp::draw(REAL dt) {}
		bool				GameApp::post_create() {return false;}
		void				GameApp::onGameLoopStart() {}
		bool				GameApp::onGameLoopEnd() {return true;}
		intptr_t			GameApp::getGameWindow() const {return (intptr_t)_game_wnd->getWnd()->getHWND();}
		intptr_t			GameApp::getGameView() const {return (intptr_t)_game_view->getWnd()->getHWND();}
		bool				GameApp::fullscreen() const {return _fullscreen;}
		int32_t				GameApp::dx() const {return _game_view->dx();}
		int32_t				GameApp::dy() const {return _game_view->dy();}
		Vec2				GameApp::getMousePosInPS() const
		{
			return Vec2(SATURATE((REAL)_input->getMouseX() / (dx() - 1) * 2.0f - 1.0f), SATURATE((REAL)(dy() - 1 - _input->getMouseY()) / (dy() - 1) * 2.0f - 1.0f));
		}
		void				GameApp::switchFullscreen(bool fullscreen)
		{
			if(fullscreen == _fullscreen)
				return;
			SPtr<GameWnd> wnd = createGameWnd(_title, fullscreen, dx(), dy(), _colorbits);
			if(!wnd)
				return;
			SetParent(_game_view->getWnd()->getHWND(), wnd->getWnd()->getHWND());
			_input->reinit((intptr_t)wnd->getWnd()->getHWND(), false);
			wnd->setInput(_input);
			_game_wnd = wnd;
			_fullscreen = fullscreen;

			_gfx->notifyDisplaySettingsChanged();
		}
		void				GameApp::setSleep(bool sleep, uint32_t sleep_time_ms)
		{
			_sleep = sleep;
			_sleep_time = sleep_time_ms;
		}
		void				GameApp::cancelPendingTimeSoFar()
		{
			_fpsctrl_update.start();
			_non_lockstep_timer.update();
		}
	};
};


