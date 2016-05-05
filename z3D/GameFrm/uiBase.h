
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace GameFrm
	{
		class uiUpdateLink;

		class uiBase: public RefBase//: public uiMouseHandler, public uiCommandHandler, public uiRenderHandler, public uiUpdateHandler
		{
		private:
			WPtr<uiBase>				_parent;
		private:
			Vec2						_pos;
			Vec2						_ext;
		private:
			struct
			{
				union
				{
					size_t						_FLAGS;
					struct
					{
						bool					_visible:1;
						bool					_enabled:1;
						bool					_topmost:1;
						bool					_visibility_fade:1;
						bool					_clip:1;
						bool					_click_to_bring_front:1;
					};
				};
			};
			REAL						_visibility;
			REAL						_transparency;
		private:
			vector<SPtr<uiBase>>		_children;
		protected:
			//uiUserData*			_user_data;
		private:
			SPtr<uiUpdateLink>			_update_link;
		public:
			uiBase();
		protected:
			~uiBase();
		public:
			void				setPos(const Vec2& pos);
			void				setExt(const Vec2& ext);
			void				setPos(int32_t x, int32_t y);
			void				setExt(int32_t dx, int32_t dy);
			const Vec2&			getPos() const;
			const Vec2&			getExt() const;
			int32_t				getX() const;
			int32_t				getY() const;
			int32_t				getDX() const;
			int32_t				getDY() const;
			bool				isVisible() const;
			void				setVisible(bool visible);
			bool				isEnabled() const;
			void				setEnabled(bool enabled);
			bool				isTopMost() const;
			void				setTopMost(bool topmost);
			bool				isVisibilityFadeEnabled() const;
			void				enableVisibilityFade(bool enable);
			bool				getClip() const;
			void				setClip(bool clip);
			bool				getClickToBringFront() const;
			void				setClickToBringFront(bool click_to_bring_front);
		public:
			REAL				getTransparency() const;
			void				setTransparency(REAL transparency);
		public:
			REAL				getVisibility() const;						// intended only for uiMgr use
			void				setVisibility(REAL visibility);				// intended only for uiMgr use
		public:
			Vec2				getAbsPos();
		public:
			SPtr<uiBase>		getParent();
			SPtr<uiBase>		getChild(ptrdiff_t i) const;				// return const reference from a vector is very dangerous as you may later resize the vector then the const reference is dangling
			ptrdiff_t			getChildIndex(const SPtr<uiBase>& child);
			size_t				getChildCount();
			void				insertChildAt(const SPtr<uiBase>& ui, ptrdiff_t pos);
			void				addChild(const SPtr<uiBase>& ui);
			void				removeChild(ptrdiff_t i);
		public:
			void				bringToFront();
			void				bringToBack();
			void				alignCenter();
		public:
			virtual bool		hittest(const Vec2& pos);
		public:
			static bool			pointInRect(const Vec2& point, const Vec2& rt_pos, const Vec2& rt_ext);
		protected:
			virtual void		onMousePressed(const Vec2& pos, uint32_t button);
			virtual void		onMouseReleased(const Vec2& pos, uint32_t button);
			virtual void		onMouseMoved(const Vec2& pos);
			virtual void		onMouseScroll(int32_t amount);
		protected:
			virtual void		onDraw();
			virtual void		onDrawOverhang();
		protected:
			virtual void		onUpdate(REAL dt);
		public:
			//uiUserData*		setUserData(uiUserData* user_data);
			//uiUserData*		getUserData() const;
			//template<class T>
			//	T&			user_data() const {return *(T*)_user_data;}
		public:
			static void			draw_rect(int32_t x, int32_t y, int32_t dx, int32_t dy, const Vec4& color);
			static void			draw_rect(const Vec2& pos, const Vec2& ext, const Vec4& color);
			static void			draw_rect_outline(int32_t x, int32_t y, int32_t dx, int32_t dy, const Vec4& color);
			static void			draw_rect_outline(const Vec2& pos, const Vec2& ext, const Vec4& color);
			static void			draw_line(const Vec2& v0, const Vec2& v1, const Vec4& color);
		public:
			friend class uiMgr;
		};
	};
};

