
#include "uiBase.h"
#include "uiMgr.h"

#pragma warning(disable:4355)

namespace z3D
{
	namespace GameFrm
	{
		uiBase::uiBase()
		{
			SPtrLocker::add(this);

			_visible = true;
			_enabled = true;
			_topmost = false;
			_visibility_fade = false;
			_clip = false;
			_click_to_bring_front = true;

			_visibility = 1.0f;
			_transparency = 1.0f;
		}
		uiBase::~uiBase()
		{
			_children.clear();
		}
		void				uiBase::setPos(const Vec2& pos) {_pos = pos;}
		void				uiBase::setExt(const Vec2& ext) {_ext = ext;}
		void				uiBase::setPos(int32_t x, int32_t y){_pos.x = (REAL)x; _pos.y = (REAL)y;}
		void				uiBase::setExt(int32_t dx, int32_t dy){_ext.x = (REAL)dx; _ext.y = (REAL)dy;}
		const Vec2&			uiBase::getPos() const {return _pos;}
		const Vec2&			uiBase::getExt() const {return _ext;}
		int32_t				uiBase::getX() const {return (int32_t)_pos.x;}
		int32_t				uiBase::getY() const {return (int32_t)_pos.y;}
		int32_t				uiBase::getDX() const {return (int32_t)_ext.x;}
		int32_t				uiBase::getDY() const {return (int32_t)_ext.y;}
		bool				uiBase::isVisible() const {return _visible && (_parent.valid() ? _parent.get_unsafe()->isVisible() : true);}
		void				uiBase::setVisible(bool visible) {_visible = visible; if(!isVisibilityFadeEnabled()) _visibility = _visible ? 1.0f : 0.0f;}
		bool				uiBase::isEnabled() const {return _enabled && (_parent.valid() ? _parent.get_unsafe()->isEnabled() : true);}
		void				uiBase::setEnabled(bool enabled) {_enabled = enabled;}
		bool				uiBase::isTopMost() const {return _topmost;}
		void				uiBase::setTopMost(bool topmost) {_topmost = topmost;}
		REAL				uiBase::getVisibility() const {return _visibility;}
		void				uiBase::setVisibility(REAL visibility) {_visibility = visibility;}
		bool				uiBase::isVisibilityFadeEnabled() const {return _visibility_fade;}
		void				uiBase::enableVisibilityFade(bool enable) {_visibility_fade = enable;}
		bool				uiBase::getClip() const {return _clip;}
		void				uiBase::setClip(bool clip) {_clip = clip;}
		bool				uiBase::getClickToBringFront() const {return _click_to_bring_front;}
		void				uiBase::setClickToBringFront(bool click_to_bring_front) {_click_to_bring_front = click_to_bring_front;}
		REAL				uiBase::getTransparency() const {return _transparency;}
		void				uiBase::setTransparency(REAL transparency) {_transparency = transparency;}
		Vec2				uiBase::getAbsPos()
		{
			if(_parent.valid())
				return _pos + _parent.get_unsafe()->getAbsPos();
			return _pos;
		}
		SPtr<uiBase>		uiBase::getParent() {return _parent.lock();}
		SPtr<uiBase>		uiBase::getChild(ptrdiff_t i) const {return _children[i];}
		ptrdiff_t			uiBase::getChildIndex(const SPtr<uiBase>& child)
		{
			for(size_t i = 0; i < getChildCount(); ++i)
				if(getChild(i) == child)
					return i;

			return -1;
		}
		size_t				uiBase::getChildCount() {return _children.size();}
		void				uiBase::insertChildAt(const SPtr<uiBase>& ui, ptrdiff_t pos)
		{
			if(ui->_parent.valid() && ui->_parent.get_unsafe() == this)
				return;
			if(ui->_parent.valid())
				ui->_parent.get_unsafe()->removeChild(ui->_parent.get_unsafe()->getChildIndex(ui));

			_children.insert(_children.begin() + pos, ui);
			ui->_parent = SPtr<uiBase>(this);
		}
		void				uiBase::addChild(const SPtr<uiBase>& ui)
		{
			if(ui->_parent.valid() && ui->_parent.get_unsafe() == this)
				return;
			if(ui->_parent.valid())
				ui->_parent.get_unsafe()->removeChild(ui->_parent.get_unsafe()->getChildIndex(ui));

			if(!ui->_topmost)
			{
				size_t ok = false;
				vector<SPtr<uiBase>>::iterator iter;
				for(iter = _children.begin(); iter != _children.end(); ++iter)
					if((*iter)->isTopMost())
					{
						_children.insert(iter, ui);
						ok = true;
						break;
					}
				if(!ok)
					_children.push_back(ui);
			}
			else
				_children.push_back(ui);

			ui->_parent = SPtr<uiBase>(this);
		}
		void				uiBase::removeChild(ptrdiff_t i)
		{
			if(i < 0 || i >= (ptrdiff_t)_children.size())
				return;
			_children[i]->_parent.reset();
			_children.erase(_children.begin() + i);
		}
		void				uiBase::bringToFront()
		{
			if(!_parent.valid())
				return;
			ptrdiff_t p = _parent.get_unsafe()->getChildIndex(this);
			if(p == -1)
				return;

			SPtr<uiBase> ui = this;
			SPtr<uiBase> parent = _parent.lock();
			parent->removeChild(p);
			parent->addChild(ui);
		}
		void				uiBase::bringToBack()
		{
			if(!_parent.valid())
				return;
			ptrdiff_t p = _parent.get_unsafe()->getChildIndex(this);
			if(p == -1)
				return;
			SPtr<uiBase> parent = _parent.lock();
			SPtr<uiBase> ui = this;
			parent->removeChild(p);
			if(_topmost)
			{
				setTopMost(false);
				parent->addChild(ui);
				setTopMost(true);
			}
			else
				parent->insertChildAt(ui, 0);
		}
		void				uiBase::alignCenter()
		{
			if(!_parent.valid())
				return;
			setPos((_parent.get_unsafe()->getDX() - getDX()) >> 1,(_parent.get_unsafe()->getDY() - getDY()) >> 1);
		}
		bool				uiBase::hittest(const Vec2& pos)
		{
			if(pos.x >= 0 && pos.y >= 0 && pos.x < _ext.x && pos.y < _ext.y)
				return true;
			return false;
		}
		bool				uiBase::pointInRect(const Vec2& point, const Vec2& rt_pos, const Vec2& rt_ext)
		{
			if(point.x >= rt_pos.x && point.y >= rt_pos.y && point.x < rt_pos.x + rt_ext.x && point.y < rt_pos.y + rt_ext.y)
				return true;
			return false;
		}
		void				uiBase::onMousePressed(const Vec2& pos, uint32_t button) {}
		void				uiBase::onMouseReleased(const Vec2& pos, uint32_t button) {}
		void				uiBase::onMouseMoved(const Vec2& pos) {}
		void				uiBase::onMouseScroll(int32_t amount) {}
		void				uiBase::onDraw() {}
		void				uiBase::onDrawOverhang() {}
		void				uiBase::onUpdate(REAL dt) {}
		void				uiBase::draw_rect(int32_t x, int32_t y, int32_t dx, int32_t dy, const Vec4& color)
		{
			Gfx* gfx = Gfx::current();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getPassThruShaders(vs, ps);
				gfx->useProgrammablePipeline(vs, ps);
				// presume the world transform
			}

