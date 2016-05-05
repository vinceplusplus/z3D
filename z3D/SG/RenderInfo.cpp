
#include "RenderInfo.h"

#include "../Core/functor_inl.h"

#include "SLightNode.h"

#include "SMgr.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

namespace z3D
{
	namespace SG
	{
		SRenderInfoStatic::SRenderInfoStaticsDestroyer::~SRenderInfoStaticsDestroyer()
		{
			if(_array_free_ri.size() != _array_ri.size())
			{
				syslog << L"RenderInfo: memory leaked!!";
				syslog << FORMATW(L"RenderInfo: total - %d, free - %d", _array_ri.size(), _array_free_ri.size());
			}
			for(size_t i = 0; i < _array_free_ri.size(); ++i)
				delete _array_free_ri[i];
		}
		SRenderInfoStatic::SRenderInfoStatic() {}
		SRenderInfo*					SRenderInfoStatic::alloc()
		{
			if(!_array_free_ri.size())
			{
				SRenderInfo* info = new SRenderInfo();
				_array_free_ri.push_back(info);
				_array_ri.push_back(info);
			}
			SRenderInfo* info = _array_free_ri[0];
			_array_free_ri.erase(_array_free_ri.begin());

			return info;
		}
		void							SRenderInfoStatic::dealloc(SRenderInfo* info)
		{
			_array_free_ri.push_back(info);
		}

		void								SRenderInfo::SceneInput::clear()
		{
			array_solid_item.clear();
			array_transparent_item.clear();
			array_light_item.clear();
		}

		void								SLightsChooser::choose(SLightNode** first, SLightNode** last, const Vec3& o, SLightNode** out_buffer, size_t& buffer_count)
		{
			class ITEM
			{
			public:
				SLightNode*					i;
				ptrdiff_t					priority;
				REAL						dist_sq;
			};
			class near_to_far
			{public:
				bool					operator () (const ITEM& e0, const ITEM& e1) const
				{
					ptrdiff_t priority_diff = e0.priority - e1.priority;
					if(priority_diff > 0)
						return true;
					else if(priority_diff < 0)
						return false;
					return e0.dist_sq < e1.dist_sq;
				}
			};

			ITEM item_buffer[64];
			size_t item_count = 0;

			for(SLightNode** iter = first; iter != last; ++iter)
			{
				SLightNode* l = *iter;

				ITEM item = {l, l->getPriority(), (l->world_pos() - o).lengthsq()};
				item_buffer[item_count] = item;
				++item_count;
				if(item_count >= _countof(item_buffer))
					break;
			}

			std::sort(item_buffer, item_buffer + item_count, near_to_far());
			size_t c = min(buffer_count, item_count);
			for(size_t i = 0; i < c; ++i)
				out_buffer[i] = item_buffer[i].i;
			buffer_count = c;
		}
		void								SLightsChooser::choose(SRenderLightItem* first, SRenderLightItem* last, const Vec3& o, SRenderLightItem** out_buffer, size_t& buffer_count)
		{
			class ITEM
			{
			public:
				SRenderLightItem*			i;
				ptrdiff_t					priority;
				REAL						dist_sq;
			};
			class near_to_far
			{public:
				bool					operator () (const ITEM& e0, const ITEM& e1) const
				{
					ptrdiff_t priority_diff = e0.priority - e1.priority;
					if(priority_diff > 0)
						return true;
					else if(priority_diff < 0)
						return false;
					return e0.dist_sq < e1.dist_sq;
				}
			};

			ITEM item_buffer[64];
			size_t item_count = 0;

			for(SRenderLightItem* iter = first; iter != last; ++iter)
			{
				SLightNode* l = iter->node;

				ITEM item = {iter, l->getPriority(), (l->world_pos() - o).lengthsq()};
				item_buffer[item_count] = item;
				++item_count;
				if(item_count >= _countof(item_buffer))
					break;
			}

			std::sort(item_buffer, item_buffer + item_count, near_to_far());
			size_t c = min(buffer_count, item_count);
			for(size_t i = 0; i < c; ++i)
				out_buffer[i] = item_buffer[i].i;
			buffer_count = c;
		}

