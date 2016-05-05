
#include "DrawHelper.h"
#include "SStructNode.h"
#include "SCameraNode.h"

namespace z3D
{
	namespace SG
	{
		void				DrawHelper::drawLines(const Jotter& jotter, const Mat4& world_xform, rsShade* rs)
		{
			Gfx* gfx = Gfx::current();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getPassThruShaders(vs, ps);
				gfx->useProgrammablePipeline(vs, ps);
			}

			if(!rs)
			{
				rsShade rs(false);
				gfx->setShadeState(rs);
			}
			else
				gfx->setShadeState(*rs);

			ShaderUniforms2 uni(vs, ps);
			uni.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", world_xform * gfx->getAdjustedViewProjectionTransform());

			gfx->setShaderUniforms(uni);

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_LINES, jotter);
		}
		void				DrawHelper::drawTriangles(const Jotter& jotter, const Mat4& world_xform, rsShade* rs)
		{
			Gfx* gfx = Gfx::current();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getPassThruShaders(vs, ps);
				gfx->useProgrammablePipeline(vs, ps);
			}

			if(!rs)
			{
				rsShade rs(false);
				rs.enableDepthTest();
				rs.enableCull();
				gfx->setShadeState(rs);
			}
			else
				gfx->setShadeState(*rs);

			ShaderUniforms2 uni(vs, ps);
			uni.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", world_xform * gfx->getAdjustedViewProjectionTransform());

			gfx->setShaderUniforms(uni);

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, jotter);
		}
		void				DrawHelper::fillBoxOutline(const AABB& box, Jotter& jotter)
		{
			jotter.vertex(box.getCorner(0)); jotter.vertex(box.getCorner(1));
			jotter.vertex(box.getCorner(1)); jotter.vertex(box.getCorner(3));
			jotter.vertex(box.getCorner(3)); jotter.vertex(box.getCorner(2));
			jotter.vertex(box.getCorner(2)); jotter.vertex(box.getCorner(0));
			jotter.vertex(box.getCorner(4)); jotter.vertex(box.getCorner(5));
			jotter.vertex(box.getCorner(5)); jotter.vertex(box.getCorner(7));
			jotter.vertex(box.getCorner(7)); jotter.vertex(box.getCorner(6));
			jotter.vertex(box.getCorner(6)); jotter.vertex(box.getCorner(4));
			jotter.vertex(box.getCorner(0)); jotter.vertex(box.getCorner(4));
			jotter.vertex(box.getCorner(5)); jotter.vertex(box.getCorner(1));
			jotter.vertex(box.getCorner(2)); jotter.vertex(box.getCorner(6));
			jotter.vertex(box.getCorner(7)); jotter.vertex(box.getCorner(3));
		}
		void				DrawHelper::fillBoxOutline(const OBB& box, Jotter& jotter)
		{
			jotter.vertex(box.getCorner(0)); jotter.vertex(box.getCorner(1));
			jotter.vertex(box.getCorner(1)); jotter.vertex(box.getCorner(3));
			jotter.vertex(box.getCorner(3)); jotter.vertex(box.getCorner(2));
			jotter.vertex(box.getCorner(2)); jotter.vertex(box.getCorner(0));
			jotter.vertex(box.getCorner(4)); jotter.vertex(box.getCorner(5));
			jotter.vertex(box.getCorner(5)); jotter.vertex(box.getCorner(7));
			jotter.vertex(box.getCorner(7)); jotter.vertex(box.getCorner(6));
			jotter.vertex(box.getCorner(6)); jotter.vertex(box.getCorner(4));
			jotter.vertex(box.getCorner(0)); jotter.vertex(box.getCorner(4));
			jotter.vertex(box.getCorner(5)); jotter.vertex(box.getCorner(1));
			jotter.vertex(box.getCorner(2)); jotter.vertex(box.getCorner(6));
			jotter.vertex(box.getCorner(7)); jotter.vertex(box.getCorner(3));
		}
		void				DrawHelper::fillSphereOutline(REAL radius, size_t segment_count, Jotter& jotter)
		{
			segment_count = max((size_t)4, segment_count);
			segment_count -= (segment_count & 3);

			size_t vsegment_count = segment_count / 2;

			for(size_t i = 1; i < vsegment_count; ++i)
			{
				REAL r = radius * SIN((REAL)i / vsegment_count * 180.0f);
				REAL y = radius * COS((REAL)i / vsegment_count * 180.0f);

				for(size_t j = 0; j < segment_count; ++j)
				{
					jotter.vertex(Vec3(r * COS(360.0f / segment_count * j), y, r * SIN(360.0f / segment_count * j)));
					jotter.vertex(Vec3(r * COS(360.0f / segment_count * (j + 1)), y, r * SIN(360.0f / segment_count * (j + 1))));
				}
			}

			for(size_t i = 0; i < segment_count; ++i)
				for(size_t j = 0; j < vsegment_count; ++j)
				{
					if(j == 0)
					{
						jotter.vertex(Vec3(0, radius, 0));
						jotter.vertex(Vec3(radius * SIN((REAL)(j + 1) / vsegment_count * 180.0f) * COS(360.0f / segment_count * i), radius * COS((REAL)(j + 1) / vsegment_count * 180.0f), radius * SIN((REAL)(j + 1) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * i)));
					}
					else if(j == vsegment_count - 1)
					{
						jotter.vertex(Vec3(radius * SIN((REAL)(j + 0) / vsegment_count * 180.0f) * COS(360.0f / segment_count * i), radius * COS((REAL)(j + 0) / vsegment_count * 180.0f), radius * SIN((REAL)(j + 0) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * i)));
						jotter.vertex(Vec3(0, -radius, 0));
					}
					else
					{
						jotter.vertex(Vec3(radius * SIN((REAL)(j + 0) / vsegment_count * 180.0f) * COS(360.0f / segment_count * i), radius * COS((REAL)(j + 0) / vsegment_count * 180.0f), radius * SIN((REAL)(j + 0) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * i)));
						jotter.vertex(Vec3(radius * SIN((REAL)(j + 1) / vsegment_count * 180.0f) * COS(360.0f / segment_count * i), radius * COS((REAL)(j + 1) / vsegment_count * 180.0f), radius * SIN((REAL)(j + 1) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * i)));
					}
				}
		}
		void				DrawHelper::fillCircleOutline(REAL radius, size_t segment_count, Jotter& jotter)
		{
			segment_count = max((size_t)4, segment_count);
			segment_count -= (segment_count & 3);

			for(size_t i = 0; i < segment_count; ++i)
			{
				jotter.vertex(Vec3(COS(i * 360.0f / segment_count) * radius, SIN(i * 360.0f / segment_count) * radius, 0));
				jotter.vertex(Vec3(COS(((i + 1) % segment_count) * 360.0f / segment_count) * radius, SIN(((i + 1) % segment_count) * 360.0f / segment_count) * radius, 0));
			}
		}
		void				DrawHelper::fillCone(REAL angle, REAL h, size_t segment_count, Jotter& jotter)
		{
			segment_count = max((size_t)4, segment_count);
			segment_count -= (segment_count & 3);

			REAL r = h * TAN(angle);

			for(size_t i = 0; i < segment_count; ++i)
			{
				jotter.vertex(Vec3::zero);
				jotter.vertex(Vec3(COS(i * 360.0f / segment_count) * r, SIN(i * 360.0f / segment_count) * r, -h));
				jotter.vertex(Vec3(COS(((i + 1) % segment_count) * 360.0f / segment_count) * r, SIN(((i + 1) % segment_count) * 360.0f / segment_count) * r, -h));
			}
		}
		void				DrawHelper::fillCircle(REAL radius, size_t segment_count, Jotter& jotter)
		{
			segment_count = max((size_t)4, segment_count);
			segment_count -= (segment_count & 3);

			for(size_t i = 0; i < segment_count; ++i)
			{
				jotter.vertex(Vec3::zero);
				jotter.vertex(Vec3(COS(((i + 1) % segment_count) * 360.0f / segment_count) * radius, SIN(((i + 1) % segment_count) * 360.0f / segment_count) * radius, 0));
				jotter.vertex(Vec3(COS(i * 360.0f / segment_count) * radius, SIN(i * 360.0f / segment_count) * radius, 0));
			}
		}
		void				DrawHelper::fillSphere(REAL radius, size_t segment_count, Jotter& jotter)
		{
			segment_count = max((size_t)4, segment_count);
			segment_count -= (segment_count & 3);

			size_t vsegment_count = segment_count / 2;

			for(size_t i = 0; i < vsegment_count; ++i)
			{
				for(size_t j = 0; j < segment_count; ++j)
				{
					if(i == 0)
					{
						jotter.vertex(0, radius, 0);
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 1) % segment_count)), radius * COS((REAL)(i + 1) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 1) % segment_count))));
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 0) % segment_count)), radius * COS((REAL)(i + 1) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 0) % segment_count))));
					}
					else if(i == vsegment_count - 1)
					{
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 0) % segment_count)), radius * COS((REAL)(i + 0) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 0) % segment_count))));
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 1) % segment_count)), radius * COS((REAL)(i + 0) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 1) % segment_count))));
						jotter.vertex(0, -radius, 0);
					}
					else
					{
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 0) % segment_count)), radius * COS((REAL)(i + 1) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 0) % segment_count))));
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 0) % segment_count)), radius * COS((REAL)(i + 0) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 0) % segment_count))));
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 1) % segment_count)), radius * COS((REAL)(i + 0) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 1) % segment_count))));

						jotter.vertex(Vec3(radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 0) % segment_count)), radius * COS((REAL)(i + 1) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 0) % segment_count))));
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 1) % segment_count)), radius * COS((REAL)(i + 0) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 0) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 1) % segment_count))));
						jotter.vertex(Vec3(radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * COS(360.0f / segment_count * ((j + 1) % segment_count)), radius * COS((REAL)(i + 1) / vsegment_count * 180.0f), radius * SIN((REAL)(i + 1) / vsegment_count * 180.0f) * SIN(360.0f / segment_count * ((j + 1) % segment_count))));
					}
				}
			}
		}
		void				DrawHelper::drawAABBOutline(const AABB& box, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			fillBoxOutline(box, jotter);

			drawLines(jotter, Mat4::identity, rs);
		}
		void				DrawHelper::drawOBBOutline(const OBB& box, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			fillBoxOutline(box, jotter);

			drawLines(jotter, Mat4::identity, rs);
		}
		void				DrawHelper::drawAABBsOutline(const SPtr<SStructNode>& node, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			const SStructNode::MAP_NAME_LOADED_NODE& node_map = node->getLoadedNodes();
			for(SStructNode::MAP_NAME_LOADED_NODE::const_iterator iter = node_map.begin(); iter != node_map.end(); ++iter)
				fillBoxOutline(iter->second->world_bound(), jotter);

			drawLines(jotter, Mat4::identity, rs);
		}
		void				DrawHelper::drawOBBsOutline(const SPtr<SStructNode>& node, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			const SStructNode::MAP_NAME_LOADED_NODE& node_map = node->getLoadedNodes();
			for(SStructNode::MAP_NAME_LOADED_NODE::const_iterator iter = node_map.begin(); iter != node_map.end(); ++iter)
				fillBoxOutline(OBB(iter->second->local_bound()).transformedBy(iter->second->world_trans()), jotter);

			drawLines(jotter, Mat4::identity, rs);
		}
		void			DrawHelper::drawSphereOutline(const Vec3& pos, REAL radius, size_t segment_count, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			fillSphereOutline(radius, segment_count, jotter);

			drawLines(jotter, Mat4::translation(pos), rs);
		}
		void				DrawHelper::drawCircleOutline(const Vec3& c, REAL r, const Vec3& a, size_t segment_count, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			fillCircleOutline(r, segment_count, jotter);

			Mat4 world;
			if(true)
			{
				REAL yaw, pitch;
				a.resolveOriRH(yaw, pitch);
				world = Mat4::rotationX(pitch) * Mat4::rotationY(yaw) * Mat4::translation(c);
			}

			drawLines(jotter, world, rs);
		}
		void				DrawHelper::drawCameraOutline(const SPtr<SCameraNode>& cam, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			if(true)
			{
				Mat4 inv_viewproj = cam->view_proj().inverse();

				jotter.vertex(Vec3(-1, 1, 0) * inv_viewproj);
				jotter.vertex(Vec3(-1, -1, 0) * inv_viewproj);

				jotter.vertex(Vec3(-1, -1, 0) * inv_viewproj);
				jotter.vertex(Vec3(1, -1, 0) * inv_viewproj);

				jotter.vertex(Vec3(1, -1, 0) * inv_viewproj);
				jotter.vertex(Vec3(1, 1, 0) * inv_viewproj);

				jotter.vertex(Vec3(1, 1, 0) * inv_viewproj);
				jotter.vertex(Vec3(-1, 1, 0) * inv_viewproj);


				jotter.vertex(Vec3(-1, 1, 1) * inv_viewproj);
				jotter.vertex(Vec3(-1, -1, 1) * inv_viewproj);

				jotter.vertex(Vec3(-1, -1, 1) * inv_viewproj);
				jotter.vertex(Vec3(1, -1, 1) * inv_viewproj);

				jotter.vertex(Vec3(1, -1, 1) * inv_viewproj);
				jotter.vertex(Vec3(1, 1, 1) * inv_viewproj);

				jotter.vertex(Vec3(1, 1, 1) * inv_viewproj);
				jotter.vertex(Vec3(-1, 1, 1) * inv_viewproj);

				jotter.vertex(Vec3(-1, 1, 0) * inv_viewproj);
				jotter.vertex(Vec3(-1, 1, 1) * inv_viewproj);

				jotter.vertex(Vec3(-1, -1, 0) * inv_viewproj);
				jotter.vertex(Vec3(-1, -1, 1) * inv_viewproj);

				jotter.vertex(Vec3(1, -1, 0) * inv_viewproj);
				jotter.vertex(Vec3(1, -1, 1) * inv_viewproj);

				jotter.vertex(Vec3(1, 1, 0) * inv_viewproj);
				jotter.vertex(Vec3(1, 1, 1) * inv_viewproj);
			}

			drawLines(jotter, Mat4::identity, rs);
		}
		void				DrawHelper::drawGrid(REAL grid_spacing, size_t grid_extent, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			jotter.vertex(-grid_spacing * grid_extent, 0, 0);
			jotter.vertex(grid_spacing * grid_extent, 0, 0);
			jotter.vertex(0, 0, -grid_spacing * grid_extent);
			jotter.vertex(0, 0, grid_spacing * grid_extent);

			jotter.color(0.8f, 0.8f, 0.8f, 1);
			for(size_t i = 1; i <= grid_extent; ++i)
			{
				jotter.vertex(-grid_spacing * grid_extent, 0, -grid_spacing * i);
				jotter.vertex(grid_spacing * grid_extent, 0, -grid_spacing * i);

				jotter.vertex(-grid_spacing * grid_extent, 0, grid_spacing * i);
				jotter.vertex(grid_spacing * grid_extent, 0, grid_spacing * i);

				jotter.vertex(-grid_spacing * i, 0, -grid_spacing * grid_extent);
				jotter.vertex(-grid_spacing * i, 0, grid_spacing * grid_extent);

				jotter.vertex(grid_spacing * i, 0, -grid_spacing * grid_extent);
				jotter.vertex(grid_spacing * i, 0, grid_spacing * grid_extent);
			}

			drawLines(jotter, Mat4::identity, rs);
		}
		void				DrawHelper::drawOrthoAxisTripod(const Mat3& cam_rot, const Vec2& pos, bool invert, const SPtr<TexFont>& font, REAL line_length, REAL text_offset)
		{
			REAL INVERT = 1.0f;
			if(invert)
				INVERT = -1.0f;

			Mat3 rot = cam_rot.transpose();
			Vec3 axes[3] = {rot[0], rot[1], rot[2]};

			REAL ext = line_length;

			Jotter jotter;

			size_t order[3] = {0, 1, 2};

			if(axes[order[0]].z > axes[order[1]].z)
				swap(order[0], order[1]);
			if(axes[order[1]].z > axes[order[2]].z)
				swap(order[1], order[2]);
			if(axes[order[0]].z > axes[order[1]].z)
				swap(order[0], order[1]);

			for(size_t i = 0; i < 3; ++i)
			{
				size_t a = order[i];

				Vec4 color(a == 0 ? 1.0f : 0.0f, a == 1 ? 1.0f : 0.0f, a == 2 ? 1.0f : 0.0f, 1);
				Vec2 end_point = pos + Vec2(axes[a].x, INVERT * axes[a].y) * ext;
				Vec2 text_pos = pos + Vec2(axes[a].x, INVERT * axes[a].y) * (ext + text_offset);

				jotter.begin(2, false, true, 0);

				jotter.color(color);
				jotter.vertex(pos);
				jotter.vertex(end_point);

				rsShade rs(false);

				DrawHelper::drawLines(jotter, Mat4::identity, &rs);

				if(font)
					font->begin().center().middle().ext(0, 0).color(color).end(wcs(L"") + wcs(L"xyz")[a], text_pos);
			}
		}
		void			DrawHelper::drawCone(const Vec3& v, const Vec3& a, REAL angle, REAL h, size_t segment_count, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			fillCone(angle, h, segment_count, jotter);

			Mat4 world;
			if(true)
			{
				REAL yaw, pitch;
				a.resolveOriRH(yaw, pitch);
				world = Mat4::rotationX(pitch) * Mat4::rotationY(yaw) * Mat4::translation(v);
			}

			drawTriangles(jotter, world, rs);
		}
		void			DrawHelper::drawCircle(const Vec3& c, REAL r, const Vec3& a, size_t segment_count, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			fillCircle(r, segment_count, jotter);

			Mat4 world;
			if(true)
			{
				REAL yaw, pitch;
				a.resolveOriRH(yaw, pitch);
				world = Mat4::rotationX(pitch) * Mat4::rotationY(yaw) * Mat4::translation(c);
			}

			drawTriangles(jotter, world, rs);
		}
		void			DrawHelper::drawSphere(const Vec3& pos, REAL radius, size_t segment_count, const Vec4& color, rsShade* rs)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			fillSphere(radius, segment_count, jotter);

			drawTriangles(jotter, Mat4::translation(pos), rs);
		}
		void			DrawHelper::drawCursor(const Vec2& p, const Vec4& color)
		{
			Jotter jotter;
			jotter.begin(3, false, true, 0);
			jotter.color(color);

			REAL scale = 0.5f;

			jotter.vertex(p + Vec2(-0.03125f, 0.0078125f) * scale);
			jotter.vertex(p + Vec2(-0.03125f, -0.0078125f) * scale);
			jotter.vertex(p + Vec2(0.03125f, 0.0078125f) * scale);

			jotter.vertex(p + Vec2(0.03125f, 0.0078125f) * scale);
			jotter.vertex(p + Vec2(-0.03125f, -0.0078125f) * scale);
			jotter.vertex(p + Vec2(0.03125f, -0.0078125f) * scale);

			jotter.vertex(p + Vec2(-0.0078125f, 0.03125f) * scale);
			jotter.vertex(p + Vec2(-0.0078125f, -0.03125f) * scale);
			jotter.vertex(p + Vec2(0.0078125f, 0.03125f) * scale);

			jotter.vertex(p + Vec2(0.0078125f, 0.03125f) * scale);
			jotter.vertex(p + Vec2(-0.0078125f, -0.03125f) * scale);
			jotter.vertex(p + Vec2(0.0078125f, -0.03125f) * scale);

			Gfx* gfx = Gfx::current();

			SPtr<Shader> vs;
			SPtr<Shader> ps;
			if(true)
			{
				gfx->getPassThruShaders(vs, ps);
				gfx->useProgrammablePipeline(vs, ps);
			}

			if(true)
			{
				rsShade rs(false);
				gfx->setShadeState(rs);
			}

			ShaderUniforms2 uni(vs, ps);
			uni.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", gfx->adjustProjectionTransform(Mat4::identity));

			gfx->setShaderUniforms(uni);

			gfx->drawPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, jotter);
		}
	};
};