			if(true)
			{
				rsShade rs(false);
				gfx->setShadeState(rs);
			}

			ShaderUniforms2 uni(vs, ps);
			uni.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedWorldViewProjectionTransform());

			gfx->setShaderUniforms(uni);

			Jotter jotter;
			jotter.begin(2, false, true, 0);

			jotter.color(color);

			jotter.vertex((REAL)x,			(REAL)y			);
			jotter.vertex((REAL)x,			(REAL)y + dy	);
			jotter.vertex((REAL)x + dx,		(REAL)y			);
			jotter.vertex((REAL)x + dx,		(REAL)y + dy	);

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLE_STRIP, jotter);
		}
		void				uiBase::draw_rect(const Vec2& pos, const Vec2& ext, const Vec4& color)
		{
			draw_rect((int32_t)pos.x,(int32_t)pos.y,(int32_t)ext.x,(int32_t)ext.y,color);
		}
		void				uiBase::draw_rect_outline(int32_t x, int32_t y, int32_t dx, int32_t dy, const Vec4& color)
		{
			draw_rect(x, y, dx, 1, color);
			draw_rect(x, y, 1, dy, color);
			draw_rect(x, y + dy - 1, dx, 1, color);
			draw_rect(x + dx - 1, y, 1, dy, color);
		}
		void				uiBase::draw_rect_outline(const Vec2& pos, const Vec2& ext, const Vec4& color)
		{
			draw_rect_outline((int32_t)pos.x, (int32_t)pos.y, (int32_t)ext.x, (int32_t)ext.y,color);
		}

		void			uiBase::draw_line(const Vec2& v0, const Vec2& v1, const Vec4& color)
		{
			Gfx* gfx = Gfx::current();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getPassThruShaders(vs, ps);
				gfx->useProgrammablePipeline(vs, ps);
				// presume the world transform
			}

			if(true)
			{
				rsShade rs(false);
				gfx->setShadeState(rs);
			}

			ShaderUniforms2 uni(vs, ps);
			uni.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->getAdjustedWorldViewProjectionTransform());

			gfx->setShaderUniforms(uni);

			Jotter jotter;
			jotter.begin(2, false, true, 0);

			jotter.color(color);

			jotter.vertex(v0);
			jotter.vertex(v1);

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_LINES, jotter);
		}
	};
};


#pragma warning(default:4355)

