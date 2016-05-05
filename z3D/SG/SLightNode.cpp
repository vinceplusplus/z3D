
#include "SLightNode.h"
#include "SMgr.h"
#include "SCameraNode.h"
#include "LightCtrl.h"

namespace z3D
{
	namespace SG
	{
		SLightNode::SLightNode()
		{
			_type=LIGHTTYPE_POINT;
			_multiplier = 1;
			_radius = 1;
			_enabled = true;
			_light_cast_shadow = false;
			_inner_angle = 60;
			_outer_angle = 60;
			_beam = 40;
			_dist = 50;
			_priority = 0;
		}
		SLightNode::~SLightNode()
		{
		}
		SPtr<SLightNode>		SLightNode::create_ambient(const Vec3& color, REAL multiplier, REAL radius)
		{
			SPtr<SLightNode> n=new SLightNode();
			n->_type = LIGHTTYPE_AMBIENT;
			n->_color = color;
			n->_multiplier = multiplier;
			n->setRadius(radius);
			n->_light_cast_shadow = false;

			return n;
		}
		SPtr<SLightNode>		SLightNode::create_dir(const Vec3& color, REAL multiplier, REAL radius, REAL beam, REAL dist, bool light_cast_shadow)
		{
			SPtr<SLightNode> n=new SLightNode();
			n->_type = LIGHTTYPE_DIRECTIONAL;
			n->_color = color;
			n->_multiplier = multiplier;
			n->setRadius(radius);
			n->_beam = beam;
			n->_dist = dist;
			n->_light_cast_shadow = light_cast_shadow;

			return n;
		}
		SPtr<SLightNode>		SLightNode::create_point(const Vec3& color,REAL multiplier,REAL radius,bool light_cast_shadow)
		{
			SPtr<SLightNode> n=new SLightNode();
			n->_type = LIGHTTYPE_POINT;
			n->_color = color;
			n->_multiplier = multiplier;
			n->setRadius(radius);
			n->_light_cast_shadow = light_cast_shadow;

			return n;
		}
		SPtr<SLightNode>		SLightNode::create_spot(const Vec3& color,REAL multiplier,REAL radius,REAL inner_angle,REAL outer_angle,bool light_cast_shadow)
		{
			SPtr<SLightNode> n=new SLightNode();
			n->_type = LIGHTTYPE_SPOT;
			n->_color = color;
			n->_multiplier = multiplier;
			n->setRadius(radius);
			n->_light_cast_shadow = light_cast_shadow;
			n->setInnerAngle(inner_angle);
			n->setOuterAngle(outer_angle);

			return n;
		}
		SLightNode::LIGHTTYPE	SLightNode::light_type() const {return _type;}
		const AABB&	SLightNode::local_bound()
		{
			return _local_bound;
		}
		bool			SLightNode::renderable() const
		{
			return true;
		}
		void			SLightNode::preRenderLighting(SRenderInfo* ri)
		{
			if(!getEnabled())
				return;
			ri->regLightItem(this);
		}
		void			SLightNode::setColor(const Vec3& color) {_color=color;}
		void			SLightNode::setMultiplier(REAL multiplier) {_multiplier=multiplier;}
		void			SLightNode::setRadius(REAL radius)
		{
			_radius = radius;
			_local_bound = AABB(Vec3(-_radius, -_radius, -_radius), Vec3(_radius, _radius, _radius));
			markDirtyBound();
		}
		void			SLightNode::setEnabled(bool enabled) {_enabled=enabled;}
		void			SLightNode::setLightCastShadow(bool cast) {_light_cast_shadow=cast;}
		void			SLightNode::setInnerAngle(REAL angle) {_inner_angle = CLAMP(angle,0,180);_outer_angle=CLAMP(_outer_angle,_inner_angle,180);}
		void			SLightNode::setOuterAngle(REAL angle) {_outer_angle = CLAMP(angle,0,180);_inner_angle=CLAMP(_inner_angle,0,_outer_angle);}
		void			SLightNode::setBeam(REAL beam) {_beam = beam;}
		void			SLightNode::setDist(REAL dist) {_dist = dist;}
		void			SLightNode::setPriority(ptrdiff_t priority) {_priority = priority;}
		const Vec3&		SLightNode::getColor() const {return _color;}
		REAL			SLightNode::getMultiplier() const {return _multiplier;}
		REAL			SLightNode::getRadius() const {return _radius;}
		bool			SLightNode::getEnabled() const {return _enabled;}
		bool			SLightNode::getLightCastShadow() const {return _light_cast_shadow;}
		REAL			SLightNode::getInnerAngle() const {return _inner_angle;}
		REAL			SLightNode::getOuterAngle() const {return _outer_angle;}
		REAL			SLightNode::getBeam() const {return _beam;}
		REAL			SLightNode::getDist() const {return _dist;}
		ptrdiff_t		SLightNode::getPriority() const {return _priority;}
		SPtr< AnimData<Vec3> >&	SLightNode::animdata_color() {return _animdata_color;}
		SPtr< AnimData<REAL> >&	SLightNode::animdata_multiplier() {return _animdata_multiplier;}
		SPtr< AnimData<REAL> >&	SLightNode::animdata_radius() {return _animdata_radius;}
		SPtr< AnimData<REAL> >&	SLightNode::animdata_inner_cone() {return _animdata_inner_cone;}
		SPtr< AnimData<REAL> >&	SLightNode::animdata_outer_cone() {return _animdata_outer_cone;}
		void			SLightNode::buildControllers(const wcs& node_name)
		{
			if(!_animdata_color && !_animdata_multiplier && !_animdata_radius && !_animdata_inner_cone && !_animdata_outer_cone)
				return;

			SPtr<LightCtrl> c = new LightCtrl(wcs(L"LightCtrl: ") + node_name, this);
			c->setAnimDataColor(_animdata_color);
			c->setAnimDataMultiplier(_animdata_multiplier);
			c->setAnimDataRadius(_animdata_radius);
			c->setAnimDataInnerAngle(_animdata_inner_cone);
			c->setAnimDataOuterAngle(_animdata_outer_cone);

			if(true)
			{
				REAL start, end;
				c->getAnimDataTimeRange(start, end);
				c->getAnimCtrl()->set(TimeAnimCfg(start, end, -1));
			}

			getCtrlSet().attachCtrl(c);
		}
	};
};

