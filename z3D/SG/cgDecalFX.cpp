
#include "cgDecalFX.h"

#include "../Core/allocators_inl.h"

#include "RenderInfo.h"
#include "SVisitor.h"
#include "SMeshNode.h"
#include "Mesh.h"

namespace z3D
{
	namespace SG
	{
		cgDecalFXBase::IntrSet::IntrSet(): array_mesh_res(_array_mesh_res)
		{
		}
		void					cgDecalFXBase::IntrSet::clear()
		{
			_array_mesh_res.clear();
			_array_triidx.clear();
		}
		cgDecalFXBase::cgDecalFXBase(const AABB& local_bound) : _local_bound(local_bound)
		{
		}
		cgDecalFXBase::~cgDecalFXBase()
		{
		}
		const AABB&			cgDecalFXBase::local_bound()
		{
			return _local_bound;
		}
		bool				cgDecalFXBase::renderable() const {return true;}
		void				cgDecalFXBase::getIntersectingSet(const AABB& search_box, const SPtr<SNode>& map_node, IntrSet& intr_set) const
		{
			SAABBIntersector intersector(search_box, SMeshNode::type_info_static());
			intersector.traverse(map_node);
			if(!intersector.array_node.size())
				return;

			if(true)
			{
				vector<int32_t, frame_temp_allocator<int32_t>> array_index;

				const SPtr<SNode>* node_end = VECTOR_END_PTR(intersector.array_node);
				for(const SPtr<SNode>* node_iter = VECTOR_BEGIN_PTR(intersector.array_node); node_iter != node_end; ++node_iter)
				{
					SMeshNode* mn = (SMeshNode*)node_iter->get();

					OBB local_box(search_box);
					local_box.transformBy(mn->inv_world_trans());
					AABB local_search_box = search_box.transformedBy(mn->inv_world_trans());

					mn->buildSPT();

					array_index.clear();
					mn->spt()->intersect(local_search_box, array_index);

					if(array_index.size())
					{
						const Vec3& local_box_center = local_box.center();
						const Vec3& local_box_extent = local_box.extent();
						Vec3 local_box_eaxis[3] = {local_box.axis()[0] * local_box_extent[0], local_box.axis()[1] * local_box_extent[1], local_box.axis()[2] * local_box_extent[2]};

						const Vec3* verts = mn->mesh()->get_verts();
						const vnFace* faces = mn->mesh()->get_faces();

						size_t last_size = intr_set._array_triidx.size();

						for(size_t i = 0; i < array_index.size(); ++i)
						{
							Z_STATIC_ASSERT(sizeof(SMeshNode*) == sizeof(uint32_t), POINTER_SIZE_ASSUMPTION_WRONG);

							int32_t triidx = array_index[i];

							const vnFace& f = faces[triidx];
							const Vec3& v0 = verts[f.v[0]];
							const Vec3& v1 = verts[f.v[1]];
							const Vec3& v2 = verts[f.v[2]];

							if(!Test::OBBTriangle(local_box_center, local_box_eaxis, v0, v1, v2))
								continue;

							intr_set._array_triidx.push_back(triidx);
						}

						if(last_size != intr_set._array_triidx.size())
						{
							IntrSet::MeshRes r = {SPtr<SMeshNode>(mn), NULL, intr_set._array_triidx.size() - last_size};				// do not assign the pointer here, the pointer may change due to vector resize of later push back

							intr_set._array_mesh_res.push_back(r);
						}
					}
				}
			}

			if(intr_set._array_triidx.size())
			{
				int32_t* triidx_ptr = VECTOR_BEGIN_PTR(intr_set._array_triidx);
				for(size_t i = 0; i < intr_set._array_mesh_res.size(); ++i)
				{
					IntrSet::MeshRes& r = intr_set._array_mesh_res[i];
					r.array_triidx = triidx_ptr;
					triidx_ptr += r.triidx_count;
				}
			}
		}
		AABB				cgDecalFXBase::projAABBOnScreen(const AABB& box, const Mat4& viewproj)
		{
			Plane p(Vec3(viewproj[0][3] + viewproj[0][2], viewproj[1][3] + viewproj[1][2], viewproj[2][3] + viewproj[2][2]), -(viewproj[3][3] + viewproj[3][2]));
			Vec3 o = p.origin();
			Vec3 n = p.normal();

			bool visibles[8];
			Vec3 corners[8] = {box.getCorner(0), box.getCorner(1), box.getCorner(2), box.getCorner(3), box.getCorner(4), box.getCorner(5), box.getCorner(6), box.getCorner(7)};

			AABB screen_bound;

			for(size_t i = 0; i < 8; ++i)
			{
				bool visible;
				visibles[i] = visible = (p.sub(corners[i]) >= 0);
				if(visible)
					screen_bound.fit(corners[i] * viewproj);
			}

			class CLIP
			{
			public:
				Vec3			operator() (const Vec3& origin, const Vec3& normal, const Vec3& p0, const Vec3& p1) const
				{
					REAL t = ((origin - p0) * normal) / ((p1 - p0) * normal);
					return (1 - t) * p0 + t * p1;
				}
			} clip;

			if(visibles[0] != visibles[1])		screen_bound.fit(clip(o, n, corners[0], corners[1]) * viewproj);
			if(visibles[0] != visibles[2])		screen_bound.fit(clip(o, n, corners[0], corners[2]) * viewproj);
			if(visibles[0] != visibles[4])		screen_bound.fit(clip(o, n, corners[0], corners[4]) * viewproj);
			if(visibles[1] != visibles[3])		screen_bound.fit(clip(o, n, corners[1], corners[3]) * viewproj);
			if(visibles[1] != visibles[5])		screen_bound.fit(clip(o, n, corners[1], corners[5]) * viewproj);
			if(visibles[2] != visibles[3])		screen_bound.fit(clip(o, n, corners[2], corners[3]) * viewproj);
			if(visibles[2] != visibles[6])		screen_bound.fit(clip(o, n, corners[2], corners[6]) * viewproj);
			if(visibles[3] != visibles[7])		screen_bound.fit(clip(o, n, corners[3], corners[7]) * viewproj);
			if(visibles[4] != visibles[5])		screen_bound.fit(clip(o, n, corners[4], corners[5]) * viewproj);
			if(visibles[4] != visibles[6])		screen_bound.fit(clip(o, n, corners[4], corners[6]) * viewproj);
			if(visibles[5] != visibles[7])		screen_bound.fit(clip(o, n, corners[5], corners[7]) * viewproj);
			if(visibles[6] != visibles[7])		screen_bound.fit(clip(o, n, corners[6], corners[7]) * viewproj);

			return screen_bound;
		}

