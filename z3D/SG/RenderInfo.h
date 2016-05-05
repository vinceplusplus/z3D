
#pragma once

#include "predecl.h"

#include "SCameraNode.h"
#include "MeshRenderer.h"
#include "RenderParamQuery.h"

namespace z3D
{
	namespace SG
	{
		class SRenderInfo;

		class SRenderInfoStatic
		{
		private:
			class SRenderInfoStaticsDestroyer
			{
			public:
				~SRenderInfoStaticsDestroyer();
			};
		protected:
			static vector<SRenderInfo*>			_array_ri;
			static vector<SRenderInfo*>			_array_free_ri;
			static SRenderInfoStaticsDestroyer	_destroyer;
		private:
			SRenderInfoStatic();
		private:
			static SRenderInfo*					alloc();
			static void							dealloc(SRenderInfo* info);
		public:
			friend class SRenderInfo;
		};

		class SRenderItem
		{
		public:
			SNode*				node;
			intptr_t			parameter;								// most likely the index of submesh to render
			REAL				cam_dist;
			ptrdiff_t			layer_index;
		};

		class SRenderLightItem
		{
		public:
			SLightNode*			node;
			ptrdiff_t			visibility_channel;
		};

		class SLightsChooser
		{
		public:
			static void					choose(SLightNode** first, SLightNode** last, const Vec3& o, SLightNode** out_buffer, size_t& buffer_count);
			static void					choose(SRenderLightItem* first, SRenderLightItem* last, const Vec3& o, SRenderLightItem** out_buffer, size_t& buffer_count);
		};

		class SRenderInfo
		{
		public:
			class SceneInput
			{
			public:
				vector<SRenderItem>						array_solid_item;
				vector<SRenderItem>						array_transparent_item;
				vector<SRenderLightItem>				array_light_item;
			public:
				void									clear();
			};
		private:
			DRAWPASS								_pass;
		private:
			SCameraNode*							_camera;
			Vec3									_camera_world_pos;
			Vec3									_camera_world_dir;
		private:
			SceneInput								_scene_input;
		private:
			AABB									_solid_bound;


		private:
			struct _RENDERING
			{
				SNode*									node;
				DRAWCONTROL								dc;
			}										_rendering;
		private:
			class LightVisibility
			{
			public:
				SPtr<Tex2D>								tex;
				SLightNode*								sorted_channels[4];
				size_t									channel_count;
			}										_light_visibility;
		private:
			union
			{
				uint64_t																	__FLAGS;
				struct
				{
					bool									_reverse_cull : 1;
				};
			};
		public:
			typedef set<SNode*>						EXCLUSIONS;
			EXCLUSIONS								_exclusions;						// TODO: make not using dynamic allocation
		private:
			size_t									_max_recursions;
			size_t									_recursion_count;
			SRenderInfo*							_initiator;
		private:
			SMgr*									_smgr;
		private:
			functor2<void, const SRenderInfo*, rsShade&>
													_post_setup_shade_state_adjust_cb;
		protected:
			SRenderInfo();
			~SRenderInfo();
		public:
			static SRenderInfo*				alloc();
			static SRenderInfo*				alloc(SRenderInfo* initiator);
			void							dealloc();
		public:
			void							clear();
		public:
			void							setDrawPass(DRAWPASS pass);
			void							setCamera(const SPtr<SCameraNode>& camera);
		public:
			void							setReverseCull(bool reverse);
			bool							getReverseCull() const;
		public:
			void							regSolidItem(SNode* node, intptr_t parameter);
			void							regTransparentItem(SNode* node, intptr_t parameter, ptrdiff_t layer_index);
			void							regLightItem(SLightNode* node);
		public:
			void							sortSolidItems();
			void							sortTransparentItems(bool sort_layer_index, bool sort_cam_dist);
		public:
			void							computeSolidBound();
			const AABB&						getSolidBound() const;
		public:
			DRAWPASS						getDrawPass() const;
			SCameraNode*					getCamera() const;
			const SceneInput&				getSceneInput() const;
		public:
			void							setRenderingNode(SNode* node);
			void							setRenderingDrawControl(DRAWCONTROL dc);
		public:
			SNode*							getRenderingNode() const;
			DRAWCONTROL						getRenderingDrawControl() const;
		public:
			void							setLightsVisibility(const SPtr<Tex2D>& tex, SLightNode * const * const sorted_channels, size_t channel_count);
			const SPtr<Tex2D>&				getLightsVisibilityMap() const;
		public:
			EXCLUSIONS&						getExclusions();
			const EXCLUSIONS&				getExclusions() const;
		public:
			void							setMaxRecursions(size_t max_recursions);
			size_t							getMaxRecursions() const;
		public:
			size_t							getRecursionCount() const;
		public:
			SRenderInfo*					getInitiator() const;
		public:
			void							setSMgr(SMgr* smgr);
			SMgr*							getSMgr() const;
		public:
			void							setPostSetupShadeStateAdjustCallback(const functor2<void, const SRenderInfo*, rsShade&>& cb);
			const functor2<void, const SRenderInfo*, rsShade&>&
											getPostSetupShadeStateAdjustCallback() const;
		public:
			bool							getPlanarReflectionMap(const Plane& p, REAL plane_offset, const Vec4& clear_color, SPtr<Tex2D>& reflection_map, Mat4& tc_xform_reflect);
			bool							getPlanarRefractionMap(const Plane& p, REAL plane_offset, const Vec4& clear_color, SPtr<Tex2D>& refraction_map, Mat4& tc_xform_reflect);
		public:
			friend class SRenderInfoStatic;
		};

		class SRenderParamQuery: public RenderParamQuery
		{
		private:
			SRenderInfo*					_ri;
		public:
											SRenderParamQuery(SRenderInfo* ri);
											~SRenderParamQuery();
		public:
			SRenderInfo*					getRenderInfo() const;
		public:
			void							preSetupShadeState(rsShade& rs);
			void							postSetupShadeState(rsShade& rs);
		public:
			const Mat4&						getWorldTransform();
		public:
			void							getLightingInfo(Vec3& ambient, size_t max_lights, LIGHTINFO* lightinfo_buffer, size_t& got_lights);
		public:
			const SPtr<Tex2D>&				getLightsVisibility();
			bool							recvShadow() const;
		public:
			const Vec4&						getControlColor() const;
		};
	};
};

