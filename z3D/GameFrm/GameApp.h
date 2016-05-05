
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace GameFrm
	{
		class GameApp: public RefBase
		{
		protected:
			SPtr<GameWnd>		_game_wnd;
			SPtr<GameView>		_game_view;
		private:
			char				_cursor_buffer[4096];
			char				_cursor_buffer2[4096];
			intptr_t			_hcur;
		protected:
			SPtr<Input>			_input;
		private:
			FPSCtrl				_fpsctrl_update;
			size_t				_lockstep_update;

			Timer				_non_lockstep_timer;
		private:
			bool				_sleep;
			uint32_t			_sleep_time;
		protected:
			SPtr<Gfx>			_gfx;
			SPtr<SMgr>			_smgr;
		private:
			wcs					_title;
			int32_t				_colorbits;
			bool				_fullscreen;
		private:
			bool				_cap_draw_fps;
		public:
			GameApp();
		protected:
			~GameApp();
		private:
			static bool			changeFullscreen(int32_t dx, int32_t dy, int32_t color_bits);
		private:
			SPtr<GameWnd>		createGameWnd(const wcs& title, bool fullscreen, int32_t dx, int32_t dy, int32_t colorbits);
			SPtr<GameView>		createGameView(intptr_t parent, int32_t dx, int32_t dy);
		public:
			bool				create(const wcs& title, bool fullscreen, int32_t dx, int32_t dy, int32_t colorbits, bool vsync);
		public:
			void				gameLoop();
		private:
			void				flowControl();
		public:
			void				setUpdateFPS(REAL fps);					// use 0.0 as fps to disable lockstep
			void				setCapDrawFPS(bool cap);
		protected:
			virtual void		update(REAL dt);
			virtual void		draw(REAL dt);
			virtual bool		post_create();
		protected:
			virtual void		onGameLoopStart();
			virtual bool		onGameLoopEnd();
		public:
			intptr_t			getGameWindow() const;
			intptr_t			getGameView() const;
		public:
			bool				fullscreen() const;
			int32_t				dx() const;
			int32_t				dy() const;
		public:
			Vec2				getMousePosInPS() const;				// in projection space
		public:
			void				switchFullscreen(bool fullscreen);
		public:
			void				setSleep(bool sleep, uint32_t sleep_time_ms);
		public:
			void				cancelPendingTimeSoFar();
		};
	};
};


