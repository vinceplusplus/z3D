
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace SG
	{
		class DrawHelper
		{
		public:
			static void			drawLines(const Jotter& jotter, const Mat4& world_xform = Mat4::identity, rsShade* rs = NULL);
			static void			drawTriangles(const Jotter& jotter, const Mat4& world_xform = Mat4::identity, rsShade* rs = NULL);
		public:
			static void			fillBoxOutline(const AABB& box, Jotter& jotter);
			static void			fillBoxOutline(const OBB& box, Jotter& jotter);
			static void			fillSphereOutline(REAL radius, size_t segment_count, Jotter& jotter);
			static void			fillCircleOutline(REAL radius, size_t segment_count, Jotter& jotter);
		public:
			static void			fillCone(REAL angle, REAL h, size_t segment_count, Jotter& jotter);
			static void			fillCircle(REAL radius, size_t segment_count, Jotter& jotter);
			static void			fillSphere(REAL radius, size_t segment_count, Jotter& jotter);
		public:
			static void			drawAABBOutline(const AABB& box, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
			static void			drawOBBOutline(const OBB& box, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
			static void			drawAABBsOutline(const SPtr<SStructNode>& node, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
			static void			drawOBBsOutline(const SPtr<SStructNode>& node, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
		public:
			static void			drawSphereOutline(const Vec3& pos, REAL radius, size_t segment_count, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
			static void			drawCircleOutline(const Vec3& c, REAL r, const Vec3& a, size_t segment_count, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
		public:
			static void			drawCameraOutline(const SPtr<SCameraNode>& cam, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
		public:
			static void			drawGrid(REAL grid_spacing, size_t grid_extent, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
			static void			drawOrthoAxisTripod(const Mat3& cam_rot, const Vec2& pos, bool invert, const SPtr<TexFont>& font, REAL line_length = 16.0f, REAL text_offset = 6.0f);
		public:
			static void			drawCone(const Vec3& v, const Vec3& a, REAL angle, REAL h, size_t segment_count, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);					// v - the tip of the cone, a - the direction from v the cone to grow
			static void			drawCircle(const Vec3& c, REAL r, const Vec3& a, size_t segment_count, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
			static void			drawSphere(const Vec3& pos, REAL radius, size_t segment_count, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* rs = NULL);
		public:
			static void			drawCursor(const Vec2& p, const Vec4& color = Vec4(1, 1, 1, 1));
		};
	};
};