		SRenderInfo::SRenderInfo()
		{
			clear();
		}
		SRenderInfo::~SRenderInfo()
		{
		}
		SRenderInfo*				SRenderInfo::alloc()
		{
			return SRenderInfoStatic::alloc();
		}
		SRenderInfo*				SRenderInfo::alloc(SRenderInfo* initiator)
		{
			Z_ASSERT( !initiator || initiator->getRecursionCount() < initiator->getMaxRecursions() );
			SRenderInfo* ri = alloc();
			if(!initiator)
				return ri;
			ri->_initiator = initiator;
			ri->_max_recursions = initiator->_max_recursions;
			ri->_recursion_count = initiator->_recursion_count + 1;
			ri->_post_setup_shade_state_adjust_cb = initiator->_post_setup_shade_state_adjust_cb;
			return ri;
		}
		void						SRenderInfo::dealloc()
		{
			clear();
			SRenderInfoStatic::dealloc(this);
		}
		void						SRenderInfo::clear()
		{
			_pass = DRAWPASS_DEFAULT;
			_camera = NULL;

			_scene_input.clear();

			_solid_bound = AABB();

			_rendering.node = NULL;
			_rendering.dc = DRAWCONTROL_SOLID;

			_light_visibility.tex.reset();
			_light_visibility.channel_count = 0;

			__FLAGS = 0;

			_exclusions.clear();

			_max_recursions = 1;
			_recursion_count = 0;
			_initiator = NULL;

			_smgr = NULL;

			_post_setup_shade_state_adjust_cb.reset();
		}
		void						SRenderInfo::setDrawPass(DRAWPASS pass)
		{
			_pass = pass;
		}
		void						SRenderInfo::setCamera(const SPtr<SCameraNode>& camera)
		{
			_camera = camera.get();
			_camera_world_pos = camera->world_pos();
			_camera_world_dir = (-Vec3(camera->world_trans()[2])).normal_safe();
		}
		void							SRenderInfo::setReverseCull(bool reverse)
		{
			_reverse_cull = reverse;
		}
		bool							SRenderInfo::getReverseCull() const
		{
			return _reverse_cull;
		}
		void						SRenderInfo::regSolidItem(SNode* node, intptr_t parameter)
		{
			if(_exclusions.size() && _exclusions.count(node))
				return;

			switch(getDrawPass())
			{
			case DRAWPASS_DEPTH_ONLY:
			case DRAWPASS_SHADE:
			case DRAWPASS_SHADE_EQUAL_DEPTH:
				{
					SRenderItem info = {node, parameter, (node->world_bound().center() - _camera_world_pos) * _camera_world_dir, 0};
					_scene_input.array_solid_item.push_back(info);
				}
				break;
			case DRAWPASS_GEN_SHADOW_MAP_VSM:
				{
					if(!node->castShadow())
						break;
					SRenderItem info = {node, parameter, (node->world_bound().center() - _camera_world_pos) * _camera_world_dir, 0};
					_scene_input.array_solid_item.push_back(info);
				}
				break;
			case DRAWPASS_DEPTH_ONLY_SOLID_RECV_SHADOW_VSM:
			case DRAWPASS_SCAN_SOLID_RECV_SHADOW_AND_SHADOW_LIGHTS:
				{
					if(!node->recvShadow())
						break;
					SRenderItem info = {node, parameter, (node->world_bound().center() - _camera_world_pos) * _camera_world_dir, 0};
					_scene_input.array_solid_item.push_back(info);
				}
				break;
			}
		}
		void						SRenderInfo::regTransparentItem(SNode* node, intptr_t parameter, ptrdiff_t layer_index)
		{
			if(_exclusions.size() && _exclusions.count(node))
				return;

			switch(getDrawPass())
			{
			case DRAWPASS_SHADE:
			case DRAWPASS_SHADE_EQUAL_DEPTH:
				{
					SRenderItem info = {node, parameter, node->world_pos() * _camera_world_dir, layer_index};
					_scene_input.array_transparent_item.push_back(info);
				}
				break;
			}
		}
		void						SRenderInfo::regLightItem(SLightNode* node)
		{
			if(_exclusions.size() && _exclusions.count(node))
				return;

			switch(getDrawPass())
			{
			case DRAWPASS_SHADE:
			case DRAWPASS_SHADE_EQUAL_DEPTH:
				{
					ptrdiff_t ch = -1;
					if(_light_visibility.channel_count)
					{
						SLightNode** first = _light_visibility.sorted_channels;
						SLightNode** last = first + _light_visibility.channel_count;
						SLightNode** iter = lower_bound(first, last, node);
						if(iter != last && *iter == node)
							ch = iter - first;
					}

					SRenderLightItem info = {node, ch};
					_scene_input.array_light_item.push_back(info);
				}
				break;
			case DRAWPASS_SCAN_SOLID_RECV_SHADOW_AND_SHADOW_LIGHTS:
				{
					if(!node->getLightCastShadow())
						break;
					switch(node->light_type())
					{
					case SLightNode::LIGHTTYPE_DIRECTIONAL:
					case SLightNode::LIGHTTYPE_SPOT:
						break;
					default:
						return;
					}
					SRenderLightItem info = {node, -1};
					_scene_input.array_light_item.push_back(info);
				}
				break;
			}
		}
		void						SRenderInfo::sortSolidItems()
		{
			class front_to_back
			{
			public:
				bool						operator () (const SRenderItem& e0, const SRenderItem& e1) const
				{
					return (e0.cam_dist < e1.cam_dist);
				}
			};

			sort(_scene_input.array_solid_item.begin(), _scene_input.array_solid_item.end(), front_to_back());
		}
		void						SRenderInfo::sortTransparentItems(bool sort_layer_index, bool sort_cam_dist)
		{
			if(!sort_layer_index && !sort_cam_dist)
				return;

			// if we can use template class here, we sort for easier implementation
			class back_to_front_sort_layer_index
			{
			public:
				bool						operator () (const SRenderItem& e0, const SRenderItem& e1) const {	return e0.layer_index < e1.layer_index;	}
			};
			class back_to_front_sort_cam_dist
			{
			public:
				bool						operator () (const SRenderItem& e0, const SRenderItem& e1) const {	return e0.cam_dist > e1.cam_dist;	}
			};
			class back_to_front_sort_layer_index_cam_dist
			{
			public:
				bool						operator () (const SRenderItem& e0, const SRenderItem& e1) const {	return (e0.layer_index == e1.layer_index) ? (e0.cam_dist > e1.cam_dist) : (e0.layer_index < e1.layer_index);	}
			};

			if(sort_layer_index)
			{
				if(sort_cam_dist)
					stable_sort(_scene_input.array_transparent_item.begin(), _scene_input.array_transparent_item.end(), back_to_front_sort_layer_index_cam_dist());
				else
					stable_sort(_scene_input.array_transparent_item.begin(), _scene_input.array_transparent_item.end(), back_to_front_sort_layer_index());
			}
			else
				stable_sort(_scene_input.array_transparent_item.begin(), _scene_input.array_transparent_item.end(), back_to_front_sort_cam_dist());
		}
		void						SRenderInfo::computeSolidBound()
		{
			const vector<SRenderItem>& array_solid_item = _scene_input.array_solid_item;

			if(!array_solid_item.size())
				return;
			AABB b;
			const SRenderItem* end = VECTOR_END_PTR(array_solid_item);
			for(const SRenderItem* iter = VECTOR_BEGIN_PTR(array_solid_item); iter != end; ++iter)
				b.fit(iter->node->world_bound());
			_solid_bound = b;
		}
		const AABB&					SRenderInfo::getSolidBound() const
		{
			return _solid_bound;
		}
		DRAWPASS					SRenderInfo::getDrawPass() const
		{
			return _pass;
		}
		SCameraNode*				SRenderInfo::getCamera() const
		{
			return _camera;
		}
		const SRenderInfo::SceneInput&			SRenderInfo::getSceneInput() const
		{
			return _scene_input;
		}
		void						SRenderInfo::setRenderingNode(SNode* node)
		{
			_rendering.node = node;
		}
		void						SRenderInfo::setRenderingDrawControl(DRAWCONTROL dc)
		{
			_rendering.dc = dc;
		}
		SNode*						SRenderInfo::getRenderingNode() const
		{
			return _rendering.node;
		}
		DRAWCONTROL					SRenderInfo::getRenderingDrawControl() const
		{
			return _rendering.dc;
		}
		void						SRenderInfo::setLightsVisibility(const SPtr<Tex2D>& tex, SLightNode * const * const sorted_channels, size_t channel_count)
		{
			_light_visibility.tex = tex;
			_light_visibility.channel_count = min(_countof(_light_visibility.sorted_channels), channel_count);
			memcpy(_light_visibility.sorted_channels, sorted_channels, sizeof(sorted_channels[0]) * _light_visibility.channel_count);
		}
		const SPtr<Tex2D>&			SRenderInfo::getLightsVisibilityMap() const
		{
			return _light_visibility.tex;
		}
		SRenderInfo::EXCLUSIONS&		SRenderInfo::getExclusions()
		{
			return _exclusions;
		}
		const SRenderInfo::EXCLUSIONS&	SRenderInfo::getExclusions() const
		{
			return _exclusions;
		}
		void							SRenderInfo::setMaxRecursions(size_t max_recursions)
		{
			_max_recursions = max_recursions;
		}
		size_t							SRenderInfo::getMaxRecursions() const
		{
			return _max_recursions;
		}
		size_t							SRenderInfo::getRecursionCount() const
		{
			return _recursion_count;
		}
		SRenderInfo*					SRenderInfo::getInitiator() const
		{
			return _initiator;
		}
		void							SRenderInfo::setSMgr(SMgr* smgr)
		{
			_smgr = smgr;
		}
		SMgr*							SRenderInfo::getSMgr() const
		{
			return _smgr;
		}
		void							SRenderInfo::setPostSetupShadeStateAdjustCallback(const functor2<void, const SRenderInfo*, rsShade&>& cb)
		{
			_post_setup_shade_state_adjust_cb = cb;
		}
		const functor2<void, const SRenderInfo*, rsShade&>&
										SRenderInfo::getPostSetupShadeStateAdjustCallback() const
		{
			return _post_setup_shade_state_adjust_cb;
		}
		bool							SRenderInfo::getPlanarReflectionMap(const Plane& p, REAL plane_offset, const Vec4& clear_color, SPtr<Tex2D>& reflection_map, Mat4& tc_xform_reflect)
		{
			if(getRecursionCount() >= getMaxRecursions())
			{
				//MessageBoxW(NULL, FORMATW(L"inner recursion limit reached %I32d %I32d", (int32_t)getRecursionCount(), (int32_t)getMaxRecursions()).c_str(), L"", 0);
				return false;
			}

			Gfx* gfx = Gfx::current();

			SMgr* smgr = getSMgr();

			SRenderInfo* new_ri = SRenderInfo::alloc(this);

			SCameraNode* cam = getCamera();
			SPtr<SCameraNode> new_cam = smgr->getStockCamera();
			new_cam->copyCameraSettings(cam);
			Vec3 plane_normal = p.normal();
			new_cam->additionalClipPlanes().push_back(Plane(p.origin() + plane_normal * plane_offset, plane_normal));
			new_cam->setWorldTransform(cam->world_trans() * Mat4::mirror(p).inverse());
			new_ri->setCamera(new_cam);

			// no longer need to reverse here, the postSetupShadeState will take care
			//new_ri->setReverseCull(!getReverseCull());
			new_ri->setReverseCull(getReverseCull());

			new_ri->getExclusions().insert(getRenderingNode());

			tc_xform_reflect = new_cam->view_proj() * Mat4::scaling(0.5f, 0.5f, 1.0f) * Mat4::translation(0.5f, 0.5f, 0.0f) * Mat4::scaling(1, -1, 1) * Mat4::translation(0, 1, 0);

			SPtr<Tex2D> color;
			SPtr<Renderbuffer> depth;
			color = smgr->getStockNonScreenRGBA();
			depth = smgr->getStockNonScreenRBDepth();

			//if(!color || !depth)
			//{
			//	if(!color)
			//		MessageBoxW(NULL, L"cannot create color buffer", L"", 0);
			//	if(!depth)
			//		MessageBoxW(NULL, L"cannot create depth buffer", L"", 0);
			//	new_ri->dealloc();
			//	return false;
			//}

			Mat4 view, proj;

			view = gfx->getViewTransform();
			proj = gfx->getUnadjustedProjectionTransform();

			rsView view_state = gfx->getViewState();
			gfx->push_RT_n_DS();

			gfx->unbindDepthStencil();
			gfx->unbindAllRenderTargets();
			gfx->bindRenderTarget(0, color);
			gfx->bindDepthStencil(depth);

			rsView new_view_state;
			new_view_state.init(gfx);

			gfx->setViewState(new_view_state);

			gfx->clear(true, true, true, clear_color, gfx->getZFarMapping(), 0);

			smgr->drawAll(DRAWPASS_DEFAULT, new_ri);

			gfx->pop_RT_n_DS();
			gfx->setViewState(view_state);
			gfx->setViewProjTransform(view, proj);

			reflection_map = color;

			new_ri->dealloc();

			return true;
		}

