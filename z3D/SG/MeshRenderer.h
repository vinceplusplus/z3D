
#pragma once

#include "predecl.h"
#include "Mesh_structs.h"
#include "RenderParamQuery.h"

namespace z3D
{
	namespace SG
	{
		typedef enum _DRAWPASS
		{
			DRAWPASS_DEFAULT = 0,											// macro pass, will use one or more of the following controls
			DRAWPASS_DEPTH_ONLY,											// ignore transparent items
			DRAWPASS_SHADE,													// simple forward shading
			DRAWPASS_SHADE_EQUAL_DEPTH,
			DRAWPASS_DEPTH_ONLY_SOLID_RECV_SHADOW_VSM,
			DRAWPASS_GEN_SHADOW_MAP_VSM,
			DRAWPASS_SCAN_SOLID_RECV_SHADOW_AND_SHADOW_LIGHTS,				// it will not render anything
		} DRAWPASS;

		typedef enum _DRAWCONTROL
		{
			DRAWCONTROL_SOLID = 0,
			DRAWCONTROL_TRANSPARENT,
		} DRAWCONTROL;

		class MeshRenderer: public RefBase
		{
		protected:
			virtual								~MeshRenderer() {}
		public:
			virtual void						draw(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param) = 0;
			virtual void						draw_depth_only(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param) = 0;
			virtual void						draw_depth_only_vsm(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param) = 0;
			virtual void						draw_gen_shadow_map_vsm(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param) = 0;
		};

		class DefMeshRenderer: public MeshRenderer
		{
		private:
			static SPtr<DefMeshRenderer>		_instance;
		protected:
												~DefMeshRenderer();
		public:
			static const SPtr<DefMeshRenderer>&	instance();
		public:
			void								draw(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param);
			void								draw_depth_only(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param);
			void								draw_depth_only_vsm(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param);
			void								draw_gen_shadow_map_vsm(const SPtr<Mesh>& mesh, size_t submesh, RenderParamQuery& param);
		public:
			void								setSamplerParam(rsShade& rs, size_t sampler, const MtlMap& mtlmap);
		};
	};
};