		cgDecalFX::cgDecalFX(const AABB& local_bound): cgDecalFXBase(local_bound)
		{
		}
		cgDecalFX::~cgDecalFX()
		{
		}
		void							cgDecalFX::setDecalTexture(const SPtr<Tex2D>& tex)
		{
			_tex_decal = tex;
		}
		SPtr<cgDecalFX::Item>			cgDecalFX::create(const SPtr<SNode>& map_node, const Vec3& world_pos, const Vec3& world_dir, REAL radius, const Vec4& color)
		{
			IntrSet intr_set;
			getIntersectingSet(AABB(world_pos - Vec3(radius, radius, radius), world_pos + Vec3(radius, radius, radius)), map_node, intr_set);

			Mat3 rot;
			if(true)
			{
				REAL pitch, yaw;
				world_dir.resolveOriRH(yaw, pitch);
				rot = Mat3::rotationY(-yaw) * Mat3::rotationX(-pitch);			// need the inverse of the rotation
			}
			REAL inv_radius = 1.0f / radius;

			SPtr<Item> s = new Item();

			s->color = color;

			vector<Vec2>& array_uv = s->_array_uv;
			vector<int32_t>& array_triidx = s->_array_triidx;

			for(size_t i = 0; i < intr_set.array_mesh_res.size(); ++i)
			{
				const IntrSet::MeshRes& res = intr_set.array_mesh_res[i];
				const SPtr<SMeshNode> mn = res.mn.lock();
				if(!mn)
					continue;

				Mat4 world_xform = mn->world_trans();
				const Vec3* pos = mn->mesh()->get_verts();
				const vnFace* faces = mn->mesh()->get_faces();

				Mat3 inv_world_rot = ((Mat3)world_xform).inverse();
				Vec3 local_dir = world_dir * inv_world_rot;
				local_dir.normalize_safe();

				size_t last_size = array_triidx.size();
				for(size_t j = 0; j < res.triidx_count; ++j)
				{
					int32_t triidx = res.array_triidx[j];
					const vnFace& f = faces[triidx];

					if(((pos[f.v[1]] - pos[f.v[0]]) ^ (pos[f.v[2]] - pos[f.v[0]])).angle(local_dir) > 89.0f)
						continue;

					for(size_t k = 0; k < 3; ++k)
					{
						Vec3 uv = ((pos[f.v[k]] * world_xform - world_pos) * rot) * inv_radius;
						array_uv.push_back(Vec2(uv.x * 0.5f + 0.5f, uv.y * 0.5f + 0.5f));
					}
					array_triidx.push_back(triidx);
				}

				if(last_size != array_triidx.size())
				{
					Item::Piece p = {mn, array_triidx.size() - last_size, NULL, NULL};			// do not assign the pointer here, the pointer may change due to vector resize of later push back
					s->_array_piece.push_back(p);
				}
			}
			if(s->_array_triidx.size())
			{
				int32_t* triidx_ptr = VECTOR_BEGIN_PTR(s->_array_triidx);
				Vec2* uv_ptr = VECTOR_BEGIN_PTR(s->_array_uv);
				for(size_t i = 0; i < s->_array_piece.size(); ++i)
				{
					Item::Piece& p = s->_array_piece[i];
					p.array_triidx = triidx_ptr;
					p.array_uv = uv_ptr;

					triidx_ptr += p.triidx_count;
					uv_ptr += p.triidx_count * 3;
				}
			}

			_array_item.push_back(s);

			if(true)
			{
				Vec3 r = Vec3(radius, radius, radius);
				s->world_bound = AABB(world_pos - r, world_pos + r);
			}

			return s;
		}
		void							cgDecalFX::preRender(SRenderInfo* ri)
		{
			if(!_array_item.size())
				return;

			ri->regTransparentItem(this, 0, -32768);
		}
		void							cgDecalFX::render(intptr_t parameter, SRenderInfo* ri)
		{
			if(!_array_item.size())
				return;

			Gfx* gfx = Gfx::current();

			Frustum frustum(gfx->getUnadjustedViewProjectionTransform(), gfx->getZNearMapping(), gfx->getZFarMapping());

			SRenderParamQuery param(ri);

			rsShade rs_preset(false);

			param.preSetupShadeState(rs_preset);

			rs_preset.setSampler(0, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);
			rs_preset.enableBlend();
			rs_preset.enableDepthTest(GfxEnum::CMPFUNC_EQUAL);

			rs_preset.enableAlphaTest(GfxEnum::CMPFUNC_GREATER, 0.0f);

			ShaderGenConf cfg;

			cfg.begin(Mat4::identity);

			cfg.setMaterial(Vec3(1, 1, 1), Vec3::zero, Vec3::zero, 0, 1.0f);
			cfg.setLightAmbient(Vec3(1, 1, 1));

			cfg.useTexCoord(0);
			cfg.useTexSampler(0, GfxEnum::TEXTARGET_2D);

			cfg.fetchTex(0, 0);

			cfg.enableVertexColor(true);

			cfg.addPSCode("	out_color = in_diff * tf_0_0 + float4(in_spec, 0);\n");

			ShaderGen* gen = gfx->getShaderGen();
			SPtr<Shader> vs, ps;
			gfx->useProgrammablePipeline(vs = gen->genVS(cfg), ps = gen->genPS(cfg));

			ShaderUniforms2 unis(vs, ps);

			gfx->bindTexture(0, _tex_decal);

			for(size_t i = 0; i < _array_item.size(); ++i)
			{
				const WPtr<Item>& wptr = _array_item[i];
				if(!wptr.valid())
				{
					_array_item.erase(_array_item.begin() + i);
					--i;
					continue;
				}

				const SPtr<Item>& s = wptr.lock();

				if(frustum.contains(s->world_bound) == Frustum::FC_OUTSIDE)
					continue;

				rsShade rs = rs_preset;

				if(true)
				{
					AABB screen_bound = projAABBOnScreen(s->world_bound, gfx->getUnadjustedViewProjectionTransform());
					if(screen_bound.isEmpty())
						continue;

					const rsView& view = gfx->getViewState();

					int32_t x = (int32_t)((screen_bound.minimum().x + 1.0f) * 0.5f * view.viewport.dx);
					int32_t y = (int32_t)((-screen_bound.maximum().y + 1.0f) * 0.5f * view.viewport.dy);
					int32_t x2 = (int32_t)((screen_bound.maximum().x + 1.0f) * 0.5f * view.viewport.dx);
					int32_t y2 = (int32_t)((-screen_bound.minimum().y + 1.0f) * 0.5f * view.viewport.dy);

					rs.enableScissorTest(x, y, x2 - x, y2 - y);
				}

				param.postSetupShadeState(rs);

				gfx->setShadeState(rs);

				for(size_t j = 0; j < s->_array_piece.size(); ++j)
				{
					const Item::Piece& p = s->_array_piece[j];
					SPtr<SMeshNode> mn = p.mn.lock();
					if(!mn)
						continue;

					//unis.clear();
					unis.reset();
					*(Mat4*)cfg.world = mn->world_trans();
					gen->setShaderUniforms(cfg, unis, true, true);
					gfx->setShaderUniforms(unis);

					_jotter.begin(3, false, true, 1);
					_jotter.color(s->color);

					const Vec3* pos = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();
					const Vec2* uv = p.array_uv;
					for(size_t k = 0; k < p.triidx_count; ++k)
					{
						const vnFace& f = faces[p.array_triidx[k]];

						_jotter.tc(0, uv[0]);
						_jotter.vertex(pos[f.v[0]]);
						_jotter.tc(0, uv[1]);
						_jotter.vertex(pos[f.v[1]]);
						_jotter.tc(0, uv[2]);
						_jotter.vertex(pos[f.v[2]]);

						uv += 3;
					}

					gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, _jotter);
				}
			}
		}
		cgPointLightFX::cgPointLightFX(const AABB& local_bound): cgDecalFXBase(local_bound)
		{
		}
		cgPointLightFX::~cgPointLightFX()
		{
		}
		void							cgPointLightFX::setLookupTexture(const SPtr<Tex2D>& tex)
		{
			_tex_lookup = tex;
		}
		bool							cgPointLightFX::loadShaders()
		{
			if(!_vs)
				_vs = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, 
"\
\n\
float4x4 wvp;\n\
float4x4 world;\n\
float4 light_info;\n\
\n\
void main(\n\
	float4 in_pos : POSITION,\n\
	out float4 out_pos: POSITION,\n\
	out float3 out_tc_0: TEXCOORD0,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	out_tc_0 = (mul(in_pos, world).xyz - light_info.xyz) * light_info.w;\n\
}\n\
\
");

