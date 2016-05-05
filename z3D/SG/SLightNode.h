
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		// for directional light's direction, its (0 0 -1) * ((Mat3)world_trans()).inverse().transpose()
		class SLightNode:public SNode
		{
		public:
			typedef enum
			{
				LIGHTTYPE_DIRECTIONAL = 1,
				LIGHTTYPE_POINT,
				LIGHTTYPE_SPOT,
				LIGHTTYPE_AMBIENT,
			} LIGHTTYPE;
		public:
			DECL_TYPEINFO();
		protected:
			LIGHTTYPE		_type;
			Vec3			_color;
			REAL			_multiplier;
			REAL			_radius;				// for point light
			REAL			_inner_angle;			// for spot light
			REAL			_outer_angle;			// for spot light
			REAL			_beam;					// for dir light
			REAL			_dist;					// for dir light
			union
			{
				struct
				{
					bool	_enabled;
					bool	_light_cast_shadow;
				};
				unsigned long	__UNNAMED0;
			};
		protected:
			ptrdiff_t		_priority;
		protected:
			AABB			_local_bound;
		protected:
			SPtr< AnimData<Vec3> >	_animdata_color;
			SPtr< AnimData<REAL> >	_animdata_multiplier;
			SPtr< AnimData<REAL> >	_animdata_radius;
			SPtr< AnimData<REAL> >	_animdata_inner_cone;
			SPtr< AnimData<REAL> >	_animdata_outer_cone;
		protected:
			//static vector<SLightNode*>		_array_global_light;						// cannot only use raw pointer, because make use of WPtr, needs a SPtr, but a SPtr constructed inside constructor will eventually be destroyed, ie the constructing object deletes itself prematurally
		protected:
			SLightNode();
		protected:
			~SLightNode();
		public:
			static SPtr<SLightNode>	create_ambient(const Vec3& color, REAL multiplier, REAL radius);
			static SPtr<SLightNode>	create_dir(const Vec3& color, REAL multiplier, REAL radius, REAL beam, REAL dist, bool light_cast_shadow);
			static SPtr<SLightNode>	create_point(const Vec3& color, REAL multiplier, REAL radius, bool light_cast_shadow);
			static SPtr<SLightNode>	create_spot(const Vec3& color, REAL multiplier, REAL radius, REAL inner_angle, REAL outer_angle, bool light_cast_shadow);
		public:
			LIGHTTYPE		light_type() const;
		public:
			const AABB&		local_bound();
		public:
			bool			renderable() const;
		public:
			void			preRenderLighting(SRenderInfo* ri);
		public:
			void			setColor(const Vec3& color);
			void			setMultiplier(REAL multiplier);
			void			setRadius(REAL radius);
			void			setEnabled(bool enabled);
			void			setLightCastShadow(bool cast);
			void			setInnerAngle(REAL angle);
			void			setOuterAngle(REAL angle);
			void			setBeam(REAL beam);
			void			setDist(REAL dist);
			void			setPriority(ptrdiff_t priority);
		public:
			const Vec3&		getColor() const;
			REAL			getMultiplier() const;
			REAL			getRadius() const;
			bool			getEnabled() const;
			bool			getLightCastShadow() const;
			REAL			getInnerAngle() const;
			REAL			getOuterAngle() const;
			REAL			getBeam() const;
			REAL			getDist() const;
			ptrdiff_t		getPriority() const;
		public:
			SPtr< AnimData<Vec3> >&	animdata_color();
			SPtr< AnimData<REAL> >&	animdata_multiplier();
			SPtr< AnimData<REAL> >&	animdata_radius();
			SPtr< AnimData<REAL> >&	animdata_inner_cone();
			SPtr< AnimData<REAL> >&	animdata_outer_cone();
		public:
			void			buildControllers(const wcs& node_name);
		};
	};
};