		bool							SRenderInfo::getPlanarRefractionMap(const Plane& p, REAL plane_offset, const Vec4& clear_color, SPtr<Tex2D>& refraction_map, Mat4& tc_xform_reflect)
		{
			if(getRecursionCount() >= getMaxRecursions())
				return false;

			Gfx* gfx = Gfx::current();

			SMgr* smgr = getSMgr();

			SRenderInfo* new_ri = SRenderInfo::alloc(this);

			SCameraNode* cam = getCamera();
			SPtr<SCameraNode> new_cam = smgr->getStockCamera();
			new_cam->copyCameraSettings(cam);
			Vec3 plane_normal = p.normal();
			new_cam->additionalClipPlanes().push_back(Plane(p.origin() + plane_normal * plane_offset, plane_normal));
			new_cam->setWorldTransform(cam->world_trans());
			new_ri->setCamera(new_cam);
			new_ri->setReverseCull(getReverseCull());
			new_ri->getExclusions().insert(getRenderingNode());

			tc_xform_reflect = new_cam->view_proj() * Mat4::scaling(0.5f, 0.5f, 1.0f) * Mat4::translation(0.5f, 0.5f, 0.0f) * Mat4::scaling(1, -1, 1) * Mat4::translation(0, 1, 0);

			SPtr<Tex2D> color;
			SPtr<Renderbuffer> depth;
			color = smgr->getStockNonScreenRGBA();
			depth = smgr->getStockNonScreenRBDepth();

			if(!color || !depth)
			{
				new_ri->dealloc();
				return false;
			}

			Mat4 view, proj;

			view = gfx->getViewTransform();
			proj = gfx->getUnadjustedProjectionTransform();

			rsView view_state = gfx->getViewState();
			gfx->push_RT_n_DS();

			gfx->unbindDepthStencil();
			gfx->unbindAllRenderTargets();
			gfx->bindRenderTarget(0, color);
			gfx->bindDepthStencil(depth);

			rsView new_view_state;
			new_view_state.init(gfx);

			gfx->setViewState(new_view_state);

			gfx->clear(true, true, true, clear_color, gfx->getZFarMapping(), 0);

			smgr->drawAll(DRAWPASS_DEFAULT, new_ri);

			gfx->pop_RT_n_DS();
			gfx->setViewState(view_state);
			gfx->setViewProjTransform(view, proj);

			refraction_map = color;

			new_ri->dealloc();

			return true;
		}