			if(!_ps)
				_ps = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, 
"\
\n\
sampler2D sampler_0: register(s0);\n\
float4 color;\n\
\n\
void main(\n\
	float3 in_tc_0: TEXCOORD0,\n\
	out float4 out_color: COLOR,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_color = color * tex2D(sampler_0, length(in_tc_0));\n\
}\n\
\n\
\n\
\
");
			return (_vs && _ps);
		}
		SPtr<cgPointLightFX::Item>		cgPointLightFX::create(const SPtr<SNode>& map_node, const Vec3& world_pos, REAL radius, const Vec4& color)
		{
			SPtr<Item> item = new Item();
			getIntersectingSet(AABB(world_pos - Vec3(radius, radius, radius), world_pos + Vec3(radius, radius, radius)), map_node, item->_intr_set);
			if(!item->_intr_set.array_mesh_res.size())
				return NULL;
			item->light_pos = world_pos;
			item->light_radius = radius;
			item->color = color;

			_array_item.push_back(item);

			return item;
		}
		void							cgPointLightFX::preRender(SRenderInfo* ri)
		{
			if(!_array_item.size())
				return;

			ri->regTransparentItem(this, 0, -32767);
		}
		void							cgPointLightFX::render(intptr_t parameter, SRenderInfo* ri)
		{
			Gfx* gfx = Gfx::current();

			Frustum frustum(gfx->getUnadjustedViewProjectionTransform(), gfx->getZNearMapping(), gfx->getZFarMapping());

			SRenderParamQuery param(ri);

			rsShade rs_preset(false);

			param.preSetupShadeState(rs_preset);

			rs_preset.setSampler(0, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::FILTERMODE_LINEAR, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP, GfxEnum::WRAPMODE_CLAMP);
			rs_preset.enableBlend(GfxEnum::BLENDFACTOR_SRC_ALPHA, GfxEnum::BLENDFACTOR_ONE, GfxEnum::BLENDOPERATION_ADD);
			rs_preset.enableDepthTest(GfxEnum::CMPFUNC_EQUAL);

			rs_preset.enableAlphaTest(GfxEnum::CMPFUNC_GREATER, 0.0f);

			Z_ASSERT( _vs && _ps );

			gfx->useProgrammablePipeline(_vs, _ps);

			gfx->bindTexture(0, _tex_lookup);

			ShaderUniforms2 unis(_vs, _ps);

			for(size_t i = 0; i < _array_item.size(); ++i)
			{
				const WPtr<Item>& wptr = _array_item[i];
				if(!wptr.valid())
				{
					_array_item.erase(_array_item.begin() + i);
					--i;
					continue;
				}

				const SPtr<Item>& s = wptr.lock();

				if(true)
				{
					const Vec3& o = s->light_pos;
					Vec3 r(s->light_radius, s->light_radius, s->light_radius);
					if(frustum.contains(AABB(o - r, o + r)) == Frustum::FC_OUTSIDE)
						continue;
				}

				rsShade rs = rs_preset;

				if(true)
				{
					Vec3 r(s->light_radius, s->light_radius, s->light_radius);
					AABB screen_bound = projAABBOnScreen(AABB(s->light_pos - r, s->light_pos + r), gfx->getUnadjustedViewProjectionTransform());
					if(screen_bound.isEmpty())
						continue;

					const rsView& view = gfx->getViewState();

					int32_t x = (int32_t)((screen_bound.minimum().x + 1.0f) * 0.5f * view.viewport.dx);
					int32_t y = (int32_t)((-screen_bound.maximum().y + 1.0f) * 0.5f * view.viewport.dy);
					int32_t x2 = (int32_t)((screen_bound.maximum().x + 1.0f) * 0.5f * view.viewport.dx);
					int32_t y2 = (int32_t)((-screen_bound.minimum().y + 1.0f) * 0.5f * view.viewport.dy);

					rs.enableScissorTest(x, y, x2 - x, y2 - y);
				}

				param.postSetupShadeState(rs);

				gfx->setShadeState(rs);

				const cgDecalFXBase::IntrSet& intr_set = s->_intr_set;
				for(size_t i = 0; i < intr_set.array_mesh_res.size(); ++i)
				{
					const cgDecalFXBase::IntrSet::MeshRes& res = intr_set.array_mesh_res[i];

					const SPtr<SMeshNode> mn = res.mn.lock();
					if(!mn)
						continue;

					//unis.clear();
					unis.reset();
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", mn->world_trans() * gfx->getAdjustedViewProjectionTransform());
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "world", mn->world_trans());
					unis.set(GfxEnum::SHADERTYPE_VERTEX, "light_info", Vec4(s->light_pos, 1.0f / s->light_radius));
					unis.set(GfxEnum::SHADERTYPE_PIXEL, "color", s->color);

					gfx->setShaderUniforms(unis);

					_jotter.begin(3, false, false, 0);

					const Vec3* pos = mn->mesh()->get_verts();
					const vnFace* faces = mn->mesh()->get_faces();
					for(size_t k = 0; k < res.triidx_count; ++k)
					{
						const vnFace& f = faces[res.array_triidx[k]];

						_jotter.vertex(pos[f.v[0]]);
						_jotter.vertex(pos[f.v[1]]);
						_jotter.vertex(pos[f.v[2]]);
					}

					gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, _jotter);
				}
			}
		}
	};
};