		SRenderParamQuery::SRenderParamQuery(SRenderInfo* ri): _ri(ri)
		{}
		SRenderParamQuery::~SRenderParamQuery()
		{}
		SRenderInfo*					SRenderParamQuery::getRenderInfo() const
		{
			return _ri;
		}
		void							SRenderParamQuery::preSetupShadeState(rsShade& rs)
		{
			switch(_ri->getRenderingDrawControl())
			{
			case DRAWCONTROL_SOLID:
				{
					rs.enableWriteDepth(true);
					rs.enableCull(GfxEnum::CULLMODE_CW);
					switch(_ri->getDrawPass())
					{
					case DRAWPASS_DEPTH_ONLY:
						rs.enableDepthTest(GfxEnum::CMPFUNC_LESS);
						break;
					case DRAWPASS_SHADE:
						rs.enableDepthTest(GfxEnum::CMPFUNC_LESS);
						break;
					case DRAWPASS_SHADE_EQUAL_DEPTH:
						rs.enableDepthTest(GfxEnum::CMPFUNC_EQUAL);
						break;
					case DRAWPASS_DEPTH_ONLY_SOLID_RECV_SHADOW_VSM:
						rs.enableDepthTest(GfxEnum::CMPFUNC_LESS);
						break;
					case DRAWPASS_GEN_SHADOW_MAP_VSM:
						rs.enableDepthTest(GfxEnum::CMPFUNC_LESS);
						break;
					default:
						rs.enableDepthTest(GfxEnum::CMPFUNC_LESS);
						break;
					}
				}
				break;
			case DRAWCONTROL_TRANSPARENT:
				{
					rs.enableCull(GfxEnum::CULLMODE_CW);
					rs.enableDepthTest(GfxEnum::CMPFUNC_LEQUAL);
				}
				break;
			}
			if(_ri->getCamera()->additionalClipPlanes().size())
			{
				const vector<Plane>& p = _ri->getCamera()->additionalClipPlanes();
				for(size_t i = 0; i < p.size(); ++i)
					rs.addClipPlane(p[i]);
			}
			if(_ri->getReverseCull())
				rs.reverseCull();
		}
		void							SRenderParamQuery::postSetupShadeState(rsShade& rs)
		{
			switch(_ri->getRenderingDrawControl())
			{
			case DRAWCONTROL_SOLID:
				{
					switch(_ri->getDrawPass())
					{
					case DRAWPASS_DEPTH_ONLY:
						rs.enableWriteColor(false, false, false, false);
						break;
					case DRAWPASS_SHADE_EQUAL_DEPTH:
						rs.disableAlphaTest();
						break;
					}
				}
				break;
			case DRAWCONTROL_TRANSPARENT:
				{
				}
				break;
			}
			Z_ASSERT( Gfx::current() );
			if(true)
			{
				class TEST_FLIP
				{
				public:
					bool			operator() (const Mat4& m)
					{
						return (((*(Vec3*)m[0]) ^ (*(Vec3*)m[1])) * (*(Vec3*)m[2])) < 0.0f;
					}
				} test_flip;
				// maybe we can optimize such that the test flip of view matrix can happen at somewhere similar to preRender() ??
				bool reverse = test_flip(_ri->getCamera()->view()) ^ test_flip(_ri->getRenderingNode()->world_trans());
				if(reverse)
					rs.reverseCull();
			}
			//if(Gfx::current()->boundRenderTarget())
			//	rs.reverseCull();

			if(_ri->getPostSetupShadeStateAdjustCallback().valid())
				_ri->getPostSetupShadeStateAdjustCallback()(_ri, rs);
		}
		const Mat4&						SRenderParamQuery::getWorldTransform()
		{
			return _ri->getRenderingNode()->world_trans();
		}
		void							SRenderParamQuery::getLightingInfo(Vec3& ambient, size_t max_lights, LIGHTINFO* lightinfo_buffer, size_t& got_lights)
		{
			const SRenderInfo::SceneInput& input = _ri->getSceneInput();

			const vector<SRenderLightItem>& array_light_item = input.array_light_item;

			AABB obj_world_bound = _ri->getRenderingNode()->world_bound();

			ambient = Vec3::zero;

			SRenderLightItem		item_buffer[64];
			size_t					item_count = 0;

			if(array_light_item.size())
			{
				Vec3 rendering_node_world_pos = _ri->getRenderingNode()->world_pos();

				SRenderLightItem const * end = VECTOR_END_PTR(array_light_item);
				for(SRenderLightItem const * iter = VECTOR_BEGIN_PTR(array_light_item); iter != end; ++iter)
				{
					SLightNode* l = (*iter).node;
					if(Test::AABBAABB(l->world_bound(), obj_world_bound))
					{
						switch(l->light_type())
						{
						case SLightNode::LIGHTTYPE_AMBIENT:
							ambient += l->getColor() * l->getMultiplier();
							break;
						case SLightNode::LIGHTTYPE_DIRECTIONAL:
						case SLightNode::LIGHTTYPE_POINT:
						case SLightNode::LIGHTTYPE_SPOT:
							{
								if(item_count >= _countof(item_buffer))
									break;
								item_buffer[item_count] = *iter;
								++item_count;
							}
							break;
						}
					}
				}
			}

			SRenderLightItem*			out_item_buffer[64];
			size_t						out_item_count = _countof(out_item_buffer);

			SLightsChooser::choose(item_buffer, item_buffer + item_count, _ri->getRenderingNode()->world_pos(), out_item_buffer, out_item_count);

			if(true)
			{
				size_t c = min(max_lights, out_item_count);
				for(size_t i = 0; i < c; ++i)
				{
					const SRenderLightItem& l = *out_item_buffer[i];
					LIGHTINFO& info = lightinfo_buffer[i];

					SLightNode* n = l.node;

					info.id = (intptr_t)n;
					info.color = n->getColor() * n->getMultiplier();
					info.range = n->getRadius();
					info.visibility_channel = l.visibility_channel;

					switch(n->light_type())
					{
					case SLightNode::LIGHTTYPE_DIRECTIONAL:
						info.type = GfxEnum::LIGHTTYPE_DIRECTIONAL;
						info.dir = Vec3::zNegAxis * n->world_rot().asMat3();
						break;
					case SLightNode::LIGHTTYPE_POINT:
						info.type = GfxEnum::LIGHTTYPE_POINT;
						info.pos = n->world_pos();
						break;
					case SLightNode::LIGHTTYPE_SPOT:
						info.type = GfxEnum::LIGHTTYPE_SPOT;
						info.pos = n->world_pos();
						info.dir = Vec3::zNegAxis * n->world_rot().asMat3();
						info.inner_cone = n->getInnerAngle();
						info.outer_cone = n->getOuterAngle();
						break;
					}
				}
				got_lights = c;
			}
		}
		const SPtr<Tex2D>&				SRenderParamQuery::getLightsVisibility()
		{
			return _ri->getLightsVisibilityMap();
		}
		bool							SRenderParamQuery::recvShadow() const
		{
			return _ri->getRenderingNode()->recvShadow();
		}
		const Vec4&						SRenderParamQuery::getControlColor() const
		{
			return _ri->getRenderingNode()->getFinalColor();
		}
	};
};

